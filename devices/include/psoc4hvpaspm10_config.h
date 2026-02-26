/***************************************************************************//**
* PSOC4HVPASPM10 device configuration header
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

#ifndef _PSOC4HVPASPM10_CONFIG_H_
#define _PSOC4HVPASPM10_CONFIG_H_

/* Clock Connections */
typedef enum
{
    PCLK_SCB0_CLOCK                 = 0x0000u,  /* scb[0].clock */
    PCLK_SCB1_CLOCK                 = 0x0001u,  /* scb[1].clock */
    PCLK_TCPWM_CLOCKS0              = 0x0002u,  /* tcpwm.clocks[0] */
    PCLK_TCPWM_CLOCKS1              = 0x0003u,  /* tcpwm.clocks[1] */
    PCLK_TCPWM_CLOCKS2              = 0x0004u,  /* tcpwm.clocks[2] */
    PCLK_TCPWM_CLOCKS3              = 0x0005u,  /* tcpwm.clocks[3] */
    PCLK_TCPWM_CLOCKS4              = 0x0006u,  /* tcpwm.clocks[4] */
    PCLK_PACSS_CLOCK_DSM            = 0x0007u,  /* pacss.clock_dsm */
    PCLK_CANFD_CLOCK_CAN0           = 0x0008u,  /* canfd.clock_can[0] */
    PCLK_ISOUART_CLOCK_IUART        = 0x0009u,  /* isouart.clock_iuart */
    PCLK_LIN_CLOCK_CH_EN0           = 0x000Au,  /* lin.clock_ch_en[0] */
    PCLK_LIN_CLOCK_CH_EN1           = 0x000Bu   /* lin.clock_ch_en[1] */
} en_clk_dst_t;

/* Trigger Group */
/* This section contains the enums related to the Trigger multiplexer (TrigMux) driver.
* Refer to the Cypress Peripheral Driver Library Documentation, section Trigger multiplexer (TrigMux) -> Enumerated Types for details.
*/
/* Trigger Group Inputs */
/* Trigger Input Group 0 - DMA Request Assignments */
typedef enum
{
    TRIG0_IN_CPUSS_ZERO             = 0x00000000u, /* cpuss.zero */
    TRIG0_IN_CPUSS_DMAC_TR_OUT0     = 0x00000001u, /* cpuss.dmac_tr_out[0] */
    TRIG0_IN_CPUSS_DMAC_TR_OUT1     = 0x00000002u, /* cpuss.dmac_tr_out[1] */
    TRIG0_IN_CPUSS_DMAC_TR_OUT2     = 0x00000003u, /* cpuss.dmac_tr_out[2] */
    TRIG0_IN_CPUSS_DMAC_TR_OUT3     = 0x00000004u, /* cpuss.dmac_tr_out[3] */
    TRIG0_IN_CPUSS_DMAC_TR_OUT4     = 0x00000005u, /* cpuss.dmac_tr_out[4] */
    TRIG0_IN_CPUSS_DMAC_TR_OUT5     = 0x00000006u, /* cpuss.dmac_tr_out[5] */
    TRIG0_IN_CPUSS_DMAC_TR_OUT6     = 0x00000007u, /* cpuss.dmac_tr_out[6] */
    TRIG0_IN_CPUSS_DMAC_TR_OUT7     = 0x00000008u, /* cpuss.dmac_tr_out[7] */
    TRIG0_IN_CPUSS_DMAC_TR_OUT8     = 0x00000009u, /* cpuss.dmac_tr_out[8] */
    TRIG0_IN_CPUSS_DMAC_TR_OUT9     = 0x0000000Au, /* cpuss.dmac_tr_out[9] */
    TRIG0_IN_CPUSS_DMAC_TR_OUT10    = 0x0000000Bu, /* cpuss.dmac_tr_out[10] */
    TRIG0_IN_CPUSS_DMAC_TR_OUT11    = 0x0000000Cu, /* cpuss.dmac_tr_out[11] */
    TRIG0_IN_CPUSS_DMAC_TR_OUT12    = 0x0000000Du, /* cpuss.dmac_tr_out[12] */
    TRIG0_IN_CPUSS_DMAC_TR_OUT13    = 0x0000000Eu, /* cpuss.dmac_tr_out[13] */
    TRIG0_IN_CPUSS_DMAC_TR_OUT14    = 0x0000000Fu, /* cpuss.dmac_tr_out[14] */
    TRIG0_IN_CPUSS_DMAC_TR_OUT15    = 0x00000010u, /* cpuss.dmac_tr_out[15] */
    TRIG0_IN_CPUSS_TR_FAULT0        = 0x00000011u, /* cpuss.tr_fault[0] */
    TRIG0_IN_CPUSS_TR_FAULT1        = 0x00000012u, /* cpuss.tr_fault[1] */
    TRIG0_IN_TCPWM_TR_OVERFLOW0     = 0x00000013u, /* tcpwm.tr_overflow[0] */
    TRIG0_IN_TCPWM_TR_OVERFLOW1     = 0x00000014u, /* tcpwm.tr_overflow[1] */
    TRIG0_IN_TCPWM_TR_OVERFLOW2     = 0x00000015u, /* tcpwm.tr_overflow[2] */
    TRIG0_IN_TCPWM_TR_OVERFLOW3     = 0x00000016u, /* tcpwm.tr_overflow[3] */
    TRIG0_IN_TCPWM_TR_OVERFLOW4     = 0x00000017u, /* tcpwm.tr_overflow[4] */
    TRIG0_IN_TCPWM_TR_UNDERFLOW0    = 0x00000018u, /* tcpwm.tr_underflow[0] */
    TRIG0_IN_TCPWM_TR_UNDERFLOW1    = 0x00000019u, /* tcpwm.tr_underflow[1] */
    TRIG0_IN_TCPWM_TR_UNDERFLOW2    = 0x0000001Au, /* tcpwm.tr_underflow[2] */
    TRIG0_IN_TCPWM_TR_UNDERFLOW3    = 0x0000001Bu, /* tcpwm.tr_underflow[3] */
    TRIG0_IN_TCPWM_TR_UNDERFLOW4    = 0x0000001Cu, /* tcpwm.tr_underflow[4] */
    TRIG0_IN_TCPWM_TR_COMPARE_MATCH0 = 0x0000001Du, /* tcpwm.tr_compare_match[0] */
    TRIG0_IN_TCPWM_TR_COMPARE_MATCH1 = 0x0000001Eu, /* tcpwm.tr_compare_match[1] */
    TRIG0_IN_TCPWM_TR_COMPARE_MATCH2 = 0x0000001Fu, /* tcpwm.tr_compare_match[2] */
    TRIG0_IN_TCPWM_TR_COMPARE_MATCH3 = 0x00000020u, /* tcpwm.tr_compare_match[3] */
    TRIG0_IN_TCPWM_TR_COMPARE_MATCH4 = 0x00000021u, /* tcpwm.tr_compare_match[4] */
    TRIG0_IN_SCB0_TR_TX_REQ         = 0x00000022u, /* scb[0].tr_tx_req */
    TRIG0_IN_SCB0_TR_RX_REQ         = 0x00000023u, /* scb[0].tr_rx_req */
    TRIG0_IN_SCB1_TR_TX_REQ         = 0x00000024u, /* scb[1].tr_tx_req */
    TRIG0_IN_SCB1_TR_RX_REQ         = 0x00000025u, /* scb[1].tr_rx_req */
    TRIG0_IN_PACSS_TR_DATA_VAL0     = 0x00000026u, /* pacss.tr_data_val[0] */
    TRIG0_IN_PACSS_TR_DATA_VAL1     = 0x00000027u, /* pacss.tr_data_val[1] */
    TRIG0_IN_PACSS_TR_DATA_VAL2     = 0x00000028u, /* pacss.tr_data_val[2] */
    TRIG0_IN_PACSS_TR_DATA_VAL3     = 0x00000029u, /* pacss.tr_data_val[3] */
    TRIG0_IN_PERI_VIRT_IN_0         = 0x0000002Au, /* peri.virt_in_0 */
    TRIG0_IN_PERI_VIRT_IN_1         = 0x0000002Bu, /* peri.virt_in_1 */
    TRIG0_IN_PERI_VIRT_IN_2         = 0x0000002Cu, /* peri.virt_in_2 */
    TRIG0_IN_PERI_VIRT_IN_3         = 0x0000002Du, /* peri.virt_in_3 */
    TRIG0_IN_EXCO_TRIGGER           = 0x0000002Eu, /* exco.trigger */
    TRIG0_IN_CANFD_TR_FIFO00        = 0x0000002Fu, /* canfd.tr_fifo0[0] */
    TRIG0_IN_CANFD_TR_FIFO10        = 0x00000030u, /* canfd.tr_fifo1[0] */
    TRIG0_IN_CANFD_TR_DBG_DMA_REQ0  = 0x00000031u, /* canfd.tr_dbg_dma_req[0] */
    TRIG0_IN_ISOUART_TR_OUT0        = 0x00000032u, /* isouart.tr_out[0] */
    TRIG0_IN_ISOUART_TR_OUT1        = 0x00000033u, /* isouart.tr_out[1] */
    TRIG0_IN_ISOUART_TR_OUT2        = 0x00000034u, /* isouart.tr_out[2] */
    TRIG0_IN_ISOUART_TR_OUT3        = 0x00000035u, /* isouart.tr_out[3] */
    TRIG0_IN_ISOUART_TR_OUT4        = 0x00000036u, /* isouart.tr_out[4] */
    TRIG0_IN_ISOUART_TR_OUT5        = 0x00000037u, /* isouart.tr_out[5] */
    TRIG0_IN_ISOUART_TR_OUT6        = 0x00000038u, /* isouart.tr_out[6] */
    TRIG0_IN_ISOUART_TR_OUT7        = 0x00000039u, /* isouart.tr_out[7] */
    TRIG0_IN_ISOUART_TR_OUT8        = 0x0000003Au, /* isouart.tr_out[8] */
    TRIG0_IN_ISOUART_TR_OUT9        = 0x0000003Bu, /* isouart.tr_out[9] */
    TRIG0_IN_ISOUART_TR_OUT10       = 0x0000003Cu, /* isouart.tr_out[10] */
    TRIG0_IN_ISOUART_TR_OUT11       = 0x0000003Du, /* isouart.tr_out[11] */
    TRIG0_IN_ISOUART_TR_OUT12       = 0x0000003Eu, /* isouart.tr_out[12] */
    TRIG0_IN_ISOUART_TR_OUT13       = 0x0000003Fu, /* isouart.tr_out[13] */
    TRIG0_IN_ISOUART_TR_OUT14       = 0x00000040u, /* isouart.tr_out[14] */
    TRIG0_IN_ISOUART_TR_OUT15       = 0x00000041u /* isouart.tr_out[15] */
} en_trig_input_grp0_t;

