/***************************************************************************//**
* \file cy_usbpd_common.h
* \version 1.30
*
* Provides Common Header File of the USBPD driver.
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

#ifndef _CY_USBPD_COMMON_H_
#define _CY_USBPD_COMMON_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "cy_device.h"
#include "cy_device_headers.h"
#include "cy_syslib.h"
#include "cy_syspm.h"
#include "cy_usbpd_defines.h"
#include "cy_usbpd_regs.h"

/**
* \addtogroup group_usbpd_common
* \{
* USBPD driver common data structures.
*
* \defgroup group_usbpd_common_macros Macros
* \defgroup group_usbpd_common_enums Enumerated Types
* \defgroup group_usbpd_common_data_structures Data Structures
*/

/**
* \addtogroup group_usbpd_common_macros
* \{
*/

#if (defined(CY_IP_MXUSBPD) || defined(CY_IP_M0S8USBPD))

/**  Get the maximum from among two numbers. */
#define CY_USBPD_GET_MAX(a,b)    (((a) > (b)) ? (a) : (b))

/**  Get the minimum from among two numbers. */
#define CY_USBPD_GET_MIN(a,b)    (((a) > (b)) ? (b) : (a))

/** Retrieve the LSB from a DWORD. */
#define CY_USBPD_DWORD_GET_BYTE0(dw)     ((uint8_t)((dw) & 0xFFUL))

/** Retrieve the bits 15-8 from a DWORD. */
#define CY_USBPD_DWORD_GET_BYTE1(dw)     ((uint8_t)(((dw) >> 8) & 0xFFUL))

/** Retrieve the bits 23-16 from a DWORD. */
#define CY_USBPD_DWORD_GET_BYTE2(dw)     ((uint8_t)(((dw) >> 16) & 0xFFUL))

/** Retrieve the MSB from a DWORD. */
#define CY_USBPD_DWORD_GET_BYTE3(dw)     ((uint8_t)(((dw) >> 24) & 0xFFUL))

/** \} group_usbpd_common_macros */

/*******************************************************************************
*                            Enumerated Types
*******************************************************************************/

/**
* \addtogroup group_usbpd_common_enums
* \{
*/

/** Enum to hold SBU connection state. There is an internal switch to route SBU1/SBU2 signals
 * to AUX_P/AUX_N, LSTX/LSRX, or Isolate. Only applicable to PMG1S1, PMG1S2 and PMG1S3. */
typedef enum
{
    CY_USBPD_SBU_NOT_CONNECTED,                  /**< SBU pin is isolated. */
    CY_USBPD_SBU_CONNECT_AUX1,                   /**< Connect SBU pin to AUX_P. */
    CY_USBPD_SBU_CONNECT_AUX2,                   /**< Connect SBU pin to AUX_N. */
    CY_USBPD_SBU_CONNECT_LSTX,                   /**< Connect SBU pin to LSTX. */
    CY_USBPD_SBU_CONNECT_LSRX,                   /**< Connect SBU pin to LSRX. */
    CY_USBPD_SBU_MAX_STATE                       /**< Invalid value: not supported. */
} cy_en_usbpd_sbu_switch_state_t;

/** List of possible settings for the DP/DM MUX. Only applicable to PMG1S1. */
typedef enum
{
    CY_USBPD_DPDM_MUX_CONN_NONE         = 0x00,          /**< No connections enabled through the DPDM Mux. */
    CY_USBPD_DPDM_MUX_CONN_USB_TOP      = 0x11,          /**< Connect D+/D- to D+/D- on the top side of the connector. */
    CY_USBPD_DPDM_MUX_CONN_UART_TOP     = 0x22,          /**< Connect UART TX/RX to D+/D- on the top side of the connector. */
    CY_USBPD_DPDM_MUX_CONN_USB_BOT      = 0x44,          /**< Connect D+/D- to D+/D- on the bottom side of the connector. */
    CY_USBPD_DPDM_MUX_CONN_UART_BOT     = 0x88,          /**< Connect UART TX/RX to D+/D- on the bottom side of the connector. */
    CY_USBPD_DPDM_MUX_CONN_USB_TOP_UART = 0x99,          /**< Connect D+/D- to top and UART TX/RX to bottom side. */
    CY_USBPD_DPDM_MUX_CONN_USB_BOT_UART = 0x66           /**< Connect D+/D- to bottom and UART TX/RX to top side. */
} cy_en_usbpd_dpdm_mux_cfg_t;

/**
 * Enum to hold resistor configuration for AUX1 and AUX2. Values assigned
 * are the bit position of corresponding configuration in sbu_ctrl register.
 * Only applicable to PMG1S2 and PMG1S3.
 */
typedef enum
{
    CY_USBPD_AUX_NO_RESISTOR = 0,                        /**< No resistor. */
    CY_USBPD_AUX_1_1MEG_PU_RESISTOR = 9,                 /**< AUX1 1M0hm Pullup resistor. */
    CY_USBPD_AUX_1_100K_PD_RESISTOR = 10,                /**< AUX1 100KOhm Pulldown resistor. */
    CY_USBPD_AUX_1_470K_PD_RESISTOR = 11,                /**< AUX1 470KOhm Pulldown resistor. */
    CY_USBPD_AUX_2_100K_PU_RESISTOR = 12,                /**< AUX2 100KOhm Pullup resistor. */
    CY_USBPD_AUX_2_4P7MEG_PD_RESISTOR = 13,              /**< AUX2 4.7M0hm Pulldown resistor. */
    CY_USBPD_AUX_2_1MEG_PD_RESISTOR = 14,                /**< AUX2 1M0hm Pulldown resistor. */
    CY_USBPD_AUX_MAX_RESISTOR_CONFIG                     /**< Not supported. */
} cy_en_usbpd_aux_resistor_config_t;

