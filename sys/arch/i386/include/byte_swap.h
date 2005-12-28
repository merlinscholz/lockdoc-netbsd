/*	$NetBSD: byte_swap.h,v 1.9 2005/12/28 18:40:13 perry Exp $	*/

/*-
 * Copyright (c) 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Charles M. Hannum.
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

#ifndef _I386_BYTE_SWAP_H_
#define	_I386_BYTE_SWAP_H_

#include <sys/types.h>

#if defined(_KERNEL_OPT)
#include "opt_cputype.h"
#endif


static __inline uint32_t __byte_swap_long_variable(uint32_t);
static __inline uint16_t __byte_swap_word_variable(uint16_t);

static __inline uint32_t
__byte_swap_long_variable(uint32_t x)
{
	__asm volatile (
#if defined(_KERNEL) && !defined(_LKM) && !defined(I386_CPU)
	    "bswap %1"
#else
	    "rorw $8, %w1\n\trorl $16, %1\n\trorw $8, %w1"
#endif
	    : "=r" (x) : "0" (x));
	return (x);
}

static __inline uint16_t
__byte_swap_word_variable(uint16_t x)
{
	__asm volatile ("rorw $8, %w1" : "=r" (x) : "0" (x)); 
	return (x);
}

#ifdef __OPTIMIZE__

#define	__byte_swap_long_constant(x) \
	((((x) & 0xff000000) >> 24) | \
	 (((x) & 0x00ff0000) >>  8) | \
	 (((x) & 0x0000ff00) <<  8) | \
	 (((x) & 0x000000ff) << 24))
#define	__byte_swap_word_constant(x) \
	((((x) & 0xff00) >> 8) | \
	 (((x) & 0x00ff) << 8))
#define	__byte_swap_long(x) \
	(__builtin_constant_p((x)) ? \
	 __byte_swap_long_constant(x) : __byte_swap_long_variable(x))
#define	__byte_swap_word(x) \
	(__builtin_constant_p((x)) ? \
	 __byte_swap_word_constant(x) : __byte_swap_word_variable(x))

#else /* __OPTIMIZE__ */

#define	__byte_swap_long(x)	__byte_swap_long_variable(x)
#define	__byte_swap_word(x)	__byte_swap_word_variable(x)

#endif /* __OPTIMIZE__ */

#endif /* !_I386_BYTE_SWAP_H_ */
