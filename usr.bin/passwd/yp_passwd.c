/*	$NetBSD: yp_passwd.c,v 1.14 1997/05/21 02:09:51 lukem Exp $	*/

/*
 * Copyright (c) 1988, 1990, 1993, 1994
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

#ifndef lint
#if 0
static char sccsid[] = "from:  @(#)local_passwd.c    8.3 (Berkeley) 4/2/94";
#else
static char rcsid[] = "$NetBSD: yp_passwd.c,v 1.14 1997/05/21 02:09:51 lukem Exp $";
#endif
#endif /* not lint */

#ifdef	YP

#include <err.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <time.h>
#include <pwd.h>
#include <errno.h>
#include <rpc/rpc.h>
#include <rpcsvc/yp_prot.h>
#include <rpcsvc/ypclnt.h>
#define passwd yp_passwd_rec
#include <rpcsvc/yppasswd.h>
#undef passwd

#ifndef _PASSWORD_LEN
#define _PASSWORD_LEN PASS_MAX
#endif

extern	char *__progname;		/* from crt0.o */

extern	int yflag, yppwd;

static char *getnewpasswd();
static struct passwd *ypgetpwnam();

static uid_t uid;
char *domain;

static
pw_error(name, err, eval)
	char *name;
	int err, eval;
{
	int sverrno;

	if (err) {
		sverrno = errno;
		(void)fprintf(stderr, "%s: ", __progname);
		if (name)
			(void)fprintf(stderr, "%s: ", name);
		(void)fprintf(stderr, "%s\n", strerror(sverrno));
	}
	errx(eval, "YP passwd database unchanged");
}

int
yp_passwd(username)
	char *username;
{
	char *master;
	char *pp;
	int r, rpcport, status;
	struct yppasswd yppasswd;
	struct passwd *pw;
	struct timeval tv;
	CLIENT *client;
	
	uid = getuid();

	/*
	 * Get local domain
	 */
	if (r = yp_get_default_domain(&domain))
		errx(1, "can't get local YP domain.  Reason: %s",
		    yperr_string(r));

	/*
	 * Find the host for the passwd map; it should be running
	 * the daemon.
	 */
	if ((r = yp_master(domain, "passwd.byname", &master)) != 0)
		errx(1, "can't find the master YP server.  Reason: %s",
		    yperr_string(r));

	/*
	 * Ask the portmapper for the port of the daemon.
	 */
	if ((rpcport = getrpcport(master, YPPASSWDPROG,
	    YPPASSWDPROC_UPDATE, IPPROTO_UDP)) == 0) {
		/*
		 * Master server isn't running rpc.yppasswdd.  Check to see
		 * if there's a local passwd entry, and if there is, use
		 * it iff:
		 *	- we were not invoked as "yppasswd"
		 *	- we were not passed "-y", and defaulted to YP
		 */
		if (yppwd == 0 && yflag == 0)
			if (getpwnam(username) != NULL)
				exit(local_passwd(username));
		errx(1, "master YP server not running yppasswd daemon.\n\t%s\n",
		    "Can't change password.");
	}

	/*
	 * Be sure the port is priviledged
	 */
	if (rpcport >= IPPORT_RESERVED)
		errx(1, "yppasswd daemon is on an invalid port.");

	/* Get user's login identity */
	if (!(pw = ypgetpwnam(username)))
		errx(1, "unknown user %s", username);

	if (uid && uid != pw->pw_uid)
		errx(1, "you may only change your own password: %s",
		    strerror(EACCES));

	/* prompt for new password */
	yppasswd.newpw.pw_passwd = getnewpasswd(pw, &yppasswd.oldpass);

	/* tell rpc.yppasswdd */
	yppasswd.newpw.pw_name	= pw->pw_name;
	yppasswd.newpw.pw_uid 	= pw->pw_uid;
	yppasswd.newpw.pw_gid	= pw->pw_gid;
	yppasswd.newpw.pw_gecos = pw->pw_gecos;
	yppasswd.newpw.pw_dir	= pw->pw_dir;
	yppasswd.newpw.pw_shell	= pw->pw_shell;
	
	client = clnt_create(master, YPPASSWDPROG, YPPASSWDVERS, "udp");
	if (client==NULL) {
		warnx("cannot contact yppasswdd on %s:  Reason: %s",
		    master, yperr_string(YPERR_YPBIND));
		return(YPERR_YPBIND);
	}

	client->cl_auth = authunix_create_default();
	tv.tv_sec = 2;
	tv.tv_usec = 0;
	r = clnt_call(client, YPPASSWDPROC_UPDATE,
	    xdr_yppasswd, &yppasswd, xdr_int, &status, tv);
	if (r)
		errx(1, "rpc to yppasswdd failed.");
	else if (status)
		printf("Couldn't change YP password.\n");
	else
		printf("The YP password has been changed on %s, %s\n",
		    master, "the master YP passwd server.");
	exit(0);
}

