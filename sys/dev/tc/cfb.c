/* $NetBSD: cfb.c,v 1.1 1998/10/29 12:24:24 nisimura Exp $ */

#include <sys/cdefs.h>			/* RCS ID & Copyright macro defns */

__KERNEL_RCSID(0, "$NetBSD: cfb.c,v 1.1 1998/10/29 12:24:24 nisimura Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/device.h>
#include <sys/malloc.h>
#include <sys/buf.h>
#include <sys/ioctl.h>
#include <vm/vm.h>

#include <machine/bus.h>
#include <machine/intr.h>

#include <dev/rcons/raster.h>
#include <dev/wscons/wsconsio.h>
#include <dev/wscons/wscons_raster.h>
#include <dev/wscons/wsdisplayvar.h>
#include <machine/autoconf.h>

#include <dev/tc/tcvar.h>
#include <dev/ic/bt459reg.h>	

#include "opt_uvm.h"
#if defined(UVM)
#include <uvm/uvm_extern.h>
#define useracc uvm_useracc
#endif

/* XXX BUS'IFYING XXX */
 
#if defined(__pmax__)
#define	machine_btop(x) mips_btop(x)
#define	MACHINE_KSEG0_TO_PHYS(x) MIPS_KSEG1_TO_PHYS(x)
#endif

#if defined(__alpha__) || defined(alpha)
/*
 * Digital UNIX never supports PMAG-BA
 */
#define machine_btop(x) alpha_btop(x)
#define MACHINE_KSEG0_TO_PHYS(x) ALPHA_K0SEG_TO_PHYS(x)
#endif

/* XXX XXX XXX */

/*
 * N.B., Bt459 registers are 8bit width.  Some of TC framebuffers have
 * weird register layout such as each of 2nd and 3rd Bt459 registers
 * is adjacent each other in a word, i.e.,
 *
 *	struct bt459triplet {
 * 		struct {
 *			u_int8_t u0;
 *			u_int8_t u1;
 *			u_int8_t u2;
 *			unsigned :8; 
 *		} bt_lo;
 *		...
 *
 * Although CX has single Bt459, 32bit R/W can be done w/o any trouble.
 */
struct bt459reg {
        u_int32_t       bt_lo;
        u_int32_t       bt_hi;
        u_int32_t       bt_reg;
        u_int32_t       bt_cmap;
};

struct fb_devconfig {
	vaddr_t dc_vaddr;		/* memory space virtual base address */
	paddr_t dc_paddr;		/* memory space physical base address */
	vsize_t	dc_size;		/* size of slot memory */
	int	dc_wid;			/* width of frame buffer */
	int	dc_ht;			/* height of frame buffer */
	int	dc_depth;		/* depth, bits per pixel */
	int	dc_rowbytes;		/* bytes in a FB scan line */
	vaddr_t dc_videobase;		/* base of flat frame buffer */
	struct raster	dc_raster;	/* raster description */
	struct rcons	dc_rcons;	/* raster blitter control info */
	int	    dc_blanked;		/* currently has video disabled */
};

struct hwcmap {
#define	CMAP_SIZE	256	/* 256 R/G/B entries */
	u_int8_t r[CMAP_SIZE];
	u_int8_t g[CMAP_SIZE];
	u_int8_t b[CMAP_SIZE];
};

struct hwcursor {
	struct wsdisplay_curpos cc_pos;
	struct wsdisplay_curpos cc_hot;
	struct wsdisplay_curpos cc_size;
#define	CURSOR_MAX_SIZE	64
	u_int8_t cc_color[6];
	u_int64_t cc_image[64 + 64];
};

struct cfb_softc {
	struct device sc_dev;
	struct fb_devconfig *sc_dc;	/* device configuration */
	struct hwcmap sc_cmap;		/* software copy of colormap */
	struct hwcursor sc_cursor;	/* software copy of cursor */
	int sc_curenb;			/* cursor sprite enabled */
	int sc_changed;			/* need update of colormap */
#define	DATA_ENB_CHANGED	0x01	/* cursor enable changed */
#define	DATA_CURCMAP_CHANGED	0x02	/* cursor colormap changed */
#define	DATA_CURSHAPE_CHANGED	0x04	/* cursor size, image, mask changed */
#define	DATA_CMAP_CHANGED	0x08	/* colormap changed */
#define	DATA_ALL_CHANGED	0x0f
	int nscreens;
};

int  cfbmatch __P((struct device *, struct cfdata *, void *));
void cfbattach __P((struct device *, struct device *, void *));

struct cfattach cfb_ca = {
	sizeof(struct cfb_softc), cfbmatch, cfbattach,
};

void cfb_getdevconfig __P((tc_addr_t, struct fb_devconfig *));
struct fb_devconfig cfb_console_dc;
tc_addr_t cfb_consaddr;

struct wsdisplay_emulops cfb_emulops = {
	rcons_cursor,			/* could use hardware cursor; punt */
	rcons_mapchar,
	rcons_putchar,
	rcons_copycols,
	rcons_erasecols,
	rcons_copyrows,
	rcons_eraserows,
	rcons_alloc_attr
};

struct wsscreen_descr cfb_stdscreen = {
	"std", 0, 0,
	&cfb_emulops,
	0, 0,
	0
};

const struct wsscreen_descr *_cfb_scrlist[] = {
	&cfb_stdscreen,
};

struct wsscreen_list cfb_screenlist = {
	sizeof(_cfb_scrlist) / sizeof(struct wsscreen_descr *), _cfb_scrlist
};

int	cfbioctl __P((void *, u_long, caddr_t, int, struct proc *));
int	cfbmmap __P((void *, off_t, int));

int	cfb_alloc_screen __P((void *, const struct wsscreen_descr *,
				      void **, int *, int *, long *));
void	cfb_free_screen __P((void *, void *));
void	cfb_show_screen __P((void *, void *));
int	cfb_load_font __P((void *, void *, int, int, int, void *));

struct wsdisplay_accessops cfb_accessops = {
	cfbioctl,
	cfbmmap,
	cfb_alloc_screen,
	cfb_free_screen,
	cfb_show_screen,
	cfb_load_font
};

int  cfb_cnattach __P((tc_addr_t));
int  cfbintr __P((void *));
void cfbinit __P((struct fb_devconfig *));

static int  get_cmap __P((struct cfb_softc *, struct wsdisplay_cmap *));
static int  set_cmap __P((struct cfb_softc *, struct wsdisplay_cmap *));
static int  set_cursor __P((struct cfb_softc *, struct wsdisplay_cursor *));
static int  get_cursor __P((struct cfb_softc *, struct wsdisplay_cursor *));
static void set_curpos __P((struct cfb_softc *, struct wsdisplay_curpos *));
static void bt459_set_curpos __P((struct cfb_softc *));

/* XXX XXX XXX */
#define	BT459_SELECT(vdac, regno) do {		\
	vdac->bt_lo = (regno) & 0x00ff;		\
	vdac->bt_hi = ((regno)& 0xff00) >> 8;	\
	tc_wmb();				\
   } while (0)