/* Trigger Input Group 1 - Trigger sources for TCPWM */
typedef enum
{
    TRIG1_IN_CPUSS_ZERO             = 0x00000100u, /* cpuss.zero */
    TRIG1_IN_CPUSS_DMAC_TR_OUT0     = 0x00000101u, /* cpuss.dmac_tr_out[0] */
    TRIG1_IN_CPUSS_DMAC_TR_OUT1     = 0x00000102u, /* cpuss.dmac_tr_out[1] */
    TRIG1_IN_CPUSS_DMAC_TR_OUT2     = 0x00000103u, /* cpuss.dmac_tr_out[2] */
    TRIG1_IN_CPUSS_DMAC_TR_OUT3     = 0x00000104u, /* cpuss.dmac_tr_out[3] */
    TRIG1_IN_CPUSS_DMAC_TR_OUT4     = 0x00000105u, /* cpuss.dmac_tr_out[4] */
    TRIG1_IN_CPUSS_DMAC_TR_OUT5     = 0x00000106u, /* cpuss.dmac_tr_out[5] */
    TRIG1_IN_CPUSS_DMAC_TR_OUT6     = 0x00000107u, /* cpuss.dmac_tr_out[6] */
    TRIG1_IN_CPUSS_DMAC_TR_OUT7     = 0x00000108u, /* cpuss.dmac_tr_out[7] */
    TRIG1_IN_CPUSS_DMAC_TR_OUT8     = 0x00000109u, /* cpuss.dmac_tr_out[8] */
    TRIG1_IN_CPUSS_DMAC_TR_OUT9     = 0x0000010Au, /* cpuss.dmac_tr_out[9] */
    TRIG1_IN_CPUSS_DMAC_TR_OUT10    = 0x0000010Bu, /* cpuss.dmac_tr_out[10] */
    TRIG1_IN_CPUSS_DMAC_TR_OUT11    = 0x0000010Cu, /* cpuss.dmac_tr_out[11] */
    TRIG1_IN_CPUSS_DMAC_TR_OUT12    = 0x0000010Du, /* cpuss.dmac_tr_out[12] */
    TRIG1_IN_CPUSS_DMAC_TR_OUT13    = 0x0000010Eu, /* cpuss.dmac_tr_out[13] */
    TRIG1_IN_CPUSS_DMAC_TR_OUT14    = 0x0000010Fu, /* cpuss.dmac_tr_out[14] */
    TRIG1_IN_CPUSS_DMAC_TR_OUT15    = 0x00000110u, /* cpuss.dmac_tr_out[15] */
    TRIG1_IN_CPUSS_TR_FAULT0        = 0x00000111u, /* cpuss.tr_fault[0] */
    TRIG1_IN_CPUSS_TR_FAULT1        = 0x00000112u, /* cpuss.tr_fault[1] */
    TRIG1_IN_TCPWM_TR_OVERFLOW0     = 0x00000113u, /* tcpwm.tr_overflow[0] */
    TRIG1_IN_TCPWM_TR_OVERFLOW1     = 0x00000114u, /* tcpwm.tr_overflow[1] */
    TRIG1_IN_TCPWM_TR_OVERFLOW2     = 0x00000115u, /* tcpwm.tr_overflow[2] */
    TRIG1_IN_TCPWM_TR_OVERFLOW3     = 0x00000116u, /* tcpwm.tr_overflow[3] */
    TRIG1_IN_TCPWM_TR_OVERFLOW4     = 0x00000117u, /* tcpwm.tr_overflow[4] */
    TRIG1_IN_TCPWM_TR_UNDERFLOW0    = 0x00000118u, /* tcpwm.tr_underflow[0] */
    TRIG1_IN_TCPWM_TR_UNDERFLOW1    = 0x00000119u, /* tcpwm.tr_underflow[1] */
    TRIG1_IN_TCPWM_TR_UNDERFLOW2    = 0x0000011Au, /* tcpwm.tr_underflow[2] */
    TRIG1_IN_TCPWM_TR_UNDERFLOW3    = 0x0000011Bu, /* tcpwm.tr_underflow[3] */
    TRIG1_IN_TCPWM_TR_UNDERFLOW4    = 0x0000011Cu, /* tcpwm.tr_underflow[4] */
    TRIG1_IN_TCPWM_TR_COMPARE_MATCH0 = 0x0000011Du, /* tcpwm.tr_compare_match[0] */
    TRIG1_IN_TCPWM_TR_COMPARE_MATCH1 = 0x0000011Eu, /* tcpwm.tr_compare_match[1] */
    TRIG1_IN_TCPWM_TR_COMPARE_MATCH2 = 0x0000011Fu, /* tcpwm.tr_compare_match[2] */
    TRIG1_IN_TCPWM_TR_COMPARE_MATCH3 = 0x00000120u, /* tcpwm.tr_compare_match[3] */
    TRIG1_IN_TCPWM_TR_COMPARE_MATCH4 = 0x00000121u, /* tcpwm.tr_compare_match[4] */
    TRIG1_IN_SCB0_TR_TX_REQ         = 0x00000122u, /* scb[0].tr_tx_req */
    TRIG1_IN_SCB0_TR_RX_REQ         = 0x00000123u, /* scb[0].tr_rx_req */
    TRIG1_IN_SCB1_TR_TX_REQ         = 0x00000124u, /* scb[1].tr_tx_req */
    TRIG1_IN_SCB1_TR_RX_REQ         = 0x00000125u, /* scb[1].tr_rx_req */
    TRIG1_IN_PACSS_TR_DATA_VAL0     = 0x00000126u, /* pacss.tr_data_val[0] */
    TRIG1_IN_PACSS_TR_DATA_VAL1     = 0x00000127u, /* pacss.tr_data_val[1] */
    TRIG1_IN_PACSS_TR_DATA_VAL2     = 0x00000128u, /* pacss.tr_data_val[2] */
    TRIG1_IN_PACSS_TR_DATA_VAL3     = 0x00000129u, /* pacss.tr_data_val[3] */
    TRIG1_IN_PERI_VIRT_IN_0         = 0x0000012Au, /* peri.virt_in_0 */
    TRIG1_IN_PERI_VIRT_IN_1         = 0x0000012Bu, /* peri.virt_in_1 */
    TRIG1_IN_PERI_VIRT_IN_2         = 0x0000012Cu, /* peri.virt_in_2 */
    TRIG1_IN_PERI_VIRT_IN_3         = 0x0000012Du, /* peri.virt_in_3 */
    TRIG1_IN_CANFD_TR_FIFO00        = 0x0000012Eu, /* canfd.tr_fifo0[0] */
    TRIG1_IN_CANFD_TR_FIFO10        = 0x0000012Fu, /* canfd.tr_fifo1[0] */
    TRIG1_IN_CANFD_TR_TMP_RTP_OUT0  = 0x00000130u, /* canfd.tr_tmp_rtp_out[0] */
    TRIG1_IN_ISOUART_TR_OUT0        = 0x00000131u, /* isouart.tr_out[0] */
    TRIG1_IN_ISOUART_TR_OUT1        = 0x00000132u, /* isouart.tr_out[1] */
    TRIG1_IN_ISOUART_TR_OUT2        = 0x00000133u, /* isouart.tr_out[2] */
    TRIG1_IN_ISOUART_TR_OUT3        = 0x00000134u, /* isouart.tr_out[3] */
    TRIG1_IN_ISOUART_TR_OUT4        = 0x00000135u, /* isouart.tr_out[4] */
    TRIG1_IN_ISOUART_TR_OUT5        = 0x00000136u, /* isouart.tr_out[5] */
    TRIG1_IN_ISOUART_TR_OUT6        = 0x00000137u, /* isouart.tr_out[6] */
    TRIG1_IN_ISOUART_TR_OUT7        = 0x00000138u, /* isouart.tr_out[7] */
    TRIG1_IN_ISOUART_TR_OUT8        = 0x00000139u, /* isouart.tr_out[8] */
    TRIG1_IN_ISOUART_TR_OUT9        = 0x0000013Au, /* isouart.tr_out[9] */
    TRIG1_IN_ISOUART_TR_OUT10       = 0x0000013Bu, /* isouart.tr_out[10] */
    TRIG1_IN_ISOUART_TR_OUT11       = 0x0000013Cu, /* isouart.tr_out[11] */
    TRIG1_IN_ISOUART_TR_OUT12       = 0x0000013Du, /* isouart.tr_out[12] */
    TRIG1_IN_ISOUART_TR_OUT13       = 0x0000013Eu, /* isouart.tr_out[13] */
    TRIG1_IN_ISOUART_TR_OUT14       = 0x0000013Fu, /* isouart.tr_out[14] */
    TRIG1_IN_ISOUART_TR_OUT15       = 0x00000140u /* isouart.tr_out[15] */
} en_trig_input_grp1_t;

/* Trigger Input Group 2 - Trigger sources for PACSS conversions */
typedef enum
{
    TRIG2_IN_CPUSS_ZERO             = 0x00000200u, /* cpuss.zero */
    TRIG2_IN_TCPWM_TR_OVERFLOW0     = 0x00000201u, /* tcpwm.tr_overflow[0] */
    TRIG2_IN_TCPWM_TR_OVERFLOW1     = 0x00000202u, /* tcpwm.tr_overflow[1] */
    TRIG2_IN_TCPWM_TR_OVERFLOW2     = 0x00000203u, /* tcpwm.tr_overflow[2] */
    TRIG2_IN_TCPWM_TR_OVERFLOW3     = 0x00000204u, /* tcpwm.tr_overflow[3] */
    TRIG2_IN_TCPWM_TR_OVERFLOW4     = 0x00000205u, /* tcpwm.tr_overflow[4] */
    TRIG2_IN_TCPWM_TR_UNDERFLOW0    = 0x00000206u, /* tcpwm.tr_underflow[0] */
    TRIG2_IN_TCPWM_TR_UNDERFLOW1    = 0x00000207u, /* tcpwm.tr_underflow[1] */
    TRIG2_IN_TCPWM_TR_UNDERFLOW2    = 0x00000208u, /* tcpwm.tr_underflow[2] */
    TRIG2_IN_TCPWM_TR_UNDERFLOW3    = 0x00000209u, /* tcpwm.tr_underflow[3] */
    TRIG2_IN_TCPWM_TR_UNDERFLOW4    = 0x0000020Au, /* tcpwm.tr_underflow[4] */
    TRIG2_IN_TCPWM_TR_COMPARE_MATCH0 = 0x0000020Bu, /* tcpwm.tr_compare_match[0] */
    TRIG2_IN_TCPWM_TR_COMPARE_MATCH1 = 0x0000020Cu, /* tcpwm.tr_compare_match[1] */
    TRIG2_IN_TCPWM_TR_COMPARE_MATCH2 = 0x0000020Du, /* tcpwm.tr_compare_match[2] */
    TRIG2_IN_TCPWM_TR_COMPARE_MATCH3 = 0x0000020Eu, /* tcpwm.tr_compare_match[3] */
    TRIG2_IN_TCPWM_TR_COMPARE_MATCH4 = 0x0000020Fu, /* tcpwm.tr_compare_match[4] */
    TRIG2_IN_PACSS_TR_DATA_VAL0     = 0x00000210u, /* pacss.tr_data_val[0] */
    TRIG2_IN_PACSS_TR_DATA_VAL1     = 0x00000211u, /* pacss.tr_data_val[1] */
    TRIG2_IN_PACSS_TR_DATA_VAL2     = 0x00000212u, /* pacss.tr_data_val[2] */
    TRIG2_IN_PACSS_TR_DATA_VAL3     = 0x00000213u, /* pacss.tr_data_val[3] */
    TRIG2_IN_PERI_VIRT_IN_0         = 0x00000214u, /* peri.virt_in_0 */
    TRIG2_IN_PERI_VIRT_IN_1         = 0x00000215u, /* peri.virt_in_1 */
    TRIG2_IN_PERI_VIRT_IN_2         = 0x00000216u, /* peri.virt_in_2 */
    TRIG2_IN_PERI_VIRT_IN_3         = 0x00000217u, /* peri.virt_in_3 */
    TRIG2_IN_ISOUART_TR_OUT0        = 0x00000218u, /* isouart.tr_out[0] */
    TRIG2_IN_ISOUART_TR_OUT1        = 0x00000219u, /* isouart.tr_out[1] */
    TRIG2_IN_ISOUART_TR_OUT2        = 0x0000021Au, /* isouart.tr_out[2] */
    TRIG2_IN_ISOUART_TR_OUT3        = 0x0000021Bu, /* isouart.tr_out[3] */
    TRIG2_IN_ISOUART_TR_OUT4        = 0x0000021Cu, /* isouart.tr_out[4] */
    TRIG2_IN_ISOUART_TR_OUT5        = 0x0000021Du, /* isouart.tr_out[5] */
    TRIG2_IN_ISOUART_TR_OUT6        = 0x0000021Eu, /* isouart.tr_out[6] */
    TRIG2_IN_ISOUART_TR_OUT7        = 0x0000021Fu, /* isouart.tr_out[7] */
    TRIG2_IN_ISOUART_TR_OUT8        = 0x00000220u, /* isouart.tr_out[8] */
    TRIG2_IN_ISOUART_TR_OUT9        = 0x00000221u, /* isouart.tr_out[9] */
    TRIG2_IN_ISOUART_TR_OUT10       = 0x00000222u, /* isouart.tr_out[10] */
    TRIG2_IN_ISOUART_TR_OUT11       = 0x00000223u, /* isouart.tr_out[11] */
    TRIG2_IN_ISOUART_TR_OUT12       = 0x00000224u, /* isouart.tr_out[12] */
    TRIG2_IN_ISOUART_TR_OUT13       = 0x00000225u, /* isouart.tr_out[13] */
    TRIG2_IN_ISOUART_TR_OUT14       = 0x00000226u, /* isouart.tr_out[14] */
    TRIG2_IN_ISOUART_TR_OUT15       = 0x00000227u /* isouart.tr_out[15] */
} en_trig_input_grp2_t;

