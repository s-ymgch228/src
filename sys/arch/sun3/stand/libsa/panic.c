/*	$NetBSD: panic.c,v 1.4 1998/01/05 07:03:27 perry Exp $	*/


#include <stdarg.h>
#include "stand.h"

extern __dead void abort();

__dead void
panic(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vprintf(fmt, ap);
	printf("\n");
	va_end(ap);
	abort();
}
