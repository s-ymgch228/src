/*	$NetBSD: pcnet_isapnp.c,v 1.2 1997/03/15 22:20:56 perry Exp $	*/

/*
 * Copyright (c) 1996
 *	Matthias Drochner.  All rights reserved.
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
 *	This product includes software developed for the NetBSD Project
 *	by Matthias Drochner.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
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
 *
 */


#include <sys/types.h>
#include <machine/pio.h>
#include <lib/libsa/stand.h>

#include <isapnpvar.h>
#include <isadmavar.h>

#include "etherdrv.h"
#include "lance.h"

#ifndef ISAPNPID
#define ISAPNPID 0x516e0010 /* TKN0010 */
#endif

char etherdev[20];

int lance_rap, lance_rdp;

u_char eth_myaddr[6];

extern void am7990_init __P((void));
extern void am7990_stop __P((void));

int EtherInit(myadr)
char *myadr;
{
  int iobase, dmachan, i;

  if(isapnp_finddev(ISAPNPID, &iobase, &dmachan)) {
    printf("cannot find PCNET\n");
    return(0);
  }

  printf("printf using PCNET @ %x\n", iobase);

  lance_rap = iobase + 0x12;
  lance_rdp = iobase + 0x10;

  /* make sure it's stopped */
  am7990_stop();

  for(i=0; i<6; i++)
    myadr[i] = eth_myaddr[i] = inb(iobase + i);

  isa_dmacascade(dmachan);

  am7990_init();

  sprintf(etherdev. "le@isapnp,0x%x", iobase);
  return(1);
}
