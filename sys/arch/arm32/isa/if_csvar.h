/*	$NetBSD: if_csvar.h,v 1.1 1998/06/08 17:49:43 tv Exp $	*/

/*
 * Copyright 1997
 * Digital Equipment Corporation. All rights reserved.
 *
 * This software is furnished under license and may be used and
 * copied only in accordance with the following terms and conditions.
 * Subject to these conditions, you may download, copy, install,
 * use, modify and distribute this software in source and/or binary
 * form. No title or ownership is transferred hereby.
 *
 * 1) Any source code used, modified or distributed must reproduce
 *    and retain this copyright notice and list of conditions as
 *    they appear in the source file.
 *
 * 2) No right is granted to use any trade name, trademark, or logo of
 *    Digital Equipment Corporation. Neither the "Digital Equipment
 *    Corporation" name nor any trademark or logo of Digital Equipment
 *    Corporation may be used to endorse or promote products derived
 *    from this software without the prior written permission of
 *    Digital Equipment Corporation.
 *
 * 3) This software is provided "AS-IS" and any express or implied
 *    warranties, including but not limited to, any implied warranties
 *    of merchantability, fitness for a particular purpose, or
 *    non-infringement are disclaimed. In no event shall DIGITAL be
 *    liable for any damages whatsoever, and in particular, DIGITAL
 *    shall not be liable for special, indirect, consequential, or
 *    incidental damages or damages for lost profits, loss of
 *    revenue or loss of use, whether such damages arise in contract,
 *    negligence, tort, under statute, in equity, at law or otherwise,
 *    even if advised of the possibility of such damage.
 */

/*
**++
**  FACILITY  Crystal CS8900 Ethernet driver header file
**
**  ABSTRACT
**
**     This module provides CS8900 register defines in addition
**	to defining the driver softc structure.
**
**  AUTHORS
**
**     Peter Dettori   SEA - Software Engineering.
**
**  CREATION DATE:
**
**       13-Feb-1997.
**
**  MODIFICATION HISTORY:
**
**--
*/

#ifndef __IF_CSVAR_H__
#define __IF_CSVAR_H__

extern struct cfdriver cs_cd;

/* Individual Address (Ethernet Address) */
typedef struct
{
   ushort word[3];
} ia, *pia;

/*
 * Ethernet software status per interface.
 *
 * Each interface is referenced by a network interface structure,
 * arpcom.ac_if, which the routing code uses to locate the interface.
 * This structure contains the output queue for the interface, 
 * its address, ...
 */
struct cs_softc {
	struct device sc_dev;		/* base device glue */
	struct ethercom sc_ethercom;	/* Ethernet common */

	void	*sc_ih;			/* interupt handler */
	void 	*sc_sh;			/* shutdown hook */
	bus_space_tag_t sc_iot;         /* bus space tag for IO */
	bus_space_tag_t sc_memt;        /* bus space tag for memory mode */
	bus_space_handle_t sc_ioh;      /* bus space handles */
	bus_space_handle_t sc_memh;
	ushort sc_iobase;               /* base IO port address */
   	ushort  sc_int;                 /* interrupt level */ 
	int pPacketPagePhys;            /* physical io address */
   	ushort  mediaType;              /* 10BaseT, thin wire */
   	ushort  configFlags;            /* chip configuration software flag */
   	int    inMemoryMode;		/* status */
   	int    txInProgress;		/* flags  */
   	int    resetting;		/* chip in reset mode flag */
	uint dmaMemSize;		/* 16K or 64K if DMA being used */
	char *dmaBase;			/* DMA memory base */
	char *dma_offset;		/* offset into DMA space */
	int sc_drq;			/* DMA request channel : 5,6,7 */

	int	sc_xe_ent;		/* current early-xmit table entry */
	int	sc_xe_togo;		/* # of packets to go at this ent */

	u_int8_t sc_enaddr[6];		/* MAC address */
};

/* Debug Status */
/*#define CS_DEBUG 0*/

#ifdef CS_DEBUG
#define dbg_print(x) printf x
#else
#define dbg_print(x) 
#endif

/* Return Status */
#define	CS_ERROR   -1
#define CS_OK       1
		
/* Hardcoded CS8900 specific configuration information */
#define CS8900_IOSIZE 16  /* CS8900 has 8 2byte IO registers */
#define CS8900_DMA_BUFFER_SIZE 16384 /* 16Kbyte or 64Kbyte, if 
				      * supported 
				      */

/* Receive buffer status */

#define RXBUF_FREE          0x0000
#define RXBUF_ALLOCATED     0x0001
#define RXBUF_LOANED        0x0002


/* Config Flags in cs_softc */

#define CFGFLG_MEM_MODE     0x0001
#define CFGFLG_USE_SA       0x0002
#define CFGFLG_IOCHRDY      0x0004
#define CFGFLG_DCDC_POL     0x0008
#define CFGFLG_FDX          0x0010
#define CFGFLG_DMA_MODE	    0x0020
#define CFGFLG_NOT_EEPROM   0x8000


/* Media Type in cs_softc */

