/*	$NetBSD: _lwp.c,v 1.7 2011/03/15 07:40:18 matt Exp $	*/

/*
 * Copyright (c) 2001 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Wayne Knowles
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

#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
__RCSID("$NetBSD: _lwp.c,v 1.7 2011/03/15 07:40:18 matt Exp $");
#endif /* LIBC_SCCS and not lint */

#include "namespace.h"
#include <sys/types.h>
#include <ucontext.h>
#include <lwp.h>
#include <stdlib.h>

#define CALLFRAME_SIZ	24

void
_lwp_makecontext(ucontext_t *u, void (*start)(void *),
	void *arg, void *tcb, caddr_t stack_base, size_t stack_size)
{
	caddr_t sp;
	__greg_t *gr;

	getcontext(u);
	gr = u->uc_mcontext.__gregs;
	u->uc_link = NULL;

	u->uc_stack.ss_sp = stack_base;
	u->uc_stack.ss_size = stack_size;
	sp = stack_base + stack_size - CALLFRAME_SIZ;

	gr[_REG_EPC] = (uintptr_t) start;
	gr[_REG_T9] = (uintptr_t) start; /* required for .abicalls */
	gr[_REG_RA] = (uintptr_t) _lwp_exit;
	gr[_REG_A0] = (uintptr_t) arg;
	gr[_REG_SP] = (uintptr_t) sp;
	u->uc_mcontext._mc_tlsbase =
	    (uintptr_t)tcb + TLS_TP_OFFSET + sizeof(struct tls_tcb);
	u->uc_flags |= _UC_TLSBASE;
}