/** List of HPD events detected by USBPD block. The UNPLUG, PLUG and IRQ events
 * are used in the case of a DisplayPort Sink implementation, and the
 * COMMAND_DONE event is used in the case of a DP Source implementation.
 */
typedef enum
{
    CY_USBPD_HPD_EVENT_NONE = 0,         /**< No event. */
    CY_USBPD_HPD_EVENT_UNPLUG,           /**< DP Unplug event. */
    CY_USBPD_HPD_EVENT_PLUG,             /**< DP Plug event. */
    CY_USBPD_HPD_EVENT_IRQ,              /**< DP IRQ event. */
    CY_USBPD_HPD_COMMAND_DONE,           /**< Requested HPD command is complete. */
    CY_USBPD_HPD_INPUT_CHANGE            /**< HPD IO raw status change. */
} cy_en_usbpd_hpd_events_t;

/** ADC block IDs */
typedef enum 
{
    CY_USBPD_ADC_ID_0,                        /**< ADC-0 in the PD block. Supported by all devices. */
    CY_USBPD_ADC_ID_1,                        /**< ADC-1 in the PD block. */
    CY_USBPD_ADC_ID_OVUV,                     /**< Dedicated ADC for Over-Voltage/Under-Voltage detection. Only
                                                   available on PMG1S2. */
    CY_USBPD_ADC_NUM_ADC                      /**< Maximum number of ADCs in the PD block. */
} cy_en_usbpd_adc_id_t;

/** Types of PD ADC input sources. Refer to the device datasheet and TRM for
 * more details.
 */
typedef enum 
{
    CY_USBPD_ADC_INPUT_AMUX_A,                /**< AMUX_A bus. */
    CY_USBPD_ADC_INPUT_AMUX_B,                /**< AMUX_B bus. */
    CY_USBPD_ADC_INPUT_BANDGAP,               /**< BANDGAP input. */
    CY_USBPD_ADC_INPUT_BJT,                   /**< BJT. */
    CY_USBPD_ADC_NUM_INPUT                    /**< Number of ADC inputs available. */
} cy_en_usbpd_adc_input_t;

/** PD comparator interrupt configuration enumeration.
 * Note: These are the settings for INTR_1_CFG ADC output.
 */
typedef enum
{
    CY_USBPD_ADC_INT_DISABLED,                /**< Comparator interrupt disabled. */
    CY_USBPD_ADC_INT_FALLING,                 /**< Comparator interrupt on falling edge. */
    CY_USBPD_ADC_INT_RISING,                  /**< Comparator interrupt on rising edge. */
    CY_USBPD_ADC_INT_BOTH                     /**< Comparator interrupt on either edge. */
} cy_en_usbpd_adc_int_t;

/** ADC block reference voltage selection. */
typedef enum PD_ADC_VREF_T
{
    CY_USBPD_ADC_VREF_PROG = 0,               /**< Programmable reference voltage from the RefGen block. */
    CY_USBPD_ADC_VREF_VDDD                    /**< VDDD supply used as ADC reference voltage. */
} cy_en_usbpd_adc_vref_t;

/** \} group_usbpd_common_enums */

/**
* \addtogroup group_usbpd_common_enums
* \{
*/

/** List of dedicated comparators supported by the PD block on various devices.
 * The actual comparators supported vary across device families. See comments
 * associated with each comparator ID for list of devices that support this
 * comparator.
 */
typedef enum
{
    CY_USBPD_VBUS_COMP_ID_UV                    = 0,    /**< UV comparator. */
    CY_USBPD_VBUS_COMP_ID_OV                    = 1,    /**< OV comparator. */

#if defined(CY_DEVICE_CCG3PA)
    CY_USBPD_VBUS_COMP_ID_VBUS_C_MON            = 2,    /**< VBUS_C_MON comparator. Only available on PMG1S0. */
    CY_USBPD_VBUS_COMP_ID_VBUS_DISCHARGE        = 3,    /**< Discharge comparator. Only available on PMG1S0. */
    CY_USBPD_VBUS_COMP_ID_LSCSA_SCP             = 4,    /**< SCP comparator. Only available on PMG1S0. */
    CY_USBPD_VBUS_COMP_ID_LSCSA_OCP             = 5,    /**< OCP comparator. Only available on PMG1S0. */
#elif defined (CY_DEVICE_PMG1S3)
    CY_USBPD_VBUS_COMP_ID_VSYS_DET              = 2,    /**< VSYS detection comparator*/
    CY_USBPD_VBUS_COMP_ID_P0_SBU1               = 3,    /**< Port-0 SBU1 Comparator. Only available on PMG1S3. */
    CY_USBPD_VBUS_COMP_ID_P0_SBU2               = 4,    /**< Port-0 SBU2 Comparator. Only available on PMG1S3. */
#elif (defined (CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S))
    CY_USBPD_VBUS_COMP_ID_VSYS_DET              = 2,    /**< VSYS detection comparator*/
    CY_USBPD_VBUS_COMP_ID_VBUS_DISCHARGE        = 3,    /**< Discharge comparator. */
    CY_USBPD_VBUS_COMP_ID_LSCSA_SCP             = 4,    /**< SCP comparator. */
    CY_USBPD_VBUS_COMP_ID_DP_DETACH             = 4,    /**< D+ voltage comparator. */
    CY_USBPD_VBUS_COMP_ID_DM_DETACH             = 5,    /**< D- voltage comparator. */
    CY_USBPD_VBUS_COMP_ID_LSCSA_PFC             = 2,    /**< CBL_1, PFC comparator. */
    CY_USBPD_VBUS_COMP_ID_LSCSA_SR              = 4,    /**< CBL_0, SR comparator. */
    CY_USBPD_VBUS_COMP_ID_VSRC_NEW_M            = 5,    /**< VSRC_NEW comparator. */
    CY_USBPD_VBUS_COMP_ID_VSRC_NEW_P            = 6,    /**< VSRC_NEW comparator. */
#else
    CY_USBPD_VBUS_COMP_ID_VBUS_MON              = 2,    /**< VBUS_MON comparator. Only available on PMG1S1. */
    CY_USBPD_VBUS_COMP_ID_VSYS_DET              = 3,    /**< VSYS detection. Only available on PMG1S1. */
    CY_USBPD_VBUS_COMP_ID_DP_DETACH             = 4,    /**< D+ voltage comparator. Only available on PMG1S1. */
    CY_USBPD_VBUS_COMP_ID_DM_DETACH             = 5,    /**< D- voltage comparator. Only available on PMG1S1. */
#endif /* CY_DEVICE_CCG3PA */

    COMP_ID_MAX                                 = 7     /**< End of comparator list. */

} cy_en_usbpd_comp_id_t;

