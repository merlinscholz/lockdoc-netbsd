/*	$NetBSD: cgsix.c,v 1.39 1998/03/23 17:37:04 pk Exp $ */

/*-
 * Copyright (c) 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Paul Kranenburg.
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

/*
 * Copyright (c) 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This software was developed by the Computer Systems Engineering group
 * at Lawrence Berkeley Laboratory under DARPA contract BG 91-66 and
 * contributed to Berkeley.
 *
 * All advertising materials mentioning features or use of this software
 * must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Lawrence Berkeley Laboratory.
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
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cgsix.c	8.4 (Berkeley) 1/21/94
 */

/*
 * color display (cgsix) driver.
 *
 * Does not handle interrupts, even though they can occur.
 *
 * XXX should defer colormap updates to vertical retrace interrupts
 */

#include "opt_uvm.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/buf.h>
#include <sys/device.h>
#include <machine/fbio.h>
#include <sys/ioctl.h>
#include <sys/malloc.h>
#include <sys/mman.h>
#include <sys/tty.h>
#include <sys/conf.h>

#ifdef DEBUG
#include <sys/proc.h>
#include <sys/syslog.h>
#endif

#include <vm/vm.h>

#include <machine/bus.h>
#include <machine/autoconf.h>
#include <machine/pmap.h>
#include <machine/fbvar.h>
#include <machine/cpu.h>
#include <machine/eeprom.h>
#include <machine/conf.h>

#include <sparc/dev/btreg.h>
#include <sparc/dev/btvar.h>
#include <sparc/dev/cgsixreg.h>
#include <sparc/dev/sbusvar.h>
#include <sparc/dev/pfourreg.h>

union cursor_cmap {		/* colormap, like bt_cmap, but tiny */
	u_char	cm_map[2][3];	/* 2 R/G/B entries */
	u_int	cm_chip[2];	/* 2 chip equivalents */
};

struct cg6_cursor {		/* cg6 hardware cursor status */
	short	cc_enable;		/* cursor is enabled */
	struct	fbcurpos cc_pos;	/* position */
	struct	fbcurpos cc_hot;	/* hot-spot */
	struct	fbcurpos cc_size;	/* size of mask & image fields */
	u_int	cc_bits[2][32];		/* space for mask & image bits */
	union	cursor_cmap cc_color;	/* cursor colormap */
};

/* per-display variables */
struct cgsix_softc {
	struct device	sc_dev;		/* base device */
	struct sbusdev	sc_sd;		/* sbus device */
	struct fbdevice	sc_fb;		/* frame buffer device */
	bus_space_tag_t	sc_bustag;
	bus_type_t	sc_btype;	/* phys address description */
	bus_addr_t	sc_paddr;	/* for device mmap() */

	volatile struct bt_regs *sc_bt;		/* Brooktree registers */
	volatile int *sc_fhc;			/* FHC register */
	volatile struct cg6_thc *sc_thc;	/* THC registers */
	volatile struct cg6_tec_xxx *sc_tec;	/* TEC registers */
	short	sc_fhcrev;		/* hardware rev */
	short	sc_blanked;		/* true if blanked */
	struct	cg6_cursor sc_cursor;	/* software cursor info */
	union	bt_cmap sc_cmap;	/* Brooktree color map */
};

/* autoconfiguration driver */
static int	cgsixmatch_sbus __P((struct device *, struct cfdata *, void *));
static int	cgsixmatch_obio __P((struct device *, struct cfdata *, void *));
static void	cgsixattach_sbus __P((struct device *, struct device *, void *));
static void	cgsixattach_obio __P((struct device *, struct device *, void *));
static void	cg6_unblank __P((struct device *));
static int	cg6_pfour_probe __P((void *, void *));
static void	cg6attach __P((struct cgsix_softc *, char *, int, int));

/* cdevsw prototypes */
cdev_decl(cgsix);

