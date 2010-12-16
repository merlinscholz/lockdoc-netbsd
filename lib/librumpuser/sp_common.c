/*      $NetBSD: sp_common.c,v 1.16 2010/12/16 12:38:20 pooka Exp $	*/

/*
 * Copyright (c) 2010 Antti Kantee.  All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Common client/server sysproxy routines.  #included.
 */

#include <sys/cdefs.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/queue.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/syslimits.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <poll.h>
#include <pthread.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//#define DEBUG
#ifdef DEBUG
#define DPRINTF(x) mydprintf x
static void
mydprintf(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}
#else
#define DPRINTF(x)
#endif

/*
 * Bah, I hate writing on-off-wire conversions in C
 */

enum { RUMPSP_REQ, RUMPSP_RESP, RUMPSP_ERROR };
enum {	RUMPSP_SYSCALL,
	RUMPSP_COPYIN, RUMPSP_COPYINSTR,
	RUMPSP_COPYOUT, RUMPSP_COPYOUTSTR,
	RUMPSP_ANONMMAP };

struct rsp_hdr {
	uint64_t rsp_len;
	uint64_t rsp_reqno;
	uint16_t rsp_class;
	uint16_t rsp_type;
	/*
	 * We want this structure 64bit-aligned for typecast fun,
	 * so might as well use the following for something.
	 */
	union {
		uint32_t sysnum;
		uint32_t error;
	} u;
};
#define HDRSZ sizeof(struct rsp_hdr)
#define rsp_sysnum u.sysnum
#define rsp_error u.error

#define MAXBANNER 96

/*
 * Data follows the header.  We have two types of structured data.
 */

/* copyin/copyout */
struct rsp_copydata {
	size_t rcp_len;
	void *rcp_addr;
	uint8_t rcp_data[0];
};

/* syscall response */
struct rsp_sysresp {
	int rsys_error;
	register_t rsys_retval[2];
};

struct respwait {
	uint64_t rw_reqno;
	void *rw_data;
	size_t rw_dlen;
	int rw_error;

	pthread_cond_t rw_cv;

	TAILQ_ENTRY(respwait) rw_entries;
};

struct spclient {
	int spc_fd;
	int spc_refcnt;
	int spc_dying;

	pthread_mutex_t spc_mtx;
	pthread_cond_t spc_cv;

	struct lwp *spc_mainlwp;
	pid_t spc_pid;

	TAILQ_HEAD(, respwait) spc_respwait;

	/* rest of the fields are zeroed upon disconnect */
#define SPC_ZEROFF offsetof(struct spclient, spc_pfd)
	struct pollfd *spc_pfd;

	struct rsp_hdr spc_hdr;
	uint8_t *spc_buf;
	size_t spc_off;

	uint64_t spc_nextreq;
	int spc_ostatus, spc_istatus;
};
#define SPCSTATUS_FREE 0
#define SPCSTATUS_BUSY 1
#define SPCSTATUS_WANTED 2

typedef int (*addrparse_fn)(const char *, struct sockaddr **, int);
typedef int (*connecthook_fn)(int);
typedef void (*cleanup_fn)(struct sockaddr *);

static int readframe(struct spclient *);
static void handlereq(struct spclient *);

static __inline void
spcresetbuf(struct spclient *spc)
{

	spc->spc_buf = NULL;
	spc->spc_off = 0;
}

static __inline void
spcfreebuf(struct spclient *spc)
{

	free(spc->spc_buf);
	spcresetbuf(spc);
}

static void
sendlockl(struct spclient *spc)
{

	/* assert(pthread_mutex_owned) */
	while (spc->spc_ostatus != SPCSTATUS_FREE) {
		spc->spc_ostatus = SPCSTATUS_WANTED;
		pthread_cond_wait(&spc->spc_cv, &spc->spc_mtx);
	}
	spc->spc_ostatus = SPCSTATUS_BUSY;
}

static void
sendlock(struct spclient *spc)
{

	pthread_mutex_lock(&spc->spc_mtx);
	sendlockl(spc);
	pthread_mutex_unlock(&spc->spc_mtx);
}

static void
sendunlockl(struct spclient *spc)
{

	/* assert(pthread_mutex_owned) */
	if (spc->spc_ostatus == SPCSTATUS_WANTED)
		pthread_cond_broadcast(&spc->spc_cv);
	spc->spc_ostatus = SPCSTATUS_FREE;
}

static void
sendunlock(struct spclient *spc)
{

	pthread_mutex_lock(&spc->spc_mtx);
	sendunlockl(spc);
	pthread_mutex_unlock(&spc->spc_mtx);
}

