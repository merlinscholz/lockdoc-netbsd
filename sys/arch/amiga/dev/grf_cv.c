/*	$NetBSD: grf_cv.c,v 1.9 1996/03/06 16:40:16 is Exp $	*/

/*
 * Copyright (c) 1995 Michael Teske
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
 *      This product includes software developed by Ezra Story, by Kari
 *      Mettinen and by Bernd Ernesti.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
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
 */
#include "grfcv.h"
#if NGRFCV > 0

/*
 * Graphics routines for the CyberVision 64 board, using the S3 Trio64.
 *
 * Modified for CV64 from
 * Kari Mettinen's Cirrus driver by Michael Teske 10/95
 * For questions mail me at teske@dice2.desy.de
 *
 * Thanks to Tekelec Airtronic for providing me with a S3 Trio64 documentation.
 * Thanks to Bernd 'the fabulous bug-finder' Ernesti for bringing my messy
 * source to NetBSD style :)
 *
 * TODO:
 *    Hardware Cursor support
 */

#include <sys/param.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <sys/device.h>
#include <sys/malloc.h>
#include <sys/systm.h>
#include <machine/cpu.h>
#include <dev/cons.h>
#include <amiga/dev/itevar.h>
#include <amiga/amiga/device.h>
#include <amiga/dev/grfioctl.h>
#include <amiga/dev/grfvar.h>
#include <amiga/dev/grf_cvreg.h>
#include <amiga/dev/zbusvar.h>

int	grfcvmatch  __P((struct device *, struct cfdata *, void *));
void	grfcvattach __P((struct device *, struct device *, void *));
int	grfcvprint  __P((void *, char *));

static int cv_has_4mb __P((volatile caddr_t));
static unsigned short compute_clock __P((unsigned long));
void	cv_boardinit __P((struct grf_softc *));
int	cv_getvmode __P((struct grf_softc *, struct grfvideo_mode *));
int	cv_setvmode __P((struct grf_softc *, unsigned int));
int	cv_blank __P((struct grf_softc *, int *));
int	cv_mode __P((register struct grf_softc *, int, void *, int, int));
int	cv_ioctl __P((register struct grf_softc *gp, int cmd, void *data));
int	cv_setmonitor __P((struct grf_softc *, struct grfvideo_mode *));
int	cv_getcmap __P((struct grf_softc *, struct grf_colormap *));
int	cv_putcmap __P((struct grf_softc *, struct grf_colormap *));
int	cv_toggle __P((struct grf_softc *));
int	cv_mondefok __P((struct grfvideo_mode *));
int	cv_load_mon __P((struct grf_softc *, struct grfcvtext_mode *));
void	cv_inittextmode __P((struct grf_softc *));
void	cv_memset __P((unsigned char *, unsigned char, int));
static	inline void cv_write_port __P((unsigned short, volatile caddr_t));
static	inline void cvscreen __P((int, volatile caddr_t));
static	inline void gfx_on_off __P((int, volatile caddr_t));

/* Graphics display definitions.
 * These are filled by 'grfconfig' using GRFIOCSETMON.
 */