/* XXX XXX XXX */

#define	CFB_FB_OFFSET		0x000000
#define	CFB_FB_SIZE		0x100000
#define	CFB_BT459_OFFSET	0x200000
#define	CFB_OFFSET_IREQ		0x300000	/* Interrupt req. control */

int
cfbmatch(parent, match, aux)
	struct device *parent;
	struct cfdata *match;
	void *aux;
{
	struct tc_attach_args *ta = aux;

	if (strncmp("PMAG-BA ", ta->ta_modname, TC_ROM_LLEN) != 0)
		return (0);

	return (1);
}

void
cfb_getdevconfig(dense_addr, dc)
	tc_addr_t dense_addr;
	struct fb_devconfig *dc;
{
	struct raster *rap;
	struct rcons *rcp;
	int i;

	dc->dc_vaddr = dense_addr;
	dc->dc_paddr = MACHINE_KSEG0_TO_PHYS(dc->dc_vaddr + CFB_FB_OFFSET);

	dc->dc_wid = 1024;
	dc->dc_ht = 864;
	dc->dc_depth = 8;
	dc->dc_rowbytes = 1024;
	dc->dc_videobase = dc->dc_vaddr + CFB_FB_OFFSET;
	dc->dc_blanked = 0;

	/* initialize colormap and cursor resource */
	cfbinit(dc);

	/* clear the screen */
	for (i = 0; i < dc->dc_ht * dc->dc_rowbytes; i += sizeof(u_int32_t))
		*(u_int32_t *)(dc->dc_videobase + i) = 0x0;