/* Trigger Input Group 3 - Trigger sources for PACSS conversions */
typedef enum
{
    TRIG3_IN_CPUSS_ZERO             = 0x00000300u, /* cpuss.zero */
    TRIG3_IN_TCPWM_TR_COMPARE_MATCH0 = 0x00000301u, /* tcpwm.tr_compare_match[0] */
    TRIG3_IN_TCPWM_TR_COMPARE_MATCH1 = 0x00000302u, /* tcpwm.tr_compare_match[1] */
    TRIG3_IN_TCPWM_TR_COMPARE_MATCH2 = 0x00000303u, /* tcpwm.tr_compare_match[2] */
    TRIG3_IN_TCPWM_TR_COMPARE_MATCH3 = 0x00000304u, /* tcpwm.tr_compare_match[3] */
    TRIG3_IN_TCPWM_TR_COMPARE_MATCH4 = 0x00000305u, /* tcpwm.tr_compare_match[4] */
    TRIG3_IN_PERI_VIRT_IN_0         = 0x00000306u, /* peri.virt_in_0 */
    TRIG3_IN_PERI_VIRT_IN_1         = 0x00000307u, /* peri.virt_in_1 */
    TRIG3_IN_PERI_VIRT_IN_2         = 0x00000308u, /* peri.virt_in_2 */
    TRIG3_IN_PERI_VIRT_IN_3         = 0x00000309u, /* peri.virt_in_3 */
    TRIG3_IN_PACSS_TR_DATA_VAL0     = 0x0000030Au, /* pacss.tr_data_val[0] */
    TRIG3_IN_PACSS_TR_DATA_VAL1     = 0x0000030Bu, /* pacss.tr_data_val[1] */
    TRIG3_IN_PACSS_TR_DATA_VAL2     = 0x0000030Cu, /* pacss.tr_data_val[2] */
    TRIG3_IN_PACSS_TR_DATA_VAL3     = 0x0000030Du /* pacss.tr_data_val[3] */
} en_trig_input_grp3_t;

/* Trigger Input Group 4 - Trigger sources for PACSS conversions */
typedef enum
{
    TRIG4_IN_CPUSS_ZERO             = 0x00000400u, /* cpuss.zero */
    TRIG4_IN_TCPWM_TR_COMPARE_MATCH0 = 0x00000401u, /* tcpwm.tr_compare_match[0] */
    TRIG4_IN_TCPWM_TR_COMPARE_MATCH1 = 0x00000402u, /* tcpwm.tr_compare_match[1] */
    TRIG4_IN_TCPWM_TR_COMPARE_MATCH2 = 0x00000403u, /* tcpwm.tr_compare_match[2] */
    TRIG4_IN_TCPWM_TR_COMPARE_MATCH3 = 0x00000404u, /* tcpwm.tr_compare_match[3] */
    TRIG4_IN_TCPWM_TR_COMPARE_MATCH4 = 0x00000405u, /* tcpwm.tr_compare_match[4] */
    TRIG4_IN_PERI_VIRT_IN_0         = 0x00000406u, /* peri.virt_in_0 */
    TRIG4_IN_PERI_VIRT_IN_1         = 0x00000407u, /* peri.virt_in_1 */
    TRIG4_IN_PERI_VIRT_IN_2         = 0x00000408u, /* peri.virt_in_2 */
    TRIG4_IN_PERI_VIRT_IN_3         = 0x00000409u /* peri.virt_in_3 */
} en_trig_input_grp4_t;

/* Trigger Input Group 5 - Trigger sources for OCD conversions */
typedef enum
{
    TRIG5_IN_CPUSS_ZERO             = 0x00000500u, /* cpuss.zero */
    TRIG5_IN_PACSS_TR_OCD0          = 0x00000501u /* pacss.tr_ocd[0] */
} en_trig_input_grp5_t;

/* Trigger Input Group 6 - Trigger sources for OCD conversions */
typedef enum
{
    TRIG6_IN_CPUSS_ZERO             = 0x00000600u, /* cpuss.zero */
    TRIG6_IN_PACSS_TR_OCD1          = 0x00000601u /* pacss.tr_ocd[1] */
} en_trig_input_grp6_t;

/* Trigger Group Outputs */
/* Trigger Output Group 0 - DMA Request Assignments */
typedef enum
{
    TRIG0_OUT_CPUSS_DMAC_TR_IN0     = 0x40000000u, /* cpuss.dmac_tr_in[0] */
    TRIG0_OUT_CPUSS_DMAC_TR_IN1     = 0x40000001u, /* cpuss.dmac_tr_in[1] */
    TRIG0_OUT_CPUSS_DMAC_TR_IN2     = 0x40000002u, /* cpuss.dmac_tr_in[2] */
    TRIG0_OUT_CPUSS_DMAC_TR_IN3     = 0x40000003u, /* cpuss.dmac_tr_in[3] */
    TRIG0_OUT_CPUSS_DMAC_TR_IN4     = 0x40000004u, /* cpuss.dmac_tr_in[4] */
    TRIG0_OUT_CPUSS_DMAC_TR_IN5     = 0x40000005u, /* cpuss.dmac_tr_in[5] */
    TRIG0_OUT_CPUSS_DMAC_TR_IN6     = 0x40000006u, /* cpuss.dmac_tr_in[6] */
    TRIG0_OUT_CPUSS_DMAC_TR_IN7     = 0x40000007u, /* cpuss.dmac_tr_in[7] */
    TRIG0_OUT_CPUSS_DMAC_TR_IN8     = 0x40000008u, /* cpuss.dmac_tr_in[8] */
    TRIG0_OUT_CPUSS_DMAC_TR_IN9     = 0x40000009u, /* cpuss.dmac_tr_in[9] */
    TRIG0_OUT_CPUSS_DMAC_TR_IN10    = 0x4000000Au, /* cpuss.dmac_tr_in[10] */
    TRIG0_OUT_CPUSS_DMAC_TR_IN11    = 0x4000000Bu, /* cpuss.dmac_tr_in[11] */
    TRIG0_OUT_CPUSS_DMAC_TR_IN12    = 0x4000000Cu, /* cpuss.dmac_tr_in[12] */
    TRIG0_OUT_CPUSS_DMAC_TR_IN13    = 0x4000000Du, /* cpuss.dmac_tr_in[13] */
    TRIG0_OUT_CPUSS_DMAC_TR_IN14    = 0x4000000Eu, /* cpuss.dmac_tr_in[14] */
    TRIG0_OUT_CPUSS_DMAC_TR_IN15    = 0x4000000Fu, /* cpuss.dmac_tr_in[15] */
    TRIG0_OUT_CANFD_TR_EVT_SWT_IN0  = 0x40000010u, /* canfd.tr_evt_swt_in[0] */
    TRIG0_OUT_CANFD_TR_DBG_DMA_ACK0 = 0x40000011u /* canfd.tr_dbg_dma_ack[0] */
} en_trig_output_grp0_t;

/* Trigger Output Group 1 - Trigger sources for TCPWM */
typedef enum
{
    TRIG1_OUT_TCPWM_TR_IN5          = 0x40000100u, /* tcpwm.tr_in[5] */
    TRIG1_OUT_TCPWM_TR_IN6          = 0x40000101u, /* tcpwm.tr_in[6] */
    TRIG1_OUT_TCPWM_TR_IN7          = 0x40000102u, /* tcpwm.tr_in[7] */
    TRIG1_OUT_TCPWM_TR_IN8          = 0x40000103u, /* tcpwm.tr_in[8] */
    TRIG1_OUT_TCPWM_TR_IN9          = 0x40000104u, /* tcpwm.tr_in[9] */
    TRIG1_OUT_TCPWM_TR_IN10         = 0x40000105u, /* tcpwm.tr_in[10] */
    TRIG1_OUT_TCPWM_TR_IN11         = 0x40000106u /* tcpwm.tr_in[11] */
} en_trig_output_grp1_t;

/* Trigger Output Group 2 - Trigger sources for PACSS conversions */
typedef enum
{
    TRIG2_OUT_PACSS_TR_START0       = 0x40000200u, /* pacss.tr_start[0] */
    TRIG2_OUT_PACSS_TR_START1       = 0x40000201u, /* pacss.tr_start[1] */
    TRIG2_OUT_PACSS_TR_START2       = 0x40000202u, /* pacss.tr_start[2] */
    TRIG2_OUT_PACSS_TR_START3       = 0x40000203u /* pacss.tr_start[3] */
} en_trig_output_grp2_t;

/* Trigger Output Group 3 - Trigger sources for PACSS conversions */
typedef enum
{
    TRIG3_OUT_ISOUART_TR_EMM_IN0    = 0x40000300u, /* isouart.tr_emm_in[0] */
    TRIG3_OUT_ISOUART_TR_EMM_IN1    = 0x40000301u /* isouart.tr_emm_in[1] */
} en_trig_output_grp3_t;

/* Trigger Output Group 4 - Trigger sources for PACSS conversions */
typedef enum
{
    TRIG4_OUT_LIN_TR_CMD_TX_HEADER0 = 0x40000400u, /* lin.tr_cmd_tx_header[0] */
    TRIG4_OUT_LIN_TR_CMD_TX_HEADER1 = 0x40000401u /* lin.tr_cmd_tx_header[1] */
} en_trig_output_grp4_t;

/* Trigger Output Group 5 - Trigger sources for OCD conversions */
typedef enum
{
    TRIG5_OUT_TCPWM_TR_IN12         = 0x40000500u /* tcpwm.tr_in[12] */
} en_trig_output_grp5_t;

