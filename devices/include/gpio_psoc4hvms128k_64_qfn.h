/***************************************************************************//**
* \file gpio_psoc4hvms128k_64_qfn.h
*
* \brief
* PSOC4HVMS128K device GPIO header for 64-QFN package
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

#ifndef _GPIO_PSOC4HVMS128K_64_QFN_H_
#define _GPIO_PSOC4HVMS128K_64_QFN_H_

/* Package type */
enum
{
    CY_GPIO_PACKAGE_QFN,
    CY_GPIO_PACKAGE_BGA,
    CY_GPIO_PACKAGE_CSP,
    CY_GPIO_PACKAGE_WLCSP,
    CY_GPIO_PACKAGE_LQFP,
    CY_GPIO_PACKAGE_TQFP,
    CY_GPIO_PACKAGE_SMT,
    CY_GPIO_PACKAGE_DFN,
    CY_GPIO_PACKAGE_SOIC,
    CY_GPIO_PACKAGE_SSOP,
    CY_GPIO_PACKAGE_LGA,
};

#define CY_GPIO_PACKAGE_TYPE            CY_GPIO_PACKAGE_QFN
#define CY_GPIO_PIN_COUNT               64u

/* AMUXBUS Segments */
enum
{
    AMUXBUS_MSC,
    AMUXBUS_SYS,
};

/* AMUX Splitter Controls */
typedef enum
{
    AMUX_SPLIT_CTL_0                = 0x0000u,  /* Left = AMUXBUS_MSC; Right = AMUXBUS_SYS */
    AMUX_SPLIT_CTL_1                = 0x0001u   /* Left = AMUXBUS_SYS; Right = AMUXBUS_MSC */
} cy_en_amux_split_t;

/* Port List */
/* PORT 0 (GPIO) */
#define P0_0_PORT                       GPIO_PRT0
#define P0_0_PIN                        0u
#define P0_0_NUM                        0u
#define P0_0_AMUXSEGMENT                AMUXBUS_SYS
#define P0_1_PORT                       GPIO_PRT0
#define P0_1_PIN                        1u
#define P0_1_NUM                        1u
#define P0_1_AMUXSEGMENT                AMUXBUS_SYS
#define P0_2_PORT                       GPIO_PRT0
#define P0_2_PIN                        2u
#define P0_2_NUM                        2u
#define P0_2_AMUXSEGMENT                AMUXBUS_SYS
#define P0_3_PORT                       GPIO_PRT0
#define P0_3_PIN                        3u
#define P0_3_NUM                        3u
#define P0_3_AMUXSEGMENT                AMUXBUS_SYS
#define P0_4_PORT                       GPIO_PRT0
#define P0_4_PIN                        4u
#define P0_4_NUM                        4u
#define P0_4_AMUXSEGMENT                AMUXBUS_SYS
#define P0_5_PORT                       GPIO_PRT0
#define P0_5_PIN                        5u
#define P0_5_NUM                        5u
#define P0_5_AMUXSEGMENT                AMUXBUS_SYS
#define P0_6_PORT                       GPIO_PRT0
#define P0_6_PIN                        6u
#define P0_6_NUM                        6u
#define P0_6_AMUXSEGMENT                AMUXBUS_SYS
#define P0_7_PORT                       GPIO_PRT0
#define P0_7_PIN                        7u
#define P0_7_NUM                        7u
#define P0_7_AMUXSEGMENT                AMUXBUS_SYS

/* PORT 1 (GPIO) */
#define P1_0_PORT                       GPIO_PRT1
#define P1_0_PIN                        0u
#define P1_0_NUM                        0u
#define P1_0_AMUXSEGMENT                AMUXBUS_MSC
#define P1_1_PORT                       GPIO_PRT1
#define P1_1_PIN                        1u
#define P1_1_NUM                        1u
#define P1_1_AMUXSEGMENT                AMUXBUS_MSC
#define P1_2_PORT                       GPIO_PRT1
#define P1_2_PIN                        2u
#define P1_2_NUM                        2u
#define P1_2_AMUXSEGMENT                AMUXBUS_MSC
#define P1_3_PORT                       GPIO_PRT1
#define P1_3_PIN                        3u
#define P1_3_NUM                        3u
#define P1_3_AMUXSEGMENT                AMUXBUS_MSC
#define P1_4_PORT                       GPIO_PRT1
#define P1_4_PIN                        4u
#define P1_4_NUM                        4u
#define P1_4_AMUXSEGMENT                AMUXBUS_MSC
#define P1_5_PORT                       GPIO_PRT1
#define P1_5_PIN                        5u
#define P1_5_NUM                        5u
#define P1_5_AMUXSEGMENT                AMUXBUS_MSC
#define P1_6_PORT                       GPIO_PRT1
#define P1_6_PIN                        6u
#define P1_6_NUM                        6u
#define P1_6_AMUXSEGMENT                AMUXBUS_MSC
#define P1_7_PORT                       GPIO_PRT1
#define P1_7_PIN                        7u
#define P1_7_NUM                        7u
#define P1_7_AMUXSEGMENT                AMUXBUS_MSC

/* PORT 3 (GPIO) */
#define P3_0_PORT                       GPIO_PRT3
#define P3_0_PIN                        0u
#define P3_0_NUM                        0u
#define P3_0_AMUXSEGMENT                AMUXBUS_MSC
#define P3_1_PORT                       GPIO_PRT3
#define P3_1_PIN                        1u
#define P3_1_NUM                        1u
#define P3_1_AMUXSEGMENT                AMUXBUS_MSC
#define P3_2_PORT                       GPIO_PRT3
#define P3_2_PIN                        2u
#define P3_2_NUM                        2u
#define P3_2_AMUXSEGMENT                AMUXBUS_SYS
#define P3_3_PORT                       GPIO_PRT3
#define P3_3_PIN                        3u
#define P3_3_NUM                        3u
#define P3_3_AMUXSEGMENT                AMUXBUS_SYS
#define P3_4_PORT                       GPIO_PRT3
#define P3_4_PIN                        4u
#define P3_4_NUM                        4u
#define P3_4_AMUXSEGMENT                AMUXBUS_SYS
#define P3_5_PORT                       GPIO_PRT3
#define P3_5_PIN                        5u
#define P3_5_NUM                        5u
#define P3_5_AMUXSEGMENT                AMUXBUS_SYS
#define P3_6_PORT                       GPIO_PRT3
#define P3_6_PIN                        6u
#define P3_6_NUM                        6u
#define P3_6_AMUXSEGMENT                AMUXBUS_SYS
#define P3_7_PORT                       GPIO_PRT3
#define P3_7_PIN                        7u
#define P3_7_NUM                        7u
#define P3_7_AMUXSEGMENT                AMUXBUS_SYS

