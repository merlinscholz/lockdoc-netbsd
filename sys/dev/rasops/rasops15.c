/* 	$NetBSD: rasops15.c,v 1.3 1999/04/26 04:27:47 ad Exp $ */

/*
 * Copyright (c) 1999 Andy Doran <ad@NetBSD.org>
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
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include "opt_rasops.h"
#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: rasops15.c,v 1.3 1999/04/26 04:27:47 ad Exp $");

#include <sys/types.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/time.h>

#include <dev/wscons/wsdisplayvar.h>
#include <dev/wscons/wsconsio.h>
#include <dev/rasops/rasops.h>

static void 	rasops15_putchar __P((void *, int, int, u_int, long attr));
static void 	rasops15_putchar8 __P((void *, int, int, u_int, long attr));
static void 	rasops15_putchar12 __P((void *, int, int, u_int, long attr));
static void 	rasops15_putchar16 __P((void *, int, int, u_int, long attr));
static void	rasops15_makestamp __P((struct rasops_info *, long));

void	rasops15_init __P((struct rasops_info *ri));

/* 
 * (2x2)x1 stamp for optimized character blitting 
 */
static int32_t	stamp[32];
static long	stamp_attr;
static int	stamp_mutex;	/* XXX see note in readme */

/*
 * XXX this confuses the hell out of gcc2 (not egcs) which always insists
 * that the shift count is negative.
 *
 * offset = STAMP_SHIFT(fontbits, nibble #) & STAMP_MASK
 * destination int32_t[0] = STAMP_READ(offset)
 * destination int32_t[1] = STAMP_READ(offset + 4)
 */
#define STAMP_SHIFT(fb,n)	((n*4-3) >= 0 ? (fb)>>(n*4-3):(fb)<<-(n*4-3))
#define STAMP_MASK		(15 << 3)
#define STAMP_READ(o)		(*(int32_t *)((caddr_t)stamp + (o)))


/*
 * Initalize rasops_info struct for this colordepth.
 */
void
rasops15_init(ri)
	struct rasops_info *ri;
{

	switch (ri->ri_font->fontwidth) {
	case 8:
		ri->ri_ops.putchar = rasops15_putchar8;
		break;
		
	case 12:
		ri->ri_ops.putchar = rasops15_putchar12;
		break;
		
	case 16:
		ri->ri_ops.putchar = rasops15_putchar16;
		break;

	default:
		ri->ri_ops.putchar = rasops15_putchar;
		break;
	}
	
	if (ri->ri_rnum == 0) {
		ri->ri_rnum = 5;
		ri->ri_rpos = 0;
		ri->ri_gnum = 5 + (ri->ri_depth == 16);
		ri->ri_gpos = 5;
		ri->ri_bnum = 5;
		ri->ri_bpos = 10 + (ri->ri_depth == 16);
	}
}


/*
 * Paint a single character.
 */
static void
rasops15_putchar(cookie, row, col, uc, attr)
	void *cookie;
	int row, col;
	u_int uc;
	long attr;
{
	struct rasops_info *ri;
	int fb, width, height, cnt, clr[2];
	u_char *dp, *rp, *fr;
	
	ri = (struct rasops_info *)cookie;

#ifdef RASOPS_CLIPPING	
	/* Catches 'row < 0' case too */ 
	if ((unsigned)row >= (unsigned)ri->ri_rows)
		return;

	if ((unsigned)col >= (unsigned)ri->ri_cols)
		return;
#endif
	
	rp = ri->ri_bits + row * ri->ri_yscale + col * ri->ri_xscale;
	height = ri->ri_font->fontheight;
	width = ri->ri_font->fontwidth;
	
	clr[1] = ri->ri_devcmap[((u_int)attr >> 24) & 15];
	clr[0] = ri->ri_devcmap[((u_int)attr >> 16) & 15];

	if (uc == ' ') {
		while (height--) {
			dp = rp;
			rp += ri->ri_stride;
		
			for (cnt = width; cnt; cnt--) {
				*(int16_t *)dp = (int16_t)clr[0];
				dp += 2;
			}
		}	
	} else {
		uc -= ri->ri_font->firstchar;
		fr = (u_char *)ri->ri_font->data + uc * ri->ri_fontscale;

		while (height--) {
			dp = rp;
			fb = fr[3] | (fr[2] << 8) | (fr[1] << 16) | (fr[0] << 24);
			fr += ri->ri_font->stride;
			rp += ri->ri_stride;
		
			for (cnt = width; cnt; cnt--) {
				*(int16_t *)dp = (int16_t)clr[(fb >> 31) & 1];
				fb <<= 1;
				dp += 2;
			}
		}	
	}
	
	/* Do underline */
	if (attr & 1) {
		rp -= ri->ri_stride << 1;

		while (width--) {
			*(int16_t *)rp = clr[1];
			rp += 2;
		}
	}	
}


