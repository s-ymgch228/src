/*	$NetBSD: memcmp.c,v 1.1.1.3 2010/12/12 15:22:09 adam Exp $	*/

/* OpenLDAP: pkg/ldap/libraries/liblutil/memcmp.c,v 1.9.2.5 2010/04/13 20:23:06 kurt Exp */
/* This work is part of OpenLDAP Software <http://www.openldap.org/>.
 *
 * Copyright 1998-2010 The OpenLDAP Foundation.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted only as authorized by the OpenLDAP
 * Public License.
 *
 * A copy of this license is available in the file LICENSE in the
 * top-level directory of the distribution or, alternatively, at
 * <http://www.OpenLDAP.org/license.html>.
 */

#include "portable.h"

#include <ac/string.h>

/* 
 * Memory Compare
 */
int
(lutil_memcmp)(const void *v1, const void *v2, size_t n) 
{
    if (n != 0) {
		const unsigned char *s1=v1, *s2=v2;
        do {
            if (*s1++ != *s2++) return *--s1 - *--s2;
        } while (--n != 0);
    }
    return 0;
} 
