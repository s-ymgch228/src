/*	$NetBSD: intcreg.h,v 1.6 2002/04/28 17:10:35 uch Exp $	*/

/*-
 * Copyright (C) 1999 SAITOH Masanobu.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
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
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _SH3_INTCREG_H_
#define	_SH3_INTCREG_H_
#include <sh3/devreg.h>

/*
 * INTC
 */
/* SH3 SH7708*, SH7709* common */
#define	SH3_ICR0		0xfffffee0	/* 16bit */
#define	SH3_IPRA		0xfffffee2	/* 16bit */
#define	SH3_IPRB		0xfffffee4	/* 16bit */

/* SH7709, SH7709A only */
#define	SH7709_ICR1		0xa4000010	/* 16bit */
#define	SH7709_ICR2		0xa4000012	/* 16bit */
#define	SH7709_PINTER		0xa4000014	/* 16bit */
#define	SH7709_IPRC		0xa4000016	/* 16bit */
#define	SH7709_IPRD		0xa4000018	/* 16bit */
#define	SH7709_IPRE		0xa400001a	/* 16bit */
#define	SH7709_IRR0		0xa4000004	/* 8bit */
#define	SH7709_IRR1		0xa4000006	/* 8bit */
#define	SH7709_IRR2		0xa4000008	/* 8bit */

#define	IPRC_IRQ3_MASK		0xf000
#define	IPRC_IRQ2_MASK		0x0f00
#define	IPRC_IRQ1_MASK		0x00f0
#define	IPRC_IRQ0_MASK		0x000f

#define	IPRD_PINT07_MASK	0xf000
#define	IPRD_PINT8F_MASK	0x0f00
#define	IPRD_IRQ5_MASK		0x00f0
#define	IPRD_IRQ4_MASK		0x000f

#define	IPRE_DMAC_MASK		0xf000
#define	IPRE_IRDA_MASK		0x0f00
#define	IPRE_SCIF_MASK		0x00f0
#define	IPRE_ADC_MASK		0x000f


/* SH4 */
#define	SH4_ICR			0xffd00000	/* 16bit */
#define	SH4_IPRA		0xffd00004	/* 16bit */
#define	SH4_IPRB		0xffd00008	/* 16bit */
#define	SH4_IPRC		0xffd0000c	/* 16bit */
#define	SH4_IPRD		0xffd00010	/* 16bit */

#define	IPRC_GPIO_MASK		0xf000
#define	IPRC_DMAC_MASK		0x0f00
#define	IPRC_SCIF_MASK		0x00f0
#define	IPRC_HUDI_MASK		0x000f

#define	IPRA_TMU0_MASK		0xf000
#define	IPRA_TMU1_MASK		0x0f00
#define	IPRA_TMU2_MASK		0x00f0
#define	IPRA_RTC_MASK		0x000f

#define	IPRB_WDT_MASK		0xf000
#define	IPRB_REF_MASK		0x0f00
#define	IPRB_SCI_MASK		0x00f0

#endif /* !_SH3_INTCREG_H_ */
