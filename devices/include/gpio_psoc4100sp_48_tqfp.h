/***************************************************************************//**
* \file gpio_psoc4100sp_48_tqfp.h
*
* \brief
* PSOC4100Sp device GPIO header for 48-TQFP package
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

#ifndef _GPIO_PSOC4100SP_48_TQFP_H_
#define _GPIO_PSOC4100SP_48_TQFP_H_

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

#define CY_GPIO_PACKAGE_TYPE            CY_GPIO_PACKAGE_TQFP
#define CY_GPIO_PIN_COUNT               48u

/* AMUXBUS Segments */
enum
{
    AMUXBUS_CSD,
    AMUXBUS_PASS,
};

/* AMUX Splitter Controls */
typedef enum
{
    AMUX_SPLIT_CTL_0                = 0x0000u,  /* Left = AMUXBUS_CSD; Right = AMUXBUS_PASS */
    AMUX_SPLIT_CTL_1                = 0x0001u   /* Left = AMUXBUS_PASS; Right = AMUXBUS_CSD */
} cy_en_amux_split_t;

/* Port List */
/* PORT 0 (GPIO) */
#define P0_0_PORT                       GPIO_PRT0
#define P0_0_PIN                        0u
#define P0_0_NUM                        0u
#define P0_0_AMUXSEGMENT                AMUXBUS_CSD
#define P0_1_PORT                       GPIO_PRT0
#define P0_1_PIN                        1u
#define P0_1_NUM                        1u
#define P0_1_AMUXSEGMENT                AMUXBUS_CSD
#define P0_2_PORT                       GPIO_PRT0
#define P0_2_PIN                        2u
#define P0_2_NUM                        2u
#define P0_2_AMUXSEGMENT                AMUXBUS_CSD
#define P0_3_PORT                       GPIO_PRT0
#define P0_3_PIN                        3u
#define P0_3_NUM                        3u
#define P0_3_AMUXSEGMENT                AMUXBUS_CSD
#define P0_4_PORT                       GPIO_PRT0
#define P0_4_PIN                        4u
#define P0_4_NUM                        4u
#define P0_4_AMUXSEGMENT                AMUXBUS_CSD
#define P0_5_PORT                       GPIO_PRT0
#define P0_5_PIN                        5u
#define P0_5_NUM                        5u
#define P0_5_AMUXSEGMENT                AMUXBUS_CSD
#define P0_6_PORT                       GPIO_PRT0
#define P0_6_PIN                        6u
#define P0_6_NUM                        6u
#define P0_6_AMUXSEGMENT                AMUXBUS_CSD
#define P0_7_PORT                       GPIO_PRT0
#define P0_7_PIN                        7u
#define P0_7_NUM                        7u
#define P0_7_AMUXSEGMENT                AMUXBUS_CSD

/* PORT 1 (GPIO) */
#define P1_0_PORT                       GPIO_PRT1
#define P1_0_PIN                        0u
#define P1_0_NUM                        0u
#define P1_0_AMUXSEGMENT                AMUXBUS_PASS
#define P1_1_PORT                       GPIO_PRT1
#define P1_1_PIN                        1u
#define P1_1_NUM                        1u
#define P1_1_AMUXSEGMENT                AMUXBUS_PASS
#define P1_2_PORT                       GPIO_PRT1
#define P1_2_PIN                        2u
#define P1_2_NUM                        2u
#define P1_2_AMUXSEGMENT                AMUXBUS_PASS
#define P1_3_PORT                       GPIO_PRT1
#define P1_3_PIN                        3u
#define P1_3_NUM                        3u
#define P1_3_AMUXSEGMENT                AMUXBUS_PASS
#define P1_4_PORT                       GPIO_PRT1
#define P1_4_PIN                        4u
#define P1_4_NUM                        4u
#define P1_4_AMUXSEGMENT                AMUXBUS_PASS
#define P1_5_PORT                       GPIO_PRT1
#define P1_5_PIN                        5u
#define P1_5_NUM                        5u
#define P1_5_AMUXSEGMENT                AMUXBUS_PASS
#define P1_6_PORT                       GPIO_PRT1
#define P1_6_PIN                        6u
#define P1_6_NUM                        6u
#define P1_6_AMUXSEGMENT                AMUXBUS_PASS
#define P1_7_PORT                       GPIO_PRT1
#define P1_7_PIN                        7u
#define P1_7_NUM                        7u
#define P1_7_AMUXSEGMENT                AMUXBUS_PASS

/* PORT 2 (GPIO) */
#define P2_0_PORT                       GPIO_PRT2
#define P2_0_PIN                        0u
#define P2_0_NUM                        0u
#define P2_0_AMUXSEGMENT                AMUXBUS_PASS
#define P2_1_PORT                       GPIO_PRT2
#define P2_1_PIN                        1u
#define P2_1_NUM                        1u
#define P2_1_AMUXSEGMENT                AMUXBUS_PASS
#define P2_2_PORT                       GPIO_PRT2
#define P2_2_PIN                        2u
#define P2_2_NUM                        2u
#define P2_2_AMUXSEGMENT                AMUXBUS_PASS
#define P2_3_PORT                       GPIO_PRT2
#define P2_3_PIN                        3u
#define P2_3_NUM                        3u
#define P2_3_AMUXSEGMENT                AMUXBUS_PASS
#define P2_4_PORT                       GPIO_PRT2
#define P2_4_PIN                        4u
#define P2_4_NUM                        4u
#define P2_4_AMUXSEGMENT                AMUXBUS_PASS
#define P2_5_PORT                       GPIO_PRT2
#define P2_5_PIN                        5u
#define P2_5_NUM                        5u
#define P2_5_AMUXSEGMENT                AMUXBUS_PASS
#define P2_6_PORT                       GPIO_PRT2
#define P2_6_PIN                        6u
#define P2_6_NUM                        6u
#define P2_6_AMUXSEGMENT                AMUXBUS_PASS
#define P2_7_PORT                       GPIO_PRT2
#define P2_7_PIN                        7u
#define P2_7_NUM                        7u
#define P2_7_AMUXSEGMENT                AMUXBUS_PASS