struct cfattach cgsix_sbus_ca = {
	sizeof(struct cgsix_softc), cgsixmatch_sbus, cgsixattach_sbus
};

struct cfattach cgsix_obio_ca = {
	sizeof(struct cgsix_softc), cgsixmatch_obio, cgsixattach_obio
};


extern struct cfdriver cgsix_cd;

/* frame buffer generic driver */
static struct fbdriver cg6_fbdriver = {
	cg6_unblank, cgsixopen, cgsixclose, cgsixioctl, cgsixpoll, cgsixmmap
};

/*
 * Unlike the bw2 and cg3 drivers, we do not need to provide an rconsole
 * interface, as the cg6 is fast enough.. but provide a knob to turn it
 * on anyway.
 */
#ifdef RASTERCONSOLE
int cgsix_use_rasterconsole = 0;
#else
#define cgsix_use_rasterconsole 0
#endif

extern int fbnode;

static void cg6_reset __P((struct cgsix_softc *));
static void cg6_loadcmap __P((struct cgsix_softc *, int, int));
static void cg6_loadomap __P((struct cgsix_softc *));
static void cg6_setcursor __P((struct cgsix_softc *));/* set position */
static void cg6_loadcursor __P((struct cgsix_softc *));/* set shape */

/*
 * Match a cgsix.
 */
int
cgsixmatch_sbus(parent, cf, aux)
	struct device *parent;
	struct cfdata *cf;
	void *aux;
{
	struct sbus_attach_args *sa = aux;

	return (strcmp(cf->cf_driver->cd_name, sa->sa_name) == 0);
}

int
cgsixmatch_obio(parent, cf, aux)
	struct device *parent;
	struct cfdata *cf;
	void *aux;
{
	union obio_attach_args *uoba = aux;
	struct obio4_attach_args *oba;

	if (uoba->uoba_isobio4 == 0)
		return (0);

	oba = &uoba->uoba_oba4;
	return (obio_bus_probe(oba->oba_bustag, oba->oba_paddr,
			       CGSIX_FHC_OFFSET, 4,
			       cg6_pfour_probe, NULL));
}

int
cg6_pfour_probe(vaddr, arg)
	void *vaddr;
	void *arg;
{

	return (fb_pfour_id(vaddr) == PFOUR_ID_FASTCOLOR);
}


/*
 * Attach a display.
 */
