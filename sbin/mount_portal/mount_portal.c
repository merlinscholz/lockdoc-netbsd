/*
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 * All rights reserved.
 *
 * This code is derived from software donated to Berkeley by
 * Jan-Simon Pendry.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	from: Id: portald.c,v 1.3 1992/05/27 07:09:27 jsp Exp jsp
 *	from: @(#)mount_portal.c	8.1 (Berkeley) 6/5/93
 *	$Id: mount_portal.c,v 1.1 1994/01/12 20:01:45 cgd Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/syslog.h>
#include <sys/mount.h>

#include "pathnames.h"
#include "portald.h"

static sig_atomic_t readcf;	/* Set when SIGHUP received */

static void sigchld(sig)
int sig;
{
	pid_t pid;

	while ((pid = waitpid((pid_t) -1, (int *) 0, WNOHANG)) > 0)
		;
	if (pid < 0)
		syslog(LOG_WARNING, "waitpid: %s", strerror(errno));
}

int main(c, v)
int c;
char *v[];
{
	struct portal_args args;
	struct sockaddr_un un;
	char *conf;
	char *mountpt;
	int mntflags = 0;
	int usage = 0;
	char tag[32];

	qelem q;
	int rc;
	int so;

	/*
	 * Crack command line args
	 */
	int ch;

	while ((ch = getopt(c, v, "F:")) != EOF) {
		switch (ch) {
		case 'F':
			mntflags = atoi(optarg);
			break;
		default:
			usage = 1;
			break;
		}
	}

	if (optind != (c - 2))
		usage = 1;

	if (usage) {
		fprintf(stderr, "Usage: mount_portal [ fsoptions ] config mount-point\n");
		exit(1);
	}

	/*
	 * Get config file and mount point
	 */
	conf = v[optind];
	mountpt = v[optind+1];

	/*
	 * Construct the listening socket
	 */
	un.sun_family = AF_UNIX;
	if (sizeof(_PATH_TMPPORTAL) >= sizeof(un.sun_path)) {
		fprintf(stderr, "mount_portal: portal socket name too long\n");
		exit(1);
	}
	strcpy(un.sun_path, _PATH_TMPPORTAL);
	mktemp(un.sun_path);
	un.sun_len = strlen(un.sun_path);

	so = socket(AF_UNIX, SOCK_STREAM, 0);
	if (so < 0) {
		fprintf(stderr, "mount_portal: socket: %s\n", strerror(errno));
		exit(1);
	}
	(void) unlink(un.sun_path);
	if (bind(so, (struct sockaddr *) &un, sizeof(un)) < 0) {
		fprintf(stderr, "mount_portal: bind: %s\n", strerror(errno));
		exit(1);
	}
	(void) unlink(un.sun_path);

	(void) listen(so, 5);

	args.pa_socket = so;
	sprintf(tag, "portal:%d", getpid());
	args.pa_config = tag;

	rc = mount(MOUNT_PORTAL, mountpt, mntflags, &args);
	if (rc < 0) {
		fprintf(stderr, "mount_portal: mount: %s\n", strerror(errno));
		exit(1);
	}

#ifdef notdef
	/*
	 * Everything is ready to go - now is a good time to fork
	 */
	daemon(0, 0);
#endif

	/*
	 * Start logging (and change name)
	 */
	openlog("portald", LOG_CONS|LOG_PID, LOG_DAEMON);

	q.q_forw = q.q_back = &q;
	readcf = 1;

	signal(SIGCHLD, sigchld);

	/*
	 * Just loop waiting for new connections and activating them
	 */
	for (;;) {
		struct sockaddr_un un2;
		int len2 = sizeof(un2);
		int so2;
		pid_t pid;
		fd_set fdset;
		int rc;

		/*
		 * Check whether we need to re-read the configuration file
		 */
		if (readcf) {
			readcf = 0;
			conf_read(&q, conf);
			continue;
		}
	
		/*
		 * Accept a new connection
		 * Will get EINTR if a signal has arrived, so just
		 * ignore that error code
		 */
		FD_SET(so, &fdset);
		rc = select(so+1, &fdset, (void *) 0, (void *) 0, (void *) 0);
		if (rc < 0) {
			if (errno == EINTR)
				continue;
			syslog(LOG_ERR, "select: %s", strerror(errno));
			exit(1);
		}
		if (rc == 0)
			break;
		so2 = accept(so, (struct sockaddr *) &un2, &len2);
		if (so2 < 0) {
			/*
			 * The unmount function does a shutdown on the socket
			 * which will generated ECONNABORTED on the accept.
			 */
			if (errno == ECONNABORTED)
				break;
			if (errno != EINTR) {
				syslog(LOG_ERR, "accept: %s", strerror(errno));
				exit(1);
			}
			continue;
		}

		/*
		 * Now fork a new child to deal with the connection
		 */
#ifdef notdef
	eagain:;
		switch (pid = fork()) {
		case -1:
			if (errno == EAGAIN) {
				sleep(1);
				goto eagain;
			}
			syslog(LOG_ERR, "fork: %s", strerror(errno));
			break;
		case 0:
			(void) close(so);
			activate(&q, so2);
			break;
		default:
			(void) close(so2);
			break;
		}
#endif
		activate(&q, so2);
	}
	syslog(LOG_INFO, "%s unmounted", mountpt);
	exit(0);
}