	/* initialize the raster */
	rap = &dc->dc_raster;
	rap->width = dc->dc_wid;
	rap->height = dc->dc_ht;
	rap->depth = dc->dc_depth;
	rap->linelongs = dc->dc_rowbytes / sizeof(u_int32_t);
	rap->pixels = (u_int32_t *)dc->dc_videobase;

	/* initialize the raster console blitter */
	rcp = &dc->dc_rcons;
	rcp->rc_sp = rap;
	rcp->rc_crow = rcp->rc_ccol = -1;
	rcp->rc_crowp = &rcp->rc_crow;
	rcp->rc_ccolp = &rcp->rc_ccol;
	rcons_init(rcp, 34, 80);

	cfb_stdscreen.nrows = dc->dc_rcons.rc_maxrow;
	cfb_stdscreen.ncols = dc->dc_rcons.rc_maxcol;
}

void
cfbattach(parent, self, aux)
	struct device *parent, *self;
	void *aux;
{
	struct cfb_softc *sc = (struct cfb_softc *)self;
	struct tc_attach_args *ta = aux;
	struct wsemuldisplaydev_attach_args waa;
	struct hwcmap *cm;
	int console, i;

	console = (ta->ta_addr == cfb_consaddr);
	if (console) {
		sc->sc_dc = &cfb_console_dc;
		sc->nscreens = 1;
	}
	else {
		sc->sc_dc = (struct fb_devconfig *)
		    malloc(sizeof(struct fb_devconfig), M_DEVBUF, M_WAITOK);
		cfb_getdevconfig(ta->ta_addr, sc->sc_dc);
	}
	printf(": %d x %d, %dbpp\n", sc->sc_dc->dc_wid, sc->sc_dc->dc_ht,
	    sc->sc_dc->dc_depth);

	cm = &sc->sc_cmap;
	cm->r[0] = cm->g[0] = cm->b[0] = 0;
	for (i = 1; i < CMAP_SIZE; i++) {
		cm->r[i] = cm->g[i] = cm->b[i] = 0xff;
	}

	/* Establish an interrupt handler, and clear any pending interrupts */
        tc_intr_establish(parent, ta->ta_cookie, TC_IPL_TTY, cfbintr, sc);
	*(u_int8_t *)(sc->sc_dc->dc_vaddr + CFB_OFFSET_IREQ) = 0;

	waa.console = console;
	waa.scrdata = &cfb_screenlist;
	waa.accessops = &cfb_accessops;
	waa.accesscookie = sc;

	config_found(self, &waa, wsemuldisplaydevprint);
}

int
cfbioctl(v, cmd, data, flag, p)
	void *v;
	u_long cmd;
	caddr_t data;
	int flag;
	struct proc *p;
{
	struct cfb_softc *sc = v;
	struct fb_devconfig *dc = sc->sc_dc;
	int turnoff;

	switch (cmd) {
	case WSDISPLAYIO_GTYPE:
		*(u_int *)data = WSDISPLAY_TYPE_CFB;
		return (0);

	case WSDISPLAYIO_GINFO:
#define	wsd_fbip ((struct wsdisplay_fbinfo *)data)
		wsd_fbip->height = sc->sc_dc->dc_ht;
		wsd_fbip->width = sc->sc_dc->dc_wid;
		wsd_fbip->depth = sc->sc_dc->dc_depth;
		wsd_fbip->cmsize = CMAP_SIZE;
#undef fbt
		return (0);

	case WSDISPLAYIO_GETCMAP:
		return get_cmap(sc, (struct wsdisplay_cmap *)data);

	case WSDISPLAYIO_PUTCMAP:
		return set_cmap(sc, (struct wsdisplay_cmap *)data);

	case WSDISPLAYIO_SVIDEO:
		turnoff = *(int *)data == WSDISPLAYIO_VIDEO_OFF;
		if ((dc->dc_blanked == 0) ^ turnoff) {
			/* sc->sc_changed |= DATA_ALL_CHANGED; */
			dc->dc_blanked = turnoff;
		}
		return (0);

	case WSDISPLAYIO_GVIDEO:
		*(u_int *)data = dc->dc_blanked ?
		    WSDISPLAYIO_VIDEO_OFF : WSDISPLAYIO_VIDEO_ON;
		return (0);

	case WSDISPLAYIO_GCURPOS:
		*(struct wsdisplay_curpos *)data = sc->sc_cursor.cc_pos;
		return (0);

	case WSDISPLAYIO_SCURPOS:
		set_curpos(sc, (struct wsdisplay_curpos *)data);
		bt459_set_curpos(sc);
		return (0);

	case WSDISPLAYIO_GCURMAX:
		((struct wsdisplay_curpos *)data)->x =
		((struct wsdisplay_curpos *)data)->y = CURSOR_MAX_SIZE;
		return (0);

	case WSDISPLAYIO_GCURSOR:
		return get_cursor(sc, (struct wsdisplay_cursor *)data);

	case WSDISPLAYIO_SCURSOR:
		return set_cursor(sc, (struct wsdisplay_cursor *)data);
	}
	return ENOTTY;
}

