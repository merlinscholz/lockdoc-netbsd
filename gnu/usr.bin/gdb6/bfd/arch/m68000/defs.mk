# This file is automatically generated.  DO NOT EDIT!
# Generated from: 	NetBSD: mknative-gdb,v 1.1 2006/05/29 19:10:58 nathanw Exp 
# Generated from: NetBSD: mknative.common,v 1.8 2006/05/26 19:17:21 mrg Exp 
#
G_libbfd_la_DEPENDENCIES=elf32-m68k.lo elf32.lo elf.lo elflink.lo elf-strtab.lo elf-eh-frame.lo dwarf1.lo m68knetbsd.lo aout32.lo m68k4knetbsd.lo hp300bsd.lo sunos.lo elf32-gen.lo cpu-m68k.lo netbsd-core.lo ofiles
G_libbfd_la_OBJECTS=archive.lo archures.lo bfd.lo bfdio.lo bfdwin.lo  cache.lo coffgen.lo corefile.lo format.lo init.lo libbfd.lo  opncls.lo reloc.lo section.lo syms.lo targets.lo hash.lo  linker.lo srec.lo binary.lo tekhex.lo ihex.lo stabs.lo  stab-syms.lo merge.lo dwarf2.lo simple.lo archive64.lo
G_DEFS=-DHAVE_CONFIG_H
G_INCLUDES=-DNETBSD_CORE   -I. -I${GNUHOSTDIST}/bfd -I${GNUHOSTDIST}/bfd/../include  -I${GNUHOSTDIST}/bfd/../intl -I../intl
G_TDEFAULTS=-DDEFAULT_VECTOR=bfd_elf32_m68k_vec -DSELECT_VECS='&bfd_elf32_m68k_vec,&m68knetbsd_vec,&m68k4knetbsd_vec,&hp300bsd_vec,&sunos_big_vec,&bfd_elf32_little_generic_vec,&bfd_elf32_big_generic_vec' -DSELECT_ARCHITECTURES='&bfd_m68k_arch' -DHAVE_bfd_elf32_m68k_vec -DHAVE_m68knetbsd_vec -DHAVE_m68k4knetbsd_vec -DHAVE_hp300bsd_vec -DHAVE_sunos_big_vec -DHAVE_bfd_elf32_little_generic_vec -DHAVE_bfd_elf32_big_generic_vec
