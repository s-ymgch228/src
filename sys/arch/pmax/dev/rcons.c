/*	$NetBSD: rcons.c,v 1.3 1995/09/11 07:45:48 jonathan Exp $	*/

/*
 * Copyright (c) 1995
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Ted Lemon.
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
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
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
 *
 */

#include <rcons.h>
#if NRCONS > 0

#include <sys/param.h>
#include <sys/proc.h>
#include <sys/systm.h>
#include <sys/buf.h>
#include <sys/ioctl.h>
#include <sys/tty.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/conf.h>
#include <sys/vnode.h>

#include <pmax/stand/dec_prom.h>

#include <pmax/dev/sccreg.h>
#include <pmax/pmax/kn01.h>
#include <pmax/pmax/kn02.h>
#include <pmax/pmax/kmin.h>
#include <pmax/pmax/maxine.h>
#include <pmax/pmax/kn03.h>
#include <pmax/pmax/asic.h>
#include <pmax/pmax/turbochannel.h>
#include <pmax/pmax/pmaxtype.h>
#include <dev/cons.h>

#include <sys/device.h>
#include <machine/fbio.h>
#include <sparc/rcons/raster.h>
#include <machine/fbvar.h>

#include <machine/machConst.h>
#include <machine/pmioctl.h>
#include <pmax/dev/fbreg.h>


/*
 * Console I/O is redirected to the appropriate device, either a screen and
 * keyboard, a serial port, or the "virtual" console.
 */

extern struct tty *constty;	/* virtual console output device */
struct tty *fbconstty;		/* Frame buffer console tty... */
struct tty rcons_tty [NRCONS];	/* Console tty struct... */
extern struct consdev *cn_tab;	/* Console I/O table... */

struct	vnode *cn_in_devvp;	/* vnode for underlying input device. */
dev_t	cn_in_dev = NODEV;	/* console input device. */

char rcons_maxcols [20];

/* rcons_connect is called by fbconnect when the first frame buffer is
   attached.   That frame buffer will always be the console frame buffer. */

rcons_connect (info)
	struct fbinfo *info;
{
	static struct fbdevice fb;
	static int row, col;

	/* If we're running a serial console, don't set up a raster console
	   even if there's a device that can support it. */
	if (cn_tab -> cn_pri == CN_REMOTE)
		return;

	fbconstty = &rcons_tty [0];
	fbconstty->t_dev = makedev(85, 0);	/* /dev/console */
	fbconstty->t_ispeed = fbconstty->t_ospeed = TTYDEF_SPEED;
	fbconstty->t_param = (int (*)(struct tty *, struct termios *))nullop;

	/* Connect the console geometry... */
	fb.fb_devinfo = info;

	/* Initialize the state information. */
	fb.fb_bits = 0;
	fb.fb_ringing = 0;
	fb.fb_belldepth = 0;
	fb.fb_scroll = 0;
	fb.fb_p0 = 0;
	fb.fb_p1 = 0;

	fb.fb_row = &row;
	fb.fb_col = &col;

#define HW_FONT_WIDTH 8
#define HW_FONT_HEIGHT 15
	sprintf (rcons_maxcols, "%d", fb.fb_type.fb_height / HW_FONT_HEIGHT);
	row = (fb.fb_type.fb_height / HW_FONT_HEIGHT) - 1;
	col = 0;
	rcons_init (&fb);

	fb.fb_xorigin = 0;
	fb.fb_yorigin = 0;
}

/* ARGSUSED */
rconsattach (n)
	int n;
{
	register struct tty *tp = &rcons_tty [0];
	int status;

	/* Set up the tty queues now... */
	clalloc(&tp->t_rawq, 1024, 1);
	clalloc(&tp->t_canq, 1024, 1);
	/* output queue doesn't need quoting */
	clalloc(&tp->t_outq, 1024, 0);
#ifdef DEBUG
	printf("rconsattach: %d raster consoles\n", n);
#endif

	/* Try to set up the input device... */
	if (cn_in_dev != NODEV && cn_in_devvp == NULLVP) {
		/* try to get a reference on its vnode, but fail silently */
		cdevvp(cn_in_dev, &cn_in_devvp);
		status = ((*cdevsw[major(cn_in_dev)].d_open)
			  (cn_in_dev, O_NONBLOCK, S_IFCHR, curproc)); /* XXX */
		if (status)
			printf ("rconsattach: input device open failed: %d\n",
			        status);
	}
	/* Now the input side has been opened cleanly, we can dispense
	 * with any special-case console input hacks, and point the
	 * console device at rcons.
	 */
/* FIXME */		cn_tab->cn_dev = makedev (/*RCONSDEV*/85, 0);
	
}

