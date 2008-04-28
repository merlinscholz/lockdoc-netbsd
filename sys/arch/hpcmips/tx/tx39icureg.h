/*	$NetBSD: tx39icureg.h,v 1.5 2008/04/28 20:23:21 martin Exp $ */

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
 *  TOSHIBA TMPR3912/3922 interrupt module.
 */
#ifdef TX391X
#define TX39_INTRSET_MAX	5
#endif /* TX391X */
#ifdef TX392X
#define TX39_INTRSET_MAX	8
#endif /* TX391X */

#define TX39_IRQHIGH_MAX	16
/* R */
#define	TX39_INTRSTATUS1_REG	0x100
#define	TX39_INTRSTATUS2_REG	0x104
#define	TX39_INTRSTATUS3_REG	0x108
#define	TX39_INTRSTATUS4_REG	0x10c
#define	TX39_INTRSTATUS5_REG	0x110
#define	TX39_INTRSTATUS6_REG	0x114
#ifdef TX392X
#define	TX39_INTRSTATUS7_REG	0x130
#define TX39_INTRSTATUS8_REG	0x138
#endif /* TX392X */
#ifdef TX391X
#define TX39_INTRSTATUS_REG(x)	(((x) - 1) * 4 + TX39_INTRSTATUS1_REG)
#endif /* TX391X */
#ifdef TX392X
#define TX39_INTRSTATUS_REG(x)	(((x) <= 6) ?				\
	(((x) - 1) * 4 + TX39_INTRSTATUS1_REG) :			\
	(((x) - 7) * 8 + TX39_INTRSTATUS7_REG))
#endif /* TX392X */

/* W */
#define	TX39_INTRCLEAR1_REG	0x100
#define	TX39_INTRCLEAR2_REG	0x104
#define	TX39_INTRCLEAR3_REG	0x108
#define	TX39_INTRCLEAR4_REG	0x10c
#define	TX39_INTRCLEAR5_REG	0x110
#ifdef TX392X
#define	TX39_INTRCLEAR7_REG	0x130
#define TX39_INTRCLEAR8_REG	0x138
#endif /* TX392X */
#ifdef TX391X
#define TX39_INTRCLEAR_REG(x)	(((x) - 1) * 4 + TX39_INTRCLEAR1_REG)
#endif /* TX391X */
#ifdef TX392X
#define TX39_INTRCLEAR_REG(x)	(((x) <= 6) ?				\
	(((x) - 1) * 4 + TX39_INTRCLEAR1_REG) :				\
	(((x) - 7) * 8 + TX39_INTRCLEAR7_REG))
#endif /* TX392X */

/* R/W */
#define	TX39_INTRENABLE1_REG	0x118
#define	TX39_INTRENABLE2_REG	0x11c
#define	TX39_INTRENABLE3_REG	0x120
#define	TX39_INTRENABLE4_REG	0x124
#define	TX39_INTRENABLE5_REG	0x128
#define	TX39_INTRENABLE6_REG	0x12c
#ifdef TX392X
#define	TX39_INTRENABLE7_REG	0x134
#define	TX39_INTRENABLE8_REG	0x13c
#endif /* TX392X */
#ifdef TX391X
#define TX39_INTRENABLE_REG(x)	(((x) - 1) * 4 + TX39_INTRENABLE1_REG)
#endif /* TX391X */
#ifdef TX392X
#define TX39_INTRENABLE_REG(x)	(((x) <= 6) ?				\
	(((x) - 1) * 4 + TX39_INTRENABLE1_REG) :			\
	(((x) - 7) * 8 + TX39_INTRENABLE7_REG))
#endif /* TX392X */
/*
 *	IRQLOW
 */
/*
 *	Interrupt status/clear 1 register.
 *		  -> Enable 1 register
 */