/* Trigger Output Group 6 - Trigger sources for OCD conversions */
typedef enum
{
    TRIG6_OUT_TCPWM_TR_IN13         = 0x40000600u /* tcpwm.tr_in[13] */
} en_trig_output_grp6_t;

/* Fault connections */
typedef enum
{
    CPUSS_FAULT_RAMC_C_ECC          = 0x0000u,
    CPUSS_FAULT_RAMC_NC_ECC         = 0x0001u,
    CPUSS_FAULT_FLASHC_C_ECC        = 0x0004u,
    CPUSS_FAULT_FLASHC_NC_ECC       = 0x0005u,
    CPUSS_FAULT_FLASHC_BUS_ERR      = 0x0006u,
    CPUSS_FAULT_FLASHC1_C_ECC       = 0x0007u,
    CPUSS_FAULT_FLASHC1_NC_ECC      = 0x0008u,
    CPUSS_FAULT_FLASHC1_BUS_ERR     = 0x0009u,
    SRSS_FAULT_CRWDT                = 0x000Au,
    HVSS_FAULT_PWR                  = 0x000Bu,
    ISOUART_FAULT_ECC               = 0x000Cu,
    CY_SYSFAULT_NO_FAULT            = 0x000Du
} en_sysfault_source_t;

/* Include IP definitions */
#include "ip/cyip_sflash_psoc4hvpaspm10.h"
#include "ip/cyip_peri.h"
#include "ip/cyip_hsiom.h"
#include "ip/cyip_srsshv.h"
#include "ip/cyip_gpio.h"
#include "ip/cyip_hvss_v2.h"
#include "ip/cyip_crypto_v2.h"
#include "ip/cyip_exco_v2.h"
#include "ip/cyip_cpuss_v3_ver3.h"
#include "ip/cyip_dmac_v3_ver3.h"
#include "ip/cyip_spcif_v3_ver3.h"
#include "ip/cyip_spcif1_v3_ver3.h"
#include "ip/cyip_fault_v3.h"
#include "ip/cyip_tcpwm_v2.h"
#include "ip/cyip_canfd.h"
#include "ip/cyip_scb_v2.h"
#include "ip/cyip_lin.h"
#include "ip/cyip_isouart.h"
#include "ip/cyip_pacss.h"

/* Parameter Defines */
/* Number of TTCAN instances */
#define CANFD_CAN_NR                    1u
/* ECC logic present or not */
#define CANFD_ECC_PRESENT               0u
/* address included in ECC logic or not */
#define CANFD_ECC_ADDR_PRESENT          0u
/* Time Stamp counter present or not (required for instance 0, otherwise not
   allowed) */
#define CANFD_TS_PRESENT                1u
/* Message RAM size in KB */
#define CANFD_MRAM_SIZE                 4u
/* Message RAM address width */
#define CANFD_MRAM_ADDR_WIDTH           10u
/* CPUSS version (2 or 3) */
#define CPUSS_CPUSS_VER                 3u
/* CM0 present or CM0+ present (1=CM0, 0=CM0+) */
#define CPUSS_CM0_PRESENT               0u
/* CM0+ Memory protection unit present/not (0=not present, 8=present). */
#define CPUSS_CM0_MPU                   8u
/* CM0+ Micro Trace Buffer (MTB) Present or not (Must be '0' for CM0) */
#define CPUSS_MTB_PRESENT               0u
/* MTB SRAM size in kilo bytes (0, 1, 2, or 4). Must be '0' for CM0, Non zero
   value only for CM0+. */
#define CPUSS_MTB_SRAM_SIZE             0u
/* Maximum speed that system will run at (0=48MHz, 1=16MHz) */
#define CPUSS_MAX_16_MHZ                0u
/* System RAM parition 0, size in kilobytes */
#define CPUSS_SRAM_SIZE                 8u
/* System RAM parition 1, size in kilobytes */
#define CPUSS_SRAM1_SIZE                0u
/* Total ROM size in kilobytes */
#define CPUSS_ROM_SIZE                  32u
/* Product has ROM available for system usage (1) or not (0) */
#define CPUSS_SYSTEM_ROM                1u
/* Boot ROM partition size in kilobytes in multiple of 2^n (2,4,8,16,32,64) */
#define CPUSS_BOOT_ROM_SIZE             16u
/* Boot ROM available (1) or all classified as System Rom (0) */
#define CPUSS_ROMC_BOOT_ROM_PRESENT     1u
/* Number of external slave ports on System Interconnect */
#define CPUSS_SL_NR                     3u
/* Define Erased Value of Flash Controller 0 */
#define CPUSS_FLASHC_ERASE_STATE        0u
/* Define Erased Value of Flash Controller 1 */
#define CPUSS_FLASHC_1_ERASE_STATE      0u
/* Flash size in kilobytes */
#define CPUSS_FLASH_SIZE                128u
/* Flash memory present or not (1=Flash present, 0=Flash not present) */
#define CPUSS_FLASHC_PRESENT            1u
/* FLASH has ECC and fault system */
#define CPUSS_FLASHC_PRESENT_WITH_ECC   1u
/* Flash size in kilobytes for 2nd Flash Controller */
#define CPUSS_FLASHC1_SIZE              16u
/* Flash memory Controller 1 present or not (1=Flash present, 0=Flash not present) */
#define CPUSS_FLASHC1_PRESENT           1u
/* FLASH has ECC and fault system */
#define CPUSS_FLASHC1_PRESENT_WITH_ECC  1u
/* Debug support (0=No, 1=Yes) */
#define CPUSS_CM0_DBG                   1u
/* Number of break-point comparators ([0,4]) */
#define CPUSS_CM0_BKPT                  4u
/* Number of watch-point comparators ([0,2]) */
#define CPUSS_CM0_WPT                   2u
/* Serial multiplier (1) or parallel multiplier (0) */
#define CPUSS_CM0_SMUL                  0u
/* Clock Source clk_lf implemented in SysTick Counter. When 0, not implemented,
   1=implemented */
#define CPUSS_CM0_SYST_CLKSOURCE_LF     1u
/* Number of interrupt request inputs to CM0 ([8,32]) - called CM0_NUMIRQ in
   design */
#define CPUSS_INT_NR                    32u
/* Number of wakeup interrupt controller lines (always includes NMI, RXEV, so
   min=2) */
#define CPUSS_CM0_WICLINES              12u
/* Indicates whether DSI/FixedFunction interrupt multiplexer is present (0=No,
   1=Yes) */
#define CPUSS_INT_DSI                   0u
/* Implement DSI interrupt sync and pulse generation (1) or not (0) */
#define CPUSS_INT_DSI_SYNC              0u
/* Wounding supported (1) or not supported (0) - must be 1 currently */
#define CPUSS_WOUNDING                  1u
/* Protection modes supported (1) or not supported (0) - must be 1 currently */
#define CPUSS_PROTECTION                1u
/* DW/DMA Controller present (0=No, 1=Yes) */
#define CPUSS_DMAC_PRESENT              1u
/* Product is a multi-master system (same as CPUMEMSS.DMAC_PRESENT) */
#define CPUSS_MULTI_MASTER              1u
/* PTM interface present (0=No, 1=Yes) */
#define CPUSS_PTM_PRESENT               0u
/* Width of the PTM interface in bits ([2,32]) */
#define CPUSS_PTM_NR                    0u
/* Implement full (8-step) RAM BIST (1) or simple (1-step) RAM BIST (0) */
#define CPUSS_BIST_FULL                 1u
/* Number of external SRAMs connected to the CPUSS BIST controller (0 to 12) */
#define CPUSS_BIST_EXT_SRAM_NR          9u
/* Number of SRAMs connected to the CPUSS BIST controller (BIST_EXT_SRAM_NR+4) */
#define CPUSS_BIST_SRAM_NR              13u
/* CoreSight Part Identification Number */
#define CPUSS_JEPID                     52u
/* CoreSight Part Identification Number */
#define CPUSS_JEPCONTINUATION           0u
/* CoreSight Part Identification Number */
#define CPUSS_PARTNUMBER                199u
/* External Master Present */
#define CPUSS_EXT_MS_PRESENT            0u
/* External Master M1 Present (Note either DMA_PRESENT or EXT_MS_PRESENT must also
   be set) */
#define CPUSS_EXT_MS1_PRESENT           0u
/* RAM controller 1 present (0=No, 1=Yes) */
#define CPUSS_RAMC1_PRESENT             0u
/* Enforce Secure Access restrictions (0=No, 1=Yes) */
#define CPUSS_SECURE_ACCESS             1u
/* SWD MultiDrop Present */
#define CPUSS_SWD_MULTIDROP             0u
/* Enable SROM DMA Access */
#define CPUSS_SROM_ACCESS_PRESENT       0u
/* Enable ECC for RAMC (SRAM Controller 0) */
#define CPUSS_RAMC_ECC_PRESENT          1u
/* Is the Address included in ECC calculation, impacts timing (SRAM Controller 0) */
#define CPUSS_RAMC_ECC_ADDR_PRESENT     1u
/* Enable ECC for RAMC1 (SRAM Controller 1) */
#define CPUSS_RAMC1_ECC_PRESENT         0u
/* Is the Address included in ECC calculation, impacts timing (SRAM Controller 0) */
#define CPUSS_RAMC1_ECC_ADDR_PRESENT    0u
/* Vector Table Relocation ROM Offset (In KB. Supported Values are 4/8/16/32 KB) */
#define CPUSS_VECT_TABLE_ROM_OFFSET     4u
/* Fault logging system present */
#define CPUSS_FAULT_PRESENT             1u
/* Fault start address in MMIO address space */
#define CPUSS_FAULT_ADDR                0x00030000u
/* Mask for Fault MMIO */
#define CPUSS_FAULT_MASK                0x00001FF3u
/* Enables Fault Report by Fault structure for SRAM/Flash (0=Use Interrupts, 1=Use
   Fault signaling) */
#define CPUSS_ECC_REPORT_BY_FAULT       1u
/* Inverse value of ECC_REPORT_BY_FAULT */
#define CPUSS_ECC_REPORT_BY_INTR        0u
/* 0=ULL65, 1=MXS40-ULP, 2=MXS40E, 3=M0S8, 4=MXS40Sv2 -HD, 5=F45, 6=MXS40Sv2, ,
   7=t28hpm, 8=t28hpl, 9=t28hpc */
#define CPUSS_PLATFORM_VARIANT          3u
/* 0=IFX (Infineon), 1=SNPS (Synopsys), 2=ARM,3=Broadcom, 4=TSMC */
#define CPUSS_RAM_VEND                  0u
/* Select Cell VT type. Drives the type of standard cell to be used in MXTK. (0 =
   scs8hd, 1 = scs8hdll, 2 = scs8hv) */
