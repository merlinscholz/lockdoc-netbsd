/*	$NetBSD: announce.c,v 1.25 2009/03/16 01:13:38 lukem Exp $	*/

/*
 * Copyright (c) 1983, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
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
 */

#include <sys/cdefs.h>
#ifndef lint
#if 0
static char sccsid[] = "@(#)announce.c	8.3 (Berkeley) 4/28/95";
#else
__RCSID("$NetBSD: announce.c,v 1.25 2009/03/16 01:13:38 lukem Exp $");
#endif
#endif /* not lint */

#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <protocols/talkd.h>
#include <errno.h>
#include <syslog.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vis.h>
#include <paths.h>
#include <util.h>
#include "extern.h"

extern char hostname[];

/*
 * Announce an invitation to talk.
 */

/*
 * See if the user is accepting messages. If so, announce that
 * a talk is requested.
 */
int
announce(CTL_MSG *request, const char *remote_machine)
{
	char full_tty[32];
	struct stat stbuf;

	(void)snprintf(full_tty, sizeof(full_tty),
	    "%s%s", _PATH_DEV, request->r_tty);
	if (stat(full_tty, &stbuf) < 0 || (stbuf.st_mode&S_IWGRP) == 0)
		return (PERMISSION_DENIED);
	return (print_mesg(request->r_tty, request, remote_machine));
}

#define max(a, b) ((a) > (b) ? (a) : (b))
#define N_LINES 5
#define N_CHARS 256

/*
 * Build a block of characters containing the message.
 * It is sent blank filled and in a single block to
 * try to keep the message in one piece if the recipient
 * is in vi at the time.
 */
int
print_mesg(const char *tty, CTL_MSG *request, const char *remote_machine)
{
	struct timeval clocktv;
	time_t clocktime;
	struct tm *localclock;
	struct iovec iovec;
	char line_buf[N_LINES][N_CHARS];
	int sizes[N_LINES];
	char big_buf[(N_LINES + 1) * N_CHARS];
	char *bptr, *lptr, vis_user[sizeof(request->l_name) * 4];
	int i, j, max_size;

	i = 0;
	max_size = 0;
	(void)gettimeofday(&clocktv, NULL);
	clocktime = clocktv.tv_sec;
	localclock = localtime(&clocktime);
	(void)snprintf(line_buf[i], N_CHARS, " ");
	sizes[i] = strlen(line_buf[i]);
	max_size = max(max_size, sizes[i]);
	i++;

	(void)snprintf(line_buf[i], N_CHARS,
	    "Message from Talk_Daemon@%s at %d:%02d ...",
	    hostname, localclock->tm_hour, localclock->tm_min);
	sizes[i] = strlen(line_buf[i]);
	max_size = max(max_size, sizes[i]);
	i++;
	if (strlen(request->l_name) + 1 > sizeof(vis_user) / 4)
		return (FAILED);
	strvis(vis_user, request->l_name, VIS_CSTYLE);
	(void)snprintf(line_buf[i], N_CHARS,
	    "talk: connection requested by %s@%s.", vis_user, remote_machine);
	sizes[i] = strlen(line_buf[i]);
	max_size = max(max_size, sizes[i]);
	i++;
	(void)snprintf(line_buf[i], N_CHARS,
	    "talk: respond with:  talk %s@%s", vis_user, remote_machine);
	sizes[i] = strlen(line_buf[i]);
	max_size = max(max_size, sizes[i]);
	i++;
	(void)snprintf(line_buf[i], N_CHARS, " ");
	sizes[i] = strlen(line_buf[i]);
	max_size = max(max_size, sizes[i]);
	i++;
	bptr = big_buf;
	*bptr++ = '\a';			/* send something to wake them up */
	*bptr++ = '\r';			/* add a \r in case of raw mode */
	*bptr++ = '\n';
	for (i = 0; i < N_LINES; i++) {
		/* copy the line into the big buffer */
		lptr = line_buf[i];
		while (*lptr != '\0' && lptr < (line_buf[i] + N_CHARS))
			*(bptr++) = *(lptr++);
		/* pad out the rest of the lines with blanks */
		for (j = sizes[i]; j < max_size + 2; j++)
			*(bptr++) = ' ';
		*(bptr++) = '\r';	/* add a \r in case of raw mode */
		*(bptr++) = '\n';
	}
	*bptr = '\0';
	iovec.iov_base = big_buf;
	iovec.iov_len = bptr - big_buf;
	/*
	 * we choose a timeout of RING_WAIT-5 seconds so that we don't
	 * stack up processes trying to write messages to a tty
	 * that is permanently blocked.
	 */
	if (ttymsg(&iovec, 1, tty, RING_WAIT - 5) != NULL)
		return (FAILED);

	return (SUCCESS);
}
