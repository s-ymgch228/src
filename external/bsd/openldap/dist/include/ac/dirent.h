/*	$NetBSD: dirent.h,v 1.1.1.3 2010/12/12 15:21:26 adam Exp $	*/

/* Generic dirent.h */
/* OpenLDAP: pkg/ldap/include/ac/dirent.h,v 1.14.2.7 2010/04/13 20:22:51 kurt Exp */
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

#ifndef _AC_DIRENT_H
#define _AC_DIRENT_H

#ifdef HAVE_DIRENT_H
# include <dirent.h>
# define NAMLEN(dirent) strlen((dirent)->d_name)
#elif defined(_MSC_VER)
#include <windows.h>
#ifndef MAX_PATH
#define MAX_PATH	260
#endif
struct dirent {
	char *d_name;
};
typedef struct DIR {
	HANDLE dir;
	struct dirent data;
	int first;
	char buf[MAX_PATH+1];
} DIR;
DIR *opendir(const char *name);
struct dirent *readdir(DIR *dir);
int closedir(DIR *dir);
#else
# define dirent direct
# define NAMLEN(dirent) (dirent)->d_namlen
# ifdef HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# ifdef HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# ifdef HAVE_NDIR_H
#  include <ndir.h>
# endif
#endif

#endif /* _AC_DIRENT_H */