#define monitor_def_max 24
static struct grfvideo_mode monitor_def[24] = {
	{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
	{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
	{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}
};
static struct grfvideo_mode *monitor_current = &monitor_def[0];
#define MAXPIXELCLOCK 135000000 /* safety */


/* Console display definition.
 *   Default hardcoded text mode.  This grf_cv is set up to
 *   use one text mode only, and this is it.  You may use
 *   grfconfig to change the mode after boot.
 */

/* Console font */
#ifdef KFONT_8X11
#define S3FONT kernel_font_8x11
#define S3FONTY 11
#else
#define S3FONT kernel_font_8x8
#define S3FONTY 8
#endif
extern unsigned char S3FONT[];

/*
 * Define default console mode
 * (Internally, we still have to use hvalues/8!)
 */
struct grfcvtext_mode cvconsole_mode = {
	{255, "", 25000000, 640, 480, 4, 640/8, 784/8, 680/8, 768/8, 800/8,
	 481, 521, 491, 493, 525},
	8, S3FONTY, 80, 480 / S3FONTY, S3FONT, 32, 255
};

/* Console colors */
unsigned char cvconscolors[4][3] = {	/* background, foreground, hilite */
	{0x30, 0x30, 0x30}, {0, 0, 0}, {0x18, 0x20, 0x34}, {0x2a, 0x2a, 0x2a}
};

static unsigned char clocks[]={
0x13, 0x61, 0x6b, 0x6d, 0x51, 0x69, 0x54, 0x69,
0x4f, 0x68, 0x6b, 0x6b, 0x18, 0x61, 0x7b, 0x6c,
0x51, 0x67, 0x24, 0x62, 0x56, 0x67, 0x77, 0x6a,
0x1d, 0x61, 0x53, 0x66, 0x6b, 0x68, 0x79, 0x69,
0x7c, 0x69, 0x7f, 0x69, 0x22, 0x61, 0x54, 0x65,
0x56, 0x65, 0x58, 0x65, 0x67, 0x66, 0x41, 0x63,
0x27, 0x61, 0x13, 0x41, 0x37, 0x62, 0x6b, 0x4d,
0x23, 0x43, 0x51, 0x49, 0x79, 0x66, 0x54, 0x49,
0x7d, 0x66, 0x34, 0x56, 0x4f, 0x63, 0x1f, 0x42,
0x6b, 0x4b, 0x7e, 0x4d, 0x18, 0x41, 0x2a, 0x43,
0x7b, 0x4c, 0x74, 0x4b, 0x51, 0x47, 0x65, 0x49,
0x24, 0x42, 0x68, 0x49, 0x56, 0x47, 0x75, 0x4a,
0x77, 0x4a, 0x31, 0x43, 0x1d, 0x41, 0x71, 0x49,
0x53, 0x46, 0x29, 0x42, 0x6b, 0x48, 0x1f, 0x41,
0x79, 0x49, 0x6f, 0x48, 0x7c, 0x49, 0x38, 0x43,
0x7f, 0x49, 0x5d, 0x46, 0x22, 0x41, 0x53, 0x45,
0x54, 0x45, 0x55, 0x45, 0x56, 0x45, 0x57, 0x45,
0x58, 0x45, 0x25, 0x41, 0x67, 0x46, 0x5b, 0x45,
0x41, 0x43, 0x78, 0x47, 0x27, 0x41, 0x51, 0x44,
0x13, 0x21, 0x7d, 0x47, 0x37, 0x42, 0x71, 0x46,
0x6b, 0x2d, 0x14, 0x21, 0x23, 0x23, 0x7d, 0x2f,
0x51, 0x29, 0x61, 0x2b, 0x79, 0x46, 0x1d, 0x22,
0x54, 0x29, 0x45, 0x27, 0x7d, 0x46, 0x7f, 0x46,
0x4f, 0x43, 0x2f, 0x41, 0x1f, 0x22, 0x6a, 0x2b,
0x6b, 0x2b, 0x5b, 0x29, 0x7e, 0x2d, 0x65, 0x44,
0x18, 0x21, 0x5e, 0x29, 0x2a, 0x23, 0x45, 0x26,
0x7b, 0x2c, 0x19, 0x21, 0x74, 0x2b, 0x75, 0x2b,
0x51, 0x27, 0x3f, 0x25, 0x65, 0x29, 0x40, 0x25,
0x24, 0x22, 0x41, 0x25, 0x68, 0x29, 0x42, 0x25,
0x56, 0x27, 0x7e, 0x2b, 0x75, 0x2a, 0x1c, 0x21,
0x77, 0x2a, 0x4f, 0x26, 0x31, 0x23, 0x6f, 0x29,
0x1d, 0x21, 0x32, 0x23, 0x71, 0x29, 0x72, 0x29,
0x53, 0x26, 0x69, 0x28, 0x29, 0x22, 0x75, 0x29,
0x6b, 0x28, 0x1f, 0x21, 0x1f, 0x21, 0x6d, 0x28,
0x79, 0x29, 0x2b, 0x22, 0x6f, 0x28, 0x59, 0x26,
0x7c, 0x29, 0x7d, 0x29, 0x38, 0x23, 0x21, 0x21,
0x7f, 0x29, 0x39, 0x23, 0x5d, 0x26, 0x75, 0x28,
0x22, 0x21, 0x77, 0x28, 0x53, 0x25, 0x6c, 0x27,
0x54, 0x25, 0x61, 0x26, 0x55, 0x25, 0x30, 0x22,
0x56, 0x25, 0x63, 0x26, 0x57, 0x25, 0x71, 0x27,
0x58, 0x25, 0x7f, 0x28, 0x25, 0x21, 0x74, 0x27,
0x67, 0x26, 0x40, 0x23, 0x5b, 0x25, 0x26, 0x21,
0x41, 0x23, 0x34, 0x22, 0x78, 0x27, 0x6b, 0x26,
0x27, 0x21, 0x35, 0x22, 0x51, 0x24, 0x7b, 0x27,
0x13, 0x1,  0x13, 0x1,  0x7d, 0x27, 0x4c, 0x9,
0x37, 0x22, 0x5b, 0xb,  0x71, 0x26, 0x5c, 0xb,
0x6b, 0xd,  0x47, 0x23, 0x14, 0x1,  0x4f, 0x9,
0x23, 0x3,  0x75, 0x26, 0x7d, 0xf,  0x1c, 0x2,
0x51, 0x9,  0x59, 0x24, 0x61, 0xb,  0x69, 0x25,
0x79, 0x26, 0x34, 0x5,  0x1d, 0x2,  0x6b, 0x25,
0x54, 0x9,  0x35, 0x5,  0x45, 0x7,  0x6d, 0x25,
0x7d, 0x26, 0x16, 0x1,  0x7f, 0x26, 0x77, 0xd,
0x4f, 0x23, 0x78, 0xd,  0x2f, 0x21, 0x27, 0x3,
0x1f, 0x2,  0x59, 0x9,  0x6a, 0xb,  0x73, 0x25,
0x6b, 0xb,  0x63, 0x24, 0x5b, 0x9,  0x20, 0x2,
0x7e, 0xd,  0x4b, 0x7,  0x65, 0x24, 0x43, 0x22,
0x18, 0x1,  0x6f, 0xb,  0x5e, 0x9,  0x70, 0xb,
0x2a, 0x3,  0x33, 0x4,  0x45, 0x6,  0x60, 0x9,
0x7b, 0xc,  0x19, 0x1,  0x19, 0x1,  0x7d, 0xc,
0x74, 0xb,  0x50, 0x7,  0x75, 0xb,  0x63, 0x9,
0x51, 0x7,  0x23, 0x2,  0x3f, 0x5,  0x1a, 0x1,
0x65, 0x9,  0x2d, 0x3,  0x40, 0x5,  0x0,  0x0,
};


/* Board Address of CV64 */
static volatile caddr_t cv_boardaddr;
static int cv_fbsize;

/*
 * Memory clock (binpatchable).
 * Let's be defensive: 45 MHz runs on all boards I know of.
 * 55 MHz runs on most boards. But you should know what you're doing
 * if you set this flag. Again: This flag may destroy your CV Board.
 * Use it at your own risk!!!
 * Anyway, this doesn't imply that I'm responsible if your board breaks
 * without setting this flag :-).
 */
#ifdef CV_AGGRESSIVE_TIMING
long cv_memclk = 55000000;
#else
long cv_memclk = 45000000;
#endif

/* standard driver stuff */
struct cfdriver grfcvcd = {
	NULL, "grfcv", (cfmatch_t)grfcvmatch, grfcvattach,
	DV_DULL, sizeof(struct grf_softc), NULL, 0
};
static struct cfdata *cfdata;


/*
 * Get frambuffer memory size.
 * phase5 didn't provide the bit in CR36,
 * so we have to do it this way.
 * Return 0 for 2MB, 1 for 4MB
 */
static int
cv_has_4mb(fb)
	volatile caddr_t fb;
{
	volatile unsigned long *testfbw, *testfbr;

	/* write patterns in memory and test if they can be read */
	testfbw = (volatile unsigned long *)fb;
	testfbr = (volatile unsigned long *)(fb + 0x02000000);
	*testfbw = 0x87654321;
	if (*testfbr != 0x87654321)
		return (0);

	/* upper memory region */
	testfbw = (volatile unsigned long *)(fb + 0x00200000);
	testfbr = (volatile unsigned long *)(fb + 0x02200000);
	*testfbw = 0x87654321;
	if (*testfbr != 0x87654321)
		return (0);
	*testfbw = 0xAAAAAAAA;
	if (*testfbr != 0xAAAAAAAA)
		return (0);
	*testfbw = 0x55555555;
	if (*testfbr != 0x55555555)
		return (0);
	return (1);
}

int
grfcvmatch(pdp, cfp, auxp)
	struct device *pdp;
	struct cfdata *cfp;
	void *auxp;
{
	struct zbus_args *zap;
	static int cvcons_unit = -1;

	zap = auxp;

	if (amiga_realconfig == 0)
#ifdef CV64CONSOLE
		if (cvcons_unit != -1)
#endif
			 return (0);

	/* Lets be Paranoid: Test man and prod id */
	if (zap->manid != 8512 || zap->prodid != 34)
		return (0);

	cv_boardaddr = zap->va;

#ifdef CV64CONSOLE
	if (amiga_realconfig == 0) {
		cvcons_unit = cfp->cf_unit;
		cfdata = cfp;
	}
#endif

	return (1);
}

void
grfcvattach(pdp, dp, auxp)
	struct device *pdp, *dp;
	void *auxp;
{
	static struct grf_softc congrf;
	struct zbus_args *zap;
	struct grf_softc *gp;
	static char attachflag = 0; 

	zap = auxp;

	/* 
	 * This function is called twice, once on console init (dp == NULL)
	 * and once on "normal" grf5 init. 
	 */

	if (dp == NULL) /* console init */
		gp = &congrf;
	else
		gp = (struct grf_softc *)dp;

	if (dp != NULL && congrf.g_regkva != 0) {
		/*
		 * inited earlier, just copy (not device struct)
		 */

		printf("\n");
#ifdef CV64CONSOLE
		bcopy(&congrf.g_display, &gp->g_display,
			(char *) &gp[1] - (char *) &gp->g_display);
#else
		gp->g_regkva = (volatile caddr_t)cv_boardaddr + 0x02000000;
		gp->g_fbkva = (volatile caddr_t)cv_boardaddr + 0x01400000;

		gp->g_unit = GRF_CV64_UNIT;
		gp->g_mode = cv_mode;
		gp->g_conpri = grfcv_cnprobe();
		gp->g_flags = GF_ALIVE;
#endif
	} else {
		gp->g_regkva = (volatile caddr_t)cv_boardaddr + 0x02000000;
		gp->g_fbkva = (volatile caddr_t)cv_boardaddr + 0x01400000;

		gp->g_unit = GRF_CV64_UNIT;
		gp->g_mode = cv_mode;
		gp->g_conpri = grfcv_cnprobe();
		gp->g_flags = GF_ALIVE;

		/* wakeup the board */
		cv_boardinit(gp);

#ifdef CV64CONSOLE
		grfcv_iteinit(gp);
		(void)cv_load_mon(gp, &cvconsole_mode);
#endif
	}

	/*
	 * attach grf
	 */
	if (amiga_config_found(cfdata, &gp->g_device, gp, grfcvprint)) {
		if (dp != NULL)
			printf("grfcv: CyberVision64 with %dMB being used\n", cv_fbsize/0x100000);
		attachflag = 1;
	} else {
		if (!attachflag)
			/*printf("grfcv unattached!!\n")*/;
	}
}

int
grfcvprint(auxp, pnp)
	void *auxp;
	char *pnp;
{
	if (pnp)
		printf("ite at %s: ", pnp);
	return (UNCONF);
}


/*
 * Computes M, N, and R values from
 * given input frequency. It uses a table of
 * precomputed values, to keep CPU time low.
 *
 * The return value consist of:
 * lower byte:  Bits 4-0: N Divider Value
 *	        Bits 5-6: R Value          for e.g. SR10 or SR12
 * higher byte: Bits 0-6: M divider value  for e.g. SR11 or SR13
 */

static unsigned short
compute_clock(freq)
	unsigned long freq;
{
	static unsigned char *mnr, *save;	/* M, N + R vals */
	unsigned long work_freq, r;
	unsigned short erg;
	long diff, d2;

	if (freq < 12500000 || freq > MAXPIXELCLOCK) {
		printf("grfcv: Illegal clock frequency: %ldMHz\n", freq/1000000);
		printf("grfcv: Using default frequency: 25MHz\n");
		printf("grfcv: See the manpage of grfconfig for more informations.\n");
		freq = 25000000;
	}

	mnr = clocks;	/* there the vals are stored */
	d2 = 0x7fffffff;

	while (*mnr) {	/* mnr vals are 0-terminated */
		work_freq = (0x37EE * (mnr[0] + 2)) / ((mnr[1] & 0x1F) + 2);

		r = (mnr[1] >> 5) & 0x03;
		if (r != 0)
			work_freq=work_freq >> r;	/* r is the freq divider */

		work_freq *= 0x3E8;	/* 2nd part of OSC */

		diff = abs(freq - work_freq);

		if (d2 >= diff) {
			d2 = diff;
			/* In save are the vals for minimal diff */
			save = mnr;
		}
		mnr += 2;
	}
	erg = *((unsigned short *)save);

	return (erg);
}


void
cv_boardinit(gp)
	struct grf_softc *gp;
{
	volatile caddr_t ba;
	unsigned char test;
	unsigned int clockpar;
	int i;
	struct grfinfo *gi;

