/***************************************************************************//**
* SFLASH IP definitions
*
********************************************************************************
* \copyright
* (c) 2016-2026, Infineon Technologies AG or an affiliate of
* Infineon Technologies AG.
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

#ifndef _CYIP_SFLASH_PSOC4HVMS64K_H_
#define _CYIP_SFLASH_PSOC4HVMS64K_H_

#include "cyip_headers.h"

/*******************************************************************************
*                                    SFLASH
*******************************************************************************/

#define SFLASH_SECTION_SIZE                     0x00000400UL

/**
  * \brief Supervisory Flash Area (Infineon Trim & Wounding Info) (SFLASH)
  */
typedef struct {
  __IOM uint16_t SILICON_ID;                    /*!< 0x00000000 Silicon ID */
  __IOM uint8_t  SWD_CONFIG;                    /*!< 0x00000002 SWD pinout selector */
   __IM uint8_t  RESERVED[15];
  __IOM uint16_t DPSLP_KEY_DELAY;               /*!< 0x00000012 DeepSleep wakeup value for PWR_KEY_DELAY */
  __IOM uint32_t SWD_LISTEN;                    /*!< 0x00000014 Listen Window Length */
  __IOM uint32_t FLASH_START;                   /*!< 0x00000018 Flash Image Start Address */
   __IM uint32_t RESERVED1[9];
  __IOM uint32_t USER_SFLASH_AREA[2];           /*!< 0x00000040 Pointer to between 0 and 2 customer usable SFLASH regions */
   __IM uint32_t RESERVED2[2];
  __IOM uint32_t DIE_UNIQUE_ID[6];              /*!< 0x00000050 Uses manufacturing identity to create a unique die ID */
   __IM uint32_t RESERVED3[102];
  __IOM uint8_t  IMO_TRIM_LT[8];                /*!< 0x00000200 IMO Frequency Trim Register (SRSS-Lite) - From 24 to 48 MHz in
                                                                steps of 4 MHz */
  __IOM uint8_t  IMO_TCTRIM_LT[8];              /*!< 0x00000208 IMO Temperature Compensation Trim - From 24 to 48 MHz in steps
                                                                of 4 MHz */
  __IOM uint8_t  IMO_STEPSIZE_LT[8];            /*!< 0x00000210 IMO Stepsize Trim - From 24 to 48 MHz in steps of 4 MHz */
  __IOM uint8_t  IMO_4PCT_LIM;                  /*!< 0x00000218 IMO trim 4 percent deviation from nominal */
  __IOM uint8_t  IMO_3PCT_LIM;                  /*!< 0x00000219 IMO trim 3 percent deviation from nominal */
  __IOM uint8_t  PILO_6PCT_LIM;                 /*!< 0x0000021A PILO trim 6 percent deviation from nominal */
   __IM uint8_t  RESERVED4[353];
  __IOM uint16_t SAR_TEMP_MULTIPLIER;           /*!< 0x0000037C SAR Temperature Sensor Multiplication Factor */
  __IOM uint16_t SAR_TEMP_OFFSET;               /*!< 0x0000037E SAR Temperature Sensor Offset */
} SFLASH_Type;                                  /*!< Size = 896 (0x380) */


/* SFLASH.SILICON_ID */
#define SFLASH_SILICON_ID_SI_ID_Pos             0UL
#define SFLASH_SILICON_ID_SI_ID_Msk             0xFFFFUL
/* SFLASH.SWD_CONFIG */
#define SFLASH_SWD_CONFIG_SWD_SELECT_Pos        0UL
#define SFLASH_SWD_CONFIG_SWD_SELECT_Msk        0x1UL
/* SFLASH.DPSLP_KEY_DELAY */
#define SFLASH_DPSLP_KEY_DELAY_WAKEUP_HOLDOFF_Pos 0UL
#define SFLASH_DPSLP_KEY_DELAY_WAKEUP_HOLDOFF_Msk 0x3FFUL
/* SFLASH.SWD_LISTEN */
#define SFLASH_SWD_LISTEN_CYCLES_Pos            0UL
#define SFLASH_SWD_LISTEN_CYCLES_Msk            0xFFFFFFFFUL
/* SFLASH.FLASH_START */
#define SFLASH_FLASH_START_ADDRESS_Pos          0UL
#define SFLASH_FLASH_START_ADDRESS_Msk          0xFFFFFFFFUL
/* SFLASH.USER_SFLASH_AREA */
#define SFLASH_USER_SFLASH_AREA_LOCATION_Pos    0UL
#define SFLASH_USER_SFLASH_AREA_LOCATION_Msk    0xFFFFUL
#define SFLASH_USER_SFLASH_AREA_SIZE_Pos        16UL
#define SFLASH_USER_SFLASH_AREA_SIZE_Msk        0xFFFF0000UL
/* SFLASH.DIE_UNIQUE_ID */
#define SFLASH_DIE_UNIQUE_ID_ID_Pos             0UL
#define SFLASH_DIE_UNIQUE_ID_ID_Msk             0xFFFFFFFFUL
/* SFLASH.IMO_TRIM_LT */
#define SFLASH_IMO_TRIM_LT_OFFSET_Pos           0UL
#define SFLASH_IMO_TRIM_LT_OFFSET_Msk           0xFFUL
/* SFLASH.IMO_TCTRIM_LT */
#define SFLASH_IMO_TCTRIM_LT_FSOFFSET_Pos       0UL
#define SFLASH_IMO_TCTRIM_LT_FSOFFSET_Msk       0x7UL
#define SFLASH_IMO_TCTRIM_LT_TCTRIM_Pos         4UL
#define SFLASH_IMO_TCTRIM_LT_TCTRIM_Msk         0xF0UL
/* SFLASH.IMO_STEPSIZE_LT */
#define SFLASH_IMO_STEPSIZE_LT_STEPSIZE_Pos     0UL
#define SFLASH_IMO_STEPSIZE_LT_STEPSIZE_Msk     0x1FUL
/* SFLASH.IMO_4PCT_LIM */
#define SFLASH_IMO_4PCT_LIM_TRIM_LIMIT_Pos      0UL
#define SFLASH_IMO_4PCT_LIM_TRIM_LIMIT_Msk      0xFFUL
/* SFLASH.IMO_3PCT_LIM */
#define SFLASH_IMO_3PCT_LIM_TRIM_LIMIT_Pos      0UL
#define SFLASH_IMO_3PCT_LIM_TRIM_LIMIT_Msk      0xFFUL
/* SFLASH.PILO_6PCT_LIM */
#define SFLASH_PILO_6PCT_LIM_TRIM_LIMIT_Pos     0UL
#define SFLASH_PILO_6PCT_LIM_TRIM_LIMIT_Msk     0xFFUL
/* SFLASH.SAR_TEMP_MULTIPLIER */
#define SFLASH_SAR_TEMP_MULTIPLIER_TEMP_MULTIPLIER_Pos 0UL
#define SFLASH_SAR_TEMP_MULTIPLIER_TEMP_MULTIPLIER_Msk 0xFFFFUL
/* SFLASH.SAR_TEMP_OFFSET */
#define SFLASH_SAR_TEMP_OFFSET_TEMP_OFFSET_Pos  0UL
#define SFLASH_SAR_TEMP_OFFSET_TEMP_OFFSET_Msk  0xFFFFUL


#endif /* _CYIP_SFLASH_PSOC4HVMS64K_H_ */


/* [] END OF FILE */