static int
dosend(struct spclient *spc, const void *data, size_t dlen)
{
	struct pollfd pfd;
	const uint8_t *sdata = data;
	ssize_t n;
	size_t sent;
	int fd = spc->spc_fd;

	pfd.fd = fd;
	pfd.events = POLLOUT;

	for (sent = 0, n = 0; sent < dlen; ) {
		if (n) {
			if (poll(&pfd, 1, INFTIM) == -1) {
				if (errno == EINTR)
					continue;
				return errno;
			}
		}

		n = send(fd, sdata + sent, dlen - sent, MSG_NOSIGNAL);
		if (n == 0) {
			return EFAULT;
		}
		if (n == -1)  {
			if (errno != EAGAIN)
				return EFAULT;
			continue;
		}
		sent += n;
	}

	return 0;
}

static void
putwait(struct spclient *spc, struct respwait *rw, struct rsp_hdr *rhdr)
{

	rw->rw_data = NULL;
	rw->rw_dlen = 0;
	pthread_cond_init(&rw->rw_cv, NULL);

	pthread_mutex_lock(&spc->spc_mtx);
	rw->rw_reqno = rhdr->rsp_reqno = spc->spc_nextreq++;
	TAILQ_INSERT_TAIL(&spc->spc_respwait, rw, rw_entries);

	sendlockl(spc);
}

static void
unputwait(struct spclient *spc, struct respwait *rw)
{

	sendunlockl(spc);

	TAILQ_REMOVE(&spc->spc_respwait, rw, rw_entries);
	pthread_mutex_unlock(&spc->spc_mtx);
	pthread_cond_destroy(&rw->rw_cv);
}

static void
kickwaiter(struct spclient *spc)
{
	struct respwait *rw;
	int error;

	pthread_mutex_lock(&spc->spc_mtx);
	TAILQ_FOREACH(rw, &spc->spc_respwait, rw_entries) {
		if (rw->rw_reqno == spc->spc_hdr.rsp_reqno)
			break;
	}
	if (rw == NULL) {
		DPRINTF(("no waiter found, invalid reqno %" PRIu64 "?\n",
		    spc->spc_hdr.rsp_reqno));
		return;
	}
	DPRINTF(("rump_sp: client %p woke up waiter at %p\n", spc, rw));
	rw->rw_data = spc->spc_buf;
	rw->rw_dlen = (size_t)(spc->spc_off - HDRSZ);
	if (spc->spc_hdr.rsp_class == RUMPSP_ERROR) {
		error = rw->rw_error = spc->spc_hdr.rsp_error;
	} else {
		error = rw->rw_error = 0;
	}
	pthread_cond_signal(&rw->rw_cv);
	pthread_mutex_unlock(&spc->spc_mtx);

	if (error)
		spcfreebuf(spc);
	else
		spcresetbuf(spc);
}

static void
kickall(struct spclient *spc)
{
	struct respwait *rw;

	/* DIAGASSERT(mutex_owned(spc_lock)) */
	TAILQ_FOREACH(rw, &spc->spc_respwait, rw_entries)
		pthread_cond_broadcast(&rw->rw_cv);
}

static int
waitresp(struct spclient *spc, struct respwait *rw)
{
	struct pollfd pfd;
	int rv = 0;

	sendunlockl(spc);

	rw->rw_error = 0;
	while (rw->rw_data == NULL && rw->rw_error == 0 && spc->spc_dying == 0){
		/* are we free to receive? */
		if (spc->spc_istatus == SPCSTATUS_FREE) {
			int gotresp;

			spc->spc_istatus = SPCSTATUS_BUSY;
			pthread_mutex_unlock(&spc->spc_mtx);

			pfd.fd = spc->spc_fd;
			pfd.events = POLLIN;

			for (gotresp = 0; !gotresp; ) {
				switch (readframe(spc)) {
				case 0:
					poll(&pfd, 1, INFTIM);
					continue;
				case -1:
					rv = errno;
					spc->spc_dying = 1;
					goto cleanup;
				default:
					break;
				}

				switch (spc->spc_hdr.rsp_class) {
				case RUMPSP_RESP:
				case RUMPSP_ERROR:
					kickwaiter(spc);
					gotresp = spc->spc_hdr.rsp_reqno ==
					    rw->rw_reqno;
					break;
				case RUMPSP_REQ:
					handlereq(spc);
					break;
				default:
					/* panic */
					break;
				}
			}
 cleanup:
			pthread_mutex_lock(&spc->spc_mtx);
			if (spc->spc_istatus == SPCSTATUS_WANTED)
				kickall(spc);
			spc->spc_istatus = SPCSTATUS_FREE;
		} else {
			spc->spc_istatus = SPCSTATUS_WANTED;
			pthread_cond_wait(&rw->rw_cv, &spc->spc_mtx);
		}
	}

	TAILQ_REMOVE(&spc->spc_respwait, rw, rw_entries);
	pthread_mutex_unlock(&spc->spc_mtx);

	pthread_cond_destroy(&rw->rw_cv);

	if (rv)
		return rv;
	if (spc->spc_dying)
		return ENOTCONN;
	return rw->rw_error;
}

