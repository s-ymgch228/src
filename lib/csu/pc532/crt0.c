/*-
 * Copyright (c) 1990 The Regents of the University of California.
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
 */

#if defined(LIBC_SCCS) && !defined(lint)
/*static char sccsid[] = "@(#)crt0.c	5.7 (Berkeley) 7/3/91"; */
static char rcsid[] = "";
#endif /* LIBC_SCCS and not lint */


/*
 *	$Id: crt0.c,v 1.1.1.1 1993/09/17 18:42:11 phil Exp $
 *
 *	C start up routine.
 *	Robert Henry, UCB, 20 Oct 81
 *
 *	Modified by Phil Nelson for the PC532, July 18, 1993.
 *
 *	We make the following (true) assumption:
 *	1) We can trust fp and sp.  Both "start" pointing to the
 *	first entry in the argc/argv/envp structure...  After
 *	the "enter [???],?", fp points to the word just before
 *	the argc/argv/envp structure!
 *		
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

char	**environ = (char **)0;
static char empty[1];
char	*__progname = empty;   
int	errno = 0;

/* To make sure "start" is past the a.out header.  This should go away soon. */
asm(".text");
asm(".long 0,0,0,0,0,0,0,0");

extern	unsigned char	etext;
extern	unsigned char	eprol asm ("eprol");
extern			start() asm("start");
extern			mcount() asm ("mcount");

start(long param)
{
	struct kframe {
		int	kargc;
		char	*kargv[1];	/* size depends on kargc */
		char	kargstr[1];	/* size varies */
		char	kenvstr[1];	/* size varies */
	};
	/*
	 *	ALL REGISTER VARIABLES!!!
	 */
	register struct kframe *kfp;
	register char **targv;
	register char **argv;
	extern void _mcleanup();

	kfp = (struct kframe *) (&param-1);
	for (argv = targv = &kfp->kargv[0]; *targv++; /* void */)
		/* void */ ;
	if (targv >= (char **)(*argv))
		--targv;
	environ = targv;
asm("eprol:");

#ifdef MCRT0
	atexit(_mcleanup);
	monstartup(&eprol, &etext);
#endif MCRT0
        errno = 0;
        if (argv[0])
                if ((__progname = strrchr(argv[0], '/')) == NULL)
                        __progname = argv[0];
                else
                        ++__progname;
	exit(main(kfp->kargc, argv, environ));
}

#ifdef CRT0
/*
 * null mcount and moncontrol,
 * just in case some routine is compiled for profiling
 */
moncontrol(val)
	int val;
{
}

mcount() { }
#endif CRT0