/* PORT 3 (GPIO) */
#define P3_0_PORT                       GPIO_PRT3
#define P3_0_PIN                        0u
#define P3_0_NUM                        0u
#define P3_0_AMUXSEGMENT                AMUXBUS_CSD
#define P3_1_PORT                       GPIO_PRT3
#define P3_1_PIN                        1u
#define P3_1_NUM                        1u
#define P3_1_AMUXSEGMENT                AMUXBUS_CSD
#define P3_2_PORT                       GPIO_PRT3
#define P3_2_PIN                        2u
#define P3_2_NUM                        2u
#define P3_2_AMUXSEGMENT                AMUXBUS_CSD
#define P3_3_PORT                       GPIO_PRT3
#define P3_3_PIN                        3u
#define P3_3_NUM                        3u
#define P3_3_AMUXSEGMENT                AMUXBUS_CSD
#define P3_4_PORT                       GPIO_PRT3
#define P3_4_PIN                        4u
#define P3_4_NUM                        4u
#define P3_4_AMUXSEGMENT                AMUXBUS_CSD
#define P3_5_PORT                       GPIO_PRT3
#define P3_5_PIN                        5u
#define P3_5_NUM                        5u
#define P3_5_AMUXSEGMENT                AMUXBUS_CSD
#define P3_6_PORT                       GPIO_PRT3
#define P3_6_PIN                        6u
#define P3_6_NUM                        6u
#define P3_6_AMUXSEGMENT                AMUXBUS_CSD
#define P3_7_PORT                       GPIO_PRT3
#define P3_7_PIN                        7u
#define P3_7_NUM                        7u
#define P3_7_AMUXSEGMENT                AMUXBUS_CSD

/* PORT 4 (GPIO) */
#define P4_0_PORT                       GPIO_PRT4
#define P4_0_PIN                        0u
#define P4_0_NUM                        0u
#define P4_0_AMUXSEGMENT                AMUXBUS_CSD
#define P4_1_PORT                       GPIO_PRT4
#define P4_1_PIN                        1u
#define P4_1_NUM                        1u
#define P4_1_AMUXSEGMENT                AMUXBUS_CSD
#define P4_2_PORT                       GPIO_PRT4
#define P4_2_PIN                        2u
#define P4_2_NUM                        2u
#define P4_2_AMUXSEGMENT                AMUXBUS_CSD
#define P4_3_PORT                       GPIO_PRT4
#define P4_3_PIN                        3u
#define P4_3_NUM                        3u
#define P4_3_AMUXSEGMENT                AMUXBUS_CSD

/* PORT 7 (GPIO) */
#define P7_0_PORT                       GPIO_PRT7
#define P7_0_PIN                        0u
#define P7_0_NUM                        0u
#define P7_0_AMUXSEGMENT                AMUXBUS_CSD
#define P7_1_PORT                       GPIO_PRT7
#define P7_1_PIN                        1u
#define P7_1_NUM                        1u
#define P7_1_AMUXSEGMENT                AMUXBUS_CSD