void
cgsixattach_sbus(parent, self, aux)
	struct device *parent, *self;
	void *aux;
{
	struct cgsix_softc *sc = (struct cgsix_softc *)self;
	struct sbus_attach_args *sa = aux;
	struct fbdevice *fb = &sc->sc_fb;
	int node, isconsole;
	char *name;
	bus_space_handle_t bh;
	extern struct tty *fbconstty;

	/* Remember cookies for cgsix_mmap() */
	sc->sc_bustag = sa->sa_bustag;
	sc->sc_btype = (bus_type_t)sa->sa_slot;
	sc->sc_paddr = (bus_addr_t)sa->sa_offset;

	node = sa->sa_node;

	fb->fb_driver = &cg6_fbdriver;
	fb->fb_device = &sc->sc_dev;
	fb->fb_type.fb_type = FBTYPE_SUNFAST_COLOR;
	fb->fb_flags = sc->sc_dev.dv_cfdata->cf_flags & FB_USERMASK;
	fb->fb_type.fb_depth = 8;

	fb_setsize_obp(fb, fb->fb_type.fb_depth, 1152, 900, node);

	/*
	 * Dunno what the PROM has mapped, though obviously it must have
	 * the video RAM mapped.  Just map what we care about for ourselves
	 * (the FHC, THC, and Brooktree registers).
	 */
	if (sbus_bus_map(sa->sa_bustag, sa->sa_slot,
			 sa->sa_offset + CGSIX_BT_OFFSET,
			 sizeof(*sc->sc_bt),
			 BUS_SPACE_MAP_LINEAR,
			 0, &bh) != 0) {
		printf("%s: cannot map brooktree registers\n", self->dv_xname);
		return;
	}
	sc->sc_bt = (struct bt_regs *)bh;

	if (sbus_bus_map(sa->sa_bustag, sa->sa_slot,
			 sa->sa_offset + CGSIX_FHC_OFFSET,
			 sizeof(*sc->sc_fhc),
			 BUS_SPACE_MAP_LINEAR,
			 0, &bh) != 0) {
		printf("%s: cannot map FHC registers\n", self->dv_xname);
		return;
	}
	sc->sc_fhc = (int *)bh;

	if (sbus_bus_map(sa->sa_bustag, sa->sa_slot,
			 sa->sa_offset + CGSIX_THC_OFFSET,
			 sizeof(*sc->sc_thc),
			 BUS_SPACE_MAP_LINEAR,
			 0, &bh) != 0) {
		printf("%s: cannot map THC registers\n", self->dv_xname);
		return;
	}
	sc->sc_thc = (struct cg6_thc *)bh;

	if (sbus_bus_map(sa->sa_bustag, sa->sa_slot,
			 sa->sa_offset + CGSIX_TEC_OFFSET,
			 sizeof(*sc->sc_tec),
			 BUS_SPACE_MAP_LINEAR,
			 0, &bh) != 0) {
		printf("%s: cannot map TEC registers\n", self->dv_xname);
		return;
	}
	sc->sc_tec = (struct cg6_tec_xxx *)bh;

	sbus_establish(&sc->sc_sd, &sc->sc_dev);
	name = getpropstring(node, "model");

	isconsole = node == fbnode && fbconstty != NULL;
	if (isconsole && cgsix_use_rasterconsole) {
		int ramsize = fb->fb_type.fb_height * fb->fb_linebytes;
		if (sbus_bus_map(sa->sa_bustag, sa->sa_slot,
				 sa->sa_offset + CGSIX_RAM_OFFSET,
				 ramsize,
				 BUS_SPACE_MAP_LINEAR,
				 0, &bh) != 0) {
			printf("%s: cannot map pixels\n", self->dv_xname);
			return;
		}
		sc->sc_fb.fb_pixels = (caddr_t)bh;
	}

	cg6attach(sc, name, isconsole, node == fbnode);

}

