#! /usr/bin/awk -f
#	$NetBSD: devlist2h.awk,v 1.4 2003/12/15 07:32:20 jmc Exp $
#
# Copyright (c) 1998 The NetBSD Foundation, Inc.
# All rights reserved.
#
# This code is derived from software contributed to The NetBSD Foundation
# by Christos Zoulas.
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
#        This product includes software developed by the NetBSD
#        Foundation, Inc. and its contributors.
# 4. Neither the name of The NetBSD Foundation nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
# ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
# TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
# BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
BEGIN {
	nproducts = 0
	dfile="acpidevs_data.h"
}
NR == 1 {
	VERSION = $0
	gsub("\\$", "", VERSION)

	printf("/*\t$NetBSD" "$\t*/\n\n") > dfile
	printf("/*\n") > dfile
	printf(" * THIS FILE AUTOMATICALLY GENERATED.  DO NOT EDIT.\n") \
	    > dfile
	printf(" *\n") > dfile
	printf(" * generated from:\n") > dfile
	printf(" *\t%s\n", VERSION) > dfile
	printf(" */\n") > dfile
	nproducts = 0;

	next
}
/^#/ {
	next
}
/^[A-Z][A-Z][A-Z].*/ {
	nproducts++

	products[nproducts, 1] = toupper($1);
	$1 = "";
	sub ("^ *", "");
	products[nproducts, 2] = $0;
	next
}
END {
	printf("\n") > dfile

	printf("const struct { const char *pnp, *str; } acpi_knowndevs[] = {\n") > dfile
	for (i = 1; i <= nproducts; i++) {
		printf("\t{\n") > dfile
		printf("\t    \"%s\",\n", products[i, 1]) > dfile
		printf("\t    \"%s\",\n", products[i, 2]) > dfile
		printf("\t},\n") > dfile
	}
	printf("};\n") > dfile
	close(dfile)
}
