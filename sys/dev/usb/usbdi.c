/*	$NetBSD: usbdi.c,v 1.4 1998/07/24 21:02:51 augustss Exp $	*/

/*
 * Copyright (c) 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * Author: Lennart Augustsson <augustss@carlstedt.se>
 *         Carlstedt Research & Technology
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

#include "opt_usbverbose.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/proc.h>
#include <sys/device.h>

#include <dev/usb/usb.h>

#include <dev/usb/usbdi.h>
#include <dev/usb/usbdi_util.h>
#include <dev/usb/usbdivar.h>

#ifdef USB_DEBUG
#define DPRINTF(x)	if (usbdebug) printf x
#define DPRINTFN(n,x)	if (usbdebug>(n)) printf x
extern int usbdebug;
#else
#define DPRINTF(x)
#define DPRINTFN(n,x)
#endif

static usbd_status usbd_ar_pipe  __P((usbd_pipe_handle pipe));
static usbd_status usbd_ar_iface __P((usbd_interface_handle iface));
static void usbd_transfer_cb __P((usbd_request_handle reqh));
static void usbd_sync_transfer_cb __P((usbd_request_handle reqh));
static usbd_status usbd_do_transfer __P((usbd_request_handle reqh));
static usbd_status usbd_start __P((usbd_pipe_handle pipe));

static SIMPLEQ_HEAD(, usbd_request) usbd_free_requests;

usbd_status 
usbd_open_pipe(iface, address, flags, pipe)
	usbd_interface_handle iface;
	u_int8_t address;
	u_int8_t flags;
	usbd_pipe_handle *pipe;
{ 
	usbd_pipe_handle p;
	struct usbd_endpoint *ep;
	int i, r;

	if (iface->state != USBD_INTERFACE_ACTIVE)
		return (USBD_INTERFACE_NOT_ACTIVE);
	for (i = 0; i < iface->idesc->bNumEndpoints; i++) {
		ep = &iface->endpoints[i];
		if (ep->edesc->bEndpointAddress == address)
			goto found;
	}
	return (USBD_BAD_ADDRESS);
 found:
	if ((flags & USBD_EXCLUSIVE_USE) &&
	    ep->refcnt != 0)
		return (USBD_IN_USE);
	r = usbd_setup_pipe(iface->device, iface, ep, &p);
	if (r != USBD_NORMAL_COMPLETION)
		return (r);
	LIST_INSERT_HEAD(&iface->pipes, p, next);
	*pipe = p;
	return (USBD_NORMAL_COMPLETION);
}

usbd_status 
usbd_open_pipe_intr(iface, address, flags, pipe, priv, buffer, length, cb)
	usbd_interface_handle iface;
	u_int8_t address;
	u_int8_t flags;
	usbd_pipe_handle *pipe;
	usbd_private_handle priv;
	void *buffer;
	u_int32_t length;
	usbd_callback cb;
{
	usbd_status r;
	usbd_request_handle reqh;
	usbd_pipe_handle ipipe;

	reqh = usbd_alloc_request();
	if (reqh == 0)
		return (USBD_NOMEM);
	r = usbd_open_pipe(iface, address, USBD_EXCLUSIVE_USE, &ipipe);
	if (r != USBD_NORMAL_COMPLETION)
		goto bad1;
	r = usbd_setup_request(reqh, ipipe, priv, buffer, length, 
			       USBD_XFER_IN | flags, USBD_NO_TIMEOUT, cb);
	if (r != USBD_NORMAL_COMPLETION)
		goto bad2;
	ipipe->intrreqh = reqh;
	r = usbd_transfer(reqh);
	*pipe = ipipe;
	if (r != USBD_IN_PROGRESS)
		goto bad3;
	return (USBD_NORMAL_COMPLETION);

 bad3:
	ipipe->intrreqh = 0;
 bad2:
	usbd_close_pipe(ipipe);
 bad1:
	usbd_free_request(reqh);
	return r;
}

usbd_status
usbd_close_pipe(pipe)
	usbd_pipe_handle pipe;
{
	if (pipe->iface->state != USBD_INTERFACE_ACTIVE)
		return (USBD_INTERFACE_NOT_ACTIVE);
	if (--pipe->refcnt != 0)
		return (USBD_NORMAL_COMPLETION);
	if (SIMPLEQ_FIRST(&pipe->queue) != 0)
		return (USBD_PENDING_REQUESTS);
	LIST_REMOVE(pipe, next);
	pipe->endpoint->refcnt--;
	pipe->methods->close(pipe);
	if (pipe->intrreqh)
		usbd_free_request(pipe->intrreqh);
	free(pipe, M_USB);
	return (USBD_NORMAL_COMPLETION);
}

usbd_status 
usbd_transfer(reqh)
	usbd_request_handle reqh;
{
	reqh->xfercb = usbd_transfer_cb;
	return (usbd_do_transfer(reqh));
}

static usbd_status
usbd_do_transfer(reqh)
	usbd_request_handle reqh;
{
	usbd_pipe_handle pipe = reqh->pipe;
	usbd_interface_handle iface = pipe->iface;
	usbd_status r;
	int s;

	DPRINTFN(10,("usbd_do_transfer: reqh=%p\n", reqh));
	reqh->done = 0;
	s = splusb();
	if (pipe->state == USBD_PIPE_IDLE ||
	    (iface && iface->state == USBD_INTERFACE_IDLE)) {
		splx(s);
		return (USBD_IS_IDLE);
	}
	SIMPLEQ_INSERT_TAIL(&pipe->queue, reqh, next);
	if (pipe->state == USBD_PIPE_ACTIVE &&
	    (!iface || iface->state == USBD_INTERFACE_ACTIVE)) {
		r = usbd_start(pipe);
	} else
		r = USBD_NOT_STARTED;
	splx(s);
	return (r);
}

static usbd_status
usbd_start(pipe)
	usbd_pipe_handle pipe;
{
	usbd_request_handle reqh;

	DPRINTFN(1, ("usbd_start: pipe=%p, running=%d\n", 
		     pipe, pipe->running));
	if (pipe->running)
		return (USBD_IN_PROGRESS);
	reqh = SIMPLEQ_FIRST(&pipe->queue);
	if (!reqh) {
		/* XXX */
		printf("usbd_start: pipe empty!\n");
		pipe->running = 0;
		return (USBD_XXX);
	}
	SIMPLEQ_REMOVE_HEAD(&pipe->queue, reqh, next);
	pipe->running = 1;
	pipe->curreqh = reqh;
	return (pipe->methods->transfer(reqh));
}


