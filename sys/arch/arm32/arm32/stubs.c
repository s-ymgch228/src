/* $NetBSD: stubs.c,v 1.9 1996/10/11 00:06:55 christos Exp $ */

/*
 * Copyright (c) 1994,1995 Mark Brinicombe.
 * Copyright (c) 1994 Brini.
 * All rights reserved.
 *
 * This code is derived from software written for Brini by Mark Brinicombe
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
 *	This product includes software developed by Brini.
 * 4. The name of the company nor the name of the author may be used to
 *    endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY BRINI ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL BRINI OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * RiscBSD kernel project
 *
 * stubs.c
 *
 * Routines that are temporary or do not have a home yet.
 *
 * Created      : 17/09/94
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/errno.h>
#include <sys/proc.h>
#include <sys/types.h>
#include <sys/device.h>
#include <sys/buf.h>
#include <sys/exec.h>
#include <sys/vnode.h>
#include <sys/conf.h> 
#include <sys/reboot.h> 
#include <vm/vm.h>
#include <vm/vm_kern.h>
#include <vm/vm_page.h>
#include <dev/ramdisk.h>
#include <machine/param.h>
#include <machine/vmparam.h>
#include <machine/cpu.h>
#include <machine/irqhandler.h>
#include <machine/vidc.h>
#include <machine/bootconfig.h>
#include <machine/katelib.h>
#include <machine/psl.h>
#include <machine/pcb.h>

#include "hydrabus.h"

extern u_int soft_interrupts;

extern int msgbufmapped;
extern dev_t dumpdev;
extern BootConfig bootconfig;
extern videomemory_t videomemory;

#ifdef GENERIC

#include "fdc.h" 
#include "rd.h" 

extern dev_t rootdev;
extern int ffs_mountroot();
extern int cd9660_mountroot();
extern int nfs_mountroot();
int do_mountroot();
int (*mountroot)() = do_mountroot;

#ifdef RAMDISK_HOOKS
extern struct rd_conf *bootrd;
#endif

int load_ramdisc_from_floppy __P((struct rd_conf *rd, dev_t dev));


int
do_mountroot()
{
	int error;

#if (NFDC > 0 && NRD > 0 && defined(RAMDISK_HOOKS))

/*
 * Ok ideally the ramdisc would be loaded via the rd_open_hook() but since
 * we are loading the ramdisc from floppy we only want to load it during
 * the boot and not at any other time.
 */

/*
 * Ok bit of bodging here. The ramdisc minor is the unit number. However if booting
 * from the ramdisc we limit to always booting off minor 0 i.e. rd0 The ramdisc
 * device passed as the root device is only used to identify the ramdisc major. The
 * minor, instead of indicating the ramdisc unit is used to indicate the floppy
 * minor that should be used for loading the boot ramdisc which is unit 0.
 */

	if (major(rootdev) == 18 && bootrd) {
		if (load_ramdisc_from_floppy(bootrd, makedev(17, minor(rootdev))) != 0)
			panic("Failed to load ramdisc\n");
		boothowto |= RB_SINGLE;
		rootdev = makedev(major(rootdev), 0);
	}
#endif

/*
 * Slight bug with mounting CD's sometimes. The first mount may fail
 * so we will try again. This only happens with the temporary ATAPI
 * CDROM driver we are using
 */

#ifdef NFSCLIENT
	/* Test for the fake nfs device */
	if (major(rootdev) == 1)
		return(nfs_mountroot());
#endif
#ifdef CD9660
	if (major(rootdev) == 20 || major(rootdev) == 26) {
		error = cd9660_mountroot();
		if (error)
			error = cd9660_mountroot();
	}
	else {
#endif
#ifdef FFS
		error = ffs_mountroot();
#else
#error	FFS not configured
#endif
#ifdef CD9660
		if (error)
			error = cd9660_mountroot();
	}
#endif
	return(error);
} 
#endif


void
setsoftintr(irqmask)
	u_int irqmask;
{
	u_int oldcpsr;

	oldcpsr = disable_interrupts(I32_bit);
	soft_interrupts |= irqmask;
	restore_interrupts(oldcpsr);
}

/* Eventually this will become macros */

void
setsoftclock()
{
	setsoftintr(IRQMASK_SOFTCLOCK);
}

void
setsoftnet()
{
	setsoftintr(IRQMASK_SOFTNET);
}

int astpending;

void
setsoftast()
{
	astpending = 1;
}

extern int want_resched;

void
need_resched(void)
{
	want_resched = 1;
	setsoftast();
}


struct queue {
	struct queue *q_next, *q_prev;
};

/*
 * insert an element into a queue
 */

void
_insque(v1, v2)
	void *v1;
	void *v2;
{
	register struct queue *elem = v1, *head = v2;
	register struct queue *next;

	next = head->q_next;
	elem->q_next = next;
	head->q_next = elem;
	elem->q_prev = head;
	next->q_prev = elem;
}

/*
 * remove an element from a queue
 */

void
_remque(v)
	void *v;
{
	register struct queue *elem = v;
	register struct queue *next, *prev;

	next = elem->q_next;
	prev = elem->q_prev;
	next->q_prev = prev;
	prev->q_next = next;
	elem->q_prev = 0;
}



/*
 * These variables are needed by /sbin/savecore
 */
u_long	dumpmag = 0x8fca0101;	/* magic number */
int 	dumpsize = 0;		/* pages */
long	dumplo = 0; 		/* blocks */

/*
 * This is called by configure to set dumplo and dumpsize.
 * Dumps always skip the first CLBYTES of disk space
 * in case there might be a disk label stored there.
 * If there is extra space, put dump at the end to
 * reduce the chance that swapping trashes it.
 */

