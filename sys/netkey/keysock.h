/*	$NetBSD: keysock.h,v 1.4 1999/07/06 12:23:26 itojun Exp $	*/

/*
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
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
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/* KAME Id: keysock.h,v 1.1.6.3.6.1 1999/05/17 17:03:19 itojun Exp */

#ifndef _NETKEY_KEYSOCK_H_
#define _NETKEY_KEYSOCK_H_

#ifdef __NetBSD__
# ifdef _KERNEL
#  define KERNEL
# endif
#endif

#if defined(KERNEL)
struct keycb {
	struct rawcb kp_raw;	/* rawcb */
	int kp_promisc;		/* promiscuous mode */
	int kp_registered;	/* registered socket */
};

extern int key_output __P((struct mbuf *, ...));
#ifndef __NetBSD__
extern int key_usrreq __P((struct socket *, int, struct mbuf *, struct mbuf *, struct mbuf *));
#else
extern int key_usrreq __P((struct socket *,
	int, struct mbuf *, struct mbuf *, struct mbuf *, struct proc *));
#endif

#define KEY_SENDUP_ONE		0
#define KEY_SENDUP_ALL		1
#define KEY_SENDUP_REGISTERED	2

extern int key_sendup __P((struct socket *, struct sadb_msg *, u_int, int));
#else
#if 0	/* no library defined for this */
extern int key_sendup __P((int, struct sadb_msg *, u_int, int));
#endif
#endif /* defined(KERNEL) */

#endif _NETKEY_KEYSOCK_H_
