/*	$NetBSD: svr4_exec.c,v 1.39 2000/12/09 12:38:25 jdolecek Exp $	 */

/*-
 * Copyright (c) 1994 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Christos Zoulas.
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
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
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

#define	ELFSIZE		32				/* XXX should die */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/proc.h>

#include <machine/svr4_machdep.h>

#include <compat/svr4/svr4_types.h>
#include <compat/svr4/svr4_syscall.h>
#include <compat/svr4/svr4_errno.h>
#include <compat/svr4/svr4_signal.h>

extern char svr4_sigcode[], svr4_esigcode[];
extern struct sysent svr4_sysent[];
extern const char * const svr4_syscallnames[];
void syscall __P((void));

const struct emul emul_svr4 = {
	"svr4",
	"/emul/svr4",
	native_to_svr4_errno,
	svr4_sendsig,
	SVR4_SYS_syscall,
	SVR4_SYS_MAXSYSCALL,
	svr4_sysent,
	svr4_syscallnames,
	svr4_sigcode,
	svr4_esigcode,
	NULL,
	NULL,
	NULL,
	0,
#ifdef	SVR4_MACHDEP_HAS_SEPARATED_SYSCALL
	svr4_syscall,
#else
	syscall,
#endif
};
