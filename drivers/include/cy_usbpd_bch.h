/***************************************************************************//**
* \file cy_usbpd_bch.h
* \version 1.10
*
* Header file for legacy charging feature of USBPD PDL.
*
********************************************************************************
* \copyright
* (c) (2021), Cypress Semiconductor Corporation (an Infineon company) or
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

#ifndef _CY_USBPD_BCH_H_
#define _CY_USBPD_BCH_H_

#include <cy_usbpd_common.h>

/**
* \addtogroup group_usbpd_legacy
* \{
* USBPD Legacy Charging driver.
*
* \defgroup group_usbpd_legacy_macros Macros
* \defgroup group_usbpd_legacy_enums Enumerated Types
* \defgroup group_usbpd_legacy_functions Functions
*/
#if (defined(CY_IP_MXUSBPD) || defined(CY_IP_M0S8USBPD))

/*******************************************************************************
 * MACRO Definition
 ******************************************************************************/

/**
* \addtogroup group_usbpd_legacy_macros
* \{
*/
#define BC_EVT_NONE                             (0x00000000u)
/**< No legacy charging events pending. */

#define BC_EVT_ENTRY                            (0x00000001u)
/**< Legacy charging state change pending. */

#define BC_EVT_CMP1_FIRE                        (0x00000002u)
/**< Charger detect comparator-1 interrupt raised. */

#define BC_EVT_CMP2_FIRE                        (0x00000004u)
/**< Charger detect comparator-2 interrupt raised. */

#define BC_EVT_QC_CHANGE                        (0x00000008u)
/**< Qualcomm charging state change. */

#define BC_EVT_QC_CONT                          (0x00000010u)
/**< QC charging continuous mode entry. */

#define BC_EVT_AFC_RESET_RCVD                   (0x00000020u)
/**< Adaptive Fast Charging Reset received. */

#define BC_EVT_AFC_MSG_RCVD                     (0x00000040u)
/**< Adaptive Fast Charging message received. */

#define BC_EVT_AFC_MSG_SENT                     (0x00000080u)
/**< Adaptive Fast Charging message sent. */

#define BC_EVT_AFC_MSG_SEND_FAIL                (0x00000100u)
/**< Adaptive Fast Charging message sending failed. */

#define BC_EVT_TIMEOUT1                         (0x00000200u)
/**< Charger detect state machine timeout #1. */

#define BC_EVT_TIMEOUT2                         (0x00000400u)
/**< Charger detect state machine timeout #1. */

#define BC_EVT_DISCONNECT                       (0x00000800u)
/**< Disconnect detected by charger detect state machine. */

#define BC_EVT_ALL_MASK                         (0xFFFFFFFFu)
/**< Mask all charger detect states. */

#define BCH_PORT_0_CMP1_INTR_MASK               (0x1UL)
/**< Interrupt mask for charger detect comparator #1 on port 0. */

#define BCH_PORT_0_CMP2_INTR_MASK               (0x2UL)
/**< Interrupt mask for charger detect comparator #2 on port 0. */

#define BCH_PORT_0_CMP1_2_INTR_MASK             (BCH_PORT_0_CMP1_INTR_MASK | BCH_PORT_0_CMP2_INTR_MASK)
/**< Interrupt mask for both charger detect comparators on port 0. */

#define BCH_PORT_1_CMP1_INTR_MASK               (0x4UL)
/**< Interrupt mask for charger detect comparator #1 on port 1. */

#define BCH_PORT_1_CMP2_INTR_MASK               (0x8UL)
/**< Interrupt mask for charger detect comparator #1 on port 1. */

#define BCH_PORT_1_CMP1_2_INTR_MASK             (0xcUL)
/**< Interrupt mask for both charger detect comparators on port 1. */

#define QC3_PORT_0_DP_PULSE_MASK                (0x01u)
/**< Interrupt mask for QC 3.0 pulse on D+ pin of port 0. */

#define QC3_PORT_0_DM_PULSE_MASK                (0x10u)
/**< Interrupt mask for QC 3.0 pulse on D- pin of port 0. */