/** The outputs of the comparators listed in comp_id_t and comp_tr_id_t can be passed through a
 * filter for debouncing. As with the comparators, the filters supported vary across device families.
 * This enumeration lists various filters supported and the comments indicate the devices that support
 * them.
 */
typedef enum
{
    CY_USBPD_VBUS_FILTER_ID_UV        = 0,              /**< UV comparator filter. */
    CY_USBPD_VBUS_FILTER_ID_OV        = 1,              /**< OV comparator filter. */

#if (defined(CY_DEVICE_CCG3PA))
    CY_USBPD_VBUS_FILTER_ID_DISCH_EN  = 2,              /**< Discharge enable filter for PMG1S0. */
    CY_USBPD_VBUS_FILTER_ID_LSCSA_SCP = 3,              /**< SCP filter for PMG1S0. */
    CY_USBPD_VBUS_FILTER_ID_LSCSA_OCP = 4,              /**< OCP filter for PMG1S0. */
#elif (defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S))
    CY_USBPD_VBUS_FILTER_ID_DISCH_EN  = 2,              /**< Discharge enable filter for PMG1S0. */
    CY_USBPD_VBUS_FILTER_ID_LSCSA_SR  = 3,              /**< CBL_0, SR filter for CCG7D. Can run based on HF or LF clock. */
    CY_USBPD_VBUS_FILTER_ID_LSCSA_PFC = 4,              /**< CBL_1, PFC filter for CCG7D. Can run based on HF or LF clock. */
    CY_USBPD_VBUS_FILTER_ID_VSRC_NEW_P = 5,             /**< VSRC_NEW_P filter for CCG7D. Can run based on LF clock. */
    CY_USBPD_VBUS_FILTER_ID_VSRC_NEW_M = 6,             /**< VSRC_NEW_M filter for CCG7D. Can run based on LF clock. */
    CY_USBPD_VBUS_FILTER_ID_CSA_SCP = 10,               /**< Virtual number for SCP as it is to be configured differently. */
    CY_USBPD_VBUS_FILTER_ID_CSA_OCP = 11,               /**< Virtual number for OCP as it is to be configured differently. */
    CY_USBPD_VBUS_FILTER_ID_PDS_SCP = 12,               /**< Virtual number for VBAT_GND SCP as it is to be configured differently. */
    CY_USBPD_VBUS_FILTER_ID_VIN_UVP = 13,               /**< Virtual number for VIN UVP as it is to be configured differently. */
    CY_USBPD_VBUS_FILTER_ID_VIN_OVP = 14,               /**< Virtual number for VIN OVP as it is to be configured differently. */
    CY_USBPD_VBUS_FILTER_ID_ILIM_DET = 15,              /**< Virtual number for ILIM DET as it is to be configured differently. */
    CY_USBPD_VBUS_FILTER_ID_VREG_INRUSH_DET = 16,       /**< Virtual number for VREG INRUSH DET as it is to be configured differently. */
    CY_USBPD_VBUS_FILTER_ID_BROWN_OUT_DET = 17,         /**< Virtual number for VDDD brown out as it is to be configured differently. */
#else
    CY_USBPD_VBUS_FILTER_ID_HSCSA_SCP = 2,              /**< SCP filter for PMG1S1. */
    CY_USBPD_VBUS_FILTER_ID_HSCSA_OCP = 3,              /**< OCP filter for PMG1S1. */
    CY_USBPD_VBUS_FILTER_ID_HSCSA_RCP = 4,              /**< RCP filter for PMG1S1. */
#endif /* defined(CY_DEVICE_CCG3PA) */

    CY_USBPD_VBUS_FILTER_ID_MAX                         /**< Number of supported filters. */
} cy_en_usbpd_vbus_filter_id_t;

/** List of edge triggered interrupt based on the filter output. */
typedef enum
{
    CY_USBPD_VBUS_FILTER_CFG_POS_DIS_NEG_DIS,           /**< Interrupt disabled. */
    CY_USBPD_VBUS_FILTER_CFG_POS_DIS_NEG_EN,            /**< Negative edge detection interrupt. */
    CY_USBPD_VBUS_FILTER_CFG_POS_EN_NEG_DIS,            /**< Positive edge detection interrupt. */
    CY_USBPD_VBUS_FILTER_CFG_POS_EN_NEG_EN,             /**< Both edge detection interrupt. */
    CY_USBPD_VBUS_FILTER_CFG_MAX                        /**< Invalid interrupt configuration. */
} cy_usbpd_vbus_filter_edge_detect_cfg_t;

/** PMG1 OVP modes enumeration. */
typedef enum
{
    CY_USBPD_VBUS_OVP_MODE_ADC,                  /**< OVP detection using PMG1 internal ADC. */
    CY_USBPD_VBUS_OVP_MODE_UVOV,                 /**< OVP detection using the UVOV block on PMG1. FET control
                                                      is done by firmware. */
    CY_USBPD_VBUS_OVP_MODE_UVOV_AUTOCTRL         /**< OVP detection and FET control using the OVOV block on PMG1. */
} cy_en_usbpd_vbus_ovp_mode_t;

