#	$NetBSD: Makefile.sh3.be,v 1.6 2000/12/17 15:52:43 jdolecek Exp $

# Makefile for NetBSD
#
# This makefile is constructed from a machine description:
#	config machineid
# Most changes should be made in the machine description
#	/sys/arch/sh3/conf/``machineid''
# after which you should do
#	config machineid
# Machine generic makefile changes should be made in
#	/sys/arch/sh3/conf/Makefile.sh3
# after which config should be rerun for all machines of that type.

# DEBUG is set to -g if debugging.
# PROF is set to -pg if profiling.

AR?=	ar
AS?=	as
CC?=	cc
CPP?=	cpp
LD?=	ld
LORDER?=lorder
MKDEP?=	mkdep
NM?=	nm
RANLIB?=ranlib
SIZE?=	size
STRIP?=	strip
TSORT?=	tsort -q

COPTS?=	-g -O 

# source tree is located via $S relative to the compilation directory
.ifndef S
S!=	cd ../../../..; pwd
.endif
SH3=	$S/arch/sh3

INCLUDES=	-I. -I$S/arch -I$S -nostdinc
CPPFLAGS=	${INCLUDES} ${IDENT} ${PARAM} -D_KERNEL \
		-Dsh3
CWARNFLAGS=	-Werror -Wall -Wmissing-prototypes -Wstrict-prototypes
CFLAGS=		${DEBUG} ${COPTS} ${CWARNFLAGS}
AFLAGS=		-x assembler-with-cpp -traditional-cpp -D_LOCORE
LINKFLAGS=	-e start -Map netbsd.map -T ../../conf/sh.x
STRIPFLAGS=	--strip-debug
MACHINE=sh3
MACHINE_ARCH=sh3

### find out what to use for libkern
.include "$S/lib/libkern/Makefile.inc"
.ifndef PROF
LIBKERN=	${KERNLIB}
.else
LIBKERN=	${KERNLIB_PROF}
.endif

### find out what to use for libcompat
.include "$S/compat/common/Makefile.inc"
.ifndef PROF
LIBCOMPAT=	${COMPATLIB}
.else
LIBCOMPAT=	${COMPATLIB_PROF}
.endif

# compile rules: rules are named ${TYPE}_${SUFFIX} where TYPE is NORMAL or
# HOSTED}, and SUFFIX is the file suffix, capitalized (e.g. C for a .c file).

NORMAL_C=	${CC} ${CFLAGS} ${CPPFLAGS} ${PROF} -c $<
NORMAL_S=	${CC} ${AFLAGS} ${CPPFLAGS} -c $<

%OBJS

%CFILES

%SFILES

# load lines for config "xxx" will be emitted as:
# xxx: ${SYSTEM_DEP} swapxxx.o
#	${SYSTEM_LD_HEAD}
#	${SYSTEM_LD} swapxxx.o
#	${SYSTEM_LD_TAIL}
SYSTEM_OBJ=	locore.o \
		param.o ioconf.o ${OBJS} ${LIBKERN} ${LIBCOMPAT}
SYSTEM_DEP=	Makefile ${SYSTEM_OBJ}
SYSTEM_LD_HEAD=	rm -f $@
SYSTEM_LD=	@echo ${LD} ${LINKFLAGS} -o $@.out '$${SYSTEM_OBJ}' vers.o; \
		${LD} ${LINKFLAGS} -o $@.out ${SYSTEM_OBJ} vers.o
SYSTEM_LD_TAIL=	

DEBUG?=
.if ${DEBUG} == "-g"
LINKFLAGS+=	-X
SYSTEM_LD_TAIL+=; \
		echo mv -f $@ $@.gdb; mv -f $@ $@.gdb; \
		echo ${STRIP} ${STRIPFLAGS} -o $@ $@.gdb; \
		${STRIP} ${STRIPFLAGS} -o $@ $@.gdb
.else
LINKFLAGS+=	
.endif

%LOAD

assym.h: $S/kern/genassym.sh ${SH3}/sh3/genassym.cf
	sh $S/kern/genassym.sh ${CC} ${CFLAGS} ${CPPFLAGS} ${PROF} \
	    < ${SH3}/sh3/genassym.cf > assym.h.tmp && \
	mv -f assym.h.tmp assym.h

param.c: $S/conf/param.c
	rm -f param.c
	cp $S/conf/param.c .

param.o: param.c Makefile
	${NORMAL_C}

ioconf.o: ioconf.c
	${NORMAL_C}

newvers: ${SYSTEM_DEP} ${SYSTEM_SWAP_DEP}
	sh $S/conf/newvers.sh
	${CC} ${CFLAGS} ${CPPFLAGS} ${PROF} -c vers.c


__CLEANKERNEL: .USE
	@echo "${.TARGET}ing the kernel objects"
	rm -f eddep *netbsd netbsd netbsd.gdb tags *.[io] [a-z]*.s \
	    [Ee]rrs linterrs makelinks assym.h.tmp assym.h

__CLEANDEPEND: .USE
	rm -f .depend

clean: __CLEANKERNEL

cleandir: __CLEANKERNEL __CLEANDEPEND

lint:
	@lint -hbxncez -Dvolatile= ${CPPFLAGS} -UKGDB \
	    ${SH3}/sh3/Locore.c ${CFILES} \
	    ioconf.c param.c | \
	    grep -v 'static function .* unused'

tags:
	@echo "see $S/kern/Makefile for tags"

links:
	egrep '#if' ${CFILES} | sed -f $S/conf/defines | \
	  sed -e 's/:.*//' -e 's/\.c/.o/' | sort -u > dontlink
	echo ${CFILES} | tr -s ' ' '\12' | sed 's/\.c/.o/' | \
	  sort -u | comm -23 - dontlink | \
	  sed 's,../.*/\(.*.o\),rm -f \1; ln -s ../GENERIC/\1 \1,' > makelinks
	sh makelinks && rm -f dontlink

SRCS=	${SH3}/sh3/locore.s \
	param.c ioconf.c ${CFILES} ${SFILES}
depend: .depend
.depend: ${SRCS} assym.h param.c
	${MKDEP} ${AFLAGS} ${CPPFLAGS} ${SH3}/sh3/locore.s
	${MKDEP} -a ${CFLAGS} ${CPPFLAGS} param.c ioconf.c ${CFILES}
	${MKDEP} -a ${AFLAGS} ${CPPFLAGS} ${SFILES}
	sh $S/kern/genassym.sh ${MKDEP} -f assym.dep ${CFLAGS} \
	  ${CPPFLAGS} < ${SH3}/sh3/genassym.cf
	@sed -e 's/.*\.o:.*\.c/assym.h:/' < assym.dep >> .depend
	@rm -f assym.dep

dependall: depend all


# depend on root or device configuration
autoconf.o conf.o: Makefile
 
# depend on network or filesystem configuration 
uipc_proto.o vfs_conf.o: Makefile 

# depend on maxusers
machdep.o: Makefile

# depend on CPU configuration 
locore.o machdep.o: Makefile


locore.o: ${SH3}/sh3/locore.s assym.h
	${NORMAL_S}

# The install target can be redefined by putting a
# install-kernel-${MACHINE_NAME} target into /etc/mk.conf
MACHINE_NAME!=  uname -n
install: install-kernel-${MACHINE_NAME}
.if !target(install-kernel-${MACHINE_NAME}})
install-kernel-${MACHINE_NAME}:
	rm -f /onetbsd
	ln /netbsd /onetbsd
	cp netbsd /nnetbsd
	mv /nnetbsd /netbsd
.endif

%RULES
