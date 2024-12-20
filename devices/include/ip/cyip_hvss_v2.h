/***************************************************************************//**
* \file cyip_hvss_v2.h
*
* \brief
* HVSS IP definitions
*
********************************************************************************
* \copyright
* (c) (2016-2024), Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.
*
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#ifndef _CYIP_HVSS_V2_H_
#define _CYIP_HVSS_V2_H_

#include "cyip_headers.h"

/*******************************************************************************
*                                     HVSS
*******************************************************************************/

#define HVSS_SECTION_SIZE                       0x00010000UL

/**
  * \brief High Voltage Subsystem (HVSS)
  */
typedef struct {
   __IM uint32_t HVREG_STATUS;                  /*!< 0x00000000 HVREG Status */
  __IOM uint32_t HVREG_BIST;                    /*!< 0x00000004 HVREG Bist */
  __IOM uint32_t HVREG_CTRL;                    /*!< 0x00000008 HVREG Control */
   __IM uint32_t RESERVED;
  __IOM uint32_t RDIV_CTL;                      /*!< 0x00000010 Resistor Attenuator Control */
   __IM uint32_t RESERVED1[3];
  __IOM uint32_t LIN_CTL;                       /*!< 0x00000020 LIN PHY Control */
  __IOM uint32_t LIN_TIMER;                     /*!< 0x00000024 LIN Timer Control */
   __IM uint32_t LIN_STATUS;                    /*!< 0x00000028 LIN Status */
  __IOM uint32_t LIN_BIST;                      /*!< 0x0000002C LIN BIST Control */
  __IOM uint32_t LIN_INTR;                      /*!< 0x00000030 HVSS Interrupt Request Register */
  __IOM uint32_t LIN_INTR_SET;                  /*!< 0x00000034 HVSS Interrupt set register */
  __IOM uint32_t LIN_INTR_MASK;                 /*!< 0x00000038 HVSS Interrupt mask register */
   __IM uint32_t LIN_INTR_MASKED;               /*!< 0x0000003C HVSS Interrupt masked register */
  __IOM uint32_t HVSS_DDFT;                     /*!< 0x00000040 HVSS DDFT Control */
   __IM uint32_t RESERVED2[16303];
  __IOM uint32_t HVREG_TRIM;                    /*!< 0x0000FF00 HVREG Master Trim (Output Voltage) */
  __IOM uint32_t LIN_TRIM;                      /*!< 0x0000FF04 LIN Trim Settings */
   __IM uint32_t RESERVED3[2];
  __IOM uint32_t HVREG_TRIM_VREF;               /*!< 0x0000FF10 HVREG Vref Trim */
  __IOM uint32_t HVREG_TRIM_VREF_TC;            /*!< 0x0000FF14 HVREG Vref Output Voltage temp-co Trim */
  __IOM uint32_t HVREG_TRIM_TSD;                /*!< 0x0000FF18 HVREG TSD detection temperatureTrim */
  __IOM uint32_t HVREG_TRIM_S;                  /*!< 0x0000FF1C HVREG Slave Trim (Output Voltage) */
} HVSS_Type;                                    /*!< Size = 65312 (0xFF20) */