void
dumpconf()
{
	int nblks;	/* size of dump area */
	int maj;

	if (dumpdev == NODEV)
		return;
	maj = major(dumpdev);
	if (maj < 0 || maj >= nblkdev)
		panic("dumpconf: bad dumpdev=0x%x", dumpdev);
	if (bdevsw[maj].d_psize == NULL)
		return;
	nblks = (*bdevsw[maj].d_psize)(dumpdev);
	if (nblks <= ctod(1))
		return;

	dumpsize = physmem;

	/* Always skip the first CLBYTES, in case there is a label there. */
	if (dumplo < ctod(1))
		dumplo = ctod(1);

	/* Put dump at end of partition, and make it fit. */
	if (dumpsize > dtoc(nblks - dumplo))
		dumpsize = dtoc(nblks - dumplo);
	if (dumplo < nblks - ctod(dumpsize))
		dumplo = nblks - ctod(dumpsize);
}


extern pagehook_t page_hook0;

/*
 * Doadump comes here after turning off memory management and
 * getting on the dump stack, either when called above, or by
 * the auto-restart code.
 */

void
dumpsys()
{
	daddr_t blkno;
	int psize;
	int error;
	int addr;
	int block;
	vm_offset_t dumpspace;

	msgbufmapped = 0;
	if (dumpdev == NODEV)
		return;
	if (dumpsize == 0) {
		dumpconf();
		if (dumpsize == 0)
			return;
	}
	kprintf("\ndumping to dev %x, offset %d\n", (u_int)dumpdev,
	    (u_int)dumplo);

	blkno = dumplo;
	dumpspace = page_hook0.va;

	psize = (*bdevsw[major(dumpdev)].d_psize)(dumpdev);
	kprintf("dump ");
	if (psize == -1) {
		kprintf("area unavailable\n");
		return;
	}

	error = 0;
	for (block = 0; block < bootconfig.dramblocks && error == 0; ++block) {
		for (addr = bootconfig.dram[block].address;
		    addr < bootconfig.dram[block].address
		    + bootconfig.dram[block].pages * NBPG; addr += NBPG) {
	                pmap_map(dumpspace, addr, addr + NBPG, VM_PROT_READ);
			error = (*bdevsw[major(dumpdev)].d_dump)(dumpdev, blkno, (caddr_t) dumpspace, NBPG);
			if (error) break;
			blkno += btodb(NBPG);
		}
	}

	if (error == 0 && videomemory.vidm_type == VIDEOMEM_TYPE_VRAM) {
		for (addr = videomemory.vidm_pbase; addr < videomemory.vidm_pbase
		    + videomemory.vidm_size; addr += NBPG) {
     	           pmap_map(dumpspace, addr, addr + NBPG, VM_PROT_READ);
	                error = (*bdevsw[major(dumpdev)].d_dump)(dumpdev, blkno, (caddr_t) dumpspace, NBPG);
	                if (error) break;
			blkno += btodb(NBPG);
		}
	}                         

	switch (error) {
	case ENXIO:
		kprintf("device bad\n");
		break;

	case EFAULT:
		kprintf("device not ready\n");
		break;

	case EINVAL:
		kprintf("area improper\n");
		break;

	case EIO:
		kprintf("i/o error\n");
		break;

	case EINTR:
		kprintf("aborted from console\n");
		break;

	default:
		kprintf("succeeded\n");
		break;
	}
	kprintf("\n\n");
	delay(1000000);
}

/*
 * Dummy function is case no audio device has been configured
 * Need to fix the code that uses this function (console) to check NBEEP.
 */

#include "beep.h"
#include "vidcaudio.h"
#if NBEEP == 0
void
beep_generate()
{
#if NVIDCAUDIO > 0
	vidcaudio_beep_generate();
#endif /* NVIDCAUDIO */
}
#endif /* NBEEP */


extern u_int spl_mask;

int current_spl_level = SPL_0;
u_int spl_masks[SPL_LEVELS];

int safepri = SPL_0;

void
set_spl_masks()
{
	spl_masks[SPL_0]	= 0xffffffff;
	spl_masks[SPL_SOFT]	= ~(IRQMASK_SOFTNET | IRQMASK_SOFTCLOCK | IRQMASK_SOFTPLIP);
	spl_masks[SPL_BIO]	= irqmasks[IPL_BIO];
	spl_masks[SPL_NET]	= irqmasks[IPL_NET];
	spl_masks[SPL_TTY]	= irqmasks[IPL_TTY];
	spl_masks[SPL_CLOCK]	= irqmasks[IPL_CLOCK];
	spl_masks[SPL_IMP]	= irqmasks[IPL_IMP];
	spl_masks[SPL_HIGH]	= 0x00000000;
}

void
dump_spl_masks()
{
	kprintf("spl0=%08x splsoft=%08x splbio=%08x splnet=%08x\n",
	    spl_masks[SPL_0], spl_masks[SPL_SOFT], spl_masks[SPL_BIO], spl_masks[SPL_NET]);
	kprintf("spltty=%08x splclock=%08x splimp=%08x splhigh=%08x\n",
	    spl_masks[SPL_TTY], spl_masks[SPL_CLOCK], spl_masks[SPL_IMP], spl_masks[SPL_HIGH]);
}

/*
 * Ok things are broken here. If we lower the spl level to SPL_SOFT
 * then, for the most things work. However wd interrupts start to get
 * lost ... i.e. you either get wdc interrupt lost messages or
 * wdc timeout messages.
 * The fault is the CLKF_FRAME macro uses in kern_clock.c. This
 * currently always returns 1 thus splsoftclock() is always
 * called before calling softclock().
 *
 * This is about to be fixed
 */

int
splsoftclock()
{
	return(lowerspl(SPL_SOFT));
/*	return(current_spl_level);*/
}

/* End of stubs.c */