#define QC3_PORT_0_DP_DM_PULSE_MASK             (0x11u)
/**< Interrupt mask for QC 3.0 pulse on D+/D- pin of port 0. */

#define QC3_PORT_1_DP_PULSE_MASK                (0x02u)
/**< Interrupt mask for QC 3.0 pulse on D+ pin of port 1. */

#define QC3_PORT_1_DM_PULSE_MASK                (0x20u)
/**< Interrupt mask for QC 3.0 pulse on D- pin of port 1. */

#define QC3_PORT_1_DP_DM_PULSE_MASK             (0x22u)
/**< Interrupt mask for QC 3.0 pulse on D+/D- pin of port 1. */

#define QC3_DP_DM_PULSE_FILTER_CLOCK_SEL        (160u)
/**< QC 3.0 D+/D- pulse filter setting: 160us assuming 1MHz clock. */

#define AFC_UI_CLK_CYCLE_COUNT                  (160u)
/**< AFC UI (160us) in terms of number of 1MHz clock cycles. */

#define AFC_IDLE_OPCODE                         (0u)
/**< AFC state machine idle. */

#define AFC_TX_PING_OPCODE                      (1u)
/**< AFC transmitter ping opcode. */

#define AFC_RX_PING_OPCODE                      (2u)
/**< AFC receiver ping opcode. */

#define AFC_TX_DATA_M_OPCODE                    (3u)
/**< AFC master data transmit opcode. */

#define AFC_TX_DATA_S_OPCODE                    (4u)
/**< AFC slave data transmit opcode. */

#define AFC_RX_DATA_OPCODE                      (5u)
/**< AFC receive data opcode. */

#define AFC_SOURCE_OPCODE                       ((AFC_RX_PING_OPCODE << 0) | (AFC_TX_PING_OPCODE << 3) | \
                                                (AFC_RX_DATA_OPCODE << 6) | (AFC_TX_PING_OPCODE << 9) | \
                                                (AFC_TX_DATA_S_OPCODE << 12) | (AFC_RX_PING_OPCODE << 15) | \
                                                (AFC_TX_PING_OPCODE << 18) | (AFC_IDLE_OPCODE << 21))
/**< AFC source opcode selection. */

#define AFC_MAX_BYTES                           (16u)
/**< Maximum number of bytes in an AFC message. */

#define AFC_BASE_VOLT                           (5000u)
/**< Base voltage level during AFC negotiation. */

#define AFC_VOLT_STEP                           (1000u)
/**< AFC voltage unit increment. */

#define AFC_BASE_AMP                            (75u)
/**< Minimum AFC current level in 10 mA units. */

#define AFC_AMP_STEP                            (15u)
/**< AFC current level unit increment in 10 mA units. */

#define AFC_MAX_AMP                             (0x0Fu)
/**< Max current bit value for AFC VI message */

#define CDP_CHGDET_VREF_325mV                   (0u)     /**< VRef selection: 325 mV */
#define CDP_CHGDET_VREF_600mV                   (1u)     /**< VRef selection: 700 mV */
#define CDP_CHGDET_VREF_850mV                   (2u)     /**< VRef selection: 850 mV */

#define CDP_CHGDET_COMP_INP_DM                  (0u)     /**< Chg.Det. comparator input selection: D- */
#define CDP_CHGDET_COMP_INP_VREF                (1u)     /**< Chg.Det. comparator input selection: VRef */
#define CDP_CHGDET_COMP_INP_DP                  (2u)     /**< Chg.Det. comparator input selection: D+ */

/** \} group_usbpd_legacy_macros */

/*******************************************************************************
*                              Function Prototypes
*******************************************************************************/

/**
* \addtogroup group_usbpd_legacy_functions
* \{
* The functions provide a set of APIs to handle battery charging operations
*
*/

cy_en_usbpd_status_t Cy_USBPD_Bch_Phy_Init(cy_stc_usbpd_context_t *context,
                                           cy_cb_bc_phy_events_t bcPhyCbk);

cy_en_usbpd_status_t Cy_USBPD_Bch_Phy_En(cy_stc_usbpd_context_t *context);

cy_en_usbpd_status_t Cy_USBPD_Bch_Phy_Dis(cy_stc_usbpd_context_t *context);