#define CPUSS_CELL_VT_TYPE              0u
/* ECC enabled for all FLASH */
#define CPUSS_CHIP_TOP_FLASH_ECC        1u
/* Flash block size (per macro). */
#define CPUSS_CHIP_TOP_FLASH_BLOCK_SIZE 64u
/* Flash ECC enabled with no fault system */
#define CPUSS_CHIP_TOP_FLASHC_ECC_INT   0u
/* Flash block size (per macro). */
#define CPUSS_CHIP_TOP_FLASHC1_BLOCK_SIZE 16u
/* Flash ECC enabled with no fault system, controller 1 */
#define CPUSS_CHIP_TOP_FLASHC1_ECC_INT  0u
/* Enable ECC for RAMC with no fault system (SRAM Controller 0) */
#define CPUSS_CHIP_TOP_RAMC_ECC_INT     0u
/* Enable ECC for RAMC1 with no fault system (SRAM Controller 1) */
#define CPUSS_CHIP_TOP_RAMC1_ECC_INT    0u
/* MTB SRAM Base adress - Value should be computed from Memory Map */
#define CPUSS_CM0PMTB_MTB_SRAMBASE      0u
/* Flash memory present or not (1=Flash present, 0=Flash not present) */
#define CPUSS_CPUMEMSS_FLASHC_PRESENT   1u
/* Flash memory Controller 1 present or not (1=Flash present, 0=Flash not present) */
#define CPUSS_CPUMEMSS_FLASHC1_PRESENT  1u
/* DW/DMA Controller present (0=No, 1=Yes) */
#define CPUSS_CPUMEMSS_DMAC_PRESENT     1u
/* Fault logging system present */
#define CPUSS_CPUMEMSS_FAULT_PRESENT    1u
/* Number of DMA Channels (8, 16, or 32) */
#define CPUSS_DMAC_CH_NR                16u
/* Width in bits of a DMA channel number (derived from CH_NR as
   roundup(log2(CH_NR))) */
#define CPUSS_DMAC_CH_ADDR_WIDTH        4u
/* Number of fault structures. Legal range [1, 4] */
#define CPUSS_FAULT_FAULT_NR            2u
/* Flash memory present or not (1=Flash present, 0=Flash not present) */
#define CPUSS_SPCIF_FLASHC_PRESENT      1u
/* Flash memory present with no ECC */
#define CPUSS_SPCIF_FLASHC_PRESENT_NO_ECC 0u
/* FLASH has ECC and fault system */
#define CPUSS_SPCIF_FLASHC_PRESENT_WITH_ECC 1u
/* Enables Fault Report by Fault structure for SRAM/Flash (0=Use Interrupts, 1=Use
   Fault signaling) */
#define CPUSS_SPCIF_ECC_REPORT_BY_FAULT 1u
/* Flash type (0=FM, 1=FMLT, 2=FS, 3=FSLT) */
#define CPUSS_SPCIF_FLASH_TYPE          3u
/* Use 2X?? Option for FLASH to allow parallel code execution and programming of
   FLASH */
#define CPUSS_SPCIF_FLASH_PARALLEL_PGM_EN 1u
/* HOBTO-M ULL65 SRAM/SROM required */
#define CPUSS_SPCIF_ULL65               0u
/* Product uses FLASH-Lite (1) or regular FLASH (0) */
#define CPUSS_SPCIF_FMLT                0u
/* Flash type being used (0=Lite, 1=Regular) */
#define CPUSS_SPCIF_FM                  0u
/* Flash type is FMLT or S8FS */
#define CPUSS_SPCIF_FMLT_OR_S8FS        1u
/* S8FS_ver2 */
#define CPUSS_SPCIF_FLASH_S8FS_VER2     1u
/* Not S8FS_ver2 */
#define CPUSS_SPCIF_FLASH_NOT_S8FS_VER2 0u
/* Total Flash size in multiples of 256 bytes */
#define CPUSS_SPCIF_FLASH_SIZE          511u
/* The Flash read word width in bits (32, 64 or 128) */
#define CPUSS_SPCIF_FLASH_READ_WIDTH    64u
/* Total Supervisory Flash size in multiples of 256 bytes (0=1x256, 1=2x256, ...) */
#define CPUSS_SPCIF_SFLASH_SIZE         7u
/* Flash program/erase page size (64, 128 or 256 bytes) */
#define CPUSS_SPCIF_FLASH_PAGE_SIZE     128u
/* Number of Flash macros used in the device (1, 2, 3 or 4) */
#define CPUSS_SPCIF_FLASH_MACROS        2u
/* Number of Flash macros minus 1 */
#define CPUSS_SPCIF_FLASH_MACROS_MINUS1 1u
/* Flash macro #1 (the second) present (0=No, 1=Yes) */
#define CPUSS_SPCIF_FLASH_MACRO_1       1u
/* Flash macro #2 (the third) present (0=No, 1=Yes) */
#define CPUSS_SPCIF_FLASH_MACRO_2       0u
/* Flash macro #3 (the fourth) present (0=No, 1=Yes) */
#define CPUSS_SPCIF_FLASH_MACRO_3       0u
/* S8FS_ver2 & Flash Macro 1 */
#define CPUSS_SPCIF_FLASH_S8FS_VER2_M1  1u
/* S8FS_ver2 & Flash Macro 2 */
#define CPUSS_SPCIF_FLASH_S8FS_VER2_M2  0u
/* S8FS_ver2 & Flash Macro 3 */
#define CPUSS_SPCIF_FLASH_S8FS_VER2_M3  0u
/* Not S8FS_ver2 & Flash Macro 1 */
#define CPUSS_SPCIF_FLASH_NOT_S8FS_VER2_M1 0u
/* Not S8FS_ver2 & Flash Macro 2 */
#define CPUSS_SPCIF_FLASH_NOT_S8FS_VER2_M2 0u
/* Not S8FS_ver2 & Flash Macro 3 */
#define CPUSS_SPCIF_FLASH_NOT_S8FS_VER2_M3 0u
/* SPCIF Timer clock is synchronous (1) or asynchronous (0) to clk_sys */
#define CPUSS_SPCIF_SYNCHRONOUS         1u
/* Product uses SRSS-Lite (1) or SRSSv2 (0) - used to determine NVL parameters */
#define CPUSS_SPCIF_SRSSLT              1u
/* NVLatches present (0=No, 1=Yes) */
#define CPUSS_SPCIF_NVLATCH             0u
/* NVLatches size in bytes (default=8) */
#define CPUSS_SPCIF_NVLATCH_SIZE        8u
/* Inverse value of ECC_REPORT_BY_FAULT */
#define CPUSS_SPCIF_ECC_REPORT_BY_INTR  0u
/* Flash memory present or not (1=Flash present, 0=Flash not present) */
#define CPUSS_SPCIF1_FLASHC_PRESENT     1u
/* Flash memory present with no ECC */
#define CPUSS_SPCIF1_FLASHC_PRESENT_NO_ECC 0u
/* FLASH has ECC and fault system */
#define CPUSS_SPCIF1_FLASHC_PRESENT_WITH_ECC 1u
/* Enables Fault Report by Fault structure for SRAM/Flash (0=Use Interrupts, 1=Use
   Fault signaling) */
#define CPUSS_SPCIF1_ECC_REPORT_BY_FAULT 1u
/* Flash type (0=FM, 1=FMLT, 2=FS, 3=FSLT) */
#define CPUSS_SPCIF1_FLASH_TYPE         3u
/* Use 2X?? Option for FLASH to allow parallel code execution and programming of
   FLASH */
#define CPUSS_SPCIF1_FLASH_PARALLEL_PGM_EN 0u
/* HOBTO-M ULL65 SRAM/SROM required */
#define CPUSS_SPCIF1_ULL65              0u
/* Product uses FLASH-Lite (1) or regular FLASH (0) */
#define CPUSS_SPCIF1_FMLT               0u
/* Flash type being used (0=Lite, 1=Regular) */
#define CPUSS_SPCIF1_FM                 0u
/* Flash type is FMLT or S8FS */
#define CPUSS_SPCIF1_FMLT_OR_S8FS       1u
/* S8FS_ver2 */
#define CPUSS_SPCIF1_FLASH_S8FS_VER2    1u
/* Not S8FS_ver2 */
#define CPUSS_SPCIF1_FLASH_NOT_S8FS_VER2 0u
/* Total Flash size in multiples of 256 bytes */
#define CPUSS_SPCIF1_FLASH_SIZE         63u
/* The Flash read word width in bits (32, 64 or 128) */
#define CPUSS_SPCIF1_FLASH_READ_WIDTH   64u
/* Total Supervisory Flash size in multiples of 256 bytes */
#define CPUSS_SPCIF1_SFLASH_SIZE        3u
/* Flash program/erase page size (64, 128 or 256 bytes) */
#define CPUSS_SPCIF1_FLASH_PAGE_SIZE    128u
/* Number of Flash macros used in the device (1, 2, 3 or 4) */
#define CPUSS_SPCIF1_FLASH_MACROS       1u
/* Number of Flash macros minus 1 */
#define CPUSS_SPCIF1_FLASH_MACROS_MINUS1 0u
/* Flash macro #1 (the second) present (0=No, 1=Yes) */
#define CPUSS_SPCIF1_FLASH_MACRO_1      0u
/* Flash macro #2 (the second) present (0=No, 1=Yes) */
#define CPUSS_SPCIF1_FLASH_MACRO_2      0u
/* Flash macro #3 (the second) present (0=No, 1=Yes) */
#define CPUSS_SPCIF1_FLASH_MACRO_3      0u
/* S8FS_ver2 & Flash Macro 1 */
#define CPUSS_SPCIF1_FLASH_S8FS_VER2_M1 0u
/* S8FS_ver2 & Flash Macro 2 */
#define CPUSS_SPCIF1_FLASH_S8FS_VER2_M2 0u
/* S8FS_ver2 & Flash Macro 3 */
#define CPUSS_SPCIF1_FLASH_S8FS_VER2_M3 0u
/* Not S8FS_ver2 & Flash Macro 1 */
#define CPUSS_SPCIF1_FLASH_NOT_S8FS_VER2_M1 0u
/* Not S8FS_ver2 & Flash Macro 2 */
#define CPUSS_SPCIF1_FLASH_NOT_S8FS_VER2_M2 0u
/* Not S8FS_ver2 & Flash Macro 3 */
#define CPUSS_SPCIF1_FLASH_NOT_S8FS_VER2_M3 0u
/* SPCIF Timer clock is synchronous (1) or asynchronous (0) to clk_sys */
#define CPUSS_SPCIF1_SYNCHRONOUS        1u
/* NVLatches present (0=No, 1=Yes) */
#define CPUSS_SPCIF1_NVLATCH            0u
/* NVLatches size in bytes (default=8) */
#define CPUSS_SPCIF1_NVLATCH_SIZE       8u
/* Inverse value of ECC_REPORT_BY_FAULT */
#define CPUSS_SPCIF1_ECC_REPORT_BY_INTR 0u
/* AES cipher support (0 = no support, 1 = support */
#define CRYPTO_AES                      1u
/* Pseudo random number generation support (0 = no support, 1 = support) */
#define CRYPTO_PR                       1u
/* True random number generation support (0 = no support, 1 = support) */
#define CRYPTO_TR                       1u
/* SHA hash support (0 = no support, 1 = support) */
#define CRYPTO_SHA                      1u
/* Cyclic Redundancy Check support (0 = no support, 1 = support) */
#define CRYPTO_CRC                      1u
/* Source Pointer 0 */
#define CRYPTO_SRC_CTL0_PRESENT         1u
/* Source Pointer 1 */
#define CRYPTO_SRC_CTL1_PRESENT         1u
/* Destination Pointer 0 */
#define CRYPTO_DST_CTL0_PRESENT         1u
/* Destination Pointer 1 */
#define CRYPTO_DST_CTL1_PRESENT         1u
/* Number of 32-bit words in the memory buffer (from the set [64, 128, 256, 512],
   to allow for a 256 B, 512 B, 1 kB, and 2 kB memory buffer) */