/* PORT 4 (GPIO) */
#define P4_0_PORT                       GPIO_PRT4
#define P4_0_PIN                        0u
#define P4_0_NUM                        0u
#define P4_0_AMUXSEGMENT                AMUXBUS_MSC
#define P4_1_PORT                       GPIO_PRT4
#define P4_1_PIN                        1u
#define P4_1_NUM                        1u
#define P4_1_AMUXSEGMENT                AMUXBUS_MSC
#define P4_2_PORT                       GPIO_PRT4
#define P4_2_PIN                        2u
#define P4_2_NUM                        2u
#define P4_2_AMUXSEGMENT                AMUXBUS_MSC
#define P4_3_PORT                       GPIO_PRT4
#define P4_3_PIN                        3u
#define P4_3_NUM                        3u
#define P4_3_AMUXSEGMENT                AMUXBUS_MSC
#define P4_4_PORT                       GPIO_PRT4
#define P4_4_PIN                        4u
#define P4_4_NUM                        4u
#define P4_4_AMUXSEGMENT                AMUXBUS_MSC
#define P4_5_PORT                       GPIO_PRT4
#define P4_5_PIN                        5u
#define P4_5_NUM                        5u
#define P4_5_AMUXSEGMENT                AMUXBUS_MSC
#define P4_6_PORT                       GPIO_PRT4
#define P4_6_PIN                        6u
#define P4_6_NUM                        6u
#define P4_6_AMUXSEGMENT                AMUXBUS_MSC
#define P4_7_PORT                       GPIO_PRT4
#define P4_7_PIN                        7u
#define P4_7_NUM                        7u
#define P4_7_AMUXSEGMENT                AMUXBUS_MSC

/* PORT 5 (GPIO) */
#define P5_0_PORT                       GPIO_PRT5
#define P5_0_PIN                        0u
#define P5_0_NUM                        0u
#define P5_0_AMUXSEGMENT                AMUXBUS_MSC
#define P5_1_PORT                       GPIO_PRT5
#define P5_1_PIN                        1u
#define P5_1_NUM                        1u
#define P5_1_AMUXSEGMENT                AMUXBUS_MSC
#define P5_2_PORT                       GPIO_PRT5
#define P5_2_PIN                        2u
#define P5_2_NUM                        2u
#define P5_2_AMUXSEGMENT                AMUXBUS_MSC
#define P5_3_PORT                       GPIO_PRT5
#define P5_3_PIN                        3u
#define P5_3_NUM                        3u
#define P5_3_AMUXSEGMENT                AMUXBUS_MSC
#define P5_4_PORT                       GPIO_PRT5
#define P5_4_PIN                        4u
#define P5_4_NUM                        4u
#define P5_4_AMUXSEGMENT                AMUXBUS_MSC
#define P5_5_PORT                       GPIO_PRT5
#define P5_5_PIN                        5u
#define P5_5_NUM                        5u
#define P5_5_AMUXSEGMENT                AMUXBUS_MSC
#define P5_6_PORT                       GPIO_PRT5
#define P5_6_PIN                        6u
#define P5_6_NUM                        6u
#define P5_6_AMUXSEGMENT                AMUXBUS_MSC
#define P5_7_PORT                       GPIO_PRT5
#define P5_7_PIN                        7u
#define P5_7_NUM                        7u
#define P5_7_AMUXSEGMENT                AMUXBUS_MSC

/* PORT 6 (GPIO) */
#define P6_0_PORT                       GPIO_PRT6
#define P6_0_PIN                        0u
#define P6_0_NUM                        0u
#define P6_0_AMUXSEGMENT                AMUXBUS_MSC
#define P6_1_PORT                       GPIO_PRT6
#define P6_1_PIN                        1u
#define P6_1_NUM                        1u
#define P6_1_AMUXSEGMENT                AMUXBUS_MSC
#define P6_2_PORT                       GPIO_PRT6
#define P6_2_PIN                        2u
#define P6_2_NUM                        2u
#define P6_2_AMUXSEGMENT                AMUXBUS_MSC
#define P6_3_PORT                       GPIO_PRT6
#define P6_3_PIN                        3u
#define P6_3_NUM                        3u
#define P6_3_AMUXSEGMENT                AMUXBUS_SYS
#define P6_4_PORT                       GPIO_PRT6
#define P6_4_PIN                        4u
#define P6_4_NUM                        4u
#define P6_4_AMUXSEGMENT                AMUXBUS_SYS
#define P6_5_PORT                       GPIO_PRT6
#define P6_5_PIN                        5u
#define P6_5_NUM                        5u
#define P6_5_AMUXSEGMENT                AMUXBUS_SYS
#define P6_6_PORT                       GPIO_PRT6
#define P6_6_PIN                        6u
#define P6_6_NUM                        6u
#define P6_6_AMUXSEGMENT                AMUXBUS_SYS

/* PORT 7 (GPIO) */
#define P7_0_PORT                       GPIO_PRT7
#define P7_0_PIN                        0u
#define P7_0_NUM                        0u
#define P7_0_AMUXSEGMENT                AMUXBUS_MSC
#define P7_1_PORT                       GPIO_PRT7
#define P7_1_PIN                        1u
#define P7_1_NUM                        1u
#define P7_1_AMUXSEGMENT                AMUXBUS_MSC

