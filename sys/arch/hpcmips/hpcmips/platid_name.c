/* $Id: platid_name.c,v 1.10 2000/10/21 07:30:34 takemura Exp $ */

/*-
 * Copyright (c) 1999, 2000
 *         Shin Takemura and PocketBSD Project. All rights reserved.
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
 *	This product includes software developed by the PocketBSD project
 *	and its contributors.
 * 4. Neither the name of the project nor the names of its contributors
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
/*
 *  Do not edit.
 *  This file is automatically generated by platid.awk.
 */
#include <machine/platid.h>
#include <machine/platid_mask.h>
struct platid_name platid_name_table[] = {
	{ &platid_mask_CPU_MIPS,
	  "MIPS" },
	{ &platid_mask_CPU_MIPS_VR,
	  "MIPS VR" },
	{ &platid_mask_CPU_MIPS_VR_41XX,
	  "MIPS VR 41XX" },
	{ &platid_mask_CPU_MIPS_VR_4102,
	  "MIPS VR 4102" },
	{ &platid_mask_CPU_MIPS_VR_4111,
	  "MIPS VR 4111" },
	{ &platid_mask_CPU_MIPS_VR_4121,
	  "MIPS VR 4121" },
	{ &platid_mask_CPU_MIPS_TX,
	  "MIPS TX" },
	{ &platid_mask_CPU_MIPS_TX_3900,
	  "MIPS TX 3900" },
	{ &platid_mask_CPU_MIPS_TX_3911,
	  "MIPS TX 3911" },
	{ &platid_mask_CPU_MIPS_TX_3912,
	  "MIPS TX 3912" },
	{ &platid_mask_CPU_MIPS_TX_3920,
	  "MIPS TX 3920" },
	{ &platid_mask_CPU_MIPS_TX_3922,
	  "MIPS TX 3922" },
	{ &platid_mask_MACH_NEC,
	  "NEC" },
	{ &platid_mask_MACH_NEC_MCCS,
	  "NEC MC-CS" },
	{ &platid_mask_MACH_NEC_MCCS_1X,
	  "NEC MC-CS series" },
	{ &platid_mask_MACH_NEC_MCCS_11,
	  "NEC MC-CS11" },
	{ &platid_mask_MACH_NEC_MCCS_12,
	  "NEC MC-CS12" },
	{ &platid_mask_MACH_NEC_MCCS_13,
	  "NEC MC-CS13" },
	{ &platid_mask_MACH_NEC_MCR,
	  "NEC MC-R" },
	{ &platid_mask_MACH_NEC_MCR_3XX,
	  "NEC MC-R300 series" },
	{ &platid_mask_MACH_NEC_MCR_300,
	  "NEC MC-R300" },
	{ &platid_mask_MACH_NEC_MCR_320,
	  "NEC MC-R320" },
	{ &platid_mask_MACH_NEC_MCR_FORDOCOMO,
	  "NEC MobileGearII for DoCoMo" },
	{ &platid_mask_MACH_NEC_MCR_MPRO700,
	  "NEC MobilePro 700" },
	{ &platid_mask_MACH_NEC_MCR_330,
	  "NEC MC-R330" },
	{ &platid_mask_MACH_NEC_MCR_5XX,
	  "NEC MC-R500 series" },
	{ &platid_mask_MACH_NEC_MCR_500,
	  "NEC MC-R500" },
	{ &platid_mask_MACH_NEC_MCR_510,
	  "NEC MC-R510" },
	{ &platid_mask_MACH_NEC_MCR_520,
	  "NEC MC-R520" },
	{ &platid_mask_MACH_NEC_MCR_520A,
	  "NEC MobilePro 770" },
	{ &platid_mask_MACH_NEC_MCR_500A,
	  "NEC MobilePro 750c" },
	{ &platid_mask_MACH_NEC_MCR_530,
	  "NEC MC-R530" },
	{ &platid_mask_MACH_NEC_MCR_430,
	  "NEC MC-R430" },
	{ &platid_mask_MACH_NEC_MCR_530A,
	  "NEC MobilePro 780" },
	{ &platid_mask_MACH_NEC_MCR_SIGMARION,
	  "DoCoMo sigmarion" },
	{ &platid_mask_MACH_NEC_MCR_7XX,
	  "NEC MC-R700 series" },
	{ &platid_mask_MACH_NEC_MCR_700,
	  "NEC MC-R700" },
	{ &platid_mask_MACH_NEC_MCR_700A,
	  "NEC MobilePro 800" },
	{ &platid_mask_MACH_NEC_MCR_730,
	  "NEC MC-R730" },
	{ &platid_mask_MACH_NEC_MCR_730A,
	  "NEC MobilePro 880" },
	{ &platid_mask_MACH_EVEREX,
	  "Everex" },
	{ &platid_mask_MACH_EVEREX_FREESTYLE,
	  "Everex Freestyle" },
	{ &platid_mask_MACH_EVEREX_FREESTYLE_AXX,
	  "Everex Freestyle AXX" },
	{ &platid_mask_MACH_EVEREX_FREESTYLE_A10,
	  "Everex Freestyle A10" },
	{ &platid_mask_MACH_EVEREX_FREESTYLE_A15,
	  "Everex Freestyle A15" },
	{ &platid_mask_MACH_EVEREX_FREESTYLE_A20,
	  "Everex Freestyle A20" },
	{ &platid_mask_MACH_CASIO,
	  "CASIO" },
	{ &platid_mask_MACH_CASIO_CASSIOPEIAE,
	  "CASIO Cassiopeia" },
	{ &platid_mask_MACH_CASIO_CASSIOPEIAE_EXX,
	  "CASIO Cassiopeia EXX" },
	{ &platid_mask_MACH_CASIO_CASSIOPEIAE_E10,
	  "CASIO Cassiopeia E10" },
	{ &platid_mask_MACH_CASIO_CASSIOPEIAE_E11,
	  "CASIO Cassiopeia E11" },
	{ &platid_mask_MACH_CASIO_CASSIOPEIAE_E15,
	  "CASIO Cassiopeia E15" },
	{ &platid_mask_MACH_CASIO_CASSIOPEIAE_E55,
	  "CASIO Cassiopeia E-55" },
	{ &platid_mask_MACH_CASIO_CASSIOPEIAE_FORDOCOMO,
	  "CASIO Cassiopeia for DoCoMo" },
	{ &platid_mask_MACH_CASIO_CASSIOPEIAE_E65,
	  "CASIO Cassiopeia E-65" },
	{ &platid_mask_MACH_CASIO_CASSIOPEIAE_EXXX,
	  "CASIO Cassiopeia EXXX" },
	{ &platid_mask_MACH_CASIO_CASSIOPEIAE_E100,
	  "CASIO Cassiopeia E100" },
	{ &platid_mask_MACH_CASIO_CASSIOPEIAE_E105,
	  "CASIO Cassiopeia E105" },
	{ &platid_mask_MACH_CASIO_CASSIOPEIAE_E500,
	  "CASIO Cassiopeia E500" },
	{ &platid_mask_MACH_CASIO_CASSIOPEIAE_E507,
	  "CASIO Cassiopeia E507" },
	{ &platid_mask_MACH_SHARP,
	  "Sharp" },
	{ &platid_mask_MACH_SHARP_TRIPAD,
	  "Sharp Tripad" },
	{ &platid_mask_MACH_SHARP_TRIPAD_PV,
	  "Sharp Tripad PV" },
	{ &platid_mask_MACH_SHARP_TRIPAD_PV6000,
	  "Sharp Tripad PV6000" },
	{ &platid_mask_MACH_SHARP_TELIOS,
	  "Sharp Telios" },
	{ &platid_mask_MACH_SHARP_TELIOS_HC,
	  "Sharp Telios HC" },
	{ &platid_mask_MACH_SHARP_TELIOS_HCAJ1,
	  "Sharp HC-AJ1/AJ2" },
	{ &platid_mask_MACH_SHARP_TELIOS_HCVJ1C_JP,
	  "Sharp HC-VJ1C (Japanese)" },
	{ &platid_mask_MACH_SHARP_MOBILON,
	  "Sharp Mobilon" },
	{ &platid_mask_MACH_SHARP_MOBILON_HC,
	  "Sharp Mobilon HC" },
	{ &platid_mask_MACH_SHARP_MOBILON_HC4100,
	  "Sharp Mobilon HC4100" },
	{ &platid_mask_MACH_SHARP_MOBILON_HC4500,
	  "Sharp Mobilon HC4500" },
	{ &platid_mask_MACH_SHARP_MOBILON_HC1200,
	  "Sharp Mobilon HC1200" },
	{ &platid_mask_MACH_FUJITSU,
	  "Fujitsu" },
	{ &platid_mask_MACH_FUJITSU_INTERTOP,
	  "Fujitsu INTERTOP" },
	{ &platid_mask_MACH_FUJITSU_INTERTOP_ITXXX,
	  "Fujitsu INTERTOP ITXXX" },
	{ &platid_mask_MACH_FUJITSU_INTERTOP_IT300,
	  "Fujitsu INTERTOP IT300" },
	{ &platid_mask_MACH_FUJITSU_INTERTOP_IT310,
	  "Fujitsu INTERTOP IT310" },
	{ &platid_mask_MACH_PHILIPS,
	  "Philips" },
	{ &platid_mask_MACH_PHILIPS_NINO,
	  "Philips Nino" },
	{ &platid_mask_MACH_PHILIPS_NINO_3XX,
	  "Philips Nino 3XX" },
	{ &platid_mask_MACH_PHILIPS_NINO_312,
	  "Philips Nino 312" },
	{ &platid_mask_MACH_COMPAQ,
	  "Compaq" },
	{ &platid_mask_MACH_COMPAQ_C,
	  "Compaq C" },
	{ &platid_mask_MACH_COMPAQ_C_8XX,
	  "Compaq C 8XX" },
	{ &platid_mask_MACH_COMPAQ_C_810,
	  "Compaq C 810" },
	{ &platid_mask_MACH_COMPAQ_C_201X,
	  "Compaq C 201X" },
	{ &platid_mask_MACH_COMPAQ_C_2010,
	  "Compaq C 2010" },
	{ &platid_mask_MACH_COMPAQ_C_2015,
	  "Compaq C 2015" },
	{ &platid_mask_MACH_COMPAQ_AERO,
	  "Compaq AERO" },
	{ &platid_mask_MACH_COMPAQ_AERO_15XX,
	  "Compaq AERO 15XX" },
	{ &platid_mask_MACH_COMPAQ_AERO_1530,
	  "Compaq AERO 1530" },
	{ &platid_mask_MACH_COMPAQ_AERO_21XX,
	  "Compaq AERO 21XX" },
	{ &platid_mask_MACH_COMPAQ_AERO_2110,
	  "Compaq AERO 2110" },
	{ &platid_mask_MACH_COMPAQ_AERO_2130,
	  "Compaq AERO 2130" },
	{ &platid_mask_MACH_COMPAQ_AERO_2140,
	  "Compaq AERO 2140" },
	{ &platid_mask_MACH_COMPAQ_PRESARIO,
	  "Compaq PRESARIO" },
	{ &platid_mask_MACH_COMPAQ_PRESARIO_21X,
	  "Compaq PRESARIO 21X" },
	{ &platid_mask_MACH_COMPAQ_PRESARIO_213,
	  "Compaq PRESARIO 213" },
	{ &platid_mask_MACH_VICTOR,
	  "Victor" },
	{ &platid_mask_MACH_VICTOR_INTERLINK,
	  "Victor InterLink" },
	{ &platid_mask_MACH_VICTOR_INTERLINK_MP,
	  "Victor InterLink MP" },
	{ &platid_mask_MACH_VICTOR_INTERLINK_MPC101,
	  "Victor InterLink MPC101" },
	{ &platid_mask_MACH_IBM,
	  "IBM" },
	{ &platid_mask_MACH_IBM_WORKPAD,
	  "IBM WorkPad" },
	{ &platid_mask_MACH_IBM_WORKPAD_Z50,
	  "IBM WorkPad z50" },
	{ &platid_mask_MACH_IBM_WORKPAD_26011AU,
	  "IBM WorkPad z50 2601 1AU" },
	{ &platid_mask_MACH_VADEM,
	  "VADEM" },
	{ &platid_mask_MACH_VADEM_CLIO,
	  "VADEM CLIO" },
	{ &platid_mask_MACH_VADEM_CLIO_C,
	  "VADEM CLIO C" },
	{ &platid_mask_MACH_VADEM_CLIO_C1000,
	  "VADEM CLIO C-1000" },
	{ &platid_mask_MACH_VADEM_CLIO_C1050,
	  "VADEM CLIO C-1050" },
};
int platid_name_table_size = 112;
