/*	$NetBSD: tutor.h,v 1.5 1998/09/13 15:27:26 hubertf Exp $	*/

/*
 * Copyright (c) 1980, 1993
 *	The Regents of the University of California.  All rights reserved.
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
 *	@(#)tutor.h	8.1 (Berkeley) 5/31/93
 */

struct situatn {
	int	brd[26];
	int	roll1;
	int	roll2;
	int	mp[4];
	int	mg[4];
	int	new1;
	int	new2;
	char	*(*com[8]);
};


extern	char	*doubl[];
extern	char	*endgame[];
extern	char	*finis[];
extern	char	*hello[];
extern	char	*hits[];
extern	char	*intro1[];
extern	char	*intro2[];
extern	char	*lastch[];
extern	char	*list[];
extern	int	 maxmoves;
extern	char	*moves[];
extern	char	*opts;
extern	char	*prog[];
extern	char	*prompt;
extern	char	*removepiece[];
extern	char	*stragy[];
extern	struct situatn	test[];


int	brdeq __P((int *, int *));
void	clrest __P((void));
void	leave __P((void)) __attribute__((__noreturn__));
void	tutor __P((void)) __attribute__((__noreturn__));