	ba = gp->g_regkva;
	/* Reset board */
	for (i = 0; i < 6; i++)
		cv_write_port (0xff, ba - 0x02000000);	/* Clear all bits */

	/* Return to operational Mode */
	cv_write_port(0x8004, ba - 0x02000000);

	/* Wakeup Chip */
	vgaw(ba, SREG_VIDEO_SUBS_ENABLE, 0x10);
	vgaw(ba, SREG_OPTION_SELECT, 0x1);
	vgaw(ba, SREG_VIDEO_SUBS_ENABLE, 0x8);

	vgaw(ba, GREG_MISC_OUTPUT_W, 0x23);

	WCrt(ba, CRT_ID_REGISTER_LOCK_1, 0x48);	/* unlock S3 VGA regs */
	WCrt(ba, CRT_ID_REGISTER_LOCK_2, 0xA5);	/* unlock syscontrol */

	test = RCrt(ba, CRT_ID_SYSTEM_CONFIG);
	test = test | 0x01;	/* enable enhaced register access */
	test = test & 0xEF;	/* clear bit 4, 0 wait state */
	WCrt(ba, CRT_ID_SYSTEM_CONFIG, test);

	/*
	 * bit 1=1: enable enhanced mode functions
	 * bit 4=1: enable linear adressing
	 * bit 5=1: enable MMIO
 	 */
	vgaw(ba, ECR_ADV_FUNC_CNTL, 0x31);

	/* enable cpu acess, color mode, high 64k page */
	vgaw(ba, GREG_MISC_OUTPUT_W, 0x23);

	/* Cpu base addr */
	WCrt(ba, CRT_ID_EXT_SYS_CNTL_4, 0x0);

	/* Reset. This does nothing, but everyone does it:) */
	WSeq(ba, SEQ_ID_RESET, 0x3);

	WSeq(ba, SEQ_ID_CLOCKING_MODE, 0x1);	/* 8 Dot Clock */
	WSeq(ba, SEQ_ID_MAP_MASK, 0xF);		/* Enable write planes */
	WSeq(ba, SEQ_ID_CHAR_MAP_SELECT, 0x0);	/* Character Font */

	WSeq(ba, SEQ_ID_MEMORY_MODE, 0x2);	/* Complete mem access */

	WSeq(ba, SEQ_ID_UNLOCK_EXT, 0x6);	/* Unlock extensions */
	test = RSeq(ba, SEQ_ID_BUS_REQ_CNTL);	/* Bus Request */

	/* enable 4MB fast Page Mode */
	test = test | 1 << 6;
	WSeq(ba, SEQ_ID_BUS_REQ_CNTL, test);
	/* faster LUT write */
	WSeq(ba, SEQ_ID_RAMDAC_CNTL, 0xC0);

	test = RSeq(ba, SEQ_ID_CLKSYN_CNTL_2);	/* Clksyn2 read */

	/* immediately Clkload bit clear */
	test = test & 0xDF;
	
	/* 2 MCLK Memory Write.... */
	if (cv_memclk >= 55000000)
		test |= 0x80;

	WSeq(ba, SEQ_ID_CLKSYN_CNTL_2, test);

	/* Memory CLK */
	clockpar = compute_clock(cv_memclk);
	test = (clockpar & 0xFF00) >> 8;
	WSeq(ba, SEQ_ID_MCLK_HI, test);		/* PLL N-Divider Value */
	if (RCrt(ba, CRT_ID_REVISION) == 0x10)	/* bugfix for new S3 chips */
		WSeq(ba, SEQ_ID_MORE_MAGIC, test);