/* HVSS.HVREG_STATUS */
#define HVSS_HVREG_STATUS_PWR_GOOD_Pos          0UL
#define HVSS_HVREG_STATUS_PWR_GOOD_Msk          0x1UL
#define HVSS_HVREG_STATUS_PWR_GOOD_MAST_Pos     1UL
#define HVSS_HVREG_STATUS_PWR_GOOD_MAST_Msk     0x2UL
#define HVSS_HVREG_STATUS_PWR_GOOD_SLAVE_Pos    2UL
#define HVSS_HVREG_STATUS_PWR_GOOD_SLAVE_Msk    0x4UL
#define HVSS_HVREG_STATUS_THERMAL_SHUTDOWN_Pos  3UL
#define HVSS_HVREG_STATUS_THERMAL_SHUTDOWN_Msk  0x8UL
/* HVSS.HVREG_BIST */
#define HVSS_HVREG_BIST_HVREG_ADFT_Pos          0UL
#define HVSS_HVREG_BIST_HVREG_ADFT_Msk          0xFUL
#define HVSS_HVREG_BIST_HVREG_DDFT_Pos          4UL
#define HVSS_HVREG_BIST_HVREG_DDFT_Msk          0x10UL
#define HVSS_HVREG_BIST_EN_BURN_IN_Pos          8UL
#define HVSS_HVREG_BIST_EN_BURN_IN_Msk          0x100UL
#define HVSS_HVREG_BIST_SLAVE_ADFT_Pos          12UL
#define HVSS_HVREG_BIST_SLAVE_ADFT_Msk          0xF000UL
/* HVSS.HVREG_CTRL */
#define HVSS_HVREG_CTRL_ZENER_CLAMP_MODE_Pos    0UL
#define HVSS_HVREG_CTRL_ZENER_CLAMP_MODE_Msk    0x3UL
#define HVSS_HVREG_CTRL_TSD_MODE_Pos            4UL
#define HVSS_HVREG_CTRL_TSD_MODE_Msk            0x30UL
#define HVSS_HVREG_CTRL_EN_VDIV_Pos             8UL
#define HVSS_HVREG_CTRL_EN_VDIV_Msk             0x100UL
#define HVSS_HVREG_CTRL_EN_SLAVE_Pos            15UL
#define HVSS_HVREG_CTRL_EN_SLAVE_Msk            0x8000UL
/* HVSS.RDIV_CTL */
#define HVSS_RDIV_CTL_RDIV_EN_0_Pos             0UL
#define HVSS_RDIV_CTL_RDIV_EN_0_Msk             0x1UL
#define HVSS_RDIV_CTL_RDIV_EN_1_Pos             1UL
#define HVSS_RDIV_CTL_RDIV_EN_1_Msk             0x2UL
#define HVSS_RDIV_CTL_RDIV_ACT_EN_Pos           2UL
#define HVSS_RDIV_CTL_RDIV_ACT_EN_Msk           0x4UL
#define HVSS_RDIV_CTL_RDIV_SCALE_0_Pos          4UL
#define HVSS_RDIV_CTL_RDIV_SCALE_0_Msk          0x10UL
#define HVSS_RDIV_CTL_RDIV_SCALE_1_Pos          5UL
#define HVSS_RDIV_CTL_RDIV_SCALE_1_Msk          0x20UL
/* HVSS.LIN_CTL */
#define HVSS_LIN_CTL_LIN_MODE_Pos               0UL
#define HVSS_LIN_CTL_LIN_MODE_Msk               0x7UL
#define HVSS_LIN_CTL_RF_DETECT_Pos              4UL
#define HVSS_LIN_CTL_RF_DETECT_Msk              0x10UL
#define HVSS_LIN_CTL_USE_ALT_INTERFACE_Pos      8UL
#define HVSS_LIN_CTL_USE_ALT_INTERFACE_Msk      0x100UL
#define HVSS_LIN_CTL_SEL_CXPI_LIN_Pos           12UL
#define HVSS_LIN_CTL_SEL_CXPI_LIN_Msk           0x3000UL
#define HVSS_LIN_CTL_LIN_EN_Pos                 31UL
#define HVSS_LIN_CTL_LIN_EN_Msk                 0x80000000UL
/* HVSS.LIN_TIMER */
#define HVSS_LIN_TIMER_WAKEUP_TIMER_Pos         0UL
#define HVSS_LIN_TIMER_WAKEUP_TIMER_Msk         0xFFFUL
#define HVSS_LIN_TIMER_FAULT_TIMER_Pos          16UL
#define HVSS_LIN_TIMER_FAULT_TIMER_Msk          0xFFF0000UL
#define HVSS_LIN_TIMER_WAKEUP_TIMER_EN_Pos      30UL
#define HVSS_LIN_TIMER_WAKEUP_TIMER_EN_Msk      0x40000000UL
#define HVSS_LIN_TIMER_FAULT_TIMER_EN_Pos       31UL
#define HVSS_LIN_TIMER_FAULT_TIMER_EN_Msk       0x80000000UL
/* HVSS.LIN_STATUS */
#define HVSS_LIN_STATUS_LIN_RXD_Pos             0UL
#define HVSS_LIN_STATUS_LIN_RXD_Msk             0x1UL
/* HVSS.LIN_BIST */
#define HVSS_LIN_BIST_LIN_ADFT_Pos              0UL
#define HVSS_LIN_BIST_LIN_ADFT_Msk              0x7UL
#define HVSS_LIN_BIST_LIN_DDFT_Pos              4UL
#define HVSS_LIN_BIST_LIN_DDFT_Msk              0x10UL
#define HVSS_LIN_BIST_LIN_DFT_START_Pos         5UL
#define HVSS_LIN_BIST_LIN_DFT_START_Msk         0x20UL
/* HVSS.LIN_INTR */
#define HVSS_LIN_INTR_WAKEUP_Pos                0UL
#define HVSS_LIN_INTR_WAKEUP_Msk                0x1UL
#define HVSS_LIN_INTR_FAULT_Pos                 1UL
#define HVSS_LIN_INTR_FAULT_Msk                 0x2UL
#define HVSS_LIN_INTR_HVREG_TSD_Pos             2UL
#define HVSS_LIN_INTR_HVREG_TSD_Msk             0x4UL
/* HVSS.LIN_INTR_SET */
#define HVSS_LIN_INTR_SET_WAKEUP_Pos            0UL
#define HVSS_LIN_INTR_SET_WAKEUP_Msk            0x1UL
#define HVSS_LIN_INTR_SET_FAULT_Pos             1UL
#define HVSS_LIN_INTR_SET_FAULT_Msk             0x2UL
#define HVSS_LIN_INTR_SET_HVREG_TSD_Pos         2UL
#define HVSS_LIN_INTR_SET_HVREG_TSD_Msk         0x4UL
/* HVSS.LIN_INTR_MASK */
#define HVSS_LIN_INTR_MASK_WAKEUP_Pos           0UL
#define HVSS_LIN_INTR_MASK_WAKEUP_Msk           0x1UL
#define HVSS_LIN_INTR_MASK_FAULT_Pos            1UL
#define HVSS_LIN_INTR_MASK_FAULT_Msk            0x2UL
#define HVSS_LIN_INTR_MASK_HVREG_TSD_Pos        2UL
#define HVSS_LIN_INTR_MASK_HVREG_TSD_Msk        0x4UL
/* HVSS.LIN_INTR_MASKED */
#define HVSS_LIN_INTR_MASKED_WAKEUP_Pos         0UL
#define HVSS_LIN_INTR_MASKED_WAKEUP_Msk         0x1UL
#define HVSS_LIN_INTR_MASKED_FAULT_Pos          1UL
#define HVSS_LIN_INTR_MASKED_FAULT_Msk          0x2UL
#define HVSS_LIN_INTR_MASKED_HVREG_TSD_Pos      2UL
#define HVSS_LIN_INTR_MASKED_HVREG_TSD_Msk      0x4UL
/* HVSS.HVSS_DDFT */
#define HVSS_HVSS_DDFT_LSOUT_DDFT_Pos           0UL
#define HVSS_HVSS_DDFT_LSOUT_DDFT_Msk           0x1UL
/* HVSS.HVREG_TRIM */
#define HVSS_HVREG_TRIM_TRIM_Pos                0UL
#define HVSS_HVREG_TRIM_TRIM_Msk                0x3FUL
/* HVSS.LIN_TRIM */
#define HVSS_LIN_TRIM_SLEW_CTL_Pos              0UL
#define HVSS_LIN_TRIM_SLEW_CTL_Msk              0xFFUL
/* HVSS.HVREG_TRIM_VREF */
#define HVSS_HVREG_TRIM_VREF_VREF_CTL_Pos       0UL
#define HVSS_HVREG_TRIM_VREF_VREF_CTL_Msk       0x3FUL
/* HVSS.HVREG_TRIM_VREF_TC */
#define HVSS_HVREG_TRIM_VREF_TC_TC_CTL_Pos      0UL
#define HVSS_HVREG_TRIM_VREF_TC_TC_CTL_Msk      0x3FUL
/* HVSS.HVREG_TRIM_TSD */
#define HVSS_HVREG_TRIM_TSD_TSD_CTL_Pos         0UL
#define HVSS_HVREG_TRIM_TSD_TSD_CTL_Msk         0x3FUL
/* HVSS.HVREG_TRIM_S */
#define HVSS_HVREG_TRIM_S_TRIM_Pos              0UL
#define HVSS_HVREG_TRIM_S_TRIM_Msk              0x3FUL


#endif /* _CYIP_HVSS_V2_H_ */


/* [] END OF FILE */