usbd_request_handle 
usbd_alloc_request()
{
	usbd_request_handle reqh;

	reqh = SIMPLEQ_FIRST(&usbd_free_requests);
	if (reqh)
		SIMPLEQ_REMOVE_HEAD(&usbd_free_requests, reqh, next);
	else
		reqh = malloc(sizeof(*reqh), M_USB, M_NOWAIT);
	if (!reqh)
		return (0);
	memset(reqh, 0, sizeof *reqh);
	return (reqh);
}

usbd_status 
usbd_free_request(reqh)
	usbd_request_handle reqh;
{
	SIMPLEQ_INSERT_HEAD(&usbd_free_requests, reqh, next);
	return (USBD_NORMAL_COMPLETION);
}

usbd_status 
usbd_setup_request(reqh, pipe, priv, buffer, length, flags, timeout, callback)
	usbd_request_handle reqh;
	usbd_pipe_handle pipe;
	usbd_private_handle priv;
	void *buffer;
	u_int32_t length;
	u_int16_t flags;
	u_int32_t timeout;
	void (*callback) __P((usbd_request_handle,
			      usbd_private_handle,
			      usbd_status));
{
	reqh->pipe = pipe;
	reqh->isreq = 0;
	reqh->priv = priv;
	reqh->buffer = buffer;
	reqh->length = length;
	reqh->actlen = 0;
	reqh->flags = flags;
	reqh->callback = callback;
	reqh->status = USBD_NOT_STARTED;
	reqh->retries = 1;
	return (USBD_NORMAL_COMPLETION);
}

