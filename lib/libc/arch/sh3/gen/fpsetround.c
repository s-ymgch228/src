/*	$NetBSD: fpsetround.c,v 1.1 2000/01/05 14:07:32 msaitoh Exp $	*/

/*
 * Copyright (c) 1996 Mark Brinicombe
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
 *      This product includes software developed by Mark Brinicombe
 *	for the NetBSD Project.
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

#include <sys/types.h>
#include <ieeefp.h>

/*
 * Return the current FP rounding mode
 *
 * ok all we need to do is return the current FP rounding mode
 *
 * HOWEVER:
 * The ARM FPA codes the rounding mode into the actual FP instructions
 * so there is no such thing as a global rounding mode.
 * The default is round to nearest if rounding is not explictly specified.
 * FP instructions generated by GCC will not explicitly specify a rounding
 * mode.
 *
 * So the best we can do it to return the rounding mode FP instructions
 * use if rounding is not specified which is round to nearest.
 *
 * This could change in the future with new floating point emulators or
 * soft float FP libraries.
 */

#include <sys/cdefs.h>
#include <machine/ieeefp.h>

extern fp_rnd sfp_setround __P((fp_rnd));

fp_rnd
fpsetround(dir)
	fp_rnd dir;
{
#ifdef SOFTFLOAT
	fp_rnd old;

	old = fpgetround();
	sfp_setround(dir);
	return old;
#else
#error "soft float only"
#endif
}
