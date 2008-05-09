#	$NetBSD: std.viper,v 1.5 2008/05/09 00:08:37 matt Exp $
#
# Arcom Viper standard kernel options
#

machine evbarm arm
include		"conf/std"	# MI standard options
include		"arch/arm/conf/std.arm"	# arch standard options

include "arch/evbarm/conf/files.viper"

options 	EXEC_ELF32
options 	EXEC_SCRIPT

options 	ARM32

options 	KERNEL_EXT_BASE=0xc0000000
makeoptions	LOADADDRESS="0xc0200000"
makeoptions	BOARDTYPE="viper"
makeoptions	BOARDMKFRAG="${THISARM}/conf/mk.viper"

options         ARM_INTR_IMPL="<arch/arm/xscale/pxa2x0_intr.h>"

# OS Timer    This is compatible to SA1110's OS Timer.
saost*  at pxaip? addr 0x40a00000 size 0x20
