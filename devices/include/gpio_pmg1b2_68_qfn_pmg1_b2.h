/***************************************************************************//**
* PMG1B2 device GPIO header for 68-QFN-PMG1-B2 package
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

#ifndef _GPIO_PMG1B2_68_QFN_PMG1_B2_H_
#define _GPIO_PMG1B2_68_QFN_PMG1_B2_H_

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
#define CY_GPIO_PIN_COUNT               68u

/* AMUXBUS Segments */
enum
{
    AMUXBUS_PD0,
    AMUXBUS_PD1,
};

/* AMUX Splitter Controls */
typedef enum
{
    AMUX_SPLIT_CTL_0                = 0x0000u,  /* Left = AMUXBUS_PD0; Right = AMUXBUS_PD1 */
    AMUX_SPLIT_CTL_1                = 0x0001u   /* Left = AMUXBUS_PD1; Right = AMUXBUS_PD0 */
} cy_en_amux_split_t;

/* Port List */
/* PORT 0 (GPIO_OVT) */
#define P0_0_PORT                       GPIO_PRT0
#define P0_0_PIN                        0u
#define P0_0_NUM                        0u
#define P0_0_AMUXSEGMENT                AMUXBUS_PD0
#define P0_1_PORT                       GPIO_PRT0
#define P0_1_PIN                        1u
#define P0_1_NUM                        1u
#define P0_1_AMUXSEGMENT                AMUXBUS_PD0

/* PORT 1 (GPIO) */
#define P1_0_PORT                       GPIO_PRT1
#define P1_0_PIN                        0u
#define P1_0_NUM                        0u
#define P1_0_AMUXSEGMENT                AMUXBUS_PD0
#define P1_1_PORT                       GPIO_PRT1
#define P1_1_PIN                        1u
#define P1_1_NUM                        1u
#define P1_1_AMUXSEGMENT                AMUXBUS_PD0
#define P1_2_PORT                       GPIO_PRT1
#define P1_2_PIN                        2u
#define P1_2_NUM                        2u
#define P1_2_AMUXSEGMENT                AMUXBUS_PD0
#define P1_3_PORT                       GPIO_PRT1
#define P1_3_PIN                        3u
#define P1_3_NUM                        3u
#define P1_3_AMUXSEGMENT                AMUXBUS_PD0
#define P1_4_PORT                       GPIO_PRT1
#define P1_4_PIN                        4u
#define P1_4_NUM                        4u
#define P1_4_AMUXSEGMENT                AMUXBUS_PD0
#define P1_5_PORT                       GPIO_PRT1
#define P1_5_PIN                        5u
#define P1_5_NUM                        5u
#define P1_5_AMUXSEGMENT                AMUXBUS_PD0
#define P1_7_PORT                       GPIO_PRT1
#define P1_7_PIN                        7u
#define P1_7_NUM                        7u
#define P1_7_AMUXSEGMENT                AMUXBUS_PD0

/* PORT 2 (GPIO) */
#define P2_0_PORT                       GPIO_PRT2
#define P2_0_PIN                        0u
#define P2_0_NUM                        0u
#define P2_0_AMUXSEGMENT                AMUXBUS_PD0
#define P2_1_PORT                       GPIO_PRT2
#define P2_1_PIN                        1u
#define P2_1_NUM                        1u
#define P2_1_AMUXSEGMENT                AMUXBUS_PD0
#define P2_4_PORT                       GPIO_PRT2
#define P2_4_PIN                        4u
#define P2_4_NUM                        4u
#define P2_4_AMUXSEGMENT                AMUXBUS_PD1
#define P2_5_PORT                       GPIO_PRT2
#define P2_5_PIN                        5u
#define P2_5_NUM                        5u
#define P2_5_AMUXSEGMENT                AMUXBUS_PD1
#define P2_6_PORT                       GPIO_PRT2
#define P2_6_PIN                        6u
#define P2_6_NUM                        6u
#define P2_6_AMUXSEGMENT                AMUXBUS_PD1

/* PORT 3 (GPIO) */
#define P3_2_PORT                       GPIO_PRT3
#define P3_2_PIN                        2u
#define P3_2_NUM                        2u
#define P3_2_AMUXSEGMENT                AMUXBUS_PD1
#define P3_3_PORT                       GPIO_PRT3
#define P3_3_PIN                        3u
#define P3_3_NUM                        3u
#define P3_3_AMUXSEGMENT                AMUXBUS_PD1
#define P3_6_PORT                       GPIO_PRT3
#define P3_6_PIN                        6u
#define P3_6_NUM                        6u
#define P3_6_AMUXSEGMENT                AMUXBUS_PD1

/* PORT 4 (GPIO) */
#define P4_0_PORT                       GPIO_PRT4
#define P4_0_PIN                        0u
#define P4_0_NUM                        0u
#define P4_0_AMUXSEGMENT                AMUXBUS_PD1
#define P4_3_PORT                       GPIO_PRT4
#define P4_3_PIN                        3u
#define P4_3_NUM                        3u
#define P4_3_AMUXSEGMENT                AMUXBUS_PD1

