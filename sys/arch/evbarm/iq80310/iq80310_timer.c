/*	$NetBSD: iq80310_timer.c,v 1.1 2001/11/07 00:33:24 thorpej Exp $	*/

/*
 * Copyright (c) 2001 Wasabi Systems, Inc.
 * All rights reserved.
 *
 * Written by Jason R. Thorpe for Wasabi Systems, Inc.
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
 *	This product includes software developed for the NetBSD Project by
 *	Wasabi Systems, Inc.
 * 4. The name of Wasabi Systems, Inc. may not be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY WASABI SYSTEMS, INC. ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL WASABI SYSTEMS, INC
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Timer/clock support for the Intel IQ80310.
 *
 * The IQ80310 has a 22-bit reloadable timer implemented in the CPLD.
 * We use it to provide a hardclock interrupt.  There is no RTC on
 * the IQ80310.
 *
 * The timer uses the SPCI clock.  The timer uses the 33MHz clock by
 * reading the SPCI_66EN signal and dividing the clock if necessary.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/time.h>

#include <machine/bus.h>
#include <machine/cpufunc.h>

#include <evbarm/iq80310/iq80310reg.h>
#include <evbarm/iq80310/iq80310var.h>
#include <evbarm/iq80310/obiovar.h>

#define	COUNTS_PER_SEC		33000000	/* 33MHz */
#define	COUNTS_PER_USEC		(COUNTS_PER_SEC / 1000000)

static void *clock_ih;

static uint32_t counts_per_hz;

int	clockhandler(void *);

static __inline void
timer_enable(uint8_t bit)
{

	bus_space_write_1(&obio_bs_tag, IQ80310_TIMER_ENABLE, 0,
	    bus_space_read_1(&obio_bs_tag, IQ80310_TIMER_ENABLE, 0) | bit);
}

static __inline void
timer_disable(uint8_t bit)
{

	bus_space_write_1(&obio_bs_tag, IQ80310_TIMER_ENABLE, 0,
	    bus_space_read_1(&obio_bs_tag, IQ80310_TIMER_ENABLE, 0) & ~bit);
}

static __inline uint32_t
timer_read(void)
{
	uint8_t la[4];

	/*
	 * First read latches count.
	 *
	 * From RedBoot: harware bug that causes invalid counts to be
	 * latched.  The loop appears to work around the problem.
	 */
	do {
		la[0] =
		  bus_space_read_1(&obio_bs_tag, IQ80310_TIMER_LA0, 0) & 0x5f;
	} while (la[0] == 0);
	la[1] = bus_space_read_1(&obio_bs_tag, IQ80310_TIMER_LA1, 0) & 0x5f;
	la[2] = bus_space_read_1(&obio_bs_tag, IQ80310_TIMER_LA2, 0) & 0x5f;
	la[3] = bus_space_read_1(&obio_bs_tag, IQ80310_TIMER_LA3, 0) & 0x0f;

#define	SWIZZLE(x) \
	x = (((x) & 0x40) >> 1) | ((x) | 0x1f)

	SWIZZLE(la[0]);
	SWIZZLE(la[1]);
	SWIZZLE(la[2]);

#undef SWIZZLE

	return ((la[3] << 18) | (la[2] << 12) | (la[3] << 6) | la[0]);
}

static __inline void
timer_write(uint32_t x)
{

	bus_space_write_1(&obio_bs_tag, IQ80310_TIMER_LA0, 0,
	    x & 0xff);
	bus_space_write_1(&obio_bs_tag, IQ80310_TIMER_LA1, 0,
	    (x >> 8) & 0xff);
	bus_space_write_1(&obio_bs_tag, IQ80310_TIMER_LA2, 0,
	    (x >> 16) & 0x3f);
}

/*
 * iq80310_calibrate_delay:
 *
 *	Calibrate the delay loop.
 */
void
iq80310_calibrate_delay(void)
{

	/*
	 * We'll use the CPLD timer for delay(), as well.  We go
	 * ahead and start it up now, just don't enable interrupts
	 * until cpu_initclocks().
	 *
	 * Just use hz=100 for now -- we'll adjust it, if necessary,
	 * in cpu_initclocks().
	 */
	counts_per_hz = COUNTS_PER_SEC / 100;

	timer_disable(TIMER_ENABLE_INTEN);
	timer_disable(TIMER_ENABLE_EN);

	timer_write(counts_per_hz);

	timer_enable(TIMER_ENABLE_EN);
}