/* R/W */
#ifdef TX391X
#define	TX39_INTRSTATUS1_LCDINT		0x80000000
#define TX39_INTRSTATUS1_DFINT		0x40000000
#endif /* TX391X */
#define TX39_INTRSTATUS1_CHI0_5INT	0x20000000
#define TX39_INTRSTATUS1_CHI1_0INT	0x10000000
#define TX39_INTRSTATUS1_CHIDMACNTINT	0x08000000
#define TX39_INTRSTATUS1_CHININTA	0x04000000
#define TX39_INTRSTATUS1_CHININTB	0x02000000
#define TX39_INTRSTATUS1_CHIACTINT	0x01000000
#define TX39_INTRSTATUS1_CHIERRINT	0x00800000
#define TX39_INTRSTATUS1_SND0_5INT	0x00400000
#define TX39_INTRSTATUS1_SND1_0INT	0x00200000
#define TX39_INTRSTATUS1_TEL0_5INT	0x00100000
#define TX39_INTRSTATUS1_TEL1_0INT	0x00080000
#define TX39_INTRSTATUS1_SNDDMACNTINT	0x00040000
#define TX39_INTRSTATUS1_TELDMACNTINT	0x00020000
#define TX39_INTRSTATUS1_LSNDCLIPINT	0x00010000
#define TX39_INTRSTATUS1_RSNDCLIPINT	0x00008000
#define TX39_INTRSTATUS1_VALSNDPOSINT	0x00004000
#define TX39_INTRSTATUS1_VALSNDNEGINT	0x00002000
#define TX39_INTRSTATUS1_VALTELPOSINT	0x00001000
#define TX39_INTRSTATUS1_VALTELNEGINT	0x00000800
#define TX39_INTRSTATUS1_SNDININT	0x00000400
#define TX39_INTRSTATUS1_TELININT	0x00000200
#define TX39_INTRSTATUS1_SIBSF0INT	0x00000100
#define TX39_INTRSTATUS1_SIBSF1INT	0x00000080
#define TX39_INTRSTATUS1_SIBIRQPOSINT	0x00000040
#define TX39_INTRSTATUS1_SIBIRQNEGINT	0x00000020

#ifdef TX391X
#define TX39_INTRSTATUS1_VIDEO		0xc0000000
#endif /* TX391X */
#define TX39_INTRSTATUS1_CHI		0x3f800000
#define TX39_INTRSTATUS1_SND		0x007ffe00
#define TX39_INTRSTATUS1_SIB		0x000001e0

/*
 *	Interrupt status/clear 2 register.
 *		  -> Enable 2 register
 */
/* R/W */
#define	TX39_INTRSTATUS2_UARTARXINT		0x80000000
#define TX39_INTRSTATUS2_UARTARXOVERRUNINT	0x40000000
#define TX39_INTRSTATUS2_UARTAFRAMEERRINT	0x20000000
#define TX39_INTRSTATUS2_UARTABREAKINT		0x10000000
#define TX39_INTRSTATUS2_UARTAPARITYERRINT	0x08000000
#define TX39_INTRSTATUS2_UARTATXINT		0x04000000
#define TX39_INTRSTATUS2_UARTATXOVERRUNINT	0x02000000
#define TX39_INTRSTATUS2_UARTAEMPTYINT		0x01000000
#define TX39_INTRSTATUS2_UARTADMAFULLINT	0x00800000
#define TX39_INTRSTATUS2_UARTADMAHALFINT	0x00400000

#define TX39_INTRSTATUS2_UARTBRXINT		0x00200000
#define TX39_INTRSTATUS2_UARTBRXOVERRUNINT	0x00100000
#define TX39_INTRSTATUS2_UARTBFRAMEERRINT	0x00080000
#define TX39_INTRSTATUS2_UARTBBREAKINT		0x00040000
#define TX39_INTRSTATUS2_UARTBPARITYERRINT	0x00020000
#define TX39_INTRSTATUS2_UARTBTXINT		0x00010000
#define TX39_INTRSTATUS2_UARTBTXOVERRUNINT	0x00008000
#define TX39_INTRSTATUS2_UARTBEMPTYINT		0x00004000
#define TX39_INTRSTATUS2_UARTBDMAFULLINT	0x00002000
#define TX39_INTRSTATUS2_UARTBDMAHALFINT	0x00001000

