/*	$NetBSD: pow.c,v 1.1.1.1 1996/05/05 12:17:03 oki Exp $	*/

/*
 * Copyright (c) 1995 MINOURA Makoto.
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Minoura Makoto.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Power switch device driver.
 * Useful for
 *  1. accessing boot information.
 *  2. looking at the front or external power switch.
 */

#include <string.h>
#include <sys/param.h>
#include <sys/proc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/kernel.h>
#include <sys/fcntl.h>

#include <machine/powioctl.h>
#include <x68k/dev/powvar.h>
#include <x68k/x68k/iodevice.h>
#include "pow.h"

#define sramtop (IODEVbase->io_sram)
#define rtc (IODEVbase->io_rtc)

static int powinit ();

struct pow_softc pows[NPOW];

/* ARGSUSED */
powattach(num)
	int num;
{
	int minor;
	int sw;

	sw = ~mfp.gpip & 7;

	mfp.ierb &= ~7;		/* disable mfp power switch interrupt */
	mfp.aer &= ~7;

	for (minor = 0; minor < num; minor++) {
		if (minor == 0) {
			pows[minor].status = POW_FREE;
		} else {
			pows[minor].status = POW_ANY;
		}
		pows[minor].sw = sw;

		if (sw) {
			mfp.aer |= sw;
			mfp.ierb |= sw;
		}

		printf ("pow%d: started by ", minor);
		if ((sw & POW_ALARMSW) && sramtop[0x26] == 0)
			printf ("RTC alarm.\n");
		else if (sw & POW_EXTERNALSW)
			printf ("external power switch.\n");
		else if (sw & POW_FRONTSW)
			printf ("front power switch.\n");
		else
			printf ("???.\n");
	}

	return 1;
}

/*ARGSUSED*/
int
powopen(dev, flags)
	dev_t dev;
	int flags;
{
	struct pow_softc *sc = &pows[minor(dev)];

	if (minor(dev) >= NPOW)
		return EXDEV;

	if (sc->status == POW_BUSY)
		return EBUSY;

	sc->pid = 0;
	if (sc->status == POW_FREE)
		sc->status = POW_BUSY;

	sc->rw = (flags & (FREAD|FWRITE));

	return 0;
}

/*ARGSUSED*/
void
powclose (dev, flags)
	dev_t dev;
	int flags;
{
	struct pow_softc *sc = &pows[minor(dev)];

	if (sc->status == POW_BUSY)
		sc->status = POW_FREE;
	sc->pid = 0;
}

#define SRAMINT(offset)	(*((int*) (&sramtop[offset])))
#define RTCWAIT DELAY(100)

static int
setalarm (bp)
	struct x68k_alarminfo *bp;
{
	int s = splclock ();
	int ontime;

	sysport.sramwp = 0x31;
	if (bp->al_enable) {
		SRAMINT (0x1e) = bp->al_dowhat;
		SRAMINT (0x22) = bp->al_ontime;
		SRAMINT (0x14) = (bp->al_offtime / 60) - 1;
		sramtop[0x26] = 0;
	} else {
		sramtop[0x26] = 7;
	}
	sysport.sramwp = 0;

	rtc.bank0.mode = 9;
	RTCWAIT;
	rtc.bank1.reset = 5;
	RTCWAIT;

	if (bp->al_enable) {
		ontime = bp->al_ontime;
		if ((ontime & 0x0f) <= 9)
			rtc.bank1.al_min = ontime & 0x0f;
		RTCWAIT;
		ontime >>= 4;
		if ((ontime & 0x0f) <= 6)
			rtc.bank1.al_min10 = ontime & 0x0f;
		RTCWAIT;
		ontime >>= 4;
		if ((ontime & 0x0f) <= 9)
			rtc.bank1.al_hour = ontime & 0x0f;
		RTCWAIT;
		ontime >>= 4;
		if ((ontime & 0x0f) <= 2)
			rtc.bank1.al_hour10 = ontime & 0x0f;
		RTCWAIT;
		ontime >>= 4;
		if ((ontime & 0x0f) <= 9)
			rtc.bank1.al_day = ontime & 0x0f;
		RTCWAIT;
		ontime >>= 4;
		if ((ontime & 0x0f) <= 3)
			rtc.bank1.al_day10 = ontime & 0x0f;
		RTCWAIT;
		ontime >>= 4;
		if ((ontime & 0x0f) <= 6)
			rtc.bank1.al_week = ontime & 0x0f;
		RTCWAIT;
		rtc.bank1.clkout = 0;
		RTCWAIT;
		rtc.bank1.mode = 0x0c;
	} else {
		rtc.bank1.clkout = 7;
		RTCWAIT;
		rtc.bank1.mode = 0x08;
	}

	splx (s);

	return 0;
}


/*ARGSUSED*/
int
powioctl (dev, cmd, addr, flag, p)
	dev_t dev;
	u_long cmd;
	caddr_t addr;
	int flag;
	struct proc *p;
{
	struct pow_softc *sc = &pows[minor(dev)];

	switch (cmd) {
	case POWIOCGPOWERINFO:
		{
			struct x68k_powerinfo *bp = (struct x68k_powerinfo*) addr;
			if (!(sc->rw & FREAD))
				return EBADF;
			bp->pow_switch_boottime = sc->sw;
			bp->pow_switch_current = ~mfp.gpip & 7;
			bp->pow_boottime = boottime.tv_sec;
			bp->pow_bootcount = SRAMINT (0x44);
			bp->pow_usedtotal = SRAMINT (0x40) * 60;
		}
		break;

	case POWIOCGALARMINFO:
		{
			struct x68k_alarminfo *bp = (struct x68k_alarminfo*) addr;
			if (!(sc->rw & FREAD))
				return EBADF;
			bp->al_enable = (sramtop[0x26] == 0);
			bp->al_ontime = SRAMINT (0x22);
			bp->al_dowhat = SRAMINT (0x1e);
			bp->al_offtime = (SRAMINT (0x14) + 1) * 60;
		}
		break;

	case POWIOCSALARMINFO:
		if (!(sc->rw & FWRITE))
			return EBADF;
		return setalarm ((struct x68k_alarminfo*) addr);

	case POWIOCSSIGNAL:
		if (minor(dev) != 0)
			return EOPNOTSUPP;
		if (!(sc->rw & FWRITE))
			return EBADF;
		{
			int signum = *(int*) addr;
			if (signum <= 0 || signum > 31)
				return EINVAL;

			sc->signum = signum;
			sc->proc = p;
			sc->pid = p->p_pid;
		}

		break;
	default:
		return EINVAL;
	}

	return 0;
}

void
powintr ()
{
	int sw;
	int s;

	s = spl6 ();

	sw = ~mfp.gpip & 6;
	mfp.aer &= ~sw;
	mfp.ierb |= sw;

	if (pows[0].status == POW_BUSY && pows[0].pid != 0)
		psignal (pows[0].proc, pows[0].signum);

	splx (s);
}
