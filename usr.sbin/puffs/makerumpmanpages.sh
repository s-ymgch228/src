#!/bin/sh
#
#	$NetBSD: makerumpmanpages.sh,v 1.10 2010/11/21 19:29:01 pooka Exp $
#

IFS=' '
MANTMPL=".\\\"	\$NetBSD\$"'
.\"
.\"	WARNING: GENERATED FILE, DO NOT EDIT
.\"	INSTEAD, EDIT makerumpmanpages.sh AND REGEN
.\"
.\" Copyright (c) 2008-2010 Antti Kantee.  All rights reserved.
.\"
.\" Redistribution and use in source and binary forms, with or without
.\" modification, are permitted provided that the following conditions
.\" are met:
.\" 1. Redistributions of source code must retain the above copyright
.\"    notice, this list of conditions and the following disclaimer.
.\" 2. Redistributions in binary form must reproduce the above copyright
.\"    notice, this list of conditions and the following disclaimer in the
.\"    documentation and/or other materials provided with the distribution.
.\"
.\" THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS "AS IS" AND
.\" ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
.\" IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
.\" ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
.\" FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
.\" DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
.\" OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
.\" HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
.\" LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
.\" OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
.\" SUCH DAMAGE.
.\"
.Dd November 21, 2010
.Dt RUMP_XXXFSXXX 8
.Os
.Sh NAME
.Nm rump_xxxfsxxx
.Nd mount the xxxfsxxx file system using a userspace server
.Sh SYNOPSIS
.Cd "file-system PUFFS"
.Cd "pseudo-device putter"
.Pp
.Nm
.Op options
.Ar image
.Ar mountpoint
.Sh DESCRIPTION
.Em NOTE!
This manual page has been generated from a common source shared between all
.Xr rump 3
file servers.
Some parts of this manual page may not apply to this particular server.
After reading this manual page, you may want to verify the details from
.Xr mount_xxxfsxxx 8 .
.Pp
The
.Nm
utility can be used to mount xxxfsxxx file systems.
It uses
.Xr rump 3
and
.Xr p2k 3
to facilitate running the file system as a server in userspace.
As opposed to
.Xr mount_xxxfsxxx 8 ,
.Nm
does not use file system code within the kernel and therefore does
not require kernel support except
.Xr puffs 4 .
Apart from a minor speed penalty there is no downside with respect to
in-kernel code.
.Pp
.Nm
does not require using
.Xr vnconfig 8
for mounts from regular files and the file path can be passed
directly as the
.Ar image
parameter.
In fact, the use of
.Xr vnconfig 8
is discouraged, since it is unable to properly deal with images on
sparse files.
.Pp
In case the image contains multiple partitions, the desired partition
must be indicated by appending the token
.Dq %DISKLABEL:p%
to the
.Ar image
path.
The letter
.Dq p
specifies the partition as obtained via
.Xr disklabel 8 .
For example, to mount partition
.Dq e
from image
.Pa /tmp/wd0.img ,
use
.Dq /tmp/wd0.img%DISKLABEL:e% .
.Pp
It is recommended that untrusted file system images be mounted with
.Nm
instead of
.Xr mount_xxxfsxxx 8 .
Corrupt file system images commonly cause the file system
to crash the entire kernel, but with
.Nm
only the userspace server process will dump core.
.Pp
To use
.Nm
via
.Xr mount 8 ,
the flags
.Fl o Ar rump
and
.Fl t Ar xxxfsxxx
should be given.
Similarly,
.Nm
is run instead of
.Xr mount_xxxfsxxx 8
if
.Dq rump
is added to the options field of
.Xr fstab 5 .
.Pp
Please see
.Xr mount_xxxfsxxx 8
for a full description of the available command line options.
.Sh SEE ALSO
.Xr p2k 3 ,
.Xr puffs 3 ,
.Xr rump 3 ,
.Xr mount_xxxfsxxx 8
.Sh HISTORY
The
.Nm
utility first appeared in
.Nx 5.0 .'

# generate the manual pages
#
for x in rump_*
do
	fs=${x#rump_}

	# syspuffs is special, it has a handwritten manpage
	if [ "$fs" = "syspuffs" ]
	then
		continue
	fi

	fsc=`echo $fs | tr '[:lower:]' '[:upper:]'`
	echo ${MANTMPL} | sed "s/xxxfsxxx/$fs/g;s/XXXFSXXX/$fsc/g" > \
	    rump_${fs}/rump_${fs}.8
done