#define CRYPTO_MEM_SIZE                 128u
/* Memory address width in bytes */
#define CRYPTO_MEM_B_ADDR_WIDTH         16u
/* Memory address width in words */
#define CRYPTO_MEM_W_ADDR_WIDTH         16u
/* Set to 1 if IP will instantiate spares (0=None, 1=Max, 2=Min) */
#define CRYPTO_SPARE_EN                 1u
/* Spare Technology choice 1=scs8ls, 2=scs8hd,3=scs8hv(invalid), others reserved */
#define CRYPTO_SPARE_STDLIB_CFG         2u
/* Enable instantiate of PLL within IP */
#define EXCO_PLL_PRESENT                0u
/* Set to 1 if IP will instantiate spares (0=None, 1=Max, 2=Min) */
#define EXCO_SPARE_EN                   1u
/* Enable use of alternate PLL Reference source (0=disable, 1=enable) Note: Not
   valid unless PLL_PRESENT==1. */
#define EXCO_PLL_REF_IN_EN              0u
/* LIN PHY Present (1 - present, 0 - not present) */
#define HVSS_LIN_PRESENT                0u
/* HVDivider Present (1 - present, 0 - not present), when present, two Hvdividers
   exist */
#define HVSS_HVDIV_PRESENT              0u
/* IOSS Phase */
#define IOSS_IOSS_PHASE                 5u
/* Route REFGEN vinref & voutref to AMUX bus when set */
#define IOSS_SIO_REF_HOOKUP             0u
/* Use 1.8V signaling on XRES (Xres4 cell only) */
#define IOSS_USE_1P8V_SIGNALING_XRES4   0u
/* Number of ports in device */
#define IOSS_GPIO_GPIO_PORT_NR          5u
/* Indicates port is present in the device */
#define IOSS_GPIO_GPIO_PORT_NR0_GPIO_PRT_PRESENT 1u
/* Indicates port is either GPIO or SIO (i.e. all GPIO registers present) */
#define IOSS_GPIO_GPIO_PORT_NR0_GPIO_PRT_GPIO 1u
/* Indicates port is an SIO port (i.e. both GPIO and SIO registers present) */
#define IOSS_GPIO_GPIO_PORT_NR0_GPIO_PRT_SIO 0u
/* Indicates port is a GPIO port with OVT */
#define IOSS_GPIO_GPIO_PORT_NR0_GPIO_PRT_GPIO_OVT 0u
/* Indicates port is a GPIO port with OVT reference generator */
#define IOSS_GPIO_GPIO_PORT_NR0_GPIO_PRT_GPIO_OVT_VREFGEN 0u
/* Indicates port is a GPIO port with 1.2V I/O */
#define IOSS_GPIO_GPIO_PORT_NR0_GPIO_PRT_GPIO_V1P2 0u
/* Indicates port is an AUX port */
#define IOSS_GPIO_GPIO_PORT_NR0_GPIO_PRT_AUX 0u
/* Indicates that pin #0 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR0_GPIO_PRT_IO0 1u
/* Indicates that pin #1 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR0_GPIO_PRT_IO1 1u
/* Indicates that pin #2 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR0_GPIO_PRT_IO2 1u
/* Indicates that pin #3 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR0_GPIO_PRT_IO3 1u
/* Indicates that pin #4 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR0_GPIO_PRT_IO4 1u
/* Indicates that pin #5 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR0_GPIO_PRT_IO5 1u
/* Indicates that pin #6 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR0_GPIO_PRT_IO6 1u
/* Indicates that pin #7 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR0_GPIO_PRT_IO7 1u
/* Indicates that pin #0 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR0_GPIO_PRT_FILTER_EN0 0u
/* Indicates that pin #1 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR0_GPIO_PRT_FILTER_EN1 0u
/* Indicates that pin #2 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR0_GPIO_PRT_FILTER_EN2 0u
/* Indicates that pin #3 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR0_GPIO_PRT_FILTER_EN3 0u
/* Indicates that pin #4 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR0_GPIO_PRT_FILTER_EN4 0u
/* Indicates that pin #5 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR0_GPIO_PRT_FILTER_EN5 0u
/* Indicates that pin #6 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR0_GPIO_PRT_FILTER_EN6 0u
/* Indicates that pin #7 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR0_GPIO_PRT_FILTER_EN7 0u
/* Indicates port is present in the device */
#define IOSS_GPIO_GPIO_PORT_NR1_GPIO_PRT_PRESENT 1u
/* Indicates port is either GPIO or SIO (i.e. all GPIO registers present) */
#define IOSS_GPIO_GPIO_PORT_NR1_GPIO_PRT_GPIO 1u
/* Indicates port is an SIO port (i.e. both GPIO and SIO registers present) */
#define IOSS_GPIO_GPIO_PORT_NR1_GPIO_PRT_SIO 0u
/* Indicates port is a GPIO port with OVT */
#define IOSS_GPIO_GPIO_PORT_NR1_GPIO_PRT_GPIO_OVT 0u
/* Indicates port is a GPIO port with OVT reference generator */
#define IOSS_GPIO_GPIO_PORT_NR1_GPIO_PRT_GPIO_OVT_VREFGEN 0u
/* Indicates port is a GPIO port with 1.2V I/O */
#define IOSS_GPIO_GPIO_PORT_NR1_GPIO_PRT_GPIO_V1P2 0u
/* Indicates port is an AUX port */
#define IOSS_GPIO_GPIO_PORT_NR1_GPIO_PRT_AUX 0u
/* Indicates that pin #0 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR1_GPIO_PRT_IO0 1u
/* Indicates that pin #1 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR1_GPIO_PRT_IO1 1u
/* Indicates that pin #2 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR1_GPIO_PRT_IO2 1u
/* Indicates that pin #3 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR1_GPIO_PRT_IO3 0u
/* Indicates that pin #4 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR1_GPIO_PRT_IO4 0u
/* Indicates that pin #5 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR1_GPIO_PRT_IO5 0u
/* Indicates that pin #6 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR1_GPIO_PRT_IO6 0u
/* Indicates that pin #7 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR1_GPIO_PRT_IO7 0u
/* Indicates that pin #0 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR1_GPIO_PRT_FILTER_EN0 0u
/* Indicates that pin #1 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR1_GPIO_PRT_FILTER_EN1 0u
/* Indicates that pin #2 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR1_GPIO_PRT_FILTER_EN2 0u
/* Indicates that pin #3 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR1_GPIO_PRT_FILTER_EN3 0u
/* Indicates that pin #4 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR1_GPIO_PRT_FILTER_EN4 0u
/* Indicates that pin #5 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR1_GPIO_PRT_FILTER_EN5 0u
/* Indicates that pin #6 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR1_GPIO_PRT_FILTER_EN6 0u
/* Indicates that pin #7 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR1_GPIO_PRT_FILTER_EN7 0u
/* Indicates port is present in the device */
#define IOSS_GPIO_GPIO_PORT_NR2_GPIO_PRT_PRESENT 1u
/* Indicates port is either GPIO or SIO (i.e. all GPIO registers present) */
#define IOSS_GPIO_GPIO_PORT_NR2_GPIO_PRT_GPIO 1u
/* Indicates port is an SIO port (i.e. both GPIO and SIO registers present) */
#define IOSS_GPIO_GPIO_PORT_NR2_GPIO_PRT_SIO 0u
/* Indicates port is a GPIO port with OVT */
#define IOSS_GPIO_GPIO_PORT_NR2_GPIO_PRT_GPIO_OVT 0u
/* Indicates port is a GPIO port with OVT reference generator */
#define IOSS_GPIO_GPIO_PORT_NR2_GPIO_PRT_GPIO_OVT_VREFGEN 0u
/* Indicates port is a GPIO port with 1.2V I/O */
#define IOSS_GPIO_GPIO_PORT_NR2_GPIO_PRT_GPIO_V1P2 0u
/* Indicates port is an AUX port */
#define IOSS_GPIO_GPIO_PORT_NR2_GPIO_PRT_AUX 0u
/* Indicates that pin #0 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR2_GPIO_PRT_IO0 1u
/* Indicates that pin #1 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR2_GPIO_PRT_IO1 1u
/* Indicates that pin #2 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR2_GPIO_PRT_IO2 1u
/* Indicates that pin #3 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR2_GPIO_PRT_IO3 1u
/* Indicates that pin #4 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR2_GPIO_PRT_IO4 1u
/* Indicates that pin #5 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR2_GPIO_PRT_IO5 1u
/* Indicates that pin #6 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR2_GPIO_PRT_IO6 1u
/* Indicates that pin #7 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR2_GPIO_PRT_IO7 1u
/* Indicates that pin #0 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR2_GPIO_PRT_FILTER_EN0 0u
/* Indicates that pin #1 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR2_GPIO_PRT_FILTER_EN1 0u
/* Indicates that pin #2 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR2_GPIO_PRT_FILTER_EN2 0u
/* Indicates that pin #3 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR2_GPIO_PRT_FILTER_EN3 0u
/* Indicates that pin #4 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR2_GPIO_PRT_FILTER_EN4 0u
/* Indicates that pin #5 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR2_GPIO_PRT_FILTER_EN5 0u
/* Indicates that pin #6 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR2_GPIO_PRT_FILTER_EN6 0u
/* Indicates that pin #7 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR2_GPIO_PRT_FILTER_EN7 0u
/* Indicates port is present in the device */
#define IOSS_GPIO_GPIO_PORT_NR3_GPIO_PRT_PRESENT 1u
/* Indicates port is either GPIO or SIO (i.e. all GPIO registers present) */
#define IOSS_GPIO_GPIO_PORT_NR3_GPIO_PRT_GPIO 1u
/* Indicates port is an SIO port (i.e. both GPIO and SIO registers present) */
#define IOSS_GPIO_GPIO_PORT_NR3_GPIO_PRT_SIO 0u
/* Indicates port is a GPIO port with OVT */
#define IOSS_GPIO_GPIO_PORT_NR3_GPIO_PRT_GPIO_OVT 0u
/* Indicates port is a GPIO port with OVT reference generator */
#define IOSS_GPIO_GPIO_PORT_NR3_GPIO_PRT_GPIO_OVT_VREFGEN 0u
/* Indicates port is a GPIO port with 1.2V I/O */
#define IOSS_GPIO_GPIO_PORT_NR3_GPIO_PRT_GPIO_V1P2 0u
/* Indicates port is an AUX port */
#define IOSS_GPIO_GPIO_PORT_NR3_GPIO_PRT_AUX 0u
/* Indicates that pin #0 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR3_GPIO_PRT_IO0 1u
/* Indicates that pin #1 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR3_GPIO_PRT_IO1 1u
/* Indicates that pin #2 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR3_GPIO_PRT_IO2 1u
/* Indicates that pin #3 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR3_GPIO_PRT_IO3 0u
/* Indicates that pin #4 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR3_GPIO_PRT_IO4 0u
/* Indicates that pin #5 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR3_GPIO_PRT_IO5 0u
/* Indicates that pin #6 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR3_GPIO_PRT_IO6 0u
/* Indicates that pin #7 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR3_GPIO_PRT_IO7 0u
/* Indicates that pin #0 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR3_GPIO_PRT_FILTER_EN0 0u
/* Indicates that pin #1 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR3_GPIO_PRT_FILTER_EN1 0u
/* Indicates that pin #2 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR3_GPIO_PRT_FILTER_EN2 0u
/* Indicates that pin #3 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR3_GPIO_PRT_FILTER_EN3 0u
/* Indicates that pin #4 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR3_GPIO_PRT_FILTER_EN4 0u
/* Indicates that pin #5 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR3_GPIO_PRT_FILTER_EN5 0u
/* Indicates that pin #6 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR3_GPIO_PRT_FILTER_EN6 0u
/* Indicates that pin #7 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR3_GPIO_PRT_FILTER_EN7 0u
/* Indicates port is present in the device */
#define IOSS_GPIO_GPIO_PORT_NR4_GPIO_PRT_PRESENT 1u
/* Indicates port is either GPIO or SIO (i.e. all GPIO registers present) */
#define IOSS_GPIO_GPIO_PORT_NR4_GPIO_PRT_GPIO 0u
/* Indicates port is an SIO port (i.e. both GPIO and SIO registers present) */
#define IOSS_GPIO_GPIO_PORT_NR4_GPIO_PRT_SIO 0u
/* Indicates port is a GPIO port with OVT */
#define IOSS_GPIO_GPIO_PORT_NR4_GPIO_PRT_GPIO_OVT 0u
/* Indicates port is a GPIO port with OVT reference generator */
#define IOSS_GPIO_GPIO_PORT_NR4_GPIO_PRT_GPIO_OVT_VREFGEN 0u
/* Indicates port is a GPIO port with 1.2V I/O */
#define IOSS_GPIO_GPIO_PORT_NR4_GPIO_PRT_GPIO_V1P2 0u
/* Indicates port is an AUX port */
#define IOSS_GPIO_GPIO_PORT_NR4_GPIO_PRT_AUX 1u
/* Indicates that pin #0 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR4_GPIO_PRT_IO0 1u
/* Indicates that pin #1 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR4_GPIO_PRT_IO1 1u
/* Indicates that pin #2 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR4_GPIO_PRT_IO2 1u
/* Indicates that pin #3 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR4_GPIO_PRT_IO3 1u
/* Indicates that pin #4 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR4_GPIO_PRT_IO4 0u
/* Indicates that pin #5 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR4_GPIO_PRT_IO5 0u
/* Indicates that pin #6 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR4_GPIO_PRT_IO6 0u
/* Indicates that pin #7 exists for this port */
#define IOSS_GPIO_GPIO_PORT_NR4_GPIO_PRT_IO7 0u
/* Indicates that pin #0 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR4_GPIO_PRT_FILTER_EN0 0u
/* Indicates that pin #1 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR4_GPIO_PRT_FILTER_EN1 0u
/* Indicates that pin #2 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR4_GPIO_PRT_FILTER_EN2 0u
/* Indicates that pin #3 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR4_GPIO_PRT_FILTER_EN3 0u
/* Indicates that pin #4 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR4_GPIO_PRT_FILTER_EN4 0u
/* Indicates that pin #5 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR4_GPIO_PRT_FILTER_EN5 0u
/* Indicates that pin #6 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR4_GPIO_PRT_FILTER_EN6 0u
/* Indicates that pin #7 filter is present ( Add 50ns Glitch Filter in datapath) */
#define IOSS_GPIO_GPIO_PORT_NR4_GPIO_PRT_FILTER_EN7 0u
/* Control register to improve IO testing */
#define IOSS_GPIO_IO_TEST_CTL           1u
/* GPIO_V1P2 cells used ? */
#define IOSS_GPIO_IO_GPIOV1P2           0u
/* Is there a pump in IOSS ? (same as GLOBAL.HAS_PUMP_IN_IOSS) */
#define IOSS_HSIOM_PUMP                 0u
/* Number of AMUX splitter cells */
#define IOSS_HSIOM_AMUX_SPLIT_NR        0u
/* Number of HSIOM ports in device (same as GPIO.GPIO_PRT_NR) */
#define IOSS_HSIOM_HSIOM_PORT_NR        5u
/* Indicates that pin #0 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR0_HSIOM_PRT_IO0 1u
/* Indicates that pin #1 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR0_HSIOM_PRT_IO1 1u
/* Indicates that pin #2 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR0_HSIOM_PRT_IO2 1u
/* Indicates that pin #3 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR0_HSIOM_PRT_IO3 1u
/* Indicates that pin #4 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR0_HSIOM_PRT_IO4 1u
/* Indicates that pin #5 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR0_HSIOM_PRT_IO5 1u
/* Indicates that pin #6 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR0_HSIOM_PRT_IO6 1u
/* Indicates that pin #7 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR0_HSIOM_PRT_IO7 1u
/* Indicates that pin #0 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR1_HSIOM_PRT_IO0 1u
/* Indicates that pin #1 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR1_HSIOM_PRT_IO1 1u
/* Indicates that pin #2 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR1_HSIOM_PRT_IO2 1u
/* Indicates that pin #3 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR1_HSIOM_PRT_IO3 0u
/* Indicates that pin #4 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR1_HSIOM_PRT_IO4 0u
/* Indicates that pin #5 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR1_HSIOM_PRT_IO5 0u
/* Indicates that pin #6 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR1_HSIOM_PRT_IO6 0u
/* Indicates that pin #7 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR1_HSIOM_PRT_IO7 0u
/* Indicates that pin #0 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR2_HSIOM_PRT_IO0 1u
/* Indicates that pin #1 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR2_HSIOM_PRT_IO1 1u
/* Indicates that pin #2 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR2_HSIOM_PRT_IO2 1u
/* Indicates that pin #3 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR2_HSIOM_PRT_IO3 1u
/* Indicates that pin #4 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR2_HSIOM_PRT_IO4 1u
/* Indicates that pin #5 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR2_HSIOM_PRT_IO5 1u
/* Indicates that pin #6 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR2_HSIOM_PRT_IO6 1u
/* Indicates that pin #7 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR2_HSIOM_PRT_IO7 1u
/* Indicates that pin #0 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR3_HSIOM_PRT_IO0 1u
/* Indicates that pin #1 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR3_HSIOM_PRT_IO1 1u
/* Indicates that pin #2 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR3_HSIOM_PRT_IO2 1u
/* Indicates that pin #3 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR3_HSIOM_PRT_IO3 0u
/* Indicates that pin #4 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR3_HSIOM_PRT_IO4 0u
/* Indicates that pin #5 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR3_HSIOM_PRT_IO5 0u
/* Indicates that pin #6 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR3_HSIOM_PRT_IO6 0u
/* Indicates that pin #7 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR3_HSIOM_PRT_IO7 0u
/* Indicates that pin #0 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR4_HSIOM_PRT_IO0 1u
/* Indicates that pin #1 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR4_HSIOM_PRT_IO1 1u
/* Indicates that pin #2 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR4_HSIOM_PRT_IO2 1u
/* Indicates that pin #3 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR4_HSIOM_PRT_IO3 1u
/* Indicates that pin #4 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR4_HSIOM_PRT_IO4 0u
/* Indicates that pin #5 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR4_HSIOM_PRT_IO5 0u
/* Indicates that pin #6 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR4_HSIOM_PRT_IO6 0u
/* Indicates that pin #7 exists for this port */
#define IOSS_HSIOM_HSIOM_PORT_NR4_HSIOM_PRT_IO7 0u
/* Number of PRGIO instances (in 8b ports) */
#define IOSS_PRGIO_PRGIO_NR             0u
/* Include Master functionality */
#define ISOUART_MASTER                  1u
/* Number of trigger outputs */
#define ISOUART_NUMTRIGOUT              16u
/* Number of trig_int structures */
#define ISOUART_NUMTISTRUCT             18u
/* Number of trigger inputs */
#define ISOUART_NUMTRIGIN               2u
/* Set to 1 if IP will instantiate spares (0=None, 1=Max, 2=Min) */
#define ISOUART_SPARE_EN                1u
/* Spare Technology choice 1=scs8ls, 2=scs8hd, 3=scs8hv(invalid), others reserved */
#define ISOUART_SPARE_STDLIB_CFG        2u
/* Number of LIN channels ([2, 32]). For test functionality (two channels are
   connected), the minimal number of LIN channels is 2. */