#define	TX39_INTRSTATUS2_UARTRXINT(x)					\
	((x) ? TX39_INTRSTATUS2_UARTBRXINT :				\
	 TX39_INTRSTATUS2_UARTARXINT)
#define TX39_INTRSTATUS2_UARTRXOVERRUNINT(x)				\
	((x) ? TX39_INTRSTATUS2_UARTBRXOVERRUNINT :			\
	 TX39_INTRSTATUS2_UARTARXOVERRUNINT)
#define TX39_INTRSTATUS2_UARTFRAMEERRINT(x)				\
	((x) ? TX39_INTRSTATUS2_UARTBFRAMEERRINT :			\
	 TX39_INTRSTATUS2_UARTAFRAMEERRINT)
#define TX39_INTRSTATUS2_UARTBREAKINT(x)				\
	((x) ? TX39_INTRSTATUS2_UARTBBREAKINT :				\
	TX39_INTRSTATUS2_UARTABREAKINT)
#define TX39_INTRSTATUS2_UARTPARITYERRINT(x)				\
	((x) ? TX39_INTRSTATUS2_UARTBPARITYERRINT :			\
	 TX39_INTRSTATUS2_UARTAPARITYERRINT)
#define TX39_INTRSTATUS2_UARTTXINT(x)					\
	((x) ? TX39_INTRSTATUS2_UARTBTXINT :				\
	TX39_INTRSTATUS2_UARTATXINT)
#define TX39_INTRSTATUS2_UARTTXOVERRUNINT(x)				\
	((x) ? TX39_INTRSTATUS2_UARTBTXOVERRUNINT :			\
	TX39_INTRSTATUS2_UARTATXOVERRUNINT)
#define TX39_INTRSTATUS2_UARTEMPTYINT(x)				\
	((x) ? TX39_INTRSTATUS2_UARTBEMPTYINT :				\
	TX39_INTRSTATUS2_UARTEMPTYINT)
#define TX39_INTRSTATUS2_UARTDMAFULLINT(x)				\
	((x) ? TX39_INTRSTATUS2_UARTBDMAFULLINT :			\
	TX39_INTRSTATUS2_UARTADMAFULLINT)
#define TX39_INTRSTATUS2_UARTDMAHALFINT(x)				\
	((x) ? TX39_INTRSTATUS2_UARTBDMAHALFINT :			\
	TX39_INTRSTATUS2_UARTADMAHALFINT)

#ifdef TX391X
#define TX39_INTRSTATUS2_MBUSTXBUFAVAILINT	0x00000800
#define TX39_INTRSTATUS2_MBUSTXERRINT		0x00000400
#define TX39_INTRSTATUS2_MBUSEMPTYINT		0x00000200
#define TX39_INTRSTATUS2_MBUSRXBUFAVAILINT	0x00000100
#define TX39_INTRSTATUS2_MBUSRXERRINT		0x00000080
#define TX39_INTRSTATUS2_MBUSDETINT		0x00000040
#define TX39_INTRSTATUS2_MBUSDMAFULLINT		0x00000020
#define TX39_INTRSTATUS2_MBUSDMAHALFINT		0x00000010
#define TX39_INTRSTATUS2_MBUSPOSINT		0x00000008
#define TX39_INTRSTATUS2_MBUSNEGINT		0x00000004
#endif /* TX391X */

#define TX39_INTRSTATUS2_UARTA			0xffc00000
#define TX39_INTRSTATUS2_UARTB			0x003ff000
#ifdef TX391X
#define TX39_INTRSTATUS2_MBUS			0x00000ffc
#endif /* TX391X */
/*
 *	Interrupt status/clear 3 register. (Multifunction I/O pin)
 *		  -> Enable 3 register
 */
/* R/W */
#define TX39_INTRSTATUS3_MFIOPOSINT(r)	((r) << 1)

