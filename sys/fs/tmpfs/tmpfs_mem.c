/*	$NetBSD: tmpfs_mem.c,v 1.1 2010/06/22 18:32:07 rmind Exp $	*/

/*
 * Copyright (c) 2010 The NetBSD Foundation, Inc.
 * All rights reserved.
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

/*
 * tmpfs memory allocation routines.
 * Implements memory usage accounting and limiting.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: tmpfs_mem.c,v 1.1 2010/06/22 18:32:07 rmind Exp $");

#include <sys/param.h>
#include <sys/atomic.h>
#include <sys/kmem.h>
#include <sys/namei.h>
#include <sys/pool.h>

#include <fs/tmpfs/tmpfs.h>

void
tmpfs_mntmem_init(struct tmpfs_mount *mp, uint64_t memlimit)
{

	sprintf(mp->tm_dwchan, "tmpfs_dirent_%p", mp);
	pool_init(&mp->tm_dirent_pool, sizeof(struct tmpfs_dirent), 0, 0, 0,
	    mp->tm_dwchan, &pool_allocator_nointr, IPL_NONE);

	sprintf(mp->tm_nwchan, "tmpfs_node_%p", mp);
	pool_init(&mp->tm_node_pool, sizeof(struct tmpfs_node), 0, 0, 0,
	    mp->tm_dwchan, &pool_allocator_nointr, IPL_NONE);

	mutex_init(&mp->tm_acc_lock, MUTEX_DEFAULT, IPL_NONE);
	mp->tm_mem_limit = memlimit;
	mp->tm_bytes_used = 0;
}

void
tmpfs_mntmem_destroy(struct tmpfs_mount *mp)
{

	KASSERT(mp->tm_bytes_used == 0);
	mutex_destroy(&mp->tm_acc_lock);
	pool_destroy(&mp->tm_dirent_pool);
	pool_destroy(&mp->tm_node_pool);
}

/*
 * tmpfs_mem_info: return the number of available memory pages.
 *
 * => If 'total' is true, then return _total_ amount of pages.
 * => If false, then return the amount of _free_ memory pages.
 *
 * Remember to remove TMPFS_PAGES_RESERVED from the returned value to avoid
 * excessive memory usage.
 */
size_t
tmpfs_mem_info(bool total)
{
	size_t size = 0;

	/* XXX: unlocked */
	size += uvmexp.swpgavail;
	if (!total) {
		size -= uvmexp.swpgonly;
	}
	size += uvmexp.free;
	size += uvmexp.filepages;
	if (size > uvmexp.wired) {
		size -= uvmexp.wired;
	} else {
		size = 0;
	}
	return size;
}

uint64_t
tmpfs_bytes_max(struct tmpfs_mount *mp)
{
	size_t freepages = tmpfs_mem_info(false);
	uint64_t avail_mem;

	if (freepages < TMPFS_PAGES_RESERVED) {
		freepages = 0;
	} else {
		freepages -= TMPFS_PAGES_RESERVED;
	}
	avail_mem = round_page(mp->tm_bytes_used) + (freepages << PAGE_SHIFT);
	return min(mp->tm_mem_limit, avail_mem);
}

size_t
tmpfs_pages_avail(struct tmpfs_mount *mp)
{

	return (tmpfs_bytes_max(mp) - mp->tm_bytes_used) >> PAGE_SHIFT;
}

bool
tmpfs_mem_incr(struct tmpfs_mount *mp, size_t sz)
{
	const uint64_t lim = tmpfs_bytes_max(mp);

	mutex_enter(&mp->tm_acc_lock);
	if (mp->tm_bytes_used + sz >= lim) {
		mutex_exit(&mp->tm_acc_lock);
		return false;
	}
	mp->tm_bytes_used += sz;
	mutex_exit(&mp->tm_acc_lock);
	return true;
}

void
tmpfs_mem_decr(struct tmpfs_mount *mp, size_t sz)
{

	mutex_enter(&mp->tm_acc_lock);
	KASSERT(mp->tm_bytes_used >= sz);
	mp->tm_bytes_used -= sz;
	mutex_exit(&mp->tm_acc_lock);
}

struct tmpfs_dirent *
tmpfs_dirent_get(struct tmpfs_mount *mp)
{

	if (!tmpfs_mem_incr(mp, sizeof(struct tmpfs_dirent))) {
		return NULL;
	}
	return pool_get(&mp->tm_dirent_pool, PR_WAITOK);
}

void
tmpfs_dirent_put(struct tmpfs_mount *mp, struct tmpfs_dirent *de)
{

	tmpfs_mem_decr(mp, sizeof(struct tmpfs_dirent));
	pool_put(&mp->tm_dirent_pool, de);
}

struct tmpfs_node *
tmpfs_node_get(struct tmpfs_mount *mp)
{

	if (!tmpfs_mem_incr(mp, sizeof(struct tmpfs_node))) {
		return NULL;
	}
	return pool_get(&mp->tm_node_pool, PR_WAITOK);
}

void
tmpfs_node_put(struct tmpfs_mount *mp, struct tmpfs_node *tn)
{

	tmpfs_mem_decr(mp, sizeof(struct tmpfs_node));
	pool_put(&mp->tm_node_pool, tn);
}

/*
 * Quantum size to round-up the tmpfs names in order to reduce re-allocations.
 */

#define	TMPFS_NAME_QUANTUM	(32)

char *
tmpfs_strname_alloc(struct tmpfs_mount *mp, size_t len)
{
	const size_t sz = roundup2(len, TMPFS_NAME_QUANTUM);

	KASSERT(sz > 0 && sz <= 1024);
	if (!tmpfs_mem_incr(mp, sz)) {
		return NULL;
	}
	return kmem_alloc(sz, KM_SLEEP);
}

void
tmpfs_strname_free(struct tmpfs_mount *mp, char *str, size_t len)
{
	const size_t sz = roundup2(len, TMPFS_NAME_QUANTUM);

	KASSERT(sz > 0 && sz <= 1024);
	tmpfs_mem_decr(mp, sz);
	kmem_free(str, sz);
}

bool
tmpfs_strname_neqlen(struct componentname *fcnp, struct componentname *tcnp)
{
	const size_t fln = roundup2(fcnp->cn_namelen, TMPFS_NAME_QUANTUM);
	const size_t tln = roundup2(tcnp->cn_namelen, TMPFS_NAME_QUANTUM);

	return (fln != tln) || memcmp(fcnp->cn_nameptr, tcnp->cn_nameptr, fln);
}