#define LIN_CH_NR                       2u
/* Number of AHB-Lite "hmaster[]" bits ([1, 8]). */
#define LIN_MASTER_WIDTH                2u
/* 0=ULL65, 1=MXS40-ULP1, 2=MXS40E2, 3=M0S8, 4=MXS40-HD */
#define LIN_CHIP_TOP_PLATFORM_VARIANT   3u
/* Number of digital channels */
#define PACSS_DCH_NR                    4u
/* Include FIR in digital channel */
#define PACSS_DCH_NR0_DCHAN_DCH_FIR     1u
/* Include FIR in digital channel */
#define PACSS_DCH_NR1_DCHAN_DCH_FIR     1u
/* Include FIR in digital channel */
#define PACSS_DCH_NR2_DCHAN_DCH_FIR     0u
/* Include FIR in digital channel */
#define PACSS_DCH_NR3_DCHAN_DCH_FIR     0u
/* Number of analog channels */
#define PACSS_ACH_NR                    2u
/* Number of FIR filters */
#define PACSS_FIR_NR                    2u
/* Spare Enable 0=no spare, 1=max, 2=min */
#define PACSS_SPARE_EN                  1u
/* Ground vdiv_ret output ( tie_vssa equivalent because output is not a supply,
   for HVSS_VER2 support ) */
#define PACSS_GND_VDIV_RET              1u
/* Number of iztat_2p4u inputs (if POS_PUMP = 1, then IZTAT_NR=7, else IZTAT_NR=6) */
#define PACSS_IZTAT_NR                  6u
/* Spare Technology choice 1=scs8ls, 2=scs8hd, 3=scs8hv(invalid), others reserved */
#define PACSS_SPARE_STDLIB_CFG          2u
/* Select Cell VT type. Drives the type of standard cell to be used in MXTK. (0 =
   scs8hd, 1 = scs8hdll, 2 = scs8hv) */
