#!/bin/sh
#
# Copyright (C) 2010  Internet Systems Consortium, Inc. ("ISC")
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
# REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
# INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
# LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
# OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.

# Id: tests.sh,v 1.2.2.2 2010/08/16 05:14:14 marka Exp

SYSTEMTESTTOP=..
. $SYSTEMTESTTOP/conf.sh

status=0
n=0

rm -f dig.out.*

DIGOPTS="+tcp +noadd +nosea +nostat +nocmd +dnssec -p 5300"

# Check the example.com. domain

echo "I:checking DNAME at apex works ($n)"
ret=0
$DIG $DIGOPTS +norec foo.example.com. \
	@10.53.0.1 a > dig.out.ns1.test$n || ret=1
grep "status: NOERROR" dig.out.ns1.test$n > /dev/null || ret=1
grep "example.com..*DNAME.*example.net." dig.out.ns1.test$n > /dev/null || ret=1
grep "foo.example.com..*CNAME.*foo.example.net." dig.out.ns1.test$n > /dev/null || ret=1
n=`expr $n + 1`
if [ $ret != 0 ]; then echo "I:failed"; fi
status=`expr $status + $ret`

echo "I:exit status: $status"
exit $status
