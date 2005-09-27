/* $NetBSD: gscpcibreg.h,v 1.1 2005/09/27 02:42:44 jmcneill Exp $ */
/*	$OpenBSD: gscpcibreg.h,v 1.1 2004/06/03 18:22:21 grange Exp $	*/
/*
 * Copyright (c) 2004 Alexander Yurchenko <grange@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _I386_PCI_GSCGPIOREG_H_
#define _I386_PCI_GSCGPIOREG_H_

#define GSCGPIO_NPINS	64

#define GSCGPIO_BASE	0x10
#define GSCGPIO_SIZE	64

#define GSCGPIO_GPDO0	0x00
#define GSCGPIO_GPDI0	0x04
#define GSCGPIO_GPIEN0	0x08
#define GSCGPIO_GPST0	0x0c
#define GSCGPIO_GPDO1	0x10
#define GSCGPIO_GPDI1	0x14
#define GSCGPIO_GPIEN1	0x18
#define GSCGPIO_GPST1	0x1c
#define GSCGPIO_SEL	0x20
#define GSCGPIO_CONF	0x24
#define GSCGPIO_RESET	0x28

#define GSCGPIO_CONF_OUTPUTEN	(1 << 0)
#define GSCGPIO_CONF_PUSHPULL	(1 << 1)
#define GSCGPIO_CONF_PULLUP	(1 << 2)

#endif	/* !_I386_PCI_GSCGPIOREG_H_ */