	test = clockpar & 0xFF;
	WSeq(ba, SEQ_ID_MCLK_LO, test);		/* PLL M-Divider Value */

	/* We now load an 25 MHz, 31 kHz, 640x480 standard VGA Mode. */
	/* DCLK */
	WSeq(ba, SEQ_ID_DCLK_HI, 0x13);
	WSeq(ba, SEQ_ID_DCLK_LO, 0x41);

	test = RSeq (ba, SEQ_ID_CLKSYN_CNTL_2);
	test = test | 0x22;

	/* DCLK + MCLK Clock immediate load! */
	WSeq(ba,SEQ_ID_CLKSYN_CNTL_2, test);

	/* DCLK load */
	test = vgar(ba, 0x3cc);
	test = test | 0x0c;
	vgaw(ba, 0x3c2, test);

	/* Clear bit 5 again, prevent further loading. */
	WSeq(ba, SEQ_ID_CLKSYN_CNTL_2, 0x2);

	WCrt(ba, CRT_ID_HOR_TOTAL, 0x5F);
	WCrt(ba, CRT_ID_HOR_DISP_ENA_END, 0x4F);
	WCrt(ba, CRT_ID_START_HOR_BLANK, 0x50);
	WCrt(ba, CRT_ID_END_HOR_BLANK, 0x82);
	WCrt(ba, CRT_ID_START_HOR_RETR, 0x54);
	WCrt(ba, CRT_ID_END_HOR_RETR, 0x80);
	WCrt(ba, CRT_ID_VER_TOTAL, 0xBF);

	WCrt(ba, CRT_ID_OVERFLOW, 0x1F);	/* overflow reg */

	WCrt(ba, CRT_ID_PRESET_ROW_SCAN, 0x0);	/* no panning */

	WCrt(ba, CRT_ID_MAX_SCAN_LINE, 0x40);	/* vscan */

	WCrt(ba, CRT_ID_CURSOR_START, 0x00);
	WCrt(ba, CRT_ID_CURSOR_END, 0x00);

	/* Display start adress */
	WCrt(ba, CRT_ID_START_ADDR_HIGH, 0x00);
	WCrt(ba, CRT_ID_START_ADDR_LOW, 0x00);

	/* Cursor location */
	WCrt(ba, CRT_ID_CURSOR_LOC_HIGH, 0x00);
	WCrt(ba, CRT_ID_CURSOR_LOC_LOW, 0x00);

	/* Vertical retrace */
	WCrt(ba, CRT_ID_START_VER_RETR, 0x9C);
	WCrt(ba, CRT_ID_END_VER_RETR, 0x0E);

	WCrt(ba, CRT_ID_VER_DISP_ENA_END, 0x8F);
	WCrt(ba, CRT_ID_SCREEN_OFFSET, 0x50);

	WCrt(ba, CRT_ID_UNDERLINE_LOC, 0x00);

	WCrt(ba, CRT_ID_START_VER_BLANK, 0x96);
	WCrt(ba, CRT_ID_END_VER_BLANK, 0xB9);

	WCrt(ba, CRT_ID_MODE_CONTROL, 0xE3);

	WCrt(ba, CRT_ID_LINE_COMPARE, 0xFF);

	WCrt(ba, CRT_ID_BACKWAD_COMP_3, 0x10);	/* FIFO enabled */

	/* Refresh count 1, High speed text font, enhanced color mode */
	WCrt(ba, CRT_ID_MISC_1, 0x35);

	/* start fifo position */
	WCrt(ba, CRT_ID_DISPLAY_FIFO, 0x5a);

	WCrt(ba, CRT_ID_EXT_MEM_CNTL_2, 0x70);

	/* address window position */
	WCrt(ba, CRT_ID_LAW_POS_LO, 0x40);

	/* N Parameter for Display FIFO */
	WCrt(ba, CRT_ID_EXT_MEM_CNTL_3, 0xFF);

	WGfx(ba, GCT_ID_SET_RESET, 0x0);
	WGfx(ba, GCT_ID_ENABLE_SET_RESET, 0x0);
	WGfx(ba, GCT_ID_COLOR_COMPARE, 0x0);
	WGfx(ba, GCT_ID_DATA_ROTATE, 0x0);
	WGfx(ba, GCT_ID_READ_MAP_SELECT, 0x0);
	WGfx(ba, GCT_ID_GRAPHICS_MODE, 0x40);
	WGfx(ba, GCT_ID_MISC, 0x01);
	WGfx(ba, GCT_ID_COLOR_XCARE, 0x0F);
	WGfx(ba, GCT_ID_BITMASK, 0xFF);

	/* colors for text mode */
	for (i = 0; i <= 0xf; i++)
		WAttr (ba, i, i);

	WAttr(ba, ACT_ID_ATTR_MODE_CNTL, 0x41);
	WAttr(ba, ACT_ID_OVERSCAN_COLOR, 0x01);
	WAttr(ba, ACT_ID_COLOR_PLANE_ENA, 0x0F);
	WAttr(ba, ACT_ID_HOR_PEL_PANNING, 0x0);
	WAttr(ba, ACT_ID_COLOR_SELECT, 0x0);

	vgaw(ba, VDAC_MASK, 0xFF);	/* DAC Mask */

	*((unsigned long *)(ba + ECR_FRGD_COLOR)) = 0xFF;
	*((unsigned long *)(ba + ECR_BKGD_COLOR)) = 0;

	/* colors initially set to greyscale */

	vgaw(ba, VDAC_ADDRESS_W, 0);
	for (i = 255; i >= 0 ; i--) {
		vgaw(ba, VDAC_DATA, i);
		vgaw(ba, VDAC_DATA, i);
		vgaw(ba, VDAC_DATA, i);
	}

	/* GFx hardware cursor off */
	WCrt(ba, CRT_ID_HWGC_MODE, 0x00);

	/* Set first to 4 MB, so test will work */
	WCrt(ba, CRT_ID_LAW_CNTL, 0x13);

	/* find *correct* fbsize of z3 board */
	if (cv_has_4mb((volatile caddr_t)cv_boardaddr + 0x01400000)) {
		cv_fbsize = 1024 * 1024 * 4;
		WCrt(ba, CRT_ID_LAW_CNTL, 0x13); /* 4 MB */
	} else {
		cv_fbsize = 1024 * 1024 * 2;
		WCrt(ba, CRT_ID_LAW_CNTL, 0x12); /* 2 MB */
	}

	/* Enable Video Display (Set Bit 5) */
	WAttr(ba, 0x33, 0);

	gi = &gp->g_display;
	gi->gd_regaddr	= (caddr_t) kvtop (ba);
	gi->gd_regsize	= 64 * 1024;
	gi->gd_fbaddr	= (caddr_t) kvtop (gp->g_fbkva);
	gi->gd_fbsize	= cv_fbsize;
}


int
cv_getvmode(gp, vm)
	struct grf_softc *gp;
	struct grfvideo_mode *vm;
{
	struct grfvideo_mode *gv;

#ifdef CV64CONSOLE
	/* Handle grabbing console mode */
	if (vm->mode_num == 255) {
		bcopy(&cvconsole_mode, vm, sizeof(struct grfvideo_mode));
		/* XXX so grfconfig can tell us the correct text dimensions. */
		vm->depth = cvconsole_mode.fy;
	} else
#endif
	{
		if (vm->mode_num == 0)
			vm->mode_num = (monitor_current - monitor_def) + 1;
		if (vm->mode_num < 1 || vm->mode_num > monitor_def_max)
			return (EINVAL);
		gv = monitor_def + (vm->mode_num - 1);
		if (gv->mode_num == 0)
			return (EINVAL);

		bcopy(gv, vm, sizeof(struct grfvideo_mode));
	}