static int
readframe(struct spclient *spc)
{
	int fd = spc->spc_fd;
	size_t left;
	size_t framelen;
	ssize_t n;

	/* still reading header? */
	if (spc->spc_off < HDRSZ) {
		DPRINTF(("rump_sp: readframe getting header at offset %zu\n",
		    spc->spc_off));

		left = HDRSZ - spc->spc_off;
		/*LINTED: cast ok */
		n = read(fd, (uint8_t *)&spc->spc_hdr + spc->spc_off, left);
		if (n == 0) {
			return -1;
		}
		if (n == -1) {
			if (errno == EAGAIN)
				return 0;
			return -1;
		}

		spc->spc_off += n;
		if (spc->spc_off < HDRSZ)
			return -1;

		/*LINTED*/
		framelen = spc->spc_hdr.rsp_len;

		if (framelen < HDRSZ) {
			return -1;
		} else if (framelen == HDRSZ) {
			return 1;
		}

		spc->spc_buf = malloc(framelen - HDRSZ);
		if (spc->spc_buf == NULL) {
			return -1;
		}
		memset(spc->spc_buf, 0, framelen - HDRSZ);

		/* "fallthrough" */
	} else {
		/*LINTED*/
		framelen = spc->spc_hdr.rsp_len;
	}

	left = framelen - spc->spc_off;

	DPRINTF(("rump_sp: readframe getting body at offset %zu, left %zu\n",
	    spc->spc_off, left));

	if (left == 0)
		return 1;
	n = read(fd, spc->spc_buf + (spc->spc_off - HDRSZ), left);
	if (n == 0) {
		return -1;
	}
	if (n == -1) {
		if (errno == EAGAIN)
			return 0;
		return -1;
	}
	spc->spc_off += n;
	left -= n;

	/* got everything? */
	if (left == 0)
		return 1;
	else
		return 0;
}

static int
tcp_parse(const char *addr, struct sockaddr **sa, int allow_wildcard)
{
	struct sockaddr_in sin;
	char buf[64];
	const char *p;
	size_t l;
	int port;

	memset(&sin, 0, sizeof(sin));
	sin.sin_len = sizeof(sin);
	sin.sin_family = AF_INET;

	p = strchr(addr, ':');
	if (!p) {
		fprintf(stderr, "rump_sp_tcp: missing port specifier\n");
		return EINVAL;
	}

	l = p - addr;
	if (l > sizeof(buf)-1) {
		fprintf(stderr, "rump_sp_tcp: address too long\n");
		return EINVAL;
	}
	strncpy(buf, addr, l);
	buf[l] = '\0';

	/* special INADDR_ANY treatment */
	if (strcmp(buf, "*") == 0 || strcmp(buf, "0") == 0) {
		sin.sin_addr.s_addr = INADDR_ANY;
	} else {
		switch (inet_pton(AF_INET, buf, &sin.sin_addr)) {
		case 1:
			break;
		case 0:
			fprintf(stderr, "rump_sp_tcp: cannot parse %s\n", buf);
			return EINVAL;
		case -1:
			fprintf(stderr, "rump_sp_tcp: inet_pton failed\n");
			return errno;
		default:
			assert(/*CONSTCOND*/0);
			return EINVAL;
		}
	}

	if (!allow_wildcard && sin.sin_addr.s_addr == INADDR_ANY) {
		fprintf(stderr, "rump_sp_tcp: client needs !INADDR_ANY\n");
		return EINVAL;
	}

	/* advance to port number & parse */
	p++;
	l = strspn(p, "0123456789");
	if (l == 0) {
		fprintf(stderr, "rump_sp_tcp: port now found: %s\n", p);
		return EINVAL;
	}
	strncpy(buf, p, l);
	buf[l] = '\0';

	if (*(p+l) != '/' && *(p+l) != '\0') {
		fprintf(stderr, "rump_sp_tcp: junk at end of port: %s\n", addr);
		return EINVAL;
	}

	port = atoi(buf);
	if (port < 0 || port >= (1<<(8*sizeof(in_port_t)))) {
		fprintf(stderr, "rump_sp_tcp: port %d out of range\n", port);
		return ERANGE;
	}
	sin.sin_port = htons(port);

	*sa = malloc(sizeof(sin));
	if (*sa == NULL)
		return errno;
	memcpy(*sa, &sin, sizeof(sin));
	return 0;
}