cy_en_usbpd_status_t Cy_USBPD_Bch_Phy_ConfigSrcTerm(cy_stc_usbpd_context_t *context,
                                                    cy_en_usbpd_bch_src_term_t chargerTerm);

cy_en_usbpd_status_t Cy_USBPD_Bch_Phy_ConfigSnkTerm(cy_stc_usbpd_context_t *context,
                                                    cy_en_usbpd_bch_snk_term_t chargerTerm);

cy_en_usbpd_status_t Cy_USBPD_Bch_Phy_RemoveTerm(cy_stc_usbpd_context_t *context);

bool Cy_USBPD_Bch_Phy_Config_Comp(cy_stc_usbpd_context_t *context,
                                  uint8_t compIdx,
                                  cy_en_usbpd_bch_comp_pinput_t pInput,
                                  cy_en_usbpd_bch_comp_ninput_t mInput,
                                  cy_en_usbpd_bch_vref_t vref,
                                  cy_en_usbpd_bch_comp_edge_t edge);

cy_en_usbpd_status_t Cy_USBPD_Bch_Phy_DisableComp(cy_stc_usbpd_context_t *context,
                                                  uint8_t compIdx);

cy_en_usbpd_status_t Cy_USBPD_Bch_Phy_Isolate_DpDm(cy_stc_usbpd_context_t *context);

bool Cy_USBPD_Bch_Phy_CompResult(cy_stc_usbpd_context_t *context, uint8_t compIdx);

cy_en_usbpd_status_t Cy_USBPD_Bch_ApplyDpPd(cy_stc_usbpd_context_t *context);

cy_en_usbpd_status_t Cy_USBPD_Bch_RemoveDpPd(cy_stc_usbpd_context_t *context);

void Cy_USBPD_Bch_ApplyRdatLkgDp(cy_stc_usbpd_context_t *context);

void Cy_USBPD_Bch_ApplyRdatLkgDm(cy_stc_usbpd_context_t *context);

void Cy_USBPD_Bch_RemoveRdatLkgDp(cy_stc_usbpd_context_t *context);

void Cy_USBPD_Bch_RemoveRdatLkgDm(cy_stc_usbpd_context_t *context);

void Cy_USBPD_Bch_Apply_AppleTermDp(cy_stc_usbpd_context_t *context,
                                       cy_en_usbpd_bch_src_term_t appleTermId);

void Cy_USBPD_Bch_Apply_AppleTermDm(cy_stc_usbpd_context_t *context,
                                       cy_en_usbpd_bch_src_term_t appleTermId);

void Cy_USBPD_Bch_Apply_VdmSrc(cy_stc_usbpd_context_t *context);

void Cy_USBPD_Bch_Remove_VdmSrc(cy_stc_usbpd_context_t *context);

bool Cy_USBPD_Bch_Is_VdmSrc_On(cy_stc_usbpd_context_t *context);

void Cy_USBPD_Bch_Remove_AppleSrcDp(cy_stc_usbpd_context_t *context);

void Cy_USBPD_Bch_Remove_AppleSrcDm(cy_stc_usbpd_context_t *context);

void Cy_USBPD_Bch_Enable_AppleDet(cy_stc_usbpd_context_t *context);

void Cy_USBPD_Bch_Disable_AppleDet(cy_stc_usbpd_context_t *context);

bool Cy_USBPD_Bch_Phy_DpStat(cy_stc_usbpd_context_t *context);

bool Cy_USBPD_Bch_Phy_DmStat(cy_stc_usbpd_context_t *context);

void Cy_USBPD_Bch_Phy_Config_DeepSleep(cy_stc_usbpd_context_t *context);

void Cy_USBPD_Bch_Phy_Config_Wakeup(cy_stc_usbpd_context_t *context);

void Cy_USBPD_Bch_Intr1Handler(cy_stc_usbpd_context_t *context);

/** \} group_usbpd_legacy_functions */

#endif /* (defined(CY_IP_MXUSBPD) || defined(CY_IP_M0S8USBPD)) */

#endif /* _CY_USBPD_BCH_H_ */

/** \} group_usbpd_legacy */

/* [] End of File */