static char *
getnewpasswd(pw, old_pass)
	struct passwd *pw;
	char **old_pass;
{
	int tries;
	char *p, *t;
	static char buf[_PASSWORD_LEN+1];
	char salt[9], *crypt(), *getpass();
	
	(void)printf("Changing YP password for %s.\n", pw->pw_name);

	if (old_pass) {
		*old_pass = NULL;
	
		if (pw->pw_passwd[0]) {
			if (strcmp(crypt(p = getpass("Old password:"),
					 pw->pw_passwd),  pw->pw_passwd)) {
				   errno = EACCES;
				   pw_error(NULL, 1, 1);
			}
		} else {
			p = "";
		}

		*old_pass = strdup(p);
	}
	for (buf[0] = '\0', tries = 0;;) {
		p = getpass("New password:");
		if (!*p) {
			(void)printf("Password unchanged.\n");
			pw_error(NULL, 0, 0);
		}
		if (strlen(p) <= 5 && ++tries < 2) {
			(void)printf("Please enter a longer password.\n");
			continue;
		}
		for (t = p; *t && islower(*t); ++t);
		if (!*t && ++tries < 2) {
			(void)printf("Please don't use an all-lower case "
				     "password.\nUnusual capitalization, "
				     "control characters or digits are "
				     "suggested.\n");
			continue;
		}
		(void)strncpy(buf, p, sizeof(buf) - 1);
		if (!strcmp(buf, getpass("Retype new password:")))
			break;
		(void)printf("Mismatch; try again, EOF to quit.\n");
	}
	/* grab a random printable character that isn't a colon */
	(void)srandom((int)time((time_t *)NULL));
#ifdef NEWSALT
	salt[0] = _PASSWORD_EFMT1;
	to64(&salt[1], (long)(29 * 25), 4);
	to64(&salt[5], random(), 4);
#else
	to64(&salt[0], random(), 2);
#endif
	return(strdup(crypt(buf, salt)));
}

static char *
pwskip(char *p)
{
	while (*p && *p != ':' && *p != '\n')
		++p;
	if (*p)
		*p++ = 0;
	return (p);
}

struct passwd *
interpret(struct passwd *pwent, char *line)
{
	char	*p = line;
	int	c;

	pwent->pw_passwd = "*";
	pwent->pw_uid = 0;
	pwent->pw_gid = 0;
	pwent->pw_gecos = "";
	pwent->pw_dir = "";
	pwent->pw_shell = "";
	pwent->pw_change = 0;
	pwent->pw_expire = 0;
	pwent->pw_class = "";
	
	/* line without colon separators is no good, so ignore it */
	if(!strchr(p,':'))
		return(NULL);

	pwent->pw_name = p;
	p = pwskip(p);
	pwent->pw_passwd = p;
	p = pwskip(p);
	pwent->pw_uid = (uid_t)strtoul(p, NULL, 10);
	p = pwskip(p);
	pwent->pw_gid = (gid_t)strtoul(p, NULL, 10);
	p = pwskip(p);
	pwent->pw_gecos = p;
	p = pwskip(p);
	pwent->pw_dir = p;
	p = pwskip(p);
	pwent->pw_shell = p;
	while (*p && *p != '\n')
		p++;
	*p = '\0';
	return (pwent);
}

static struct passwd *
ypgetpwnam(nam)
	char *nam;
{
	static struct passwd pwent;
	static char line[1024];
	char *val;
	int reason, vallen;
	
	val = NULL;
	reason = yp_match(domain, "passwd.byname", nam, strlen(nam),
			  &val, &vallen);
	if (reason != 0) {
		if (val != NULL)
			free(val);
		return (NULL);
	}
	val[vallen] = '\0';
	(void)strncpy(line, val, sizeof(line) - 1);
	free(val);

	return(interpret(&pwent, line));
}

#endif	/* YP */