usbd_status 
usbd_setup_device_request(reqh, req)
	usbd_request_handle reqh;
	usb_device_request_t *req;
{
	reqh->isreq = 1;
	reqh->request = *req;
	return (USBD_NORMAL_COMPLETION);
}

usbd_status 
usbd_setup_default_request(reqh, dev, priv, timeout, req, buffer, 
			   length, flags, callback)
	usbd_request_handle reqh;
	usbd_device_handle dev;
	usbd_private_handle priv;
	u_int32_t timeout;
	usb_device_request_t *req;
	void *buffer;
	u_int32_t length;
	u_int16_t flags;
	void (*callback) __P((usbd_request_handle,
			      usbd_private_handle,
			      usbd_status));
{
	reqh->pipe = dev->default_pipe;
	reqh->priv = priv;
	reqh->buffer = buffer;
	reqh->length = length;
	reqh->actlen = 0;
	reqh->flags = flags;
	reqh->timeout = timeout;
	reqh->status = USBD_NOT_STARTED;
	reqh->callback = callback;
	reqh->request = *req;
	reqh->isreq = 1;
	reqh->retries = 1;
	return (USBD_NORMAL_COMPLETION);
}

usbd_status 
usbd_set_request_timeout(reqh, timeout)
	usbd_request_handle reqh;
	u_int32_t timeout;
{
	reqh->timeout = timeout;
	return (USBD_NORMAL_COMPLETION);
}

usbd_status 
usbd_get_request_status(reqh, priv, buffer, count, status)
	usbd_request_handle reqh;
	usbd_private_handle *priv;
	void **buffer;
	u_int32_t *count;
	usbd_status *status;
{
	*priv = reqh->priv;
	*buffer = reqh->buffer;
	*count = reqh->actlen;
	*status = reqh->status;
	return (USBD_NORMAL_COMPLETION);
}

usbd_status 
usbd_request_device_data(reqh, req)
	usbd_request_handle reqh;
	usb_device_request_t *req;
{
	if (!reqh->isreq) 
		return (USBD_INVAL);
	*req = reqh->request;
	return (USBD_NORMAL_COMPLETION);
}

#if 0
usb_descriptor_t *
usbd_get_descriptor(iface, desc_type)
	usbd_interface_handle *iface;
	u_int8_t desc_type;
XX
#endif

usb_config_descriptor_t *
usbd_get_config_descriptor(dev)
	usbd_device_handle dev;
{
	return (dev->cdesc);
}

usb_interface_descriptor_t *
usbd_get_interface_descriptor(iface)
	usbd_interface_handle iface;
{
	return (iface->idesc);
}

usb_device_descriptor_t *
usbd_get_device_descriptor(dev)
	usbd_device_handle dev;
{
	return (&dev->ddesc);
}

usb_endpoint_descriptor_t *
usbd_interface2endpoint_descriptor(iface, index)
	usbd_interface_handle iface;
	u_int8_t index;
{
	if (index >= iface->idesc->bNumEndpoints)
		return 0;
	return (iface->endpoints[index].edesc);
}

usbd_status usbd_set_configuration(dev, conf)
	usbd_device_handle dev;
	u_int16_t conf;
{
	return usbd_set_config_no(dev, conf);
}

usbd_status 
usbd_retry_request(reqh, retry_count)
	usbd_request_handle reqh;
	u_int32_t retry_count;
{
	usbd_status r;

	r = usbd_set_pipe_state(reqh->pipe, USBD_PIPE_ACTIVE);
	if (r != USBD_NORMAL_COMPLETION)
		return (r);
	reqh->retries = retry_count;
	return (usbd_transfer(reqh));
}