void
cgsixattach_obio(parent, self, aux)
	struct device *parent, *self;
	void *aux;
{
	struct cgsix_softc *sc = (struct cgsix_softc *)self;
	union obio_attach_args *uoba = aux;
	struct obio4_attach_args *oba;
	struct eeprom *eep = (struct eeprom *)eeprom_va;
	struct fbdevice *fb = &sc->sc_fb;
	bus_space_handle_t bh;
	int constype, isconsole;
	char *name;
	extern struct tty *fbconstty;

	if (uoba->uoba_isobio4 == 0) {
		cgsixattach_sbus(parent, self, aux);
		return;
	}

	oba = &uoba->uoba_oba4;

	/* Remember cookies for cgsix_mmap() */
	sc->sc_bustag = oba->oba_bustag;
	sc->sc_btype = (bus_type_t)0;
	sc->sc_paddr = (bus_addr_t)oba->oba_paddr;

	fb->fb_driver = &cg6_fbdriver;
	fb->fb_device = &sc->sc_dev;
	fb->fb_type.fb_type = FBTYPE_SUNFAST_COLOR;
	fb->fb_flags = sc->sc_dev.dv_cfdata->cf_flags & FB_USERMASK;
	fb->fb_type.fb_depth = 8;

	fb_setsize_eeprom(fb, fb->fb_type.fb_depth, 1152, 900);

	/*
	 * Dunno what the PROM has mapped, though obviously it must have
	 * the video RAM mapped.  Just map what we care about for ourselves
	 * (the FHC, THC, and Brooktree registers).
	 */
	if (obio_bus_map(oba->oba_bustag, oba->oba_paddr,
			 CGSIX_BT_OFFSET,
			 sizeof(*sc->sc_bt),
			 BUS_SPACE_MAP_LINEAR,
			 0, &bh) != 0) {
		printf("%s: cannot map brooktree registers\n", self->dv_xname);
		return;
	}
	sc->sc_bt = (struct bt_regs *)bh;

	if (obio_bus_map(oba->oba_bustag, oba->oba_paddr,
			 CGSIX_FHC_OFFSET,
			 sizeof(*sc->sc_fhc),
			 BUS_SPACE_MAP_LINEAR,
			 0, &bh) != 0) {
		printf("%s: cannot map FHC registers\n", self->dv_xname);
		return;
	}
	sc->sc_fhc = (int *)bh;

	if (obio_bus_map(oba->oba_bustag, oba->oba_paddr,
			 CGSIX_THC_OFFSET,
			 sizeof(*sc->sc_thc),
			 BUS_SPACE_MAP_LINEAR,
			 0, &bh) != 0) {
		printf("%s: cannot map THC registers\n", self->dv_xname);
		return;
	}
	sc->sc_thc = (struct cg6_thc *)bh;

	if (obio_bus_map(oba->oba_bustag, oba->oba_paddr,
			 CGSIX_TEC_OFFSET,
			 sizeof(*sc->sc_tec),
			 BUS_SPACE_MAP_LINEAR,
			 0, &bh) != 0) {
		printf("%s: cannot map TEC registers\n", self->dv_xname);
		return;
	}
	sc->sc_tec = (struct cg6_tec_xxx *)bh;


	if (fb_pfour_id((void *)sc->sc_fhc) == PFOUR_ID_FASTCOLOR) {
		fb->fb_flags |= FB_PFOUR;
		name = "cgsix/p4";
	} else
		name = "cgsix";

	constype = (fb->fb_flags & FB_PFOUR) ? EE_CONS_P4OPT : EE_CONS_COLOR;

	/*
	 * Assume this is the console if there's no eeprom info
	 * to be found.
	 */
	if (eep == NULL || eep->eeConsole == constype)
		isconsole = (fbconstty != NULL);
	else
		isconsole = 0;

	if (isconsole && cgsix_use_rasterconsole) {
		int ramsize = fb->fb_type.fb_height * fb->fb_linebytes;
		if (obio_bus_map(oba->oba_bustag, oba->oba_paddr,
				 CGSIX_RAM_OFFSET,
				 ramsize,
				 BUS_SPACE_MAP_LINEAR,
				 0, &bh) != 0) {
			printf("%s: cannot map pixels\n", self->dv_xname);
			return;
		}
		sc->sc_fb.fb_pixels = (caddr_t)bh;
	}

	cg6attach(sc, name, isconsole, 1);
}

void
cg6attach(sc, name, isconsole, isfb)
	struct cgsix_softc *sc;
	char *name;
	int isconsole;
	int isfb;
{
	int i;
	volatile struct bt_regs *bt = sc->sc_bt;
	struct fbdevice *fb = &sc->sc_fb;

	/* Don't have to map the pfour register on the cgsix. */
	fb->fb_pfour = NULL;

	fb->fb_type.fb_cmsize = 256;
	fb->fb_type.fb_size = fb->fb_type.fb_height * fb->fb_linebytes;
	printf(": %s, %d x %d", name,
	       fb->fb_type.fb_width, fb->fb_type.fb_height);

	sc->sc_fhcrev = (*sc->sc_fhc >> FHC_REV_SHIFT) &
			(FHC_REV_MASK >> FHC_REV_SHIFT);

	printf(", rev %d", sc->sc_fhcrev);

	/* reset cursor & frame buffer controls */
	cg6_reset(sc);

	/* grab initial (current) color map (DOES THIS WORK?) */
	bt->bt_addr = 0;
	for (i = 0; i < 256 * 3; i++)
		((char *)&sc->sc_cmap)[i] = bt->bt_cmap >> 24;

	/* enable video */
	sc->sc_thc->thc_misc |= THC_MISC_VIDEN;

	if (isconsole) {
		printf(" (console)");
#ifdef RASTERCONSOLE
		if (cgsix_use_rasterconsole)
			fbrcons_init(&sc->sc_fb);
#endif
	}

	printf("\n");
	if (isfb)
		fb_attach(&sc->sc_fb, isconsole);
}