#define MEDIA_AUI           0x0001
#define MEDIA_10BASE2       0x0002
#define MEDIA_10BASET       0x0003


/* Chip Identification */

#define EISA_NUM_CRYSTAL    0x630E
#define PROD_ID_MASK        0xE000
#define PROD_ID_CS8900      0x0000
#define PROD_ID_CS8920      0x4000
#define PROD_ID_CS892X      0x6000
#define PROD_REV_MASK       0x1F00


/* IO Port Offsets */

#define PORT_RXTX_DATA     0x0000
#define PORT_RXTX_DATA_1   0x0002
#define PORT_TX_CMD        0x0004
#define PORT_TX_LENGTH     0x0006
#define PORT_ISQ           0x0008
#define PORT_PKTPG_PTR     0x000A
#define PORT_PKTPG_DATA    0x000C
#define PORT_PKTPG_DATA_1  0x000E


/* PacketPage Offsets */

#define PKTPG_EISA_NUM     0x0000
#define PKTPG_PRODUCT_ID   0x0002
#define PKTPG_IO_BASE      0x0020
#define PKTPG_INT_NUM      0x0022
#define PKTPG_DMA_CHANNEL  0x0024
#define PKTPG_DMA_START_FRAME 0x0026
#define PKTPG_DMA_FRAME_COUNT 0x0028
#define PKTPG_DMA_BYTE_COUNT  0x002A
#define PKTPG_MEM_BASE     0x002C
#define PKTPG_EEPROM_CMD   0x0040
#define PKTPG_EEPROM_DATA  0x0042
#define PKTPG_FRAME_BYTE_COUNT	0x0050
#define PKTPG_RX_CFG       0x0102
#define PKTPG_RX_CTL       0x0104
#define PKTPG_TX_CFG       0x0106
#define PKTPG_BUF_CFG      0x010A
#define PKTPG_LINE_CTL     0x0112
#define PKTPG_SELF_CTL     0x0114
#define PKTPG_BUS_CTL      0x0116
#define PKTPG_TEST_CTL     0x0118
#define PKTPG_ISQ          0x0120
#define PKTPG_RX_EVENT     0x0124
#define PKTPG_TX_EVENT     0x0128
#define PKTPG_BUF_EVENT    0x012C
#define PKTPG_RX_MISS      0x0130
#define PKTPG_TX_COL       0x0132
#define PKTPG_LINE_ST      0x0134
#define PKTPG_SELF_ST      0x0136
#define PKTPG_BUS_ST       0x0138
#define PKTPG_TX_CMD       0x0144
#define PKTPG_TX_LENGTH    0x0146
#define PKTPG_LOG_ADDR	   0x0150   /* logical address filter 
				     * (hash table) 
				     */
#define PKTPG_IND_ADDR     0x0158
#define PKTPG_RX_STATUS    0x0400
#define PKTPG_RX_LENGTH    0x0402
#define PKTPG_RX_FRAME     0x0404
#define PKTPG_TX_FRAME     0x0A00


/* EEPROM Offsets */

#define EEPROM_IND_ADDR_H  0x001C
#define EEPROM_IND_ADDR_M  0x001D
#define EEPROM_IND_ADDR_L  0x001E
#define EEPROM_ISA_CFG     0x001F
#define EEPROM_MEM_BASE    0x0020
#define EEPROM_XMIT_CTL    0x0023
#define EEPROM_ADPTR_CFG   0x0024


/* Register Numbers */

#define REG_NUM_MASK       0x003F
#define REG_NUM_RX_EVENT   0x0004
#define REG_NUM_TX_EVENT   0x0008
#define REG_NUM_BUF_EVENT  0x000C
#define REG_NUM_RX_MISS    0x0010
#define REG_NUM_TX_COL     0x0012


/* Self Control Register */

#define SELF_CTL_RESET     0x0040
#define SELF_CTL_HC1E      0x2000
#define SELF_CTL_HCB1      0x8000


/* Self Status Register */

#define SELF_ST_INIT_DONE  0x0080
#define SELF_ST_SI_BUSY    0x0100
#define SELF_ST_EEP_PRES   0x0200
#define SELF_ST_EEP_OK     0x0400
#define SELF_ST_EL_PRES    0x0800


/* EEPROM Command Register */

#define EEPROM_CMD_READ    0x0200
#define EEPROM_CMD_ELSEL   0x0400


/* Bus Control Register */

#define BUS_CTL_RESET_DMA  0x0040
#define BUS_CTL_USE_SA     0x0200
#define BUS_CTL_MEM_MODE   0x0400
#define BUS_CTL_DMA_BURST  0x0800
#define BUS_CTL_IOCHRDY    0x1000
#define BUS_CTL_DMA_SIZE   0x2000
#define BUS_CTL_INT_ENBL   0x8000


/* Bus Status Register */

#define BUS_ST_TX_BID_ERR  0x0080
#define BUS_ST_RDY4TXNOW   0x0100


/* Line Control Register */

#define LINE_CTL_RX_ON     0x0040
#define LINE_CTL_TX_ON     0x0080
#define LINE_CTL_AUI_ONLY  0x0100
#define LINE_CTL_10BASET   0x0000
#define LINE_CTL_AUTO_SEL  0x0200


