#!/bin/sh -
# This script runs the .ed scripts generated by mkscripts.sh
# and compares their output against the .r files, which contain
# the correct output

PATH="/bin:/usr/bin:/usr/local/bin/:."
[ X"$ED" = X ] && ED="../ed"

for i in *.ed; do
	base=`echo $i | sed 's/\..*/'`
#	base=`$ED - <<-EOF
#	r !echo $i
#	s/\..*
#	EOF`
	if $base.ed >/dev/null 2>&1; then
		if cmp -s $base.o $base.r; then :; else
			echo "*** Output $base.o of script $i is incorrect ***"
		fi
	else
		echo "*** The script $i exited abnormally ***"
	fi
done
