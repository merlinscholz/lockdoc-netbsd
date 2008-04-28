/*	$NetBSD: bcopy.s,v 1.9 2008/04/28 20:23:13 martin Exp $	*/

/*-
 * Copyright (c) 1996 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Ignatios Souvatzis.
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

/*
 * Small but possibly slow bcopy/memcpy combo.
 */

#include <machine/asm.h>

	.text
	.even

ENTRY_NOPROFILE(memmove)
ENTRY_NOPROFILE(memcpy)
	movel %sp@(4),%a0
	movel %sp@(8),%a1
	jra Lcpy

ENTRY_NOPROFILE(bcopy)
	movel %sp@(4),%a1
	movel %sp@(8),%a0
Lcpy:
	movel %sp@(12),%d0
	jeq L1
	cmpl %a1,%a0
	jcc L3
L4:
	moveb %a1@+,%a0@+
	subql #1,%d0
	jne L4
	rts
L3:
	addl %d0,%a1
	addl %d0,%a0
L9:
	moveb %a1@-,%a0@-
	subql #1,%d0
	jne L9
L1:
	rts