int
cfbmmap(v, offset, prot)
	void *v;
	off_t offset;
	int prot;
{
	struct cfb_softc *sc = v;

	if (offset > CFB_FB_SIZE)
		return (-1);
	return machine_btop(sc->sc_dc->dc_paddr + offset);
}

int
cfb_alloc_screen(v, type, cookiep, curxp, curyp, attrp)
	void *v;
	const struct wsscreen_descr *type;
	void **cookiep;
	int *curxp, *curyp;
	long *attrp;
{
	struct cfb_softc *sc = v;
	long defattr;

	if (sc->nscreens > 0)
		return (ENOMEM);

	*cookiep = &sc->sc_dc->dc_rcons; /* one and only for now */
	*curxp = 0;
	*curyp = 0;
	rcons_alloc_attr(&sc->sc_dc->dc_rcons, 0, 0, 0, &defattr);
	*attrp = defattr;
	sc->nscreens++;
	return (0);
}

void
cfb_free_screen(v, cookie)
	void *v;
	void *cookie;
{
	struct cfb_softc *sc = v;

	if (sc->sc_dc == &cfb_console_dc)
		panic("cfb_free_screen: console");

	sc->nscreens--;
}

void
cfb_show_screen(v, cookie)
	void *v;
	void *cookie;
{
}

int
cfb_load_font(v, cookie, first, num, stride, data)
	void *v;
	void *cookie;
	int first, num, stride;
	void *data;
{
	return (EINVAL);
}

int
cfb_cnattach(addr)
        tc_addr_t addr;
{
        struct fb_devconfig *dcp = &cfb_console_dc;
        long defattr;

        cfb_getdevconfig(addr, dcp);
 
        rcons_alloc_attr(&dcp->dc_rcons, 0, 0, 0, &defattr);

        wsdisplay_cnattach(&cfb_stdscreen, &dcp->dc_rcons,
                           0, 0, defattr);
        cfb_consaddr = addr;
        return(0);
}


int
cfbintr(arg)
	void *arg;
{
	struct cfb_softc *sc = arg;
	caddr_t cfbbase = (caddr_t)sc->sc_dc->dc_vaddr;
	struct bt459reg *vdac;
	int v;
	
	*(u_int8_t *)(cfbbase + CFB_OFFSET_IREQ) = 0;
	if (sc->sc_changed == 0)
		return (1);

	vdac = (void *)(cfbbase + CFB_BT459_OFFSET);
	v = sc->sc_changed;
	sc->sc_changed = 0;
	if (v & DATA_ENB_CHANGED) {
		BT459_SELECT(vdac, BT459_REG_CCR);
		vdac->bt_reg = (sc->sc_curenb) ? 0xc0 : 0x00;
	}
	if (v & DATA_CURCMAP_CHANGED) {
		u_int8_t *cp = sc->sc_cursor.cc_color;

		BT459_SELECT(vdac, BT459_REG_CCOLOR_2);
		vdac->bt_cmap = cp[1];	tc_wmb();
		vdac->bt_cmap = cp[3];	tc_wmb();
		vdac->bt_cmap = cp[5];	tc_wmb();

		BT459_SELECT(vdac, BT459_REG_CCOLOR_3);
		vdac->bt_cmap = cp[0];	tc_wmb();
		vdac->bt_cmap = cp[2];	tc_wmb();
		vdac->bt_cmap = cp[4];	tc_wmb();
	}
	if (v & DATA_CURSHAPE_CHANGED) {
		u_int8_t *bp;
		int i;

		bp = (u_int8_t *)&sc->sc_cursor.cc_image;
		BT459_SELECT(vdac, BT459_REG_CRAM_BASE);
		for (i = 0; i < sizeof(sc->sc_cursor.cc_image); i++) {
			vdac->bt_reg = *bp++;
			tc_wmb();
		}
	}
	if (v & DATA_CMAP_CHANGED) {
		struct hwcmap *cm = &sc->sc_cmap;
		int index;

		BT459_SELECT(vdac, 0);
		for (index = 0; index < CMAP_SIZE; index++) {
			vdac->bt_cmap = cm->r[index];
			vdac->bt_cmap = cm->g[index];
			vdac->bt_cmap = cm->b[index];
		}
	}
	return (1);
}

