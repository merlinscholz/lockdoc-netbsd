/*	$NetBSD: ip_private.h,v 1.3 2008/04/28 20:24:09 martin Exp $	*/

/*-
 * Copyright (c) 2008 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe.
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
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _NETINET_IP_PRIVATE_H_
#define _NETINET_IP_PRIVATE_H_

#ifdef _KERNEL
#include <net/net_stats.h>

extern	percpu_t *ipstat_percpu;

#define	IP_STAT_GETREF()	_NET_STAT_GETREF(ipstat_percpu)
#define	IP_STAT_PUTREF()	_NET_STAT_PUTREF(ipstat_percpu)

#define	IP_STATINC(x)		_NET_STATINC(ipstat_percpu, x)
#define	IP_STATDEC(x)		_NET_STATDEC(ipstat_percpu, x)

#ifdef __NO_STRICT_ALIGNMENT
#define	IP_HDR_ALIGNED_P(ip)	1
#else
#define	IP_HDR_ALIGNED_P(ip)	((((vaddr_t) (ip)) & 3) == 0)
#endif
#endif /* _KERNEL */

#endif /* !_NETINET_IP_PRIVATE_H_ */
