/*	$NetBSD: framebuf.c,v 1.9 2007/05/16 09:41:04 pooka Exp $	*/

/*
 * Copyright (c) 2007  Antti Kantee.  All Rights Reserved.
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
#if !defined(lint)
__RCSID("$NetBSD: framebuf.c,v 1.9 2007/05/16 09:41:04 pooka Exp $");
#endif /* !lint */

#include <sys/types.h>
#include <sys/queue.h>

#include <assert.h>
#include <errno.h>
#include <poll.h>
#include <puffs.h>
#include <stdlib.h>
#include <unistd.h>

#include "puffs_priv.h"

struct puffs_framebuf {
	struct puffs_cc *pcc;	/* pcc to continue with */
	/* OR */
	puffs_framev_cb fcb;	/* non-blocking callback */
	void *fcb_arg;		/* argument for previous */

	uint8_t *buf;		/* buffer base */
	size_t len;		/* total length */

	size_t offset;		/* cursor, telloff() */
	size_t maxoff;		/* maximum offset for data, tellsize() */

	volatile int rv;	/* errno value */

	int	istat;

	TAILQ_ENTRY(puffs_framebuf) pfb_entries;
};
#define ISTAT_NODESTROY	0x01	/* indestructible by framebuf_destroy() */
#define ISTAT_INTERNAL	0x02	/* never leaves library			*/
#define ISTAT_NOREPLY	0x04	/* nuke after sending 			*/

#define PUFBUF_INCRALLOC 65536	/* 64k ought to be enough for anyone */
#define PUFBUF_REMAIN(p) (p->len - p->offset)

static struct puffs_fctrl_io *
getfiobyfd(struct puffs_usermount *pu, int fd)
{
	struct puffs_framectrl *pfctrl = &pu->pu_framectrl;
	struct puffs_fctrl_io *fio;

	LIST_FOREACH(fio, &pfctrl->fb_ios, fio_entries)
		if (fio->io_fd == fd)
			return fio;
	return NULL;
}

struct puffs_framebuf *
puffs_framebuf_make()
{
	struct puffs_framebuf *pufbuf;

	pufbuf = malloc(sizeof(struct puffs_framebuf));
	if (pufbuf == NULL)
		return NULL;
	memset(pufbuf, 0, sizeof(struct puffs_framebuf));

	pufbuf->buf = malloc(PUFBUF_INCRALLOC);
	pufbuf->len = PUFBUF_INCRALLOC;
	if (pufbuf->buf == NULL) {
		free(pufbuf);
		return NULL;
	}

	puffs_framebuf_recycle(pufbuf);
	return pufbuf;
}

void
puffs_framebuf_destroy(struct puffs_framebuf *pufbuf)
{

	assert((pufbuf->istat & ISTAT_NODESTROY) == 0);

	free(pufbuf->buf);
	free(pufbuf);
}

void
puffs_framebuf_recycle(struct puffs_framebuf *pufbuf)
{

	assert((pufbuf->istat & ISTAT_NODESTROY) == 0);

	pufbuf->offset = 0;
	pufbuf->maxoff = 0;
	pufbuf->istat = 0;
}

static int
reservespace(struct puffs_framebuf *pufbuf, size_t off, size_t wantsize)
{
	size_t incr;
	void *nd;

	if (off <= pufbuf->len && pufbuf->len - off >= wantsize)
		return 0;

	for (incr = PUFBUF_INCRALLOC;
	    pufbuf->len + incr < off + wantsize;
	    incr += PUFBUF_INCRALLOC)
		continue;

	nd = realloc(pufbuf->buf, pufbuf->offset + incr);
	if (nd == NULL)
		return -1;

	pufbuf->buf = nd;
	pufbuf->len += incr;

	return 0;
}

int
puffs_framebuf_reserve_space(struct puffs_framebuf *pufbuf, size_t wantsize)
{

	return reservespace(pufbuf, pufbuf->offset, wantsize);
}