#define TX39_INTRSTATUS3_CHIFSPOSINT		0x80000000
#define TX39_INTRSTATUS3_CHICLKPOSINT		0x40000000
#define TX39_INTRSTATUS3_CHIDOUTPOSINT		0x20000000
#define TX39_INTRSTATUS3_CHIDINPOSINT		0x10000000
#define TX39_INTRSTATUS3_DREQPOSINT		0x08000000
#define TX39_INTRSTATUS3_DGRINTPOSINT		0x04000000
#define TX39_INTRSTATUS3_BC32KPOSINT		0x02000000
#define TX39_INTRSTATUS3_TXDPOSINT		0x01000000
#define TX39_INTRSTATUS3_RXDPOSINT		0x00800000
#define TX39_INTRSTATUS3_CS1POSINT		0x00400000
#define TX39_INTRSTATUS3_CS2POSINT		0x00200000
#define TX39_INTRSTATUS3_CS3POSINT		0x00100000
#define TX39_INTRSTATUS3_MCS0POSINT		0x00080000
#define TX39_INTRSTATUS3_MCS1POSINT		0x00040000
#define TX39_INTRSTATUS3_MCS2POSINT		0x00020000
#define TX39_INTRSTATUS3_MCS3POSINT		0x00010000
#define TX39_INTRSTATUS3_SPICLKPOSINT		0x00008000
#define TX39_INTRSTATUS3_SPIOUTPOSINT		0x00004000
#define TX39_INTRSTATUS3_SPINPOSINT		0x00002000
#define TX39_INTRSTATUS3_SIBMCLKPOSINT		0x00001000
#define TX39_INTRSTATUS3_CARDREGPOSINT		0x00000800
#define TX39_INTRSTATUS3_CARDIOWRPOSINT		0x00000400
#define TX39_INTRSTATUS3_CARDIORDPOSINT		0x00000200
#define TX39_INTRSTATUS3_CARD1CSLPOSINT		0x00000100
#define TX39_INTRSTATUS3_CARD1CSHPOSINT		0x00000080
#define TX39_INTRSTATUS3_CARD2CSLPOSINT		0x00000040
#define TX39_INTRSTATUS3_CARD2CSHPOSINT		0x00000020
#define TX39_INTRSTATUS3_CARD1WAITPOSINT	0x00000010
#define TX39_INTRSTATUS3_CARD2WAITPOSINT	0x00000008
#define TX39_INTRSTATUS3_CARDDIRPOSINT		0x00000004

/*
 *	Interrupt status/clear 4 register. (Multifunction I/O pin)
 *		  -> Enable 4 register
 */
/* R/W */
#define TX39_INTRSTATUS4_MFIONEGINT(r)	((r) << 1)

#define TX39_INTRSTATUS4_CHIFSNEGINT		0x80000000
#define TX39_INTRSTATUS4_CHICLKNEGINT		0x40000000
#define TX39_INTRSTATUS4_CHIDOUTNEGINT		0x20000000
#define TX39_INTRSTATUS4_CHIDINNEGINT		0x10000000
#define TX39_INTRSTATUS4_DREQNEGINT		0x08000000
#define TX39_INTRSTATUS4_DGRINTNEGINT		0x04000000
#define TX39_INTRSTATUS4_BC32KNEGINT		0x02000000
#define TX39_INTRSTATUS4_TXDNEGINT		0x01000000
#define TX39_INTRSTATUS4_RXDNEGINT		0x00800000
#define TX39_INTRSTATUS4_CS1NEGINT		0x00400000
#define TX39_INTRSTATUS4_CS2NEGINT		0x00200000
#define TX39_INTRSTATUS4_CS3NEGINT		0x00100000
#define TX39_INTRSTATUS4_MCS0NEGINT		0x00080000
#define TX39_INTRSTATUS4_MCS1NEGINT		0x00040000
#define TX39_INTRSTATUS4_MCS2NEGINT		0x00020000
#define TX39_INTRSTATUS4_MCS3NEGINT		0x00010000
#define TX39_INTRSTATUS4_SPICLKNEGINT		0x00008000
#define TX39_INTRSTATUS4_SPIOUTNEGINT		0x00004000
#define TX39_INTRSTATUS4_SPINNEGINT		0x00002000
#define TX39_INTRSTATUS4_SIBMCLKNEGINT		0x00001000
#define TX39_INTRSTATUS4_CARDREGNEGINT		0x00000800
#define TX39_INTRSTATUS4_CARDIOWRNEGINT		0x00000400
#define TX39_INTRSTATUS4_CARDIORDNEGINT		0x00000200
#define TX39_INTRSTATUS4_CARD1CSLNEGINT		0x00000100
#define TX39_INTRSTATUS4_CARD1CSHNEGINT		0x00000080
#define TX39_INTRSTATUS4_CARD2CSLNEGINT		0x00000040
#define TX39_INTRSTATUS4_CARD2CSHNEGINT		0x00000020
#define TX39_INTRSTATUS4_CARD1WAITNEGINT	0x00000010
#define TX39_INTRSTATUS4_CARD2WAITNEGINT	0x00000008
#define TX39_INTRSTATUS4_CARDDIRNEGINT		0x00000004