usbd_status 
usbd_abort_pipe(pipe)
	usbd_pipe_handle pipe;
{
	usbd_status r;
	int s, st;

	if (pipe->iface->state != USBD_INTERFACE_ACTIVE)
		return (USBD_INTERFACE_NOT_ACTIVE);
	s = splusb();
	st = pipe->state;
	r = usbd_ar_pipe(pipe);
	pipe->state = st;
	splx(s);
	return (r);
}
	
usbd_status 
usbd_abort_interface(iface)
	usbd_interface_handle iface;
{
	usbd_status r;
	int s, st;

	s = splusb();
	st = iface->state;
	r = usbd_ar_iface(iface);
	iface->state = st;
	splx(s);
	return (r);
}

usbd_status 
usbd_reset_pipe(pipe)
	usbd_pipe_handle pipe;
{
	usbd_status r;
	int s;

	if (pipe->iface->state != USBD_INTERFACE_ACTIVE)
		return (USBD_INTERFACE_NOT_ACTIVE);
	s = splusb();
	r = usbd_ar_pipe(pipe);
	/* XXX anything else */
	pipe->state = USBD_PIPE_ACTIVE;
	splx(s);
	return (r);
}

usbd_status 
usbd_reset_interface(iface)
	usbd_interface_handle iface;
{
	usbd_status r;
	int s;

	s = splusb();
	r = usbd_ar_iface(iface);
	/* XXX anything else */
	iface->state = USBD_INTERFACE_ACTIVE;
	splx(s);
	return (r);
}

usbd_status 
usbd_clear_endpoint_stall(pipe)
	usbd_pipe_handle pipe;
{
	usbd_device_handle dev = pipe->device;
	usb_device_request_t req;
	usbd_status r;

	req.bmRequestType = UT_WRITE_ENDPOINT;
	req.bRequest = UR_CLEAR_FEATURE;
	USETW(req.wValue, UF_ENDPOINT_STALL);
	USETW(req.wIndex, pipe->endpoint->edesc->bEndpointAddress); /* XXX mask/ */
	USETW(req.wLength, 0);
	r = usbd_do_request(dev, &req, 0);
#if 0
XXX should we do this?
	if (r == USBD_NORMAL_COMPLETION) {
		pipe->state = USBD_PIPE_ACTIVE;
		/* XXX activate pipe */
	}
#endif
	return (r);
}

usbd_status 
usbd_set_pipe_state(pipe, state)
	usbd_pipe_handle pipe;
	usbd_pipe_state state;
{
	int s;

	if (pipe->iface->state != USBD_INTERFACE_ACTIVE)
		return (USBD_INTERFACE_NOT_ACTIVE);
	if (state != USBD_PIPE_ACTIVE &&
	    state != USBD_PIPE_STALLED &&
	    state != USBD_PIPE_IDLE)
		return (USBD_INVAL);
	pipe->state = state;
	if (state == USBD_PIPE_ACTIVE) {
		s = splusb();
		usbd_start(pipe);
		splx(s);
	}
	return (USBD_NORMAL_COMPLETION);
}

usbd_status 
usbd_get_pipe_state(pipe, state, endpoint_state, request_count)
	usbd_pipe_handle pipe;
	usbd_pipe_state *state;
	u_int32_t *endpoint_state;
	u_int32_t *request_count;
{
	int n;
	usbd_request_handle r;

	*state = pipe->state;
	*endpoint_state = pipe->endpoint->state;
	for (r = SIMPLEQ_FIRST(&pipe->queue), n = 0; 
	     r != 0; 
	     r = SIMPLEQ_NEXT(r, next), n++)
		;
	*request_count = n;
	return (USBD_NORMAL_COMPLETION);
}