int
puffs_framebuf_putdata(struct puffs_framebuf *pufbuf,
	const void *data, size_t dlen)
{

	if (PUFBUF_REMAIN(pufbuf) < dlen)
		if (puffs_framebuf_reserve_space(pufbuf, dlen) == -1)
			return -1;

	memcpy(pufbuf->buf + pufbuf->offset, data, dlen);
	pufbuf->offset += dlen;

	if (pufbuf->offset > pufbuf->maxoff)
		pufbuf->maxoff = pufbuf->offset;

	return 0;
}

int
puffs_framebuf_putdata_atoff(struct puffs_framebuf *pufbuf, size_t offset,
	const void *data, size_t dlen)
{

	if (reservespace(pufbuf, offset, dlen) == -1)
		return -1;

	memcpy(pufbuf->buf + offset, data, dlen);

	if (offset + dlen > pufbuf->maxoff)
		pufbuf->maxoff = offset + dlen;

	return 0;
}

int
puffs_framebuf_getdata(struct puffs_framebuf *pufbuf, void *data, size_t dlen)
{

	if (pufbuf->maxoff < pufbuf->offset + dlen) {
		errno = ENOBUFS;
		return -1;
	}

	memcpy(data, pufbuf->buf + pufbuf->offset, dlen);
	pufbuf->offset += dlen;

	return 0;
}

int
puffs_framebuf_getdata_atoff(struct puffs_framebuf *pufbuf, size_t offset,
	void *data, size_t dlen)
{

	if (pufbuf->maxoff < offset + dlen) {
		errno = ENOBUFS;
		return -1;
	}

	memcpy(data, pufbuf->buf + offset, dlen);
	return 0;
}

size_t
puffs_framebuf_telloff(struct puffs_framebuf *pufbuf)
{

	return pufbuf->offset;
}

size_t
puffs_framebuf_tellsize(struct puffs_framebuf *pufbuf)
{

	return pufbuf->maxoff;
}

size_t
puffs_framebuf_remaining(struct puffs_framebuf *pufbuf)
{

	return puffs_framebuf_tellsize(pufbuf) - puffs_framebuf_telloff(pufbuf);
}

int
puffs_framebuf_seekset(struct puffs_framebuf *pufbuf, size_t newoff)
{

	if (reservespace(pufbuf, newoff, 0) == -1)
		return -1;

	pufbuf->offset = newoff;
	return 0;
}

int
puffs_framebuf_getwindow(struct puffs_framebuf *pufbuf, size_t winoff,
	void **data, size_t *dlen)
{
	size_t winlen;

#ifdef WINTESTING
	winlen = MIN(*dlen, 32);
#else
	winlen = *dlen;
#endif

	if (reservespace(pufbuf, winoff, winlen) == -1)
		return -1;

	*data = pufbuf->buf + winoff;
	if (pufbuf->maxoff < winoff + winlen)
		pufbuf->maxoff = winoff + winlen;

	return 0;
}

static void
errnotify(struct puffs_framebuf *pufbuf, int error)
{

	pufbuf->rv = error;
	if (pufbuf->pcc) {
		puffs_goto(pufbuf->pcc);
	} else if (pufbuf->fcb) {
		pufbuf->istat &= ~ISTAT_NODESTROY;
		pufbuf->fcb(puffs_cc_getusermount(pufbuf->pcc),
		    pufbuf, pufbuf->fcb_arg);
	} else {
		pufbuf->istat &= ~ISTAT_NODESTROY;
		puffs_framebuf_destroy(pufbuf);
	}
}

#define GETFIO(fd)							\
do {									\
	fio = getfiobyfd(pu, fd);					\
	if (fio == NULL) {						\
		errno = EINVAL;						\
		return -1;						\
	}								\
	if (fio->stat & FIO_WRGONE) {					\
		errno = ESHUTDOWN;					\
		return -1;						\
	}								\
} while (/*CONSTCOND*/0)

int
puffs_framev_enqueue_cc(struct puffs_cc *pcc, int fd,
	struct puffs_framebuf *pufbuf)
{
	struct puffs_usermount *pu = puffs_cc_getusermount(pcc);
	struct puffs_fctrl_io *fio;

	/*
	 * Technically we shouldn't allow this is RDGONE, but it's
	 * difficult to trap write close without allowing writes.
	 * And besides, there's probably a disconnect sequence in
	 * the protocol, so unexpectedly getting a closed fd is
	 * most likely an error condition.
	 */
	GETFIO(fd);