#define PACSS_CELL_VT_TYPE              0u
/* Number of AHB-Lite "hmaster[]" bits ([1, 8]). */
#define PACSS_MASTER_WIDTH              2u
/* Number of OverCurrent Detection Circuits ( 0 , 1 , or 2 ) */
#define PACSS_MMIO_OCD_NR               2u
/* OverCurrent Detection Circuit 0 Exists (OCD_NR > 0) */
#define PACSS_MMIO_OCD0_EXISTS          1u
/* OverCurrent Detection Circuit 1 Exists (OCD_NR = 2) */
#define PACSS_MMIO_OCD1_EXISTS          1u
/* Include positive pump */
#define PACSS_MMIO_POS_PUMP             0u
/* Number of programmable clocks (outputs) */
#define PERI_PCLK_CLOCK_NR              12u
/* Number of 8.0 dividers */
#define PERI_PCLK_DIV_8_NR              0u
/* Number of 16.0 dividers */
#define PERI_PCLK_DIV_16_NR             6u
/* Number of 16.5 (fractional) dividers */
#define PERI_PCLK_DIV_16_5_NR           2u
/* Number of 24.5 (fractional) dividers */
#define PERI_PCLK_DIV_24_5_NR           0u
/* Divider number width: roundup(log2(max(DIV_*_NR)) */
#define PERI_PCLK_DIV_ADDR_WIDTH        3u
/* Trigger module present (0=No, 1=Yes) */
#define PERI_TR                         1u
/* Number of trigger groups */
#define PERI_TR_GROUP_NR                7u
/* Number of input triggers */
#define PERI_TR_GROUP_NR0_TR_GROUP_TR_IN_NR 66u
/* Number of output triggers */
#define PERI_TR_GROUP_NR0_TR_GROUP_TR_OUT_NR 18u
/* Input trigger number width: roundup(log2(TR_IN_NR)) */
#define PERI_TR_GROUP_NR0_TR_GROUP_TR_IN_ADDR_WIDTH 7u
/* Number of input triggers */
#define PERI_TR_GROUP_NR1_TR_GROUP_TR_IN_NR 65u
/* Number of output triggers */
#define PERI_TR_GROUP_NR1_TR_GROUP_TR_OUT_NR 7u
/* Input trigger number width: roundup(log2(TR_IN_NR)) */
#define PERI_TR_GROUP_NR1_TR_GROUP_TR_IN_ADDR_WIDTH 7u
/* Number of input triggers */
#define PERI_TR_GROUP_NR2_TR_GROUP_TR_IN_NR 40u
/* Number of output triggers */
#define PERI_TR_GROUP_NR2_TR_GROUP_TR_OUT_NR 4u
/* Input trigger number width: roundup(log2(TR_IN_NR)) */
#define PERI_TR_GROUP_NR2_TR_GROUP_TR_IN_ADDR_WIDTH 6u
/* Number of input triggers */
#define PERI_TR_GROUP_NR3_TR_GROUP_TR_IN_NR 14u
/* Number of output triggers */
#define PERI_TR_GROUP_NR3_TR_GROUP_TR_OUT_NR 2u
/* Input trigger number width: roundup(log2(TR_IN_NR)) */
#define PERI_TR_GROUP_NR3_TR_GROUP_TR_IN_ADDR_WIDTH 4u
/* Number of input triggers */
#define PERI_TR_GROUP_NR4_TR_GROUP_TR_IN_NR 10u
/* Number of output triggers */
#define PERI_TR_GROUP_NR4_TR_GROUP_TR_OUT_NR 2u
/* Input trigger number width: roundup(log2(TR_IN_NR)) */
#define PERI_TR_GROUP_NR4_TR_GROUP_TR_IN_ADDR_WIDTH 4u
/* Number of input triggers */
#define PERI_TR_GROUP_NR5_TR_GROUP_TR_IN_NR 2u
/* Number of output triggers */
#define PERI_TR_GROUP_NR5_TR_GROUP_TR_OUT_NR 1u
/* Input trigger number width: roundup(log2(TR_IN_NR)) */
#define PERI_TR_GROUP_NR5_TR_GROUP_TR_IN_ADDR_WIDTH 1u
/* Number of input triggers */
#define PERI_TR_GROUP_NR6_TR_GROUP_TR_IN_NR 2u
/* Number of output triggers */
#define PERI_TR_GROUP_NR6_TR_GROUP_TR_OUT_NR 1u
/* Input trigger number width: roundup(log2(TR_IN_NR)) */
#define PERI_TR_GROUP_NR6_TR_GROUP_TR_IN_ADDR_WIDTH 1u
/* Externally Clocked capable? (0=No,1=Yes) */
#define SCB0_EC                         1u
/* I2C Glitch filters present (0=No, 1=Yes) */
#define SCB0_GLITCH                     1u
/* I2C capable? (0=No,1=Yes) */
#define SCB0_I2C                        1u
/* I2C Externally Clocked capable? (0=No,1=Yes) */
#define SCB0_I2C_EC                     1u
/* I2C Master capable? (0=No, 1=Yes) */
#define SCB0_I2C_M                      1u
/* I2C Slave capable? (0=No, 1=Yes) */
#define SCB0_I2C_S                      1u
/* I2C Master and Slave capable? (0=No, 1=Yes) */
#define SCB0_I2C_M_S                    1u
/* I2C Slave with EC? (0=No, 1=Yes) */
#define SCB0_I2C_S_EC                   1u
/* SPI capable? (0=No,1=Yes) */
#define SCB0_SPI                        1u
/* SPI Externally Clocked capable? (0=No,1=Yes) */
#define SCB0_SPI_EC                     1u
/* SPI Master capable? (0=No, 1=Yes) */
#define SCB0_SPI_M                      1u
/* SPI Slave capable? (0=No, 1=Yes) */
#define SCB0_SPI_S                      1u
/* SPI Slave with EC? (0=No, 1=Yes) */
#define SCB0_SPI_S_EC                   1u
/* UART capable? (0=No,1=Yes) */
#define SCB0_UART                       1u
/* UART Flow control? (0=No, 1=Yes) */
#define SCB0_UART_FLOW                  0u
/* Number of bits to represent a FIFO address */
#define SCB0_FF_DATA_NR_LOG2            4u
/* Number of bits to represent #bytes in FIFO */
#define SCB0_FF_DATA_NR_LOG2_PLUS1      5u
/* Number of words in EZ memory */
#define SCB0_EZ_DATA_NR                 32u
/* Number of bits to represent #bytes in EZ memory */
#define SCB0_EZ_DATA_NR_LOG2            5u
/* Command/response mode capable? (0=No, 1=Yes) */
#define SCB0_CMD_RESP                   1u
/* EZ mode capable? (0=No, 1=Yes) */
#define SCB0_EZ                         1u
/* Is at least one of EZ or CMD_RESP set to 1? (0=No,1=Yes) */
#define SCB0_EZ_CMD_RESP                1u
/* I2C slave with EZ mode (0 = N0, 1= Yes) */
#define SCB0_I2C_S_EZ                   1u
/* SPI slave with EZ mode (0 = N0, 1= Yes) */
#define SCB0_SPI_S_EZ                   1u
/* Support I2C Fast+ 1MBit/Sec speed (0=No, 1=Yes) */
#define SCB0_i2C_FAST_PLUS              1u
/* Set to 1 to add enhancement to tolerate SPI SELECT glitch in SPI slave EC EZ
   and CMDRESP mode (0=No Enhancement) */
#define SCB0_M0S8SCB_VER2_SPI_ENHANCEMENT 1u
/* Set to 1 if IP will instantiate spares (0=None, 1=Max, 2=Min) */
#define SCB0_SPARE_EN                   0u
/* Spare Technology choice 1=scs8ls, 2=scs8hd, 3=scs8hv(invalid), others reserved */
#define SCB0_SPARE_STDLIB_CFG           2u
/* Externally Clocked capable? (0=No,1=Yes) */
#define SCB1_EC                         1u
/* I2C Glitch filters present (0=No, 1=Yes) */
#define SCB1_GLITCH                     1u
/* I2C capable? (0=No,1=Yes) */
#define SCB1_I2C                        1u
/* I2C Externally Clocked capable? (0=No,1=Yes) */
#define SCB1_I2C_EC                     1u
/* I2C Master capable? (0=No, 1=Yes) */
#define SCB1_I2C_M                      1u
/* I2C Slave capable? (0=No, 1=Yes) */
#define SCB1_I2C_S                      1u
/* I2C Master and Slave capable? (0=No, 1=Yes) */
#define SCB1_I2C_M_S                    1u
/* I2C Slave with EC? (0=No, 1=Yes) */
#define SCB1_I2C_S_EC                   1u
/* SPI capable? (0=No,1=Yes) */
#define SCB1_SPI                        1u
/* SPI Externally Clocked capable? (0=No,1=Yes) */
#define SCB1_SPI_EC                     1u
/* SPI Master capable? (0=No, 1=Yes) */
#define SCB1_SPI_M                      1u
/* SPI Slave capable? (0=No, 1=Yes) */
#define SCB1_SPI_S                      1u
/* SPI Slave with EC? (0=No, 1=Yes) */
#define SCB1_SPI_S_EC                   1u
/* UART capable? (0=No,1=Yes) */
#define SCB1_UART                       1u
/* UART Flow control? (0=No, 1=Yes) */
#define SCB1_UART_FLOW                  0u
/* Number of bits to represent a FIFO address */
#define SCB1_FF_DATA_NR_LOG2            4u
/* Number of bits to represent #bytes in FIFO */
#define SCB1_FF_DATA_NR_LOG2_PLUS1      5u
/* Number of words in EZ memory */
#define SCB1_EZ_DATA_NR                 32u
/* Number of bits to represent #bytes in EZ memory */
#define SCB1_EZ_DATA_NR_LOG2            5u
/* Command/response mode capable? (0=No, 1=Yes) */
#define SCB1_CMD_RESP                   1u
/* EZ mode capable? (0=No, 1=Yes) */
#define SCB1_EZ                         1u
/* Is at least one of EZ or CMD_RESP set to 1? (0=No,1=Yes) */
#define SCB1_EZ_CMD_RESP                1u
/* I2C slave with EZ mode (0 = N0, 1= Yes) */
#define SCB1_I2C_S_EZ                   1u
/* SPI slave with EZ mode (0 = N0, 1= Yes) */
#define SCB1_SPI_S_EZ                   1u
/* Support I2C Fast+ 1MBit/Sec speed (0=No, 1=Yes) */
#define SCB1_i2C_FAST_PLUS              1u
/* Set to 1 to add enhancement to tolerate SPI SELECT glitch in SPI slave EC EZ
   and CMDRESP mode (0=No Enhancement) */
#define SCB1_M0S8SCB_VER2_SPI_ENHANCEMENT 1u
/* Set to 1 if IP will instantiate spares (0=None, 1=Max, 2=Min) */
#define SCB1_SPARE_EN                   0u
/* Spare Technology choice 1=scs8ls, 2=scs8hd, 3=scs8hv(invalid), others reserved */
#define SCB1_SPARE_STDLIB_CFG           2u
/* Is this product using automotive SFLASH layout ? */
#define SFLASH_AUTO_SFLASH              1u
/* Does this product have standard SWD placement control ? */
#define SFLASH_STD_SWD                  1u
/* Does this product use SRSS-HV ? */
#define SFLASH_SRSSHV                   1u
/* Does this product include PACSS (m0s8pacss) ? */
#define SFLASH_HAS_PACSS                1u
/* Does this product include PACSS + OCD (m0s8pacss + s8panatk) ? */
#define SFLASH_HAS_OCD                  1u
/* Does this product include SAR (m0s8sar) ? */
#define SFLASH_HAS_SAR                  0u
/* Does this product include PACSSv2 and CMC (m0s8pacss_ver2 and m0s8cmc) ? */
#define SFLASH_HAS_PACSSV2              0u
/* SRSSHV will use the NOESD version when set to 1 */
#define SRSSHV_SRSSHV_NOESD             0u
/* Number of bits from WDT_MATCH that are ignore in comparison */
#define SRSSHV_WDT_MATCH_MASK           0u
/* Connect vddio bus to pad_vddd (0) or pad_vccq_or_vddio (1) */
#define SRSSHV_HAS_VDDIO_PAD            1u
/* Enable ASIL-B Support - Add PILO/HPOSC, etc. {0=No, 1=Yes} */
#define SRSSHV_ENABLE_ASILB_SUPPORT     1u
/* PILO Enable (0=Not Present, 1=Present) */
#define SRSSHV_PILO_PRESENT             1u
/* HPOSC Enable (0=Not Present, 1=Present) */
#define SRSSHV_HPOSC_PRESENT            1u
/* CRWDT Enable (0=Not Present, 1=Present) */
#define SRSSHV_CRWDT_PRESENT            1u
/* Number of counters per IP (1..8) */
#define TCPWM_CNT_NR                    5u
/* Set to 1 if IP will instantiate spares (0=None, 1=Max, 2=Min) */
#define TCPWM_SPARE_EN                  0u
/* Spare Technology choice 1=scs8ls, 2=scs8hd, 3=scs8hv(invalid), others reserved */
#define TCPWM_SPARE_STDLIB_CFG          2u
/* Number of words in EZ memory */
#define SCB_EZ_DATA_NR                  32u

#endif /* _PSOC4HVPASPM10_CONFIG_H_ */


/* [] END OF FILE */