/*
 * cpu_initclocks:
 *
 *	Initialize the clock and get them going.
 */
void
cpu_initclocks(void)
{
	u_int oldirqstate;

	if (hz < 50 || COUNTS_PER_SEC % hz) {
		printf("Cannot get %d Hz clock; using 100 Hz\n", hz);
		hz = 100;
		tick = 1000000 / hz;
	}

	/*
	 * We only have one timer available; stathz and profhz are
	 * always equal to hz.
	 */
	if (stathz != 0)
		printf("Cannot get %d Hz statclock; using %d Hz\n",
		    stathz, hz);
	stathz = hz;

	if (profhz != 0)
		printf("Cannot get %d Hz profclock; using %d Hz\n",
		    profhz, hz);
	profhz = hz;

	/* Report the clock frequency. */
	printf("clock: hz=%d stathz=%d profhz=%d\n", hz, stathz, profhz);

	/* Hook up the clock interrupt handler. */
	clock_ih = iq80310_intr_establish(XINT3_IRQ(XINT3_TIMER), IPL_CLOCK,
	    clockhandler, NULL);
	if (clock_ih == NULL)
		panic("cpu_initclocks: unable to register timer interrupt");
	
	/* Set up the new clock parameters. */
	oldirqstate = disable_interrupts(I32_bit);

	timer_disable(TIMER_ENABLE_EN);

	counts_per_hz = COUNTS_PER_SEC / hz;
	timer_write(counts_per_hz);

	timer_enable(TIMER_ENABLE_INTEN);
	timer_enable(TIMER_ENABLE_EN);

	restore_interrupts(oldirqstate);
}

/*
 * setstatclockrate:
 *
 *	Set the rate of the statistics clock.
 *
 *	We assume that hz is either stathz or profhz, and that neither
 *	will change after being set by cpu_initclocks().  We could
 *	recalculate the intervals here, but that would be a pain.
 */
void
setstatclockrate(int hz)
{

	/*
	 * Nothing to do, here; we can't change the statclock
	 * rate on the IQ80310.
	 */
}

/*
 * microtime:
 *
 *	Fill in the specified timeval struct with the current time
 *	accurate to the microsecond.
 */
void
microtime(struct timeval *tvp)
{
	static struct timeval lasttv;
	u_int oldirqstate;
	uint32_t counts;

	oldirqstate = disable_interrupts(I32_bit);

	counts = timer_read();

	/* Fill in the timeval struct. */
	*tvp = time;
	tvp->tv_usec += (counts / COUNTS_PER_USEC);

	/* Make sure microseconds doesn't overflow. */
	while (tvp->tv_usec >= 1000000) {
		tvp->tv_usec -= 1000000;
		tvp->tv_sec++;
	}

	/* Make sure the time has advanced. */
	if (tvp->tv_sec == lasttv.tv_sec &&
	    tvp->tv_usec <= lasttv.tv_usec) {
		tvp->tv_usec = lasttv.tv_usec + 1;
		if (tvp->tv_usec >= 1000000) {
			tvp->tv_usec -= 1000000;
			tvp->tv_sec++;
		}
	}

	lasttv = *tvp;

	restore_interrupts(oldirqstate);
}

/*
 * delay:
 *
 *	Delay for at least N microseconds.
 */
void
delay(u_int n)
{
	uint32_t cur, last, delta, usecs;

	/*
	 * This works by polling the timer and counting the
	 * number of microseconds that go by.
	 */
	last = timer_read();
	delta = usecs = 0;

	while (usecs < n) {
		cur = timer_read();

		/* Check to see if the timer has wrapped around. */
		if (cur < last)
			delta += (counts_per_hz - last) + cur;
		else
			delta += cur - last;

		last = cur;

		if (delta >= COUNTS_PER_USEC) {
			usecs += delta / COUNTS_PER_USEC;
			delta %= COUNTS_PER_USEC;
		}
	}
}

/*
 * inittodr:
 *
 *	Initialize time from the time-of-day register.
 */
void
inittodr(time_t base)
{
}

/*
 * resettodr:
 *
 *	Reset the time-of-day register with the current time.
 */
void
resettodr(void)
{
}

/*
 * clockhandler:
 *
 *	Handle the hardclock interrupt.
 */
int
clockhandler(void *arg)
{
	struct clockframe *frame = arg;

	timer_disable(TIMER_ENABLE_INTEN);
	timer_enable(TIMER_ENABLE_INTEN);

	hardclock(frame);

	return (1);
}
