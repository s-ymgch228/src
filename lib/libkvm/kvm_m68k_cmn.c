/*	$NetBSD: kvm_m68k_cmn.c,v 1.4 1997/08/15 02:22:00 mikel Exp $	*/

/*-
 * Copyright (c) 1997 Jason R. Thorpe.  All rights reserved.
 * Copyright (c) 1989, 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software developed by the Computer Systems
 * Engineering group at Lawrence Berkeley Laboratory under DARPA contract
 * BG 91-66 and contributed to Berkeley.
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
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
#if 0
static char sccsid[] = "@(#)kvm_hp300.c	8.1 (Berkeley) 6/4/93";
#else
__RCSID("$NetBSD: kvm_m68k_cmn.c,v 1.4 1997/08/15 02:22:00 mikel Exp $");
#endif
#endif /* LIBC_SCCS and not lint */

/*
 * Common m68k machine dependent routines for kvm.
 *
 * Note: This file has to build on ALL m68k machines,
 * so do NOT include any <machine/*.h> files here.
 */

#include <sys/types.h>
#include <sys/kcore.h>

#include <unistd.h>
#include <limits.h>
#include <nlist.h>
#include <kvm.h>
#include <db.h>

#include <m68k/cpu.h>
#include <m68k/kcore.h>

#include "kvm_private.h"
#include "kvm_m68k.h"

int   _kvm_cmn_initvtop __P((kvm_t *));
void  _kvm_cmn_freevtop __P((kvm_t *));
int	  _kvm_cmn_kvatop   __P((kvm_t *, u_long, u_long *));
off_t _kvm_cmn_pa2off   __P((kvm_t *, u_long));

struct kvm_ops _kvm_ops_cmn = {
	_kvm_cmn_initvtop,
	_kvm_cmn_freevtop,
	_kvm_cmn_kvatop,
	_kvm_cmn_pa2off };

static int vatop_030 __P((kvm_t *, u_int32_t, u_long, u_long *));
static int vatop_040 __P((kvm_t *, u_int32_t, u_long, u_long *));

#define	_kvm_btop(v, a)	(((unsigned)(a)) >> (v)->pgshift)

#define KREAD(kd, addr, p)\
	(kvm_read(kd, addr, (char *)(p), sizeof(*(p))) != sizeof(*(p)))

void
_kvm_cmn_freevtop(kd)
	kvm_t *kd;
{
	/* No private state information to keep. */
}

int
_kvm_cmn_initvtop(kd)
	kvm_t *kd;
{
	/* No private state information to keep. */
	return (0);
}

int
_kvm_cmn_kvatop(kd, va, pa)
	kvm_t *kd;
	u_long va;
	u_long *pa;
{
	cpu_kcore_hdr_t *h = kd->cpu_data;
	struct m68k_kcore_hdr *m = &h->un._m68k;
	struct vmstate *vm = kd->vmst;
	int (*vtopf) __P((kvm_t *, u_int32_t, u_long, u_long *));

	if (ISALIVE(kd)) {
		_kvm_err(kd, 0, "vatop called in live kernel!");
		return (0);
	}

	/*
	 * 68040 and 68040 use same translation functions,
	 * as do 68030, 68851, HP MMU.
	 */
	if (m->mmutype == MMU_68040 || m->mmutype == MMU_68060)
		vtopf = vatop_040;
	else
		vtopf = vatop_030;

	return ((*vtopf)(kd, m->sysseg_pa, va, pa));
}

/*
 * Translate a physical address to a file-offset in the crash-dump.
 */
off_t
_kvm_cmn_pa2off(kd, pa)
	kvm_t	*kd;
	u_long	pa;
{
	cpu_kcore_hdr_t *h = kd->cpu_data;
	struct m68k_kcore_hdr *m = &h->un._m68k;
	phys_ram_seg_t *rsp;
	off_t off;
	int i;

	off = 0;
	rsp = m->ram_segs;
	for (i = 0; i < M68K_NPHYS_RAM_SEGS && rsp[i].size != 0; i++) {
		if (pa >= rsp[i].start &&
		    pa < (rsp[i].start + rsp[i].size)) {
			pa -= rsp[i].start;
			break;
		}
		off += rsp[i].size;
	}
	return (kd->dump_off + off + pa);
}

/*****************************************************************
 * Local stuff...
 */

