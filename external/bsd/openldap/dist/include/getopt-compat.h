/*	$NetBSD: getopt-compat.h,v 1.1.1.3 2010/12/12 15:21:22 adam Exp $	*/

/* getopt-compat.h -- getopt(3) compatibility header */
/* OpenLDAP: pkg/ldap/include/getopt-compat.h,v 1.19.2.5 2010/04/13 20:22:47 kurt Exp */
/* This work is part of OpenLDAP Software <http://www.openldap.org/>.
 *
 * Copyright 1998-2010 The OpenLDAP Foundation.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted only as authorized by the OpenLDAP
 * Public License.
 *
 * A copy of this license is available in file LICENSE in the
 * top-level directory of the distribution or, alternatively, at
 * <http://www.OpenLDAP.org/license.html>.
 */

/*
 * getopt(3) declarations
 */
#ifndef _GETOPT_COMPAT_H
#define _GETOPT_COMPAT_H

#include <ldap_cdefs.h>

LDAP_BEGIN_DECL

/* change symbols to avoid clashing */
#define optarg lutil_optarg
#define optind lutil_optind
#define opterr lutil_opterr
#define optopt lutil_optopt
#define getopt lutil_getopt

LDAP_LUTIL_V (char *) optarg;
LDAP_LUTIL_V (int) optind, opterr, optopt;
LDAP_LUTIL_F (int) getopt LDAP_P(( int, char * const [], const char *));

LDAP_END_DECL

#endif /* _GETOPT_COMPAT_H */