/** PMG1 UVP modes enumeration. */
typedef enum
{
    VBUS_UVP_MODE_ADC,                          /**< UVP detection using PMG1 internal ADC. */
    VBUS_UVP_MODE_INT_COMP,                     /**< UVP detection using the UVOV block on PMG1. FET control is
                                                     done by firmware. */
    VBUS_UVP_MODE_INT_COMP_AUTOCTRL             /**< UVP detection and FET control using the UVOV block on PMG1. */
} cy_en_usbpd_vbus_uvp_mode_t;

/**
 * @typedef cy_en_usbpd_supply_t
 * @brief List of power supplies input to and monitored by the PMG1 device.
 * This does not include the VBus supply which is monitored by all PMG1 devices as required by
 * the Type-C and USB-PD specifications.
 */
typedef enum
{
    CY_USBPD_SUPPLY_VSYS = 0x00,                /**< Vsys supply input which powers the device. */
    CY_USBPD_SUPPLY_V5V  = 0x01                 /**< V5V input used to derive the VConn supply from.
                                                     Not supported on PMG1S0. */
} cy_en_usbpd_supply_t;

/** PD PHY state events */
typedef enum
{
    CY_USBPD_PHY_EVT_TX_MSG_COLLISION,        /**< Bus busy at message transmission. */
    CY_USBPD_PHY_EVT_TX_MSG_PHY_IDLE,         /**< Bus idle, ready for message transmission. */
    CY_USBPD_PHY_EVT_TX_MSG_FAILED,           /**< Message transmission was not successful. */
    CY_USBPD_PHY_EVT_TX_MSG_SUCCESS,          /**< Message transmission was successful. */
    CY_USBPD_PHY_EVT_TX_RST_COLLISION,        /**< Bus busy just before reset transmission. */
    CY_USBPD_PHY_EVT_TX_RST_SUCCESS,          /**< Reset transmission was successful. */
    CY_USBPD_PHY_EVT_RX_MSG,                  /**< Message received. */
    CY_USBPD_PHY_EVT_RX_MSG_CMPLT,            /**< Message received and GoodCRC sent aka collision type 3. */
    CY_USBPD_PHY_EVT_RX_RST,                  /**< Reset was received. */
    CY_USBPD_PHY_EVT_FRS_SIG_RCVD,            /**< FRS signal was received. */
    CY_USBPD_PHY_EVT_FRS_SIG_SENT,            /**< FRS signal was transmitted. */
    CY_USBPD_PHY_EVT_CRC_ERROR                /**< Message received with CRC error. */
} cy_en_usbpd_phy_events_t;

/** USBPD Driver Events */
typedef enum {
    CY_USBPD_EVT_FRS_SIGNAL_RCVD = 0,         /**< FRS Signal Was received. */
    CY_USBPD_EVT_FRS_SIGNAL_SENT              /**< FRS Signal was sent. */
} cy_en_usbpd_events_t;

/** USBPD status codes */
typedef enum cy_en_usbpd_status
{
    CY_USBPD_STAT_NO_RESPONSE = -2,          /**< Special status code indicating
                                             no response. */
    CY_USBPD_STAT_SUCCESS = 0,               /**< Success status. */
    CY_USBPD_STAT_FLASH_DATA_AVAILABLE,      /**< Special status code indicating flash data
                                             availability. */
    CY_USBPD_STAT_BAD_PARAM,                 /**< Bad input parameter. */
    CY_USBPD_STAT_INVALID_COMMAND = 3,       /**< Operation failed due to invalid command. */
    CY_USBPD_STAT_FLASH_UPDATE_FAILED = 5,   /**< Flash write operation failed. */
    CY_USBPD_STAT_INVALID_FW,                /**< Special status code indicating invalid firmware */
    CY_USBPD_STAT_INVALID_ARGUMENT,          /**< Operation failed due to invalid arguments. */
    CY_USBPD_STAT_NOT_SUPPORTED,             /**< Feature not supported. */
    CY_USBPD_STAT_INVALID_SIGNATURE,         /**< Invalid signature parameter identified. */
    CY_USBPD_STAT_TRANS_FAILURE,             /**< Transaction failure status. */
    CY_USBPD_STAT_CMD_FAILURE,               /**< Command failure status */
    CY_USBPD_STAT_FAILURE,                   /**< Generic failure status. */
    CY_USBPD_STAT_READ_DATA,                 /**< Special status code indicating read data
                                             availability. */
    CY_USBPD_STAT_NOT_READY,                 /**< Operation failed due to device/stack not ready. */
    CY_USBPD_STAT_BUSY,                      /**< Operation failed due to device/stack busy status. */
    CY_USBPD_STAT_TIMEOUT,                   /**< Operation timed out. */
    CY_USBPD_STAT_INVALID_PORT               /**< Invalid port number. */
} cy_en_usbpd_status_t;

/** \} group_usbpd_common_enums */

/** \} group_usbpd_common */

/**
* \addtogroup group_usbpd_legacy_enums
* \{
*/

/** List of charger detect block events sent by the HAL to the
 * state machine.
 */
typedef enum
{
    CY_USBPD_BC_PHY_EVT_COMP1_TRIG = 0,          /**< Comp1 triggered */
    CY_USBPD_BC_PHY_EVT_COMP2_TRIG               /**< Comp2 triggered */
} cy_en_usbpd_bch_phy_evt_t;