/* ARGSUSED */
rconsopen(dev, flag, mode, p)
	dev_t dev;
	int flag, mode;
	struct proc *p;
{
	register struct tty *tp = &rcons_tty [0];
 	static int firstopen = 1;
	int status;

	if ((tp->t_state & TS_ISOPEN) == 0) {
		/*
		 * Leave baud rate alone!
		 */
		ttychars(tp);
		tp->t_iflag = TTYDEF_IFLAG;
		tp->t_oflag = TTYDEF_OFLAG;
		tp->t_lflag = TTYDEF_LFLAG;
		tp->t_cflag = TTYDEF_CFLAG;
		tp->t_state = TS_ISOPEN | TS_CARR_ON;
		(void)(*tp->t_param)(tp, &tp->t_termios);
		ttsetwater(tp);
	} else if (tp->t_state & TS_XCLUDE && p->p_ucred->cr_uid != 0)
		return (EBUSY);

	status = (*linesw[tp->t_line].l_open)(dev, tp);
	return status;
}

/* ARGSUSED */
rconsclose(dev, flag, mode, p)
	dev_t dev;
	int flag, mode;
	struct proc *p;
{
	register struct tty *tp = &rcons_tty [0];
	struct vnode *vp;

	(*linesw[tp->t_line].l_close)(tp, flag);
	ttyclose(tp);

	return (0);
}

/* ARGSUSED */
rconsread(dev, uio, flag)
	dev_t dev;
	struct uio *uio;
	int flag;
{
	register struct tty *tp = &rcons_tty [0];

	return ((*linesw[tp->t_line].l_read)(tp, uio, flag));
}

/* ARGSUSED */
rconswrite(dev, uio, flag)
	dev_t dev;
	struct uio *uio;
	int flag;
{
	register struct tty *tp;

	tp = &rcons_tty [0];
	return ((*linesw[tp->t_line].l_write)(tp, uio, flag));
}

struct tty *
rconstty(dev)
        dev_t dev;
{
        register struct tty *tp = &rcons_tty [0];
        return (tp);
}

rconsioctl(dev, cmd, data, flag, p)
	dev_t dev;
	u_long cmd;
	caddr_t data;
	int flag;
	struct proc *p;
{
	register struct tty *tp;
	int error;

	tp = &rcons_tty [0];
	if ((error = linesw[tp->t_line].l_ioctl(tp, cmd, data, flag, p)) >= 0)
		return (error);
	if ((error = ttioctl(tp, cmd, data, flag, p)) >= 0)
		return (error);
	return (ENOTTY);
}

/* ARGSUSED */
rconsstop (tp, rw)
	struct tty *tp;
	int rw;
{
}

/*ARGSUSED*/
rconsreset (tp, rw)
	struct tty *tp;
	int rw;
{
}

/*ARGSUSED*/
rconsselect(dev, which, p)
	dev_t dev;
	int which;
	struct proc *p;
{
	return (ttselect(dev, which, p));
}

/*ARGSUSED*/
rconsmmap ()
{
	return 0;
}

rconsstrategy(bp)
	struct buf *bp;
{
}

/* Called by real input device when there is input for rcons.   Passes
   input through line discpline interrupt routine... */

rcons_input (dev, ic)
	dev_t dev;
	int ic;
{
	struct tty *tp;
	int unit = minor (dev);
	if (unit > NRCONS)
		return;
	tp = &rcons_tty [unit];
	if (!(tp -> t_state & TS_ISOPEN)) {
		return;
	}
	(*linesw [tp -> t_line].l_rint)(ic, tp);
}
#endif /* NRCONS > 0 */
