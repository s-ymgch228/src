/* $Id: platid_generated.h,v 1.16 2000/02/06 14:02:44 takemura Exp $ */

/*-
 * Copyright (c) 1999
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
#define PLATID_CPU_MIPS_NUM	1
#define PLATID_CPU_MIPS	\
  ((PLATID_CPU_MIPS_NUM << PLATID_CPU_ARCH_SHIFT))
#define PLATID_CPU_MIPS_VR_NUM	1
#define PLATID_CPU_MIPS_VR	\
  ((PLATID_CPU_MIPS_VR_NUM << PLATID_CPU_SERIES_SHIFT)| \
    PLATID_CPU_MIPS)
#define PLATID_CPU_MIPS_VR_41XX_NUM	1
#define PLATID_CPU_MIPS_VR_41XX	\
  ((PLATID_CPU_MIPS_VR_41XX_NUM << PLATID_CPU_MODEL_SHIFT)| \
    PLATID_CPU_MIPS_VR)
#define PLATID_CPU_MIPS_VR_4102_NUM	1
#define PLATID_CPU_MIPS_VR_4102	\
  ((PLATID_CPU_MIPS_VR_4102_NUM << PLATID_CPU_SUBMODEL_SHIFT)| \
    PLATID_CPU_MIPS_VR_41XX)
#define PLATID_CPU_MIPS_VR_4111_NUM	2
#define PLATID_CPU_MIPS_VR_4111	\
  ((PLATID_CPU_MIPS_VR_4111_NUM << PLATID_CPU_SUBMODEL_SHIFT)| \
    PLATID_CPU_MIPS_VR_41XX)
#define PLATID_CPU_MIPS_VR_4121_NUM	3
#define PLATID_CPU_MIPS_VR_4121	\
  ((PLATID_CPU_MIPS_VR_4121_NUM << PLATID_CPU_SUBMODEL_SHIFT)| \
    PLATID_CPU_MIPS_VR_41XX)
#define PLATID_CPU_MIPS_TX_NUM	2
#define PLATID_CPU_MIPS_TX	\
  ((PLATID_CPU_MIPS_TX_NUM << PLATID_CPU_SERIES_SHIFT)| \
    PLATID_CPU_MIPS)
#define PLATID_CPU_MIPS_TX_3900_NUM	1
#define PLATID_CPU_MIPS_TX_3900	\
  ((PLATID_CPU_MIPS_TX_3900_NUM << PLATID_CPU_MODEL_SHIFT)| \
    PLATID_CPU_MIPS_TX)
#define PLATID_CPU_MIPS_TX_3911_NUM	1
#define PLATID_CPU_MIPS_TX_3911	\
  ((PLATID_CPU_MIPS_TX_3911_NUM << PLATID_CPU_SUBMODEL_SHIFT)| \
    PLATID_CPU_MIPS_TX_3900)
#define PLATID_CPU_MIPS_TX_3912_NUM	2
#define PLATID_CPU_MIPS_TX_3912	\
  ((PLATID_CPU_MIPS_TX_3912_NUM << PLATID_CPU_SUBMODEL_SHIFT)| \
    PLATID_CPU_MIPS_TX_3900)
#define PLATID_CPU_MIPS_TX_3920_NUM	2
#define PLATID_CPU_MIPS_TX_3920	\
  ((PLATID_CPU_MIPS_TX_3920_NUM << PLATID_CPU_MODEL_SHIFT)| \
    PLATID_CPU_MIPS_TX)
#define PLATID_CPU_MIPS_TX_3922_NUM	1
#define PLATID_CPU_MIPS_TX_3922	\
  ((PLATID_CPU_MIPS_TX_3922_NUM << PLATID_CPU_SUBMODEL_SHIFT)| \
    PLATID_CPU_MIPS_TX_3920)
#define PLATID_MACH_NEC_NUM	1
#define PLATID_MACH_NEC	\
  ((PLATID_MACH_NEC_NUM << PLATID_VENDOR_SHIFT))
#define PLATID_MACH_NEC_MCCS_NUM	1
#define PLATID_MACH_NEC_MCCS	\
  ((PLATID_MACH_NEC_MCCS_NUM << PLATID_SERIES_SHIFT)| \
    PLATID_MACH_NEC)
#define PLATID_MACH_NEC_MCCS_1X_NUM	1
#define PLATID_MACH_NEC_MCCS_1X	\
  ((PLATID_MACH_NEC_MCCS_1X_NUM << PLATID_MODEL_SHIFT)| \
    PLATID_MACH_NEC_MCCS)
#define PLATID_MACH_NEC_MCCS_11_NUM	1
#define PLATID_MACH_NEC_MCCS_11	\
  ((PLATID_MACH_NEC_MCCS_11_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_NEC_MCCS_1X)
#define PLATID_MACH_NEC_MCCS_12_NUM	2
#define PLATID_MACH_NEC_MCCS_12	\
  ((PLATID_MACH_NEC_MCCS_12_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_NEC_MCCS_1X)
#define PLATID_MACH_NEC_MCCS_13_NUM	3
#define PLATID_MACH_NEC_MCCS_13	\
  ((PLATID_MACH_NEC_MCCS_13_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_NEC_MCCS_1X)
#define PLATID_MACH_NEC_MCR_NUM	2
#define PLATID_MACH_NEC_MCR	\
  ((PLATID_MACH_NEC_MCR_NUM << PLATID_SERIES_SHIFT)| \
    PLATID_MACH_NEC)
#define PLATID_MACH_NEC_MCR_3XX_NUM	1
#define PLATID_MACH_NEC_MCR_3XX	\
  ((PLATID_MACH_NEC_MCR_3XX_NUM << PLATID_MODEL_SHIFT)| \
    PLATID_MACH_NEC_MCR)
#define PLATID_MACH_NEC_MCR_300_NUM	1
#define PLATID_MACH_NEC_MCR_300	\
  ((PLATID_MACH_NEC_MCR_300_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_NEC_MCR_3XX)
#define PLATID_MACH_NEC_MCR_320_NUM	2
#define PLATID_MACH_NEC_MCR_320	\
  ((PLATID_MACH_NEC_MCR_320_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_NEC_MCR_3XX)
#define PLATID_MACH_NEC_MCR_FORDOCOMO_NUM	3
#define PLATID_MACH_NEC_MCR_FORDOCOMO	\
  ((PLATID_MACH_NEC_MCR_FORDOCOMO_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_NEC_MCR_3XX)
#define PLATID_MACH_NEC_MCR_5XX_NUM	2
#define PLATID_MACH_NEC_MCR_5XX	\
  ((PLATID_MACH_NEC_MCR_5XX_NUM << PLATID_MODEL_SHIFT)| \
    PLATID_MACH_NEC_MCR)
#define PLATID_MACH_NEC_MCR_500_NUM	1
#define PLATID_MACH_NEC_MCR_500	\
  ((PLATID_MACH_NEC_MCR_500_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_NEC_MCR_5XX)
#define PLATID_MACH_NEC_MCR_510_NUM	2
#define PLATID_MACH_NEC_MCR_510	\
  ((PLATID_MACH_NEC_MCR_510_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_NEC_MCR_5XX)
#define PLATID_MACH_NEC_MCR_520_NUM	3
#define PLATID_MACH_NEC_MCR_520	\
  ((PLATID_MACH_NEC_MCR_520_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_NEC_MCR_5XX)
#define PLATID_MACH_NEC_MCR_520A_NUM	4
#define PLATID_MACH_NEC_MCR_520A	\
  ((PLATID_MACH_NEC_MCR_520A_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_NEC_MCR_5XX)
#define PLATID_MACH_NEC_MCR_500A_NUM	5
#define PLATID_MACH_NEC_MCR_500A	\
  ((PLATID_MACH_NEC_MCR_500A_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_NEC_MCR_5XX)
#define PLATID_MACH_NEC_MCR_530_NUM	6
#define PLATID_MACH_NEC_MCR_530	\
  ((PLATID_MACH_NEC_MCR_530_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_NEC_MCR_5XX)
#define PLATID_MACH_NEC_MCR_430_NUM	7
#define PLATID_MACH_NEC_MCR_430	\
  ((PLATID_MACH_NEC_MCR_430_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_NEC_MCR_5XX)
#define PLATID_MACH_NEC_MCR_7XX_NUM	3
#define PLATID_MACH_NEC_MCR_7XX	\
  ((PLATID_MACH_NEC_MCR_7XX_NUM << PLATID_MODEL_SHIFT)| \
    PLATID_MACH_NEC_MCR)
#define PLATID_MACH_NEC_MCR_700_NUM	1
#define PLATID_MACH_NEC_MCR_700	\
  ((PLATID_MACH_NEC_MCR_700_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_NEC_MCR_7XX)
#define PLATID_MACH_NEC_MCR_700A_NUM	2
#define PLATID_MACH_NEC_MCR_700A	\
  ((PLATID_MACH_NEC_MCR_700A_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_NEC_MCR_7XX)
#define PLATID_MACH_NEC_MCR_730_NUM	3
#define PLATID_MACH_NEC_MCR_730	\
  ((PLATID_MACH_NEC_MCR_730_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_NEC_MCR_7XX)
#define PLATID_MACH_EVEREX_NUM	2
#define PLATID_MACH_EVEREX	\
  ((PLATID_MACH_EVEREX_NUM << PLATID_VENDOR_SHIFT))
#define PLATID_MACH_EVEREX_FREESTYLE_NUM	1
#define PLATID_MACH_EVEREX_FREESTYLE	\
  ((PLATID_MACH_EVEREX_FREESTYLE_NUM << PLATID_SERIES_SHIFT)| \
    PLATID_MACH_EVEREX)
#define PLATID_MACH_EVEREX_FREESTYLE_AXX_NUM	1
#define PLATID_MACH_EVEREX_FREESTYLE_AXX	\
  ((PLATID_MACH_EVEREX_FREESTYLE_AXX_NUM << PLATID_MODEL_SHIFT)| \
    PLATID_MACH_EVEREX_FREESTYLE)
#define PLATID_MACH_EVEREX_FREESTYLE_A10_NUM	1
#define PLATID_MACH_EVEREX_FREESTYLE_A10	\
  ((PLATID_MACH_EVEREX_FREESTYLE_A10_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_EVEREX_FREESTYLE_AXX)
#define PLATID_MACH_EVEREX_FREESTYLE_A15_NUM	2
#define PLATID_MACH_EVEREX_FREESTYLE_A15	\
  ((PLATID_MACH_EVEREX_FREESTYLE_A15_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_EVEREX_FREESTYLE_AXX)
#define PLATID_MACH_EVEREX_FREESTYLE_A20_NUM	3
#define PLATID_MACH_EVEREX_FREESTYLE_A20	\
  ((PLATID_MACH_EVEREX_FREESTYLE_A20_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_EVEREX_FREESTYLE_AXX)
#define PLATID_MACH_CASIO_NUM	3
#define PLATID_MACH_CASIO	\
  ((PLATID_MACH_CASIO_NUM << PLATID_VENDOR_SHIFT))
#define PLATID_MACH_CASIO_CASSIOPEIAE_NUM	1
#define PLATID_MACH_CASIO_CASSIOPEIAE	\
  ((PLATID_MACH_CASIO_CASSIOPEIAE_NUM << PLATID_SERIES_SHIFT)| \
    PLATID_MACH_CASIO)
#define PLATID_MACH_CASIO_CASSIOPEIAE_EXX_NUM	1
#define PLATID_MACH_CASIO_CASSIOPEIAE_EXX	\
  ((PLATID_MACH_CASIO_CASSIOPEIAE_EXX_NUM << PLATID_MODEL_SHIFT)| \
    PLATID_MACH_CASIO_CASSIOPEIAE)
#define PLATID_MACH_CASIO_CASSIOPEIAE_E10_NUM	1
#define PLATID_MACH_CASIO_CASSIOPEIAE_E10	\
  ((PLATID_MACH_CASIO_CASSIOPEIAE_E10_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_CASIO_CASSIOPEIAE_EXX)
#define PLATID_MACH_CASIO_CASSIOPEIAE_E11_NUM	2
#define PLATID_MACH_CASIO_CASSIOPEIAE_E11	\
  ((PLATID_MACH_CASIO_CASSIOPEIAE_E11_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_CASIO_CASSIOPEIAE_EXX)
#define PLATID_MACH_CASIO_CASSIOPEIAE_E15_NUM	3
#define PLATID_MACH_CASIO_CASSIOPEIAE_E15	\
  ((PLATID_MACH_CASIO_CASSIOPEIAE_E15_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_CASIO_CASSIOPEIAE_EXX)
#define PLATID_MACH_CASIO_CASSIOPEIAE_E55_NUM	4
#define PLATID_MACH_CASIO_CASSIOPEIAE_E55	\
  ((PLATID_MACH_CASIO_CASSIOPEIAE_E55_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_CASIO_CASSIOPEIAE_EXX)
#define PLATID_MACH_CASIO_CASSIOPEIAE_EXXX_NUM	2
#define PLATID_MACH_CASIO_CASSIOPEIAE_EXXX	\
  ((PLATID_MACH_CASIO_CASSIOPEIAE_EXXX_NUM << PLATID_MODEL_SHIFT)| \
    PLATID_MACH_CASIO_CASSIOPEIAE)
#define PLATID_MACH_CASIO_CASSIOPEIAE_E100_NUM	1
#define PLATID_MACH_CASIO_CASSIOPEIAE_E100	\
  ((PLATID_MACH_CASIO_CASSIOPEIAE_E100_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_CASIO_CASSIOPEIAE_EXXX)
#define PLATID_MACH_CASIO_CASSIOPEIAE_E105_NUM	2
#define PLATID_MACH_CASIO_CASSIOPEIAE_E105	\
  ((PLATID_MACH_CASIO_CASSIOPEIAE_E105_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_CASIO_CASSIOPEIAE_EXXX)
#define PLATID_MACH_CASIO_CASSIOPEIAE_E500_NUM	3
#define PLATID_MACH_CASIO_CASSIOPEIAE_E500	\
  ((PLATID_MACH_CASIO_CASSIOPEIAE_E500_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_CASIO_CASSIOPEIAE_EXXX)
#define PLATID_MACH_CASIO_CASSIOPEIAE_E507_NUM	4
#define PLATID_MACH_CASIO_CASSIOPEIAE_E507	\
  ((PLATID_MACH_CASIO_CASSIOPEIAE_E507_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_CASIO_CASSIOPEIAE_EXXX)
#define PLATID_MACH_SHARP_NUM	4
#define PLATID_MACH_SHARP	\
  ((PLATID_MACH_SHARP_NUM << PLATID_VENDOR_SHIFT))
#define PLATID_MACH_SHARP_TRIPAD_NUM	1
#define PLATID_MACH_SHARP_TRIPAD	\
  ((PLATID_MACH_SHARP_TRIPAD_NUM << PLATID_SERIES_SHIFT)| \
    PLATID_MACH_SHARP)
#define PLATID_MACH_SHARP_TRIPAD_PV_NUM	1
#define PLATID_MACH_SHARP_TRIPAD_PV	\
  ((PLATID_MACH_SHARP_TRIPAD_PV_NUM << PLATID_MODEL_SHIFT)| \
    PLATID_MACH_SHARP_TRIPAD)
#define PLATID_MACH_SHARP_TRIPAD_PV6000_NUM	1
#define PLATID_MACH_SHARP_TRIPAD_PV6000	\
  ((PLATID_MACH_SHARP_TRIPAD_PV6000_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_SHARP_TRIPAD_PV)
#define PLATID_MACH_SHARP_TELIOS_NUM	2
#define PLATID_MACH_SHARP_TELIOS	\
  ((PLATID_MACH_SHARP_TELIOS_NUM << PLATID_SERIES_SHIFT)| \
    PLATID_MACH_SHARP)
#define PLATID_MACH_SHARP_TELIOS_HC_NUM	1
#define PLATID_MACH_SHARP_TELIOS_HC	\
  ((PLATID_MACH_SHARP_TELIOS_HC_NUM << PLATID_MODEL_SHIFT)| \
    PLATID_MACH_SHARP_TELIOS)
#define PLATID_MACH_SHARP_TELIOS_HCAJ1_NUM	1
#define PLATID_MACH_SHARP_TELIOS_HCAJ1	\
  ((PLATID_MACH_SHARP_TELIOS_HCAJ1_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_SHARP_TELIOS_HC)
#define PLATID_MACH_SHARP_MOBILON_NUM	3
#define PLATID_MACH_SHARP_MOBILON	\
  ((PLATID_MACH_SHARP_MOBILON_NUM << PLATID_SERIES_SHIFT)| \
    PLATID_MACH_SHARP)
#define PLATID_MACH_SHARP_MOBILON_HC_NUM	1
#define PLATID_MACH_SHARP_MOBILON_HC	\
  ((PLATID_MACH_SHARP_MOBILON_HC_NUM << PLATID_MODEL_SHIFT)| \
    PLATID_MACH_SHARP_MOBILON)
#define PLATID_MACH_SHARP_MOBILON_HC4100_NUM	1
#define PLATID_MACH_SHARP_MOBILON_HC4100	\
  ((PLATID_MACH_SHARP_MOBILON_HC4100_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_SHARP_MOBILON_HC)
#define PLATID_MACH_FUJITSU_NUM	5
#define PLATID_MACH_FUJITSU	\
  ((PLATID_MACH_FUJITSU_NUM << PLATID_VENDOR_SHIFT))
#define PLATID_MACH_FUJITSU_INTERTOP_NUM	1
#define PLATID_MACH_FUJITSU_INTERTOP	\
  ((PLATID_MACH_FUJITSU_INTERTOP_NUM << PLATID_SERIES_SHIFT)| \
    PLATID_MACH_FUJITSU)
#define PLATID_MACH_FUJITSU_INTERTOP_ITXXX_NUM	1
#define PLATID_MACH_FUJITSU_INTERTOP_ITXXX	\
  ((PLATID_MACH_FUJITSU_INTERTOP_ITXXX_NUM << PLATID_MODEL_SHIFT)| \
    PLATID_MACH_FUJITSU_INTERTOP)
#define PLATID_MACH_FUJITSU_INTERTOP_IT300_NUM	1
#define PLATID_MACH_FUJITSU_INTERTOP_IT300	\
  ((PLATID_MACH_FUJITSU_INTERTOP_IT300_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_FUJITSU_INTERTOP_ITXXX)
#define PLATID_MACH_FUJITSU_INTERTOP_IT310_NUM	2
#define PLATID_MACH_FUJITSU_INTERTOP_IT310	\
  ((PLATID_MACH_FUJITSU_INTERTOP_IT310_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_FUJITSU_INTERTOP_ITXXX)
#define PLATID_MACH_PHILIPS_NUM	6
#define PLATID_MACH_PHILIPS	\
  ((PLATID_MACH_PHILIPS_NUM << PLATID_VENDOR_SHIFT))
#define PLATID_MACH_PHILIPS_NINO_NUM	1
#define PLATID_MACH_PHILIPS_NINO	\
  ((PLATID_MACH_PHILIPS_NINO_NUM << PLATID_SERIES_SHIFT)| \
    PLATID_MACH_PHILIPS)
#define PLATID_MACH_PHILIPS_NINO_3XX_NUM	1
#define PLATID_MACH_PHILIPS_NINO_3XX	\
  ((PLATID_MACH_PHILIPS_NINO_3XX_NUM << PLATID_MODEL_SHIFT)| \
    PLATID_MACH_PHILIPS_NINO)
#define PLATID_MACH_PHILIPS_NINO_312_NUM	1
#define PLATID_MACH_PHILIPS_NINO_312	\
  ((PLATID_MACH_PHILIPS_NINO_312_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_PHILIPS_NINO_3XX)
#define PLATID_MACH_COMPAQ_NUM	7
#define PLATID_MACH_COMPAQ	\
  ((PLATID_MACH_COMPAQ_NUM << PLATID_VENDOR_SHIFT))
#define PLATID_MACH_COMPAQ_C_NUM	1
#define PLATID_MACH_COMPAQ_C	\
  ((PLATID_MACH_COMPAQ_C_NUM << PLATID_SERIES_SHIFT)| \
    PLATID_MACH_COMPAQ)
#define PLATID_MACH_COMPAQ_C_8XX_NUM	1
#define PLATID_MACH_COMPAQ_C_8XX	\
  ((PLATID_MACH_COMPAQ_C_8XX_NUM << PLATID_MODEL_SHIFT)| \
    PLATID_MACH_COMPAQ_C)
#define PLATID_MACH_COMPAQ_C_810_NUM	1
#define PLATID_MACH_COMPAQ_C_810	\
  ((PLATID_MACH_COMPAQ_C_810_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_COMPAQ_C_8XX)
#define PLATID_MACH_COMPAQ_C_201X_NUM	2
#define PLATID_MACH_COMPAQ_C_201X	\
  ((PLATID_MACH_COMPAQ_C_201X_NUM << PLATID_MODEL_SHIFT)| \
    PLATID_MACH_COMPAQ_C)
#define PLATID_MACH_COMPAQ_C_2010_NUM	1
#define PLATID_MACH_COMPAQ_C_2010	\
  ((PLATID_MACH_COMPAQ_C_2010_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_COMPAQ_C_201X)
#define PLATID_MACH_COMPAQ_C_2015_NUM	2
#define PLATID_MACH_COMPAQ_C_2015	\
  ((PLATID_MACH_COMPAQ_C_2015_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_COMPAQ_C_201X)
#define PLATID_MACH_COMPAQ_AERO_NUM	2
#define PLATID_MACH_COMPAQ_AERO	\
  ((PLATID_MACH_COMPAQ_AERO_NUM << PLATID_SERIES_SHIFT)| \
    PLATID_MACH_COMPAQ)
#define PLATID_MACH_COMPAQ_AERO_15XX_NUM	1
#define PLATID_MACH_COMPAQ_AERO_15XX	\
  ((PLATID_MACH_COMPAQ_AERO_15XX_NUM << PLATID_MODEL_SHIFT)| \
    PLATID_MACH_COMPAQ_AERO)
#define PLATID_MACH_COMPAQ_AERO_1530_NUM	1
#define PLATID_MACH_COMPAQ_AERO_1530	\
  ((PLATID_MACH_COMPAQ_AERO_1530_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_COMPAQ_AERO_15XX)
#define PLATID_MACH_COMPAQ_AERO_21XX_NUM	2
#define PLATID_MACH_COMPAQ_AERO_21XX	\
  ((PLATID_MACH_COMPAQ_AERO_21XX_NUM << PLATID_MODEL_SHIFT)| \
    PLATID_MACH_COMPAQ_AERO)
#define PLATID_MACH_COMPAQ_AERO_2110_NUM	1
#define PLATID_MACH_COMPAQ_AERO_2110	\
  ((PLATID_MACH_COMPAQ_AERO_2110_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_COMPAQ_AERO_21XX)
#define PLATID_MACH_COMPAQ_AERO_2130_NUM	2
#define PLATID_MACH_COMPAQ_AERO_2130	\
  ((PLATID_MACH_COMPAQ_AERO_2130_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_COMPAQ_AERO_21XX)
#define PLATID_MACH_COMPAQ_AERO_2140_NUM	3
#define PLATID_MACH_COMPAQ_AERO_2140	\
  ((PLATID_MACH_COMPAQ_AERO_2140_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_COMPAQ_AERO_21XX)
#define PLATID_MACH_COMPAQ_PRESARIO_NUM	3
#define PLATID_MACH_COMPAQ_PRESARIO	\
  ((PLATID_MACH_COMPAQ_PRESARIO_NUM << PLATID_SERIES_SHIFT)| \
    PLATID_MACH_COMPAQ)
#define PLATID_MACH_COMPAQ_PRESARIO_21X_NUM	1
#define PLATID_MACH_COMPAQ_PRESARIO_21X	\
  ((PLATID_MACH_COMPAQ_PRESARIO_21X_NUM << PLATID_MODEL_SHIFT)| \
    PLATID_MACH_COMPAQ_PRESARIO)
#define PLATID_MACH_COMPAQ_PRESARIO_213_NUM	1
#define PLATID_MACH_COMPAQ_PRESARIO_213	\
  ((PLATID_MACH_COMPAQ_PRESARIO_213_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_COMPAQ_PRESARIO_21X)
#define PLATID_MACH_VICTOR_NUM	8
#define PLATID_MACH_VICTOR	\
  ((PLATID_MACH_VICTOR_NUM << PLATID_VENDOR_SHIFT))
#define PLATID_MACH_VICTOR_INTERLINK_NUM	1
#define PLATID_MACH_VICTOR_INTERLINK	\
  ((PLATID_MACH_VICTOR_INTERLINK_NUM << PLATID_SERIES_SHIFT)| \
    PLATID_MACH_VICTOR)
#define PLATID_MACH_VICTOR_INTERLINK_MP_NUM	1
#define PLATID_MACH_VICTOR_INTERLINK_MP	\
  ((PLATID_MACH_VICTOR_INTERLINK_MP_NUM << PLATID_MODEL_SHIFT)| \
    PLATID_MACH_VICTOR_INTERLINK)
#define PLATID_MACH_VICTOR_INTERLINK_MPC101_NUM	1
#define PLATID_MACH_VICTOR_INTERLINK_MPC101	\
  ((PLATID_MACH_VICTOR_INTERLINK_MPC101_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_VICTOR_INTERLINK_MP)
#define PLATID_MACH_IBM_NUM	9
#define PLATID_MACH_IBM	\
  ((PLATID_MACH_IBM_NUM << PLATID_VENDOR_SHIFT))
#define PLATID_MACH_IBM_WORKPAD_NUM	1
#define PLATID_MACH_IBM_WORKPAD	\
  ((PLATID_MACH_IBM_WORKPAD_NUM << PLATID_SERIES_SHIFT)| \
    PLATID_MACH_IBM)
#define PLATID_MACH_IBM_WORKPAD_Z50_NUM	1
#define PLATID_MACH_IBM_WORKPAD_Z50	\
  ((PLATID_MACH_IBM_WORKPAD_Z50_NUM << PLATID_MODEL_SHIFT)| \
    PLATID_MACH_IBM_WORKPAD)
#define PLATID_MACH_IBM_WORKPAD_26011AU_NUM	1
#define PLATID_MACH_IBM_WORKPAD_26011AU	\
  ((PLATID_MACH_IBM_WORKPAD_26011AU_NUM << PLATID_SUBMODEL_SHIFT)| \
    PLATID_MACH_IBM_WORKPAD_Z50)
