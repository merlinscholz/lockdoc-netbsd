#! /bin/sh -
#	$NetBSD: makesyscalls.sh,v 1.27 1998/02/18 23:14:55 thorpej Exp $
#
# Copyright (c) 1994,1996 Christopher G. Demetriou
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. All advertising materials mentioning features or use of this software
#    must display the following acknowledgement:
#      This product includes software developed for the NetBSD Project
#      by Christopher G. Demetriou.
# 4. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#	@(#)makesyscalls.sh	8.1 (Berkeley) 6/10/93

set -e

case $# in
    2)	;;
    *)	echo "Usage: $0 config-file input-file" 1>&2
	exit 1
	;;
esac

# source the config file.
. $1

# the config file sets the following variables:
#	sysnames	the syscall names file
#	sysnumhdr	the syscall numbers file
#	syssw		the syscall switch file
#	sysarghdr	the syscall argument struct definitions
#	compatopts	those syscall types that are for 'compat' syscalls
#	switchname	the name for the 'struct sysent' we define
#	namesname	the name for the 'char *[]' we define
#	constprefix	the prefix for the system call constants
#
# NOTE THAT THIS makesyscalls.sh DOES NOT SUPPORT 'LIBCOMPAT'.

# tmp files:
sysdcl="sysent.dcl"
sysprotos="sys.protos"
syscompat_pref="sysent."
sysent="sysent.switch"
sysnamesbottom="sysnames.bottom"

trap "rm $sysdcl $sysprotos $sysent $sysnamesbottom" 0

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

# before handing it off to awk, make a few adjustments:
#	(1) insert spaces around {, }, (, ), *, and commas.
#	(2) get rid of any and all dollar signs (so that rcs id use safe)
#
# The awk script will deal with blank lines and lines that
# start with the comment character (';').

sed -e '
s/\$//g
:join
	/\\$/{a\

	N
	s/\\\n//
	b join
	}
