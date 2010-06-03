/* Id: crti.c,v 1.1 2009/08/14 02:24:36 gmcgarry Exp  */	
/* $NetBSD: crti.c,v 1.1.1.2 2010/06/03 18:58:05 plunky Exp $ */
/*-
 * Copyright (c) 2009 Gregory McGarry <g.mcgarry@ieee.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "common.h"

asm(	"	.section .init		\n"
	"	.globl _init	\n"
	"	.align 16	\n"
	"_init:			\n"
	"	subq $8,%rsp	\n"
	"	.previous	\n");

asm(	"	.section .fini	\n"
	"	.globl _fini	\n"
	"	.align 16	\n"
	"_fini:			\n"
	"	subq $8,%rsp	\n"
	"	.previous	\n");

IDENT("Id: crti.c,v 1.1 2009/08/14 02:24:36 gmcgarry Exp ");	
IDENT("$NetBSD: crti.c,v 1.1.1.2 2010/06/03 18:58:05 plunky Exp $");
