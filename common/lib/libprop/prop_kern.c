/*	$NetBSD: prop_kern.c,v 1.5 2006/10/26 18:51:21 thorpej Exp $	*/

/*-
 * Copyright (c) 2006 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe.
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
 *      This product includes software developed by the NetBSD
 *      Foundation, Inc. and its contributors.
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

#if defined(__NetBSD__)

#include <sys/types.h>
#include <sys/ioctl.h>

#include <prop/proplib.h>

#if !defined(_KERNEL) && !defined(_STANDALONE)
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static int
_prop_object_pack_pref(prop_object_t obj, struct plistref *pref, char **bufp)
{
	char *buf;

	switch (prop_object_type(obj)) {
	case PROP_TYPE_DICTIONARY:
		buf = prop_dictionary_externalize(obj);
		break;
	case PROP_TYPE_ARRAY:
		buf = prop_array_externalize(obj);
		break;
	default:
		return (ENOTSUP);
	}
	if (buf == NULL) {
		/* Assume we ran out of memory. */
		return (ENOMEM);
	}
	pref->pref_plist = buf;
	pref->pref_len = strlen(buf) + 1;

	*bufp = buf;

	return (0);
}

static int
_prop_object_send_ioctl(prop_object_t obj, int fd, unsigned long cmd)
{
	struct plistref pref;
	char *buf;
	int error;

	error = _prop_object_pack_pref(obj, &pref, &buf);
	if (error)
		return (error);

	if (ioctl(fd, cmd, &pref) == -1)
		error = errno;
	else
		error = 0;
	
	free(buf);

	return (error);
}

/*
 * prop_array_send_ioctl --
 *	Send an array to the kernel using the specified ioctl.
 */
int
prop_array_send_ioctl(prop_array_t array, int fd, unsigned long cmd)
{

	return (_prop_object_send_ioctl(array, fd, cmd));
}

/*
 * prop_dictionary_send_ioctl --
 *	Send a dictionary to the kernel using the specified ioctl.
 */
int
prop_dictionary_send_ioctl(prop_dictionary_t dict, int fd, unsigned long cmd)
{

	return (_prop_object_send_ioctl(dict, fd, cmd));
}

static int
_prop_object_unpack_pref(const struct plistref *pref, prop_type_t type,
			 prop_object_t *objp)
{
	prop_object_t obj = NULL;
	char *buf;
	int error = 0;

	if (pref->pref_len == 0) {
		/*
		 * This should never happen; we should always get the XML
		 * for an empty dictionary if it's really empty.
		 */
		error = EIO;
		goto out;
	} else {
		buf = pref->pref_plist;
		buf[pref->pref_len - 1] = '\0';	/* extra insurance */
		switch (type) {
		case PROP_TYPE_DICTIONARY:
			obj = prop_dictionary_internalize(buf);
			break;
		case PROP_TYPE_ARRAY:
			obj = prop_array_internalize(buf);
			break;
		default:
			error = ENOTSUP;
		}
		(void) munmap(buf, pref->pref_len);
		if (obj == NULL && error == 0)
			error = EIO;
	}

 out:
	if (error == 0)
		*objp = obj;
	return (error);
}

/*
 * prop_array_recv_ioctl --
 *	Receive an array from the kernel using the specified ioctl.
 */
int
prop_array_recv_ioctl(int fd, unsigned long cmd, prop_array_t *arrayp)
{
	struct plistref pref;

	if (ioctl(fd, cmd, &pref) == -1)
		return (errno);
	
	return (_prop_object_unpack_pref(&pref, PROP_TYPE_ARRAY,
					 (prop_object_t *)arrayp));
}

/*
 * prop_dictionary_recv_ioctl --
 *	Receive a dictionary from the kernel using the specified ioctl.
 */
int
prop_dictionary_recv_ioctl(int fd, unsigned long cmd, prop_dictionary_t *dictp)
{
	struct plistref pref;

	if (ioctl(fd, cmd, &pref) == -1)
		return (errno);

	return (_prop_object_unpack_pref(&pref, PROP_TYPE_DICTIONARY,
					 (prop_object_t *)dictp));
}

/*
 * prop_dictionary_sendrecv_ioctl --
 *	Combination send/receive a dictionary to/from the kernel using
 *	the specified ioctl.
 */
int
prop_dictionary_sendrecv_ioctl(prop_dictionary_t dict, int fd,
			       unsigned long cmd, prop_dictionary_t *dictp)
{
	struct plistref pref;
	char *buf;
	int error;

	error = _prop_object_pack_pref(dict, &pref, &buf);
	if (error)
		return (error);

	if (ioctl(fd, cmd, &pref) == -1)
		error = errno;
	else
		error = 0;
	
	free(buf);

	if (error)
		return (error);

	return (_prop_object_unpack_pref(&pref, PROP_TYPE_DICTIONARY,
			    (prop_object_t *)dictp));
}
#endif /* !_KERNEL && !_STANDALONE */