void
cfbinit(dc)
	struct fb_devconfig *dc;
{
	caddr_t cfbbase = (caddr_t)dc->dc_vaddr;
	struct bt459reg *vdac = (void *)(cfbbase + CFB_BT459_OFFSET);
	int i;

	BT459_SELECT(vdac, 0);
	vdac->bt_cmap = 0;	tc_wmb();
	vdac->bt_cmap = 0;	tc_wmb();
	vdac->bt_cmap = 0;	tc_wmb();
	for (i = 1; i < CMAP_SIZE; i++) {
		vdac->bt_cmap = 0xff;	tc_wmb();
		vdac->bt_cmap = 0xff;	tc_wmb();
		vdac->bt_cmap = 0xff;	tc_wmb();
	}

	BT459_SELECT(vdac, BT459_REG_COMMAND_0);
	vdac->bt_reg = 0x40; /* CMD0 */	tc_wmb();
	vdac->bt_reg = 0x0;  /* CMD1 */	tc_wmb();
	vdac->bt_reg = 0xc0; /* CMD2 */	tc_wmb();
	vdac->bt_reg = 0xff; /* PRM */	tc_wmb();
	vdac->bt_reg = 0;    /* 205 */	tc_wmb();
	vdac->bt_reg = 0x0;  /* PBM */	tc_wmb();
	vdac->bt_reg = 0;    /* 207 */	tc_wmb();
	vdac->bt_reg = 0x0;  /* ORM */	tc_wmb();
	vdac->bt_reg = 0x0;  /* OBM */	tc_wmb();
	vdac->bt_reg = 0x0;  /* ILV */	tc_wmb();
	vdac->bt_reg = 0x0;  /* TEST */	tc_wmb();

	BT459_SELECT(vdac, BT459_REG_CCR);
	vdac->bt_reg = 0x0;	tc_wmb();
	vdac->bt_reg = 0x0;	tc_wmb();
	vdac->bt_reg = 0x0;	tc_wmb();
	vdac->bt_reg = 0x0;	tc_wmb();
	vdac->bt_reg = 0x0;	tc_wmb();
	vdac->bt_reg = 0x0;	tc_wmb();
	vdac->bt_reg = 0x0;	tc_wmb();
	vdac->bt_reg = 0x0;	tc_wmb();
	vdac->bt_reg = 0x0;	tc_wmb();
	vdac->bt_reg = 0x0;	tc_wmb();
	vdac->bt_reg = 0x0;	tc_wmb();
	vdac->bt_reg = 0x0;	tc_wmb();
	vdac->bt_reg = 0x0;	tc_wmb();
}

static int
get_cmap(sc, p)
	struct cfb_softc *sc;
	struct wsdisplay_cmap *p;
{
	u_int index = p->index, count = p->count;

	if (index >= CMAP_SIZE || (index + count) > CMAP_SIZE)
		return (EINVAL);

	if (!useracc(p->red, count, B_WRITE) ||
	    !useracc(p->green, count, B_WRITE) ||
	    !useracc(p->blue, count, B_WRITE))
		return (EFAULT);

	copyout(&sc->sc_cmap.r[index], p->red, count);
	copyout(&sc->sc_cmap.g[index], p->green, count);
	copyout(&sc->sc_cmap.b[index], p->blue, count);

	return (0);
}

static int
set_cmap(sc, p)
	struct cfb_softc *sc;
	struct wsdisplay_cmap *p;
{
	u_int index = p->index, count = p->count;

	if (index >= CMAP_SIZE || (index + count) > CMAP_SIZE)
		return (EINVAL);

	if (!useracc(p->red, count, B_READ) ||
	    !useracc(p->green, count, B_READ) ||
	    !useracc(p->blue, count, B_READ))
		return (EFAULT);

	copyin(p->red, &sc->sc_cmap.r[index], count);
	copyin(p->green, &sc->sc_cmap.g[index], count);
	copyin(p->blue, &sc->sc_cmap.b[index], count);

	sc->sc_changed |= DATA_CMAP_CHANGED;