/** Charger block source (DP/DM) termination options. */
typedef enum
{
    CHGB_SRC_TERM_APPLE_1A = 0,         /**< Apple 1A termination */
    CHGB_SRC_TERM_APPLE_2_1A,           /**< Apple 2.1A termination */
    CHGB_SRC_TERM_APPLE_2_4A,           /**< Apple 2.4A termination */
    CHGB_SRC_TERM_QC,                   /**< Quick Charge termination */
    CHGB_SRC_TERM_AFC,                  /**< AFC termination */
    CHGB_SRC_TERM_DCP,                  /**< DCP termination */
    CHGB_SRC_TERM_CDP,                  /**< CDP termination */
    CHGB_SRC_TERM_SDP                   /**< SDP termination */
} cy_en_usbpd_bch_src_term_t;

/** Charger block sink termination options. */
typedef enum
{
    CHGB_SINK_TERM_SPD = 0,             /**< Standard port detect */
    CHGB_SINK_TERM_PCD,                 /**< Primary charger detect. */
    CHGB_SINK_TERM_SCD,                 /**< Secondary charger detect. */
    CHGB_SINK_TERM_AFC,                 /**< AFC detect */
    CHGB_SINK_TERM_APPLE,               /**< Apple detect */
    CHGB_SINK_TERM_QC_5V,               /**< QC 5V detect */
    CHGB_SINK_TERM_QC_9V,               /**< QC 9V detect */
    CHGB_SINK_TERM_QC_12V,              /**< QC 12V detect */
    CHGB_SINK_TERM_QC_20V              /**< QC 20V detect */
} cy_en_usbpd_bch_snk_term_t;


/**  Signals that can be connected to the positive input of the charger detect 
 * block comparators.
 */
typedef enum
{
    CHGB_COMP_P_DM = 0,                 /**< DM input */
    CHGB_COMP_P_VREF,                   /**< VREF input. Reference voltage level selected separately. */
    CHGB_COMP_P_DP,                     /**< DP input */
    CHGB_COMP_P_GND                     /**< GND input */
}cy_en_usbpd_bch_comp_pinput_t;

/** Signals that can be connected to the negative input of the charger detect
 * block comparators.
 */
typedef enum
{
    CHGB_COMP_N_DM = 0,                 /**< DM input */
    CHGB_COMP_N_VREF,                   /**< VREF input. Reference voltage level selected separately. */
    CHGB_COMP_N_DP,                     /**< DP input */
    CHGB_COMP_N_GND                     /**< GND input */
} cy_en_usbpd_bch_comp_ninput_t;

/** List of reference voltage levels available from the charger detect block.
 */
typedef enum
{
#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3) || defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S))
    CHGB_VREF_0_325V = 0,               /**< 0.325 V */
    CHGB_VREF_0_425V = 0,               /**< 0.425 V. Use of 0.325 with offset of 100 mV */
    CHGB_VREF_0_7V,                     /**< 0.7 V */
    CHGB_VREF_0_85V,                    /**< 0.85 V */
    CHGB_VREF_1_4V,                     /**< 1.4 V */
    CHGB_VREF_1_7V,                     /**< 1.7 V */
    CHGB_VREF_2V,                       /**< 2.0 V */
    CHGB_VREF_2_2V,                     /**< 2.2 V */
    CHGB_VREF_2_9V                      /**< 2.9 V */
#elif (defined(CY_DEVICE_CCG3))
    CHGB_VREF_0_325V = 0,               /**< 0.325 V */
    CHGB_VREF_0_6V,                     /**< 0.6 V */
    CHGB_VREF_0_85V,                    /**< 0.85 V */
    CHGB_VREF_1_4V,                     /**< 1.4 V */
    CHGB_VREF_GND,                      /**< GND */
    CHGB_VREF_2V,                       /**< 2.0 V */
    CHGB_VREF_2_2V,                     /**< 2.2 V */
    CHGB_VREF_2_9V                      /**< 2.9 V */
#else
    CHGB_VREF_INVALID = 0               /**< Not supported. */
#endif /* CY_DEVICE */
} cy_en_usbpd_bch_vref_t;

/** Interrupt generation options for the charger detect block comparators.
 */
typedef enum
{
    CHGB_COMP_NO_INTR = 0,              /**< Interrupt disabled */
    CHGB_COMP_EDGE_FALLING,             /**< Interrupt on falling edge. */
    CHGB_COMP_EDGE_RISING,              /**< Interrupt on rising edge. */
    CHGB_COMP_EDGE_BOTH                 /**< Interrupt on either  edge. */
} cy_en_usbpd_bch_comp_edge_t;

/** \} group_usbpd_legacy_enums */

/**
* \addtogroup group_usbpd_common
* \{
* */

/*******************************************************************************
*                              Type Definitions
*******************************************************************************/

/**
* \addtogroup group_usbpd_common_data_structures
* \{
*/

/**
 * Callback function used to provide notification about input supply changes.
 *
 * @param context Pointer to the context structure.
 * @param supply_id ID of the supply on which change is detected.
 * @param present Whether the identified supply is now present (true) or absent (false).
 */
typedef void (*cy_usbpd_supply_change_cbk_t)(void *context, cy_en_usbpd_supply_t supply_id, bool present);

/**
 * Notifies the PD stack about the occurrences of
 * \ref cy_en_usbpd_phy_events_t events.
 */
typedef void(*cy_cb_usbpd_phy_handle_events_t)(void *context, cy_en_usbpd_phy_events_t event);

/** Provides notification on VBUS load change */
typedef void (*cy_cb_usbpd_vbus_load_chg_t)(void *context);

/** VBus OVP/OCP/SCP/RCP protection callback function. */
typedef bool (*cy_cb_vbus_fault_t)(void *context, bool compOut);

/** ADC Events callback function. */
typedef void (*cy_cb_adc_events_t)(void *context, bool compOut);

/** SBU level detected callback function. */
typedef void (*cy_cb_sbu_level_detect_t)(void *context, bool sbu1Detect, bool sbu2Detect);

/** HPD event callback function. 
 * This function is used by the HAL layer to notify the DisplayPort
 * alternate mode layer about HPD related events that are detected.
 */
