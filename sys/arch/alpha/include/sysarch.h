/* $NetBSD: sysarch.h,v 1.4 2000/02/26 18:53:12 thorpej Exp $ */

/*-
 * Copyright (c) 2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the NetBSD
 *	Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _ALPHA_SYSARCH_H_
#define _ALPHA_SYSARCH_H_

#include <machine/bus.h>
#include <machine/ieeefp.h>

/*
 * Architecture specific syscalls (ALPHA)
 */

#define	ALPHA_FPGETMASK			0
#define	ALPHA_FPSETMASK			1
#define	ALPHA_FPSETSTICKY		2
struct alpha_fp_except_args {
	fp_except mask;
};

#define	ALPHA_BUS_GET_WINDOW_COUNT	3
struct alpha_bus_get_window_count_args {
	u_int type;
	u_int count;	/* output */
};

#define	ALPHA_BUS_GET_WINDOW		4
struct alpha_bus_get_window_args {
	u_int type;
	u_int window;
	struct alpha_bus_space_translation *translation; /* output */
};

#define	ALPHA_BUS_TYPE_PCI_IO		0
#define	ALPHA_BUS_TYPE_PCI_MEM		1
#define	ALPHA_BUS_TYPE_MAX		1

#define	ALPHA_PCI_CONF_READWRITE	5
struct alpha_pci_conf_readwrite_args {
	int write;
	u_int bus;
	u_int device;
	u_int function;
	u_int reg;
	u_int32_t val;
};

#ifdef _KERNEL
extern	u_int alpha_bus_window_count[];
extern	int (*alpha_bus_get_window) __P((int, int,
	    struct alpha_bus_space_translation *));
extern	struct alpha_pci_chipset *alpha_pci_chipset;
#else
struct alpha_bus_window {
	caddr_t		abw_addr;
	size_t		abw_size;
	struct alpha_bus_space_translation abw_abst;
};

int	alpha_bus_getwindows __P((int, struct alpha_bus_window **));
int	alpha_bus_mapwindow __P((struct alpha_bus_window *));
void	alpha_bus_unmapwindow __P((struct alpha_bus_window *));

void	*alpha_pci_mem_map __P((bus_addr_t, bus_size_t, int,
	    struct alpha_bus_space_translation *));
void	alpha_pci_mem_unmap __P((void *addr, bus_size_t));

u_int32_t alpha_pci_conf_read __P((u_int, u_int, u_int, u_int));
void	alpha_pci_conf_write __P((u_int, u_int, u_int, u_int, u_int32_t));

int	sysarch __P((int, void *));
#endif /* _KERNEL */

#endif /* !_ALPHA_SYSARCH_H_ */