usbd_status 
usbd_set_interface_state(iface, state)
	usbd_interface_handle iface;
	usbd_interface_state state;
{
	int ps;
	usbd_pipe_handle p;

	if (state == USBD_INTERFACE_ACTIVE)
		ps = USBD_PIPE_ACTIVE;
	else if (state == USBD_INTERFACE_STALLED)
		ps = USBD_PIPE_STALLED;
	else if (state == USBD_INTERFACE_IDLE)
		ps = USBD_PIPE_IDLE;
	else
		return (USBD_INVAL);
	iface->state = USBD_INTERFACE_ACTIVE; /* to allow setting the pipe */
	for (p = LIST_FIRST(&iface->pipes); p != 0; p = LIST_NEXT(p, next))
		usbd_set_pipe_state(p, ps);
	iface->state = state;
	return (USBD_NORMAL_COMPLETION);
}

usbd_status 
usbd_get_interface_state(iface, state)
	usbd_interface_handle iface;
	usbd_interface_state *state;
{
	*state = iface->state;
	return (USBD_NORMAL_COMPLETION);
}

usbd_status 
usbd_get_device_state(dev, state)
	usbd_device_handle dev;
	usbd_device_state *state;
{
	*state = dev->state;
	return (USBD_NORMAL_COMPLETION);
}

#if 0
usbd_status 
usbd_set_device_state(dev, state)
	usbd_device_handle dev;
	usbd_device_state state;
X
#endif

usbd_status 
usbd_device_address(dev, address)
	usbd_device_handle dev;
	u_int8_t *address;
{
	*address = dev->address;
	return (USBD_NORMAL_COMPLETION);
}

usbd_status 
usbd_endpoint_address(pipe, address)
	usbd_pipe_handle pipe;
	u_int8_t *address;
{
	*address = pipe->endpoint->edesc->bEndpointAddress;
	return (USBD_NORMAL_COMPLETION);
}

usbd_status 
usbd_endpoint_count(iface, count)
	usbd_interface_handle iface;
	u_int8_t *count;
{
	*count = iface->idesc->bNumEndpoints;
	return (USBD_NORMAL_COMPLETION);
}

usbd_status 
usbd_interface_count(dev, count)
	usbd_device_handle dev;
	u_int8_t *count;
{
	if (!dev->cdesc)
		return (USBD_NOT_CONFIGURED);
	*count = dev->cdesc->bNumInterface;
	return (USBD_NORMAL_COMPLETION);
}

u_int8_t 
usbd_bus_count()
{
	return (usb_bus_count());
}

usbd_status 
usbd_get_bus_handle(index, bus)
	u_int8_t index;
	usbd_bus_handle *bus;
{
	return (usb_get_bus_handle(index, bus));
}

usbd_status 
usbd_get_root_hub(bus, dev)
	usbd_bus_handle bus; 
	usbd_device_handle *dev;
{
	*dev = bus->root_hub;
	return (USBD_NORMAL_COMPLETION);
}

usbd_status 
usbd_port_count(dev, nports)
	usbd_device_handle dev;
	u_int8_t *nports;
{
	if (dev->hub == 0)
		return (USBD_INVAL);
	*nports = dev->hub->hubdesc.bNbrPorts;
	return (USBD_NORMAL_COMPLETION);
}

usbd_status 
usbd_hub2device_handle(dev, port, devp)
	usbd_device_handle dev;
	u_int8_t port;
	usbd_device_handle *devp;
{
	if (dev->hub == 0 || port >= dev->hub->hubdesc.bNbrPorts || 
	    dev->hub->ports[port].device == 0)
		return (USBD_INVAL);
	*devp = dev->hub->ports[port].device;
	return (USBD_NORMAL_COMPLETION);
}

usbd_status 
usbd_request2pipe_handle(reqh, pipe)
	usbd_request_handle reqh;
	usbd_pipe_handle *pipe;
{
	*pipe = reqh->pipe;
	return (USBD_NORMAL_COMPLETION);
}

usbd_status 
usbd_pipe2interface_handle(pipe, iface)
	usbd_pipe_handle pipe;
	usbd_interface_handle *iface;
{
	*iface = pipe->iface;
	return (USBD_NORMAL_COMPLETION);
}

