/*	$NetBSD: podule_data.h,v 1.11 2002/05/23 22:01:14 bjh21 Exp $	*/

/*
 * THIS FILE AUTOMATICALLY GENERATED.  DO NOT EDIT.
 *
 * generated from:
 *	NetBSD: podules,v 1.13 2002/05/23 22:00:49 bjh21 Exp 
 */

/*
 * Copyright (c) 1996 Mark Brinicombe
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
 *      This product includes software developed by Mark Brinicombe
 *	for the NetBSD Project.
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

static struct podule_description known_podules[] = {
	{ PODULE_HOSTTUBE,	"Host TUBE (to BBC)" },
	{ PODULE_PARASITETUBE,	"Parastite TUBE (to 2nd processor)" },
	{ PODULE_ACORN_SCSI,	"Acorn SCSI interface" },
	{ PODULE_ETHER1,	"Ether1 interface" },
	{ PODULE_IBMDISC,	"IBM disc" },
	{ PODULE_ROMRAM,	"ROM/RAM podule" },
	{ PODULE_BBCIO,	"BBC I/O podule" },
	{ PODULE_FAXPACK,	"FaxPack modem" },
	{ PODULE_TELETEXT,	"Teletext" },
	{ PODULE_CDROM,	"CD-ROM" },
	{ PODULE_IEEE488,	"IEEE 488 interface" },
	{ PODULE_ST506,	"ST506 HD interface" },
	{ PODULE_ESDI,	"ESDI interface" },
	{ PODULE_SMD,	"SMD interface" },
	{ PODULE_LASERPRINTER,	"laser printer" },
	{ PODULE_SCANNER,	"scanner" },
	{ PODULE_FASTRING,	"Fast Ring interface" },
	{ PODULE_FASTRING2,	"Fast Ring II interface" },
	{ PODULE_PROMPROGRAMMER,	"PROM programmer" },
	{ PODULE_ACORN_MIDI,	"MIDI interface" },
	{ PODULE_LASERDIRECT,	"LaserDirect (Canon LBP-4)" },
	{ PODULE_FRAMEGRABBER,	"frame grabber" },
	{ PODULE_A448,	"A448 sound sampler" },
	{ PODULE_VIDEODIGITISER,	"video digitiser" },
	{ PODULE_GENLOCK,	"genlock" },
	{ PODULE_CODECSAMPLER,	"codec sampler" },
	{ PODULE_IMAGEANALYSER,	"image analyser" },
	{ PODULE_ANALOGUEINPUT,	"analogue input" },
	{ PODULE_CDSOUNDSAMPLER,	"CD sound sampler" },
	{ PODULE_6MIPSSIGPROC,	"6 MIPS signal processor" },
	{ PODULE_12MIPSSIGPROC,	"12 MIPS signal processor" },
	{ PODULE_33MIPSSIGPROC,	"33 MIPS signal processor" },
	{ PODULE_TOUCHSCREEN,	"touch screen" },
	{ PODULE_TRANSPUTERLINK,	"Transputer link" },
	{ PODULE_HCCS_IDESCSI,	"HCCS IDE or SCSI interface" },
	{ PODULE_LASERSCANNER,	"laser scanner" },
	{ PODULE_GNOME_TRANSPUTERLINK,	"Transputer link" },
	{ PODULE_VMEBUS,	"VME bus interface" },
	{ PODULE_TAPESTREAMER,	"tape streamer" },
	{ PODULE_LASERTEST,	"laser test" },
	{ PODULE_COLOURDIGITISER,	"colour digitiser" },
	{ PODULE_WEATHERSATELLITE,	"weather satellite" },
	{ PODULE_AUTOCUE,	"autocue" },
	{ PODULE_PARALLELIO16BIT,	"16-bit parallel I/O" },
	{ PODULE_12BITATOD,	"12-bit ADC" },
	{ PODULE_SERIALPORTSRS423,	"RS423 serial ports" },
	{ PODULE_MINI,	"mini" },
	{ PODULE_FRAMEGRABBER2,	"frame grabber II" },
	{ PODULE_INTERACTIVEVIDEO2,	"interactive video II" },
	{ PODULE_WILDVISION_ATOD,	"ADC" },
	{ PODULE_WILDVISION_DTOA,	"DAC" },
	{ PODULE_EMR_MIDI4,	"MIDI 4" },
	{ PODULE_FPCP,	"floating-point co-processor" },
	{ PODULE_PRISMA3,	"Prisma 3" },
	{ PODULE_ARVIS,	"ARVIS" },
	{ PODULE_4BY4MIDI,	"4x4 MIDI" },
	{ PODULE_BISERIALPARALLEL,	"Bi-directional serial/parallel" },
	{ PODULE_CHROMA300,	"Chroma 300 genlock" },
	{ PODULE_CUMANA_SCSI2,	"SCSI II interface" },
	{ PODULE_COLOURCONVERTER,	"Colour Converter" },
	{ PODULE_8BITSAMPLER,	"8-bit sampler" },
	{ PODULE_PLUTO,	"Pluto interface" },
	{ PODULE_LOGICANALYSER,	"Logic Analyser" },
	{ PODULE_ACORN_USERMIDI,	"User Port/MIDI interface" },
	{ PODULE_LINGENUITY_SCSI8,	"8 bit SCSI interface" },
	{ PODULE_ARXE_SCSI,	"16 bit SCSI interface" },
	{ PODULE_DUALUSERPORT,	"dual User Port" },
	{ PODULE_EMR_SAMPLER8,	"Sampler8" },
	{ PODULE_EMR_SMTP,	"SMTP" },
	{ PODULE_EMR_MIDI2,	"MIDI2" },
	{ PODULE_PINEAPPLE_DIGITISER,	"digitiser" },
	{ PODULE_VIDEOFRAMECAPTURE,	"video frame capture" },
	{ PODULE_MONOOVERLAYFRSTORE,	"mono overlay frame store" },
	{ PODULE_MARKETBUFFER,	"market buffer" },
	{ PODULE_PAGESTORE,	"page store" },
	{ PODULE_TRAMMOTHERBOARD,	"TRAM motherboard" },
	{ PODULE_TRANSPUTER,	"Transputer" },
	{ PODULE_OPTICALSCANNER,	"optical scanner" },
	{ PODULE_DIGITISINGTABLET,	"digitising tablet" },
	{ PODULE_200DPISCANNER,	"200-dpi scanner" },
	{ PODULE_COLOURCARD,	"ColourCard" },
	{ PODULE_PRESENTERGENLOCK,	"Presenter Genlock" },
	{ PODULE_HAWKV9,	"Hawk v9 mark2" },
	{ PODULE_CROMA200,	"Chroma 200 genlock" },
	{ PODULE_WILDVISION_SOUNDSAMPLER,	"Wild Vision Sound Sampler" },
	{ PODULE_DTSOFT_IDE,	"IDE interface" },
	{ PODULE_8BITATOD,	"8-bit ADC" },
	{ PODULE_MFMHDCONTROLLER,	"MFM hard disc controller" },
	{ PODULE_OAK_SCSI,	"16 bit SCSI interface" },
	{ PODULE_QUADSERIAL,	"quad serial" },
	{ PODULE_PALPROGRAMMER,	"PAL programmer" },
	{ PODULE_I2CBUS,	"I^2C bus" },
	{ PODULE_BEEBUG_SCANNER,	"scanner interface" },
	{ PODULE_PANDORA_QUADMIDI,	"quad MIDI" },
	{ PODULE_PRES_DISCBUFFER,	"disc buffer" },
	{ PODULE_PRES_USERPORT,	"User Port" },
	{ PODULE_MICROYEAI,	"Micro YEAI" },
	{ PODULE_ETHER2,	"Ether2 interface" },
	{ PODULE_SGB_EXPANSIONBOX,	"SGB expansion box" },
	{ PODULE_ULTIMATE,	"Ultimate micropodule carrier" },
	{ PODULE_NEXUS,	"Nexus interface (Podule)" },
	{ PODULE_PHOBOX_USERANALOGUE,	"User and Analogue ports" },
	{ PODULE_MORLEY_STATICRAM,	"static RAM" },
	{ PODULE_MORLEY_SCSI,	"SCSI interface" },
	{ PODULE_MORLEY_TELETEXT,	"teletext interface" },
	{ PODULE_TECHNOMATIC_SCANNER,	"scanner" },
	{ PODULE_BEEBUG_QUADRANT,	"Quadrant" },
	{ PODULE_RCC_VOICEPROCESSOR,	"voice processor" },
	{ PODULE_RCC_UHFLINK,	"UHF link" },
	{ PODULE_MORLEY_USERANALOGUE,	"User and Analogue ports" },
	{ PODULE_HCCS_USERANALOGUE,	"User and Analogue ports" },
	{ PODULE_WILDVISION_CENTRONICS,	"Bi-directional Centronics" },
	{ PODULE_HCCS_A3000SCSI,	"A3000 SCSI interface" },
	{ PODULE_LINDIS_DIGITISER,	"digitiser" },
	{ PODULE_CCC_PEAKPROGMETER,	"peak prog. meter" },
	{ PODULE_LASERLIGHTCONTROL,	"laser light control" },
	{ PODULE_HARDDISCINTERFACE,	"hard disc interface" },
	{ PODULE_EXTRAMOUSE,	"extra mouse" },
	{ PODULE_STEBUSINTERFACE,	"STE bus interface" },
	{ PODULE_MORLEY_ST506,	"ST506 disc interface" },
	{ PODULE_BRAINSOFT_MULTI1,	"Multi_1" },
	{ PODULE_BRAINSOFT_MULTI2,	"Multi_2" },
	{ PODULE_BRAINSOFT_24DIGITISER,	"24-bit digitiser" },
	{ PODULE_BRAINSOFT_24GRAPHICS,	"24-bit graphics" },
	{ PODULE_SYNTEC_SPECTRON,	"Spectron" },
	{ PODULE_SYNTEC_QUAD16DTOA,	"Quad 16-bit DAC" },
	{ PODULE_ROMBO_4BITDIGIISER,	"4-bit digitiser" },
	{ PODULE_DONGLEANDKEYPAD,	"dongle and keypad" },
	{ PODULE_3SL_SCSI,	"SCSI interface" },
	{ PODULE_ARMADILLO_BTM1,	"BTM1" },
	{ PODULE_ARMADILLO_DSO1,	"DSO1" },
	{ PODULE_DELTRONICS_USER,	"User Port" },
	{ PODULE_JPEGCOMPRESSOR,	"JPEG compressor" },
	{ PODULE_BEEBUG_A3000SCSI,	"A3000 SCSI" },
	{ PODULE_BEEBUG_COLOURSCAN,	"colour scanner interface" },
	{ PODULE_EXTENSIONROM,	"extension ROM" },
	{ PODULE_GRAPHICSENHANCER,	"Graphics Enhancer" },
	{ PODULE_SIMIS_AFB300,	"AFB300" },
	{ PODULE_FAXPACKSENIOR,	"FaxPack Senior" },
	{ PODULE_FAXPACKJUNIOR,	"FaxPack Junior" },
	{ PODULE_LINGENUITY_SCSI8SHARE,	"8 bit SCSIShare interface" },
	{ PODULE_VTI_SCSI,	"SCSI interface" },
	{ PODULE_ATOMWIDE_PIA,	"PIA" },
	{ PODULE_NEXUSNS,	"Nexus interface (A3020 netslot)" },
	{ PODULE_ATOMWIDE_SERIAL,	"multiport serial interface" },
	{ PODULE_WATFORD_IDE,	"IDE interface" },
	{ PODULE_ATOMWIDE_IDE,	"IDE interface" },
	{ PODULE_ARMADILLO_RSI,	"RSI" },
	{ PODULE_ARMADILLO_TCR,	"TCR" },
	{ PODULE_LINGENUITY_SCSI,	"16 bit SCSI interface" },
	{ PODULE_LINGENUITY_SCSISHARE,	"16 bit SCSIShare interface" },
	{ PODULE_BEEBUG_IDE,	"IDE interface" },
	{ PODULE_WATFORD_PRISMRT,	"Prism RT" },
	{ PODULE_HCCS_VIDEODIGITISER,	"video digitiser" },
	{ PODULE_DTSOFT_SCANPORT,	"ScanPort" },
	{ PODULE_DTSOFT_PACCEL,	"Paccel" },
	{ PODULE_DTSOFT_CANONION,	"Canon ION interface" },
	{ PODULE_BIA_AUDIO,	"BIA audio" },
	{ PODULE_IRLAM_FAXIM,	"FaxIm" },
	{ PODULE_IRLAM_MOVINGIMAGE,	"Moving Image" },
	{ PODULE_CUMANA_SCSI1,	"SCSI I interface" },
	{ PODULE_NEXUS_A3000ETHERNET,	"A3000 Ethernet" },
	{ PODULE_NEXUS_PCEMACCELL,	"PC Emulator accelerator" },
	{ PODULE_NEXUS_64CANSERIAL,	"64-channel serial" },
	{ PODULE_ETHER3,	"Ether3/Ether5 interface" },
	{ PODULE_IOTA_SCANNER,	"scanner interface" },
	{ PODULE_NEXUS_I860MATHACCELL,	"i860 floating-point accelerator" },
	{ PODULE_II_QUADSERIAL,	"quad serial port" },
	{ PODULE_WATFORD_SCANNERGREY,	"grey-scale scanner" },
	{ PODULE_WATFORD_SCANNERRGB,	"RGB scanner" },
	{ PODULE_WATFORD_PRISMCOLOUR,	"Prism Colour" },
	{ PODULE_WATFORD_USERANALOGUE,	"Analogue and User Ports" },
	{ PODULE_BAILDON_DISCBUFFER,	"disc buffer" },
	{ PODULE_BAILDON_A3000UPBUS,	"A3000 UP bus" },
	{ PODULE_ICS_IDE,	"IDE Interface" },
	{ PODULE_HCCS_BWDIGITISER,	"b/w digitiser" },
	{ PODULE_CSD_IDE8,	"8-bit IDE interface" },
	{ PODULE_CSD_IDE16,	"16-bit IDE interface" },
	{ PODULE_SERIALPORT_IDE,	"IDE interface" },
	{ PODULE_SERIALPORT_4MFLOPPY,	"4 MB floppy" },
	{ PODULE_CADSOFT_MAESTROINTER,	"Maestro Inter" },
	{ PODULE_ARXE_QUADFS,	"Quad-density floppy interface" },
	{ PODULE_SERIALPORT_DUALSERIAL,	"Serial interface" },
	{ PODULE_ETHERLAN200,	"EtherLan 200-series" },
	{ PODULE_SCANLIGHTV256,	"ScanLight Video 256" },
	{ PODULE_EAGLEM2,	"Eagle M2" },
	{ PODULE_LARKA16,	"Lark A16" },
	{ PODULE_ETHERLAN100,	"EtherLan 100-series" },
	{ PODULE_ETHERLAN500,	"EtherLan 500-series" },
	{ PODULE_ETHERM,	"EtherM dual interface NIC" },
	{ PODULE_CUMANA_SLCD,	"CDFS & SLCD expansion card" },
	{ PODULE_BRINILINK,	"BriniLink transputer link adapter" },
	{ PODULE_ETHERB,	"EtherB network slot interface" },
	{ PODULE_24I16,	"24i16 digitiser" },
	{ PODULE_PCCARD,	"PC card" },
	{ PODULE_ETHERLAN600,	"EtherLan 600-series" },
	{ PODULE_CASTLE_SCSI16SHARE,	"8 or 16 bit SCSI2Share interface" },
	{ PODULE_CASTLE_ETHERSCSISHARE,	"8 or 16 bit SCSI2Share interface, possibly with Ethernet" },
	{ PODULE_CASTLE_ETHERSCSI,	"EtherSCSI" },
	{ PODULE_CASTLE_SCSI16,	"8 or 16 bit SCSI2 interface" },
	{ PODULE_ALSYSTEMS_SCSI,	"SCSI II host adapter" },
	{ PODULE_RAPIDE,	"RapIDE32 interface" },
	{ PODULE_ETHERLAN100AEH,	"AEH77 (EtherLan 102)" },
	{ PODULE_ETHERLAN200AEH,	"AEH79 (EtherLan 210)" },
	{ PODULE_ETHERLAN600AEH,	"AEH62/78/99 (EtherLan 602)" },
	{ PODULE_ETHERLAN500AEH,	"AEH75 (EtherLan 512)" },
	{ PODULE_CONNECT32,	"Connect32 SCSI II interface" },
	{ PODULE_CASTLE_SCSI32,	"32 bit SCSI2 + DMA interface" },
	{ PODULE_ETHERLAN700AEH,	"AEH98 (EtherLan 700-series)" },
	{ PODULE_ETHERLAN700,	"EtherLan 700-series" },
	{ PODULE_SIMTEC_IDE8,	"8 bit IDE interface" },
	{ PODULE_SIMTEC_IDE,	"16 bit IDE interface" },
	{ PODULE_MIDICONNECT,	"Midi-Connect" },
	{ PODULE_ETHERI,	"EtherI interface" },
	{ PODULE_MIDIMAX,	"MIDI max" },
	{ PODULE_MMETHERV,	"Multi-media/EtherV" },
	{ PODULE_ETHERN,	"EtherN interface" },
	{ 0x0000, NULL }
};


struct manufacturer_description known_manufacturers[] = {
	{ MANUFACTURER_ACORN, 		"Acorn Computers" },
	{ MANUFACTURER_ACORNUSA, 	"Acorn Computers (USA)" },
	{ MANUFACTURER_OLIVETTI, 	"Olivetti" },
	{ MANUFACTURER_WATFORD, 	"Watford Electronics" },
	{ MANUFACTURER_CCONCEPTS, 	"Computer Concepts" },
	{ MANUFACTURER_IINTERFACES, 	"Intelligent Interfaces" },
	{ MANUFACTURER_CAMAN, 		"Caman" },
	{ MANUFACTURER_ARMADILLO, 	"Armadillo Systems" },
	{ MANUFACTURER_SOFTOPTION, 	"Soft Option" },
	{ MANUFACTURER_WILDVISION, 	"Wild Vision" },
	{ MANUFACTURER_ANGLOCOMPUTERS, 	"Anglo Computers" },
	{ MANUFACTURER_RESOURCE, 	"Resource" },
	{ MANUFACTURER_HCCS, 		"HCCS" },
	{ MANUFACTURER_MUSBURYCONSULT, 	"Musbury Consultants" },
	{ MANUFACTURER_GNOME, 		"Gnome" },
	{ MANUFACTURER_AANDGELEC, 	"A and G Electronics" },
	{ MANUFACTURER_SPACETECH, 	"Spacetech" },
	{ MANUFACTURER_ATOMWIDE, 	"Atomwide" },
	{ MANUFACTURER_SYNTEC, 		"Syntec" },
	{ MANUFACTURER_EMR, 		"ElectroMusic Research" },
	{ MANUFACTURER_MILLIPEDE, 	"Millipede" },
	{ MANUFACTURER_VIDEOELEC, 	"Video Electronics" },
	{ MANUFACTURER_BRAINSOFT, 	"Brainsoft" },
	{ MANUFACTURER_ATOMWIDE2, 	"Atomwide" },
	{ MANUFACTURER_LENDAC, 		"Lendac Data Systems" },
	{ MANUFACTURER_CAMMICROSYS, 	"Cambridge Micro Systems" },
	{ MANUFACTURER_LINGENUITY, 	"Lingenuity" },
	{ MANUFACTURER_SIPLAN, 		"Siplan Electronics Research" },
	{ MANUFACTURER_SCIFRONTIERS, 	"Science Frontiers" },
	{ MANUFACTURER_PINEAPPLE, 	"Pineapple Software" },
	{ MANUFACTURER_TECHNOMATIC, 	"Technomatic" },
	{ MANUFACTURER_IRLAM, 		"Irlam Instruments" },
	{ MANUFACTURER_NEXUS, 		"Nexus Electronics" },
	{ MANUFACTURER_OAK, 		"Oak Solutions" },
	{ MANUFACTURER_HUGHSYMONS, 	"Hugh Symons" },
	{ MANUFACTURER_BEEBUG, 		"BEEBUG (RISC Developments)" },
	{ MANUFACTURER_TEKNOMUSIK, 	"Teknomusik" },
	{ MANUFACTURER_REELTIME, 	"Reel Time" },
	{ MANUFACTURER_PRES, 		"PRES" },
	{ MANUFACTURER_DIGIHURST, 	"Digihurst" },
	{ MANUFACTURER_SGBCOMPSERV, 	"SGB Computer Services" },
	{ MANUFACTURER_SJ, 		"SJ Research" },
	{ MANUFACTURER_PHOBOX, 		"Phobox Electronics" },
	{ MANUFACTURER_MORLEY, 		"Morley Electronics" },
	{ MANUFACTURER_RACINGCAR, 	"Raching Car Computers" },
	{ MANUFACTURER_HCCS2, 		"HCCS" },
	{ MANUFACTURER_LINDIS, 		"Lindis International" },
	{ MANUFACTURER_CCC, 		"Computer Control Consultants" },
	{ MANUFACTURER_UNILAB, 		"Unilab" },
	{ MANUFACTURER_SEFANFROHLING, 	"Sefan Frohling" },
	{ MANUFACTURER_ROMBO, 		"Rombo Productions" },
	{ MANUFACTURER_3SL, 		"3SL" },
	{ MANUFACTURER_DELTRONICS, 	"Deltronics" },
	{ MANUFACTURER_VTI, 		"Vertical Twist" },
	{ MANUFACTURER_SIMIS, 		"Simis" },
	{ MANUFACTURER_DTSOFT, 		"D.T. Software" },
	{ MANUFACTURER_ARMINTERFACES, 	"ARM Interfaces" },
	{ MANUFACTURER_BIA, 		"BIA" },
	{ MANUFACTURER_CUMANA, 		"Cumana" },
	{ MANUFACTURER_IOTA, 		"Iota" },
	{ MANUFACTURER_ICS, 		"Ian Copestake Software" },
	{ MANUFACTURER_BAILDON, 	"Baildon Electronics" },
	{ MANUFACTURER_CSD, 		"CSD" },
	{ MANUFACTURER_SERIALPORT, 	"Serial Port" },
	{ MANUFACTURER_CADSOFT, 	"CADsoft" },
	{ MANUFACTURER_ARXE, 		"ARXE" },
	{ MANUFACTURER_ALEPH1, 		"Aleph 1" },
	{ MANUFACTURER_ICUBED, 		"I-Cubed" },
	{ MANUFACTURER_BRINI, 		"Brini" },
	{ MANUFACTURER_ANT, 		"ANT" },
	{ MANUFACTURER_CASTLE, 		"Castle Technology" },
	{ MANUFACTURER_ALSYSTEMS, 	"Alsystems" },
	{ MANUFACTURER_SIMTEC, 		"Simtec Electronics" },
	{ MANUFACTURER_YES, 		"Yellowstone Educational Solutions" },
	{ MANUFACTURER_MCS, 		"MCS" },
	{ MANUFACTURER_EESOX, 		"EESOX" },
	{ 0, NULL }
};
