/*
 * Copyright (c) 1993 Winning Strategies, Inc.
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Winning Strategies, Inc.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software withough specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *	$Id: memcmp.s,v 1.5 1993/09/07 16:50:00 jtc Exp $
 */

#if defined(LIBC_RCS) && !defined(lint)
	.text
	.asciz "$Id: memcmp.s,v 1.5 1993/09/07 16:50:00 jtc Exp $"
#endif /* LIBC_RCS and not lint */

#include "DEFS.h"

/*
 * memcmp (void *b1, void *b2, size_t len)
 *
 * Written by:
 *	J.T. Conklin (jtc@wimsey.com), Winning Strategies, Inc.
 */

ENTRY(memcmp)
	pushl	%edi
	pushl	%esi
	movl	12(%esp),%edi
	movl	16(%esp),%esi
	movl	20(%esp),%edx
	cld				/* set compare direction forward */

	movl	%edx,%ecx		/* compare by words */
	shrl	$2,%ecx
	repe
	cmpsl
	jne	L5			/* do we match so far? */

	movl	%edx,%ecx		/* compare remainder by bytes */
	andl	$3,%ecx
	repe
	cmpsb
	jne	L6			/* do we match? */

	xorl	%eax,%eax		/* we match, return zero	*/
	popl	%esi
	popl	%edi
	ret

L5:	movl	$4,%ecx			/* We know that one of the next	*/
	subl	%ecx,%edi		/* four pairs of bytes do not	*/
	subl	%ecx,%esi		/* match.			*/
	repe
	cmpsb
L6:	movsbl  -1(%edi),%eax		/* Perform unsigned comparison	*/
        movsbl  -1(%esi),%edx
        subl    %edx,%eax
	popl	%esi
	popl	%edi
	ret