	/* adjust internal values to pixel values */

	vm->hblank_start *= 8;
	vm->hblank_stop *= 8;
	vm->hsync_start *= 8;
	vm->hsync_stop *= 8;
	vm->htotal *= 8;

	return (0);
}


int
cv_setvmode(gp, mode)
	struct grf_softc *gp;
	unsigned mode;
{

	if (!mode || (mode > monitor_def_max) ||
	    monitor_def[mode - 1].mode_num == 0)
		return (EINVAL);

	monitor_current = monitor_def + (mode - 1);

	return (0);
}


int
cv_blank(gp, on)
	struct grf_softc *gp;
	int *on;
{
	volatile caddr_t ba;

	ba = gp->g_regkva;
	gfx_on_off(*on ? 0 : 1, ba);
	return (0);
}


/*
 * Change the mode of the display.
 * Return a UNIX error number or 0 for success.
 */
int
cv_mode(gp, cmd, arg, a2, a3)
	register struct grf_softc *gp;
	int cmd;
	void *arg;
	int a2, a3;
{
	int error;

	switch (cmd) {
	case GM_GRFON:
		error = cv_load_mon (gp,
		    (struct grfcvtext_mode *) monitor_current) ? 0 : EINVAL;
		return (error);

	case GM_GRFOFF:
#ifndef CV64CONSOLE
		(void)cv_toggle(gp);
#else
		cv_load_mon(gp, &cvconsole_mode);
		ite_reinit(gp->g_itedev);
#endif
		return (0);

	case GM_GRFCONFIG:
		return (0);

	case GM_GRFGETVMODE:
		return (cv_getvmode (gp, (struct grfvideo_mode *) arg));

	case GM_GRFSETVMODE:
		error = cv_setvmode (gp, *(unsigned *) arg);
		if (!error && (gp->g_flags & GF_GRFON))
			cv_load_mon(gp,
			    (struct grfcvtext_mode *) monitor_current);
		return (error);

	case GM_GRFGETNUMVM:
		*(int *)arg = monitor_def_max;
		return (0);

	case GM_GRFIOCTL:
		return (cv_ioctl (gp, (int) arg, (caddr_t) a2));

	default:
		break;
	}

	return (EINVAL);
}

int
cv_ioctl (gp, cmd, data)
	register struct grf_softc *gp;
	int cmd;
	void *data;
{
	switch (cmd) {
	case GRFIOCGSPRITEPOS:
	case GRFIOCSSPRITEPOS:
	case GRFIOCSSPRITEINF:
	case GRFIOCGSPRITEINF:
	case GRFIOCGSPRITEMAX:
		break;

	case GRFIOCGETCMAP:
		return (cv_getcmap (gp, (struct grf_colormap *) data));

	case GRFIOCPUTCMAP:
		return (cv_putcmap (gp, (struct grf_colormap *) data));

	case GRFIOCBITBLT:
		break;

	case GRFTOGGLE:
		return (cv_toggle (gp));

	case GRFIOCSETMON:
		return (cv_setmonitor (gp, (struct grfvideo_mode *)data));

	case GRFIOCBLANK:
		return (cv_blank (gp, (int *)data));
	}
	return (EINVAL);
}

int
cv_setmonitor(gp, gv)
	struct grf_softc *gp;
	struct grfvideo_mode *gv;
{
	struct grfvideo_mode *md;

	if (!cv_mondefok(gv))
		return (EINVAL);

#ifdef CV64CONSOLE
	/* handle interactive setting of console mode */
	if (gv->mode_num == 255) {
		bcopy(gv, &cvconsole_mode.gv, sizeof(struct grfvideo_mode));
		cvconsole_mode.gv.hblank_start /= 8;
		cvconsole_mode.gv.hblank_stop /= 8;
		cvconsole_mode.gv.hsync_start /= 8;
		cvconsole_mode.gv.hsync_stop /= 8;
		cvconsole_mode.gv.htotal /= 8;
		cvconsole_mode.rows = gv->disp_height / cvconsole_mode.fy;
		cvconsole_mode.cols = gv->disp_width / cvconsole_mode.fx;
		if (!(gp->g_flags & GF_GRFON))
			cv_load_mon(gp, &cvconsole_mode);
		ite_reinit(gp->g_itedev);
		return (0);
	}
#endif

	md = monitor_def + (gv->mode_num - 1);
	bcopy(gv, md, sizeof(struct grfvideo_mode));

	/* adjust pixel oriented values to internal rep. */

	md->hblank_start /= 8;
	md->hblank_stop /= 8;
	md->hsync_start /= 8;
	md->hsync_stop /= 8;
	md->htotal /= 8;

	return (0);
}

int
cv_getcmap(gfp, cmap)
	struct grf_softc *gfp;
	struct grf_colormap *cmap;
{
	volatile caddr_t ba;
	u_char red[256], green[256], blue[256], *rp, *gp, *bp;
	short x;
	int error;

	ba = gfp->g_regkva;
	if (cmap->count == 0 || cmap->index >= 256)
		return (0);

	if (cmap->index + cmap->count > 256)
		cmap->count = 256 - cmap->index;

	/* first read colors out of the chip, then copyout to userspace */
	vgaw (ba, VDAC_ADDRESS_W, cmap->index);
	x = cmap->count - 1;

	rp = red + cmap->index;
	gp = green + cmap->index;
	bp = blue + cmap->index;

	do {
		*rp++ = vgar (ba, VDAC_DATA) << 2;
		*gp++ = vgar (ba, VDAC_DATA) << 2;
		*bp++ = vgar (ba, VDAC_DATA) << 2;
	} while (x-- > 0);

	if (!(error = copyout (red + cmap->index, cmap->red, cmap->count))
	    && !(error = copyout (green + cmap->index, cmap->green, cmap->count))
	    && !(error = copyout (blue + cmap->index, cmap->blue, cmap->count)))
		return (0);

	return (error);
}

int
cv_putcmap(gfp, cmap)
	struct grf_softc *gfp;
	struct grf_colormap *cmap;
{
	volatile caddr_t ba;
	u_char red[256], green[256], blue[256], *rp, *gp, *bp;
	short x;
	int error;

	ba = gfp->g_regkva;
	if (cmap->count == 0 || cmap->index >= 256)
		return (0);

	if (cmap->index + cmap->count > 256)
		cmap->count = 256 - cmap->index;

