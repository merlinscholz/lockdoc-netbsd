/*	$NetBSD: isr.h,v 1.7 2001/07/06 19:00:14 scw Exp $	*/

/*-
 * Copyright (c) 1996 The NetBSD Foundation, Inc.
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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

#include <sys/queue.h>

/*
 * The location and size of the autovectored interrupt portion
 * of the vector table.
 */
#define ISRAUTOVEC	0x18
#define NISRAUTOVEC	8
#define NIPLS		8

/*
 * The location and size of the vectored interrupt portion
 * of the vector table.
 */
#define ISRVECTORED	0x40
#define NISRVECTORED	192

/*
 * Autovectored interupt handler cookie.
 */
struct isr_autovec {
	LIST_ENTRY(isr_autovec) isr_link;
	int		(*isr_func) __P((void *));
	void		*isr_arg;
	int		isr_ipl;
	int		isr_priority;
	struct evcnt	*isr_evcnt;
};

typedef LIST_HEAD(, isr_autovec) isr_autovec_list_t;

/*
 * Vectored interrupt handler cookie.  The handler may request to
 * receive the exception frame as an argument by specifying NULL
 * when establishing the interrupt.
 */
struct isr_vectored {
	int		(*isr_func) __P((void *));
	void		*isr_arg;
	int		isr_ipl;
	struct evcnt	*isr_evcnt;
};

/*
 * Autovectored ISR priorities.  These are not the same as interrupt levels.
 */
#define ISRPRI_BIO		0
#define ISRPRI_NET		1
#define ISRPRI_TTY		2
#define ISRPRI_TTYNOBUF		3

extern	struct evcnt mvme68k_irq_evcnt[];

void	isrinit __P((void));
#define isrlink_evcnt(ipl)	&mvme68k_irq_evcnt[(ipl)]
void	isrlink_autovec __P((int (*)(void *), void *, int, int,
	    struct evcnt *));
void	isrlink_vectored __P((int (*)(void *), void *, int, int,
	    struct evcnt *));
void	isrunlink_vectored __P((int));
void	isrdispatch_autovec __P((struct clockframe *));
void	isrdispatch_vectored __P((int, struct clockframe *));
void	netintr __P((void));
