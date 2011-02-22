/*	$NetBSD: radixtree.h,v 1.1 2011/02/22 21:31:15 yamt Exp $	*/

/*-
 * Copyright (c)2011 YAMAMOTO Takashi,
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#if !defined(_SYS_RADIXTREE_H_)
#define	_SYS_RADIXTREE_H_

struct radix_tree {
	void *t_root;
	unsigned int t_height;
};

#if defined(_KERNEL)
#include <sys/types.h>
#else /* defined(_KERNEL) */
#include <stdbool.h>
#include <stdint.h>
#endif /* defined(_KERNEL) */

/*
 * subsystem
 */

#if defined(_KERNEL)
void radix_tree_init(void);
#endif /* defined(_KERNEL) */

/*
 * tree
 */

void radix_tree_init_tree(struct radix_tree *);
void radix_tree_fini_tree(struct radix_tree *);

/*
 * node
 */

int radix_tree_insert_node(struct radix_tree *, uint64_t, void *);
void *radix_tree_replace_node(struct radix_tree *, uint64_t, void *);
void *radix_tree_remove_node(struct radix_tree *, uint64_t);
void *radix_tree_lookup_node(struct radix_tree *, uint64_t);
unsigned int radix_tree_gang_lookup_node(struct radix_tree *, uint64_t,
    void **, unsigned int);

/*
 * tag
 */

typedef int radix_tree_tagid_t;
#define	RADIX_TREE_TAG_ID_MAX	2
bool radix_tree_get_tag(struct radix_tree *, uint64_t, radix_tree_tagid_t);
void radix_tree_set_tag(struct radix_tree *, uint64_t, radix_tree_tagid_t);
void radix_tree_clear_tag(struct radix_tree *, uint64_t, radix_tree_tagid_t);
unsigned int radix_tree_gang_lookup_tagged_node(struct radix_tree *, uint64_t,
    void **, unsigned int, radix_tree_tagid_t);

#endif /* !defined(_SYS_RADIXTREE_H_) */
