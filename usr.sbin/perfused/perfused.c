/*  $NetBSD: perfused.c,v 1.8 2010/09/15 01:51:44 manu Exp $ */

/*-
 *  Copyright (c) 2010 Emmanuel Dreyfus. All rights reserved.
 * 
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 
 *  THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 *  ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 *  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */ 

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <syslog.h>
#include <ctype.h>
#include <paths.h>
#include <stdarg.h>
#include <err.h>
#include <errno.h>
#include <string.h>
#include <sysexits.h>
#include <signal.h>
#include <puffs.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <sys/queue.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <machine/vmparam.h>

#include "../../lib/libperfuse/perfuse_if.h"
#include "perfused.h"

static int access_mount(const char *, uid_t, int);
static void new_mount(int, int);
static int parse_debug(char *);
static void siginfo_handler(int);
static int parse_options(int, char **);
static void get_mount_info(int, struct perfuse_mount_info *);
int main(int, char **);

/*
 * Flags for new_mount()
 */
#define  PMNT_DEVFUSE	0x0	/* We use /dev/fuse */
#define  PMNT_SOCKPAIR	0x1	/* We use socketpair */


static int
access_mount(mnt, uid, ro)
	const char *mnt;
	uid_t uid;
	int ro;
{
	struct stat st;
	mode_t mode;

	if (uid == 0)
		return 0;

	if (stat(mnt, &st) == -1)
		return -1;
	
	if (st.st_uid != uid)
		return -1;

	mode = S_IRUSR;
	if (!ro)
		mode |= S_IWUSR;
	
	if ((st.st_mode & mode) == mode)
		return 0;

	return -1;
}

static void
get_mount_info(fd, pmi)
	int fd;
	struct perfuse_mount_info *pmi;
{
	struct perfuse_mount_out *pmo;
	struct sockcred cred;
	char *cp;
	char *source = NULL;
	char *target = NULL;
	char *filesystemtype = NULL;
	long mountflags = 0;
	void *data = NULL;
	char *sock = NULL;

	pmo = (struct perfuse_mount_out *)
		perfuse_recv_early(fd, &cred, sizeof(cred));

	if (pmo == NULL) {
		if (shutdown(fd, SHUT_RDWR) != 0)
			DERR(EX_OSERR, "shutdown failed");
		exit(EX_PROTOCOL);
	}

#ifdef PERFUSE_DEBUG
	if (perfuse_diagflags & PDF_MISC)
		DPRINTF("perfuse lengths: source = %"PRId32", "
			"target = %"PRId32", filesystemtype = %"PRId32", "
			"data = %"PRId32", sock = %"PRId32"\n", 
			pmo->pmo_source_len, pmo->pmo_target_len, 
			pmo->pmo_filesystemtype_len, pmo->pmo_data_len,
			pmo->pmo_sock_len);
#endif
	cp = (char *)(void *)(pmo + 1);
	
	if (pmo->pmo_source_len != 0) {
		source = cp;
		cp += pmo->pmo_source_len;
	}

	if (pmo->pmo_target_len != 0) {
		target = cp;
		cp += pmo->pmo_target_len;
	}

	if (pmo->pmo_filesystemtype_len != 0) {
		filesystemtype = cp;
		cp += pmo->pmo_filesystemtype_len;
	}

	mountflags = pmo->pmo_mountflags;

	if (pmo->pmo_data_len != 0) {
		data = cp;
		cp += pmo->pmo_data_len;
	}

	if (pmo->pmo_sock_len != 0) {
		sock = cp;
		cp += pmo->pmo_sock_len;
	}

#ifdef PERFUSE_DEBUG
	if (perfuse_diagflags & PDF_MISC)
		DPRINTF("%s(\"%s\", \"%s\", \"%s\", 0x%lx, \"%s\", \"%s\")\n", 
		__func__, source, target, filesystemtype, 
		mountflags, (const char *)data, sock);
#endif
	pmi->pmi_source = source;
	pmi->pmi_target = target;
	pmi->pmi_filesystemtype = filesystemtype;
	pmi->pmi_mountflags = (int)mountflags;
	pmi->pmi_data = data;

