#!/bin/sh -
copyright="\
/*
 * Copyright (c) 1992, 1993, 1994, 1995
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS \`\`AS IS'' AND
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
 */
"
SCRIPT_ID='$NetBSD: vnode_if.sh,v 1.56 2010/04/14 13:58:51 pooka Exp $'

# Script to produce VFS front-end sugar.
#
# usage: vnode_if.sh srcfile
#	(where srcfile is currently /sys/kern/vnode_if.src)
#

if [ $# -ne 1 ] ; then
	echo 'usage: vnode_if.sh srcfile'
	exit 1
fi

# Name and revision of the source file.
src=$1
SRC_ID=`head -1 $src | sed -e 's/.*\$\(.*\)\$.*/\1/'`

# Names of the created files.
out_c=vnode_if.c
out_rumpc=../rump/librump/rumpvfs/rumpvnode_if.c
out_h=../sys/vnode_if.h
out_rumph=../rump/include/rump/rumpvnode_if.h

# generate VNODE_LOCKDEBUG checks (not fully functional)
lockdebug=0

# Awk program (must support nawk extensions)
# Use "awk" at Berkeley, "nawk" or "gawk" elsewhere.
awk=${AWK:-awk}

# Does this awk have a "toupper" function? (i.e. is it GNU awk)
isgawk=`$awk 'BEGIN { print toupper("true"); exit; }' 2>/dev/null`

# If this awk does not define "toupper" then define our own.
if [ "$isgawk" = TRUE ] ; then
	# GNU awk provides it.
	toupper=
else
	# Provide our own toupper()
	toupper='
function toupper(str) {
	_toupper_cmd = "echo "str" |tr a-z A-Z"
	_toupper_cmd | getline _toupper_str;
	close(_toupper_cmd);
	return _toupper_str;
}'
fi

#
# This is the common part of all awk programs that read $src
# This parses the input for one function into the arrays:
#	argdir, argtype, argname, willrele
# and calls "doit()" to generate output for the function.
#
# Input to this parser is pre-processed slightly by sed
# so this awk parser doesn't have to work so hard.  The
# changes done by the sed pre-processing step are:
#	insert a space beween * and pointer name
#	replace semicolons with spaces
#
sed_prep='s:\*\([^\*/]\):\* \1:g
s/;/ /'
awk_parser='
# Comment line
/^#/	{ next; }
# First line of description
/^vop_/	{
	name=$1;
	argc=0;
	willmake=-1;
	next;
}
# Last line of description
/^}/	{
	doit();
	next;
}
# Middle lines of description
{
	argdir[argc] = $1; i=2;

	if ($2 == "LOCKED=YES") {
		lockstate[argc] = 1;
		i++;
	} else if ($2 == "LOCKED=NO") {
		lockstate[argc] = 0;
		i++;
	} else
		lockstate[argc] = -1;

	if ($2 == "WILLRELE" ||
	    $3 == "WILLRELE") {
		willrele[argc] = 1;
		i++;
	} else if ($2 == "WILLUNLOCK" ||
		   $3 == "WILLUNLOCK") {
		willrele[argc] = 2;
		i++;
	} else if ($2 == "WILLPUT" ||
		   $3 == "WILLPUT") {
		willrele[argc] = 3;
		i++;
	} else
		willrele[argc] = 0;

	if ($2 == "WILLMAKE") {
		willmake=argc;
		i++;
	}

	# XXX: replace non-portable types for rump.  We should really
	# nuke the types from the kernel, but that is a battle for
	# another day.
	at = $i;
	if (rump) {
		if (at == "vm_prot_t")
			at = "int";
		if (at == "voff_t")
			at = "off_t";
		if (at == "kauth_cred_t")
			at = "struct kauth_cred *"
	}
	argtype[argc] = at;
	i++;
	while (i < NF) {
		argtype[argc] = argtype[argc]" "$i;
		i++;
	}
	argname[argc] = $i;
	argc++;
	next;
}
'

# This is put before the copyright on each generated file.
warning="\
/*	@NetBSD@	*/

/*
 * Warning: DO NOT EDIT! This file is automatically generated!
 * (Modifications made here may easily be lost!)
 *
 * Created from the file:
 *	${SRC_ID}
 * by the script:
 *	${SCRIPT_ID}
 */
"

# This is to satisfy McKusick (get rid of evil spaces 8^)
anal_retentive='s:\([^/]\*\) :\1:g'

do_hfile () {
#
# Redirect stdout to the H file.
#
echo "$0: Creating $1" 1>&2
exec > $1
rump=$2

# Begin stuff
if [ -z "${rump}" ]; then
	SYS='SYS_'
else
	SYS='RUMP_RUMP'
fi
echo -n "$warning" | sed -e 's/\$//g;s/@/\$/g;s/ $//'
echo ""
echo -n "$copyright"
echo ''
echo "#ifndef _${SYS}VNODE_IF_H_"
echo "#define _${SYS}VNODE_IF_H_"
if [ ${lockdebug} -ne 0 ] ; then
	echo ''
	echo '#ifdef _KERNEL_OPT'
	echo '#include "opt_vnode_lockdebug.h"'
	echo '#endif /* _KERNEL_OPT */'
fi
[ -z "${rump}" ] && echo "
extern const struct vnodeop_desc ${rump}vop_default_desc;"
echo

# Body stuff
# This awk program needs toupper() so define it if necessary.
sed -e "$sed_prep" $src | $awk -v rump=${rump} "$toupper"'
function doit() {
	name = rump name
	# Declare arg struct, descriptor.
	if (!rump) {
		printf("\n#define %s_DESCOFFSET %d\n",
		    toupper(name), vop_offset++);
		printf("struct %s_args {\n", name);
		printf("\tconst struct vnodeop_desc * a_desc;\n");
		for (i=0; i<argc; i++) {
			printf("\t%s a_%s;\n", argtype[i], argname[i]);
		}
		printf("};\n");
		printf("extern const struct vnodeop_desc %s_desc;\n", name);
	}
	# Prototype it.
	protoarg = sprintf("int %s(", toupper(name));
	protolen = length(protoarg);
	printf("%s", protoarg);
	for (i=0; i<argc; i++) {
		protoarg = sprintf("%s", argtype[i]);
		if (i < (argc-1)) protoarg = (protoarg ", ");
		arglen = length(protoarg);
		if ((protolen + arglen) > 77) {
			protoarg = ("\n    " protoarg);
			arglen += 4;
			protolen = 0;
		}
		printf("%s", protoarg);
		protolen += arglen;
	}
	printf(");\n");
}
BEGIN	{
	arg0special="";
	vop_offset = 1; # start at 1, to count the 'default' op

	printf("struct buf;\n");
	if (rump) {
		printf("struct flock;\n");
		printf("struct knote;\n");
		printf("struct vm_page;\n");
	}
	printf("\n#ifndef _KERNEL\n#include <stdbool.h>\n#endif\n");
	printf("\n/* Special cases: */\n");

	argc=1;
	argtype[0]="struct buf *";
	argname[0]="bp";
	lockstate[0] = -1;
	arg0special="->b_vp";
	name="vop_bwrite";
	doit();
	printf("\n/* End of special cases */\n");
	if (rump)
		printf("\n");
}
END	{
	if (!rump) {
		printf("\n#define VNODE_OPS_COUNT\t%d\n", vop_offset);
	}
}
'"$awk_parser" | sed -e "$anal_retentive"

# End stuff
echo ''
echo "#endif /* !_${SYS}VNODE_IF_H_ */"
}
do_hfile $out_h ''
do_hfile $out_rumph 'rump_'

do_cfile () {
#
# Redirect stdout to the C file.
#
echo "$0: Creating $1" 1>&2
exec > $1
rump=$2

# Begin stuff
echo -n "$warning" | sed -e 's/\$//g;s/@/\$/g;s/ $//'
echo ""
echo -n "$copyright"
echo "
#include <sys/cdefs.h>
__KERNEL_RCSID(0, \"\$NetBSD\$\");"

[ ${lockdebug} -ne 0 ] && echo && echo '#include "opt_vnode_lockdebug.h"'

echo '
#include <sys/param.h>
#include <sys/mount.h>
#include <sys/buf.h>
#include <sys/vnode.h>
#include <sys/lock.h>'
[ ! -z "${rump}" ] && echo '#include <rump/rumpvnode_if.h>'		\
	&& echo '#include "rump_private.h"'

if [ -z "${rump}" ] ; then
	echo "
const struct vnodeop_desc vop_default_desc = {"
echo '	0,
	"default",
	0,
	NULL,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};
'
fi

# Body stuff
sed -e "$sed_prep" $src | $awk -v rump=${rump} -v lockdebug=${lockdebug} '
function do_offset(typematch) {
	for (i=0; i<argc; i++) {
		if (argtype[i] == typematch) {
			printf("\tVOPARG_OFFSETOF(struct %s_args, a_%s),\n",
				name, argname[i]);
			return i;
		};
	};
	print "\tVDESC_NO_OFFSET,";
	return -1;
}

function offsets() {
	# Define offsets array
	printf("const int %s_vp_offsets[] = {\n", name);
	for (i=0; i<argc; i++) {
		if (argtype[i] == "struct vnode *") {
			printf ("\tVOPARG_OFFSETOF(struct %s_args,a_%s),\n",
				name, argname[i]);
		}
	}
	print "\tVDESC_NO_OFFSET";
	print "};";
	# Define F_desc
	printf("const struct vnodeop_desc %s_desc = {\n", name);
	# offset
	printf ("\t%s_DESCOFFSET,\n", toupper(name));
	# printable name
	printf ("\t\"%s\",\n", name);
	# flags
	printf("\t0");
	vpnum = 0;
	for (i=0; i<argc; i++) {
		if (willrele[i]) {
			if (willrele[i] == 2) {
				word = "UNLOCK";
			} else if (willrele[i] == 3) {
				word = "PUT";
			} else {
				word = "RELE";
			}
			if (argdir[i] ~ /OUT/) {
				printf(" | VDESC_VPP_WILL%s", word);
			} else {
				printf(" | VDESC_VP%s_WILL%s", vpnum, word);
			};
			vpnum++;
		}
	}
	print ",";
	# vp offsets
	printf ("\t%s_vp_offsets,\n", name);
	# vpp (if any)
	do_offset("struct vnode **");
	# cred (if any)
	do_offset("kauth_cred_t");
	# componentname
	do_offset("struct componentname *");
	printf ("};\n");
}

function bodyrump() {
	printf("{\n\tint error;\n\n");
	printf("\trump_schedule();\n");
	printf("\terror = %s(", toupper(name));
	for (i=0; i<argc; i++) {
		printf("%s", argname[i]);
		if (i < (argc-1)) printf(", ");
	}
	printf(");\n");
	printf("\trump_unschedule();\n\n");
	printf("\treturn error;\n}\n");
}

function bodynorm() {
	printf("{\n\tint error;\n\tbool mpsafe;\n\tstruct %s_args a;\n", name);
	if (lockdebug) {
		printf("#ifdef VNODE_LOCKDEBUG\n");
		for (i=0; i<argc; i++) {
			if (lockstate[i] != -1)
				printf("\tint islocked_%s;\n", argname[i]);
		}
		printf("#endif\n");
	}
	printf("\ta.a_desc = VDESC(%s);\n", name);
	for (i=0; i<argc; i++) {
		printf("\ta.a_%s = %s;\n", argname[i], argname[i]);
		if (lockdebug && lockstate[i] != -1) {
			printf("#ifdef VNODE_LOCKDEBUG\n");
			printf("\tislocked_%s = (%s->v_vflag & VV_LOCKSWORK) ? (VOP_ISLOCKED(%s) == LK_EXCLUSIVE) : %d;\n",
			    argname[i], argname[i], argname[i], lockstate[i]);
			printf("\tif (islocked_%s != %d)\n", argname[i],
			    lockstate[i]);
			printf("\t\tpanic(\"%s: %s: locked %%d, expected %%d\", islocked_%s, %d);\n", name, argname[i], argname[i], lockstate[i]);
			printf("#endif\n");
		}
	}
	printf("\tmpsafe = (%s%s->v_vflag & VV_MPSAFE);\n", argname[0], arg0special);
	printf("\tif (!mpsafe) { KERNEL_LOCK(1, curlwp); }\n");
	printf("\terror = (VCALL(%s%s, VOFFSET(%s), &a));\n",
		argname[0], arg0special, name);
	printf("\tif (!mpsafe) { KERNEL_UNLOCK_ONE(curlwp); }\n");
	if (willmake != -1) {
		printf("#ifdef DIAGNOSTIC\n");
		printf("\tif (error == 0)\n"				\
		    "\t\tKASSERT((*%s)->v_size != VSIZENOTSET\n"	\
		    "\t\t    && (*%s)->v_writesize != VSIZENOTSET);\n",
		    argname[willmake], argname[willmake]);
		printf("#endif /* DIAGNOSTIC */\n");
	}
	printf("\treturn error;\n}\n");
}

function doit() {
	printf("\n");
	if (!rump)
		offsets();

	if (rump)
		extname = "RUMP_" toupper(name);
	else
		extname = toupper(name);

	# Define function.
	printf("int\n%s(", extname);
	for (i=0; i<argc; i++) {
		printf("%s %s", argtype[i], argname[i]);
		if (i < (argc-1)) printf(",\n    ");
	}
	printf(")\n");

	if (rump)
		bodyrump();
	else
		bodynorm();
}
BEGIN	{
	printf("\n/* Special cases: */\n");
	# start from 1 (vop_default is at 0)
	argc=1;
	willmake=-1;
	argdir[0]="IN";
	argtype[0]="struct buf *";
	argname[0]="bp";
	lockstate[0] = -1;
	arg0special="->b_vp";
	willrele[0]=0;
	name="vop_bwrite";
	doit();
	printf("\n/* End of special cases */\n");

	arg0special="";
}
'"$awk_parser" | sed -e "$anal_retentive"

# End stuff
[ -n "${rump}" ] && return

# Add the vfs_op_descs array to the C file.
# Begin stuff
echo "
const struct vnodeop_desc * const ${rump}vfs_op_descs[] = {
	&${rump}vop_default_desc,	/* MUST BE FIRST */
	&${rump}vop_bwrite_desc,	/* XXX: SPECIAL CASE */
"

# Body stuff
sed -e "$sed_prep" $src | $awk -v rump=${rump} '
function doit() {
	printf("\t&%s_desc,\n", name);
}
'"$awk_parser"

# End stuff
echo '	NULL
};'
}
do_cfile $out_c ''
do_cfile $out_rumpc 'rump_'

exit 0

# Local Variables:
# tab-width: 4
# End:
