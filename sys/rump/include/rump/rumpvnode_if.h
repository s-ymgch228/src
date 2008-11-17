/*	$NetBSD: rumpvnode_if.h,v 1.2 2008/11/17 08:59:33 pooka Exp $	*/

/*
 * Warning: DO NOT EDIT! This file is automatically generated!
 * (Modifications made here may easily be lost!)
 *
 * Created from the file:
 *	NetBSD: vnode_if.src,v 1.58 2008/11/15 19:08:12 pooka Exp
 * by the script:
 *	NetBSD: vnode_if.sh,v 1.50 2008/11/17 08:46:03 pooka Exp
 */

/*
 * Copyright (c) 1992, 1993, 1994, 1995
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
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

#ifndef _RUMP_RUMPVNODE_IF_H_
#define _RUMP_RUMPVNODE_IF_H_

#ifdef _KERNEL_OPT
#include "opt_vnode_lockdebug.h"
#endif /* _KERNEL_OPT */

extern const struct vnodeop_desc rump_vop_default_desc;


/* Special cases: */
struct buf;
#ifndef _KERNEL
#include <stdbool.h>
#endif


#define RUMP_VOP_BWRITE_DESCOFFSET 1
struct rump_vop_bwrite_args {
	const struct vnodeop_desc *a_desc;
	struct buf *a_bp;
};
extern const struct vnodeop_desc rump_vop_bwrite_desc;
int RUMP_VOP_BWRITE(struct buf *);
/* End of special cases */

#define RUMP_VOP_LOOKUP_DESCOFFSET 2
struct rump_vop_lookup_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_dvp;
	struct vnode **a_vpp;
	struct componentname *a_cnp;
};
extern const struct vnodeop_desc rump_vop_lookup_desc;
int RUMP_VOP_LOOKUP(struct vnode *, struct vnode **, struct componentname *);

#define RUMP_VOP_CREATE_DESCOFFSET 3
struct rump_vop_create_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_dvp;
	struct vnode **a_vpp;
	struct componentname *a_cnp;
	struct vattr *a_vap;
};
extern const struct vnodeop_desc rump_vop_create_desc;
int RUMP_VOP_CREATE(struct vnode *, struct vnode **, struct componentname *, 
    struct vattr *);

#define RUMP_VOP_MKNOD_DESCOFFSET 4
struct rump_vop_mknod_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_dvp;
	struct vnode **a_vpp;
	struct componentname *a_cnp;
	struct vattr *a_vap;
};
extern const struct vnodeop_desc rump_vop_mknod_desc;
int RUMP_VOP_MKNOD(struct vnode *, struct vnode **, struct componentname *, 
    struct vattr *);

#define RUMP_VOP_OPEN_DESCOFFSET 5
struct rump_vop_open_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	int a_mode;
	kauth_cred_t a_cred;
};
extern const struct vnodeop_desc rump_vop_open_desc;
int RUMP_VOP_OPEN(struct vnode *, int, kauth_cred_t);

#define RUMP_VOP_CLOSE_DESCOFFSET 6
struct rump_vop_close_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	int a_fflag;
	kauth_cred_t a_cred;
};
extern const struct vnodeop_desc rump_vop_close_desc;
int RUMP_VOP_CLOSE(struct vnode *, int, kauth_cred_t);

#define RUMP_VOP_ACCESS_DESCOFFSET 7
struct rump_vop_access_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	int a_mode;
	kauth_cred_t a_cred;
};
extern const struct vnodeop_desc rump_vop_access_desc;
int RUMP_VOP_ACCESS(struct vnode *, int, kauth_cred_t);

#define RUMP_VOP_GETATTR_DESCOFFSET 8
struct rump_vop_getattr_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	struct vattr *a_vap;
	kauth_cred_t a_cred;
};
extern const struct vnodeop_desc rump_vop_getattr_desc;
int RUMP_VOP_GETATTR(struct vnode *, struct vattr *, kauth_cred_t);

#define RUMP_VOP_SETATTR_DESCOFFSET 9
struct rump_vop_setattr_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	struct vattr *a_vap;
	kauth_cred_t a_cred;
};
extern const struct vnodeop_desc rump_vop_setattr_desc;
int RUMP_VOP_SETATTR(struct vnode *, struct vattr *, kauth_cred_t);

#define RUMP_VOP_READ_DESCOFFSET 10
struct rump_vop_read_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	struct uio *a_uio;
	int a_ioflag;
	kauth_cred_t a_cred;
};
extern const struct vnodeop_desc rump_vop_read_desc;
int RUMP_VOP_READ(struct vnode *, struct uio *, int, kauth_cred_t);

#define RUMP_VOP_WRITE_DESCOFFSET 11
struct rump_vop_write_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	struct uio *a_uio;
	int a_ioflag;
	kauth_cred_t a_cred;
};
extern const struct vnodeop_desc rump_vop_write_desc;
int RUMP_VOP_WRITE(struct vnode *, struct uio *, int, kauth_cred_t);

#define RUMP_VOP_IOCTL_DESCOFFSET 12
struct rump_vop_ioctl_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	u_long a_command;
	void *a_data;
	int a_fflag;
	kauth_cred_t a_cred;
};
extern const struct vnodeop_desc rump_vop_ioctl_desc;
int RUMP_VOP_IOCTL(struct vnode *, u_long, void *, int, kauth_cred_t);

#define RUMP_VOP_FCNTL_DESCOFFSET 13
struct rump_vop_fcntl_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	u_int a_command;
	void *a_data;
	int a_fflag;
	kauth_cred_t a_cred;
};
extern const struct vnodeop_desc rump_vop_fcntl_desc;
int RUMP_VOP_FCNTL(struct vnode *, u_int, void *, int, kauth_cred_t);

#define RUMP_VOP_POLL_DESCOFFSET 14
struct rump_vop_poll_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	int a_events;
};
extern const struct vnodeop_desc rump_vop_poll_desc;
int RUMP_VOP_POLL(struct vnode *, int);

#define RUMP_VOP_KQFILTER_DESCOFFSET 15
struct rump_vop_kqfilter_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	struct knote *a_kn;
};
extern const struct vnodeop_desc rump_vop_kqfilter_desc;
int RUMP_VOP_KQFILTER(struct vnode *, struct knote *);

#define RUMP_VOP_REVOKE_DESCOFFSET 16
struct rump_vop_revoke_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	int a_flags;
};
extern const struct vnodeop_desc rump_vop_revoke_desc;
int RUMP_VOP_REVOKE(struct vnode *, int);

#define RUMP_VOP_MMAP_DESCOFFSET 17
struct rump_vop_mmap_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	int a_prot;
	kauth_cred_t a_cred;
};
extern const struct vnodeop_desc rump_vop_mmap_desc;
int RUMP_VOP_MMAP(struct vnode *, int, kauth_cred_t);

#define RUMP_VOP_FSYNC_DESCOFFSET 18
struct rump_vop_fsync_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	kauth_cred_t a_cred;
	int a_flags;
	off_t a_offlo;
	off_t a_offhi;
};
extern const struct vnodeop_desc rump_vop_fsync_desc;
int RUMP_VOP_FSYNC(struct vnode *, kauth_cred_t, int, off_t, off_t);

#define RUMP_VOP_SEEK_DESCOFFSET 19
struct rump_vop_seek_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	off_t a_oldoff;
	off_t a_newoff;
	kauth_cred_t a_cred;
};
extern const struct vnodeop_desc rump_vop_seek_desc;
int RUMP_VOP_SEEK(struct vnode *, off_t, off_t, kauth_cred_t);

#define RUMP_VOP_REMOVE_DESCOFFSET 20
struct rump_vop_remove_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_dvp;
	struct vnode *a_vp;
	struct componentname *a_cnp;
};
extern const struct vnodeop_desc rump_vop_remove_desc;
int RUMP_VOP_REMOVE(struct vnode *, struct vnode *, struct componentname *);

#define RUMP_VOP_LINK_DESCOFFSET 21
struct rump_vop_link_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_dvp;
	struct vnode *a_vp;
	struct componentname *a_cnp;
};
extern const struct vnodeop_desc rump_vop_link_desc;
int RUMP_VOP_LINK(struct vnode *, struct vnode *, struct componentname *);

#define RUMP_VOP_RENAME_DESCOFFSET 22
struct rump_vop_rename_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_fdvp;
	struct vnode *a_fvp;
	struct componentname *a_fcnp;
	struct vnode *a_tdvp;
	struct vnode *a_tvp;
	struct componentname *a_tcnp;
};
extern const struct vnodeop_desc rump_vop_rename_desc;
int RUMP_VOP_RENAME(struct vnode *, struct vnode *, struct componentname *, 
    struct vnode *, struct vnode *, struct componentname *);

#define RUMP_VOP_MKDIR_DESCOFFSET 23
struct rump_vop_mkdir_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_dvp;
	struct vnode **a_vpp;
	struct componentname *a_cnp;
	struct vattr *a_vap;
};
extern const struct vnodeop_desc rump_vop_mkdir_desc;
int RUMP_VOP_MKDIR(struct vnode *, struct vnode **, struct componentname *, 
    struct vattr *);

#define RUMP_VOP_RMDIR_DESCOFFSET 24
struct rump_vop_rmdir_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_dvp;
	struct vnode *a_vp;
	struct componentname *a_cnp;
};
extern const struct vnodeop_desc rump_vop_rmdir_desc;
int RUMP_VOP_RMDIR(struct vnode *, struct vnode *, struct componentname *);

#define RUMP_VOP_SYMLINK_DESCOFFSET 25
struct rump_vop_symlink_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_dvp;
	struct vnode **a_vpp;
	struct componentname *a_cnp;
	struct vattr *a_vap;
	char *a_target;
};
extern const struct vnodeop_desc rump_vop_symlink_desc;
int RUMP_VOP_SYMLINK(struct vnode *, struct vnode **, 
    struct componentname *, struct vattr *, char *);

#define RUMP_VOP_READDIR_DESCOFFSET 26
struct rump_vop_readdir_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	struct uio *a_uio;
	kauth_cred_t a_cred;
	int *a_eofflag;
	off_t **a_cookies;
	int *a_ncookies;
};
extern const struct vnodeop_desc rump_vop_readdir_desc;
int RUMP_VOP_READDIR(struct vnode *, struct uio *, kauth_cred_t, int *, 
    off_t **, int *);

#define RUMP_VOP_READLINK_DESCOFFSET 27
struct rump_vop_readlink_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	struct uio *a_uio;
	kauth_cred_t a_cred;
};
extern const struct vnodeop_desc rump_vop_readlink_desc;
int RUMP_VOP_READLINK(struct vnode *, struct uio *, kauth_cred_t);

#define RUMP_VOP_ABORTOP_DESCOFFSET 28
struct rump_vop_abortop_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_dvp;
	struct componentname *a_cnp;
};
extern const struct vnodeop_desc rump_vop_abortop_desc;
int RUMP_VOP_ABORTOP(struct vnode *, struct componentname *);

#define RUMP_VOP_INACTIVE_DESCOFFSET 29
struct rump_vop_inactive_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	bool *a_recycle;
};
extern const struct vnodeop_desc rump_vop_inactive_desc;
int RUMP_VOP_INACTIVE(struct vnode *, bool *);

#define RUMP_VOP_RECLAIM_DESCOFFSET 30
struct rump_vop_reclaim_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
};
extern const struct vnodeop_desc rump_vop_reclaim_desc;
int RUMP_VOP_RECLAIM(struct vnode *);

#define RUMP_VOP_LOCK_DESCOFFSET 31
struct rump_vop_lock_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	int a_flags;
};
extern const struct vnodeop_desc rump_vop_lock_desc;
int RUMP_VOP_LOCK(struct vnode *, int);

#define RUMP_VOP_UNLOCK_DESCOFFSET 32
struct rump_vop_unlock_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	int a_flags;
};
extern const struct vnodeop_desc rump_vop_unlock_desc;
int RUMP_VOP_UNLOCK(struct vnode *, int);

#define RUMP_VOP_BMAP_DESCOFFSET 33
struct rump_vop_bmap_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	daddr_t a_bn;
	struct vnode **a_vpp;
	daddr_t *a_bnp;
	int *a_runp;
};
extern const struct vnodeop_desc rump_vop_bmap_desc;
int RUMP_VOP_BMAP(struct vnode *, daddr_t, struct vnode **, daddr_t *, int *);

#define RUMP_VOP_STRATEGY_DESCOFFSET 34
struct rump_vop_strategy_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	struct buf *a_bp;
};
extern const struct vnodeop_desc rump_vop_strategy_desc;
int RUMP_VOP_STRATEGY(struct vnode *, struct buf *);

#define RUMP_VOP_PRINT_DESCOFFSET 35
struct rump_vop_print_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
};
extern const struct vnodeop_desc rump_vop_print_desc;
int RUMP_VOP_PRINT(struct vnode *);

#define RUMP_VOP_ISLOCKED_DESCOFFSET 36
struct rump_vop_islocked_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
};
extern const struct vnodeop_desc rump_vop_islocked_desc;
int RUMP_VOP_ISLOCKED(struct vnode *);

#define RUMP_VOP_PATHCONF_DESCOFFSET 37
struct rump_vop_pathconf_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	int a_name;
	register_t *a_retval;
};
extern const struct vnodeop_desc rump_vop_pathconf_desc;
int RUMP_VOP_PATHCONF(struct vnode *, int, register_t *);

#define RUMP_VOP_ADVLOCK_DESCOFFSET 38
struct rump_vop_advlock_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	void *a_id;
	int a_op;
	struct flock *a_fl;
	int a_flags;
};
extern const struct vnodeop_desc rump_vop_advlock_desc;
int RUMP_VOP_ADVLOCK(struct vnode *, void *, int, struct flock *, int);

#define RUMP_VOP_WHITEOUT_DESCOFFSET 39
struct rump_vop_whiteout_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_dvp;
	struct componentname *a_cnp;
	int a_flags;
};
extern const struct vnodeop_desc rump_vop_whiteout_desc;
int RUMP_VOP_WHITEOUT(struct vnode *, struct componentname *, int);

#define RUMP_VOP_GETPAGES_DESCOFFSET 40
struct rump_vop_getpages_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	off_t a_offset;
	struct vm_page **a_m;
	int *a_count;
	int a_centeridx;
	int a_access_type;
	int a_advice;
	int a_flags;
};
extern const struct vnodeop_desc rump_vop_getpages_desc;
int RUMP_VOP_GETPAGES(struct vnode *, off_t, struct vm_page **, int *, int, 
    int, int, int);

#define RUMP_VOP_PUTPAGES_DESCOFFSET 41
struct rump_vop_putpages_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	off_t a_offlo;
	off_t a_offhi;
	int a_flags;
};
extern const struct vnodeop_desc rump_vop_putpages_desc;
int RUMP_VOP_PUTPAGES(struct vnode *, off_t, off_t, int);

#define RUMP_VOP_CLOSEEXTATTR_DESCOFFSET 42
struct rump_vop_closeextattr_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	int a_commit;
	kauth_cred_t a_cred;
};
extern const struct vnodeop_desc rump_vop_closeextattr_desc;
int RUMP_VOP_CLOSEEXTATTR(struct vnode *, int, kauth_cred_t);

#define RUMP_VOP_GETEXTATTR_DESCOFFSET 43
struct rump_vop_getextattr_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	int a_attrnamespace;
	const char *a_name;
	struct uio *a_uio;
	size_t *a_size;
	kauth_cred_t a_cred;
};
extern const struct vnodeop_desc rump_vop_getextattr_desc;
int RUMP_VOP_GETEXTATTR(struct vnode *, int, const char *, struct uio *, 
    size_t *, kauth_cred_t);

#define RUMP_VOP_LISTEXTATTR_DESCOFFSET 44
struct rump_vop_listextattr_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	int a_attrnamespace;
	struct uio *a_uio;
	size_t *a_size;
	kauth_cred_t a_cred;
};
extern const struct vnodeop_desc rump_vop_listextattr_desc;
int RUMP_VOP_LISTEXTATTR(struct vnode *, int, struct uio *, size_t *, 
    kauth_cred_t);

#define RUMP_VOP_OPENEXTATTR_DESCOFFSET 45
struct rump_vop_openextattr_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	kauth_cred_t a_cred;
};
extern const struct vnodeop_desc rump_vop_openextattr_desc;
int RUMP_VOP_OPENEXTATTR(struct vnode *, kauth_cred_t);

#define RUMP_VOP_DELETEEXTATTR_DESCOFFSET 46
struct rump_vop_deleteextattr_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	int a_attrnamespace;
	const char *a_name;
	kauth_cred_t a_cred;
};
extern const struct vnodeop_desc rump_vop_deleteextattr_desc;
int RUMP_VOP_DELETEEXTATTR(struct vnode *, int, const char *, kauth_cred_t);

#define RUMP_VOP_SETEXTATTR_DESCOFFSET 47
struct rump_vop_setextattr_args {
	const struct vnodeop_desc *a_desc;
	struct vnode *a_vp;
	int a_attrnamespace;
	const char *a_name;
	struct uio *a_uio;
	kauth_cred_t a_cred;
};
extern const struct vnodeop_desc rump_vop_setextattr_desc;
int RUMP_VOP_SETEXTATTR(struct vnode *, int, const char *, struct uio *, 
    kauth_cred_t);

#define VNODE_OPS_COUNT	48

/* End of special cases. */

#endif /* !_RUMP_RUMPVNODE_IF_H_ */