static int
vatop_030(kd, stpa, va, pa)
	kvm_t *kd;
	u_int32_t stpa;
	u_long va;
	u_long *pa;
{
	cpu_kcore_hdr_t *h = kd->cpu_data;
	struct m68k_kcore_hdr *m = &h->un._m68k;
	struct vmstate *vm = kd->vmst;
	u_long addr;
	u_int32_t ste, pte;
	u_int p, offset;

	offset = va & vm->pgofset;

	/*
	 * We may be called before address translation is initialized.
	 * This is typically used to find the dump magic number.  This
	 * means we do not yet have the kernel page tables available,
	 * so we must to a simple relocation.
	 */
	if (va < m->relocend) {
		*pa = (va - h->kernbase) + m->reloc;
		return (h->page_size - offset);
	}

	addr = stpa + ((va >> m->sg_ishift) * sizeof(u_int32_t));

	/*
	 * Can't use KREAD to read kernel segment table entries.
	 * Fortunately it is 1-to-1 mapped so we don't have to. 
	 */
	if (stpa == m->sysseg_pa) {
		if (lseek(kd->pmfd, _kvm_cmn_pa2off(kd, addr), 0) == -1 ||
			read(kd->pmfd, (char *)&ste, sizeof(ste)) < 0)
			goto invalid;
	} else if (KREAD(kd, addr, &ste))
		goto invalid;
	if ((ste & m->sg_v) == 0) {
		_kvm_err(kd, 0, "invalid segment (%x)", ste);
		return(0);
	}
	p = _kvm_btop(vm, va & m->sg_pmask);
	addr = (ste & m->sg_frame) + (p * sizeof(u_int32_t));

	/*
	 * Address from STE is a physical address so don't use kvm_read.
	 */
	if (lseek(kd->pmfd, _kvm_cmn_pa2off(kd, addr), 0) == -1 || 
	    read(kd->pmfd, (char *)&pte, sizeof(pte)) < 0)
		goto invalid;
	addr = pte & m->pg_frame;
	if ((pte & m->pg_v) == 0) {
		_kvm_err(kd, 0, "page not valid");
		return (0);
	}
	*pa = addr + offset;
	
	return (h->page_size - offset);
invalid:
	_kvm_err(kd, 0, "invalid address (%x)", va);
	return (0);
}

static int
vatop_040(kd, stpa, va, pa)
	kvm_t *kd;
	u_int32_t stpa;
	u_long va;
	u_long *pa;
{
	cpu_kcore_hdr_t *h = kd->cpu_data;
	struct m68k_kcore_hdr *m = &h->un._m68k;
	struct vmstate *vm = kd->vmst;
	u_long addr;
	u_int32_t stpa2;
	u_int32_t ste, pte;
	u_int p, offset;

	offset = va & vm->pgofset;

	/*
	 * We may be called before address translation is initialized.
	 * This is typically used to find the dump magic number.  This
	 * means we do not yet have the kernel page tables available,
	 * so we must to a simple relocation.
	 */
	if (va < m->relocend) {
		*pa = (va - h->kernbase) + m->reloc;
		return (h->page_size - offset);
	}

	addr = stpa + ((va >> m->sg40_shift1) * sizeof(u_int32_t));

	/*
	 * Can't use KREAD to read kernel segment table entries.
	 * Fortunately it is 1-to-1 mapped so we don't have to. 
	 */
	if (stpa == m->sysseg_pa) {
		if (lseek(kd->pmfd, _kvm_cmn_pa2off(kd, addr), 0) == -1 ||
			read(kd->pmfd, (char *)&ste, sizeof(ste)) < 0)
			goto invalid;
	} else if (KREAD(kd, addr, &ste))
		goto invalid;
	if ((ste & m->sg_v) == 0) {
		_kvm_err(kd, 0, "invalid level 1 descriptor (%x)",
				 ste);
		return((off_t)0);
	}
	stpa2 = (ste & m->sg40_addr1);
	addr = stpa2 + (((va & m->sg40_mask2) >> m->sg40_shift2) *
	    sizeof(u_int32_t));

	/*
	 * Address from level 1 STE is a physical address,
	 * so don't use kvm_read.
	 */
	if (lseek(kd->pmfd, _kvm_cmn_pa2off(kd, addr), 0) == -1 || 
		read(kd->pmfd, (char *)&ste, sizeof(ste)) < 0)
		goto invalid;
	if ((ste & m->sg_v) == 0) {
		_kvm_err(kd, 0, "invalid level 2 descriptor (%x)",
				 ste);
		return((off_t)0);
	}
	stpa2 = (ste & m->sg40_addr2);
	addr = stpa2 + (((va & m->sg40_mask3) >> m->sg40_shift3) *
	    sizeof(u_int32_t));

	/*
	 * Address from STE is a physical address so don't use kvm_read.
	 */
	if (lseek(kd->pmfd, _kvm_cmn_pa2off(kd, addr), 0) == -1 || 
	    read(kd->pmfd, (char *)&pte, sizeof(pte)) < 0)
		goto invalid;
	addr = pte & m->pg_frame;
	if ((pte & m->pg_v) == 0) {
		_kvm_err(kd, 0, "page not valid");
		return (0);
	}
	*pa = addr + offset;
	
	return (h->page_size - offset);

invalid:
	_kvm_err(kd, 0, "invalid address (%x)", va);
	return (0);
}
