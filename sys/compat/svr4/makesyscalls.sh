#! /bin/sh -
#	$NetBSD: makesyscalls.sh,v 1.2 1994/06/29 06:30:30 cgd Exp $
#
#	@(#)makesyscalls.sh	8.1 (Berkeley) 6/11/93

set -e

# name of compat option:
compat=XXX_UNUSED

# output files:
sysnames="svr4_syscalls.c"
syshdr="svr4_syscall.h"
syssw="svr4_sysent.c"

# tmp files:
sysdcl="sysent.dcl"
syscompat="sysent.compat"
sysent="sysent.switch"

trap "rm $sysdcl $syscompat $sysent" 0

case $# in
    0)	echo "Usage: $0 input-file" 1>&2
	exit 1
	;;
esac

awk < $1 "
	BEGIN {
		sysdcl = \"$sysdcl\"
		syscompat = \"$syscompat\"
		sysent = \"$sysent\"
		sysnames = \"$sysnames\"
		syshdr = \"$syshdr\"
		compat = \"$compat\"
		infile = \"$1\"
		"'

		printf "/*\n * System call switch table.\n *\n" > sysdcl
		printf " * DO NOT EDIT-- this file is automatically generated.\n" > sysdcl

		printf "\n#ifdef %s\n", compat > syscompat
		printf "#define compat(n, name) n, __CONCAT(o,name)\n\n" > syscompat

		printf "/*\n * System call names.\n *\n" > sysnames
		printf " * DO NOT EDIT-- this file is automatically generated.\n" > sysnames

		printf "/*\n * System call numbers.\n *\n" > syshdr
		printf " * DO NOT EDIT-- this file is automatically generated.\n" > syshdr
	}
	NR == 1 {
		printf " * created from: %s %s %s %s\n */\n\n",$1,$3,$4,$5,$6 > sysdcl
		printf "#include <sys/param.h>\n" > sysdcl
		printf "#include <sys/systm.h>\n\n" > sysdcl
		printf "int\tnosys();\n\n" > sysdcl

		printf "struct sysent svr4_sysent[] = {\n" > sysent

		printf " * created from: %s %s %s %s\n */\n\n",$1,$3,$4,$5,$6 > sysnames
		printf "char *svr4_syscallnames[] = {\n" > sysnames

		printf " * created from: %s %s %s %s\n */\n\n",$1,$3,$4,$5,$6 > syshdr
		next
	}
	NF == 0 || $1 ~ /^;/ {
		next
	}
	$1 ~ /^#[ 	]*if/ {
		print > sysent
		print > sysdcl
		print > syscompat
		print > sysnames
		savesyscall = syscall
		next
	}
	$1 ~ /^#[ 	]*else/ {
		print > sysent
		print > sysdcl
		print > syscompat
		print > sysnames
		syscall = savesyscall
		next
	}
	$1 ~ /^#/ {
		print > sysent
		print > sysdcl
		print > syscompat
		print > sysnames
		next
	}
	syscall != $1 {
		printf "%s: line %d: syscall number out of sync at %d\n", \
		   infile, NR, syscall
		printf "line is:\n"
		print
		exit 1
	}
	{	comment = $4
		for (i = 5; i <= NF; i++)
			comment = comment " " $i
		if (NF < 5)
			$5 = $4
	}
	$2 == "STD" {
		printf("int\t%s();\n", $4) > sysdcl
		printf("\t{ %d, %s },\t\t\t/* %d = %s */\n", \
		    $3, $4, syscall, $5) > sysent
		printf("\t\"%s\",\t\t\t/* %d = %s */\n", \
		    $5, syscall, $5) > sysnames
		printf("#define\tSVR4_SYS_%s\t%d\n", \
		    $5, syscall) > syshdr
		syscall++
		next
	}
	$2 == "COMPAT" {
		printf("int\to%s();\n", $4) > syscompat
		printf("\t{ compat(%d,%s) },\t\t/* %d = old %s */\n", \
		    $3, $4, syscall, $5) > sysent
		printf("\t\"old.%s\",\t\t/* %d = old %s */\n", \
		    $5, syscall, $5) > sysnames
		printf("\t\t\t\t/* %d is old %s */\n", \
		    syscall, comment) > syshdr
		syscall++
		next
	}
	$2 == "LIBCOMPAT" {
		printf("int\to%s();\n", $4) > syscompat
		printf("\t{ compat(%d,%s) },\t\t/* %d = old %s */\n", \
		    $3, $4, syscall, $5) > sysent
		printf("\t\"old.%s\",\t\t/* %d = old %s */\n", \
		    $5, syscall, $5) > sysnames
		printf("#define\tSVR4_SYS_%s\t%d\t/* compatibility; still used by libc */\n", \
		    $5, syscall) > syshdr
		syscall++
		next
	}
	$2 == "OBSOL" {
		printf("\t{ 0, nosys },\t\t\t/* %d = obsolete %s */\n", \
		    syscall, comment) > sysent
		printf("\t\"obs_%s\",\t\t\t/* %d = obsolete %s */\n", \
		    $4, syscall, comment) > sysnames
		printf("\t\t\t\t/* %d is obsolete %s */\n", \
		    syscall, comment) > syshdr
		syscall++
		next
	}
	$2 == "UNIMPL" {
		printf("\t{ 0, nosys },\t\t\t/* %d = %s */\n", \
		    syscall, comment) > sysent
		printf("\t\"#%d\",\t\t\t/* %d = %s */\n", \
		    syscall, syscall, comment) > sysnames
		syscall++
		next
	}
	{
		printf "%s: line %d: unrecognized keyword %s\n", infile, NR, $2
		exit 1
	}
	END {
		printf("\n#else /* %s */\n", compat) > syscompat
		printf("#define compat(n, name) 0, nosys\n") > syscompat
		printf("#endif /* %s */\n\n", compat) > syscompat

		printf("};\n\n") > sysent
		printf("int\tnsvr4_sysent = sizeof(svr4_sysent) / sizeof(svr4_sysent[0]);\n") > sysent

		printf("};\n") > sysnames
	} '

cat $sysdcl $syscompat $sysent >$syssw

#chmod 444 $sysnames $syshdr $syssw