/*
 * Recompute the (2x2)x1 blitting stamp.
 */
static void
rasops15_makestamp(ri, attr)
	struct rasops_info *ri;
	long attr;
{
	int32_t fg, bg;
	int i;
	
	fg = ri->ri_devcmap[((u_int)attr >> 24) & 15] & 0xffff;
	bg = ri->ri_devcmap[((u_int)attr >> 16) & 15] & 0xffff;
	stamp_attr = attr;
	
	for (i = 0; i < 32; i += 2) {
#if BYTE_ORDER == LITTLE_ENDIAN
		stamp[i] = (i & 16 ? fg : bg);
		stamp[i] |= ((i & 8 ? fg : bg) << 16);
		stamp[i + 1] = (i & 4 ? fg : bg);
		stamp[i + 1] |= ((i & 2 ? fg : bg) << 16);
#else
		stamp[i] = (i & 2 ? fg : bg);
		stamp[i] |= ((i & 4 ? fg : bg) << 16);
		stamp[i + 1] = (i & 8 ? fg : bg);
		stamp[i + 1] |= ((i & 16 ? fg : bg) << 16);
#endif
	}
}


/*
 * Paint a single character. This is for 8-pixel wide fonts.
 */
static void
rasops15_putchar8(cookie, row, col, uc, attr)
	void *cookie;
	int row, col;
	u_int uc;
	long attr;
{
	struct rasops_info *ri;
	int height, so, fs;
	int32_t *rp;
	u_char *fr;
	
	/* Can't risk remaking the stamp if it's already in use */
	if (stamp_mutex++) {
		stamp_mutex--;
		rasops15_putchar(cookie, row, col, uc, attr);
		return;
	}

	ri = (struct rasops_info *)cookie;

#ifdef RASOPS_CLIPPING	
	if ((unsigned)row >= (unsigned)ri->ri_rows) {
		stamp_mutex--;
		return;
	}

	if ((unsigned)col >= (unsigned)ri->ri_cols) {
		stamp_mutex--;
		return;
	}
#endif

	/* Recompute stamp? */
	if (attr != stamp_attr)
		rasops15_makestamp(ri, attr);
	
	rp = (int32_t *)(ri->ri_bits + row*ri->ri_yscale + col*ri->ri_xscale);
	height = ri->ri_font->fontheight;
	
	if (uc == (u_int)-1) {
		while (height--) {
			rp[0] = stamp[0];
			rp[1] = stamp[0];
			rp[2] = stamp[0];
			rp[3] = stamp[0];
			DELTA(rp, ri->ri_stride, int32_t *);
		}	
	} else {
		uc -= ri->ri_font->firstchar;
		fr = (u_char *)ri->ri_font->data + uc*ri->ri_fontscale;
		fs = ri->ri_font->stride;
	
		while (height--) {
			so = STAMP_SHIFT(fr[0], 1) & STAMP_MASK;
			rp[0] = STAMP_READ(so);
			rp[1] = STAMP_READ(so + 4);
			
			so = STAMP_SHIFT(fr[0], 0) & STAMP_MASK;
			rp[2] = STAMP_READ(so);
			rp[3] = STAMP_READ(so + 4);

			fr += fs;
			DELTA(rp, ri->ri_stride, int32_t *);	
		}
	}	

	/* Do underline */
	if (attr & 1) {
		DELTA(rp, -(ri->ri_stride << 1), int32_t *);
		rp[0] = STAMP_READ(30);
		rp[1] = STAMP_READ(30);
		rp[2] = STAMP_READ(30);
		rp[3] = STAMP_READ(30);
	}	
	
	stamp_mutex--;
}


/*
 * Paint a single character. This is for 12-pixel wide fonts.
 */