usbd_status 
usbd_interface2device_handle(iface, dev)
	usbd_interface_handle iface;
	usbd_device_handle *dev;
{
	*dev = iface->device;
	return (USBD_NORMAL_COMPLETION);
}

usbd_status 
usbd_device2bus_handle(dev, bus)
	usbd_device_handle dev;
	usbd_bus_handle *bus;
{
	*bus = dev->bus;
	return (USBD_NORMAL_COMPLETION);
}

usbd_status 
usbd_device2interface_handle(dev, ifaceno, iface)
	usbd_device_handle dev;
	u_int8_t ifaceno;
	usbd_interface_handle *iface;
{
	if (!dev->cdesc)
		return (USBD_NOT_CONFIGURED);
	if (ifaceno >= dev->cdesc->bNumInterface)
		return (USBD_INVAL);
	*iface = &dev->ifaces[ifaceno];
	return (USBD_NORMAL_COMPLETION);
}

usbd_status 
usbd_set_interface_private_handle(iface, priv)
	usbd_interface_handle iface;
	usbd_private_handle priv;
{
	iface->priv = priv;
	return (USBD_NORMAL_COMPLETION);
}

usbd_status 
usbd_get_interface_private_handle(iface, priv)
	usbd_interface_handle iface;
	usbd_private_handle *priv;
{
	*priv = iface->priv;
	return (USBD_NORMAL_COMPLETION);
}

usbd_status 
usbd_reference_pipe(pipe)
	usbd_pipe_handle pipe;
{
	pipe->refcnt++;
	return (USBD_NORMAL_COMPLETION);
}

usbd_status 
usbd_dereference_pipe(pipe)
	usbd_pipe_handle pipe;
{
	pipe->refcnt--;
	return (USBD_NORMAL_COMPLETION);
}

usbd_lock_token
usbd_lock()
{
	return (splusb());
}

void
usbd_unlock(tok)
	usbd_lock_token tok;
{
	splx(tok);
}

/* XXX need to check that the interface is idle */
usbd_status
usbd_set_interface(iface, aiface)
	usbd_interface_handle iface;
	int aiface;
{
	usb_device_request_t req;

	req.bmRequestType = UT_WRITE_INTERFACE;
	req.bRequest = UR_SET_INTERFACE;
	USETW(req.wValue, aiface);
	USETW(req.wIndex, iface->idesc->iInterface);
	USETW(req.wLength, 0);
	return usbd_do_request(iface->device, &req, 0);
}

/*** Internal routines ***/

/* Dequeue all pipe operations, called at splusb(). */
static usbd_status
usbd_ar_pipe(pipe)
	usbd_pipe_handle pipe;
{
	usbd_request_handle reqh;

	if (pipe->curreqh != 0)
		pipe->methods->abort(pipe->curreqh);

	for (;;) {
		reqh = SIMPLEQ_FIRST(&pipe->queue);
		if (reqh == 0)
			break;
		SIMPLEQ_REMOVE_HEAD(&pipe->queue, reqh, next);
		reqh->status = USBD_CANCELLED;
		if (reqh->callback)
			reqh->callback(reqh, reqh->priv, reqh->status);
	}
	return (USBD_NORMAL_COMPLETION);
}

/* Dequeue all interface operations, called at splusb(). */
static usbd_status
usbd_ar_iface(iface)
	usbd_interface_handle iface;
{
	usbd_pipe_handle p;
	usbd_status r, ret = USBD_NORMAL_COMPLETION;

	for (p = LIST_FIRST(&iface->pipes); p != 0; p = LIST_NEXT(p, next)) {
		r = usbd_ar_pipe(p);
		if (r != USBD_NORMAL_COMPLETION)
			ret = r;
	}
	return (ret);
}

static int usbd_global_init_done = 0;

void
usbd_init()
{
	if (!usbd_global_init_done) {
		usbd_global_init_done = 1;
		SIMPLEQ_INIT(&usbd_free_requests);
	}
}