int
cgsixopen(dev, flags, mode, p)
	dev_t dev;
	int flags, mode;
	struct proc *p;
{
	int unit = minor(dev);

	if (unit >= cgsix_cd.cd_ndevs || cgsix_cd.cd_devs[unit] == NULL)
		return (ENXIO);
	return (0);
}

int
cgsixclose(dev, flags, mode, p)
	dev_t dev;
	int flags, mode;
	struct proc *p;
{
	struct cgsix_softc *sc = cgsix_cd.cd_devs[minor(dev)];

	cg6_reset(sc);
	return (0);
}

int
cgsixioctl(dev, cmd, data, flags, p)
	dev_t dev;
	u_long cmd;
	caddr_t data;
	int flags;
	struct proc *p;
{
	struct cgsix_softc *sc = cgsix_cd.cd_devs[minor(dev)];
	u_int count;
	int v, error;
	union cursor_cmap tcm;

	switch (cmd) {

	case FBIOGTYPE:
		*(struct fbtype *)data = sc->sc_fb.fb_type;
		break;

	case FBIOGATTR:
#define fba ((struct fbgattr *)data)
		fba->real_type = sc->sc_fb.fb_type.fb_type;
		fba->owner = 0;		/* XXX ??? */
		fba->fbtype = sc->sc_fb.fb_type;
		fba->sattr.flags = 0;
		fba->sattr.emu_type = sc->sc_fb.fb_type.fb_type;
		fba->sattr.dev_specific[0] = -1;
		fba->emu_types[0] = sc->sc_fb.fb_type.fb_type;
		fba->emu_types[1] = -1;
#undef fba
		break;

	case FBIOGETCMAP:
		return (bt_getcmap((struct fbcmap *)data, &sc->sc_cmap, 256));

	case FBIOPUTCMAP:
		/* copy to software map */
#define	p ((struct fbcmap *)data)
		error = bt_putcmap(p, &sc->sc_cmap, 256);
		if (error)
			return (error);
		/* now blast them into the chip */
		/* XXX should use retrace interrupt */
		cg6_loadcmap(sc, p->index, p->count);
#undef p
		break;

	case FBIOGVIDEO:
		*(int *)data = sc->sc_blanked;
		break;

	case FBIOSVIDEO:
		if (*(int *)data)
			cg6_unblank(&sc->sc_dev);
		else if (!sc->sc_blanked) {
			sc->sc_blanked = 1;
			sc->sc_thc->thc_misc &= ~THC_MISC_VIDEN;
		}
		break;

/* these are for both FBIOSCURSOR and FBIOGCURSOR */
#define p ((struct fbcursor *)data)
#define cc (&sc->sc_cursor)

	case FBIOGCURSOR:
		/* do not quite want everything here... */
		p->set = FB_CUR_SETALL;	/* close enough, anyway */
		p->enable = cc->cc_enable;
		p->pos = cc->cc_pos;
		p->hot = cc->cc_hot;
		p->size = cc->cc_size;

		/* begin ugh ... can we lose some of this crap?? */
		if (p->image != NULL) {
			count = cc->cc_size.y * 32 / NBBY;
			error = copyout((caddr_t)cc->cc_bits[1],
			    (caddr_t)p->image, count);
			if (error)
				return (error);
			error = copyout((caddr_t)cc->cc_bits[0],
			    (caddr_t)p->mask, count);
			if (error)
				return (error);
		}
		if (p->cmap.red != NULL) {
			error = bt_getcmap(&p->cmap,
			    (union bt_cmap *)&cc->cc_color, 2);
			if (error)
				return (error);
		} else {
			p->cmap.index = 0;
			p->cmap.count = 2;
		}
		/* end ugh */
		break;

	case FBIOSCURSOR:
		/*
		 * For setcmap and setshape, verify parameters, so that
		 * we do not get halfway through an update and then crap
		 * out with the software state screwed up.
		 */
		v = p->set;
		if (v & FB_CUR_SETCMAP) {
			/*
			 * This use of a temporary copy of the cursor
			 * colormap is not terribly efficient, but these
			 * copies are small (8 bytes)...
			 */
			tcm = cc->cc_color;
			error = bt_putcmap(&p->cmap, (union bt_cmap *)&tcm, 2);
			if (error)
				return (error);
		}
		if (v & FB_CUR_SETSHAPE) {
			if ((u_int)p->size.x > 32 || (u_int)p->size.y > 32)
				return (EINVAL);
			count = p->size.y * 32 / NBBY;
#if defined(UVM)
			if (!uvm_useracc(p->image, count, B_READ) ||
			    !uvm_useracc(p->mask, count, B_READ))
				return (EFAULT);
#else
			if (!useracc(p->image, count, B_READ) ||
			    !useracc(p->mask, count, B_READ))
				return (EFAULT);
#endif
		}

		/* parameters are OK; do it */
		if (v & (FB_CUR_SETCUR | FB_CUR_SETPOS | FB_CUR_SETHOT)) {
			if (v & FB_CUR_SETCUR)
				cc->cc_enable = p->enable;
			if (v & FB_CUR_SETPOS)
				cc->cc_pos = p->pos;
			if (v & FB_CUR_SETHOT)
				cc->cc_hot = p->hot;
			cg6_setcursor(sc);
		}
		if (v & FB_CUR_SETCMAP) {
			cc->cc_color = tcm;
			cg6_loadomap(sc); /* XXX defer to vertical retrace */
		}
		if (v & FB_CUR_SETSHAPE) {
			cc->cc_size = p->size;
			count = p->size.y * 32 / NBBY;
			bzero((caddr_t)cc->cc_bits, sizeof cc->cc_bits);
			bcopy(p->mask, (caddr_t)cc->cc_bits[0], count);
			bcopy(p->image, (caddr_t)cc->cc_bits[1], count);
			cg6_loadcursor(sc);
		}
		break;

#undef p
#undef cc

	case FBIOGCURPOS:
		*(struct fbcurpos *)data = sc->sc_cursor.cc_pos;
		break;

	case FBIOSCURPOS:
		sc->sc_cursor.cc_pos = *(struct fbcurpos *)data;
		cg6_setcursor(sc);
		break;

	case FBIOGCURMAX:
		/* max cursor size is 32x32 */
		((struct fbcurpos *)data)->x = 32;
		((struct fbcurpos *)data)->y = 32;
		break;

	default:
#ifdef DEBUG
		log(LOG_NOTICE, "cgsixioctl(0x%lx) (%s[%d])\n", cmd,
		    p->p_comm, p->p_pid);
#endif
		return (ENOTTY);
	}
	return (0);
}

