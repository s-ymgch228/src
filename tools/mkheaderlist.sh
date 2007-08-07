#!/bin/sh
# $NetBSD: mkheaderlist.sh,v 1.1 2007/08/07 12:38:23 yamt Exp $

set -e

mklist()
{
	echo "# \$NetBSD\$"
	echo "#"
	echo '# do not edit; this file was automatically generated by:'
	echo '#	$NetBSD: mkheaderlist.sh,v 1.1 2007/08/07 12:38:23 yamt Exp $' | sed -e 's/\$//g'
	echo
	cd ../sys/arch && find . -name disklabel.h -o -name elf_machdep.h
}

LIST=headerlist
rm -f ${LIST}
mklist > ${LIST}