#if defined(_KERNEL)
#include <sys/param.h>
#include <sys/mman.h>
#include <sys/errno.h>
#include <sys/malloc.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/resource.h>

#include <uvm/uvm.h>

static int
_prop_object_copyin_ioctl(const struct plistref *pref, const prop_type_t type,
			  const u_long cmd, prop_object_t *objp)
{
	prop_object_t obj = NULL;
	char *buf;
	int error;

	if ((cmd & IOC_IN) == 0)
		return (EFAULT);

	/*
	 * Allocate an extra byte so we can guarantee NUL-termination.
	 * XXX Some sanity check on the size?
	 */
	buf = malloc(pref->pref_len + 1, M_TEMP, M_WAITOK);
	error = copyin(pref->pref_plist, buf, pref->pref_len);
	if (error) {
		free(buf, M_TEMP);
		return (error);
	}
	buf[pref->pref_len] = '\0';

	switch (type) {
	case PROP_TYPE_ARRAY:
		obj = prop_array_internalize(buf);
		break;
	case PROP_TYPE_DICTIONARY:
		obj = prop_dictionary_internalize(buf);
		break;
	default:
		error = ENOTSUP;
	}

	free(buf, M_TEMP);
	if (obj == NULL) {
		if (error == 0)
			error = EIO;
	} else {
		*objp = obj;
	}
	return (error);
}

/*
 * prop_array_copyin_ioctl --
 *	Copy in an array send with an ioctl.
 */
int
prop_array_copyin_ioctl(const struct plistref *pref, const u_long cmd,
			prop_array_t *arrayp)
{
	return (_prop_object_copyin_ioctl(pref, PROP_TYPE_ARRAY,
					  cmd, (prop_object_t *)arrayp));
}

/*
 * prop_dictionary_copyin_ioctl --
 *	Copy in a dictionary sent with an ioctl.
 */
int
prop_dictionary_copyin_ioctl(const struct plistref *pref, const u_long cmd,
			     prop_dictionary_t *dictp)
{
	return (_prop_object_copyin_ioctl(pref, PROP_TYPE_DICTIONARY,
					  cmd, (prop_object_t *)dictp));
}

static int
_prop_object_copyout_ioctl(struct plistref *pref, const u_long cmd,
			   prop_object_t obj)
{
	struct lwp *l = curlwp;		/* XXX */
	struct proc *p = l->l_proc;
	char *buf;
	size_t len;
	int error = 0;
	vaddr_t uaddr;

	if ((cmd & IOC_OUT) == 0)
		return (EFAULT);

	switch (prop_object_type(obj)) {
	case PROP_TYPE_ARRAY:
		buf = prop_array_externalize(obj);
		break;
	case PROP_TYPE_DICTIONARY:
		buf = prop_dictionary_externalize(obj);
		break;
	default:
		return (ENOTSUP);
	}
	if (buf == NULL)
		return (ENOMEM);

	len = strlen(buf) + 1;

	error = uvm_mmap(&p->p_vmspace->vm_map,
			 &uaddr, round_page(len),
			 VM_PROT_READ|VM_PROT_WRITE,
			 VM_PROT_READ|VM_PROT_WRITE,
			 MAP_PRIVATE|MAP_ANON,
			 NULL, 0,
			 p->p_rlimit[RLIMIT_MEMLOCK].rlim_cur);
	
	if (error == 0) {
		error = copyout(buf, (char *)uaddr, len);
		if (error == 0) {
			pref->pref_plist = (char *)uaddr;
			pref->pref_len   = len;
		}
	}

	free(buf, M_TEMP);

	return (error);
}

/*
 * prop_array_copyout_ioctl --
 *	Copy out an array being received with an ioctl.
 */
int
prop_array_copyout_ioctl(struct plistref *pref, const u_long cmd,
			 prop_array_t array)
{
	return (_prop_object_copyout_ioctl(pref, cmd, array));
}

/*
 * prop_dictionary_copyout_ioctl --
 *	Copy out a dictionary being received with an ioctl.
 */
int
prop_dictionary_copyout_ioctl(struct plistref *pref, const u_long cmd,
			      prop_dictionary_t dict)
{
	return (_prop_object_copyout_ioctl(pref, cmd, dict));
}
#endif /* _KERNEL */

#endif /* __NetBSD__ */