/* Analog Connections */
#define SRSS_ADFT_POR_PAD_HV_PORT       1u
#define SRSS_ADFT_POR_PAD_HV_PIN        7u

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
    P0_0_SRSS_EXT_CLK               =  8,       /* Digital Active - srss.ext_clk:1 */
    P0_0_SCB0_UART_CTS              = 10,       /* Digital Active - scb[0].uart_cts */
    P0_0_SCB2_UART_RTS              = 11,       /* Digital Active - scb[2].uart_rts:1 */
    P0_0_CPUSS_SWD_DATA             = 12,       /* Digital Deep Sleep - cpuss.swd_data:1 */
    P0_0_SCB0_SPI_SELECT0           = 13,       /* Digital Deep Sleep - scb[0].spi_select0:0 */
    P0_0_SCB2_SPI_SELECT0           = 14,       /* Digital Deep Sleep - scb[2].spi_select0:1 */
    P0_0_SCB0_I2C_SDA               = 15,       /* Digital Deep Sleep - scb[0].i2c_sda:0 */

    /* P0.1 */
    P0_1_GPIO                       =  0,       /* SW controlled GPIO. */
    P0_1_SCB0_UART_RTS              = 10,       /* Digital Active - scb[0].uart_rts */
    P0_1_SCB3_UART_RTS              = 11,       /* Digital Active - scb[3].uart_rts:1 */
    P0_1_CPUSS_SWD_CLK              = 12,       /* Digital Deep Sleep - cpuss.swd_clk:1 */
    P0_1_SCB0_SPI_MISO              = 13,       /* Digital Deep Sleep - scb[0].spi_miso:0 */
    P0_1_SCB3_SPI_SELECT0           = 14,       /* Digital Deep Sleep - scb[3].spi_select0:1 */
    P0_1_SCB0_I2C_SCL               = 15,       /* Digital Deep Sleep - scb[0].i2c_scl:0 */

    /* P1.0 */
    P1_0_GPIO                       =  0,       /* SW controlled GPIO. */
    P1_0_TCPWM_LINE0                =  8,       /* Digital Active - tcpwm.line[0] */
    P1_0_TCPWM_TR_COMPARE_MATCH0    =  9,       /* Digital Active - tcpwm.tr_compare_match[0] */
    P1_0_SCB1_UART_TX               = 10,       /* Digital Active - scb[1].uart_tx */
    P1_0_TCPWM_TR_OVERFLOW0         = 11,       /* Digital Active - tcpwm.tr_overflow[0] */
    P1_0_USBPD0_TX_DATA_EN          = 12,       /* Digital Deep Sleep - usbpd[0].tx_data_en */
    P1_0_SCB1_SPI_CLK               = 14,       /* Digital Deep Sleep - scb[1].spi_clk */
    P1_0_SCB1_I2C_SCL               = 15,       /* Digital Deep Sleep - scb[1].i2c_scl */

    /* P1.1 */
    P1_1_GPIO                       =  0,       /* SW controlled GPIO. */
    P1_1_TCPWM_LINE1                =  8,       /* Digital Active - tcpwm.line[1] */
    P1_1_TCPWM_TR_COMPARE_MATCH1    =  9,       /* Digital Active - tcpwm.tr_compare_match[1] */
    P1_1_SCB1_UART_CTS              = 10,       /* Digital Active - scb[1].uart_cts */
    P1_1_TCPWM_TR_OVERFLOW1         = 11,       /* Digital Active - tcpwm.tr_overflow[1] */
    P1_1_SCB1_SPI_MOSI              = 14,       /* Digital Deep Sleep - scb[1].spi_mosi */

    /* P1.2 */
    P1_2_GPIO                       =  0,       /* SW controlled GPIO. */

    /* P1.3 */
    P1_3_GPIO                       =  0,       /* SW controlled GPIO. */
    P1_3_SRSS_EXT_CLK               =  8,       /* Digital Active - srss.ext_clk:0 */
    P1_3_SCB1_UART_RX               = 10,       /* Digital Active - scb[1].uart_rx */
    P1_3_USBPD0_CMP_OUT1            = 12,       /* Digital Deep Sleep - usbpd[0].cmp_out[1]:0 */
    P1_3_SCB0_SPI_SELECT0           = 13,       /* Digital Deep Sleep - scb[0].spi_select0:1 */
    P1_3_SCB1_SPI_MISO              = 14,       /* Digital Deep Sleep - scb[1].spi_miso */
    P1_3_SCB1_I2C_SDA               = 15,       /* Digital Deep Sleep - scb[1].i2c_sda */

    /* P1.4 */
    P1_4_GPIO                       =  0,       /* SW controlled GPIO. */

    /* P1.5 */
    P1_5_GPIO                       =  0,       /* SW controlled GPIO. */
    P1_5_SCB1_UART_RTS              = 10,       /* Digital Active - scb[1].uart_rts */
    P1_5_USBPD0_CMP_OUT0            = 12,       /* Digital Deep Sleep - usbpd[0].cmp_out[0]:1 */
    P1_5_SCB0_SPI_MISO              = 13,       /* Digital Deep Sleep - scb[0].spi_miso:1 */
    P1_5_SCB1_SPI_SELECT0           = 14,       /* Digital Deep Sleep - scb[1].spi_select0 */
    P1_5_USBPD1_HPD                 = 15,       /* Digital Deep Sleep - usbpd[1].hpd:1 */

    /* P1.7 */
    P1_7_GPIO                       =  0,       /* SW controlled GPIO. */
    P1_7_SCB0_UART_TX               = 10,       /* Digital Active - scb[0].uart_tx */
    P1_7_USBPD0_TX_DATA             = 11,       /* Digital Active - usbpd[0].tx_data */
    P1_7_USBPD0_SWAPT_OUT0          = 12,       /* Digital Deep Sleep - usbpd[0].swapt_out[0] */
    P1_7_SCB0_SPI_MOSI              = 13,       /* Digital Deep Sleep - scb[0].spi_mosi */
    P1_7_USBPD1_CMP_OUT1            = 14,       /* Digital Deep Sleep - usbpd[1].cmp_out[1]:1 */
    P1_7_SCB0_I2C_SDA               = 15,       /* Digital Deep Sleep - scb[0].i2c_sda:1 */

    /* P2.0 */
    P2_0_GPIO                       =  0,       /* SW controlled GPIO. */

    /* P2.1 */
    P2_1_GPIO                       =  0,       /* SW controlled GPIO. */
    P2_1_SCB0_UART_RX               = 10,       /* Digital Active - scb[0].uart_rx */
    P2_1_USBPD0_CMP_OUT0            = 12,       /* Digital Deep Sleep - usbpd[0].cmp_out[0]:0 */
    P2_1_SCB0_SPI_CLK               = 13,       /* Digital Deep Sleep - scb[0].spi_clk */
    P2_1_USBPD0_GPIO_DDFT1          = 14,       /* Digital Deep Sleep - usbpd[0].gpio_ddft1 */
    P2_1_SCB0_I2C_SCL               = 15,       /* Digital Deep Sleep - scb[0].i2c_scl:1 */

    /* P2.4 */
    P2_4_GPIO                       =  0,       /* SW controlled GPIO. */

    /* P2.5 */
    P2_5_GPIO                       =  0,       /* SW controlled GPIO. */
    P2_5_USBPD0_SWAPT_IN            = 15,       /* Digital Deep Sleep - usbpd[0].swapt_in */

    /* P2.6 */
    P2_6_GPIO                       =  0,       /* SW controlled GPIO. */
    P2_6_SCB2_UART_TX               = 10,       /* Digital Active - scb[2].uart_tx */
    P2_6_USBPD1_TX_DATA             = 12,       /* Digital Deep Sleep - usbpd[1].tx_data */
    P2_6_SCB2_SPI_MOSI              = 13,       /* Digital Deep Sleep - scb[2].spi_mosi */
    P2_6_SCB2_I2C_SDA               = 15,       /* Digital Deep Sleep - scb[2].i2c_sda */

    /* P3.2 */
    P3_2_GPIO                       =  0,       /* SW controlled GPIO. */
    P3_2_SCB3_UART_TX               = 10,       /* Digital Active - scb[3].uart_tx */
    P3_2_SCB3_SPI_MOSI              = 13,       /* Digital Deep Sleep - scb[3].spi_mosi */
    P3_2_USBPD1_CMP_OUT1            = 14,       /* Digital Deep Sleep - usbpd[1].cmp_out[1]:0 */
    P3_2_SCB3_I2C_SDA               = 15,       /* Digital Deep Sleep - scb[3].i2c_sda */

    /* P3.3 */
    P3_3_GPIO                       =  0,       /* SW controlled GPIO. */
    P3_3_SCB3_UART_RX               = 10,       /* Digital Active - scb[3].uart_rx */
    P3_3_USBPD1_CMP_OUT0            = 12,       /* Digital Deep Sleep - usbpd[1].cmp_out[0]:1 */
    P3_3_SCB3_SPI_MISO              = 13,       /* Digital Deep Sleep - scb[3].spi_miso */
    P3_3_SCB3_I2C_SCL               = 15,       /* Digital Deep Sleep - scb[3].i2c_scl */

    /* P3.6 */
    P3_6_GPIO                       =  0,       /* SW controlled GPIO. */
    P3_6_SCB3_UART_CTS              = 10,       /* Digital Active - scb[3].uart_cts */
    P3_6_SCB3_SPI_CLK               = 13,       /* Digital Deep Sleep - scb[3].spi_clk */
    P3_6_USBPD1_SWAPT_OUT1          = 15,       /* Digital Deep Sleep - usbpd[1].swapt_out[1] */

    /* P4.0 */
    P4_0_GPIO                       =  0,       /* SW controlled GPIO. */

    /* P4.3 */
    P4_3_GPIO                       =  0,       /* SW controlled GPIO. */
    P4_3_USBPD1_SWAPT_IN            = 15        /* Digital Deep Sleep - usbpd[1].swapt_in */
} en_hsiom_sel_t;

#endif /* _GPIO_PMG1B2_68_QFN_PMG1_B2_H_ */


/* [] END OF FILE */
