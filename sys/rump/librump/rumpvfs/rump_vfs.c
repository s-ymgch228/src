/*	$NetBSD: rump_vfs.c,v 1.60 2010/11/21 16:19:19 pooka Exp $	*/

/*
 * Copyright (c) 2008 Antti Kantee.  All Rights Reserved.
 *
 * Development of this software was supported by the
 * Finnish Cultural Foundation.
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

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: rump_vfs.c,v 1.60 2010/11/21 16:19:19 pooka Exp $");

#include <sys/param.h>
#include <sys/buf.h>
#include <sys/conf.h>
#include <sys/evcnt.h>
#include <sys/filedesc.h>
#include <sys/fstrans.h>
#include <sys/lockf.h>
#include <sys/kthread.h>
#include <sys/module.h>
#include <sys/namei.h>
#include <sys/queue.h>
#include <sys/stat.h>
#include <sys/vfs_syscalls.h>
#include <sys/vnode.h>
#include <sys/wapbl.h>

#include <miscfs/specfs/specdev.h>
#include <miscfs/syncfs/syncfs.h>

#include <rump/rump.h>
#include <rump/rumpuser.h>

#include "rump_private.h"
#include "rump_vfs_private.h"

struct cwdinfo cwdi0;
const char *rootfstype = ROOT_FSTYPE_ANY;

static void
pvfs_init(struct proc *p)
{

	p->p_cwdi = cwdinit();
}

static void
pvfs_rele(struct proc *p)
{

	cwdfree(p->p_cwdi);
}

void
rump_vfs_init(void)
{
	extern struct devsw_conv devsw_conv0[];
	extern int max_devsw_convs;
	extern struct vfsops rumpfs_vfsops;
	char buf[64];
	int error;
	int rv, i;

	if (rumpuser_getenv("RUMP_NVNODES", buf, sizeof(buf), &error) == 0) {
		desiredvnodes = strtoul(buf, NULL, 10);
	} else {
		desiredvnodes = 1<<10;
	}

	rumpblk_init();

	for (i = 0; i < ncpu; i++) {
		struct cpu_info *ci = cpu_lookup(i);
		cache_cpu_init(ci);
	}

	/* make number of bufpages 5% of total memory limit */
	if (rump_physmemlimit != RUMPMEM_UNLIMITED) {
		extern u_int bufpages;
		bufpages = rump_physmemlimit / (20 * PAGE_SIZE);
	}

	vfsinit();
	bufinit();
	cwd_sys_init();
	lf_init();
	spec_init();
	fstrans_init();

	if (rump_threads) {
		if ((rv = kthread_create(PRI_BIO, KTHREAD_MPSAFE, NULL,
		    rumpuser_biothread, rump_biodone, NULL, "rmpabio")) != 0)
			panic("syncer thread create failed: %d", rv);
	}

	root_device = &rump_rootdev;

	/* bootstrap cwdi (rest done in vfs_mountroot() */
	rw_init(&cwdi0.cwdi_lock);
	proc0.p_cwdi = &cwdi0;
	proc0.p_cwdi = cwdinit();

	vfs_attach(&rumpfs_vfsops);
	vfs_mountroot();

	/* "mtree": create /dev */
	do_sys_mkdir("/dev", 0777, UIO_SYSSPACE);
	rump_devnull_init();

	rump_proc_vfs_init = pvfs_init;
	rump_proc_vfs_release = pvfs_rele;

	if (rump_threads) {
		if ((rv = kthread_create(PRI_IOFLUSH, KTHREAD_MPSAFE, NULL,
		    sched_sync, NULL, NULL, "ioflush")) != 0)
			panic("syncer thread create failed: %d", rv);
	} else {
		syncdelay = 0;
	}

	/*
	 * On archs where the native kernel ABI is supported, map
	 * host module directory to rump.  This means that kernel
	 * modules from the host will be autoloaded to rump kernels.
	 */
#ifdef _RUMP_NATIVE_ABI
	{
	char *mbase;

	if (rumpuser_getenv("RUMP_MODULEBASE", buf, sizeof(buf), &error) == 0)
		mbase = buf;
	else
		mbase = module_base;

	if (strlen(mbase) != 0 && *mbase != '0') {
		rump_etfs_register(module_base, mbase, RUMP_ETFS_DIR_SUBDIRS);
	}
	}