/* Analog Connections */
#define CSD_CMODPADD_PORT               4u
#define CSD_CMODPADD_PIN                2u
#define CSD_CMODPADS_PORT               4u
#define CSD_CMODPADS_PIN                2u
#define CSD_CSH_TANKPADD_PORT           4u
#define CSD_CSH_TANKPADD_PIN            3u
#define CSD_CSH_TANKPADS_PORT           4u
#define CSD_CSH_TANKPADS_PIN            3u
#define CSD_CSHIELDPADS_PORT            4u
#define CSD_CSHIELDPADS_PIN             1u
#define CSD_VREF_EXT_PORT               4u
#define CSD_VREF_EXT_PIN                0u
#define CSD_VREF_EXT_HSCOMP_PORT        4u
#define CSD_VREF_EXT_HSCOMP_PIN         0u
#define EXCO_ECO_IN_PORT                0u
#define EXCO_ECO_IN_PIN                 6u
#define EXCO_ECO_OUT_PORT               0u
#define EXCO_ECO_OUT_PIN                7u
#define LPCOMP_IN_N0_PORT               0u
#define LPCOMP_IN_N0_PIN                1u
#define LPCOMP_IN_N1_PORT               0u
#define LPCOMP_IN_N1_PIN                3u
#define LPCOMP_IN_P0_PORT               0u
#define LPCOMP_IN_P0_PIN                0u
#define LPCOMP_IN_P1_PORT               0u
#define LPCOMP_IN_P1_PIN                2u
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
#define PASS0_SAR_EXT_VREF0_PORT        1u
#define PASS0_SAR_EXT_VREF0_PIN         7u
#define PASS0_SAR_EXT_VREF1_PORT        1u
#define PASS0_SAR_EXT_VREF1_PIN         7u
#define PASS0_SARMUX_PADS0_PORT         2u
#define PASS0_SARMUX_PADS0_PIN          0u
#define PASS0_SARMUX_PADS1_PORT         2u
#define PASS0_SARMUX_PADS1_PIN          1u
#define PASS0_SARMUX_PADS2_PORT         2u
#define PASS0_SARMUX_PADS2_PIN          2u
#define PASS0_SARMUX_PADS3_PORT         2u
#define PASS0_SARMUX_PADS3_PIN          3u
#define PASS0_SARMUX_PADS4_PORT         2u
#define PASS0_SARMUX_PADS4_PIN          4u
#define PASS0_SARMUX_PADS5_PORT         2u
#define PASS0_SARMUX_PADS5_PIN          5u
#define PASS0_SARMUX_PADS6_PORT         2u
#define PASS0_SARMUX_PADS6_PIN          6u
#define PASS0_SARMUX_PADS7_PORT         2u
#define PASS0_SARMUX_PADS7_PIN          7u
#define SRSS_ADFT_POR_PAD_HV_PORT       0u
#define SRSS_ADFT_POR_PAD_HV_PIN        6u
#define WCO_WCO_IN_PORT                 0u
#define WCO_WCO_IN_PIN                  4u
#define WCO_WCO_OUT_PORT                0u
#define WCO_WCO_OUT_PIN                 5u

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
    P0_0_CSD_SENSE                  =  4,       /* csd.sense:0 */
    P0_0_CSD_SHIELD                 =  5,       /* csd.shield:0 */
    P0_0_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P0_0_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P0_0_TCPWM_TR_IN0               =  9,       /* Digital Active - tcpwm.tr_in[0] */
    P0_0_PASS0_DSI_SAR_DATA_VALID   = 10,       /* Digital Active - pass[0].dsi_sar_data_valid */
    P0_0_SCB2_UART_CTS              = 11,       /* Digital Active - scb[2].uart_cts:0 */
    P0_0_LCD_COM0                   = 12,       /* Digital Deep Sleep - lcd.com[0] */
    P0_0_LCD_SEG0                   = 13,       /* Digital Deep Sleep - lcd.seg[0] */
    P0_0_SCB2_I2C_SCL               = 14,       /* Digital Deep Sleep - scb[2].i2c_scl:0 */
    P0_0_SCB0_SPI_SELECT1           = 15,       /* Digital Deep Sleep - scb[0].spi_select1:0 */

    /* P0.1 */
    P0_1_GPIO                       =  0,       /* SW controlled GPIO. */
    P0_1_CSD_SENSE                  =  4,       /* csd.sense:1 */
    P0_1_CSD_SHIELD                 =  5,       /* csd.shield:1 */
    P0_1_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P0_1_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P0_1_TCPWM_TR_IN1               =  9,       /* Digital Active - tcpwm.tr_in[1] */
    P0_1_PASS0_TR_SAR_OUT           = 10,       /* Digital Active - pass[0].tr_sar_out */
    P0_1_SCB2_UART_RTS              = 11,       /* Digital Active - scb[2].uart_rts:0 */
    P0_1_LCD_COM1                   = 12,       /* Digital Deep Sleep - lcd.com[1] */
    P0_1_LCD_SEG1                   = 13,       /* Digital Deep Sleep - lcd.seg[1] */
    P0_1_SCB2_I2C_SDA               = 14,       /* Digital Deep Sleep - scb[2].i2c_sda:0 */
    P0_1_SCB0_SPI_SELECT2           = 15,       /* Digital Deep Sleep - scb[0].spi_select2:0 */

    /* P0.2 */
    P0_2_GPIO                       =  0,       /* SW controlled GPIO. */
    P0_2_CSD_SENSE                  =  4,       /* csd.sense:2 */
    P0_2_CSD_SHIELD                 =  5,       /* csd.shield:2 */
    P0_2_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P0_2_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P0_2_PASS0_DSI_SAR_SAMPLE_DONE  = 10,       /* Digital Active - pass[0].dsi_sar_sample_done */
    P0_2_LCD_COM2                   = 12,       /* Digital Deep Sleep - lcd.com[2] */
    P0_2_LCD_SEG2                   = 13,       /* Digital Deep Sleep - lcd.seg[2] */
    P0_2_SCB0_SPI_SELECT3           = 15,       /* Digital Deep Sleep - scb[0].spi_select3:0 */

    /* P0.3 */
    P0_3_GPIO                       =  0,       /* SW controlled GPIO. */
    P0_3_CSD_SENSE                  =  4,       /* csd.sense:3 */
    P0_3_CSD_SHIELD                 =  5,       /* csd.shield:3 */
    P0_3_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P0_3_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P0_3_PASS0_DSI_SAR_DATA2        = 10,       /* Digital Active - pass[0].dsi_sar_data[2] */
    P0_3_LCD_COM3                   = 12,       /* Digital Deep Sleep - lcd.com[3]:0 */
    P0_3_LCD_SEG3                   = 13,       /* Digital Deep Sleep - lcd.seg[3] */
    P0_3_SCB2_SPI_SELECT0           = 15,       /* Digital Deep Sleep - scb[2].spi_select0:1 */

    /* P0.4 */
    P0_4_GPIO                       =  0,       /* SW controlled GPIO. */
    P0_4_CSD_SENSE                  =  4,       /* csd.sense:4 */
    P0_4_CSD_SHIELD                 =  5,       /* csd.shield:4 */
    P0_4_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P0_4_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P0_4_SCB1_UART_RX               =  9,       /* Digital Active - scb[1].uart_rx:0 */
    P0_4_PASS0_DSI_SAR_DATA0        = 10,       /* Digital Active - pass[0].dsi_sar_data[0] */
    P0_4_SCB2_UART_RX               = 11,       /* Digital Active - scb[2].uart_rx:0 */
    P0_4_LCD_COM4                   = 12,       /* Digital Deep Sleep - lcd.com[4] */
    P0_4_LCD_SEG4                   = 13,       /* Digital Deep Sleep - lcd.seg[4] */
    P0_4_SCB1_I2C_SCL               = 14,       /* Digital Deep Sleep - scb[1].i2c_scl:0 */
    P0_4_SCB1_SPI_MOSI              = 15,       /* Digital Deep Sleep - scb[1].spi_mosi:1 */

    /* P0.5 */
    P0_5_GPIO                       =  0,       /* SW controlled GPIO. */
    P0_5_CSD_SENSE                  =  4,       /* csd.sense:5 */
    P0_5_CSD_SHIELD                 =  5,       /* csd.shield:5 */
    P0_5_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P0_5_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P0_5_SCB1_UART_TX               =  9,       /* Digital Active - scb[1].uart_tx:0 */
    P0_5_PASS0_DSI_SAR_DATA1        = 10,       /* Digital Active - pass[0].dsi_sar_data[1] */
    P0_5_SCB2_UART_TX               = 11,       /* Digital Active - scb[2].uart_tx:0 */
    P0_5_LCD_COM5                   = 12,       /* Digital Deep Sleep - lcd.com[5] */
    P0_5_LCD_SEG5                   = 13,       /* Digital Deep Sleep - lcd.seg[5] */
    P0_5_SCB1_I2C_SDA               = 14,       /* Digital Deep Sleep - scb[1].i2c_sda:0 */
    P0_5_SCB1_SPI_MISO              = 15,       /* Digital Deep Sleep - scb[1].spi_miso:1 */

    /* P0.6 */
    P0_6_GPIO                       =  0,       /* SW controlled GPIO. */
    P0_6_CSD_SENSE                  =  4,       /* csd.sense:6 */
    P0_6_CSD_SHIELD                 =  5,       /* csd.shield:6 */
    P0_6_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P0_6_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P0_6_SRSS_EXT_CLK               =  8,       /* Digital Active - srss.ext_clk */
    P0_6_SCB1_UART_CTS              =  9,       /* Digital Active - scb[1].uart_cts:0 */
    P0_6_SCB2_UART_TX               = 11,       /* Digital Active - scb[2].uart_tx:1 */
    P0_6_LCD_COM6                   = 12,       /* Digital Deep Sleep - lcd.com[6] */
    P0_6_LCD_SEG6                   = 13,       /* Digital Deep Sleep - lcd.seg[6] */
    P0_6_SCB1_SPI_CLK               = 15,       /* Digital Deep Sleep - scb[1].spi_clk:1 */

    /* P0.7 */
    P0_7_GPIO                       =  0,       /* SW controlled GPIO. */
    P0_7_CSD_SENSE                  =  4,       /* csd.sense:7 */
    P0_7_CSD_SHIELD                 =  5,       /* csd.shield:7 */
    P0_7_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P0_7_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P0_7_TCPWM_LINE0                =  8,       /* Digital Active - tcpwm.line[0]:3 */
    P0_7_SCB1_UART_RTS              =  9,       /* Digital Active - scb[1].uart_rts:0 */
    P0_7_LCD_COM7                   = 12,       /* Digital Deep Sleep - lcd.com[7] */
    P0_7_LCD_SEG7                   = 13,       /* Digital Deep Sleep - lcd.seg[7] */
    P0_7_SCB1_SPI_SELECT0           = 15,       /* Digital Deep Sleep - scb[1].spi_select0:1 */

    /* P1.0 */
    P1_0_GPIO                       =  0,       /* SW controlled GPIO. */
    P1_0_CSD_SENSE                  =  4,       /* csd.sense:14 */
    P1_0_CSD_SHIELD                 =  5,       /* csd.shield:14 */
    P1_0_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P1_0_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P1_0_TCPWM_LINE2                =  8,       /* Digital Active - tcpwm.line[2]:1 */
    P1_0_SCB0_UART_RX               =  9,       /* Digital Active - scb[0].uart_rx:1 */
    P1_0_LCD_COM8                   = 12,       /* Digital Deep Sleep - lcd.com[8] */
    P1_0_LCD_SEG8                   = 13,       /* Digital Deep Sleep - lcd.seg[8] */
    P1_0_SCB0_I2C_SCL               = 14,       /* Digital Deep Sleep - scb[0].i2c_scl:0 */
    P1_0_SCB0_SPI_MOSI              = 15,       /* Digital Deep Sleep - scb[0].spi_mosi:1 */

    /* P1.1 */
    P1_1_GPIO                       =  0,       /* SW controlled GPIO. */
    P1_1_CSD_SENSE                  =  4,       /* csd.sense:15 */
    P1_1_CSD_SHIELD                 =  5,       /* csd.shield:15 */
    P1_1_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P1_1_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P1_1_TCPWM_LINE_COMPL2          =  8,       /* Digital Active - tcpwm.line_compl[2]:1 */
    P1_1_SCB0_UART_TX               =  9,       /* Digital Active - scb[0].uart_tx:1 */
    P1_1_LCD_COM9                   = 12,       /* Digital Deep Sleep - lcd.com[9] */
    P1_1_LCD_SEG9                   = 13,       /* Digital Deep Sleep - lcd.seg[9] */
    P1_1_SCB0_I2C_SDA               = 14,       /* Digital Deep Sleep - scb[0].i2c_sda:0 */
    P1_1_SCB0_SPI_MISO              = 15,       /* Digital Deep Sleep - scb[0].spi_miso:1 */

    /* P1.2 */
    P1_2_GPIO                       =  0,       /* SW controlled GPIO. */
    P1_2_CSD_SENSE                  =  4,       /* csd.sense:16 */
    P1_2_CSD_SHIELD                 =  5,       /* csd.shield:16 */
    P1_2_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P1_2_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P1_2_TCPWM_LINE3                =  8,       /* Digital Active - tcpwm.line[3]:1 */
    P1_2_SCB0_UART_CTS              =  9,       /* Digital Active - scb[0].uart_cts:1 */
    P1_2_PASS0_DSI_SAR_DATA3        = 10,       /* Digital Active - pass[0].dsi_sar_data[3]:0 */
    P1_2_TCPWM_TR_IN2               = 11,       /* Digital Active - tcpwm.tr_in[2] */
    P1_2_LCD_COM10                  = 12,       /* Digital Deep Sleep - lcd.com[10] */
    P1_2_LCD_SEG10                  = 13,       /* Digital Deep Sleep - lcd.seg[10] */
    P1_2_SCB2_I2C_SCL               = 14,       /* Digital Deep Sleep - scb[2].i2c_scl:2 */
    P1_2_SCB0_SPI_CLK               = 15,       /* Digital Deep Sleep - scb[0].spi_clk:1 */

    /* P1.3 */
    P1_3_GPIO                       =  0,       /* SW controlled GPIO. */
    P1_3_CSD_SENSE                  =  4,       /* csd.sense:17 */
    P1_3_CSD_SHIELD                 =  5,       /* csd.shield:17 */
    P1_3_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P1_3_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P1_3_TCPWM_LINE_COMPL3          =  8,       /* Digital Active - tcpwm.line_compl[3]:1 */
    P1_3_SCB0_UART_RTS              =  9,       /* Digital Active - scb[0].uart_rts:1 */
    P1_3_PASS0_DSI_SAR_DATA4        = 10,       /* Digital Active - pass[0].dsi_sar_data[4]:0 */
    P1_3_TCPWM_TR_IN3               = 11,       /* Digital Active - tcpwm.tr_in[3] */
    P1_3_LCD_COM11                  = 12,       /* Digital Deep Sleep - lcd.com[11] */
    P1_3_LCD_SEG11                  = 13,       /* Digital Deep Sleep - lcd.seg[11] */
    P1_3_SCB2_I2C_SDA               = 14,       /* Digital Deep Sleep - scb[2].i2c_sda:2 */
    P1_3_SCB0_SPI_SELECT0           = 15,       /* Digital Deep Sleep - scb[0].spi_select0:1 */

    /* P1.4 */
    P1_4_GPIO                       =  0,       /* SW controlled GPIO. */
    P1_4_CSD_SENSE                  =  4,       /* csd.sense:18 */
    P1_4_CSD_SHIELD                 =  5,       /* csd.shield:18 */
    P1_4_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P1_4_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P1_4_TCPWM_LINE6                =  8,       /* Digital Active - tcpwm.line[6]:1 */
    P1_4_LCD_COM12                  = 12,       /* Digital Deep Sleep - lcd.com[12] */
    P1_4_LCD_SEG12                  = 13,       /* Digital Deep Sleep - lcd.seg[12] */
    P1_4_SCB3_I2C_SCL               = 14,       /* Digital Deep Sleep - scb[3].i2c_scl:0 */
    P1_4_SCB0_SPI_SELECT1           = 15,       /* Digital Deep Sleep - scb[0].spi_select1:1 */

    /* P1.5 */
    P1_5_GPIO                       =  0,       /* SW controlled GPIO. */
    P1_5_CSD_SENSE                  =  4,       /* csd.sense:19 */
    P1_5_CSD_SHIELD                 =  5,       /* csd.shield:19 */
    P1_5_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P1_5_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P1_5_TCPWM_LINE_COMPL6          =  8,       /* Digital Active - tcpwm.line_compl[6]:1 */
    P1_5_LCD_COM13                  = 12,       /* Digital Deep Sleep - lcd.com[13] */
    P1_5_LCD_SEG13                  = 13,       /* Digital Deep Sleep - lcd.seg[13] */
    P1_5_SCB3_I2C_SDA               = 14,       /* Digital Deep Sleep - scb[3].i2c_sda:0 */
    P1_5_SCB0_SPI_SELECT2           = 15,       /* Digital Deep Sleep - scb[0].spi_select2:1 */

    /* P1.6 */
    P1_6_GPIO                       =  0,       /* SW controlled GPIO. */
    P1_6_CSD_SENSE                  =  4,       /* csd.sense:20 */
    P1_6_CSD_SHIELD                 =  5,       /* csd.shield:20 */
    P1_6_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P1_6_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P1_6_TCPWM_LINE7                =  8,       /* Digital Active - tcpwm.line[7]:1 */
    P1_6_LCD_COM14                  = 12,       /* Digital Deep Sleep - lcd.com[14] */
    P1_6_LCD_SEG14                  = 13,       /* Digital Deep Sleep - lcd.seg[14] */
    P1_6_SCB0_SPI_SELECT3           = 15,       /* Digital Deep Sleep - scb[0].spi_select3:1 */

    /* P1.7 */
    P1_7_GPIO                       =  0,       /* SW controlled GPIO. */
    P1_7_CSD_SENSE                  =  4,       /* csd.sense:21 */
    P1_7_CSD_SHIELD                 =  5,       /* csd.shield:21 */
    P1_7_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P1_7_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P1_7_TCPWM_LINE_COMPL7          =  8,       /* Digital Active - tcpwm.line_compl[7]:1 */
    P1_7_LCD_COM15                  = 12,       /* Digital Deep Sleep - lcd.com[15] */
    P1_7_LCD_SEG15                  = 13,       /* Digital Deep Sleep - lcd.seg[15] */
    P1_7_SCB2_SPI_CLK               = 15,       /* Digital Deep Sleep - scb[2].spi_clk:1 */

    /* P2.0 */
    P2_0_GPIO                       =  0,       /* SW controlled GPIO. */
    P2_0_CSD_SENSE                  =  4,       /* csd.sense:22 */
    P2_0_CSD_SHIELD                 =  5,       /* csd.shield:22 */
    P2_0_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P2_0_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P2_0_TCPWM_LINE4                =  8,       /* Digital Active - tcpwm.line[4]:0 */
    P2_0_CSD_COMP                   =  9,       /* Digital Active - csd.comp */
    P2_0_TCPWM_TR_IN4               = 11,       /* Digital Active - tcpwm.tr_in[4] */
    P2_0_LCD_COM16                  = 12,       /* Digital Deep Sleep - lcd.com[16] */
    P2_0_LCD_SEG16                  = 13,       /* Digital Deep Sleep - lcd.seg[16] */
    P2_0_SCB1_I2C_SCL               = 14,       /* Digital Deep Sleep - scb[1].i2c_scl:1 */
    P2_0_SCB1_SPI_MOSI              = 15,       /* Digital Deep Sleep - scb[1].spi_mosi:2 */

    /* P2.1 */
    P2_1_GPIO                       =  0,       /* SW controlled GPIO. */
    P2_1_CSD_SENSE                  =  4,       /* csd.sense:23 */
    P2_1_CSD_SHIELD                 =  5,       /* csd.shield:23 */
    P2_1_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P2_1_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P2_1_TCPWM_LINE_COMPL4          =  8,       /* Digital Active - tcpwm.line_compl[4]:0 */
    P2_1_TCPWM_TR_IN5               = 11,       /* Digital Active - tcpwm.tr_in[5] */
    P2_1_LCD_COM17                  = 12,       /* Digital Deep Sleep - lcd.com[17] */
    P2_1_LCD_SEG17                  = 13,       /* Digital Deep Sleep - lcd.seg[17] */
    P2_1_SCB1_I2C_SDA               = 14,       /* Digital Deep Sleep - scb[1].i2c_sda:1 */
    P2_1_SCB1_SPI_MISO              = 15,       /* Digital Deep Sleep - scb[1].spi_miso:2 */

    /* P2.2 */
    P2_2_GPIO                       =  0,       /* SW controlled GPIO. */
    P2_2_CSD_SENSE                  =  4,       /* csd.sense:24 */
    P2_2_CSD_SHIELD                 =  5,       /* csd.shield:24 */
    P2_2_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P2_2_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P2_2_TCPWM_LINE5                =  8,       /* Digital Active - tcpwm.line[5]:1 */
    P2_2_LCD_COM18                  = 12,       /* Digital Deep Sleep - lcd.com[18] */
    P2_2_LCD_SEG18                  = 13,       /* Digital Deep Sleep - lcd.seg[18] */
    P2_2_SCB1_SPI_CLK               = 15,       /* Digital Deep Sleep - scb[1].spi_clk:2 */

    /* P2.3 */
    P2_3_GPIO                       =  0,       /* SW controlled GPIO. */
    P2_3_CSD_SENSE                  =  4,       /* csd.sense:25 */
    P2_3_CSD_SHIELD                 =  5,       /* csd.shield:25 */
    P2_3_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P2_3_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P2_3_TCPWM_LINE_COMPL5          =  8,       /* Digital Active - tcpwm.line_compl[5]:1 */
    P2_3_LCD_COM19                  = 12,       /* Digital Deep Sleep - lcd.com[19] */
    P2_3_LCD_SEG19                  = 13,       /* Digital Deep Sleep - lcd.seg[19] */
    P2_3_SCB1_SPI_SELECT0           = 15,       /* Digital Deep Sleep - scb[1].spi_select0:2 */

    /* P2.4 */
    P2_4_GPIO                       =  0,       /* SW controlled GPIO. */
    P2_4_CSD_SENSE                  =  4,       /* csd.sense:26 */
    P2_4_CSD_SHIELD                 =  5,       /* csd.shield:26 */
    P2_4_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P2_4_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P2_4_TCPWM_LINE0                =  8,       /* Digital Active - tcpwm.line[0]:1 */
    P2_4_SCB3_UART_RX               =  9,       /* Digital Active - scb[3].uart_rx:1 */
    P2_4_LCD_COM20                  = 12,       /* Digital Deep Sleep - lcd.com[20] */
    P2_4_LCD_SEG20                  = 13,       /* Digital Deep Sleep - lcd.seg[20] */
    P2_4_SCB1_SPI_SELECT1           = 15,       /* Digital Deep Sleep - scb[1].spi_select1:1 */

    /* P2.5 */
    P2_5_GPIO                       =  0,       /* SW controlled GPIO. */
    P2_5_CSD_SENSE                  =  4,       /* csd.sense:27 */
    P2_5_CSD_SHIELD                 =  5,       /* csd.shield:27 */
    P2_5_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P2_5_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P2_5_TCPWM_LINE_COMPL0          =  8,       /* Digital Active - tcpwm.line_compl[0]:1 */
    P2_5_SCB3_UART_TX               =  9,       /* Digital Active - scb[3].uart_tx:1 */
    P2_5_LCD_COM21                  = 12,       /* Digital Deep Sleep - lcd.com[21] */
    P2_5_LCD_SEG21                  = 13,       /* Digital Deep Sleep - lcd.seg[21] */
    P2_5_SCB1_SPI_SELECT2           = 15,       /* Digital Deep Sleep - scb[1].spi_select2:1 */

    /* P2.6 */
    P2_6_GPIO                       =  0,       /* SW controlled GPIO. */
    P2_6_CSD_SENSE                  =  4,       /* csd.sense:28 */
    P2_6_CSD_SHIELD                 =  5,       /* csd.shield:28 */
    P2_6_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P2_6_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P2_6_TCPWM_LINE1                =  8,       /* Digital Active - tcpwm.line[1]:1 */
    P2_6_SCB3_UART_CTS              =  9,       /* Digital Active - scb[3].uart_cts:1 */
    P2_6_PASS0_DSI_SAR_DATA5        = 10,       /* Digital Active - pass[0].dsi_sar_data[5]:0 */
    P2_6_LCD_COM22                  = 12,       /* Digital Deep Sleep - lcd.com[22] */
    P2_6_LCD_SEG22                  = 13,       /* Digital Deep Sleep - lcd.seg[22] */
    P2_6_SCB1_SPI_SELECT3           = 15,       /* Digital Deep Sleep - scb[1].spi_select3:1 */

    /* P2.7 */
    P2_7_GPIO                       =  0,       /* SW controlled GPIO. */
    P2_7_CSD_SENSE                  =  4,       /* csd.sense:29 */
    P2_7_CSD_SHIELD                 =  5,       /* csd.shield:29 */
    P2_7_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P2_7_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P2_7_TCPWM_LINE_COMPL1          =  8,       /* Digital Active - tcpwm.line_compl[1]:1 */
    P2_7_SCB3_UART_RTS              =  9,       /* Digital Active - scb[3].uart_rts:1 */
    P2_7_PASS0_DSI_SAR_DATA6        = 10,       /* Digital Active - pass[0].dsi_sar_data[6]:0 */
    P2_7_LCD_COM23                  = 12,       /* Digital Deep Sleep - lcd.com[23] */
    P2_7_LCD_SEG23                  = 13,       /* Digital Deep Sleep - lcd.seg[23] */
    P2_7_LPCOMP_COMP0               = 14,       /* Digital Deep Sleep - lpcomp.comp[0]:0 */
    P2_7_SCB2_SPI_MOSI              = 15,       /* Digital Deep Sleep - scb[2].spi_mosi:1 */

    /* P3.0 */
    P3_0_GPIO                       =  0,       /* SW controlled GPIO. */
    P3_0_CSD_SENSE                  =  4,       /* csd.sense:36 */
    P3_0_CSD_SHIELD                 =  5,       /* csd.shield:36 */
    P3_0_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P3_0_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P3_0_TCPWM_LINE0                =  8,       /* Digital Active - tcpwm.line[0]:0 */
    P3_0_SCB1_UART_RX               =  9,       /* Digital Active - scb[1].uart_rx:1 */
    P3_0_PASS0_DSI_SAR_DATA7        = 10,       /* Digital Active - pass[0].dsi_sar_data[7]:0 */
    P3_0_LCD_COM24                  = 12,       /* Digital Deep Sleep - lcd.com[24] */
    P3_0_LCD_SEG24                  = 13,       /* Digital Deep Sleep - lcd.seg[24] */
    P3_0_SCB1_I2C_SCL               = 14,       /* Digital Deep Sleep - scb[1].i2c_scl:2 */
    P3_0_SCB1_SPI_MOSI              = 15,       /* Digital Deep Sleep - scb[1].spi_mosi:0 */

    /* P3.1 */
    P3_1_GPIO                       =  0,       /* SW controlled GPIO. */
    P3_1_CSD_SENSE                  =  4,       /* csd.sense:37 */
    P3_1_CSD_SHIELD                 =  5,       /* csd.shield:37 */
    P3_1_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P3_1_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P3_1_TCPWM_LINE_COMPL0          =  8,       /* Digital Active - tcpwm.line_compl[0]:0 */
    P3_1_SCB1_UART_TX               =  9,       /* Digital Active - scb[1].uart_tx:1 */
    P3_1_PASS0_DSI_SAR_DATA8        = 10,       /* Digital Active - pass[0].dsi_sar_data[8]:0 */
    P3_1_LCD_COM25                  = 12,       /* Digital Deep Sleep - lcd.com[25] */
    P3_1_LCD_SEG25                  = 13,       /* Digital Deep Sleep - lcd.seg[25] */
    P3_1_SCB1_I2C_SDA               = 14,       /* Digital Deep Sleep - scb[1].i2c_sda:2 */
    P3_1_SCB1_SPI_MISO              = 15,       /* Digital Deep Sleep - scb[1].spi_miso:0 */

    /* P3.2 */
    P3_2_GPIO                       =  0,       /* SW controlled GPIO. */
    P3_2_CSD_SENSE                  =  4,       /* csd.sense:38 */
    P3_2_CSD_SHIELD                 =  5,       /* csd.shield:38 */
    P3_2_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P3_2_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P3_2_TCPWM_LINE1                =  8,       /* Digital Active - tcpwm.line[1]:0 */
    P3_2_SCB1_UART_CTS              =  9,       /* Digital Active - scb[1].uart_cts:1 */
    P3_2_LCD_COM26                  = 12,       /* Digital Deep Sleep - lcd.com[26] */
    P3_2_LCD_SEG26                  = 13,       /* Digital Deep Sleep - lcd.seg[26] */
    P3_2_CPUSS_SWD_DATA             = 14,       /* Digital Deep Sleep - cpuss.swd_data */
    P3_2_SCB1_SPI_CLK               = 15,       /* Digital Deep Sleep - scb[1].spi_clk:0 */

    /* P3.3 */
    P3_3_GPIO                       =  0,       /* SW controlled GPIO. */
    P3_3_CSD_SENSE                  =  4,       /* csd.sense:39 */
    P3_3_CSD_SHIELD                 =  5,       /* csd.shield:39 */
    P3_3_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P3_3_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P3_3_TCPWM_LINE_COMPL1          =  8,       /* Digital Active - tcpwm.line_compl[1]:0 */
    P3_3_SCB1_UART_RTS              =  9,       /* Digital Active - scb[1].uart_rts:1 */
    P3_3_LCD_COM27                  = 12,       /* Digital Deep Sleep - lcd.com[27] */
    P3_3_LCD_SEG27                  = 13,       /* Digital Deep Sleep - lcd.seg[27] */
    P3_3_CPUSS_SWD_CLK              = 14,       /* Digital Deep Sleep - cpuss.swd_clk */
    P3_3_SCB1_SPI_SELECT0           = 15,       /* Digital Deep Sleep - scb[1].spi_select0:0 */

    /* P3.4 */
    P3_4_GPIO                       =  0,       /* SW controlled GPIO. */
    P3_4_CSD_SENSE                  =  4,       /* csd.sense:40 */
    P3_4_CSD_SHIELD                 =  5,       /* csd.shield:40 */
    P3_4_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P3_4_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P3_4_TCPWM_LINE2                =  8,       /* Digital Active - tcpwm.line[2]:0 */
    P3_4_TCPWM_TR_IN6               = 11,       /* Digital Active - tcpwm.tr_in[6] */
    P3_4_LCD_COM28                  = 12,       /* Digital Deep Sleep - lcd.com[28] */
    P3_4_LCD_SEG28                  = 13,       /* Digital Deep Sleep - lcd.seg[28] */
    P3_4_SCB1_SPI_SELECT1           = 15,       /* Digital Deep Sleep - scb[1].spi_select1:0 */

    /* P3.5 */
    P3_5_GPIO                       =  0,       /* SW controlled GPIO. */
    P3_5_CSD_SENSE                  =  4,       /* csd.sense:41 */
    P3_5_CSD_SHIELD                 =  5,       /* csd.shield:41 */
    P3_5_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P3_5_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P3_5_TCPWM_LINE_COMPL2          =  8,       /* Digital Active - tcpwm.line_compl[2]:0 */
    P3_5_LCD_COM29                  = 12,       /* Digital Deep Sleep - lcd.com[29] */
    P3_5_LCD_SEG29                  = 13,       /* Digital Deep Sleep - lcd.seg[29] */
    P3_5_SCB1_SPI_SELECT2           = 15,       /* Digital Deep Sleep - scb[1].spi_select2:0 */

    /* P3.6 */
    P3_6_GPIO                       =  0,       /* SW controlled GPIO. */
    P3_6_CSD_SENSE                  =  4,       /* csd.sense:42 */
    P3_6_CSD_SHIELD                 =  5,       /* csd.shield:42 */
    P3_6_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P3_6_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P3_6_TCPWM_LINE3                =  8,       /* Digital Active - tcpwm.line[3]:0 */
    P3_6_PASS0_DSI_CTB_CMP0         = 10,       /* Digital Active - pass[0].dsi_ctb_cmp0 */
    P3_6_LCD_COM30                  = 12,       /* Digital Deep Sleep - lcd.com[30] */
    P3_6_LCD_SEG30                  = 13,       /* Digital Deep Sleep - lcd.seg[30] */
    P3_6_SCB4_SPI_SELECT3           = 14,       /* Digital Deep Sleep - scb[4].spi_select3 */
    P3_6_SCB1_SPI_SELECT3           = 15,       /* Digital Deep Sleep - scb[1].spi_select3:0 */

    /* P3.7 */
    P3_7_GPIO                       =  0,       /* SW controlled GPIO. */
    P3_7_CSD_SENSE                  =  4,       /* csd.sense:43 */
    P3_7_CSD_SHIELD                 =  5,       /* csd.shield:43 */
    P3_7_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P3_7_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P3_7_TCPWM_LINE_COMPL3          =  8,       /* Digital Active - tcpwm.line_compl[3]:0 */
    P3_7_PASS0_DSI_CTB_CMP1         = 10,       /* Digital Active - pass[0].dsi_ctb_cmp1 */
    P3_7_LCD_COM31                  = 12,       /* Digital Deep Sleep - lcd.com[31] */
    P3_7_LCD_SEG31                  = 13,       /* Digital Deep Sleep - lcd.seg[31] */
    P3_7_LPCOMP_COMP1               = 14,       /* Digital Deep Sleep - lpcomp.comp[1]:1 */
    P3_7_SCB2_SPI_MISO              = 15,       /* Digital Deep Sleep - scb[2].spi_miso:1 */

    /* P4.0 */
    P4_0_GPIO                       =  0,       /* SW controlled GPIO. */
    P4_0_CSD_SENSE                  =  4,       /* csd.sense:44 */
    P4_0_CSD_SHIELD                 =  5,       /* csd.shield:44 */
    P4_0_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P4_0_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P4_0_SCB0_UART_RX               =  9,       /* Digital Active - scb[0].uart_rx:0 */
    P4_0_PASS0_DSI_SAR_DATA9        = 10,       /* Digital Active - pass[0].dsi_sar_data[9]:0 */
    P4_0_CAN_CAN_RX                 = 11,       /* Digital Active - can.can_rx:1 */
    P4_0_LCD_COM32                  = 12,       /* Digital Deep Sleep - lcd.com[32] */
    P4_0_LCD_SEG32                  = 13,       /* Digital Deep Sleep - lcd.seg[32] */
    P4_0_SCB0_I2C_SCL               = 14,       /* Digital Deep Sleep - scb[0].i2c_scl:1 */
    P4_0_SCB0_SPI_MOSI              = 15,       /* Digital Deep Sleep - scb[0].spi_mosi:0 */

    /* P4.1 */
    P4_1_GPIO                       =  0,       /* SW controlled GPIO. */
    P4_1_CSD_SENSE                  =  4,       /* csd.sense:45 */
    P4_1_CSD_SHIELD                 =  5,       /* csd.shield:45 */
    P4_1_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P4_1_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P4_1_SCB0_UART_TX               =  9,       /* Digital Active - scb[0].uart_tx:0 */
    P4_1_CAN_CAN_TX                 = 11,       /* Digital Active - can.can_tx:1 */
    P4_1_LCD_COM33                  = 12,       /* Digital Deep Sleep - lcd.com[33] */
    P4_1_LCD_SEG33                  = 13,       /* Digital Deep Sleep - lcd.seg[33] */
    P4_1_SCB0_I2C_SDA               = 14,       /* Digital Deep Sleep - scb[0].i2c_sda:1 */
    P4_1_SCB0_SPI_MISO              = 15,       /* Digital Deep Sleep - scb[0].spi_miso:0 */

    /* P4.2 */
    P4_2_GPIO                       =  0,       /* SW controlled GPIO. */
    P4_2_CSD_SENSE                  =  4,       /* csd.sense:46 */
    P4_2_CSD_SHIELD                 =  5,       /* csd.shield:46 */
    P4_2_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P4_2_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P4_2_SCB0_UART_CTS              =  9,       /* Digital Active - scb[0].uart_cts:0 */
    P4_2_PASS0_DSI_SAR_DATA10       = 10,       /* Digital Active - pass[0].dsi_sar_data[10]:0 */
    P4_2_CAN_CAN_TX_ENB_N           = 11,       /* Digital Active - can.can_tx_enb_n:1 */
    P4_2_LCD_COM34                  = 12,       /* Digital Deep Sleep - lcd.com[34] */
    P4_2_LCD_SEG34                  = 13,       /* Digital Deep Sleep - lcd.seg[34] */
    P4_2_LPCOMP_COMP0               = 14,       /* Digital Deep Sleep - lpcomp.comp[0]:1 */
    P4_2_SCB0_SPI_CLK               = 15,       /* Digital Deep Sleep - scb[0].spi_clk:0 */

    /* P4.3 */
    P4_3_GPIO                       =  0,       /* SW controlled GPIO. */
    P4_3_CSD_SENSE                  =  4,       /* csd.sense:47 */
    P4_3_CSD_SHIELD                 =  5,       /* csd.shield:47 */
    P4_3_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P4_3_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P4_3_SCB0_UART_RTS              =  9,       /* Digital Active - scb[0].uart_rts:0 */
    P4_3_PASS0_DSI_SAR_DATA11       = 10,       /* Digital Active - pass[0].dsi_sar_data[11]:0 */
    P4_3_LCD_COM35                  = 12,       /* Digital Deep Sleep - lcd.com[35] */
    P4_3_LCD_SEG35                  = 13,       /* Digital Deep Sleep - lcd.seg[35] */
    P4_3_LPCOMP_COMP1               = 14,       /* Digital Deep Sleep - lpcomp.comp[1]:2 */
    P4_3_SCB0_SPI_SELECT0           = 15,       /* Digital Deep Sleep - scb[0].spi_select0:0 */

    /* P7.0 */
    P7_0_GPIO                       =  0,       /* SW controlled GPIO. */
    P7_0_CSD_SENSE                  =  4,       /* csd.sense:54 */
    P7_0_CSD_SHIELD                 =  5,       /* csd.shield:54 */
    P7_0_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P7_0_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P7_0_TCPWM_LINE0                =  8,       /* Digital Active - tcpwm.line[0]:2 */
    P7_0_SCB3_UART_RX               =  9,       /* Digital Active - scb[3].uart_rx:2 */
    P7_0_LCD_COM54                  = 12,       /* Digital Deep Sleep - lcd.com[54]:0 */
    P7_0_LCD_SEG54                  = 13,       /* Digital Deep Sleep - lcd.seg[54] */
    P7_0_SCB3_I2C_SCL               = 14,       /* Digital Deep Sleep - scb[3].i2c_scl:2 */
    P7_0_SCB3_SPI_MOSI              = 15,       /* Digital Deep Sleep - scb[3].spi_mosi:1 */

    /* P7.1 */
    P7_1_GPIO                       =  0,       /* SW controlled GPIO. */
    P7_1_CSD_SENSE                  =  4,       /* csd.sense:55 */
    P7_1_CSD_SHIELD                 =  5,       /* csd.shield:55 */
    P7_1_AMUXA                      =  6,       /* AMUXBUS A connection. */
    P7_1_AMUXB                      =  7,       /* AMUXBUS B connection. */
    P7_1_TCPWM_LINE_COMPL0          =  8,       /* Digital Active - tcpwm.line_compl[0]:2 */
    P7_1_SCB3_UART_TX               =  9,       /* Digital Active - scb[3].uart_tx:2 */
    P7_1_LCD_COM55                  = 12,       /* Digital Deep Sleep - lcd.com[55]:0 */
    P7_1_LCD_SEG55                  = 13,       /* Digital Deep Sleep - lcd.seg[55] */
    P7_1_SCB3_I2C_SDA               = 14,       /* Digital Deep Sleep - scb[3].i2c_sda:2 */
    P7_1_SCB3_SPI_MISO              = 15        /* Digital Deep Sleep - scb[3].spi_miso:1 */
} en_hsiom_sel_t;

#endif /* _GPIO_PSOC4100SP_48_TQFP_H_ */


/* [] END OF FILE */
