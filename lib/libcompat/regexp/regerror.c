#include <sys/cdefs.h>
#ifndef lint
__RCSID("$NetBSD: regerror.c,v 1.6 1999/05/04 17:14:54 christos Exp $");
#endif /* not lint */

#include <regexp.h>
#include <stdio.h>

/*ARGSUSED*/
void
__compat_regerror(s)
const char *s;
{
#ifdef ERRAVAIL
	error("regexp: %s", s);
#else
/*
	fprintf(stderr, "regexp(3): %s\n", s);
	exit(1);
*/
	return;	  /* let std. egrep handle errors */
#endif
	/* NOTREACHED */
}