typedef void (*cy_cb_usbpd_hpd_events_t)(void *context, 
                                         cy_en_usbpd_hpd_events_t event);

/** Charger Detect PHY callback prototype. This function will be used to notify
 * the stack about PHY events.
 *
 * \param context
 * USBPD PDL Context pointer.
 *
 * \param event 
 * Type of BC PHY event.
 */
typedef void(*cy_cb_bc_phy_events_t)(void *context, uint32_t event);


/** USBPD Driver events callback.
 *
 * \param context
 * USBPD PDL Context pointer.
 *
 * \param event 
 * \ref cy_en_usbpd_events_t
 *
 * \param evtParam
 * Data associated with the event
 *
 * \return uint8_t
 * Returns the status of the event handling in the callback.
 *
 */
typedef uint8_t (*cy_usbpd_evt_cbk_t) (
        void *context,
        cy_en_usbpd_events_t event,
        void *evtParam);

/** Type C Driver events callback.
 *
 * \param context
 * Caller Context pointer.
 *
 * \param event 
 * \ref cy_en_pd_typec_events_t
 *
 * \return
 * None
 *
 */
typedef void (*cy_usbpd_typec_evt_cbk_t) (
        void *context,
        cy_en_pd_typec_events_t event);


/** Config structure for VBUS OVP parameters */
typedef struct
{
    /** Whether to enable/disable VBUS OVP Feature
     *  1 - Enable
     *  0 - Disable */
    uint8_t enable;

    /** Vbus OVP Mode Selection 
     *  0 - OVP using ADC comparator 
     *  1 - OVP using dedicated comparator, Firmware detects trips and turns
     *      off FETS 
     *  2 - OVP using dedicated comparator, Hardware detects trips and turns
     *      off FETS */
    uint8_t mode;

    /** Current threshold percentage (0-100) above the contract current to
     * trigger fault. */
    uint8_t threshold;

    /** Fault event debounce period in ms (0-255) */
    uint8_t debounce;

    /** Number of times to retry recovery from fault(0-255) */
    uint8_t retryCount;

} cy_stc_fault_vbus_ovp_cfg_t;

/** Config structure for VBUS UVP parameters */
typedef struct
{
    /** Whether to enable/disable VBUS UVP Feature
     *  1 - Enable
     *  0 - Disable */
    uint8_t enable;
    
    /** Vbus UVP Mode Selection 
     *  0 - UVP using ADC comparator 
     *  1 - UVP using dedicated comparator, Firmware detects trips and turns
     *      off FETS 
     *  2 - UVP using dedicated comparator, Hardware detects trips and turns
     *      off FETS */
    uint8_t mode;

    /** Current threshold percentage (0-100) above the contract current to
     * trigger fault. */
    uint8_t threshold;

    /** Fault event debounce period in ms (0-255) */
    uint8_t debounce;

    /** Number of times to retry recovery from fault(0-255) */
    uint8_t retryCount;

} cy_stc_fault_vbus_uvp_cfg_t;

/** Structure for legacy charging state machine */
typedef struct
{
    /** Whether the state machine is active. */
    bool     is_active;            
    
    /** Whether the VDM_SRC supply has been turned ON. */
    bool     vdmsrc_on;           
    
    /** Number of times the VDM_SRC output has been detected as low. */
    uint8_t  vdmsrc_lv_cnt;       

} cy_stc_usbpd_legacy_charging_state_t;

/** Config structure for VBUS OCP parameters */
typedef struct
{
    /** Whether to enable/disable VBUS OCP Feature
     *  1 - Enable
     *  0 - Disable */
    uint8_t enable;

    /** Vbus OCP Mode Selection 
     *  0 - OCP using external hardware
     *  1 - Internal OCP with neither software debounce nor automatic FET
     *      control
     *  2 - Internal OCP with automatic FET control by hardware when an OCP
     *      event is detected
     *  3 - Internal OCP with software debounce using delay in milliseconds
     *      specified by the user */
    uint8_t mode;

    /** Current threshold percentage (0-100) above the contract current to
     * trigger fault. */
    uint8_t threshold;

    /** Fault event debounce period in ms (0-255) */
    uint8_t debounce;

    /** Number of times to retry recovery from fault(0-255) */
    uint8_t retryCount;

    /** Sense Resistor impedance in milli-Ohm units */
    uint8_t senseRes;

    /** Current sense tuning resistor impedance in 100 Ohm units */
    uint8_t csRes;

} cy_stc_fault_vbus_ocp_cfg_t;

/** Config structure for VCONN OCP parameters */
typedef struct
{
    /** Whether to enable/disable VConn OCP Feature
     *  1 - Enable
     *  0 - Disable */
    uint8_t enable;

    /** Current threshold percentage (0-100) above the contract current to
     * trigger fault. */
    uint8_t threshold;

    /** Fault event debounce period in ms (0-255) */
    uint8_t debounce;

    /** Number of times to retry recovery from fault(0-255) */
    uint8_t retryCount;

} cy_stc_fault_vconn_ocp_cfg_t;

/** Config structure for VBUS SCP parameters */
typedef struct
{
    /** Whether to enable/disable VBUS SCP Feature
     *  1 - Enable
     *  0 - Disable */
    uint8_t enable;

    /** Sense Resistor impedance in milli-Ohm units */
    uint8_t senseRes;

    /** Current threshold percentage (0-100) above the contract current to
     * trigger fault. */
    uint8_t threshold;

    /** Fault event debounce period in us (0-255) */
    uint8_t debounce;

    /** Number of times to retry recovery from fault(0-255) */
    uint8_t retryCount;

} cy_stc_fault_vbus_scp_cfg_t;

/** Config structure for VBUS RCP parameters */
typedef struct
{
    /** Whether to enable/disable VBUS RCP Feature
     *  1 - Enable
     *  0 - Disable */
    uint8_t enable;

    /** Number of times to retry recovery from fault(0-255) */
    uint8_t retryCount;

} cy_stc_fault_vbus_rcp_cfg_t;

