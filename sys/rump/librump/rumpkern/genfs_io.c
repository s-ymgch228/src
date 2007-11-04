/*	$NetBSD: genfs_io.c,v 1.3 2007/11/04 19:37:29 pooka Exp $	*/

/*
 * Copyright (c) 2007 Antti Kantee.  All Rights Reserved.
 *
 * Development of this software was supported by Google Summer of Code.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/param.h>
#include <sys/vnode.h>

#include <miscfs/genfs/genfs_node.h>
#include <miscfs/genfs/genfs.h>

#include "rump_private.h"
#include "rumpuser.h"

void
genfs_directio(struct vnode *vp, struct uio *uio, int ioflag)
{

	panic("%s: not implemented", __func__);
}

int
genfs_getpages(void *v)
{
	struct vop_getpages_args /* {
		struct vnode *a_vp;
		voff_t a_offset;
		struct vm_page **a_m;
		int *a_count;
		int a_centeridx;
		vm_prot_t a_access_type;
		int a_advice;
		int a_flags;
	} */ *ap = v;
	struct vnode *vp = ap->a_vp;
	struct vm_page *pg;
	voff_t curoff, endoff;
	size_t remain, bufoff, xfersize;
	uint8_t *tmpbuf;
	int bshift = vp->v_mount->mnt_fs_bshift;
	int bsize = 1<<bshift;
	int count = *ap->a_count;
	int async;
	int i, error;

	/*
	 * Ignore async for now, the structure of this routine
	 * doesn't exactly allow for it ...
	 */
	async = 0;

	if (ap->a_centeridx != 0)
		panic("%s: centeridx != not supported", __func__);

	if (ap->a_access_type & VM_PROT_WRITE)
		vp->v_iflag |= VI_ONWORKLST;

	curoff = ap->a_offset & ~PAGE_MASK;
	for (i = 0; i < count; i++, curoff += PAGE_SIZE) {
		pg = uvm_pagelookup(&vp->v_uobj, curoff);
		if (pg == NULL)
			break;
		ap->a_m[i] = pg;
	}

	/* got everything?  if so, just return */
	if (i == count)
		return 0;

	/*
	 * else, transfer entire range for simplicity and copy into
	 * page buffers
	 */

	/* align to boundaries */
	endoff = trunc_page(ap->a_offset) + (count << PAGE_SHIFT);
	endoff = MIN(endoff, ((vp->v_writesize+bsize-1) & ~(bsize-1)));
	curoff = ap->a_offset & ~(MAX(bsize,PAGE_SIZE)-1);
	remain = endoff - curoff;

	DPRINTF(("a_offset: %llx, startoff: 0x%llx, endoff 0x%llx\n",
	    (unsigned long long)ap->a_offset, (unsigned long long)curoff,
	    (unsigned long long)endoff));

	/* read everything into a buffer */
	tmpbuf = rumpuser_malloc(round_page(remain), 0);
	memset(tmpbuf, 0, round_page(remain));
	for (bufoff = 0; remain; remain -= xfersize, bufoff+=xfersize) {
		struct buf *bp;
		struct vnode *devvp;
		daddr_t lbn, bn;
		int run;

		lbn = (curoff + bufoff) >> bshift;
		/* XXX: assume eof */
		error = VOP_BMAP(vp, lbn, &devvp, &bn, &run);
		if (error)
			panic("%s: VOP_BMAP & lazy bum: %d", __func__, error);

		DPRINTF(("lbn %d (off %d) -> bn %d run %d\n", (int)lbn,
		    (int)(curoff+bufoff), (int)bn, run));
		xfersize = MIN(((lbn+1+run)<<bshift)-(curoff+bufoff), remain);

		/* hole? */
		if (bn == -1) {
			memset(tmpbuf + bufoff, 0, xfersize);
			continue;
		}

		bp = getiobuf();

		bp->b_data = tmpbuf + bufoff;
		bp->b_bcount = xfersize;
		bp->b_blkno = bn;
		bp->b_lblkno = 0;
		bp->b_flags = B_READ | B_BUSY;

		if (async) {
			bp->b_flags |= B_ASYNC | B_CALL;
			bp->b_iodone = uvm_aio_biodone;
		}

		bp->b_vp = vp;

		VOP_STRATEGY(devvp, bp);
		if (bp->b_error)
			panic("%s: VOP_STRATEGY, lazy bum", __func__);
		
		if (!async)
			putiobuf(bp);
	}

	/* skip to beginning of pages we're interested in */
	bufoff = 0;
	while (round_page(curoff + bufoff) < trunc_page(ap->a_offset))
		bufoff += PAGE_SIZE;

	DPRINTF(("first page offset 0x%x\n", (int)(curoff + bufoff)));

	for (i = 0; i < count; i++, bufoff += PAGE_SIZE) {
		/* past our prime? */
		if (curoff + bufoff >= endoff)
			break;

		pg = uvm_pagelookup(&vp->v_uobj, curoff + bufoff);
		DPRINTF(("got page %p (off 0x%x)\n", pg, (int)(curoff+bufoff)));
		if (pg == NULL) {
			pg = rumpvm_makepage(&vp->v_uobj, curoff + bufoff);
			memcpy((void *)pg->uanon, tmpbuf+bufoff, PAGE_SIZE);
			pg->flags |= PG_CLEAN;
		}
		ap->a_m[i] = pg;
	}
	*ap->a_count = i;

	rumpuser_free(tmpbuf);

	return 0;
}

int
genfs_compat_getpages(void *v)
{

	panic("%s: not implemented", __func__);
}

/*
 * simplesimplesimple: we put all pages every time.
 */
