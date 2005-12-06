/*	$NetBSD: types.h,v 1.15 2005/12/06 17:06:00 tsutsui Exp $	*/

#ifndef _HP300_TYPES_H_
#define	_HP300_TYPES_H_

#include <m68k/types.h>

#define	__HAVE_DEVICE_REGISTER
#define	__HAVE_GENERIC_SOFT_INTERRUPTS

#if defined(_KERNEL)
#define	__HAVE_RAS
#endif

#endif /* !_HP300_TYPES_H_ */