	/* first copy the colors into kernelspace */
	if (!(error = copyin (cmap->red, red + cmap->index, cmap->count))
	    && !(error = copyin (cmap->green, green + cmap->index, cmap->count))
	    && !(error = copyin (cmap->blue, blue + cmap->index, cmap->count))) {
		vgaw (ba, VDAC_ADDRESS_W, cmap->index);
		x = cmap->count - 1;

		rp = red + cmap->index;
		gp = green + cmap->index;
		bp = blue + cmap->index;

		do {
			vgaw (ba, VDAC_DATA, *rp++ >> 2);
			vgaw (ba, VDAC_DATA, *gp++ >> 2);
			vgaw (ba, VDAC_DATA, *bp++ >> 2);
		} while (x-- > 0);
		return (0);
	} else
		return (error);
}


int
cv_toggle(gp)
	struct grf_softc *gp;
{
	volatile caddr_t ba;

	ba = gp->g_regkva;
	cvscreen(1, ba - 0x02000000);

	return (0);
}


int
cv_mondefok(gv)
	struct grfvideo_mode *gv;
{
	unsigned long maxpix;
	int widthok = 0;

	if (gv->mode_num < 1 || gv->mode_num > monitor_def_max) {
		if (gv->mode_num != 255 || (gv->depth != 4 && gv->depth != 8))
			return (0);
		else
			/*
			 * We have 8 bit console modes. This _is_
			 * a hack but necessary to be compatible.
			 */
			gv->depth = 8;
	}

	switch(gv->depth) {
	   case 1:
	   case 4:
		return (0);
	   case 8:
		maxpix = MAXPIXELCLOCK;
		break;
	   case 15:
	   case 16:
#ifdef CV_AGGRESSIVE_TIMING
		maxpix = MAXPIXELCLOCK - 35000000;
#else
		maxpix = MAXPIXELCLOCK - 55000000;
#endif
		break;
	   case 24:
	   case 32:
#ifdef CV_AGGRESSIVE_TIMING
		maxpix = MAXPIXELCLOCK - 75000000;
#else
		maxpix = MAXPIXELCLOCK - 85000000;
#endif
		break;
	   default:
		return (0);
	}

	if (gv->pixel_clock > maxpix)
		return (0);

	/*
	 * These are the supported witdh values for the
	 * graphics engine. To Support other widths, one
	 * has to use one of these widths for memory alignment, i.e.
	 * one has to set CRT_ID_SCREEN_OFFSET to one of these values and
	 * CRT_ID_HOR_DISP_ENA_END to the desired width.
	 * Since a working graphics engine is essential
	 * for the console, console modes of other width are not supported.
	 * We could do that, though, but then you have to tell the Xserver
	 * about this strange configuration and I don't know how at the moment :-)
	 */

	switch (gv->disp_width) {
	    case 1024:
	    case 640:
	    case 800:
	    case 1280:
	    case 1152:
	    case 1600:
		widthok = 1;
		break;
	    default: /* XXX*/
		widthok = 0;
		break;
	}

	if (widthok) return (1);
	else {
		if (gv->mode_num == 255) { /* console mode */
			printf ("This display width is not supported by the CV64 console.\n");
			printf ("Use one of 640 800 1024 1152 1280 1600!\n");
			return (0);
		} else {
			printf ("Warning for mode %d:\n", (int) gv->mode_num);
			printf ("Don't use a blitter-suporting Xserver with this display width\n");
			printf ("Use one of 640 800 1024 1152 1280 1600!\n");
			return (1);
		}
	}
	return (1);
}

int
cv_load_mon(gp, md)
	struct grf_softc *gp;
	struct grfcvtext_mode *md;
{
	struct grfvideo_mode *gv;
	struct grfinfo *gi;
	volatile caddr_t ba, fb;
	unsigned short mnr;
	unsigned short HT, HDE, HBS, HBE, HSS, HSE, VDE, VBS, VBE, VSS,
		VSE, VT;
	char LACE, DBLSCAN, TEXT, CONSOLE;
	int uplim, lowlim;
	int cr50, cr33, sr15, sr18, clock_mode, test;
	int m, n;	/* For calc'ing display FIFO */
	int tfillm, temptym;	/* FIFO fill and empty mclk's */
	int hmul;	/* Multiplier for hor. Values */
	/* identity */
	gv = &md->gv;

	/*
	 * No way to get text modes to work.
	 * Blame phase5, not me!
	 */
	TEXT = 0; /* (gv->depth == 4); */
	CONSOLE = (gv->mode_num == 255);

	if (!cv_mondefok(gv)) {
		printf("grfcv: The monitor definition is not okay.\n");
		printf("grfcv: See the manpage of grfconfig for more informations\n");
		return (0);
	}
	ba = gp->g_regkva;
	fb = gp->g_fbkva;

	/* turn gfx off, don't mess up the display */
	gfx_on_off(1, ba);

	/* provide all needed information in grf device-independant locations */
	gp->g_data		= (caddr_t) gv;
	gi = &gp->g_display;
	gi->gd_colors		= 1 << gv->depth;
	gi->gd_planes		= gv->depth;
	gi->gd_fbwidth		= gv->disp_width;
	gi->gd_fbheight		= gv->disp_height;
	gi->gd_fbx		= 0;
	gi->gd_fby		= 0;
	if (CONSOLE) {
		gi->gd_dwidth	= md->fx * md->cols;
		gi->gd_dheight	= md->fy * md->rows;
	} else {
		gi->gd_dwidth	= gv->disp_width;
		gi->gd_dheight	= gv->disp_height;
	}
	gi->gd_dx		= 0;
	gi->gd_dy		= 0;

	/* get display mode parameters */
	switch (gv->depth) {
		case 15:
		case 16:
			hmul = 2;
			break;
		default:
			hmul = 1;
        		break;
	}

	HBS = gv->hblank_start * hmul;
	HBE = gv->hblank_stop * hmul;
	HSS = gv->hsync_start * hmul;
	HSE = gv->hsync_stop * hmul;
	HT  = gv->htotal*hmul - 5;
	VBS = gv->vblank_start - 1;
	VSS = gv->vsync_start;
	VSE = gv->vsync_stop;
	VBE = gv->vblank_stop;
	VT  = gv->vtotal - 2;

	if (TEXT)
		HDE = ((gv->disp_width + md->fx - 1) / md->fx) - 1;
	else
		HDE = (gv->disp_width + 3) * hmul / 8 - 1; /*HBS;*/
	VDE = gv->disp_height - 1;

	/* figure out whether lace or dblscan is needed */

	uplim = gv->disp_height + (gv->disp_height / 4);
	lowlim = gv->disp_height - (gv->disp_height / 4);
	LACE = (((VT * 2) > lowlim) && ((VT * 2) < uplim)) ? 1 : 0;
	DBLSCAN = (((VT / 2) > lowlim) && ((VT / 2) < uplim)) ? 1 : 0;

	/* adjustments */

	if (LACE)
		VDE /= 2;

	WSeq(ba, SEQ_ID_MEMORY_MODE, (TEXT || (gv->depth == 1)) ? 0x06 : 0x0e);
	WGfx(ba, GCT_ID_READ_MAP_SELECT, 0x00);
	WSeq(ba, SEQ_ID_MAP_MASK, (gv->depth == 1) ? 0x01 : 0xff);
	WSeq(ba, SEQ_ID_CHAR_MAP_SELECT, 0x00);