int
cgsixpoll(dev, events, p)
	dev_t dev;
	int events;
	struct proc *p;
{

	return (seltrue(dev, events, p));
}

/*
 * Clean up hardware state (e.g., after bootup or after X crashes).
 */
static void
cg6_reset(sc)
	struct cgsix_softc *sc;
{
	volatile struct cg6_tec_xxx *tec;
	int fhc;
	volatile struct bt_regs *bt;

	/* hide the cursor, just in case */
	sc->sc_thc->thc_cursxy = (THC_CURSOFF << 16) | THC_CURSOFF;

	/* turn off frobs in transform engine (makes X11 work) */
	tec = sc->sc_tec;
	tec->tec_mv = 0;
	tec->tec_clip = 0;
	tec->tec_vdc = 0;

	/* take care of hardware bugs in old revisions */
	if (sc->sc_fhcrev < 5) {
		/*
		 * Keep current resolution; set cpu to 68020, set test
		 * window (size 1Kx1K), and for rev 1, disable dest cache.
		 */
		fhc = (*sc->sc_fhc & FHC_RES_MASK) | FHC_CPU_68020 |
		    FHC_TEST |
		    (11 << FHC_TESTX_SHIFT) | (11 << FHC_TESTY_SHIFT);
		if (sc->sc_fhcrev < 2)
			fhc |= FHC_DST_DISABLE;
		*sc->sc_fhc = fhc;
	}