2,${
	/^#/!s/\([{}()*,]\)/ \1 /g
}
' < $2 | $awk "
$toupper
BEGIN {
	# to allow nested #if/#else/#endif sets
	savedepth = 0

	sysnames = \"$sysnames\"
	sysprotos = \"$sysprotos\"
	sysnumhdr = \"$sysnumhdr\"
	sysarghdr = \"$sysarghdr\"
	switchname = \"$switchname\"
	namesname = \"$namesname\"
	constprefix = \"$constprefix\"

	sysdcl = \"$sysdcl\"
	syscompat_pref = \"$syscompat_pref\"
	sysent = \"$sysent\"
	sysnamesbottom = \"$sysnamesbottom\"
	infile = \"$2\"

	compatopts = \"$compatopts\"
	"'

	printf "/*\t\$NetBSD\$\t*/\n\n" > sysdcl
	printf "/*\n * System call switch table.\n *\n" > sysdcl
	printf " * DO NOT EDIT-- this file is automatically generated.\n" > sysdcl

	ncompat = split(compatopts,compat)
	for (i = 1; i <= ncompat; i++) {
		compat_upper[i] = toupper(compat[i])

		printf "\n#ifdef %s\n", compat_upper[i] > sysent
		printf "#define %s(func) __CONCAT(%s_,func)\n", compat[i], \
		    compat[i] > sysent
		printf "#else\n" > sysent
		printf "#define %s(func) sys_nosys\n", compat[i] > sysent
		printf "#endif\n" > sysent
	}

	printf "\n#define\ts(type)\tsizeof(type)\n\n" > sysent
	printf "struct sysent %s[] = {\n",switchname > sysent

	printf "/*\t\$NetBSD\$\t*/\n\n" > sysnames
	printf "/*\n * System call names.\n *\n" > sysnames
	printf " * DO NOT EDIT-- this file is automatically generated.\n" > sysnames

	printf "\n/*\n * System call prototypes.\n */\n\n" > sysprotos

	printf "/*\t\$NetBSD\$\t*/\n\n" > sysnumhdr
	printf "/*\n * System call numbers.\n *\n" > sysnumhdr
	printf " * DO NOT EDIT-- this file is automatically generated.\n" > sysnumhdr

	printf "/*\t\$NetBSD\$\t*/\n\n" > sysarghdr
	printf "/*\n * System call argument lists.\n *\n" > sysarghdr
	printf " * DO NOT EDIT-- this file is automatically generated.\n" > sysarghdr
}
NR == 1 {
	printf " * created from%s\n */\n\n", $0 > sysdcl

	printf " * created from%s\n */\n\n", $0 > sysnames
	printf "\nchar *%s[] = {\n",namesname > sysnamesbottom

	printf " * created from%s\n */\n\n", $0 > sysnumhdr

	printf " * created from%s\n */\n\n", $0 > sysarghdr
	printf "#define\tsyscallarg(x)\tunion { x datum; register_t pad; }\n" \
		> sysarghdr
	next
}
NF == 0 || $1 ~ /^;/ {
	next
}
$1 ~ /^#[ 	]*include/ {
	print > sysdcl
	print > sysnames
	next
}
$1 ~ /^#[ 	]*if/ {
	print > sysent
	print > sysprotos
	print > sysnamesbottom
	savesyscall[++savedepth] = syscall
	next
}
$1 ~ /^#[ 	]*else/ {
	print > sysent
	print > sysprotos
	print > sysnamesbottom
	if (savedepth <= 0) {
		printf "%s: line %d: unbalenced #else\n", \
		    infile, NR
		exit 1
	}
	syscall = savesyscall[savedepth]
	next
}
$1 ~ /^#/ {
	if ($1 ~ /^#[       ]*endif/) {
		if (savedepth <= 0) {
			printf "%s: line %d: unbalenced #endif\n", \
			    infile, NR
			exit 1
		}
		savedepth--;
	}
	print > sysent
	print > sysprotos
	print > sysnamesbottom
	next
}
syscall != $1 {
	printf "%s: line %d: syscall number out of sync at %d\n", \
	   infile, NR, syscall
	printf "line is:\n"
	print
	exit 1
}
function parserr(was, wanted) {
	printf "%s: line %d: unexpected %s (expected %s)\n", \
	    infile, NR, was, wanted
	exit 1
}
function parseline() {
	f=3			# toss number and type
	if ($NF != "}") {
		funcalias=$NF
		end=NF-1
	} else {
		funcalias=""
		end=NF
	}
	if ($f != "{")
		parserr($f, "{")
	f++
	if ($end != "}")
		parserr($end, "}")
	end--
	if ($end != ";")
		parserr($end, ";")
	end--
	if ($end != ")")
		parserr($end, ")")
	end--

	returntype = oldf = "";
	do {
		if (returntype != "" && oldf != "*")
			returntype = returntype" ";
		returntype = returntype$f;
		oldf = $f;
		f++
	} while (f < (end - 1) && $(f+1) != "(");
	if (f == (end - 1)) {
		parserr($f, "function argument definition (maybe \"(\"?)");
	}

	funcname=$f
	if (funcalias == "") {
		funcalias=funcname
		sub(/^([^_]+_)*sys_/, "", funcalias)
	}
	f++

	if ($f != "(")
		parserr($f, ")")
	f++

	argc=0;
	if (f == end) {
		if ($f != "void")
			parserr($f, "argument definition")
		isvarargs = 0;
		varargc = 0;
		return
	}

	# some system calls (open() and fcntl()) can accept a variable
	# number of arguments.  If syscalls accept a variable number of
	# arguments, they must still have arguments specified for
	# the remaining argument "positions," because of the way the
	# kernel system call argument handling works.
	#
	# Indirect system calls, e.g. syscall(), are exceptions to this
	# rule, since they are handled entirely by machine-dependent code
	# and do not need argument structures built.

	isvarargs = 0;
	while (f <= end) {
		if ($f == "...") {
			f++;
			isvarargs = 1;
			varargc = argc;
			continue;
		}
		argc++
		argtype[argc]=""
		oldf=""
		while (f < end && $(f+1) != ",") {
			if (argtype[argc] != "" && oldf != "*")
				argtype[argc] = argtype[argc]" ";
			argtype[argc] = argtype[argc]$f;
			oldf = $f;
			f++
		}
		if (argtype[argc] == "")
			parserr($f, "argument definition")
		argname[argc]=$f;
		f += 2;			# skip name, and any comma
	}
	# must see another argument after varargs notice.
	if (isvarargs) {
		if (argc == varargc && $2 != "INDIR")
			parserr($f, "argument definition")
	} else
		varargc = argc;
}
function putent(nodefs, compatwrap) {
	# output syscall declaration for switch table.  INDIR functions
	# get none, since they always have sys_nosys() for their table
	# entries.
	if (nodefs != "INDIR") {
		prototype = "__P((struct proc *, void *, register_t *))"
		if (compatwrap == "")
			printf("int\t%s\t%s;\n", funcname,
			    prototype) > sysprotos
		else
			printf("int\t%s_%s\t%s;\n", compatwrap, funcname,
			    prototype) > sysprotos
	}

	# output syscall switch entry
	if (nodefs == "INDIR") {
		printf("\t{ 0, 0,\n\t    sys_nosys },\t\t\t/* %d = %s (indir) */\n", \
		    syscall, funcalias) > sysent
	} else {
#		printf("\t{ { %d", argc) > sysent
#		for (i = 1; i <= argc; i++) {
#			if (i == 5) 		# wrap the line
#				printf(",\n\t    ") > sysent
#			else
#				printf(", ") > sysent
#			printf("s(%s)", argtypenospc[i]) > sysent
#		}
		printf("\t{ %d, ", argc) > sysent
		if (argc == 0)
			printf("0") > sysent
		else if (compatwrap == "")
			printf("s(struct %s_args)", funcname) > sysent
		else
			printf("s(struct %s_%s_args)", compatwrap,
			    funcname) > sysent
		if (compatwrap == "")
			wfn = sprintf("%s", funcname);
		else
			wfn = sprintf("%s(%s)", compatwrap, funcname);
		printf(",\n\t    %s },", wfn) > sysent
		for (i = 0; i < (33 - length(wfn)) / 8; i++)
			printf("\t") > sysent
		if (compatwrap == "")
			printf("/* %d = %s */\n", syscall, funcalias) > sysent
		else
			printf("/* %d = %s %s */\n", syscall, compatwrap,
			    funcalias) > sysent
	}

	# output syscall name for names table
	if (compatwrap == "")
		printf("\t\"%s\",\t\t\t/* %d = %s */\n", funcalias, syscall,
		    funcalias) > sysnamesbottom
	else
		printf("\t\"%s_%s\",\t/* %d = %s %s */\n", compatwrap,
		    funcalias, syscall, compatwrap, funcalias) > sysnamesbottom

	# output syscall number of header, if appropriate
	if (nodefs == "" || nodefs == "NOARGS" || nodefs == "INDIR") {
		# output a prototype, to be used to generate lint stubs in
		# libc.
		printf("/* syscall: \"%s\" ret: \"%s\" args:", funcalias,
		    returntype) > sysnumhdr
		for (i = 1; i <= varargc; i++)
			printf(" \"%s\"", argtype[i]) > sysnumhdr
		if (isvarargs)
			printf(" \"...\"") > sysnumhdr
		printf(" */\n") > sysnumhdr

		printf("#define\t%s%s\t%d\n\n", constprefix, funcalias,
		    syscall) > sysnumhdr
	} else if (nodefs != "NODEF")
		printf("\t\t\t\t/* %d is %s %s */\n\n", syscall,
		    compatwrap, funcalias) > sysnumhdr

	# output syscall argument structure, if it has arguments
	if (argc != 0 && nodefs != "NOARGS" && nodefs != "INDIR") {
		if (compatwrap == "")
			printf("\nstruct %s_args {\n", funcname) > sysarghdr
		else
			printf("\nstruct %s_%s_args {\n", compatwrap,
			    funcname) > sysarghdr
		for (i = 1; i <= argc; i++)
			printf("\tsyscallarg(%s) %s;\n", argtype[i],
			    argname[i]) > sysarghdr
		printf("};\n") > sysarghdr
	}
}
$2 == "STD" {
	parseline()
	putent("", "");
	syscall++
	next
}
$2 == "NODEF" || $2 == "NOARGS" || $2 == "INDIR" {
	parseline()
	putent($2, "")
	syscall++
	next
}
$2 == "OBSOL" || $2 == "UNIMPL" {
	if ($2 == "OBSOL")
		comment="obsolete"
	else
		comment="unimplemented"
	for (i = 3; i <= NF; i++)
		comment=comment " " $i

	printf("\t{ 0, 0,\n\t    sys_nosys },\t\t\t/* %d = %s */\n", \
	    syscall, comment) > sysent
	printf("\t\"#%d (%s)\",\t\t/* %d = %s */\n", \
	    syscall, comment, syscall, comment) > sysnamesbottom
	if ($2 != "UNIMPL")
		printf("\t\t\t\t/* %d is %s */\n", syscall, comment) > sysnumhdr
	syscall++
	next
}
{
	for (i = 1; i <= ncompat; i++) {
		if ($2 == compat_upper[i]) {
			parseline();
			putent("COMMENT", compat[i])
			syscall++
			next
		}
	}
	printf "%s: line %d: unrecognized keyword %s\n", infile, NR, $2
	exit 1
}
END {
	printf("};\n\n") > sysent
	printf("};\n") > sysnamesbottom
	printf("#define\t%sMAXSYSCALL\t%d\n", constprefix, syscall) > sysnumhdr
} '

cat $sysprotos >> $sysarghdr
cat $sysdcl $sysent > $syssw
cat $sysnamesbottom >> $sysnames

#chmod 444 $sysnames $sysnumhdr $syssw
