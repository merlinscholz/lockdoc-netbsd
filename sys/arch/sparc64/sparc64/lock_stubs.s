/*	$NetBSD: lock_stubs.s,v 1.3 2007/02/20 15:30:53 martin Exp $	*/

/*-
 * Copyright (c) 2002, 2006 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe and Andrew Doran.
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
 *	This product includes software developed by the NetBSD
 *	Foundation, Inc. and its contributors.
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

#include "opt_multiprocessor.h"
#include "opt_lockdebug.h"

#include <machine/param.h>
#include <machine/asm.h>

#include "assym.h"

#undef CURLWP
#if defined(MULTIPROCESSOR)
#define	CURLWP	(CPUINFO_VA+CI_CURLWP)
#define	MB_READ	membar #LoadLoad
#define	MB_MEM	membar #LoadStore | #StoreStore
#else
#define	CURLWP	_C_LABEL(curlwp)
#define	MB_READ	/* nothing */
#define	MB_MEM	/* nothing */
#endif

#ifdef __arch64__
#define	CASPTR	casx
#define	LDPTR	ldx
#define	STPTR	stx
#else
#define	CASPTR	cas
#define	LDPTR	ld
#define	STPTR	st
#endif /* __arch64__ */

/*
 * int _lock_cas(uintptr_t *ptr, uintptr_t old, uintptr_t new);
 */
_ENTRY(_C_LABEL(_lock_cas))
	CASPTR	[%o0], %o1, %o2			! compare-and-swap
	MB_MEM
	cmp	%o1, %o2			! expected == actual?
	bne,pn	%icc,1f				! nope
	 nop
	retl
	 mov	1, %o0
1:	retl
	 clr	%o0

#if !defined(LOCKDEBUG)

/*
 * void mutex_enter(kmutex_t *);
 */
_ENTRY(_C_LABEL(mutex_enter))
	sethi	%hi(CURLWP), %o1
	LDPTR	[%o1 + %lo(CURLWP)], %o1	! current thread
	CASPTR	[%o0], %g0, %o1			! compare-and-swap
	MB_READ
	brnz,pn	%o1, 1f				! lock was unowned?
	 nop
	retl					! - yes, done
	 nop
1:	sethi	%hi(_C_LABEL(mutex_vector_enter)),%o2	! nope, do it the
	jmp	%o2 + %lo(_C_LABEL(mutex_vector_enter))	! hard way
	 nop

/*
 * void mutex_exit(kmutex_t *);
 *
 * XXX This should use a restartable sequence.  See mutex_vector_enter().
 */
_ENTRY(_C_LABEL(mutex_exit))
	sethi	%hi(CURLWP), %o1
	LDPTR	[%o1 + %lo(CURLWP)], %o1	! current thread
	clr	%o2				! new value (0)
	MB_MEM
	CASPTR	[%o0], %o1, %o2			! compare-and-swap
	brnz,pn	%o2, 1f				! nope, hard case
	 nop
	retl
	 nop
1:	sethi	%hi(_C_LABEL(mutex_vector_exit)), %o2
	jmp	%o2 + %lo(_C_LABEL(mutex_vector_exit))
	 nop

#endif	/* !LOCKDEBUG */
