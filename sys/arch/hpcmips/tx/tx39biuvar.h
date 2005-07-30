/*	$NetBSD: tx39biuvar.h,v 1.3 2005/07/30 22:40:34 nakayama Exp $ */

/*-
 * Copyright (c) 1999 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by UCHIYAMA Yasushi.
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

#define TX39_MAXCS	16

#define	TX39_CS0	0
#define	TX39_CS1	1
#define	TX39_CS2	2
#define	TX39_CS3	3
#define	TX39_MCS0	4
#define	TX39_MCS1	5
#ifdef TX391X
#define	TX39_MCS2	6
#define	TX39_MCS3	7
#endif /* TX391X */
#define TX39_CARD1	8
#define TX39_CARD2	9
#define TX39_CARD1MEM	10
#define TX39_CARD2MEM	11
#define	TX39_KUCS0	12
#define	TX39_KUCS1	13
#define	TX39_KUCS2	14
#define	TX39_KUCS3	15

#define TX39_ISCS(x)	((x) >= TX39_CS0 && (x) <= TX39_CS3)
#define TX39_ISCARD(x)	((x) >= TX39_CARD1 && (x) <= TX39_CARD2MEM)

#ifdef TX391X
#define TX39_ISMCS(x)	((x) >= TX39_MCS0 && (x) <= TX39_MCS3)
#elif defined TX392X
#define TX39_ISMCS(x)	((x) >= TX39_MCS0 && (x) <= TX39_MCS1)
#endif

#define TX39_ISKUCS(x)	((x) >= TX39_KUCS0 && (x) <= TX39_KUCS3)