#endif

	module_init_class(MODULE_CLASS_VFS);

	rump_vfs_builddevs(devsw_conv0, max_devsw_convs);

	rump_component_init(RUMP_COMPONENT_VFS);
}

void
rump_vfs_fini(void)
{

	vfs_shutdown();
}

struct componentname *
rump_makecn(u_long nameiop, u_long flags, const char *name, size_t namelen,
	kauth_cred_t creds, struct lwp *l)
{
	struct componentname *cnp;
	const char *cp = NULL;

	cnp = kmem_zalloc(sizeof(struct componentname), KM_SLEEP);

	cnp->cn_nameiop = nameiop;
	cnp->cn_flags = flags;

	cnp->cn_pnbuf = PNBUF_GET();
	strcpy(cnp->cn_pnbuf, name);
	cnp->cn_nameptr = cnp->cn_pnbuf;
	cnp->cn_namelen = namelen;
	cnp->cn_hash = namei_hash(name, &cp);

	cnp->cn_cred = creds;

	return cnp;
}

void
rump_freecn(struct componentname *cnp, int flags)
{

	if (flags & RUMPCN_FREECRED)
		rump_cred_put(cnp->cn_cred);

	if ((cnp->cn_flags & SAVENAME) == 0 || flags & RUMPCN_FORCEFREE)
		PNBUF_PUT(cnp->cn_pnbuf);
	kmem_free(cnp, sizeof(*cnp));
}

int
rump_checksavecn(struct componentname *cnp)
{

	if ((cnp->cn_flags & (SAVENAME | SAVESTART)) == 0) {
		return 0;
	} else {
		cnp->cn_flags |= HASBUF;
		return 1;
	}
}

/* hey baby, what's your namei? */
int
rump_namei(uint32_t op, uint32_t flags, const char *namep,
	struct vnode **dvpp, struct vnode **vpp, struct componentname **cnpp)
{
	struct pathbuf *pb;
	struct nameidata nd;
	int rv;

	pb = pathbuf_create(namep);
	if (pb == NULL) {
		return ENOMEM;
	}
	NDINIT(&nd, op, flags, pb);
	rv = namei(&nd);
	if (rv) {
		pathbuf_destroy(pb);
		return rv;
	}

	if (dvpp) {
		KASSERT(flags & LOCKPARENT);
		*dvpp = nd.ni_dvp;
	} else {
		KASSERT((flags & LOCKPARENT) == 0);
	}

	if (vpp) {
		*vpp = nd.ni_vp;
	} else {
		if (nd.ni_vp) {
			if (flags & LOCKLEAF)
				vput(nd.ni_vp);
			else
				vrele(nd.ni_vp);
		}
	}

	if (cnpp) {
		struct componentname *cnp;

		cnp = kmem_alloc(sizeof(*cnp), KM_SLEEP);
		memcpy(cnp, &nd.ni_cnd, sizeof(*cnp));
		*cnpp = cnp;
	} else if (nd.ni_cnd.cn_flags & HASBUF) {
		panic("%s: pathbuf mismatch", __func__);
	}
	pathbuf_destroy(pb);

	return rv;
}

void
rump_getvninfo(struct vnode *vp, enum vtype *vtype,
	voff_t *vsize, dev_t *vdev)
{

	*vtype = vp->v_type;
	*vsize = vp->v_size;
	if (vp->v_specnode)
		*vdev = vp->v_rdev;
	else
		*vdev = 0;
}

struct vfsops *
rump_vfslist_iterate(struct vfsops *ops)
{

	if (ops == NULL)
		return LIST_FIRST(&vfs_list);
	else
		return LIST_NEXT(ops, vfs_list);
}

struct vfsops *
rump_vfs_getopsbyname(const char *name)
{

	return vfs_getopsbyname(name);
}

int
rump_vfs_getmp(const char *path, struct mount **mpp)
{
	struct vnode *vp;
	int rv;

	if ((rv = namei_simple_user(path, NSM_FOLLOW_TRYEMULROOT, &vp)) != 0)
		return rv;

	*mpp = vp->v_mount;
	vrele(vp);
	return 0;
}