/* Analog Connections */
#define LPCOMP_IN_N0_PORT               1u
#define LPCOMP_IN_N0_PIN                2u
#define LPCOMP_IN_N1_PORT               1u
#define LPCOMP_IN_N1_PIN                4u
#define LPCOMP_IN_P0_PORT               1u
#define LPCOMP_IN_P0_PIN                1u
#define LPCOMP_IN_P1_PORT               1u
#define LPCOMP_IN_P1_PIN                3u
#define MSC0_CMOD1PADD_PORT             4u
#define MSC0_CMOD1PADD_PIN              0u
#define MSC0_CMOD1PADS_PORT             4u
#define MSC0_CMOD1PADS_PIN              0u
#define MSC0_CMOD2PADD_PORT             4u
#define MSC0_CMOD2PADD_PIN              1u
#define MSC0_CMOD2PADS_PORT             4u
#define MSC0_CMOD2PADS_PIN              1u
#define MSC0_CMOD3PADD_PORT             4u
#define MSC0_CMOD3PADD_PIN              2u
#define MSC0_CMOD3PADS_PORT             4u
#define MSC0_CMOD3PADS_PIN              2u
#define MSC0_CMOD4PADD_PORT             4u
#define MSC0_CMOD4PADD_PIN              3u
#define MSC0_CMOD4PADS_PORT             4u
#define MSC0_CMOD4PADS_PIN              3u
#define MSC0_S_PAD0_PORT                4u
#define MSC0_S_PAD0_PIN                 7u
#define MSC0_S_PAD1_PORT                4u
#define MSC0_S_PAD1_PIN                 6u
#define MSC0_S_PAD10_PORT               1u
#define MSC0_S_PAD10_PIN                4u
#define MSC0_S_PAD11_PORT               1u
#define MSC0_S_PAD11_PIN                5u
#define MSC0_S_PAD12_PORT               1u
#define MSC0_S_PAD12_PIN                6u
#define MSC0_S_PAD13_PORT               1u
#define MSC0_S_PAD13_PIN                7u
#define MSC0_S_PAD14_PORT               3u
#define MSC0_S_PAD14_PIN                0u
#define MSC0_S_PAD15_PORT               3u
#define MSC0_S_PAD15_PIN                1u
#define MSC0_S_PAD2_PORT                4u
#define MSC0_S_PAD2_PIN                 5u
#define MSC0_S_PAD3_PORT                4u
#define MSC0_S_PAD3_PIN                 4u
#define MSC0_S_PAD4_PORT                6u
#define MSC0_S_PAD4_PIN                 0u
#define MSC0_S_PAD5_PORT                7u
#define MSC0_S_PAD5_PIN                 0u
#define MSC0_S_PAD6_PORT                7u
#define MSC0_S_PAD6_PIN                 1u
#define MSC0_S_PAD7_PORT                1u
#define MSC0_S_PAD7_PIN                 1u
#define MSC0_S_PAD8_PORT                1u
#define MSC0_S_PAD8_PIN                 2u
#define MSC0_S_PAD9_PORT                1u
#define MSC0_S_PAD9_PIN                 3u
#define PASS0_CTB0_OA0_OUT_10X_PORT     1u
#define PASS0_CTB0_OA0_OUT_10X_PIN      2u
#define PASS0_CTB0_OA1_OUT_10X_PORT     1u
#define PASS0_CTB0_OA1_OUT_10X_PIN      3u
#define PASS0_CTB0_PADS0_PORT           1u
#define PASS0_CTB0_PADS0_PIN            0u
#define PASS0_CTB0_PADS1_PORT           1u
#define PASS0_CTB0_PADS1_PIN            1u
#define PASS0_CTB0_PADS2_PORT           1u
#define PASS0_CTB0_PADS2_PIN            2u
#define PASS0_CTB0_PADS3_PORT           1u
#define PASS0_CTB0_PADS3_PIN            3u
#define PASS0_CTB0_PADS4_PORT           1u
#define PASS0_CTB0_PADS4_PIN            4u
#define PASS0_CTB0_PADS5_PORT           1u
#define PASS0_CTB0_PADS5_PIN            5u
#define PASS0_CTB0_PADS6_PORT           1u
#define PASS0_CTB0_PADS6_PIN            6u
#define PASS0_CTB0_PADS7_PORT           1u
#define PASS0_CTB0_PADS7_PIN            7u
#define PASS0_EXPMUX_PADS0_PORT         3u
#define PASS0_EXPMUX_PADS0_PIN          0u
#define PASS0_EXPMUX_PADS1_PORT         3u
#define PASS0_EXPMUX_PADS1_PIN          1u
#define PASS0_EXPMUX_PADS2_PORT         3u
#define PASS0_EXPMUX_PADS2_PIN          2u
#define PASS0_EXPMUX_PADS3_PORT         3u
#define PASS0_EXPMUX_PADS3_PIN          3u
#define PASS0_EXPMUX_PADS4_PORT         3u
#define PASS0_EXPMUX_PADS4_PIN          4u
#define PASS0_EXPMUX_PADS5_PORT         3u
#define PASS0_EXPMUX_PADS5_PIN          5u
#define PASS0_EXPMUX_PADS6_PORT         3u
#define PASS0_EXPMUX_PADS6_PIN          6u
#define PASS0_EXPMUX_PADS7_PORT         3u
#define PASS0_EXPMUX_PADS7_PIN          7u
#define PASS0_SAR_EXT_VREF0_PORT        1u
#define PASS0_SAR_EXT_VREF0_PIN         0u
#define PASS0_SAR_EXT_VREF1_PORT        1u
#define PASS0_SAR_EXT_VREF1_PIN         0u
#define PASS0_SARMUX_PADS0_PORT         5u
#define PASS0_SARMUX_PADS0_PIN          0u
#define PASS0_SARMUX_PADS1_PORT         5u
#define PASS0_SARMUX_PADS1_PIN          1u
#define PASS0_SARMUX_PADS2_PORT         5u
#define PASS0_SARMUX_PADS2_PIN          2u
#define PASS0_SARMUX_PADS3_PORT         5u
#define PASS0_SARMUX_PADS3_PIN          3u
#define PASS0_SARMUX_PADS4_PORT         5u
#define PASS0_SARMUX_PADS4_PIN          4u
#define PASS0_SARMUX_PADS5_PORT         5u
#define PASS0_SARMUX_PADS5_PIN          5u
#define PASS0_SARMUX_PADS6_PORT         5u
#define PASS0_SARMUX_PADS6_PIN          6u
#define PASS0_SARMUX_PADS7_PORT         5u
#define PASS0_SARMUX_PADS7_PIN          7u
#define SRSS_ADFT_POR_PAD_HV_PORT       1u
#define SRSS_ADFT_POR_PAD_HV_PIN        0u