static int
tcp_connecthook(int s)
{
	int x;

	x = 1;
	setsockopt(s, IPPROTO_TCP, TCP_NODELAY, &x, sizeof(x));

	return 0;
}

/*ARGSUSED*/
static int
unix_parse(const char *addr, struct sockaddr **sa, int allow_wildcard)
{
	struct sockaddr_un sun;
	size_t slen;

	if (strlen(addr) > sizeof(sun.sun_path))
		return ENAMETOOLONG;

	/*
	 * The pathname can be all kinds of spaghetti elementals,
	 * so meek and obidient we accept everything.  However, use
	 * full path for easy cleanup in case someone gives a relative
	 * one and the server does a chdir() between now than the
	 * cleanup.
	 */
	memset(&sun, 0, sizeof(sun));
	sun.sun_family = AF_LOCAL;
	if (*addr != '/') {
		char mywd[PATH_MAX];

		if (getcwd(mywd, sizeof(mywd)) == NULL) {
			fprintf(stderr, "warning: cannot determine cwd, "
			    "omitting socket cleanup\n");
		} else {
			if (strlen(addr) + strlen(mywd) > sizeof(sun.sun_path))
				return ENAMETOOLONG;
			strlcpy(sun.sun_path, mywd, sizeof(sun.sun_path));
			strlcat(sun.sun_path, "/", sizeof(sun.sun_path));
		}
	}
	strlcat(sun.sun_path, addr, sizeof(sun.sun_path));
	sun.sun_len = SUN_LEN(&sun);
	slen = sun.sun_len+1; /* get the 0 too */

	*sa = malloc(slen);
	if (*sa == NULL)
		return errno;
	memcpy(*sa, &sun, slen);

	return 0;
}

static void
unix_cleanup(struct sockaddr *sa)
{
	struct sockaddr_un *sun = (void *)sa;

	/*
	 * cleanup only absolute paths.  see unix_parse() above
	 */
	if (*sun->sun_path == '/') {
		unlink(sun->sun_path);
	}
}

/*ARGSUSED*/
static int
notsupp(void)
{

	fprintf(stderr, "rump_sp: support not yet implemented\n");
	return EOPNOTSUPP;
}

static int
success(void)
{

	return 0;
}

struct {
	const char *id;
	int domain;
	addrparse_fn ap;
	connecthook_fn connhook;
	cleanup_fn cleanup;
} parsetab[] = {
	{ "tcp", PF_INET, tcp_parse, tcp_connecthook, (cleanup_fn)success },
	{ "unix", PF_LOCAL, unix_parse, (connecthook_fn)success, unix_cleanup },
	{ "tcp6", PF_INET6, (addrparse_fn)notsupp, (connecthook_fn)success,
			    (cleanup_fn)success },
};
#define NPARSE (sizeof(parsetab)/sizeof(parsetab[0]))

static int
parseurl(const char *url, struct sockaddr **sap, unsigned *idxp,
	int allow_wildcard)
{
	char id[16];
	const char *p, *p2;
	size_t l;
	unsigned i;
	int error;

	/*
	 * Parse the url
	 */

	p = url;
	p2 = strstr(p, "://");
	if (!p2) {
		fprintf(stderr, "rump_sp: invalid locator ``%s''\n", p);
		return EINVAL;
	}
	l = p2-p;
	if (l > sizeof(id)-1) {
		fprintf(stderr, "rump_sp: identifier too long in ``%s''\n", p);
		return EINVAL;
	}

	strncpy(id, p, l);
	id[l] = '\0';
	p2 += 3; /* beginning of address */

	for (i = 0; i < NPARSE; i++) {
		if (strcmp(id, parsetab[i].id) == 0) {
			error = parsetab[i].ap(p2, sap, allow_wildcard);
			if (error)
				return error;
			break;
		}
	}
	if (i == NPARSE) {
		fprintf(stderr, "rump_sp: invalid identifier ``%s''\n", p);
		return EINVAL;
	}

	*idxp = i;
	return 0;
}