struct vattr*
rump_vattr_init(void)
{
	struct vattr *vap;

	vap = kmem_alloc(sizeof(struct vattr), KM_SLEEP);
	vattr_null(vap);

	return vap;
}

void
rump_vattr_settype(struct vattr *vap, enum vtype vt)
{

	vap->va_type = vt;
}

void
rump_vattr_setmode(struct vattr *vap, mode_t mode)
{

	vap->va_mode = mode;
}

void
rump_vattr_setrdev(struct vattr *vap, dev_t dev)
{

	vap->va_rdev = dev;
}

void
rump_vattr_free(struct vattr *vap)
{

	kmem_free(vap, sizeof(*vap));
}

void
rump_vp_incref(struct vnode *vp)
{

	vref(vp);
}

int
rump_vp_getref(struct vnode *vp)
{

	return vp->v_usecount;
}

void
rump_vp_rele(struct vnode *vp)
{

	vrele(vp);
}

void
rump_vp_interlock(struct vnode *vp)
{

	mutex_enter(&vp->v_interlock);
}

int
rump_vfs_unmount(struct mount *mp, int mntflags)
{

	return VFS_UNMOUNT(mp, mntflags);
}

int
rump_vfs_root(struct mount *mp, struct vnode **vpp, int lock)
{
	int rv;

	rv = VFS_ROOT(mp, vpp);
	if (rv)
		return rv;

	if (!lock)
		VOP_UNLOCK(*vpp);

	return 0;
}

int
rump_vfs_statvfs(struct mount *mp, struct statvfs *sbp)
{

	return VFS_STATVFS(mp, sbp);
}

int
rump_vfs_sync(struct mount *mp, int wait, kauth_cred_t cred)
{

	return VFS_SYNC(mp, wait ? MNT_WAIT : MNT_NOWAIT, cred);
}

int
rump_vfs_fhtovp(struct mount *mp, struct fid *fid, struct vnode **vpp)
{

	return VFS_FHTOVP(mp, fid, vpp);
}

int
rump_vfs_vptofh(struct vnode *vp, struct fid *fid, size_t *fidsize)
{

	return VFS_VPTOFH(vp, fid, fidsize);
}

int
rump_vfs_extattrctl(struct mount *mp, int cmd, struct vnode *vp,
	int attrnamespace, const char *attrname)
{

	return VFS_EXTATTRCTL(mp, cmd, vp, attrnamespace, attrname);
}

/*ARGSUSED*/
void
rump_vfs_syncwait(struct mount *mp)
{
	int n;

	n = buf_syncwait();
	if (n)
		printf("syncwait: unsynced buffers: %d\n", n);
}

/*
 * Dump info about mount point.  No locking.
 */
void
rump_vfs_mount_print(const char *path, int full)
{
#ifdef DEBUGPRINT
	struct vnode *mvp;
	struct vnode *vp;
	int error;

	rumpuser_dprintf("\n==== dumping mountpoint at ``%s'' ====\n\n", path);
	if ((error = namei_simple_user(path, NSM_FOLLOW_NOEMULROOT, &mvp))!=0) {
		rumpuser_dprintf("==== lookup error %d ====\n\n", error);
		return;
	}
	vfs_mount_print(mvp->v_mount, full, (void *)rumpuser_dprintf);
	if (full) {
		rumpuser_dprintf("\n== dumping vnodes ==\n\n");
		TAILQ_FOREACH(vp, &mvp->v_mount->mnt_vnodelist, v_mntvnodes) {
			vfs_vnode_print(vp, full, (void *)rumpuser_dprintf);
		}
	}
	vrele(mvp);
	rumpuser_dprintf("\n==== done ====\n\n");
#else
	rumpuser_dprintf("mount dump not supported without DEBUGPRINT\n");
#endif
}

void
rump_biodone(void *arg, size_t count, int error)
{
	struct buf *bp = arg;

	bp->b_resid = bp->b_bcount - count;
	KASSERT(bp->b_resid >= 0);
	bp->b_error = error;

	biodone(bp);
}

void
rump_vfs_drainbufs(int npages)
{

	mutex_enter(&bufcache_lock);
	buf_drain(npages);
	mutex_exit(&bufcache_lock);
}