/* HSIOM Connections */
typedef enum
{
    /* Generic HSIOM connections */
    HSIOM_SEL_GPIO                  =  0,       /* SW controlled GPIO. */
    HSIOM_SEL_GPIO_DSI              =  1,       /* SW controlled 'out', DSI controlled 'oe_n'. */
    HSIOM_SEL_DSI_DSI               =  2,       /* DSI controlled 'out' and 'oe_n'. */
    HSIOM_SEL_DSI_GPIO              =  3,       /* DSI controlled 'out', SW controlled 'oe_n'. */
    HSIOM_SEL_CSD_SENSE             =  4,       /* CSD sense connection (analog mode) */
    HSIOM_SEL_CSD_SHIELD            =  5,       /* CSD shield connection (analog mode) */
    HSIOM_SEL_AMUXA                 =  6,       /* AMUXBUS A connection. */
    HSIOM_SEL_AMUXB                 =  7,       /* AMUXBUS B connection. */
    HSIOM_SEL_ACT_0                 =  8,       /* Chip specific Active source 0 connection. */
    HSIOM_SEL_ACT_1                 =  9,       /* Chip specific Active source 1 connection. */
    HSIOM_SEL_ACT_2                 = 10,       /* Chip specific Active source 2 connection. */
    HSIOM_SEL_ACT_3                 = 11,       /* Chip specific Active source 3 connection. */
    HSIOM_SEL_LCD_COM               = 12,       /* LCD common connection. */
    HSIOM_SEL_LCD_SEG               = 13,       /* LCD segment connection. */
    HSIOM_SEL_DS_0                  = 12,       /* Chip specific DeepSleep source 0 connection. */
    HSIOM_SEL_DS_1                  = 13,       /* Chip specific DeepSleep source 1 connection. */
    HSIOM_SEL_DS_2                  = 14,       /* Chip specific DeepSleep source 2 connection. */
    HSIOM_SEL_DS_3                  = 15,       /* Chip specific DeepSleep source 3 connection. */

    /* P0.0 */
    P0_0_GPIO                       =  0,       /* SW controlled GPIO. */
    P0_0_MSC0_SENSE                 =  4,       /* msc[0].sense:37 */
    P0_0_MSC0_SHIELD                =  5,       /* msc[0].shield:37 */
    P0_0_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P0_0_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P0_0_PERI_VIRT_IN_0             =  8,       /* Digital Active - peri.virt_in_0 */
    P0_0_LIN_LIN_TX0                =  9,       /* Digital Active - lin.lin_tx[0]:0 */
    P0_0_SCB0_UART_RX               = 10,       /* Digital Active - scb[0].uart_rx:0 */
    P0_0_TCPWM_LINE0                = 11,       /* Digital Active - tcpwm.line[0]:0 */
    P0_0_SCB0_SPI_CLK               = 12,       /* Digital Deep Sleep - scb[0].spi_clk:0 */
    P0_0_SCB0_I2C_SCL               = 14,       /* Digital Deep Sleep - scb[0].i2c_scl:0 */

    /* P0.1 */
    P0_1_GPIO                       =  0,       /* SW controlled GPIO. */
    P0_1_MSC0_SENSE                 =  4,       /* msc[0].sense:36 */
    P0_1_MSC0_SHIELD                =  5,       /* msc[0].shield:36 */
    P0_1_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P0_1_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P0_1_SRSS_EXT_CLK               =  8,       /* Digital Active - srss.ext_clk */
    P0_1_LIN_LIN_RX0                =  9,       /* Digital Active - lin.lin_rx[0]:0 */
    P0_1_SCB0_UART_TX               = 10,       /* Digital Active - scb[0].uart_tx:0 */
    P0_1_TCPWM_LINE_COMPL0          = 11,       /* Digital Active - tcpwm.line_compl[0]:0 */
    P0_1_SCB0_SPI_MOSI              = 12,       /* Digital Deep Sleep - scb[0].spi_mosi:0 */
    P0_1_SCB0_I2C_SDA               = 14,       /* Digital Deep Sleep - scb[0].i2c_sda:0 */

    /* P0.2 */
    P0_2_GPIO                       =  0,       /* SW controlled GPIO. */
    P0_2_MSC0_SENSE                 =  4,       /* msc[0].sense:35 */
    P0_2_MSC0_SHIELD                =  5,       /* msc[0].shield:35 */
    P0_2_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P0_2_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P0_2_TCPWM_TR_IN3               =  8,       /* Digital Active - tcpwm.tr_in[3] */
    P0_2_LIN_LIN_EN0                =  9,       /* Digital Active - lin.lin_en[0]:0 */
    P0_2_SCB0_UART_RTS              = 10,       /* Digital Active - scb[0].uart_rts:0 */
    P0_2_TCPWM_LINE1                = 11,       /* Digital Active - tcpwm.line[1]:0 */
    P0_2_SCB0_SPI_MISO              = 12,       /* Digital Deep Sleep - scb[0].spi_miso:0 */
    P0_2_SCB0_I2C_SCL               = 14,       /* Digital Deep Sleep - scb[0].i2c_scl:1 */

    /* P0.3 */
    P0_3_GPIO                       =  0,       /* SW controlled GPIO. */
    P0_3_MSC0_SENSE                 =  4,       /* msc[0].sense:34 */
    P0_3_MSC0_SHIELD                =  5,       /* msc[0].shield:34 */
    P0_3_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P0_3_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P0_3_TCPWM_TR_IN2               =  8,       /* Digital Active - tcpwm.tr_in[2] */
    P0_3_SCB0_UART_CTS              = 10,       /* Digital Active - scb[0].uart_cts:0 */
    P0_3_TCPWM_LINE_COMPL1          = 11,       /* Digital Active - tcpwm.line_compl[1]:0 */
    P0_3_SCB0_SPI_SELECT0           = 12,       /* Digital Deep Sleep - scb[0].spi_select0 */
    P0_3_SCB0_I2C_SDA               = 14,       /* Digital Deep Sleep - scb[0].i2c_sda:1 */

    /* P0.4 */
    P0_4_GPIO                       =  0,       /* SW controlled GPIO. */
    P0_4_MSC0_SENSE                 =  4,       /* msc[0].sense:33 */
    P0_4_MSC0_SHIELD                =  5,       /* msc[0].shield:33 */
    P0_4_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P0_4_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P0_4_TCPWM_TR_IN1               =  8,       /* Digital Active - tcpwm.tr_in[1] */
    P0_4_PASS0_DSI_SAR_DATA10       = 10,       /* Digital Active - pass[0].dsi_sar_data[10] */
    P0_4_TCPWM_LINE2                = 11,       /* Digital Active - tcpwm.line[2]:0 */
    P0_4_SCB0_SPI_SELECT1           = 12,       /* Digital Deep Sleep - scb[0].spi_select1 */

    /* P0.5 */
    P0_5_GPIO                       =  0,       /* SW controlled GPIO. */
    P0_5_MSC0_SENSE                 =  4,       /* msc[0].sense:32 */
    P0_5_MSC0_SHIELD                =  5,       /* msc[0].shield:32 */
    P0_5_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P0_5_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P0_5_TCPWM_TR_IN0               =  8,       /* Digital Active - tcpwm.tr_in[0] */
    P0_5_PASS0_DSI_SAR_DATA9        = 10,       /* Digital Active - pass[0].dsi_sar_data[9] */
    P0_5_TCPWM_LINE_COMPL2          = 11,       /* Digital Active - tcpwm.line_compl[2]:0 */
    P0_5_SCB0_SPI_SELECT2           = 12,       /* Digital Deep Sleep - scb[0].spi_select2 */

    /* P0.6 */
    P0_6_GPIO                       =  0,       /* SW controlled GPIO. */
    P0_6_MSC0_SENSE                 =  4,       /* msc[0].sense:31 */
    P0_6_MSC0_SHIELD                =  5,       /* msc[0].shield:31 */
    P0_6_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P0_6_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P0_6_PASS0_DSI_SAR_DATA8        = 10,       /* Digital Active - pass[0].dsi_sar_data[8] */
    P0_6_TCPWM_LINE0                = 11,       /* Digital Active - tcpwm.line[0]:3 */
    P0_6_SCB0_SPI_SELECT3           = 12,       /* Digital Deep Sleep - scb[0].spi_select3 */

    /* P0.7 */
    P0_7_GPIO                       =  0,       /* SW controlled GPIO. */
    P0_7_MSC0_SENSE                 =  4,       /* msc[0].sense:30 */
    P0_7_MSC0_SHIELD                =  5,       /* msc[0].shield:30 */
    P0_7_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P0_7_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P0_7_PASS0_DSI_SAR_DATA7        = 10,       /* Digital Active - pass[0].dsi_sar_data[7] */
    P0_7_TCPWM_LINE_COMPL0          = 11,       /* Digital Active - tcpwm.line_compl[0]:3 */
    P0_7_SCB1_SPI_CLK               = 12,       /* Digital Deep Sleep - scb[1].spi_clk:1 */

    /* P1.0 */
    P1_0_GPIO                       =  0,       /* SW controlled GPIO. */
    P1_0_MSC0_SENSE                 =  4,       /* msc[0].sense:10 */
    P1_0_MSC0_SHIELD                =  5,       /* msc[0].shield:10 */
    P1_0_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P1_0_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P1_0_CXPI_CXPI_EN0              =  8,       /* Digital Active - cxpi.cxpi_en[0]:1 */
    P1_0_HVSS_LIN_ALT_EN            =  9,       /* Digital Active - hvss.lin_alt_en */
    P1_0_SCB1_UART_CTS              = 10,       /* Digital Active - scb[1].uart_cts:1 */
    P1_0_TCPWM_LINE2                = 11,       /* Digital Active - tcpwm.line[2]:2 */
    P1_0_SCB3_SPI_SELECT3           = 12,       /* Digital Deep Sleep - scb[3].spi_select3 */
    P1_0_CPUSS_FAULT_OUT0           = 14,       /* Digital Deep Sleep - cpuss.fault_out[0] */

    /* P1.1 */
    P1_1_GPIO                       =  0,       /* SW controlled GPIO. */
    P1_1_MSC0_SENSE                 =  4,       /* msc[0].sense:11 */
    P1_1_MSC0_SHIELD                =  5,       /* msc[0].shield:11 */
    P1_1_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P1_1_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P1_1_MSC0_EXT_SYNC              =  8,       /* Digital Active - msc[0].ext_sync */
    P1_1_HVSS_LIN_ALT_TXD           =  9,       /* Digital Active - hvss.lin_alt_txd */
    P1_1_SCB1_UART_RTS              = 10,       /* Digital Active - scb[1].uart_rts:1 */
    P1_1_TCPWM_LINE_COMPL3          = 11,       /* Digital Active - tcpwm.line_compl[3]:2 */
    P1_1_SCB1_SPI_CLK               = 12,       /* Digital Deep Sleep - scb[1].spi_clk:0 */
    P1_1_CPUSS_FAULT_OUT1           = 14,       /* Digital Deep Sleep - cpuss.fault_out[1] */

    /* P1.2 */
    P1_2_GPIO                       =  0,       /* SW controlled GPIO. */
    P1_2_MSC0_SENSE                 =  4,       /* msc[0].sense:12 */
    P1_2_MSC0_SHIELD                =  5,       /* msc[0].shield:12 */
    P1_2_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P1_2_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P1_2_MSC0_EXT_SYNC_CLK          =  8,       /* Digital Active - msc[0].ext_sync_clk */
    P1_2_HVSS_LIN_ALT_RXD           =  9,       /* Digital Active - hvss.lin_alt_rxd */
    P1_2_SCB1_UART_TX               = 10,       /* Digital Active - scb[1].uart_tx:0 */
    P1_2_TCPWM_LINE3                = 11,       /* Digital Active - tcpwm.line[3]:2 */
    P1_2_SCB1_SPI_SELECT1           = 12,       /* Digital Deep Sleep - scb[1].spi_select1 */
    P1_2_SCB1_I2C_SDA               = 14,       /* Digital Deep Sleep - scb[1].i2c_sda:0 */

    /* P1.3 */
    P1_3_GPIO                       =  0,       /* SW controlled GPIO. */
    P1_3_MSC0_SENSE                 =  4,       /* msc[0].sense:13 */
    P1_3_MSC0_SHIELD                =  5,       /* msc[0].shield:13 */
    P1_3_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P1_3_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P1_3_MSC0_EXT_FRM_START         =  8,       /* Digital Active - msc[0].ext_frm_start */
    P1_3_LIN_LIN_EN0                =  9,       /* Digital Active - lin.lin_en[0]:1 */
    P1_3_SCB1_UART_RX               = 10,       /* Digital Active - scb[1].uart_rx:0 */
    P1_3_TCPWM_LINE_COMPL3          = 11,       /* Digital Active - tcpwm.line_compl[3]:1 */
    P1_3_SCB1_SPI_SELECT0           = 12,       /* Digital Deep Sleep - scb[1].spi_select0 */
    P1_3_SCB1_I2C_SCL               = 14,       /* Digital Deep Sleep - scb[1].i2c_scl:0 */

    /* P1.4 */
    P1_4_GPIO                       =  0,       /* SW controlled GPIO. */
    P1_4_MSC0_SENSE                 =  4,       /* msc[0].sense:14 */
    P1_4_MSC0_SHIELD                =  5,       /* msc[0].shield:14 */
    P1_4_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P1_4_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P1_4_PERI_VIRT_IN_2             =  8,       /* Digital Active - peri.virt_in_2 */
    P1_4_SCB1_UART_CTS              = 10,       /* Digital Active - scb[1].uart_cts:0 */
    P1_4_TCPWM_LINE_COMPL4          = 11,       /* Digital Active - tcpwm.line_compl[4]:1 */
    P1_4_SCB0_SPI_MOSI              = 12,       /* Digital Deep Sleep - scb[0].spi_mosi:1 */
    P1_4_CPUSS_SWD_CLK              = 14,       /* Digital Deep Sleep - cpuss.swd_clk */

    /* P1.5 */
    P1_5_GPIO                       =  0,       /* SW controlled GPIO. */
    P1_5_MSC0_SENSE                 =  4,       /* msc[0].sense:15 */
    P1_5_MSC0_SHIELD                =  5,       /* msc[0].shield:15 */
    P1_5_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P1_5_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P1_5_PERI_VIRT_IN_1             =  8,       /* Digital Active - peri.virt_in_1 */
    P1_5_SCB1_UART_RTS              = 10,       /* Digital Active - scb[1].uart_rts:0 */
    P1_5_TCPWM_LINE3                = 11,       /* Digital Active - tcpwm.line[3]:1 */
    P1_5_SCB0_SPI_CLK               = 12,       /* Digital Deep Sleep - scb[0].spi_clk:1 */
    P1_5_CPUSS_SWD_DATA             = 14,       /* Digital Deep Sleep - cpuss.swd_data */
    P1_5_LPCOMP_COMP0               = 15,       /* Digital Deep Sleep - lpcomp.comp[0]:1 */

    /* P1.6 */
    P1_6_GPIO                       =  0,       /* SW controlled GPIO. */
    P1_6_MSC0_SENSE                 =  4,       /* msc[0].sense:16 */
    P1_6_MSC0_SHIELD                =  5,       /* msc[0].shield:16 */
    P1_6_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P1_6_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P1_6_PERI_VIRT_IN_3             =  8,       /* Digital Active - peri.virt_in_3 */
    P1_6_TCPWM_LINE4                = 11,       /* Digital Active - tcpwm.line[4]:1 */
    P1_6_SCB1_SPI_SELECT2           = 12,       /* Digital Deep Sleep - scb[1].spi_select2 */

    /* P1.7 */
    P1_7_GPIO                       =  0,       /* SW controlled GPIO. */
    P1_7_MSC0_SENSE                 =  4,       /* msc[0].sense:17 */
    P1_7_MSC0_SHIELD                =  5,       /* msc[0].shield:17 */
    P1_7_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P1_7_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P1_7_PASS0_DSI_SAR_DATA0        = 10,       /* Digital Active - pass[0].dsi_sar_data[0] */
    P1_7_SCB1_SPI_SELECT3           = 12,       /* Digital Deep Sleep - scb[1].spi_select3 */

    /* P3.0 */
    P3_0_GPIO                       =  0,       /* SW controlled GPIO. */
    P3_0_MSC0_SENSE                 =  4,       /* msc[0].sense:18 */
    P3_0_MSC0_SHIELD                =  5,       /* msc[0].shield:18 */
    P3_0_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P3_0_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P3_0_MSC0_OBS_DATA0             =  8,       /* Digital Active - msc[0].obs_data[0] */
    P3_0_PASS0_DSI_SAR_DATA1        = 10,       /* Digital Active - pass[0].dsi_sar_data[1] */
    P3_0_TCPWM_LINE_COMPL4          = 11,       /* Digital Active - tcpwm.line_compl[4]:0 */
    P3_0_SCB0_SPI_MISO              = 12,       /* Digital Deep Sleep - scb[0].spi_miso:1 */
    P3_0_LPCOMP_COMP1               = 15,       /* Digital Deep Sleep - lpcomp.comp[1]:1 */

    /* P3.1 */
    P3_1_GPIO                       =  0,       /* SW controlled GPIO. */
    P3_1_MSC0_SENSE                 =  4,       /* msc[0].sense:19 */
    P3_1_MSC0_SHIELD                =  5,       /* msc[0].shield:19 */
    P3_1_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P3_1_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P3_1_MSC0_OBS_DATA1             =  8,       /* Digital Active - msc[0].obs_data[1] */
    P3_1_PASS0_DSI_SAR_DATA2        = 10,       /* Digital Active - pass[0].dsi_sar_data[2] */
    P3_1_TCPWM_LINE4                = 11,       /* Digital Active - tcpwm.line[4]:0 */

    /* P3.2 */
    P3_2_GPIO                       =  0,       /* SW controlled GPIO. */
    P3_2_MSC0_SENSE                 =  4,       /* msc[0].sense:20 */
    P3_2_MSC0_SHIELD                =  5,       /* msc[0].shield:20 */
    P3_2_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P3_2_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P3_2_MSC0_OBS_DATA2             =  8,       /* Digital Active - msc[0].obs_data[2] */
    P3_2_PASS0_DSI_SAR_DATA3        = 10,       /* Digital Active - pass[0].dsi_sar_data[3] */
    P3_2_TCPWM_LINE_COMPL3          = 11,       /* Digital Active - tcpwm.line_compl[3]:0 */

    /* P3.3 */
    P3_3_GPIO                       =  0,       /* SW controlled GPIO. */
    P3_3_MSC0_SENSE                 =  4,       /* msc[0].sense:21 */
    P3_3_MSC0_SHIELD                =  5,       /* msc[0].shield:21 */
    P3_3_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P3_3_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P3_3_MSC0_OBS_DATA3             =  8,       /* Digital Active - msc[0].obs_data[3] */
    P3_3_PASS0_DSI_SAR_DATA4        = 10,       /* Digital Active - pass[0].dsi_sar_data[4] */
    P3_3_TCPWM_LINE3                = 11,       /* Digital Active - tcpwm.line[3]:0 */

    /* P3.4 */
    P3_4_GPIO                       =  0,       /* SW controlled GPIO. */
    P3_4_MSC0_SENSE                 =  4,       /* msc[0].sense:22 */
    P3_4_MSC0_SHIELD                =  5,       /* msc[0].shield:22 */
    P3_4_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P3_4_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P3_4_PASS0_DSI_SAR_DATA5        = 10,       /* Digital Active - pass[0].dsi_sar_data[5] */
    P3_4_TCPWM_LINE_COMPL1          = 11,       /* Digital Active - tcpwm.line_compl[1]:3 */
    P3_4_SCB1_SPI_MISO              = 12,       /* Digital Deep Sleep - scb[1].spi_miso:1 */

    /* P3.5 */
    P3_5_GPIO                       =  0,       /* SW controlled GPIO. */
    P3_5_MSC0_SENSE                 =  4,       /* msc[0].sense:23 */
    P3_5_MSC0_SHIELD                =  5,       /* msc[0].shield:23 */
    P3_5_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P3_5_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P3_5_PASS0_DSI_SAR_DATA6        = 10,       /* Digital Active - pass[0].dsi_sar_data[6] */
    P3_5_TCPWM_LINE1                = 11,       /* Digital Active - tcpwm.line[1]:3 */
    P3_5_SCB1_SPI_MOSI              = 12,       /* Digital Deep Sleep - scb[1].spi_mosi:1 */

    /* P3.6 */
    P3_6_GPIO                       =  0,       /* SW controlled GPIO. */
    P3_6_MSC0_SENSE                 =  4,       /* msc[0].sense:24 */
    P3_6_MSC0_SHIELD                =  5,       /* msc[0].shield:24 */
    P3_6_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P3_6_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P3_6_SCB2_UART_TX               = 10,       /* Digital Active - scb[2].uart_tx */
    P3_6_TCPWM_LINE_COMPL7          = 11,       /* Digital Active - tcpwm.line_compl[7]:0 */

    /* P3.7 */
    P3_7_GPIO                       =  0,       /* SW controlled GPIO. */
    P3_7_MSC0_SENSE                 =  4,       /* msc[0].sense:25 */
    P3_7_MSC0_SHIELD                =  5,       /* msc[0].shield:25 */
    P3_7_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P3_7_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P3_7_SCB2_UART_RX               = 10,       /* Digital Active - scb[2].uart_rx */
    P3_7_TCPWM_LINE7                = 11,       /* Digital Active - tcpwm.line[7]:0 */

    /* P4.0 */
    P4_0_GPIO                       =  0,       /* SW controlled GPIO. */
    P4_0_MSC0_SENSE                 =  4,       /* msc[0].sense:45 */
    P4_0_MSC0_SHIELD                =  5,       /* msc[0].shield:45 */
    P4_0_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P4_0_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P4_0_CXPI_CXPI_RX0              =  8,       /* Digital Active - cxpi.cxpi_rx[0]:0 */
    P4_0_SCB0_UART_RX               = 10,       /* Digital Active - scb[0].uart_rx:1 */
    P4_0_TCPWM_LINE4                = 11,       /* Digital Active - tcpwm.line[4]:2 */
    P4_0_SCB2_SPI_CLK               = 12,       /* Digital Deep Sleep - scb[2].spi_clk */

    /* P4.1 */
    P4_1_GPIO                       =  0,       /* SW controlled GPIO. */
    P4_1_MSC0_SENSE                 =  4,       /* msc[0].sense:44 */
    P4_1_MSC0_SHIELD                =  5,       /* msc[0].shield:44 */
    P4_1_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P4_1_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P4_1_CXPI_CXPI_TX0              =  8,       /* Digital Active - cxpi.cxpi_tx[0]:0 */
    P4_1_SCB0_UART_TX               = 10,       /* Digital Active - scb[0].uart_tx:1 */
    P4_1_TCPWM_LINE_COMPL4          = 11,       /* Digital Active - tcpwm.line_compl[4]:2 */
    P4_1_SCB2_SPI_MOSI              = 12,       /* Digital Deep Sleep - scb[2].spi_mosi */

    /* P4.2 */
    P4_2_GPIO                       =  0,       /* SW controlled GPIO. */
    P4_2_MSC0_SENSE                 =  4,       /* msc[0].sense:43 */
    P4_2_MSC0_SHIELD                =  5,       /* msc[0].shield:43 */
    P4_2_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P4_2_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P4_2_CXPI_CXPI_EN0              =  8,       /* Digital Active - cxpi.cxpi_en[0]:0 */
    P4_2_SCB0_UART_RTS              = 10,       /* Digital Active - scb[0].uart_rts:1 */
    P4_2_TCPWM_LINE0                = 11,       /* Digital Active - tcpwm.line[0]:1 */
    P4_2_SCB2_SPI_MISO              = 12,       /* Digital Deep Sleep - scb[2].spi_miso */

    /* P4.3 */
    P4_3_GPIO                       =  0,       /* SW controlled GPIO. */
    P4_3_MSC0_SENSE                 =  4,       /* msc[0].sense:42 */
    P4_3_MSC0_SHIELD                =  5,       /* msc[0].shield:42 */
    P4_3_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P4_3_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P4_3_CXPI_CXPI_RX0              =  8,       /* Digital Active - cxpi.cxpi_rx[0]:3 */
    P4_3_LIN_LIN_TX0                =  9,       /* Digital Active - lin.lin_tx[0]:3 */
    P4_3_SCB0_UART_CTS              = 10,       /* Digital Active - scb[0].uart_cts:1 */
    P4_3_TCPWM_LINE_COMPL0          = 11,       /* Digital Active - tcpwm.line_compl[0]:1 */
    P4_3_SCB2_SPI_SELECT0           = 12,       /* Digital Deep Sleep - scb[2].spi_select0 */

    /* P4.4 */
    P4_4_GPIO                       =  0,       /* SW controlled GPIO. */
    P4_4_MSC0_SENSE                 =  4,       /* msc[0].sense:41 */
    P4_4_MSC0_SHIELD                =  5,       /* msc[0].shield:41 */
    P4_4_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P4_4_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P4_4_CXPI_CXPI_TX0              =  8,       /* Digital Active - cxpi.cxpi_tx[0]:3 */
    P4_4_LIN_LIN_RX0                =  9,       /* Digital Active - lin.lin_rx[0]:3 */
    P4_4_PASS0_DSI_SAR_DATA_VALID   = 10,       /* Digital Active - pass[0].dsi_sar_data_valid */
    P4_4_TCPWM_LINE1                = 11,       /* Digital Active - tcpwm.line[1]:1 */
    P4_4_SCB2_SPI_SELECT1           = 12,       /* Digital Deep Sleep - scb[2].spi_select1 */

    /* P4.5 */
    P4_5_GPIO                       =  0,       /* SW controlled GPIO. */
    P4_5_MSC0_SENSE                 =  4,       /* msc[0].sense:40 */
    P4_5_MSC0_SHIELD                =  5,       /* msc[0].shield:40 */
    P4_5_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P4_5_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P4_5_CXPI_CXPI_EN0              =  8,       /* Digital Active - cxpi.cxpi_en[0]:3 */
    P4_5_LIN_LIN_EN0                =  9,       /* Digital Active - lin.lin_en[0]:3 */
    P4_5_PASS0_DSI_SAR_SAMPLE_DONE  = 10,       /* Digital Active - pass[0].dsi_sar_sample_done */
    P4_5_TCPWM_LINE_COMPL1          = 11,       /* Digital Active - tcpwm.line_compl[1]:1 */
    P4_5_SCB2_SPI_SELECT2           = 12,       /* Digital Deep Sleep - scb[2].spi_select2 */
    P4_5_LPCOMP_COMP1               = 15,       /* Digital Deep Sleep - lpcomp.comp[1]:2 */

    /* P4.6 */
    P4_6_GPIO                       =  0,       /* SW controlled GPIO. */
    P4_6_MSC0_SENSE                 =  4,       /* msc[0].sense:39 */
    P4_6_MSC0_SHIELD                =  5,       /* msc[0].shield:39 */
    P4_6_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P4_6_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P4_6_PASS0_TR_SAR_OUT           = 10,       /* Digital Active - pass[0].tr_sar_out */
    P4_6_TCPWM_LINE2                = 11,       /* Digital Active - tcpwm.line[2]:1 */
    P4_6_SCB2_SPI_SELECT3           = 12,       /* Digital Deep Sleep - scb[2].spi_select3 */
    P4_6_LPCOMP_COMP0               = 15,       /* Digital Deep Sleep - lpcomp.comp[0]:2 */

    /* P4.7 */
    P4_7_GPIO                       =  0,       /* SW controlled GPIO. */
    P4_7_MSC0_SENSE                 =  4,       /* msc[0].sense:38 */
    P4_7_MSC0_SHIELD                =  5,       /* msc[0].shield:38 */
    P4_7_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P4_7_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P4_7_PASS0_DSI_SAR_DATA11       = 10,       /* Digital Active - pass[0].dsi_sar_data[11] */
    P4_7_TCPWM_LINE_COMPL2          = 11,       /* Digital Active - tcpwm.line_compl[2]:1 */

    /* P5.0 */
    P5_0_GPIO                       =  0,       /* SW controlled GPIO. */
    P5_0_MSC0_SENSE                 =  4,       /* msc[0].sense:2 */
    P5_0_MSC0_SHIELD                =  5,       /* msc[0].shield:2 */
    P5_0_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P5_0_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P5_0_CXPI_CXPI_TX0              =  8,       /* Digital Active - cxpi.cxpi_tx[0]:1 */
    P5_0_LIN_LIN_TX0                =  9,       /* Digital Active - lin.lin_tx[0]:1 */
    P5_0_SCB1_UART_TX               = 10,       /* Digital Active - scb[1].uart_tx:1 */
    P5_0_TCPWM_LINE_COMPL0          = 11,       /* Digital Active - tcpwm.line_compl[0]:2 */
    P5_0_SCB1_SPI_MISO              = 12,       /* Digital Deep Sleep - scb[1].spi_miso:0 */
    P5_0_SCB1_I2C_SDA               = 14,       /* Digital Deep Sleep - scb[1].i2c_sda:1 */
    P5_0_LPCOMP_COMP0               = 15,       /* Digital Deep Sleep - lpcomp.comp[0]:0 */

    /* P5.1 */
    P5_1_GPIO                       =  0,       /* SW controlled GPIO. */
    P5_1_MSC0_SENSE                 =  4,       /* msc[0].sense:3 */
    P5_1_MSC0_SHIELD                =  5,       /* msc[0].shield:3 */
    P5_1_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P5_1_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P5_1_CXPI_CXPI_RX0              =  8,       /* Digital Active - cxpi.cxpi_rx[0]:1 */
    P5_1_LIN_LIN_RX0                =  9,       /* Digital Active - lin.lin_rx[0]:1 */
    P5_1_SCB1_UART_RX               = 10,       /* Digital Active - scb[1].uart_rx:1 */
    P5_1_TCPWM_LINE0                = 11,       /* Digital Active - tcpwm.line[0]:2 */
    P5_1_SCB1_SPI_MOSI              = 12,       /* Digital Deep Sleep - scb[1].spi_mosi:0 */
    P5_1_SCB1_I2C_SCL               = 14,       /* Digital Deep Sleep - scb[1].i2c_scl:1 */
    P5_1_LPCOMP_COMP1               = 15,       /* Digital Deep Sleep - lpcomp.comp[1]:0 */

    /* P5.2 */
    P5_2_GPIO                       =  0,       /* SW controlled GPIO. */
    P5_2_MSC0_SENSE                 =  4,       /* msc[0].sense:4 */
    P5_2_MSC0_SHIELD                =  5,       /* msc[0].shield:4 */
    P5_2_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P5_2_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P5_2_CXPI_CXPI_EN0              =  8,       /* Digital Active - cxpi.cxpi_en[0]:2 */
    P5_2_LIN_LIN_EN0                =  9,       /* Digital Active - lin.lin_en[0]:2 */
    P5_2_SCB1_UART_TX               = 10,       /* Digital Active - scb[1].uart_tx:2 */
    P5_2_TCPWM_LINE_COMPL1          = 11,       /* Digital Active - tcpwm.line_compl[1]:2 */
    P5_2_SCB3_SPI_MISO              = 12,       /* Digital Deep Sleep - scb[3].spi_miso */

    /* P5.3 */
    P5_3_GPIO                       =  0,       /* SW controlled GPIO. */
    P5_3_MSC0_SENSE                 =  4,       /* msc[0].sense:5 */
    P5_3_MSC0_SHIELD                =  5,       /* msc[0].shield:5 */
    P5_3_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P5_3_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P5_3_CXPI_CXPI_TX0              =  8,       /* Digital Active - cxpi.cxpi_tx[0]:2 */
    P5_3_LIN_LIN_RX0                =  9,       /* Digital Active - lin.lin_rx[0]:2 */
    P5_3_SCB1_UART_RX               = 10,       /* Digital Active - scb[1].uart_rx:2 */
    P5_3_TCPWM_LINE1                = 11,       /* Digital Active - tcpwm.line[1]:2 */
    P5_3_SCB3_SPI_MOSI              = 12,       /* Digital Deep Sleep - scb[3].spi_mosi */

    /* P5.4 */
    P5_4_GPIO                       =  0,       /* SW controlled GPIO. */
    P5_4_MSC0_SENSE                 =  4,       /* msc[0].sense:6 */
    P5_4_MSC0_SHIELD                =  5,       /* msc[0].shield:6 */
    P5_4_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P5_4_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P5_4_CXPI_CXPI_RX0              =  8,       /* Digital Active - cxpi.cxpi_rx[0]:2 */
    P5_4_LIN_LIN_TX0                =  9,       /* Digital Active - lin.lin_tx[0]:2 */
    P5_4_SCB1_UART_CTS              = 10,       /* Digital Active - scb[1].uart_cts:2 */
    P5_4_TCPWM_LINE_COMPL2          = 11,       /* Digital Active - tcpwm.line_compl[2]:2 */
    P5_4_SCB3_SPI_CLK               = 12,       /* Digital Deep Sleep - scb[3].spi_clk */

    /* P5.5 */
    P5_5_GPIO                       =  0,       /* SW controlled GPIO. */
    P5_5_MSC0_SENSE                 =  4,       /* msc[0].sense:7 */
    P5_5_MSC0_SHIELD                =  5,       /* msc[0].shield:7 */
    P5_5_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P5_5_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P5_5_SCB1_UART_RTS              = 10,       /* Digital Active - scb[1].uart_rts:2 */
    P5_5_TCPWM_LINE_COMPL2          = 11,       /* Digital Active - tcpwm.line_compl[2]:3 */
    P5_5_SCB3_SPI_SELECT0           = 12,       /* Digital Deep Sleep - scb[3].spi_select0 */

    /* P5.6 */
    P5_6_GPIO                       =  0,       /* SW controlled GPIO. */
    P5_6_MSC0_SENSE                 =  4,       /* msc[0].sense:8 */
    P5_6_MSC0_SHIELD                =  5,       /* msc[0].shield:8 */
    P5_6_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P5_6_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P5_6_TCPWM_LINE2                = 11,       /* Digital Active - tcpwm.line[2]:3 */
    P5_6_SCB3_SPI_SELECT1           = 12,       /* Digital Deep Sleep - scb[3].spi_select1 */

    /* P5.7 */
    P5_7_GPIO                       =  0,       /* SW controlled GPIO. */
    P5_7_MSC0_SENSE                 =  4,       /* msc[0].sense:9 */
    P5_7_MSC0_SHIELD                =  5,       /* msc[0].shield:9 */
    P5_7_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P5_7_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P5_7_SCB3_SPI_SELECT2           = 12,       /* Digital Deep Sleep - scb[3].spi_select2 */

    /* P6.0 */
    P6_0_GPIO                       =  0,       /* SW controlled GPIO. */
    P6_0_MSC0_SENSE                 =  4,       /* msc[0].sense:46 */
    P6_0_MSC0_SHIELD                =  5,       /* msc[0].shield:46 */
    P6_0_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P6_0_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P6_0_SCB3_UART_TX               = 10,       /* Digital Active - scb[3].uart_tx */
    P6_0_TCPWM_LINE_COMPL5          = 11,       /* Digital Active - tcpwm.line_compl[5]:0 */

    /* P6.1 */
    P6_1_GPIO                       =  0,       /* SW controlled GPIO. */
    P6_1_MSC0_SENSE                 =  4,       /* msc[0].sense:47 */
    P6_1_MSC0_SHIELD                =  5,       /* msc[0].shield:47 */
    P6_1_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P6_1_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P6_1_SCB3_UART_RX               = 10,       /* Digital Active - scb[3].uart_rx */
    P6_1_TCPWM_LINE5                = 11,       /* Digital Active - tcpwm.line[5]:0 */
    P6_1_SCB3_I2C_SDA               = 14,       /* Digital Deep Sleep - scb[3].i2c_sda */

    /* P6.2 */
    P6_2_GPIO                       =  0,       /* SW controlled GPIO. */
    P6_2_MSC0_SENSE                 =  4,       /* msc[0].sense:48 */
    P6_2_MSC0_SHIELD                =  5,       /* msc[0].shield:48 */
    P6_2_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P6_2_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P6_2_SCB3_I2C_SCL               = 14,       /* Digital Deep Sleep - scb[3].i2c_scl */

    /* P6.3 */
    P6_3_GPIO                       =  0,       /* SW controlled GPIO. */
    P6_3_MSC0_SENSE                 =  4,       /* msc[0].sense:26 */
    P6_3_MSC0_SHIELD                =  5,       /* msc[0].shield:26 */
    P6_3_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P6_3_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P6_3_SCB2_UART_RTS              = 10,       /* Digital Active - scb[2].uart_rts */
    P6_3_TCPWM_LINE_COMPL6          = 11,       /* Digital Active - tcpwm.line_compl[6]:0 */

    /* P6.4 */
    P6_4_GPIO                       =  0,       /* SW controlled GPIO. */
    P6_4_MSC0_SENSE                 =  4,       /* msc[0].sense:27 */
    P6_4_MSC0_SHIELD                =  5,       /* msc[0].shield:27 */
    P6_4_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P6_4_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P6_4_SCB2_UART_CTS              = 10,       /* Digital Active - scb[2].uart_cts */
    P6_4_TCPWM_LINE6                = 11,       /* Digital Active - tcpwm.line[6]:0 */

    /* P6.5 */
    P6_5_GPIO                       =  0,       /* SW controlled GPIO. */
    P6_5_MSC0_SENSE                 =  4,       /* msc[0].sense:28 */
    P6_5_MSC0_SHIELD                =  5,       /* msc[0].shield:28 */
    P6_5_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P6_5_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P6_5_SCB3_UART_RTS              = 10,       /* Digital Active - scb[3].uart_rts */
    P6_5_SCB2_I2C_SCL               = 14,       /* Digital Deep Sleep - scb[2].i2c_scl */

    /* P6.6 */
    P6_6_GPIO                       =  0,       /* SW controlled GPIO. */
    P6_6_MSC0_SENSE                 =  4,       /* msc[0].sense:29 */
    P6_6_MSC0_SHIELD                =  5,       /* msc[0].shield:29 */
    P6_6_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P6_6_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P6_6_SCB3_UART_CTS              = 10,       /* Digital Active - scb[3].uart_cts */
    P6_6_SCB2_I2C_SDA               = 14,       /* Digital Deep Sleep - scb[2].i2c_sda */

    /* P7.0 */
    P7_0_GPIO                       =  0,       /* SW controlled GPIO. */
    P7_0_MSC0_SENSE                 =  4,       /* msc[0].sense:0 */
    P7_0_MSC0_SHIELD                =  5,       /* msc[0].shield:0 */
    P7_0_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P7_0_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P7_0_PASS0_DSI_CTB_CMP0         = 13,       /* Digital Deep Sleep - pass[0].dsi_ctb_cmp0 */

    /* P7.1 */
    P7_1_GPIO                       =  0,       /* SW controlled GPIO. */
    P7_1_MSC0_SENSE                 =  4,       /* msc[0].sense:1 */
    P7_1_MSC0_SHIELD                =  5,       /* msc[0].shield:1 */
    P7_1_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P7_1_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P7_1_PASS0_DSI_CTB_CMP1         = 13        /* Digital Deep Sleep - pass[0].dsi_ctb_cmp1 */
} en_hsiom_sel_t;

#endif /* _GPIO_PSOC4HVMS128K_64_QFN_H_ */


/* [] END OF FILE */