/** Config structure for CC OVP parameters */
typedef struct
{
    /** Whether to enable/disable CC OVP Feature
     *  1 - Enable
     *  0 - Disable */
    uint8_t enable;

    /** Number of times to retry recovery from fault(0-255) */
    uint8_t retryCount;

} cy_stc_fault_cc_ovp_cfg_t;

/** Config structure for SBU OVP parameters */
typedef struct
{
    /** Whether to enable/disable SBU OVP Feature
     *  1 - Enable
     *  0 - Disable */
    uint8_t enable;

    /** Number of times to retry recovery from fault(0-255) */
    uint8_t retryCount;

} cy_stc_fault_sbu_ovp_cfg_t;

/** Config structure for legacy charging parameters */
typedef struct
{
    uint8_t enable;           /**< Whether to enable/disable legacy charging Feature
                                 *  1 - Enable
                                 *  0 - Disable */
    uint8_t srcSel;           /**< Legacy charging source selection bit mask:
                                 *   Bit 0 --> BC 1.2 support
                                 *   Bit 1 --> Apple charger
                                 *   Bit 2 --> Quick charge
                                 *   Bit 3 --> AFC charger
                                 *   Other bits reserved
                                 */
    uint8_t snkSel;           /**< Legacy charging sink selection bit mask:
                                 *   Bit 0 --> BC 1.2
                                 *   Bit 1 --> Apple brick
                                 *   Other bits reserved
                                 */
    uint8_t appleSrcId;       /**< Apple charger brick ID to be used as source:
                                 *   Bit 0 --> 1 A
                                 *   Bit 1 --> 2.1 A
                                 *   Bit 2 --> 2.4 A
                                 */
    uint8_t qcSrcType;        /**< Quick charge source type:
                                 *   Bit 0 --> QC 2.0 Class-A
                                 *   Bit 1 --> QC 2.0 Class-B
                                 *   Bit 2 --> QC 3.0 Class-A
                                 *   Bit 3 --> QC 3.0 Class-B
                                 */
    uint8_t afcSrcCapCnt;     /**< Number of AFC source voltage-current capabilities supported. */
    uint8_t afcSrcCaps[16];   /**< AFC source voltage-current capabilities list. */
} cy_stc_legacy_charging_cfg_t;

/** Config structure for USBPD IP configuration */
typedef struct
{
    /** Pointer for VBUS OVP config structure */
    const cy_stc_fault_vbus_ovp_cfg_t *vbusOvpConfig;
    
    /** Pointer for VBUS UVP config structure */
    const cy_stc_fault_vbus_uvp_cfg_t *vbusUvpConfig;
    
    /** Pointer for VBUS OCP config structure */
    const cy_stc_fault_vbus_ocp_cfg_t *vbusOcpConfig;
    
    /** Pointer for VCONN OCP config structure */
    const cy_stc_fault_vconn_ocp_cfg_t *vconnOcpConfig;
    
    /** Pointer for VBUS SCP config structure */
    const cy_stc_fault_vbus_scp_cfg_t *vbusScpConfig;
    
    /** Pointer for VBUS RCP config structure */
    const cy_stc_fault_vbus_rcp_cfg_t *vbusRcpConfig;
    
    /** Pointer for CC OVP config structure */
    const cy_stc_fault_cc_ovp_cfg_t *ccOvpConfig;
    
    /** Pointer for SBU OVP config structure */
    const cy_stc_fault_sbu_ovp_cfg_t *sbuOvpConfig;
    
    /** Legacy Charging Configuration. */
    const cy_stc_legacy_charging_cfg_t *legacyChargingConfig;
} cy_stc_usbpd_config_t;