static void
usbd_transfer_cb(reqh)
	usbd_request_handle reqh;
{
	usbd_pipe_handle pipe = reqh->pipe;
	usbd_request_handle nreqh;
	usbd_status r;

	/* XXX check retry count */
	reqh->done = 1;
	if (reqh->status == USBD_NORMAL_COMPLETION &&
	    reqh->actlen < reqh->length &&
	    !(reqh->flags & USBD_SHORT_XFER_OK)) {
		DPRINTFN(-1, ("usbd_transfer_cb: short xfer %d < %d\n",
			      reqh->actlen, reqh->length));
		reqh->status = USBD_SHORT_XFER;
	}
	pipe->curreqh = 0;
	if (reqh->callback)
		reqh->callback(reqh, reqh->priv, reqh->status);

	if (pipe->state != USBD_PIPE_ACTIVE) {
		pipe->running = 0;
		return;
	}
	nreqh = SIMPLEQ_FIRST(&pipe->queue);
	DPRINTFN(5, ("usbd_transfer_cb: nreqh=%p\n", nreqh));
	if (!nreqh)
		pipe->running = 0;
	else {
		SIMPLEQ_REMOVE_HEAD(&pipe->queue, nreqh, next);
		pipe->curreqh = reqh;
		r = pipe->methods->transfer(nreqh);
		if (r != USBD_IN_PROGRESS)
			printf("usbd_transfer_cb: error=%d\n", r);
	}
}

static void
usbd_sync_transfer_cb(reqh)
	usbd_request_handle reqh;
{
	usbd_transfer_cb(reqh);
	if (!usbd_use_polling)
		wakeup(reqh);
}

/* Like usbd_transfer(), but waits for completion. */
usbd_status
usbd_sync_transfer(reqh)
	usbd_request_handle reqh;
{
	usbd_status r;
	int s;

	reqh->xfercb = usbd_sync_transfer_cb;
	r = usbd_do_transfer(reqh);
	if (r != USBD_IN_PROGRESS)
		return (r);
	s = splusb();
	if (!reqh->done) {
		if (usbd_use_polling)
			panic("usbd_sync_transfer: not done\n");
		tsleep(reqh, PRIBIO, "usbsyn", 0);
	}
	splx(s);
	return (reqh->status);
}

usbd_status
usbd_do_request(dev, req, data)
	usbd_device_handle dev;
	usb_device_request_t *req;
	void *data;
{
	usbd_request_handle reqh;
	usbd_status r;

	reqh = usbd_alloc_request();
	if (reqh == 0)
		return (USBD_NOMEM);
	r = usbd_setup_default_request(
		reqh, dev, 0, USBD_DEFAULT_TIMEOUT, req, data, 
		UGETW(req->wLength), 0, 0);
	if (r != USBD_NORMAL_COMPLETION) {
		usbd_free_request(reqh);
		return (r);
	}
	r = usbd_sync_transfer(reqh);
#if defined(USB_DEBUG) || defined(DIAGNOSTIC)
	if (reqh->actlen > reqh->length)
		printf("usbd_do_request: overrun addr=%d type=0x%02x req=0x%02x val=%d index=%d rlen=%d length=%d actlen=%d\n",
		       dev->address, reqh->request.bmRequestType,
		       reqh->request.bRequest, UGETW(reqh->request.wValue),
		       UGETW(reqh->request.wIndex), 
		       UGETW(reqh->request.wLength), 
		       reqh->length, reqh->actlen);
#endif
	usbd_free_request(reqh);
	return (r);
}

struct usbd_quirks *
usbd_get_quirks(dev)
	usbd_device_handle dev;
{
	return (dev->quirks);
}

void
usbd_set_disco(p, hdl, data)
	usbd_pipe_handle p;
	void (*hdl) __P((void *));
	void *data;
{
	p->disco = hdl;
	p->discoarg = data;
}

/* XXX do periodic free() of free list */