int
genfs_putpages(void *v)
{
	struct vop_putpages_args /* {
		struct vnode *a_vp;
		voff_t a_offlo;
		voff_t a_offhi;
		int a_flags;
	} */ *ap = v;

	return genfs_do_putpages(ap->a_vp, ap->a_offlo, ap->a_offhi,
	    ap->a_flags, NULL);
}

/*
 * This is a slightly strangely structured routine.  It always puts
 * all the pages for a vnode.  It starts by releasing pages which
 * are clean and simultaneously looks up the smallest offset for a
 * dirty page beloning to the object.  If there is no smallest offset,
 * all pages have been cleaned.  Otherwise, it finds a contiguous range
 * of dirty pages starting from the smallest offset and writes them out.
 * After this the scan is restarted.
 */
int
genfs_do_putpages(struct vnode *vp, off_t startoff, off_t endoff, int flags,
	struct vm_page **busypg)
{
	char databuf[MAXPHYS];
	struct uvm_object *uobj = &vp->v_uobj;
	struct vm_page *pg, *pg_next;
	voff_t smallest;
	voff_t curoff, bufoff;
	off_t eof;
	size_t xfersize;
	int bshift = vp->v_mount->mnt_fs_bshift;
	int bsize = 1 << bshift;
#if 0
	int async = (flags & PGO_SYNCIO) == 0;
#else
	int async = 0;
#endif

 restart:
	/* check if all pages are clean */
	smallest = -1;
	simple_lock(&uobj->vmobjlock);
	for (pg = TAILQ_FIRST(&uobj->memq); pg; pg = pg_next) {
		pg_next = TAILQ_NEXT(pg, listq);
		if (pg->flags & PG_CLEAN) {
			rumpvm_freepage(pg);
			simple_lock(&uobj->vmobjlock);
			continue;
		}

		if (pg->offset < smallest || smallest == -1)
			smallest = pg->offset;
	}

	/* all done? */
	if (TAILQ_EMPTY(&uobj->memq)) {
		vp->v_iflag &= ~VI_ONWORKLST;
		simple_unlock(&uobj->vmobjlock);
		return 0;
	}
	simple_unlock(&uobj->vmobjlock);

	GOP_SIZE(vp, vp->v_writesize, &eof, 0);

	/* we need to flush */
	for (curoff = smallest; curoff < eof; curoff += PAGE_SIZE) {
		void *curva;

		if (curoff - smallest >= MAXPHYS)
			break;
		pg = uvm_pagelookup(uobj, curoff);
		if (pg == NULL)
			break;
		curva = databuf + (curoff-smallest);
		memcpy(curva, (void *)pg->uanon, PAGE_SIZE);
		rumpvm_enterva((vaddr_t)curva, pg);

		pg->flags |= PG_CLEAN;
	}
	assert(curoff > smallest);

	/* then we write */
	for (bufoff = 0; bufoff < MIN(curoff-smallest,eof); bufoff+=xfersize) {
		struct buf *bp;
		struct vnode *devvp;
		daddr_t bn, lbn;
		int run, error;

		lbn = (smallest + bufoff) >> bshift;
		error = VOP_BMAP(vp, lbn, &devvp, &bn, &run);
		if (error)
			panic("%s: VOP_BMAP failed: %d", __func__, error);

		xfersize = MIN(((lbn+1+run) << bshift) - (smallest+bufoff),
		     curoff - (smallest+bufoff));

		/*
		 * We might run across blocks which aren't allocated yet.
		 * A reason might be e.g. the write operation being still
		 * in the kernel page cache while truncate has already
		 * enlarged the file.  So just ignore those ranges.
		 */
		if (bn == -1)
			continue;

		bp = getiobuf();

		/* only write max what we are allowed to write */
		bp->b_bcount = xfersize;
		if (smallest + bufoff + xfersize > eof)
			bp->b_bcount -= (smallest+bufoff+xfersize) - eof;
		bp->b_bcount = (bp->b_bcount + DEV_BSIZE-1) & ~(DEV_BSIZE-1);

		KASSERT(bp->b_bcount > 0);
		KASSERT(smallest >= 0);

		DPRINTF(("putpages writing from %x to %x (vp size %x)\n",
		    (int)(smallest + bufoff),
		    (int)(smallest + bufoff + bp->b_bcount),
		    (int)eof));

		bp->b_bufsize = round_page(bp->b_bcount);
		bp->b_lblkno = 0;
		bp->b_blkno = bn + (((smallest+bufoff)&(bsize-1))>>DEV_BSHIFT);
		bp->b_data = databuf + bufoff;
		bp->b_vp = vp;
		bp->b_flags = B_WRITE | B_BUSY;
		bp->b_iodone = uvm_aio_biodone;
		if (async) {
			bp->b_flags |= B_CALL | B_ASYNC;
			bp->b_iodone = uvm_aio_biodone;
		}

		vp->v_numoutput++;
		VOP_STRATEGY(devvp, bp);
		if (bp->b_error)
			panic("%s: VOP_STRATEGY lazy bum %d",
			    __func__, bp->b_error);
		if (!async)
			putiobuf(bp);
	}
	rumpvm_flushva();

	goto restart;
}

int
genfs_null_putpages(void *v)
{
	struct vop_putpages_args *ap = v;
	struct vnode *vp = ap->a_vp;

	KASSERT(vp->v_uobj.uo_npages == 0);
	simple_unlock(&vp->v_interlock);
	return 0;
}

int
genfs_compat_gop_write(struct vnode *vp, struct vm_page **pgs,
	int npages, int flags)
{

	panic("%s: not implemented", __func__);
}

int
genfs_gop_write(struct vnode *vp, struct vm_page **pgs, int npages, int flags)
{

	panic("%s: not implemented", __func__);
}