/*
 *	Interrupt status/clear 5 register.
 *		  -> Enable 5 register
 */
/* R/W */
#define	TX39_INTRSTATUS5_RTCINT		0x80000000
#define TX39_INTRSTATUS5_ALARMINT	0x40000000
#define TX39_INTRSTATUS5_PERINT		0x20000000
#define TX39_INTRSTATUS5_STPTIMERINT	0x10000000
#define TX39_INTRSTATUS5_POSPWRINT	0x08000000
#define TX39_INTRSTATUS5_NEGPWRINT	0x04000000
#define TX39_INTRSTATUS5_POSPWROKINT	0x02000000
#define TX39_INTRSTATUS5_NEGPWROKINT	0x01000000
#define TX39_INTRSTATUS5_POSONBUTNINT	0x00800000
#define TX39_INTRSTATUS5_NEGONBUTNINT	0x00400000
#define TX39_INTRSTATUS5_SPIBUFAVAILINT	0x00200000
#define TX39_INTRSTATUS5_SPIERRINT	0x00100000
#define TX39_INTRSTATUS5_SPIRCVINT	0x00080000
#define TX39_INTRSTATUS5_SPIEMPTYINT	0x00040000
#define TX39_INTRSTATUS5_IRCONSMINT	0x00020000
#define TX39_INTRSTATUS5_CARSTINT	0x00010000
#define TX39_INTRSTATUS5_POSCARINT	0x00008000
#define TX39_INTRSTATUS5_NEGCARINT	0x00004000
#ifdef TX391X
#define TX39_INTRSTATUS5_IOPOSINT6	0x00002000
#define TX39_INTRSTATUS5_IOPOSINT5	0x00001000
#define TX39_INTRSTATUS5_IOPOSINT4	0x00000800
#define TX39_INTRSTATUS5_IOPOSINT3	0x00000400
#define TX39_INTRSTATUS5_IOPOSINT2	0x00000200
#define TX39_INTRSTATUS5_IOPOSINT1	0x00000100
#define TX39_INTRSTATUS5_IOPOSINT0	0x00000080
#define TX39_INTRSTATUS5_IONEGINT6	0x00000040
#define TX39_INTRSTATUS5_IONEGINT5	0x00000020
#define TX39_INTRSTATUS5_IONEGINT4	0x00000010
#define TX39_INTRSTATUS5_IONEGINT3	0x00000008
#define TX39_INTRSTATUS5_IONEGINT2	0x00000004
#define TX39_INTRSTATUS5_IONEGINT1	0x00000002
#define TX39_INTRSTATUS5_IONEGINT0	0x00000001
#endif /* TX391X */

#define TX39_INTRSTATUS5_TIMER		0xe0000000
#define TX39_INTRSTATUS5_POWER		0x1fc00000
#define TX39_INTRSTATUS5_SPI		0x003c0000
#define TX39_INTRSTATUS5_IR		0x0003c000
#ifdef TX391X
#define TX39_INTRSTATUS5_IO		0x00003fff

#define TX39_INTRSTATUS5_IOPOSINT_SHIFT 7
#define TX39_INTRSTATUS5_IOPOSINT_MASK	0x7f
#define TX39_INTRSTATUS5_IOPOSINT(cr)					\
	(((cr) >> TX39_INTRSTATUS5_IOPOSINT_SHIFT) &			\
	TX39_INTRSTATUS5_IOPOSINT_MASK)