	return (0);
}

static int
set_cursor(sc, p)
	struct cfb_softc *sc;
	struct wsdisplay_cursor *p;
{
#define	cc (&sc->sc_cursor)
	int v, index, count;

	v = p->which;
	if (v & WSDISPLAY_CURSOR_DOCMAP) {
		index = p->cmap.index;
		count = p->cmap.count;
		if (index >= 2 || (index + count) > 2)
			return (EINVAL);
		if (!useracc(p->cmap.red, count, B_READ) ||
		    !useracc(p->cmap.green, count, B_READ) ||
		    !useracc(p->cmap.blue, count, B_READ))
			return (EFAULT);
	}
	if (v & WSDISPLAY_CURSOR_DOSHAPE) {
		if (p->size.x > CURSOR_MAX_SIZE || p->size.y > CURSOR_MAX_SIZE)
			return (EINVAL);
		count = (CURSOR_MAX_SIZE / NBBY) * p->size.y;
		if (!useracc(p->image, count, B_READ) ||
		    !useracc(p->mask, count, B_READ))
			return (EFAULT);
	}
	if (v & (WSDISPLAY_CURSOR_DOPOS | WSDISPLAY_CURSOR_DOCUR)) {
		if (v & WSDISPLAY_CURSOR_DOCUR)
			cc->cc_hot = p->hot;
		if (v & WSDISPLAY_CURSOR_DOPOS)
			set_curpos(sc, &p->pos);
		bt459_set_curpos(sc);
	}

	sc->sc_changed = 0;
	if (v & WSDISPLAY_CURSOR_DOCUR) {
		sc->sc_curenb = p->enable;
		sc->sc_changed |= DATA_ENB_CHANGED;
	}
	if (v & WSDISPLAY_CURSOR_DOCMAP) {
		count = p->cmap.count;
		copyin(p->cmap.red, &cc->cc_color[index], count);
		copyin(p->cmap.green, &cc->cc_color[index + 2], count);
		copyin(p->cmap.blue, &cc->cc_color[index + 4], count);
		sc->sc_changed |= DATA_CURCMAP_CHANGED;
	}
	if (v & WSDISPLAY_CURSOR_DOSHAPE) {
		cc->cc_size = p->size;
		memset(cc->cc_image, 0, sizeof cc->cc_image);
		copyin(p->image, (caddr_t)cc->cc_image, count);
		copyin(p->mask, (caddr_t)(cc->cc_image+CURSOR_MAX_SIZE), count);
		sc->sc_changed |= DATA_CURSHAPE_CHANGED;
	}

	return (0);
#undef cc
}

static int
get_cursor(sc, p)
	struct cfb_softc *sc;
	struct wsdisplay_cursor *p;
{
	return (ENOTTY); /* XXX */
}

static void
set_curpos(sc, curpos)
	struct cfb_softc *sc;
	struct wsdisplay_curpos *curpos;
{
	struct fb_devconfig *dc = sc->sc_dc;
	int x = curpos->x, y = curpos->y;

	if (y < 0)
		y = 0;
	else if (y > dc->dc_ht - sc->sc_cursor.cc_size.y - 1)
		y = dc->dc_ht - sc->sc_cursor.cc_size.y - 1;	
	if (x < 0)
		x = 0;
	else if (x > dc->dc_wid - sc->sc_cursor.cc_size.x - 1)
		x = dc->dc_wid - sc->sc_cursor.cc_size.x - 1;
	sc->sc_cursor.cc_pos.x = x;
	sc->sc_cursor.cc_pos.y = y;
}

void
bt459_set_curpos(sc)
	struct cfb_softc *sc;
{
	caddr_t cfbbase = (caddr_t)sc->sc_dc->dc_vaddr;
	struct bt459reg *vdac = (void *)(cfbbase + CFB_BT459_OFFSET);
	int x = 220, y = 35; /* magic offset of CX coordinate */
	int s;

	x += sc->sc_cursor.cc_pos.x;
	y += sc->sc_cursor.cc_pos.y;

	s = spltty();

	BT459_SELECT(vdac, BT459_REG_CURSOR_X_LOW);
	vdac->bt_reg = x;	tc_wmb();
	vdac->bt_reg = x >> 8;	tc_wmb();
	vdac->bt_reg = y;	tc_wmb();
	vdac->bt_reg = y >> 8;	tc_wmb();

	splx(s);
}
