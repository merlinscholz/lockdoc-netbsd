/*	$NetBSD: temp.c,v 1.11 2002/03/05 20:15:33 wiz Exp $	*/

/*
 * Copyright (c) 1980, 1993
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
 */

#include <sys/cdefs.h>
#ifndef lint
#if 0
static char sccsid[] = "@(#)temp.c	8.1 (Berkeley) 6/6/93";
#else
__RCSID("$NetBSD: temp.c,v 1.11 2002/03/05 20:15:33 wiz Exp $");
#endif
#endif /* not lint */

#include "rcv.h"
#include "extern.h"

/*
 * Mail -- a mail program
 *
 * Give names to all the temporary files that we will need.
 */

char	*tempMail;
char	*tempEdit;
char	*tempResid;
char	*tempMesg;
char	*tmpdir;

void
tinit(void)
{
	const char *cp;
	char *p;

	if ((tmpdir = getenv("TMPDIR")) == NULL || *tmpdir == '\0')
		tmpdir = _PATH_TMP;

	if ((tmpdir = strdup(tmpdir)) == NULL)
		errx(1, "Out of memory");

	/* Remove trailing slashes. */
	p = tmpdir + strlen(tmpdir) - 1;
	while (p > tmpdir && *p == '/') {
		*p = '\0';
		p--;
	}

	tempMail  = tempnam (tmpdir, "Rs");
	tempResid = tempnam (tmpdir, "Rq");
	tempEdit  = tempnam (tmpdir, "Re");
	tempMesg  = tempnam (tmpdir, "Rx");

	/*
	 * It's okay to call savestr in here because main will
	 * do a spreserve() after us.
	 */
	if (myname != NULL) {
		if (getuserid(myname) < 0)
			errx(1, "\"%s\" is not a user of this system", myname);
	} else {
		if ((cp = username()) == NULL) {
			myname = "nobody";
			if (rcvmode)
				exit(1);
		} else
			myname = savestr(cp);
	}
	if ((cp = getenv("HOME")) == NULL)
		cp = ".";
	homedir = savestr(cp);
	if (debug)
		printf("user = %s, homedir = %s\n", myname, homedir);
}