	pufbuf->pcc = pcc;
	pufbuf->fcb = NULL;
	pufbuf->fcb_arg = NULL;

	pufbuf->offset = 0;
	pufbuf->istat |= ISTAT_NODESTROY;

	TAILQ_INSERT_TAIL(&fio->snd_qing, pufbuf, pfb_entries);

	puffs_cc_yield(pcc);
	if (pufbuf->rv) {
		pufbuf->istat &= ~ISTAT_NODESTROY;
		errno = pufbuf->rv;
		return -1;
	}

	return 0;
}

int
puffs_framev_enqueue_cb(struct puffs_usermount *pu, int fd,
	struct puffs_framebuf *pufbuf, puffs_framev_cb fcb, void *arg)
{
	struct puffs_fctrl_io *fio;

	/* see enqueue_cc */
	GETFIO(fd);

	pufbuf->pcc = NULL;
	pufbuf->fcb = fcb;
	pufbuf->fcb_arg = arg;

	pufbuf->offset = 0;
	pufbuf->istat |= ISTAT_NODESTROY;

	TAILQ_INSERT_TAIL(&fio->snd_qing, pufbuf, pfb_entries);

	return 0;
}

int
puffs_framev_enqueue_justsend(struct puffs_usermount *pu, int fd,
	struct puffs_framebuf *pufbuf, int reply)
{
	struct puffs_fctrl_io *fio;

	GETFIO(fd);

	pufbuf->pcc = NULL;
	pufbuf->fcb = NULL;
	pufbuf->fcb_arg = NULL;

	pufbuf->offset = 0;
	pufbuf->istat |= ISTAT_NODESTROY;
	if (!reply)
		pufbuf->istat |= ISTAT_NOREPLY;

	TAILQ_INSERT_TAIL(&fio->snd_qing, pufbuf, pfb_entries);

	return 0;
}

static struct puffs_framebuf *
findbuf(struct puffs_usermount *pu, struct puffs_framectrl *fctrl,
	struct puffs_fctrl_io *fio, struct puffs_framebuf *findme)
{
	struct puffs_framebuf *cand;

	TAILQ_FOREACH(cand, &fio->res_qing, pfb_entries)
		if (fctrl->cmpfb(pu, findme, cand))
			break;

	if (cand == NULL)
		return NULL;

	TAILQ_REMOVE(&fio->res_qing, cand, pfb_entries);
	return cand;
}

static void
moveinfo(struct puffs_framebuf *from, struct puffs_framebuf *to)
{

	assert(from->istat & ISTAT_INTERNAL);

	/* migrate buffer */
	free(to->buf);
	to->buf = from->buf;
	from->buf = NULL;

	/* migrate buffer info */
	to->len = from->len;
	to->offset = from->offset;
	to->maxoff = from->maxoff;
}

void
puffs_framev_input(struct puffs_usermount *pu, struct puffs_framectrl *fctrl,
	struct puffs_fctrl_io *fio, struct puffs_putreq *ppr)
{
	struct puffs_framebuf *pufbuf, *appbuf;
	int rv, complete;

	if (fio->stat & FIO_DEAD)
		return;

	for (;;) {
		if ((pufbuf = fio->cur_in) == NULL) {
			pufbuf = puffs_framebuf_make();
			if (pufbuf == NULL)
				return;
			pufbuf->istat |= ISTAT_INTERNAL;
			fio->cur_in = pufbuf;
		}

		complete = 0;
		rv = fctrl->rfb(pu, pufbuf, fio->io_fd, &complete);

		/* error */
		if (rv) {
			puffs_framev_readclose(pu, fio, rv);
			return;
		}

		/* partial read, come back to fight another day */
		if (complete == 0)
			break;

		/* else: full read, process */

		appbuf = findbuf(pu, fctrl, fio, pufbuf);

		/* XXX: error delivery? */
		if (appbuf == NULL) {
			/* errno = ENOMSG; */
			return;
		}
			
		appbuf->istat &= ~ISTAT_NODESTROY;
		moveinfo(pufbuf, appbuf);
		if (appbuf->pcc) {
			puffs_docc(appbuf->pcc, ppr);
		} else if (appbuf->fcb) {
			appbuf->fcb(pu, appbuf, appbuf->fcb_arg);
		} else {
			puffs_framebuf_destroy(appbuf);
		}
		puffs_framebuf_destroy(pufbuf);

		/* hopeless romantics, here we go again */
		fio->cur_in = NULL;
	}
}