	/* Enable cursor in Brooktree DAC. */
	bt = sc->sc_bt;
	bt->bt_addr = 0x06 << 24;
	bt->bt_ctrl |= 0x03 << 24;
}

static void
cg6_setcursor(sc)
	struct cgsix_softc *sc;
{

	/* we need to subtract the hot-spot value here */
#define COORD(f) (sc->sc_cursor.cc_pos.f - sc->sc_cursor.cc_hot.f)
	sc->sc_thc->thc_cursxy = sc->sc_cursor.cc_enable ?
	    ((COORD(x) << 16) | (COORD(y) & 0xffff)) :
	    (THC_CURSOFF << 16) | THC_CURSOFF;
#undef COORD
}

static void
cg6_loadcursor(sc)
	struct cgsix_softc *sc;
{
	volatile struct cg6_thc *thc;
	u_int edgemask, m;
	int i;

	/*
	 * Keep the top size.x bits.  Here we *throw out* the top
	 * size.x bits from an all-one-bits word, introducing zeros in
	 * the top size.x bits, then invert all the bits to get what
	 * we really wanted as our mask.  But this fails if size.x is
	 * 32---a sparc uses only the low 5 bits of the shift count---
	 * so we have to special case that.
	 */
	edgemask = ~0;
	if (sc->sc_cursor.cc_size.x < 32)
		edgemask = ~(edgemask >> sc->sc_cursor.cc_size.x);
	thc = sc->sc_thc;
	for (i = 0; i < 32; i++) {
		m = sc->sc_cursor.cc_bits[0][i] & edgemask;
		thc->thc_cursmask[i] = m;
		thc->thc_cursbits[i] = m & sc->sc_cursor.cc_bits[1][i];
	}
}

/*
 * Load a subset of the current (new) colormap into the color DAC.
 */
static void
cg6_loadcmap(sc, start, ncolors)
	struct cgsix_softc *sc;
	int start, ncolors;
{
	volatile struct bt_regs *bt;
	u_int *ip, i;
	int count;

	ip = &sc->sc_cmap.cm_chip[BT_D4M3(start)];	/* start/4 * 3 */
	count = BT_D4M3(start + ncolors - 1) - BT_D4M3(start) + 3;
	bt = sc->sc_bt;
	bt->bt_addr = BT_D4M4(start) << 24;
	while (--count >= 0) {
		i = *ip++;
		/* hardware that makes one want to pound boards with hammers */
		bt->bt_cmap = i;
		bt->bt_cmap = i << 8;
		bt->bt_cmap = i << 16;
		bt->bt_cmap = i << 24;
	}
}

/*
 * Load the cursor (overlay `foreground' and `background') colors.
 */
static void
cg6_loadomap(sc)
	struct cgsix_softc *sc;
{
	volatile struct bt_regs *bt;
	u_int i;

	bt = sc->sc_bt;
	bt->bt_addr = 0x01 << 24;	/* set background color */
	i = sc->sc_cursor.cc_color.cm_chip[0];
	bt->bt_omap = i;		/* R */
	bt->bt_omap = i << 8;		/* G */
	bt->bt_omap = i << 16;		/* B */