/** Context structure for USBPD-IP */
typedef struct
{
    /** USBPD port Index. **/
    uint8_t port;
    
    /** USBPD register base address **/
    PPDSS_REGS_T base;
    
    /** USBPD TRIMS register base address **/
    void *trimsBase;
    
    /** PD phy callback. */
    cy_cb_usbpd_phy_handle_events_t pdPhyCbk;

    /** Supply Change callback. */
    cy_usbpd_supply_change_cbk_t supplyChangeCbk;

    /** The received PD packet. */
    cy_stc_pd_packet_extd_t rxPkt;

    /** The tx data pointer. */
    uint32_t* txDatPtr;

    /** ADC block Vref. */
    volatile bool adcRefVddd[CY_USBPD_ADC_NUM_ADC];
    
    /** ADC block Vref. */
    volatile uint16_t adcVdddMv[CY_USBPD_ADC_NUM_ADC];
    
    /** VBUS Monitor divider. */
    uint8_t vbusMonDiv;
    
    /** ADC input index. */
    cy_en_usbpd_adc_input_t vbusDetachAdcInp;
    
    /** ADC ID index. */
    cy_en_usbpd_adc_id_t vbusDetachAdcId;

    /** ADC callback. */
    cy_cb_adc_events_t adcCb[CY_USBPD_ADC_NUM_ADC];

    /** SBU level detect callback */
    cy_cb_sbu_level_detect_t sbuDetectCb;

    /** The tx data count. */
    uint8_t txDobjCount;

    /** The tx data pointer. */
    uint8_t volatile txObjSent;

    /** Holds current transmission is unchunked or not */
    uint8_t volatile txUnchunked;

    /** Holds retry count. */
    int8_t volatile retryCnt;

    /**
     * Flag to indicate a message has been transmitted and we're waiting for
     * GoodCRC.
     */
    uint8_t volatile txDone;

    /**
     * Flag to indicate currently received message is unchunked extended message
     */
    bool volatile rxUnchunked;

    /**
     * Length of currently being received extended unchunked messages in 32 bits units
     */
    uint8_t volatile rxUnchunkLen;

    /**
     * Count in 32 bits units of no of words read from rx memory for extended unchunked
     * message.
     */
    uint8_t volatile rxUnchunkCount;

    /**
     * Read memory location where the HAL should read the next portion of data from.
     */
    uint8_t volatile rxReadLocation;

    /** HPD transmit enable. */
    bool hpdTransmitEnable;

    /** HPD receive enable. */
    bool hpdReceiveEnable; 

    /** HPD event callback. */
    cy_cb_usbpd_hpd_events_t hpdCbk;


   /**
    * This flag keeps track of HPD Connection status. It is used in HPD CHANGE wakeup
    * interrupt.
    */
    bool hpdState;

    /** BC phy callback. */
    cy_cb_bc_phy_events_t bcPhyCbk;

    /** CDP State Variable */ 
    cy_stc_usbpd_legacy_charging_state_t cdp_state;

    /** QC3.0 net pulse count (+ve = D+ pulse, -ve = D- pulse).*/
    int volatile bcQcPulseCount;

    /** AFC TX buffer */
    uint8_t bcAfcTxBuf[16];

    /** AFC RX buffer */
    uint8_t bcAfcRxBuf[16];

    /** AFC RX buffer index */
    uint8_t bcAfcRxIdx;

    /** AFC TX buffer index */
    uint8_t bcAfcTxIdx;

    /** AFC TX buffer size */
    uint8_t bcAfcTxSize;

    /** PFET ON-OFF state */
    bool vbusPfetOn;
    
    /** CFET ON-OFF State */
    bool vbusCfetOn;

    /** Auto toggle enable from stack. */
    uint8_t volatile autoToggleEn;

    /** Auto toggle active state. */
    uint8_t volatile autoToggleAct;

    /** Type-C state machine re-enable pending. */
    uint8_t volatile typecStartPending;

    /** OVP fault pending on CC line. */
    uint8_t volatile ccOvpPending;
    
    /** DP/DM MUX config per port */ 
    cy_en_usbpd_dpdm_mux_cfg_t curDpdmCfg; 
    
    /** SBU Switch 1 State per port */ 
    cy_en_usbpd_sbu_switch_state_t sbu1State;

    /** SBU Switch 2 State per port */ 
    cy_en_usbpd_sbu_switch_state_t sbu2State;

    /** Aux 1 Resistor State */
    cy_en_usbpd_aux_resistor_config_t aux1Config; 

    /** Aux 2 Resistor State */
    cy_en_usbpd_aux_resistor_config_t aux2Config; 
    
    /** Callback function for VCONN OCP fault */
    cy_cb_vbus_fault_t vconnOcpCbk;
    
    /** Callback function for VBUS OVP fault */
    cy_cb_vbus_fault_t vbusOvpCbk;
    
    /** Callback function for VBUS UVP fault */
    cy_cb_vbus_fault_t vbusUvpCbk;
    
    /** Callback function for VBUS OCP fault */
    cy_cb_vbus_fault_t vbusOcpCbk;
    
    /** Callback function for CC/SBU OVP fault */
    cy_cb_vbus_fault_t ccSbuOvpCbk;

    /** Callback function for VBUS RCP fault */
    cy_cb_vbus_fault_t vbusRcpCbk;

    /** Callback function for VBUS SCP fault */
    cy_cb_vbus_fault_t vbusScpCbk;

    /** Callback function for USBPD driver events */
    cy_usbpd_evt_cbk_t usbpdEventsCbk;

    /** Callback function for Type C driver events */
    cy_usbpd_typec_evt_cbk_t typecEventsCbk;

    /** Pointer to the PDSTACK Context. */
    void *pdStackContext;

    /** Pointer to the usbpd config structure. */
    cy_stc_usbpd_config_t *usbpdConfig;

    /** Pointer to the DPM status structure. */
    cy_cb_pd_dpm_get_config_t dpmGetConfig;

} cy_stc_usbpd_context_t;


/** \} group_usbpd_common_data_structures */


/*******************************************************************************
*                            Function Prototypes
*******************************************************************************/
/** \cond INTERNAL*/
__STATIC_INLINE uint32_t Cy_USBPD_MmioRegUpdateField(uint32_t origval, uint32_t fld, uint32_t mask, uint8_t pos);

__STATIC_INLINE void Cy_USBPD_MemCopyWord(uint32_t* dest, const uint32_t* source, uint32_t size);

/*******************************************************************************
*                     In-line Function Implementation
*******************************************************************************/

/*******************************************************************************
* Function Name: Cy_USBPD_MmioRegUpdateField
****************************************************************************//**
*
* Updates a register field with a new value.
*
* \param origval
* The current value of the register.
*
* \param fld
* The register field value to be updated.
*
* \param mask
* Register field mask.
*
* \param pos
* The register field position to be updated.
*
* \return uint32_t
* Returns the new value of the register
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_USBPD_MmioRegUpdateField(uint32_t origval, uint32_t fld, uint32_t mask, uint8_t pos)
{
    return ((origval & ~mask) | (fld << pos));
}

/*******************************************************************************
* Function Name: Cy_USBPD_MemCopyWord
****************************************************************************//**
*
* Copies a 32-bit word from source address to destination address
*
* \param dest
* The destination address pointer.
*
* \param source
* The source address pointer.
*
* \param size
* Number of 32-bit words to be copied.
*
*******************************************************************************/
__STATIC_INLINE void Cy_USBPD_MemCopyWord(uint32_t* dest, const uint32_t* source, uint32_t size)
{
    uint32_t i;
    for(i = 0 ; i < size; i++)
    {
        dest[i] = source[i];
    }
}

/** \endcond */

#endif /* (defined(CY_IP_MXUSBPD) || defined(CY_IP_M0S8USBPD)) */

#endif /* _CY_USBPD_COMMON_H_ */

/** \} group_usbpd_common */

/* [] End of File */