int
puffs_framev_output(struct puffs_usermount *pu, struct puffs_framectrl *fctrl,
	struct puffs_fctrl_io *fio)
{
	struct puffs_framebuf *pufbuf, *pufbuf_next;
	int rv, complete, error;

	if (fio->stat & FIO_DEAD)
		return 0;

	for (pufbuf = TAILQ_FIRST(&fio->snd_qing), error = 0;
	    pufbuf;
	    pufbuf = pufbuf_next) {
		complete = 0;
		pufbuf_next = TAILQ_NEXT(pufbuf, pfb_entries);
		rv = fctrl->wfb(pu, pufbuf, fio->io_fd, &complete);

		if (rv) {
			puffs_framev_writeclose(pu, fio, rv);
			error = 1;
			break;
		}

		/* partial write */
		if (complete == 0)
			return error;

		/* else, complete write */
		TAILQ_REMOVE(&fio->snd_qing, pufbuf, pfb_entries);

		/* can't wait for result if we can't read */
		if (fio->stat & FIO_RDGONE) {
			errnotify(pufbuf, ENXIO);
			error = 1;

		} else  if ((pufbuf->istat & ISTAT_NOREPLY) == 0) {
			TAILQ_INSERT_TAIL(&fio->res_qing, pufbuf,
			    pfb_entries);
		} else {
			pufbuf->istat &= ~ISTAT_NODESTROY;
			puffs_framebuf_destroy(pufbuf);
		}

		/* omstart! */
	}

	return error;
}

int
puffs_framev_addfd(struct puffs_usermount *pu, int fd)
{
	struct puffs_framectrl *pfctrl = &pu->pu_framectrl;
	struct puffs_fctrl_io *fio;
	struct kevent kev[2];
	struct kevent *newevs;
	size_t nfds;
	int rv;

	nfds = pfctrl->nfds+1;
	newevs = realloc(pfctrl->evs, (2*nfds+1) * sizeof(struct kevent));
	if (newevs == NULL)
		return -1;
	pfctrl->evs = newevs;

	fio = malloc(sizeof(struct puffs_fctrl_io));
	if (fio == NULL)
		return -1;

	if (pu->pu_state & PU_INLOOP) {
		EV_SET(&kev[0], fd, EVFILT_READ, EV_ADD, 0, 0, (intptr_t)fio);
		EV_SET(&kev[1], fd, EVFILT_WRITE, EV_ADD|EV_DISABLE,
		    0, 0, (intptr_t)fio);
		rv = kevent(pu->pu_kq, kev, 2, NULL, 0, NULL);
		if (rv == -1) {
			free(fio);
			return -1;
		}
	}

	fio->io_fd = fd;
	fio->cur_in = NULL;
	fio->stat = 0;
	TAILQ_INIT(&fio->snd_qing);
	TAILQ_INIT(&fio->res_qing);

	LIST_INSERT_HEAD(&pfctrl->fb_ios, fio, fio_entries);
	pfctrl->nfds = nfds;

	return 0;
}

void
puffs_framev_readclose(struct puffs_usermount *pu,
	struct puffs_fctrl_io *fio, int error)
{
	struct puffs_framebuf *pufbuf;
	struct kevent kev;
	int notflag;

	if (fio->stat & FIO_RDGONE || fio->stat & FIO_DEAD)
		return;
	fio->stat |= FIO_RDGONE;

	if (fio->cur_in) {
		puffs_framebuf_destroy(fio->cur_in);
		fio->cur_in = NULL;
	}

	while ((pufbuf = TAILQ_FIRST(&fio->res_qing)) != NULL) {
		TAILQ_REMOVE(&fio->res_qing, pufbuf, pfb_entries);
		errnotify(pufbuf, error);
	}