/* Test Control Register */

#define TEST_CTL_DIS_LT    0x0080
#define TEST_CTL_ENDEC_LP  0x0200
#define TEST_CTL_AUI_LOOP  0x0400
#define TEST_CTL_DIS_BKOFF 0x0800
#define TEST_CTL_FDX       0x4000


/* Receiver Configuration Register */

#define RX_CFG_SKIP        0x0040
#define RX_CFG_RX_OK_IE    0x0100
#define RX_CFG_RX_DMA_ONLY 0x0200
#define RX_CFG_CRC_ERR_IE  0x1000
#define RX_CFG_RUNT_IE     0x2000
#define RX_CFG_X_DATA_IE   0x4000
#define RX_CFG_ALL_IE      0x7100


/* Receiver Event Register */

#define RX_EVENT_DRIBBLE   0x0080
#define RX_EVENT_RX_OK     0x0100
#define RX_EVENT_IND_ADDR  0x0400
#define RX_EVENT_BCAST     0x0800
#define RX_EVENT_CRC_ERR   0x1000
#define RX_EVENT_RUNT      0x2000
#define RX_EVENT_X_DATA    0x4000


/* Receiver Control Register */

#define RX_CTL_INDHASH_A   0x0040
#define RX_CTL_PROMISC_A   0x0080
#define RX_CTL_RX_OK_A     0x0100
#define RX_CTL_MCAST_A     0x0200
#define RX_CTL_IND_A       0x0400
#define RX_CTL_BCAST_A     0x0800
#define RX_CTL_CRC_ERR_A   0x1000
#define RX_CTL_RUNT_A      0x2000
#define RX_CTL_X_DATA_A    0x4000


/* Transmit Configuration Register */

#define TX_CFG_LOSS_CRS_IE 0x0040
#define TX_CFG_SQE_ERR_IE  0x0080
#define TX_CFG_TX_OK_IE    0x0100
#define TX_CFG_OUT_WIN_IE  0x0200
#define TX_CFG_JABBER_IE   0x0400
#define TX_CFG_16_COLL_IE  0x8000
#define TX_CFG_ALL_IE      0x8FC0



/* Transmit Configuration Register */

#define TX_EVENT_LOSS_CRS  0x0040
#define TX_EVENT_SQE_ERR   0x0080
#define TX_EVENT_TX_OK     0x0100
#define TX_EVENT_OUT_WIN   0x0200
#define TX_EVENT_JABBER    0x0400
#define TX_EVENT_COLL_MASK 0x7800
#define TX_EVENT_16_COLL   0x8000


/* Transmit Command Register */

#define TX_CMD_START_5     0x0000
#define TX_CMD_START_381   0x0080
#define TX_CMD_START_1021  0x0040
#define TX_CMD_START_ALL   0x00C0
#define TX_CMD_FORCE       0x0100
#define TX_CMD_ONE_COLL    0x0200
#define TX_CMD_NO_CRC      0x1000
#define TX_CMD_NO_PAD      0x2000


/* Buffer Configuration Register */

#define BUF_CFG_SW_INT     0x0040
#define BUF_CFG_RX_DMA_IE  0x0080
#define BUF_CFG_RDY4TX_IE  0x0100
#define BUF_CFG_RX_MISS_IE 0x0400
#define BUF_CFG_TX_UNDR_IE 0x0200
#define BUF_CFG_RX_128_IE  0x0800
#define BUF_CFG_TX_COL_OVER_IE 0x1000
#define BUF_CFG_RX_MISS_OVER_IE 0x2000
#define BUF_CFG_RX_DEST_IE 0x8000

/* Buffer Event Register */

#define BUF_EVENT_SW_INT   0x0040
#define BUF_EVENT_RX_DMA   0x0080
#define BUF_EVENT_RDY4TX   0x0100
#define BUF_EVENT_TX_UNDR  0x0200
#define BUF_EVENT_RX_MISS  0x0400
#define BUF_EVENT_RX_128   0x0800
#define BUF_EVENT_RX_DEST  0x8000


/* ISA Configuration from EEPROM */

#define ISA_CFG_IRQ_MASK   0x000F
#define ISA_CFG_USE_SA     0x0080
#define ISA_CFG_IOCHRDY    0x0100
#define ISA_CFG_MEM_MODE   0x8000


/* Memory Base from EEPROM */

#define MEM_BASE_MASK      0xFFF0


/* Adpater Configuration from EEPROM */

#define ADPTR_CFG_MEDIA    0x0060
#define ADPTR_CFG_10BASET  0x0020
#define ADPTR_CFG_AUI      0x0040
#define ADPTR_CFG_10BASE2  0x0060
#define ADPTR_CFG_DCDC_POL 0x0080


/* Transmission Control from EEPROM */

#define XMIT_CTL_FDX       0x8000


/* Miscellaneous definitions */

#define MAXLOOP            0x8888
#define CS_DMA_FRAME_HEADER_SIZE   (sizeof(ushort) * 2)
#define RXBUFCOUNT         16
#define MC_LOANED          5

#endif