	pmi->pmi_uid = cred.sc_euid;

	/*
	 * Connect to the remote socket, if provided
	 */
	if (sock) {
		const struct sockaddr *sa;
		struct sockaddr_un sun;

		sa = (const struct sockaddr *)(void *)&sun;
		sun.sun_len = sizeof(sun);
		sun.sun_family = AF_LOCAL;
		strcpy(sun.sun_path, sock);

		if (connect(fd, sa, sun.sun_len) != 0)
			DERR(EX_OSERR, "connect \"%s\" failed", sun.sun_path);
	}

	return;
}

static void
new_mount(fd, pmnt_flags)
	int fd;
	int pmnt_flags;
{
	struct puffs_usermount *pu;
	struct perfuse_mount_info pmi;
	struct perfuse_callbacks pc;
	int ro_flag;
	pid_t pid;
	int flags;

	pid = (perfuse_diagflags & PDF_FOREGROUND) ? 0 : fork();
	switch(pid) {
	case -1:
		DERR(EX_OSERR, "cannot fork");
		break;
	case 0:
		break;
	default:
		return;
		/* NOTREACHED */
		break;
	}

	/*
	 * Mount information (source, target, mount flags...)
	 */
	get_mount_info(fd, &pmi);

	/*
	 * Check that peer owns mountpoint and read (and write) on it?
	 */
	ro_flag = pmi.pmi_mountflags & MNT_RDONLY;
	if (access_mount(pmi.pmi_target, pmi.pmi_uid, ro_flag) != 0)
		DERRX(EX_NOPERM, "insuficient privileges to mount on %s", 
		      pmi.pmi_target);


	/*
	 * Initialize libperfuse, which will initialize libpuffs
	 */
	pc.pc_new_msg = perfuse_new_pb;
	pc.pc_xchg_msg = perfuse_xchg_pb;
	pc.pc_destroy_msg = (perfuse_destroy_msg_fn)puffs_framebuf_destroy;
	pc.pc_get_inhdr = perfuse_get_inhdr;
	pc.pc_get_inpayload = perfuse_get_inpayload;
	pc.pc_get_outhdr = perfuse_get_outhdr;
	pc.pc_get_outpayload = perfuse_get_outpayload;

	pu = perfuse_init(&pc, &pmi);
	
	puffs_framev_init(pu, perfuse_readframe, perfuse_writeframe, 
			  perfuse_cmpframe, perfuse_gotframe, perfuse_fdnotify);

	if (puffs_framev_addfd(pu, fd, PUFFS_FBIO_READ|PUFFS_FBIO_WRITE) == -1)
		DERR(EX_SOFTWARE, "puffs_framev_addfd failed");

	perfuse_setspecific(pu, (void *)(long)fd);

	setproctitle("perfused %s", pmi.pmi_target);
	(void)kill(getpid(), SIGINFO);		/* This is for -s option */

	perfuse_fs_init(pu);

	/*
	 * Non blocking I/O on /dev/fuse
	 * This must be done after perfuse_fs_init
	 */
	if ((flags = fcntl(fd, F_GETFL, 0)) == -1)
		DERR(EX_OSERR, "fcntl failed");
	if (fcntl(fd, F_SETFL, flags|O_NONBLOCK) != 0)
		DERR(EX_OSERR, "fcntl failed");

	/*
	 * Hand over control to puffs main loop.
	 */
	(void)perfuse_mainloop(pu);

	DERRX(EX_SOFTWARE, "perfuse_mainloop exit");

	/* NOTREACHED */
	return;
}