#define TX39_INTRSTATUS5_IOPOSINT_SET(cr, val)				\
	((cr) | (((val) << TX39_INTRSTATUS5_IOPOSINT_SHIFT) &		\
	(TX39_INTRSTATUS5_IOPOSINT_MASK << TX39_INTRSTATUS5_IOPOSINT_SHIFT)))

#define TX39_INTRSTATUS5_IONEGINT_SHIFT 0
#define TX39_INTRSTATUS5_IONEGINT_MASK	0x7f
#define TX39_INTRSTATUS5_IONEGINT(cr)					\
	(((cr) >> TX39_INTRSTATUS5_IONEGINT_SHIFT) &			\
	TX39_INTRSTATUS5_IONEGINT_MASK)
#define TX39_INTRSTATUS5_IONEGINT_SET(cr, val)				\
	((cr) | (((val) << TX39_INTRSTATUS5_IONEGINT_SHIFT) &		\
	(TX39_INTRSTATUS5_IONEGINT_MASK << TX39_INTRSTATUS5_IONEGINT_SHIFT)))
#endif /* TX391X */
/*
 *	Interrupt status 6 register.
 */
/* R */
#define	TX39_INTRSTATUS6_IRQHIGH	0x80000000
#define TX39_INTRSTATUS6_IRQLOW		0x40000000

#define TX39_INTRSTATUS6_INTVECT_SHIFT	2
#define TX39_INTRSTATUS6_INTVECT_MASK	0xf
#define TX39_INTRSTATUS6_INTVECT(cr)					\
	(((cr) >> TX39_INTRSTATUS6_INTVECT_SHIFT) &			\
	TX39_INTRSTATUS6_INTVECT_MASK)

/*
 *	Interrupt enable 6 register.
 */
/* R/W */
#define TX39_INTRENABLE6_GLOBALEN	0x00040000

#define TX39_INTRENABLE6_PRIORITYMASK_SHIFT	0
#define TX39_INTRENABLE6_PRIORITYMASK_MASK	0xffff
#define TX39_INTRENABLE6_PRIORITYMASK(cr)				\
	(((cr) >> TX39_INTRENABLE6_PRIORITYMASK_SHIFT) &		\
	TX39_INTRENABLE6_PRIORITYMASK_MASK)
#define TX39_INTRENABLE6_PRIORITYMASK_SET(cr, val)			\
	((cr) | (((val) << TX39_INTRENABLE6_PRIORITYMASK_SHIFT) &	\
	(TX39_INTRENABLE6_PRIORITYMASK_MASK <<				\
	TX39_INTRENABLE6_PRIORITYMASK_SHIFT)))

#ifdef TX392X
/*
 *	Interrupt Status 7 Register
 */
#define TX3922_INTRSTATUS7_IRTXCINT		0x00100000
#define TX3922_INTRSTATUS7_IRRXCINT		0x00080000
#define TX3922_INTRSTATUS7_IRTXEINT		0x00040000
#define TX3922_INTRSTATUS7_IRRXEINT		0x00020000
#define TX3922_INTRSTATUS7_IRSIRPXINT		0x00010000

/*
 *	Interrupt Status 8 Register
 */
