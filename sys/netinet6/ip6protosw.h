/*	$NetBSD: ip6protosw.h,v 1.4 1999/12/13 15:17:23 itojun Exp $	*/

/*
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/*	BSDI protosw.h,v 2.3 1996/10/11 16:02:40 pjd Exp	*/

/*-
 * Copyright (c) 1982, 1986, 1993
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
 *
 *	@(#)protosw.h	8.1 (Berkeley) 6/2/93
 */

#ifndef _NETINET6_IP6PROTOSW_H_
#define _NETINET6_IP6PROTOSW_H_

/*
 * Protocol switch table for IPv6.
 * All other definitions should refer to sys/protosw.h
 */

struct mbuf;
struct sockaddr;
struct socket;
struct domain;
struct proc;
struct ip6_hdr;
#ifdef __FreeBSD__
struct pr_usrreqs;
#endif

/*
 * argument type for the last arg of pr_ctlinput().
 * should be consulted only with AF_INET6 family.
 */
struct ip6ctlparam {
	struct mbuf *ip6c_m;		/* start of mbuf chain */
	struct ip6_hdr *ip6c_ip6;	/* ip6 header of target packet */
	int ip6c_off;			/* offset of the target proto header */
};

struct ip6protosw {
#if (defined(__FreeBSD__) && __FreeBSD__ < 3) || defined(__OpenBSD__)
	short 	pr_type;		/* socket type used for */
#else
	int 	pr_type;		/* socket type used for */
#endif
	struct	domain *pr_domain;	/* domain protocol a member of */
	short	pr_protocol;		/* protocol number */
	short	pr_flags;		/* see below */

/* protocol-protocol hooks */
	int	(*pr_input)		/* input to protocol (from below) */
			__P((struct mbuf **, int *, int));
#ifdef __bsdi__
	int	(*pr_output)();		/* output to protocol (from above) */
#else
	int	(*pr_output)		/* output to protocol (from above) */
			__P((struct mbuf *, ...));
#endif
	void	(*pr_ctlinput)		/* control input (from below) */
			__P((int, struct sockaddr *, void *));
	int	(*pr_ctloutput)		/* control output (from above) */
			__P((int, struct socket *, int, int, struct mbuf **));

/* user-protocol hook */
#if !(defined(__FreeBSD__) && __FreeBSD__ < 3) && !defined(__bsdi__)
	int	(*pr_usrreq)		/* user request: see list below */
			__P((struct socket *, int, struct mbuf *,
			     struct mbuf *, struct mbuf *, struct proc *));
#else
	int	(*pr_usrreq)		/* user request: see list below */
			__P((struct socket *, int, struct mbuf *,
			     struct mbuf *, struct mbuf *));
#endif

/* utility hooks */
	void	(*pr_init)		/* initialization hook */
			__P((void));

	void	(*pr_fasttimo)		/* fast timeout (200ms) */
			__P((void));
	void	(*pr_slowtimo)		/* slow timeout (500ms) */
			__P((void));
	void	(*pr_drain)		/* flush any excess space possible */
			__P((void));
#ifdef __FreeBSD__
	struct  pr_usrreqs *pr_usrreqs;	/* supersedes pr_usrreq() */
#else
	int	(*pr_sysctl)		/* sysctl for protocol */
			__P((int *, u_int, void *, size_t *, void *, size_t));
#endif
};

#endif /* !_NETINET6_IP6PROTOSW_H_ */