static int 
parse_debug(optstr)
	char *optstr;
{
	int retval = PDF_SYSLOG;
	char *opt;
	char *lastp;

	for (opt = strtok_r(optstr, ",", &lastp);
	     opt;
	     opt = strtok_r(NULL, ",", &lastp)) {
		if (strcmp(opt, "fuse") == 0)
			retval |= PDF_FUSE;
		else if (strcmp(opt, "puffs") == 0)
			retval |= PDF_PUFFS;
		else if (strcmp(opt, "dump") == 0)
			retval |= PDF_DUMP;
		else if (strcmp(opt, "fh") == 0)
			retval |= PDF_FH;
		else if (strcmp(opt, "readdir") == 0)
			retval |= PDF_READDIR;
		else if (strcmp(opt, "reclaim") == 0)
			retval |= PDF_RECLAIM;
		else if (strcmp(opt, "requeue") == 0)
			retval |= PDF_REQUEUE;
		else if (strcmp(opt, "sync") == 0)
			retval |= PDF_SYNC;
		else if (strcmp(opt, "misc") == 0)
			retval |= PDF_MISC;
		else
			DERRX(EX_USAGE, "unknown debug flag \"%s\"", opt);
	}

	return retval;
}

/* ARGSUSED0 */
static void
siginfo_handler(sig)
	int sig;
{
	static int old_flags = 0;
	int swap;

	swap = perfuse_diagflags;
	perfuse_diagflags = old_flags;
	old_flags = swap;

	DWARNX("debug %sabled", old_flags == 0 ? "en" : "dis");

	return;
}

static int
parse_options(argc, argv)
	int argc;
	char **argv;
{
	int ch;
	int foreground = 0;
	int retval = -1;

	perfuse_diagflags = PDF_FOREGROUND | PDF_SYSLOG;

	while ((ch = getopt(argc, argv, "d:fsi:")) != -1) {
		switch (ch) {
		case 'd':
			perfuse_diagflags |= parse_debug(optarg);
			break;
		case 's':
			if (signal(SIGINFO, siginfo_handler) != 0)
				DERR(EX_OSERR, "signal failed");
			break;
		case 'f':
			foreground = 1;
			perfuse_diagflags |= PDF_MISC;
			break;
		case 'i':
			retval = atoi(optarg);
			foreground = 1;
			break;
		default:
			DERR(EX_USAGE, "%s [-fs] [-d level] [-i fd]", argv[0]);
			break;
		}
	}
	
	if (!foreground)
		perfuse_diagflags &= ~PDF_FOREGROUND;

	return retval; 
}

int
main(argc, argv)
	int argc;
	char **argv;
{
	int s;

	s = parse_options(argc, argv);

	if (perfuse_diagflags & PDF_SYSLOG)
		openlog("perfused", 0, LOG_DAEMON);

	if (!(perfuse_diagflags & PDF_FOREGROUND))
		if (daemon(0, 0) != 0)
			DERR(EX_OSERR, "daemon failed");

	if (s != -1) {
		new_mount(s, PMNT_SOCKPAIR);
		DERRX(EX_SOFTWARE, "new_mount exit while -i is used");
	}

	s = perfuse_open_sock();
	
	do {
#if (PERFUSE_SOCKTYPE != SOCK_DGRAM)
		struct sockaddr *sa;
		struct sockaddr_storage ss;
		socklen_t ss_len;
#endif
		int fd;

#ifdef PERFUSE_DEBUG
		if (perfuse_diagflags & PDF_MISC)
			DPRINTF("waiting connexion\n");
#endif
#if (PERFUSE_SOCKTYPE == SOCK_DGRAM)
		fd = s;
#else
		sa = (struct sockaddr *)(void *)&ss;
		ss_len = sizeof(ss);

		if ((fd = accept(s, sa, &ss_len)) == -1)
			DERR(EX_OSERR,  "accept failed");

#ifdef PERFUSE_DEBUG
		if (perfuse_diagflags & PDF_MISC)
			DPRINTF("connexion accepted\n");
#endif /* PERFUSE_DEBUG */
#endif /* PERFUSE_SOCKTYPE */

		new_mount(fd, PMNT_DEVFUSE);
	} while (1 /* CONSTCOND */);
		
	/* NOTREACHED */
	return 0;
}