	bt->bt_addr = 0x03 << 24;	/* set foreground color */
	bt->bt_omap = i << 24;		/* R */
	i = sc->sc_cursor.cc_color.cm_chip[1];
	bt->bt_omap = i;		/* G */
	bt->bt_omap = i << 8;		/* B */
}

static void
cg6_unblank(dev)
	struct device *dev;
{
	struct cgsix_softc *sc = (struct cgsix_softc *)dev;

	if (sc->sc_blanked) {
		sc->sc_blanked = 0;
		sc->sc_thc->thc_misc |= THC_MISC_VIDEN;
	}
}

/* XXX the following should be moved to a "user interface" header */
/*
 * Base addresses at which users can mmap() the various pieces of a cg6.
 * Note that although the Brooktree color registers do not occupy 8K,
 * the X server dies if we do not allow it to map 8K there (it just maps
 * from 0x70000000 forwards, as a contiguous chunk).
 */
#define	CG6_USER_FBC	0x70000000
#define	CG6_USER_TEC	0x70001000
#define	CG6_USER_BTREGS	0x70002000
#define	CG6_USER_FHC	0x70004000
#define	CG6_USER_THC	0x70005000
#define	CG6_USER_ROM	0x70006000
#define	CG6_USER_RAM	0x70016000
#define	CG6_USER_DHC	0x80000000

struct mmo {
	u_int	mo_uaddr;	/* user (virtual) address */
	u_int	mo_size;	/* size, or 0 for video ram size */
	u_int	mo_physoff;	/* offset from sc_physadr */
};

/*
 * Return the address that would map the given device at the given
 * offset, allowing for the given protection, or return -1 for error.
 *
 * XXX	needs testing against `demanding' applications (e.g., aviator)
 */
int
cgsixmmap(dev, off, prot)
	dev_t dev;
	int off, prot;
{
	struct cgsix_softc *sc = cgsix_cd.cd_devs[minor(dev)];
	struct mmo *mo;
	u_int u, sz;
	static struct mmo mmo[] = {
		{ CG6_USER_RAM, 0, CGSIX_RAM_OFFSET },

		/* do not actually know how big most of these are! */
		{ CG6_USER_FBC, 1, CGSIX_FBC_OFFSET },
		{ CG6_USER_TEC, 1, CGSIX_TEC_OFFSET },
		{ CG6_USER_BTREGS, 8192 /* XXX */, CGSIX_BT_OFFSET },
		{ CG6_USER_FHC, 1, CGSIX_FHC_OFFSET },
		{ CG6_USER_THC, sizeof(struct cg6_thc), CGSIX_THC_OFFSET },
		{ CG6_USER_ROM, 65536, CGSIX_ROM_OFFSET },
		{ CG6_USER_DHC, 1, CGSIX_DHC_OFFSET },
	};
#define NMMO (sizeof mmo / sizeof *mmo)

	if (off & PGOFSET)
		panic("cgsixmmap");

	/*
	 * Entries with size 0 map video RAM (i.e., the size in fb data).
	 *
	 * Since we work in pages, the fact that the map offset table's
	 * sizes are sometimes bizarre (e.g., 1) is effectively ignored:
	 * one byte is as good as one page.
	 */
	for (mo = mmo; mo < &mmo[NMMO]; mo++) {
		if ((u_int)off < mo->mo_uaddr)
			continue;
		u = off - mo->mo_uaddr;
		sz = mo->mo_size ? mo->mo_size : sc->sc_fb.fb_type.fb_size;
		if (u < sz)
			return (bus_space_mmap (sc->sc_bustag,
						sc->sc_btype,
						sc->sc_paddr+u+mo->mo_physoff,
						BUS_SPACE_MAP_LINEAR));
	}

#ifdef DEBUG
	{
	  struct proc *p = curproc;	/* XXX */
	  log(LOG_NOTICE, "cgsixmmap(0x%x) (%s[%d])\n",
		off, p->p_comm, p->p_pid);
	}
#endif
	return (-1);	/* not a user-map offset */
}