static void
rasops15_putchar12(cookie, row, col, uc, attr)
	void *cookie;
	int row, col;
	u_int uc;
	long attr;
{
	struct rasops_info *ri;
	int height, so, fs;
	int32_t *rp;
	u_char *fr;
	
	/* Can't risk remaking the stamp if it's already in use */
	if (stamp_mutex++) {
		stamp_mutex--;
		rasops15_putchar(cookie, row, col, uc, attr);
		return;
	}

	ri = (struct rasops_info *)cookie;

#ifdef RASOPS_CLIPPING	
	if ((unsigned)row >= (unsigned)ri->ri_rows) {
		stamp_mutex--;
		return;
	}

	if ((unsigned)col >= (unsigned)ri->ri_cols) {
		stamp_mutex--;
		return;
	}
#endif

	/* Recompute stamp? */
	if (attr != stamp_attr)
		rasops15_makestamp(ri, attr);
	
	rp = (int32_t *)(ri->ri_bits + row*ri->ri_yscale + col*ri->ri_xscale);
	height = ri->ri_font->fontheight;

	if (uc == (u_int)-1) {
		while (height--) {
			rp[0] = stamp[0];
			rp[1] = stamp[0];
			rp[2] = stamp[0];
			rp[3] = stamp[0];
			rp[4] = stamp[0];
			rp[5] = stamp[0];
			DELTA(rp, ri->ri_stride, int32_t *);	
		}	
	} else {
		uc -= ri->ri_font->firstchar;
		fr = (u_char *)ri->ri_font->data + uc*ri->ri_fontscale;
		fs = ri->ri_font->stride;
	
		while (height--) {
			so = STAMP_SHIFT(fr[0], 1) & STAMP_MASK;
			rp[0] = STAMP_READ(so);
			rp[1] = STAMP_READ(so + 4);
			
			so = STAMP_SHIFT(fr[0], 0) & STAMP_MASK;
			rp[2] = STAMP_READ(so);
			rp[3] = STAMP_READ(so + 4);

			so = STAMP_SHIFT(fr[1], 1) & STAMP_MASK;
			rp[4] = STAMP_READ(so);
			rp[5] = STAMP_READ(so + 4);

			fr += fs;
			DELTA(rp, ri->ri_stride, int32_t *);	
		}
	}	

	/* Do underline */
	if (attr & 1) {
		DELTA(rp, -(ri->ri_stride << 1), int32_t *);
		rp[0] = STAMP_READ(30);
		rp[1] = STAMP_READ(30);
		rp[2] = STAMP_READ(30);
		rp[3] = STAMP_READ(30);
		rp[4] = STAMP_READ(30);
		rp[5] = STAMP_READ(30);
	}	
	
	stamp_mutex--;
}


/*
 * Paint a single character. This is for 16-pixel wide fonts.
 */
static void
rasops15_putchar16(cookie, row, col, uc, attr)
	void *cookie;
	int row, col;
	u_int uc;
	long attr;
{
	struct rasops_info *ri;
	int height, so, fs;
	int32_t *rp;
	u_char *fr;
	
	/* Can't risk remaking the stamp if it's already in use */
	if (stamp_mutex++) {
		stamp_mutex--;
		rasops15_putchar(cookie, row, col, uc, attr);
		return;
	}

	ri = (struct rasops_info *)cookie;

#ifdef RASOPS_CLIPPING	
	if ((unsigned)row >= (unsigned)ri->ri_rows) {
		stamp_mutex--;
		return;
	}

	if ((unsigned)col >= (unsigned)ri->ri_cols) {
		stamp_mutex--;
		return;
	}
#endif

	/* Recompute stamp? */
	if (attr != stamp_attr)
		rasops15_makestamp(ri, attr);
	
	rp = (int32_t *)(ri->ri_bits + row*ri->ri_yscale + col*ri->ri_xscale);
	height = ri->ri_font->fontheight;
	
	if (uc == (u_int)-1) {
		while (height--) {
			rp[0] = stamp[0];
			rp[1] = stamp[0];
			rp[2] = stamp[0];
			rp[3] = stamp[0];
			rp[4] = stamp[0];
			rp[5] = stamp[0];
			rp[6] = stamp[0];
			rp[7] = stamp[0];
			DELTA(rp, ri->ri_stride, int32_t *);	
		}	
	} else {
		uc -= ri->ri_font->firstchar;
		fr = (u_char *)ri->ri_font->data + uc*ri->ri_fontscale;
		fs = ri->ri_font->stride;
	
		while (height--) {
			so = STAMP_SHIFT(fr[0], 1) & STAMP_MASK;
			rp[0] = STAMP_READ(so);
			rp[1] = STAMP_READ(so + 4);
			
			so = STAMP_SHIFT(fr[0], 0) & STAMP_MASK;
			rp[2] = STAMP_READ(so);
			rp[3] = STAMP_READ(so + 4);

			so = STAMP_SHIFT(fr[1], 1) & STAMP_MASK;
			rp[4] = STAMP_READ(so);
			rp[5] = STAMP_READ(so + 4);
			
			so = STAMP_SHIFT(fr[1], 0) & STAMP_MASK;
			rp[6] = STAMP_READ(so);
			rp[7] = STAMP_READ(so + 4);

			DELTA(rp, ri->ri_stride, int32_t *);	
			fr += fs;
		}
	}	

	/* Do underline */
	if (attr & 1) {
		DELTA(rp, -(ri->ri_stride << 1), int32_t *);
		rp[0] = STAMP_READ(30);
		rp[1] = STAMP_READ(30);
		rp[2] = STAMP_READ(30);
		rp[3] = STAMP_READ(30);
		rp[4] = STAMP_READ(30);
		rp[5] = STAMP_READ(30);
		rp[6] = STAMP_READ(30);
		rp[7] = STAMP_READ(30);
	}	
	
	stamp_mutex--;
}