	/* Set clock */

	mnr = compute_clock(gv->pixel_clock);
	WSeq(ba, SEQ_ID_DCLK_HI, ((mnr & 0xFF00) >> 8) );
	WSeq(ba, SEQ_ID_DCLK_LO, (mnr & 0xFF));

	/* load display parameters into board */

	WCrt(ba, CRT_ID_EXT_HOR_OVF,
	   ((HT & 0x100) ? 0x01 : 0x00) |
	   ((HDE & 0x100) ? 0x02 : 0x00) |
	   ((HBS & 0x100) ? 0x04 : 0x00) |
	/* ((HBE & 0x40) ? 0x08 : 0x00) | */  /* Later... */
	   ((HSS & 0x100) ? 0x10 : 0x00) |
	/* ((HSE & 0x20) ? 0x20 : 0x00) | */
	   (((HT-5) & 0x100) ? 0x40 : 0x00) );

	WCrt(ba, CRT_ID_EXT_VER_OVF,
	    0x40 |	/* Line compare */
	    ((VT  & 0x400) ? 0x01 : 0x00) |
	    ((VDE & 0x400) ? 0x02 : 0x00) |
	    ((VBS & 0x400) ? 0x04 : 0x00) |
	    ((VSS & 0x400) ? 0x10 : 0x00) );

	WCrt(ba, CRT_ID_HOR_TOTAL, HT);
	WCrt(ba, CRT_ID_DISPLAY_FIFO, HT - 5);

	WCrt(ba, CRT_ID_HOR_DISP_ENA_END, ((HDE >= HBS) ? (HBS - 1) : HDE));
	WCrt(ba, CRT_ID_START_HOR_BLANK, HBS);
	WCrt(ba, CRT_ID_END_HOR_BLANK, ((HBE & 0x1f) | 0x80));
	WCrt(ba, CRT_ID_START_HOR_RETR, HSS);
	WCrt(ba, CRT_ID_END_HOR_RETR,
	    (HSE & 0x1f) |
	    ((HBE & 0x20) ? 0x80 : 0x00) );
	WCrt(ba, CRT_ID_VER_TOTAL, VT);
	WCrt(ba, CRT_ID_OVERFLOW,
	    0x10 |
	    ((VT  & 0x100) ? 0x01 : 0x00) |
	    ((VDE & 0x100) ? 0x02 : 0x00) |
	    ((VSS & 0x100) ? 0x04 : 0x00) |
	    ((VBS & 0x100) ? 0x08 : 0x00) |
	    ((VT  & 0x200) ? 0x20 : 0x00) |
	    ((VDE & 0x200) ? 0x40 : 0x00) |
	    ((VSS & 0x200) ? 0x80 : 0x00) );

	WCrt(ba, CRT_ID_MAX_SCAN_LINE,
	    0x40 |  /* TEXT ? 0x00 ??? */
	    (DBLSCAN ? 0x80 : 0x00) |
	    ((VBS & 0x200) ? 0x20 : 0x00) |
	    (TEXT ? ((md->fy - 1) & 0x1f) : 0x00));

	WCrt(ba, CRT_ID_MODE_CONTROL,
	    ((TEXT || (gv->depth == 1)) ? 0xc3 : 0xe3));

	/* text cursor */

	if (TEXT) {
#if 1
		WCrt(ba, CRT_ID_CURSOR_START, (md->fy & 0x1f) - 2);
		WCrt(ba, CRT_ID_CURSOR_END, (md->fy & 0x1f) - 1);
#else
		WCrt(ba, CRT_ID_CURSOR_START, 0x00);
		WCrt(ba, CRT_ID_CURSOR_END, md->fy & 0x1f);
#endif
		WCrt(ba, CRT_ID_UNDERLINE_LOC, (md->fy - 1) & 0x1f);

		WCrt(ba, CRT_ID_CURSOR_LOC_HIGH, 0x00);
		WCrt(ba, CRT_ID_CURSOR_LOC_LOW, 0x00);
	}

	WCrt(ba, CRT_ID_START_ADDR_HIGH, 0x00);
	WCrt(ba, CRT_ID_START_ADDR_LOW, 0x00);

	WCrt(ba, CRT_ID_START_VER_RETR, VSS);
	WCrt(ba, CRT_ID_END_VER_RETR, (VSE & 0x0f));
	WCrt(ba, CRT_ID_VER_DISP_ENA_END, VDE);
	WCrt(ba, CRT_ID_START_VER_BLANK, VBS);
	WCrt(ba, CRT_ID_END_VER_BLANK, VBE);

	WCrt(ba, CRT_ID_LINE_COMPARE, 0xff);
	WCrt(ba, CRT_ID_LACE_RETR_START, HT / 2);
	WCrt(ba, CRT_ID_LACE_CONTROL, (LACE ? 0x20 : 0x00));

	WGfx(ba, GCT_ID_GRAPHICS_MODE,
	    ((TEXT || (gv->depth == 1)) ? 0x00 : 0x40));
	WGfx(ba, GCT_ID_MISC, (TEXT ? 0x04 : 0x01));

	WSeq (ba, SEQ_ID_MEMORY_MODE,
	    ((TEXT || (gv->depth == 1)) ? 0x06 : 0x02));

	vgaw(ba, VDAC_MASK, 0xff);

	sr15 = RSeq(ba, SEQ_ID_CLKSYN_CNTL_2);
	sr15 &= 0xef;
	sr18 = RSeq(ba, SEQ_ID_RAMDAC_CNTL);
	sr18 &= 0x7f;
	cr33 = RCrt(ba, CRT_ID_BACKWAD_COMP_2);
	cr33 &= 0xdf;
	clock_mode = 0x00;
	cr50 = 0x00;

	test = RCrt(ba, CRT_ID_EXT_MISC_CNTL_2);
	test &= 0xd;

	/* clear roxxler  byte-swapping... */
	cv_write_port(0x0040, cv_boardaddr);
	cv_write_port(0x0020, cv_boardaddr);

	switch (gv->depth) {
	   case 1:
	   case 4: /* text */
		HDE = gv->disp_width / 16;
		break;
	   case 8:
		if (gv->pixel_clock > 80000000) {
			clock_mode = 0x10 | 0x02;
			sr15 |= 0x10;
			sr18 |= 0x80;
			cr33 |= 0x20;
		}
		HDE = gv->disp_width / 8;
		cr50 |= 0x00;
		break;
	   case 15:
		cv_write_port (0x8020, cv_boardaddr);
		clock_mode = 0x30;
		HDE = gv->disp_width / 4;
		cr50 |= 0x10;
		break;
	   case 16:
		cv_write_port (0x8020, cv_boardaddr);
		clock_mode = 0x50;
		HDE = gv->disp_width / 4;
		cr50 |= 0x10;
		break;
	   case 24: /* this is really 32 Bit on CV64 */
	   case 32:
		cv_write_port(0x8040, cv_boardaddr);
		clock_mode = 0xd0;
		HDE = (gv->disp_width / 2);
		cr50 |= 0x30;
		break;
	}