#define TX39_INTRSTATUS8_IOPOSINT15	0x80000000
#define TX39_INTRSTATUS8_IOPOSINT14	0x40000000
#define TX39_INTRSTATUS8_IOPOSINT13	0x20000000
#define TX39_INTRSTATUS8_IOPOSINT12	0x10000000
#define TX39_INTRSTATUS8_IOPOSINT11	0x08000000
#define TX39_INTRSTATUS8_IOPOSINT10	0x04000000
#define TX39_INTRSTATUS8_IOPOSINT9	0x02000000
#define TX39_INTRSTATUS8_IOPOSINT8	0x01000000
#define TX39_INTRSTATUS8_IOPOSINT7	0x00800000
#define TX39_INTRSTATUS8_IOPOSINT6	0x00400000
#define TX39_INTRSTATUS8_IOPOSINT5	0x00200000
#define TX39_INTRSTATUS8_IOPOSINT4	0x00100000
#define TX39_INTRSTATUS8_IOPOSINT3	0x00080000
#define TX39_INTRSTATUS8_IOPOSINT2	0x00040000
#define TX39_INTRSTATUS8_IOPOSINT1	0x00020000
#define TX39_INTRSTATUS8_IOPOSINT0	0x00010000
#define TX39_INTRSTATUS8_IONEGINT15	0x00008000
#define TX39_INTRSTATUS8_IONEGINT14	0x00004000
#define TX39_INTRSTATUS8_IONEGINT13	0x00002000
#define TX39_INTRSTATUS8_IONEGINT12	0x00001000
#define TX39_INTRSTATUS8_IONEGINT11	0x00000800
#define TX39_INTRSTATUS8_IONEGINT10	0x00000400
#define TX39_INTRSTATUS8_IONEGINT9	0x00000200
#define TX39_INTRSTATUS8_IONEGINT8	0x00000100
#define TX39_INTRSTATUS8_IONEGINT7	0x00000080
#define TX39_INTRSTATUS8_IONEGINT6	0x00000040
#define TX39_INTRSTATUS8_IONEGINT5	0x00000020
#define TX39_INTRSTATUS8_IONEGINT4	0x00000010
#define TX39_INTRSTATUS8_IONEGINT3	0x00000008
#define TX39_INTRSTATUS8_IONEGINT2	0x00000004
#define TX39_INTRSTATUS8_IONEGINT1	0x00000002
#define TX39_INTRSTATUS8_IONEGINT0	0x00000001

#define TX3922_INTRSTATUS8_IOPOSINT_SHIFT	16
#define TX3922_INTRSTATUS8_IOPOSINT_MASK	0xffff
#define TX3922_INTRSTATUS8_IOPOSINT(cr)					\
	(((cr) >> TX3922_INTRSTATUS8_IOPOSINT_SHIFT) &			\
	TX3922_INTRSTATUS8_IOPOSINT_MASK)
#define TX3922_INTRSTATUS8_IOPOSINT_SET(cr, val)			\
	((cr) | (((val) << TX3922_INTRSTATUS8_IOPOSINT_SHIFT) &		\
	(TX3922_INTRSTATUS8_IOPOSINT_MASK <<				\
	TX3922_INTRSTATUS8_IOPOSINT_SHIFT)))

#define TX3922_INTRSTATUS8_IONEGINT_SHIFT	0
#define TX3922_INTRSTATUS8_IONEGINT_MASK	0xffff
#define TX3922_INTRSTATUS8_IONEGINT(cr)					\
	(((cr) >> TX3922_INTRSTATUS8_IONEGINT_SHIFT) &			\
	TX3922_INTRSTATUS8_IONEGINT_MASK)
#define TX3922_INTRSTATUS8_IONEGINT_SET(cr, val)			\
	((cr) | (((val) << TX3922_INTRSTATUS8_IONEGINT_SHIFT) &		\
	(TX3922_INTRSTATUS8_IONEGINT_MASK <<				\
	TX3922_INTRSTATUS8_IONEGINT_SHIFT)))

#endif /* TX392X */

/*
 *	IRQHIGH (Priority level interrupt)
 */
#ifdef TX391X
#define TX39_INTRPRI15_PWROK_BIT		0x00008000	
#define TX39_INTRPRI14_TIMER_ALARM_BIT		0x00004000
#define TX39_INTRPRI13_TIMER_PERIODIC_BIT	0x00002000
#define TX39_INTRPRI12_MBUS_BIT			0x00001000
#define TX39_INTRPRI11_UARTARX_BIT		0x00000800
#define TX39_INTRPRI10_UARTBRX_BIT		0x00000400
#define TX39_INTRPRI9_MFIO19_18_17_16POS_BIT	0x00000200
#define TX39_INTRPRI8_MFIO1_0_IO6_5POS_BIT	0x00000100
#define TX39_INTRPRI7_MFIO19_18_17_16NEG_BIT	0x00000080
#define TX39_INTRPRI6_MFIO1_0_IO6_5NEG_BIT	0x00000040
#define TX39_INTRPRI5_MBUSDMAFULL_BIT		0x00000020
#define TX39_INTRPRI4_SNDDMACNT_BIT		0x00000010
#define TX39_INTRPRI3_TELDMACNT_BIT		0x00000008
#define TX39_INTRPRI2_CHIDMACNT_BIT		0x00000004
#define TX39_INTRPRI1_IO0POSNEG_BIT		0x00000002
#define TX39_INTRPRI0_BIT			0x00000001

