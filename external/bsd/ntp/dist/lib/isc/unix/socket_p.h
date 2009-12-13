/*	$NetBSD: socket_p.h,v 1.1.1.1 2009/12/13 16:54:33 kardel Exp $	*/

/*
 * Copyright (C) 2004, 2005, 2007, 2008  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 2000, 2001  Internet Software Consortium.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/* Id: socket_p.h,v 1.13 2008/06/23 23:47:11 tbox Exp */

#ifndef ISC_SOCKET_P_H
#define ISC_SOCKET_P_H

/*! \file */

#ifdef ISC_PLATFORM_NEEDSYSSELECTH
#include <sys/select.h>
#endif

typedef struct isc_socketwait isc_socketwait_t;
int isc__socketmgr_waitevents(struct timeval *, isc_socketwait_t **);
isc_result_t isc__socketmgr_dispatch(isc_socketwait_t *);
#endif /* ISC_SOCKET_P_H */
