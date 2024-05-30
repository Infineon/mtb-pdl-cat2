/***************************************************************************//**
* \file cyip_hsiom_v2.h
*
* \brief
* HSIOM IP definitions
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

#ifndef _CYIP_HSIOM_V2_H_
#define _CYIP_HSIOM_V2_H_

#include "cyip_headers.h"

/*******************************************************************************
*                                    HSIOM
*******************************************************************************/

#define HSIOM_PRT_SECTION_SIZE                  0x00000100UL
#define HSIOM_SECTION_SIZE                      0x00004000UL

/**
  * \brief HSIOM port registers (HSIOM_PRT)
  */
typedef struct {
  __IOM uint32_t PORT_SEL;                      /*!< 0x00000000 Port selection register */
   __IM uint32_t RESERVED[63];
} HSIOM_PRT_Type;                               /*!< Size = 256 (0x100) */

/**
  * \brief High Speed IO Matrix (HSIOM) (HSIOM)
  */
typedef struct {
        HSIOM_PRT_Type PRT[16];                 /*!< 0x00000000 HSIOM port registers */
   __IM uint32_t RESERVED[1024];
  __IOM uint32_t PUMP_CTL;                      /*!< 0x00002000 Pump control */
   __IM uint32_t RESERVED1[63];
  __IOM uint32_t AMUX_SPLIT_CTL[8];             /*!< 0x00002100 AMUX splitter cell control */
} HSIOM_Type;                                   /*!< Size = 8480 (0x2120) */


/* HSIOM_PRT.PORT_SEL */
#define HSIOM_PRT_PORT_SEL_IO0_SEL_Pos          0UL
#define HSIOM_PRT_PORT_SEL_IO0_SEL_Msk          0xFUL
#define HSIOM_PRT_PORT_SEL_IO1_SEL_Pos          4UL
#define HSIOM_PRT_PORT_SEL_IO1_SEL_Msk          0xF0UL
#define HSIOM_PRT_PORT_SEL_IO2_SEL_Pos          8UL
#define HSIOM_PRT_PORT_SEL_IO2_SEL_Msk          0xF00UL
#define HSIOM_PRT_PORT_SEL_IO3_SEL_Pos          12UL
#define HSIOM_PRT_PORT_SEL_IO3_SEL_Msk          0xF000UL
#define HSIOM_PRT_PORT_SEL_IO4_SEL_Pos          16UL
#define HSIOM_PRT_PORT_SEL_IO4_SEL_Msk          0xF0000UL
#define HSIOM_PRT_PORT_SEL_IO5_SEL_Pos          20UL
#define HSIOM_PRT_PORT_SEL_IO5_SEL_Msk          0xF00000UL
#define HSIOM_PRT_PORT_SEL_IO6_SEL_Pos          24UL
#define HSIOM_PRT_PORT_SEL_IO6_SEL_Msk          0xF000000UL
#define HSIOM_PRT_PORT_SEL_IO7_SEL_Pos          28UL
#define HSIOM_PRT_PORT_SEL_IO7_SEL_Msk          0xF0000000UL


/* HSIOM.PUMP_CTL */
#define HSIOM_PUMP_CTL_CLOCK_SEL_Pos            0UL
#define HSIOM_PUMP_CTL_CLOCK_SEL_Msk            0x1UL
#define HSIOM_PUMP_CTL_ENABLED_Pos              31UL
#define HSIOM_PUMP_CTL_ENABLED_Msk              0x80000000UL
/* HSIOM.AMUX_SPLIT_CTL */
#define HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SL_Pos   0UL
#define HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SL_Msk   0x1UL
#define HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SR_Pos   1UL
#define HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SR_Msk   0x2UL
#define HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_S0_Pos   2UL
#define HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_S0_Msk   0x4UL
#define HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SL_Pos   4UL
#define HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SL_Msk   0x10UL
#define HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SR_Pos   5UL
#define HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SR_Msk   0x20UL
#define HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_S0_Pos   6UL
#define HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_S0_Msk   0x40UL


#endif /* _CYIP_HSIOM_V2_H_ */


/* [] END OF FILE */