	EV_SET(&kev, fio->io_fd, EVFILT_READ, EV_DELETE, 0, 0, 0);
	(void) kevent(pu->pu_kq, &kev, 1, NULL, 0, NULL);

	notflag = PUFFS_FBGONE_READ;
	if (fio->stat & FIO_WRGONE)
		notflag |= PUFFS_FBGONE_WRITE;

	pu->pu_framectrl.fdnotfn(pu, fio->io_fd, notflag);
}

void
puffs_framev_writeclose(struct puffs_usermount *pu,
	struct puffs_fctrl_io *fio, int error)
{
	struct puffs_framebuf *pufbuf;
	struct kevent kev;
	int notflag;

	if (fio->stat & FIO_WRGONE || fio->stat & FIO_DEAD)
		return;
	fio->stat |= FIO_WRGONE;

	while ((pufbuf = TAILQ_FIRST(&fio->snd_qing)) != NULL) {
		TAILQ_REMOVE(&fio->snd_qing, pufbuf, pfb_entries);
		errnotify(pufbuf, error);
	}

	EV_SET(&kev, fio->io_fd, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
	(void) kevent(pu->pu_kq, &kev, 1, NULL, 0, NULL);

	notflag = PUFFS_FBGONE_WRITE;
	if (fio->stat & FIO_RDGONE)
		notflag |= PUFFS_FBGONE_READ;

	pu->pu_framectrl.fdnotfn(pu, fio->io_fd, notflag);
}

static int
removefio(struct puffs_usermount *pu, struct puffs_fctrl_io *fio, int error)
{
	struct puffs_framectrl *pfctrl = &pu->pu_framectrl;

	LIST_REMOVE(fio, fio_entries);
	if (pu->pu_state & PU_INLOOP) {
		puffs_framev_readclose(pu, fio, error);
		puffs_framev_writeclose(pu, fio, error);
	}

	/* don't bother with realloc */
	pfctrl->nfds--;

	/* don't free us yet, might have some references in event arrays */
	fio->stat |= FIO_DEAD;
	LIST_INSERT_HEAD(&pfctrl->fb_ios_rmlist, fio, fio_entries);

	return 0;

}

int
puffs_framev_removefd(struct puffs_usermount *pu, int fd, int error)
{
	struct puffs_fctrl_io *fio;

	fio = getfiobyfd(pu, fd);
	if (fio == NULL) {
		errno = ENXIO;
		return -1;
	}

	return removefio(pu, fio, error ? error : ECONNRESET);
}

static void
defaultnot(struct puffs_usermount *pu, int fd, int what)
{

	if (PUFFS_FBGONE_BOTH(what))
		(void) puffs_framev_removefd(pu, fd, ECONNRESET);
}

void
puffs_framev_unmountonclose(struct puffs_usermount *pu, int fd, int what)
{

	/* XXX & X: unmount is non-sensible */
	defaultnot(pu, fd, what);
	if (PUFFS_FBGONE_BOTH(what))
		PU_SETSTATE(pu, PUFFS_STATE_UNMOUNTED);
}

void
puffs_framev_init(struct puffs_usermount *pu,
	puffs_framev_readframe_fn rfb, puffs_framev_writeframe_fn wfb,
	puffs_framev_respcmp_fn cmpfb, puffs_framev_fdnotify_fn fdnotfn)
{
	struct puffs_framectrl *pfctrl;

	pfctrl = &pu->pu_framectrl;
	pfctrl->rfb = rfb;
	pfctrl->wfb = wfb;
	pfctrl->cmpfb = cmpfb;
	if (fdnotfn)
		pfctrl->fdnotfn = fdnotfn;
	else
		pfctrl->fdnotfn = defaultnot;
}

void
puffs_framev_exit(struct puffs_usermount *pu)
{
	struct puffs_framectrl *pfctrl = &pu->pu_framectrl;
	struct puffs_fctrl_io *fio;

	while ((fio = LIST_FIRST(&pfctrl->fb_ios)) != NULL)
		removefio(pu, fio, ENXIO);
	free(pfctrl->evs);

	/* closing pu->pu_kq takes care of puffsfd */
}