#define TX39_INTRPRI15_PWROK			15
#define TX39_INTRPRI14_TIMER_ALARM		14
#define TX39_INTRPRI13_TIMER_PERIODIC		13
#define TX39_INTRPRI12_MBUS			12
#define TX39_INTRPRI11_UARTARX			11
#define TX39_INTRPRI10_UARTBRX			10
#define TX39_INTRPRI9_MFIO19_18_17_16POS	9
#define TX39_INTRPRI8_MFIO1_0_IO6_5POS		8
#define TX39_INTRPRI7_MFIO19_18_17_16NEG	7
#define TX39_INTRPRI6_MFIO1_0_IO6_5NEG		6
#define TX39_INTRPRI5_MBUSDMAFULL		5
#define TX39_INTRPRI4_SNDDMACNT			4
#define TX39_INTRPRI3_TELDMACNT			3
#define TX39_INTRPRI2_CHIDMACNT			2
#define TX39_INTRPRI1_IO0POSNEG			1
#define TX39_INTRPRI0				0
#endif /* TX391X */

#ifdef TX392X
#define TX39_INTRPRI15_PWROK_BIT		0x00008000	
#define TX39_INTRPRI14_TIMER_ALARM_BIT		0x00004000
#define TX39_INTRPRI13_TIMER_PERIODIC_BIT	0x00002000
#define TX39_INTRPRI12_UARTABRX_BIT		0x00001000
#define TX39_INTRPRI11_MFIO19_18_17_16POS_BIT	0x00000800
#define TX39_INTRPRI10_MFIO1_0_IO6_5POS_BIT	0x00000400
#define TX39_INTRPRI9_MFIO19_18_17_16NEG_BIT	0x00000200
#define TX39_INTRPRI8_MFIO1_0_IO6_5NEG_BIT	0x00000100
#define TX39_INTRPRI5_MBUSDMAFULL_BIT		0x00000020
#define TX39_INTRPRI4_SNDDMACNT_BIT		0x00000010
#define TX39_INTRPRI3_TELDMACNT_BIT		0x00000008
#define TX39_INTRPRI2_CHIDMACNT_BIT		0x00000004
#define TX39_INTRPRI1_IO0POSNEG_BIT		0x00000002
#define TX39_INTRPRI0_BIT			0x00000001

#define TX39_INTRPRI15_PWROK			15
#define TX39_INTRPRI14_TIMER_ALARM		14
#define TX39_INTRPRI13_TIMER_PERIODIC		13
#define TX39_INTRPRI12_UARTABRX			12
#define TX39_INTRPRI11_MFIO19_18_17_16POS	11
#define TX39_INTRPRI10_MFIO1_0_IO6_5POS		10
#define TX39_INTRPRI9_MFIO19_18_17_16NEG	9
#define TX39_INTRPRI8_MFIO1_0_IO6_5NEG		8
#define TX39_INTRPRI5_IRRXCRXE			5
#define TX39_INTRPRI4_SNDDMACNT			4
#define TX39_INTRPRI3_TELDMACNT			3
#define TX39_INTRPRI2_CHIDMACNT			2
#define TX39_INTRPRI1_IO0POSNEG			1
#define TX39_INTRPRI0				0
#endif /* TX392X */

/*
 *	CPU connection
 */
#define TX39_INTRIRQHIGH_MIPS_HARD_INT		4
#define TX39_INTRIRQLOW_MIPS_HARD_INT		2