	WCrt(ba, CRT_ID_EXT_MISC_CNTL_2, clock_mode | test);
	WSeq(ba, SEQ_ID_CLKSYN_CNTL_2, sr15);
	WSeq(ba, SEQ_ID_RAMDAC_CNTL, sr18);
	WCrt(ba, CRT_ID_BACKWAD_COMP_2, cr33);
	WCrt(ba, CRT_ID_SCREEN_OFFSET, HDE);

	test = RCrt(ba, CRT_ID_EXT_SYS_CNTL_2);
	test &= ~0x30;
	/* HDE Overflow in bits 4-5 */
	test |= (HDE >> 4) & 0x30;
	WCrt(ba, CRT_ID_EXT_SYS_CNTL_2, test);

	/* Set up graphics engine */
	switch (gv->disp_width) {
	   case 1024:
		cr50 |= 0x00;
		break;
	   case 640:
		cr50 |= 0x40;
		break;
	   case 800:
		cr50 |= 0x80;
		break;
	   case 1280:
		cr50 |= 0xc0;
		break;
	   case 1152:
		cr50 |= 0x01;
		break;
	   case 1600:
		cr50 |= 0x81;
		break;
	   default: /* XXX*/
		break;
	}

	WCrt(ba, CRT_ID_EXT_SYS_CNTL_1, cr50);

	delay(100000);
	WAttr(ba, ACT_ID_ATTR_MODE_CNTL, (TEXT ? 0x0a : 0x41));
	delay(100000);
	WAttr(ba, ACT_ID_COLOR_PLANE_ENA,
	    (gv->depth == 1) ? 0x01 : 0x0f);
	delay(100000);

	/*
	 * M-Parameter of Display FIFO
	 * This is dependant on the pixel clock and the memory clock.
	 * The FIFO filling bandwidth is 240 MHz  and the FIFO is 96 Byte wide.
	 * Then the time to fill the FIFO is tfill = (96/240000000) sec, the time
	 * to empty the FIFO is tempty = (96/pixelclock) sec.
	 * Then the M parameter maximum is ((tempty-tfill)*cv_memclk-9)/2.
	 * This seems to be logical, ain't it?
	 * Remember: We have to use integer arithmetics :(
	 * Divide by 1000 to prevent overflows.
	 */

	tfillm = (96 * (cv_memclk/1000))/240000;

	switch(gv->depth) {
	   case 32:
	   case 24:
		temptym = (24 * (cv_memclk/1000)) / (gv->pixel_clock/1000);
		break;
	   case 15:
	   case 16:
		temptym = (48 * (cv_memclk/1000)) / (gv->pixel_clock/1000);
		break;
	   default:
		temptym = (96 * (cv_memclk/1000)) / (gv->pixel_clock/1000);
		break;
	}

	m = (temptym - tfillm - 9) / 2;
	m = (m & 0x1f) << 3;
	if (m < 0x18)
		m = 0x18;
	n = 0xff;

	WCrt(ba, CRT_ID_EXT_MEM_CNTL_2, m);
	WCrt(ba, CRT_ID_EXT_MEM_CNTL_3, n);
	delay(10000);

	/* text initialization */

	if (TEXT) {
		cv_inittextmode(gp);
	}

	if (CONSOLE) {
		int i;
		vgaw(ba, VDAC_ADDRESS_W, 0);
		for (i = 0; i < 4; i++) {
			vgaw(ba, VDAC_DATA, cvconscolors[i][0]);
			vgaw(ba, VDAC_DATA, cvconscolors[i][1]);
			vgaw(ba, VDAC_DATA, cvconscolors[i][2]);
		}
	}

	/* Some kind of Magic */
	WAttr(ba, 0x33, 0);

	/* turn gfx on again */
	gfx_on_off(0, ba);

	/* Pass-through */
	cvscreen(0, ba - 0x02000000);

	return (1);
}

void
cv_inittextmode(gp)
	struct grf_softc *gp;
{
	struct grfcvtext_mode *tm = (struct grfcvtext_mode *)gp->g_data;
	volatile caddr_t ba, fb;
	unsigned char *c, *f, y;
	unsigned short z;

	ba = gp->g_regkva;
	fb = gp->g_fbkva;

	/* load text font into beginning of display memory.
	 * Each character cell is 32 bytes long (enough for 4 planes)
	 */

	SetTextPlane(ba, 0x02);
	cv_memset(fb, 0, 256 * 32);
	c = (unsigned char *) (fb) + (32 * tm->fdstart);
	f = tm->fdata;
	for (z = tm->fdstart; z <= tm->fdend; z++, c += (32 - tm->fy))
		for (y = 0; y < tm->fy; y++)
			*c++ = *f++;

	/* clear out text/attr planes (three screens worth) */

	SetTextPlane(ba, 0x01);
	cv_memset(fb, 0x07, tm->cols * tm->rows * 3);
	SetTextPlane(ba, 0x00);
	cv_memset(fb, 0x20, tm->cols * tm->rows * 3);

	/* print out a little init msg */

	c = (unsigned char *)(fb) + (tm->cols-16);
	strcpy(c, "CV64");
	c[6] = 0x20;

	/* set colors (B&W) */

	vgaw(ba, VDAC_ADDRESS_W, 0);
	for (z=0; z<256; z++) {
		unsigned char r, g, b;

		y = (z & 1) ? ((z > 7) ? 2 : 1) : 0;

		r = cvconscolors[y][0];
		g = cvconscolors[y][1];
		b = cvconscolors[y][2];
		vgaw(ba, VDAC_DATA, r >> 2);
		vgaw(ba, VDAC_DATA, g >> 2);
		vgaw(ba, VDAC_DATA, b >> 2);
	}
}

void
cv_memset(d, c, l)
	unsigned char *d;
	unsigned char c;
	int l;
{
	for(; l > 0; l--)
		*d++ = c;
}


static inline void
cv_write_port(bits, BoardAddr)
	unsigned short bits;
	volatile caddr_t BoardAddr;
{
	volatile caddr_t addr;
	static unsigned char CVPortBits = 0;	/* mirror port bits here */

	addr = BoardAddr + 0x40001;
	if (bits & 0x8000)
		CVPortBits |= bits & 0xFF;	/* Set bits */
	else {
		bits = bits & 0xFF;
		bits = (~bits) & 0xFF ;
		CVPortBits &= bits;	/* Clear bits */
	}

	*addr = CVPortBits;
}


/*
 *  Monitor Switch
 *  0 = CyberVision Signal
 *  1 = Amiga Signal,
 * ba = boardaddr
 */
static inline void
cvscreen(toggle, ba)
	int toggle;
	volatile caddr_t ba;
{

	if (toggle == 1)
		cv_write_port (0x10, ba);
	else
		cv_write_port (0x8010, ba);
}

/* 0 = on, 1= off */
/* ba= registerbase */
static inline void
gfx_on_off(toggle, ba)
	int toggle;
	volatile caddr_t ba;
{
	int r;

	toggle &= 0x1;
	toggle = toggle << 5;

	r = RSeq(ba, SEQ_ID_CLOCKING_MODE);
	r &= 0xdf;	/* set Bit 5 to 0 */

	WSeq(ba, SEQ_ID_CLOCKING_MODE, r | toggle);
}

#endif  /* NGRFCV */
