/***************************************************************************//**
* \file cy_isouart.c
* \version 1.0
*
* \brief
* Provides an API implementation of the Isolated UART driver
*
********************************************************************************
* \copyright
* (c) 2026, Infineon Technologies AG or an affiliate of
* Infineon Technologies AG.
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
#include "cy_isouart.h"
#include "cy_sysclk.h"

#if defined(CY_IP_M0S8ISOUART)
#if defined(__cplusplus)
extern "C" {
#endif

/*****************************************************************************/
/* Global private defines                                                    */
/*****************************************************************************/
/** \cond Internal */
/* Interface mode */
#define CY_ISOUART_INTERFACE_MODE_NODE            (0U)
#define CY_ISOUART_INTERFACE_MODE_RECEIVER        (1U)
#define CY_ISOUART_INTERFACE_MODE_HOST            (2U)
/* Max base address used when returning registers enabled by IF_MULTI_READ_CFG register */
#define CY_ISOUART_MULTI_READ_MAX_BASE_ADDR       (127U)
/* Total number of iso UART SRAM rows (128 words) */
#define CY_ISOUART_SIZE_OF_SRAM                   (128U)
/* Maximum value of ECC parity bits */
#define CY_ISOUART_MAX_ECC_PARITY                 (128U)
/* Maximum valid index for iso UART SRAM */
#define CY_ISOUART_MAX_SRAM_INDEX                 (CY_ISOUART_SIZE_OF_SRAM - 1U)
/* Maximum valid index for iso UART SRAM 32-bit operations (aligned to 32-bit) */
#define CY_ISOUART_MAX_SRAM_INDEX_32BIT           (CY_ISOUART_MAX_SRAM_INDEX - 1U)
/* Number of SRAM rows used in RAW mode */
#define CY_ISOUART_NUM_OF_ROW_IN_RAW_MODE         (3U)
/* Number of SRAM rows used in RAW mode */
#define CY_ISOUART_NUM_OF_ROW_IN_UNPACKED_MODE    (1U)
/* Bit shift to access second byte in data frame */
#define CY_ISOUART_BITSHIFT_RAW_DATA              (8U)
/* Minimum value of delay between wake sequence and command frame */
#define CY_ISOUART_MIN_WAKE_CMD_DELAY             (0x2EE0UL)
/* Maximum value of delay between wake sequence and command frame */
#define CY_ISOUART_MAX_WAKE_CMD_DELAY             (0x3FFCUL)
/* Maximum valid block ID */
#define CY_ISOUART_MAX_BLOCK_ID                    (6U)
/* Maximum valid node ID */
#define CY_ISOUART_MAX_NODE_ID                     (30U)
/* Broadcast node ID */
#define CY_ISOUART_BROADCAST_NODE_ID               (63U)
/* Maximum valid address */
#define CY_ISOUART_MAX_ADDRESS                     (127U)
/* Maximum value of write reply data (5 bits) */
#define CY_ISOUART_MAX_WRITE_REPLY_DATA_VAL        (31U)
/* Iso UART core logic clock frequency - clk_iuart (24MHz) */
#define CY_ISOUART_CLK_IUART_FREQ                  (24000000U)

/* Lookup table mapping the upper 5 bits (bits[7:3]) of the write response byte
 * to the expected 3-bit CRC value (bits[2:0]). Index = (writeResponse >> 3). */
static const uint8_t Cy_ISOUART_Crc3BitLut[CY_ISOUART_MAX_WRITE_REPLY_DATA_VAL + 1U] = {
    0x00, 0x03, 0x06, 0x05, 0x07, 0x04, 0x01, 0x02, 0x05, 0x06, 0x03, 0x00, 0x02, 0x01, 0x04, 0x07,
    0x01, 0x02, 0x07, 0x04, 0x06, 0x05, 0x00, 0x03, 0x04, 0x07, 0x02, 0x01, 0x03, 0x00, 0x05, 0x06};

/* Flag that indicates pending wake sequence */
static bool Cy_ISOUART_WakeSequenceFlag;
/** \endcond */

/*****************************************************************************/
/* Internal function definitions                                             */
/*****************************************************************************/
static void Cy_ISOUART_InitSram(ISOUART_Type* base);
void Cy_ISOUART_SetBroadcastTurnaroundTime(ISOUART_Type* base, bool fastTurnaround);


/*****************************************************************************/
/* API                                                                       */
/*****************************************************************************/
/*******************************************************************************
* Function Name: Cy_ISOUART_InitSram
****************************************************************************//**
*
* \brief
* Initialize all iso UART SRAM (including both data field and attribute field) with zero.
* Since the SRAM value is random after reset, this I/F initialize ECC parity bits
* not to cause ECC failure.
* \param base
* The base address for the iso UART.
*
*******************************************************************************/
static void Cy_ISOUART_InitSram(ISOUART_Type* base)
{
    uint32_t index;
    for (index = 0U; index < CY_ISOUART_SIZE_OF_SRAM; index++)
    {
        base->SRAM_WITH_ATTR[index] = 0U;
    }
}

/*******************************************************************************
* Function Name: Cy_ISOUART_SetBroadcastTurnaroundTime
****************************************************************************//**
*
* \brief
* Set the broadcast write turnaround time.
* \param base
* The base address for the iso UART.
* \param fastTurnaround
* Broadcast write turnaround time:
* - true  : Fast turnaround time (clk_sys <= clk_iuart).
* - false : Normal turnaround time (clk_sys > clk_iuart).
*
*******************************************************************************/
void Cy_ISOUART_SetBroadcastTurnaroundTime(ISOUART_Type* base, bool fastTurnaround)
{
    CY_ASSERT_L1(NULL != base);

    if (fastTurnaround)
    {
        base->CTL |= ISOUART_CTL_BC_WRITE_TURN_TIME_CTL_Msk;
    }
    else
    {
        base->CTL &= ~ISOUART_CTL_BC_WRITE_TURN_TIME_CTL_Msk;
    }
}

#if (0U != ISOUART_MASTER)
/*******************************************************************************
* Function Name: Cy_ISOUART_HostInit
****************************************************************************//**
*
* \brief
* Full Init of the iso UART in host mode.
* \param base
* The base address for the iso UART.
* \param config
* Configuration options for the iso UART in host mode. See \ref cy_stc_isouart_host_config_t.
* \note
* This function is valid only for Host.
* \warning
* This function depends on the SysClk frequency and requires re-initialization
* if it changes to avoid frame errors.
* Ensure that CSV events (if enabled) are handled properly and trigger
* re-initialization if the SysClk source switches to the backup source.
* \funcusage
* \snippet isouart_snippet.c snippet_Cy_ISOUART_ConfigHost
*
*******************************************************************************/
void Cy_ISOUART_HostInit(ISOUART_Type* base, const cy_stc_isouart_host_config_t* config)
{
    CY_ASSERT_L1(NULL != base);
    CY_ASSERT_L1(NULL != config);
    CY_ASSERT_L3(CY_ISOUART_MOB_RING >= config->topology);
    CY_ASSERT_L3(CY_ISOUART_HOST_BITRATE_3_0MBPS >= config->hostBitrate);
    CY_ASSERT_L3(CY_ISOUART_RECEIVER_MODE_RAW >= config->receiverMode);
    CY_ASSERT_L3(CY_ISOUART_CRC8_H2F >= config->crcPolicy);

    base->CTL = (_VAL2FLD(ISOUART_CTL_IFL_L_CMOSRCV_EN, 0U) | \
                 _VAL2FLD(ISOUART_CTL_IFL_H_CMOSRCV_EN, 0U) | \
                 _VAL2FLD(ISOUART_CTL_IFH_L_CMOSRCV_EN, 0U) | \
                 _VAL2FLD(ISOUART_CTL_IFH_H_CMOSRCV_EN, 0U) | \
                 _VAL2FLD(ISOUART_CTL_IFL_L_AUX_EN, 0U) | \
                 _VAL2FLD(ISOUART_CTL_IFL_H_AUX_EN, 0U) | \
                 _VAL2FLD(ISOUART_CTL_IFH_L_AUX_EN, 0U) | \
                 _VAL2FLD(ISOUART_CTL_IFH_H_AUX_EN, 0U) | \
                 _VAL2FLD(ISOUART_CTL_REGULATOR_BYPASS, 0U) | \
                 _VAL2FLD(ISOUART_CTL_CRC_POLY, (uint32_t)config->crcPolicy) | \
                 _VAL2FLD(ISOUART_CTL_BLOCK_ID_EN, ((0U != (uint8_t)config->enableBlockId) ? 1U : 0U)));

    Cy_ISOUART_SetBroadcastTurnaroundTime(base, (Cy_SysClk_ClkSysGetFrequency() <= CY_ISOUART_CLK_IUART_FREQ));

    /* Enable ECC for iso UART SRAM */
    base->ECC_CTL |= ISOUART_ECC_CTL_ECC_EN_Msk;

    Cy_ISOUART_InitSram(base);

    /* Multi-read configuration */
    CY_REG32_CLR_SET(base->MULTI_READ_CTL, ISOUART_MULTI_READ_CTL_MULTI_READ_BC_EN, ((false != config->enableMultiReadBC) ? 1U : 0U));

    base->MASTER_CFG = (_VAL2FLD(ISOUART_MASTER_CFG_MASTER_BITRATE, (uint32_t)config->hostBitrate) | \
                        _VAL2FLD(ISOUART_MASTER_CFG_RECEIVE_MODE, (uint32_t)config->receiverMode) | \
                        _VAL2FLD(ISOUART_MASTER_CFG_TRIG0_WRITE_DONE, ((0U != (uint8_t)config->useWriteDoneTrig) ? 1U : 0U)));

    /* Configure topology-specific settings */
    switch(config->topology)
    {
        case CY_ISOUART_MOT:
        {
            CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_LOW_INTERFACE_MODE, CY_ISOUART_INTERFACE_MODE_HOST);
            CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_HIGH_INTERFACE_MODE, CY_ISOUART_INTERFACE_MODE_NODE);
            CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_HIGH_INTERFACE_SRAM_SIZE, 0U);
            CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_TRIG2_HIGH_SRAM_FULL, 0U);
            /* SRAM configuration for Register mode */
            if (CY_ISOUART_RECEIVER_MODE_REGISTER == config->receiverMode)
            {
                CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_LOW_INTERFACE_SRAM_SIZE, 0U);
                CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_TRIG1_LOW_SRAM_FULL, 0U);
            }
            else
            {
                CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_LOW_INTERFACE_SRAM_SIZE, (CY_ISOUART_RECEIVER_MODE_UNPACKED == config->receiverMode) ? 1U : 3U);
                CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_TRIG1_LOW_SRAM_FULL, (false != config->useLowSramFullTrig) ? 1U : 0U);
            }
            break;
        }
        case CY_ISOUART_MOB:
        {
            CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_LOW_INTERFACE_MODE, CY_ISOUART_INTERFACE_MODE_NODE);
            CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_HIGH_INTERFACE_MODE, CY_ISOUART_INTERFACE_MODE_HOST);
            CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_LOW_INTERFACE_SRAM_SIZE, 0U);
            CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_TRIG1_LOW_SRAM_FULL, 0U);
            /* SRAM configuration for Register mode */
            if (CY_ISOUART_RECEIVER_MODE_REGISTER == config->receiverMode)
            {
                CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_HIGH_INTERFACE_SRAM_SIZE, 0U);
                CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_TRIG2_HIGH_SRAM_FULL, 0U);
            }
            else
            {
                CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_HIGH_INTERFACE_SRAM_SIZE, (CY_ISOUART_RECEIVER_MODE_UNPACKED == config->receiverMode) ? 1U : 3U);
                CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_TRIG2_HIGH_SRAM_FULL, (false != config->useHighSramFullTrig) ? 1U : 0U);
            }
            break;
        }
        case CY_ISOUART_MOB_RING:
        {
            CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_LOW_INTERFACE_MODE, CY_ISOUART_INTERFACE_MODE_RECEIVER);
            CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_HIGH_INTERFACE_MODE, CY_ISOUART_INTERFACE_MODE_HOST);
            /* SRAM configuration for Register mode */
            if (CY_ISOUART_RECEIVER_MODE_REGISTER == config->receiverMode)
            {
                CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_LOW_INTERFACE_SRAM_SIZE, 0U);
                CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_HIGH_INTERFACE_SRAM_SIZE, 0U);
                CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_TRIG1_LOW_SRAM_FULL, 0U);
                CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_TRIG2_HIGH_SRAM_FULL, 0U);
            }
            else
            {
                CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_LOW_INTERFACE_SRAM_SIZE, (CY_ISOUART_RECEIVER_MODE_UNPACKED == config->receiverMode) ? 1U : 3U);
                CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_HIGH_INTERFACE_SRAM_SIZE, (CY_ISOUART_RECEIVER_MODE_UNPACKED == config->receiverMode) ? 1U : 3U);
                CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_TRIG1_LOW_SRAM_FULL, (false != config->useLowSramFullTrig) ? 1U : 0U);
                CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_TRIG2_HIGH_SRAM_FULL, (false != config->useHighSramFullTrig) ? 1U : 0U);
            }
            break;
        }
        case CY_ISOUART_MOT_RING:
        {
            CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_LOW_INTERFACE_MODE, CY_ISOUART_INTERFACE_MODE_HOST);
            CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_HIGH_INTERFACE_MODE, CY_ISOUART_INTERFACE_MODE_RECEIVER);
            /* SRAM configuration for Register mode */
            if (CY_ISOUART_RECEIVER_MODE_REGISTER == config->receiverMode)
            {
                CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_LOW_INTERFACE_SRAM_SIZE, 0U);
                CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_HIGH_INTERFACE_SRAM_SIZE, 0U);
                CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_TRIG1_LOW_SRAM_FULL, 0U);
                CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_TRIG2_HIGH_SRAM_FULL, 0U);
            }
            else
            {
                CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_LOW_INTERFACE_SRAM_SIZE, (CY_ISOUART_RECEIVER_MODE_UNPACKED == config->receiverMode) ? 1U : 3U);
                CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_HIGH_INTERFACE_SRAM_SIZE, (CY_ISOUART_RECEIVER_MODE_UNPACKED == config->receiverMode) ? 1U : 3U);
                CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_TRIG1_LOW_SRAM_FULL, (false != config->useLowSramFullTrig) ? 1U : 0U);
                CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_TRIG2_HIGH_SRAM_FULL, (false != config->useHighSramFullTrig) ? 1U : 0U);
            }
            break;
        }
        default:
        {
            /* Unknown state */
            break;
        }
    }

    /* Set internal loopback */
    CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_INTERNAL_LOOPBACK, ((false != config->enableInternalLoopback) ? 1U : 0U));

    /* Clear and disable whole interrupt */
    base->INTR_MASK = 0U;
    base->INTR = CY_ISOUART_INTR_MASK_ALL;

    /* Clear wake sequence flag */
    Cy_ISOUART_WakeSequenceFlag = false;
}

/*******************************************************************************
* Function Name: Cy_ISOUART_HostEnableWakeSequence
****************************************************************************//**
*
* \brief
* Enables the transmission of wake sequence.
* The registered wake sequence will be sent prior to next command transmission.
* \param base
* The base address for the iso UART.
* \param wakeCmdDelay
* Amount of delay between wake sequence and command frame.
* Each count corresponds to 1/(iso UART IP input clock [Hz]) seconds.
* Valid range is [0x2EE0 ... 0x3FFC].
* The appropriate delay value is specific to the node hardware - refer to the
* target node IC datasheet for the required delay specification.
* \note
* This function is valid only for Host.
* \funcusage
* \snippet isouart_snippet.c snippet_Cy_ISOUART_EnumerateNodes
*
*******************************************************************************/
void Cy_ISOUART_HostEnableWakeSequence(ISOUART_Type* base, uint32_t wakeCmdDelay)
{
    CY_ASSERT_L1(NULL != base);
    CY_ASSERT_L2(((wakeCmdDelay >= CY_ISOUART_MIN_WAKE_CMD_DELAY) && (wakeCmdDelay <= CY_ISOUART_MAX_WAKE_CMD_DELAY)));

    base->WAKE_DELAY_CFG = wakeCmdDelay;

    /* Enable the flag for sending wake sequence */
    Cy_ISOUART_WakeSequenceFlag = true;
}

/*******************************************************************************
* Function Name: Cy_ISOUART_HostSendWriteCommand
****************************************************************************//**
*
* \brief
* Sends a write command frame (sequenced mode) to a node.
* If a wake sequence was previously enabled with Cy_ISOUART_HostEnableWakeSequence(),
* the wake sequence is transmitted automatically before this frame.
* \param base
* The base address for the iso UART.
* \param blockId
* Block ID placed in the frame when block ID feature is enabled.
* Valid range: 0..6 (ignored if block ID is disabled).
* \param nodeId
* Target node ID. \n
*   0     : Enumeration \n
*   1..30 : Normal node IDs \n
*   63    : Broadcast
* \param address
* Target register / SRAM address. Valid range: 0..127.
* \param data
* 16-bit payload to be written.
* \note
* - Clears the internal wake sequence pending flag after transmission.
* - This function is valid only for Host.
* \funcusage
* \snippet isouart_snippet.c snippet_Cy_ISOUART_HostWrite
*
*******************************************************************************/
void Cy_ISOUART_HostSendWriteCommand(ISOUART_Type* base,
                                 uint32_t blockId,
                                 uint32_t nodeId,
                                 uint32_t address,
                                 uint16_t data)
{
    CY_ASSERT_L1(NULL != base);
    CY_ASSERT_L2(CY_ISOUART_MAX_ADDRESS >= address);
    CY_ASSERT_L2(CY_ISOUART_BLOCK_ID_VALID(base, blockId));
    CY_ASSERT_L2(CY_ISOUART_NODE_ID_VALID(nodeId));

    /* Configure sequenced mode for this command */
    base->MASTER_CFG |= ISOUART_MASTER_CFG_MASTER_MODE_Msk;
    CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_TX_BLOCK_ID, (_FLD2VAL(ISOUART_CTL_BLOCK_ID_EN, base->CTL) != 0U) ? blockId : 0U);

    /* Trigger transmission (write causes hardware to transmit frame) */
    base->MASTER_START = (_VAL2FLD(ISOUART_MASTER_START_WRITE_DATA, (uint32_t)data) |
                          _VAL2FLD(ISOUART_MASTER_START_ADDRESS, address) |
                          _VAL2FLD(ISOUART_MASTER_START_DEVICE_ID, nodeId) |
                          _VAL2FLD(ISOUART_MASTER_START_READ_WRITE, 1U) |
                          _VAL2FLD(ISOUART_MASTER_START_SEND_WAKE_SEQUENCE, ((false == Cy_ISOUART_WakeSequenceFlag) ? 0U : 1U)));

    Cy_ISOUART_WakeSequenceFlag = false;
}

/*******************************************************************************
* Function Name: Cy_ISOUART_HostSendReadCommand
****************************************************************************//**
*
* \brief
* Sends a read command frame (sequenced mode) to a node.
* If a wake sequence was previously enabled with Cy_ISOUART_HostEnableWakeSequence(),
* the wake sequence is transmitted automatically before this frame.
* \param base
* The base address for the iso UART.
* \param blockId
* Block ID placed in the frame when the block ID feature is enabled.
* Valid range: 0..6 (ignored if the feature is disabled).
* \param nodeId
* Target node ID. \n
*   0     : Enumeration \n
*   1..30 : Normal node IDs \n
*   63    : Broadcast
* \param address
* Target register / SRAM address. Valid range: 0..127.
* \note
* - Clears the internal wake sequence pending flag after transmission.
* - This function is valid only for Host.
* \funcusage
* \snippet isouart_snippet.c snippet_Cy_ISOUART_ReadNode
*
*******************************************************************************/
void Cy_ISOUART_HostSendReadCommand(ISOUART_Type* base,
                                uint32_t blockId,
                                uint32_t nodeId,
                                uint32_t address)
{
    CY_ASSERT_L1(NULL != base);
    CY_ASSERT_L2(CY_ISOUART_MAX_ADDRESS >= address);
    CY_ASSERT_L2(CY_ISOUART_BLOCK_ID_VALID(base, blockId));
    CY_ASSERT_L2(CY_ISOUART_NODE_ID_VALID(nodeId));

    /* Configure sequenced mode for this command */
    base->MASTER_CFG |= ISOUART_MASTER_CFG_MASTER_MODE_Msk |
                        ISOUART_MASTER_CFG_RESET_LOW_INTERFACE_SRAM_PTR_Msk |
                        ISOUART_MASTER_CFG_RESET_HIGH_INTERFACE_SRAM_PTR_Msk;
    CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_TX_BLOCK_ID, (_FLD2VAL(ISOUART_CTL_BLOCK_ID_EN, base->CTL) != 0U) ? blockId : 0U);

    /* Trigger transmission (write causes hardware to transmit frame) */
    base->MASTER_START = (_VAL2FLD(ISOUART_MASTER_START_ADDRESS, address) |
                          _VAL2FLD(ISOUART_MASTER_START_DEVICE_ID, nodeId) |
                          _VAL2FLD(ISOUART_MASTER_START_READ_WRITE, 0U) |
                          _VAL2FLD(ISOUART_MASTER_START_SEND_WAKE_SEQUENCE, ((false == Cy_ISOUART_WakeSequenceFlag) ? 0U : 1U)));

    /* Clear wake sequence flag after use */
    Cy_ISOUART_WakeSequenceFlag = false;
}

/*******************************************************************************
* Function Name: Cy_ISOUART_HostSendManualCommand
****************************************************************************//**
*
* \brief
* Sends a single 8-bit frame in manual mode.
* If a wake sequence was previously enabled with Cy_ISOUART_HostEnableWakeSequence(),
* the wake sequence is transmitted automatically before this frame.
* \param base
* The base address for the iso UART.
* \param data
* 1 byte frame to transmit.
* \note
* - Clears the internal wake sequence pending flag after transmission.
* - This function is valid only for Host.
*
*******************************************************************************/
void Cy_ISOUART_HostSendManualCommand(ISOUART_Type* base, uint8_t data)
{
    CY_ASSERT_L1(NULL != base);

    /* Configure manual mode for this command */
    base->MASTER_CFG &= ~ISOUART_MASTER_CFG_MASTER_MODE_Msk;
    base->MASTER_CFG |= ISOUART_MASTER_CFG_RESET_LOW_INTERFACE_SRAM_PTR_Msk |
                        ISOUART_MASTER_CFG_RESET_HIGH_INTERFACE_SRAM_PTR_Msk;

    /* Trigger transmission (write causes hardware to transmit frame) */
    base->MASTER_START = (_VAL2FLD(ISOUART_MASTER_START_WRITE_DATA, (uint32_t)data) |
                          _VAL2FLD(ISOUART_MASTER_START_SEND_WAKE_SEQUENCE, ((false == Cy_ISOUART_WakeSequenceFlag) ? 0U : 1U)));

    /* Clear wake sequence flag after use */
    Cy_ISOUART_WakeSequenceFlag = false;
}

/*******************************************************************************
* Function Name: Cy_ISOUART_HostVerifyCRC
****************************************************************************//**
*
* \brief
*  Verifies the CRC contained in a write response byte.
*  Compares the lower 3 bits (CRC) of the supplied write response byte against
*  the expected CRC looked up using the upper 5 bits as index.
*  The response can be obtained via \ref Cy_ISOUART_HostReadLowInterface() or
*  \ref Cy_ISOUART_HostReadHighInterface(). An interrupt after the write command
*  completion indicates the reply is available.
* \param frameData
*  Write response frame byte (bits[7:3] = payload, bits[2:0] = CRC).
* \return
*  true  - CRC matches the expected value (response byte is valid).
*  false - CRC mismatch (response byte is invalid).
* \note
*  This function is valid only for Host.
* \funcusage
* \snippet isouart_snippet.c snippet_Cy_ISOUART_HostWrite
*
*******************************************************************************/
bool Cy_ISOUART_HostVerifyCRC(uint8_t frameData)
{
    /* Compare the lower 3 bit data(3-bit CRC) of frame data with the expected value. */
    return ((frameData & 0x7U) == Cy_ISOUART_Crc3BitLut[frameData >> 3U]);
}
#endif /* (0U != ISOUART_MASTER) */

/*******************************************************************************
* Function Name: Cy_ISOUART_NodeInit
****************************************************************************//**
*
* \brief
* Full Init of the iso UART in node mode.
* \param base
* The base address for the iso UART.
* \param config
* Configuration options for the iso UART in node mode. See \ref cy_stc_isouart_node_config_t.
* \note
* This function is valid only for Node.
* \warning
* This function depends on the SysClk frequency and requires re-initialization
* if it changes to avoid frame errors.
* Ensure that CSV events (if enabled) are handled properly and trigger
* re-initialization if the SysClk source switches to the backup source.
* \funcusage
* \snippet isouart_snippet.c snippet_Cy_ISOUART_ConfigNode
*
*******************************************************************************/
void Cy_ISOUART_NodeInit(ISOUART_Type* base,  const cy_stc_isouart_node_config_t* config)
{
    CY_ASSERT_L1(NULL != base);
    CY_ASSERT_L1(NULL != config);
    CY_ASSERT_L3(CY_ISOUART_CRC8_H2F >= config->crcPolicy);
    CY_ASSERT_L3(CY_ISOUART_MULTI_READ_MAX_BASE_ADDR >= config->baseAddressA);
    CY_ASSERT_L3(CY_ISOUART_MULTI_READ_MAX_BASE_ADDR >= config->baseAddressB);

    base->CTL = (_VAL2FLD(ISOUART_CTL_IFL_L_CMOSRCV_EN, 0U) | \
                 _VAL2FLD(ISOUART_CTL_IFL_H_CMOSRCV_EN, 0U) | \
                 _VAL2FLD(ISOUART_CTL_IFH_L_CMOSRCV_EN, 0U) | \
                 _VAL2FLD(ISOUART_CTL_IFH_H_CMOSRCV_EN, 0U) | \
                 _VAL2FLD(ISOUART_CTL_IFL_L_AUX_EN, 0U) | \
                 _VAL2FLD(ISOUART_CTL_IFL_H_AUX_EN, 0U) | \
                 _VAL2FLD(ISOUART_CTL_IFH_L_AUX_EN, 0U) | \
                 _VAL2FLD(ISOUART_CTL_IFH_H_AUX_EN, 0U) | \
                 _VAL2FLD(ISOUART_CTL_REGULATOR_BYPASS, 0U) | \
                 _VAL2FLD(ISOUART_CTL_CRC_POLY, (uint32_t)config->crcPolicy) | \
                 _VAL2FLD(ISOUART_CTL_BLOCK_ID_EN, ((0U != (uint8_t)config->enableBlockId) ? 1U : 0U)) | \
                 _VAL2FLD(ISOUART_CTL_TRIG0_EMM, ((0U != (uint8_t)config->useTrig0Emm) ? 1U : 0U)) | \
                 _VAL2FLD(ISOUART_CTL_TRIG1_EMM, ((0U != (uint8_t)config->useTrig1Emm) ? 1U : 0U)) | \
                 _VAL2FLD(ISOUART_CTL_TRIG2_EMM, ((0U != (uint8_t)config->useTrig2Emm) ? 1U : 0U)) | \
                 _VAL2FLD(ISOUART_CTL_TRIG3_EMM, ((0U != (uint8_t)config->useTrig3Emm) ? 1U : 0U)));

    Cy_ISOUART_SetBroadcastTurnaroundTime(base, (Cy_SysClk_ClkSysGetFrequency() <= CY_ISOUART_CLK_IUART_FREQ));

    /* Enable ECC for iso UART SRAM */
    base->ECC_CTL |= ISOUART_ECC_CTL_ECC_EN_Msk;

    Cy_ISOUART_InitSram(base);

    /* Multi-read configuration */
    CY_REG32_CLR_SET(base->MULTI_READ_CTL, ISOUART_MULTI_READ_CTL_MULTI_READ_BC_EN, ((false != config->enableMultiReadBC) ? 1U : 0U));
    CY_REG32_CLR_SET(base->MULTI_READ_CTL, ISOUART_MULTI_READ_CTL_MULTI_READ_BASE_A, config->baseAddressA);
    CY_REG32_CLR_SET(base->MULTI_READ_CTL, ISOUART_MULTI_READ_CTL_MULTI_READ_BASE_B, config->baseAddressB);

    CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_LOW_INTERFACE_MODE, CY_ISOUART_INTERFACE_MODE_NODE);
    CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_HIGH_INTERFACE_MODE, CY_ISOUART_INTERFACE_MODE_NODE);

    /* Clear and disable whole interrupt */
    base->INTR_MASK = 0U;
    base->INTR = CY_ISOUART_INTR_MASK_ALL;

    /* Clear wake sequence flag */
    Cy_ISOUART_WakeSequenceFlag = false;
}

/*******************************************************************************
* Function Name: Cy_ISOUART_NodeConfigSramRowAttr
****************************************************************************//**
*
* \brief
* Configure iso UART SRAM attributes.
* \param base
* The base address for the iso UART.
* \param index
* Index of iso UART SRAM row. The valid range is 0...127.
* \param trigIntId
* ID of trigger and interrupt struct to be tied to this SRAM row.
* The valid range is 0...17.
* \param isInvalid
* Specifies whether the row is invalid or not.
* If TRUE, the specified index is marked as invalid. Any access to the invalid row
* by the host via iso UART will be reported as address error.
* \param isReadOnly
* Specifies whether the row is read only or not.
* If TRUE, the specified index is marked as read only. A write access to the read
* only row by the host via iso UART will be reported as access error.
* \note
* This function is valid for Node. \n
* As an exception, this function may also be used by a Host configured in internal loopback mode
* to simulate Node behavior for testing purposes.
* \funcusage
* \snippet isouart_snippet.c snippet_Cy_ISOUART_NodeIntrTrigStructConfig
*
*******************************************************************************/
void Cy_ISOUART_NodeConfigSramRowAttr(ISOUART_Type* base,
                                     uint32_t index,
                                     uint8_t trigIntId,
                                     bool isInvalid,
                                     bool isReadOnly)
{
    CY_ASSERT_L1(NULL != base);
    CY_ASSERT_L2(CY_ISOUART_SIZE_OF_SRAM > index);
    CY_ASSERT_L2(ISOUART_NUMTISTRUCT > trigIntId);

    base->SRAM_WITH_ATTR[index] = (_VAL2FLD(ISOUART_SRAM_WITH_ATTR_DATA, 0U) | \
                                   _VAL2FLD(ISOUART_SRAM_WITH_ATTR_TRIG_INT_STRUCT_PTR, trigIntId) | \
                                   _VAL2FLD(ISOUART_SRAM_WITH_ATTR_ADDR_INVALID, (0U != (uint8_t)isInvalid) ? 1U : 0U)) | \
                                   _VAL2FLD(ISOUART_SRAM_WITH_ATTR_ADDR_READONLY, (0U != (uint8_t)isReadOnly) ? 1U : 0U);
}

/*******************************************************************************
* Function Name: Cy_ISOUART_HostSetSramSize
****************************************************************************//**
*
* \brief
* Set SRAM size for data reception.
* The required SRAM size depends on the read command types (read, broadcast read, multi read).
* This function is intended to be used for re-allocating SRAM size suitable for
* the read command being sent.
* \param base
* The base address for the iso UART.
* \param lowSramSize
* SRAM size in low side. The valid range is 0..127.
* In RAW mode, this parameter must be divisible by 3.
* \param highSramSize
* SRAM size in high side. The valid range is 0..127.
* In RAW mode, this parameter must be divisible by 3.
* \note
* This function is valid only for Host.
* The sum of lowSramSize and highSramSize must not exceed total SRAM size.
* The SRAM size shall only be changed when iso UART is disabled or both interfaces are in Slave mode.
* \funcusage
* \snippet isouart_snippet.c snippet_Cy_ISOUART_ReadNode
*
*******************************************************************************/
void Cy_ISOUART_HostSetSramSize(ISOUART_Type* base,
                            uint32_t lowSramSize,
                            uint32_t highSramSize)
{
    CY_ASSERT_L1(NULL != base);
    CY_ASSERT_L2(CY_ISOUART_SIZE_OF_SRAM > (lowSramSize + highSramSize));
    CY_ASSERT_L2(CY_ISOUART_MAX_SRAM_INDEX >= highSramSize);
    CY_ASSERT_L2(CY_ISOUART_MAX_SRAM_INDEX >= lowSramSize);
    CY_ASSERT_L2(CY_ISOUART_RAW_MODE_SRAM_SIZE_VALID(base, highSramSize, lowSramSize));
    CY_ASSERT_L2(CY_ISOUART_MOT_TOPOLOGY_SRAM_SIZE_VALID(base, lowSramSize));
    CY_ASSERT_L2(CY_ISOUART_MOB_TOPOLOGY_SRAM_SIZE_VALID(base, highSramSize, lowSramSize));
    CY_ASSERT_L2(CY_ISOUART_RING_TOPOLOGY_SRAM_SIZE_VALID(base, highSramSize, lowSramSize));

    /* Update SRAM sizes */
    CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_LOW_INTERFACE_SRAM_SIZE, lowSramSize);
    CY_REG32_CLR_SET(base->MASTER_CFG, ISOUART_MASTER_CFG_HIGH_INTERFACE_SRAM_SIZE, highSramSize);
}

/*******************************************************************************
* Function Name: Cy_ISOUART_HostGetSramBaseAddr
****************************************************************************//**
*
* \brief
* Get iso UART SRAM base address of the specified interface.
* This is a helper function for combination with DMA.
* \param base
* The base address for the iso UART.
* \param iSide
* Interface side. See \ref cy_en_isouart_interface_side_t.
* \return
* Pointer to the start address of the iso UART SRAM for the specified interface side.
* \note
* This function is valid only for Host.
* \funcusage
* \snippet isouart_snippet.c snippet_Cy_ISOUART_GetSramBaseAddress
*
*******************************************************************************/
volatile uint32_t * Cy_ISOUART_HostGetSramBaseAddr(ISOUART_Type* base, cy_en_isouart_interface_side_t iSide)
{
    CY_ASSERT_L1(NULL != base);
    CY_ASSERT_L3(CY_ISOUART_IF_HIGH >= iSide);

    /* Low side SRAM has no offset from the SRAM base */
    volatile uint32_t *baseAddr = &base->SRAM_DATA[0];

    if (iSide == CY_ISOUART_IF_HIGH)
    {
        /*
         * For the high side interface, the base address of the SRAM data
         * must be offset by the size of the low side interface.
         * Each SRAM row is 2 bytes (16 bits), so the offset is:
         * LOW_INTERFACE_SRAM_SIZE * 2.
         */
        baseAddr += (_FLD2VAL(ISOUART_MASTER_CFG_LOW_INTERFACE_SRAM_SIZE, base->MASTER_CFG) << 1U);
    }

    return baseAddr;
}

/*******************************************************************************
* Function Name: Cy_ISOUART_ReadSramRow16
****************************************************************************//**
*
* \brief
* Read a 16-bit data field from the specified iso UART SRAM row.
* \param base
* The base address for the iso UART.
* \param index
* Index of iso UART SRAM row. The valid range is 0...127.
* \return
* 16-bit value read from the iso UART SRAM row.
* \note
* This function is valid for both Host and Node.
* \funcusage
* \snippet isouart_snippet.c snippet_Cy_ISOUART_NodeInterruptHandler
*
*******************************************************************************/
uint16_t Cy_ISOUART_ReadSramRow16(ISOUART_Type* base, uint32_t index)
{
    CY_ASSERT_L1(NULL != base);
    CY_ASSERT_L2(CY_ISOUART_MAX_SRAM_INDEX >= index);

    return (uint16_t)_FLD2VAL(ISOUART_SRAM_WITH_ATTR_DATA, base->SRAM_WITH_ATTR[index]);
}

/*******************************************************************************
* Function Name: Cy_ISOUART_ReadSramRow32
****************************************************************************//**
*
* \brief
* Read a 32-bit data from two consecutive iso UART SRAM rows.
* The output 32-bit data is configured as follows:
*  - Bits [15:0] contain the data from the even index (index).
*  - Bits [31:16] contain the data from the odd index (index + 1).
* \param base
* The base address for the iso UART.
* \param index
* Index of iso UART SRAM row (must be even).
* The valid range is 0...126, and this parameter must be an even value.
* \return
* 32-bit value read from two consecutive iso UART SRAM rows.
* \note
* This function is valid for both Host and Node.
*
*******************************************************************************/
uint32_t Cy_ISOUART_ReadSramRow32(ISOUART_Type* base, uint32_t index)
{
    CY_ASSERT_L1(NULL != base);
    CY_ASSERT_L2(CY_ISOUART_MAX_SRAM_INDEX_32BIT >= index);
    CY_ASSERT_L2(CY_ISOUART_IS_INDEX_EVEN(index));

    return base->SRAM_DATA[index >> 1U];
}

/*******************************************************************************
* Function Name: Cy_ISOUART_WriteSramRow16
****************************************************************************//**
*
* \brief
* Write a 16-bit value to the data field of the specified iso UART SRAM row.
* \param base
* The base address for the iso UART.
* \param index
* Index of iso UART SRAM row. The valid range is 0...127.
* \param data
* 16-bit value to be written to the SRAM row.
* \note
* This function is valid for both Host and Node.
*
*******************************************************************************/
void Cy_ISOUART_WriteSramRow16(ISOUART_Type* base, uint32_t index, uint16_t data)
{
    CY_ASSERT_L1(NULL != base);
    CY_ASSERT_L2(CY_ISOUART_MAX_SRAM_INDEX >= index);

    CY_REG32_CLR_SET(base->SRAM_WITH_ATTR[index], ISOUART_SRAM_WITH_ATTR_DATA, data);
}

/*******************************************************************************
* Function Name: Cy_ISOUART_WriteSramRow32
****************************************************************************//**
*
* \brief
* Write a 32-bit data to two consecutive iso UART SRAM rows.
* The input 32-bit data should be configured as follows:
*  - Bits [15:0] should contain the data for the even index.
*  - Bits [31:16] should contain the data for the odd index.
* \param base
* The base address for the iso UART.
* \param index
* Index of iso UART SRAM row (must be even).
* The valid range is 0...126, and this parameter must be an even value.
* \param data
* 32-bit value to be written to two consecutive SRAM rows.
* \note
* This function is valid for both Host and Node.
*
*******************************************************************************/
void Cy_ISOUART_WriteSramRow32(ISOUART_Type* base, uint32_t index, uint32_t data)
{
    CY_ASSERT_L1(NULL != base);
    CY_ASSERT_L2(CY_ISOUART_MAX_SRAM_INDEX_32BIT >= index);
    CY_ASSERT_L2(CY_ISOUART_IS_INDEX_EVEN(index));

    base->SRAM_DATA[index >> 1U] = data;
}

/*******************************************************************************
* Function Name: Cy_ISOUART_HostReadSramRaw
****************************************************************************//**
*
* \brief
* Reads RAW data frames from the iso UART SRAM of specified interface.
* \param base
* The base address for the iso UART.
* \param iSide
* Interface side to be read. See \ref cy_en_isouart_interface_side_t.
* \param packetNum
* Packet number to be read.
* Valid range is [0, (allocated SRAM size for iSide / 3) - 1].
* \param frame
* Pointer to the structure for storing RAW frame data.
* See \ref cy_stc_isouart_raw_frame_t.
* \note
* This function is valid only when:
* - The interface is initialized as Host mode.
* - The receiver mode is configured as \ref CY_ISOUART_RECEIVER_MODE_RAW.
*
*******************************************************************************/
void Cy_ISOUART_HostReadSramRaw(ISOUART_Type* base,
                            cy_en_isouart_interface_side_t iSide,
                            uint32_t packetNum,
                            cy_stc_isouart_raw_frame_t* frame)
{
    CY_ASSERT_L1(NULL != base);
    CY_ASSERT_L1(NULL != frame);
    CY_ASSERT_L1(iSide <= CY_ISOUART_IF_HIGH);
    CY_ASSERT_L2(CY_ISOUART_IS_RECEIVER_MODE_RAW(base));
    CY_ASSERT_L2(!CY_ISOUART_INTERFACE_MODE_IS_NODE(base, iSide));
    CY_ASSERT_L2(CY_ISOUART_RAW_MODE_PACKET_NUM_VALID(base, iSide, packetNum));

    /* Convert packet number to SRAM index */
    uint32_t sramIdx = packetNum * CY_ISOUART_NUM_OF_ROW_IN_RAW_MODE;

    if (iSide == CY_ISOUART_IF_HIGH)
    {
        /* For the high interface side address must be incremented by size of low interface side */
        sramIdx += _FLD2VAL(ISOUART_MASTER_CFG_LOW_INTERFACE_SRAM_SIZE, base->MASTER_CFG);
    }

    /* In RAW mode the entire frame is stored in the iso UART SRAM. This includes ID, address, and CRC.
       Three SRAM rows are used in this mode. */
    frame->addr = (uint8_t)(base->SRAM_WITH_ATTR[sramIdx]);
    frame->nodeId = (uint8_t)(base->SRAM_WITH_ATTR[sramIdx] >> CY_ISOUART_BITSHIFT_RAW_DATA);
    sramIdx++;
    frame->data = (uint16_t)(base->SRAM_WITH_ATTR[sramIdx]);
    sramIdx++;
    frame->crc = (uint8_t)(base->SRAM_WITH_ATTR[sramIdx]);
}

/*******************************************************************************
* Function Name: Cy_ISOUART_HostReadSramUnpack
****************************************************************************//**
*
* \brief
* Reads UNPACKED data from the iso UART SRAM of specified interface.
* \param base
* The base address for the iso UART.
* \param iSide
* Interface side to be read. See \ref cy_en_isouart_interface_side_t.
* \param packetNum
* Packet number to be read.
* Valid range is [0, allocated SRAM size for iSide - 1].
* \return
* Unpacked data (16-bit value).
* \note
* This function is valid only when:
* - The interface is initialized as Host mode.
* - The receiver mode is configured as \ref CY_ISOUART_RECEIVER_MODE_UNPACKED.
* \funcusage
* \snippet isouart_snippet.c snippet_Cy_ISOUART_ReadNode
*
*******************************************************************************/
uint16_t Cy_ISOUART_HostReadSramUnpack(ISOUART_Type* base,
                                   cy_en_isouart_interface_side_t iSide,
                                   uint32_t packetNum)
{
    CY_ASSERT_L1(NULL != base);
    CY_ASSERT_L1(iSide <= CY_ISOUART_IF_HIGH);
    CY_ASSERT_L2(CY_ISOUART_IS_RECEIVER_MODE_UNPACKED(base));
    CY_ASSERT_L2(!CY_ISOUART_INTERFACE_MODE_IS_NODE(base, iSide));
    CY_ASSERT_L2(CY_ISOUART_UNPACKED_MODE_PACKET_NUM_VALID(base, iSide, packetNum));

    uint32_t sramIdx = packetNum;

    if (iSide == CY_ISOUART_IF_HIGH)
    {
        /* For the high interface side address must be incremented by size of low interface side */
        sramIdx += _FLD2VAL(ISOUART_MASTER_CFG_LOW_INTERFACE_SRAM_SIZE, base->MASTER_CFG);
    }

    /* In UNPACKED mode, only one row of iso UART SRAM is used per frame. */
    return (uint16_t)(base->SRAM_WITH_ATTR[sramIdx]);
}

/*******************************************************************************
* Function Name: Cy_ISOUART_EnableEccInjection
****************************************************************************//**
*
* \brief
* Enable ECC error injection for iso UART SRAM.
* \param base
* The base address for the iso UART.
* \param index
* Index of iso UART SRAM row where an error will be injected. The valid range is 0...127.
* \param parity
* ECC parity to use for ECC error injection at SRAM index. The valid range is 0...127.
* During the ECC error injection is enabled, when the SRAM[index] is written,
* ECC parity is forced to be updated with this value instead of the value calculated by HW.
* \note
* This function is valid for both Host and Node. \n
* During the ECC error injection is enabled, access to the SRAM[index] will never cause
* bus error on uncorrectable ECC error or address error.
* Reporting to FAULT structure is valid even if the ECC error injection is enabled.
* The ECC error injection mechanism actually updates the SRAM parity, so the inserted
* ECC error will still cause the error after the ECC error injection is disabled.
* It's recommended to re-initialize the SRAM[index] with 32 bits write
* after disabling the ECC error injection to reset the ECC parity with correct value.
*
*******************************************************************************/
void Cy_ISOUART_EnableEccInjection(ISOUART_Type* base, uint32_t index, uint32_t parity)
{
    CY_ASSERT_L1(NULL != base);
    CY_ASSERT_L2(CY_ISOUART_SIZE_OF_SRAM > index);
    CY_ASSERT_L2(CY_ISOUART_MAX_ECC_PARITY > parity);
    /* Set error injection address and parity, and enable error injection */
    base->ECC_ERR_INJ = _VAL2FLD(ISOUART_ECC_ERR_INJ_ERR_ADDR, (uint32_t)&base->SRAM_WITH_ATTR[index]) |
                        _VAL2FLD(ISOUART_ECC_ERR_INJ_ERR_PAR, parity) |
                        ISOUART_ECC_ERR_INJ_ERR_EN_Msk;
}

/*******************************************************************************
* Function Name: Cy_ISOUART_DisableEccInjection
****************************************************************************//**
*
* \brief
* Disable ECC error injection for iso UART SRAM.
* \param base
* The base address for the iso UART.
* \note
* This function is valid for both Host and Node.
*
*******************************************************************************/
void Cy_ISOUART_DisableEccInjection(ISOUART_Type* base)
{
    CY_ASSERT_L1(NULL != base);
    /* Disable ECC injection */
    base->ECC_ERR_INJ = 0U;
}

/*******************************************************************************
* Function Name: Cy_ISOUART_GetEccStatus
****************************************************************************//**
*
* \brief
* Get ECC status of iso UART SRAM.
* \param base
* The base address for the iso UART.
* \param status
* Pointer to a status structure to be filled. See \ref cy_stc_isouart_ecc_status_t.
* \note
* This function is valid for both Host and Node.
*
*******************************************************************************/
void Cy_ISOUART_GetEccStatus(const ISOUART_Type* base, cy_stc_isouart_ecc_status_t* status)
{
    CY_ASSERT_L1(NULL != base);
    CY_ASSERT_L1(NULL != status);

    uint32_t reg = base->ECC_STATUS;
    bool corrected = _FLD2BOOL(ISOUART_ECC_STATUS_CORR_ERR, reg);
    bool notCorrected = _FLD2BOOL(ISOUART_ECC_STATUS_NOT_CORR_ERR, reg);

    /* Determine error type */
    if (notCorrected)
    {
        status->errorType = CY_ISOUART_ECC_NOT_CORRECTED;
    }
    else if (corrected)
    {
        status->errorType = CY_ISOUART_ECC_CORRECTED;
    }
    else
    {
        status->errorType = CY_ISOUART_ECC_NO_ERROR;
    }

    status->syndrome = _FLD2VAL(ISOUART_ECC_STATUS_SYNDROME, reg);
}

/*******************************************************************************
* Function Name: Cy_ISOUART_NodeConfigTriggerInt
****************************************************************************//**
*
* \brief
* Configure one trigger and interrupt struct.
* \param base
* The base address for the iso UART.
* \param trigIntId
* The trigger interrupt ID. The valid range is from 0 to 17.
* \param trigSrc
* Source of trigger. See \ref cy_en_isouart_event_t.
* \param primIntSrc
* Source of primary interrupt. See \ref cy_en_isouart_event_t.
* \param secIntSrc
* Source of secondary interrupt. See \ref cy_en_isouart_event_t.
* \note
* This function is valid only for Node.
* \funcusage
* \snippet isouart_snippet.c snippet_Cy_ISOUART_NodeIntrTrigStructConfig
*
*******************************************************************************/
void Cy_ISOUART_NodeConfigTriggerInt(ISOUART_Type* base,
                                 uint8_t trigIntId,
                                 cy_en_isouart_event_t trigSrc,
                                 cy_en_isouart_event_t primIntSrc,
                                 cy_en_isouart_event_t secIntSrc)
{
    CY_ASSERT_L1(NULL != base);
    CY_ASSERT_L2(ISOUART_NUMTISTRUCT > trigIntId);
    CY_ASSERT_L3(CY_ISOUART_EVENT_LATE_READ >= trigSrc);
    CY_ASSERT_L3(CY_ISOUART_EVENT_LATE_READ >= primIntSrc);
    CY_ASSERT_L3(CY_ISOUART_EVENT_LATE_READ >= secIntSrc);
    /* Configure trigger interrupt */
    base->TRIG_INT_STRUCT[trigIntId] = (_VAL2FLD(ISOUART_TRIG_INT_STRUCT_TRIG_SRC, trigSrc) | \
                                        _VAL2FLD(ISOUART_TRIG_INT_STRUCT_PRIM_INT_SRC, primIntSrc) | \
                                        _VAL2FLD(ISOUART_TRIG_INT_STRUCT_SEC_INT_SRC, secIntSrc));
}

/*******************************************************************************
* Function Name: Cy_ISOUART_ConfigIORouting
****************************************************************************//**
*
* \brief
* Configure all dedicated iso UART pins as regular GPIOs so that they can be used by other peripheral blocks.
* \param base
* The base address for the iso UART.
* \param enInputIflLow
* Determines whether IFL_L operates as a GPIO input (true) or output (false).
* \param enInputIflHigh
* Determines whether IFL_H operates as a GPIO input (true) or output (false).
* \param enInputIfhLow
* Determines whether IFH_L operates as a GPIO input (true) or output (false).
* \param enInputIfhHigh
* Determines whether IFH_H operates as a GPIO input (true) or output (false).
* \warning
* When this function is used, iso UART functionality becomes unavailable.
* \note
* This function is valid for both Host and Node. \n
* To enable/disable the IO pin setting, call \ref Cy_ISOUART_Enable/\ref Cy_ISOUART_Disable after calling \ref Cy_ISOUART_ConfigIORouting. \n
* To restore all dedicated iso UART pins to iso UART functionality, call \ref Cy_ISOUART_NodeInit or \ref Cy_ISOUART_HostInit.
* \funcusage
* \snippet isouart_snippet.c snippet_Cy_ISOUART_ReconfigurePinsAsGpio
*
*******************************************************************************/
void Cy_ISOUART_ConfigIORouting(ISOUART_Type* base,
                                bool enInputIflLow,
                                bool enInputIflHigh,
                                bool enInputIfhLow,
                                bool enInputIfhHigh)
{
    CY_ASSERT_L1(NULL != base);
    /* Configure I/O routing */
    base->CTL = (_VAL2FLD(ISOUART_CTL_REGULATOR_BYPASS, 1U) | \
                 _VAL2FLD(ISOUART_CTL_IFL_L_AUX_EN, 1U) | \
                 _VAL2FLD(ISOUART_CTL_IFL_H_AUX_EN, 1U) | \
                 _VAL2FLD(ISOUART_CTL_IFH_L_AUX_EN, 1U) | \
                 _VAL2FLD(ISOUART_CTL_IFH_H_AUX_EN, 1U) | \
                 _VAL2FLD(ISOUART_CTL_IFL_L_CMOSRCV_EN, ((uint8_t)enInputIflLow == 0U) ? 0U : 1U) | \
                 _VAL2FLD(ISOUART_CTL_IFL_H_CMOSRCV_EN, ((uint8_t)enInputIflHigh == 0U) ? 0U : 1U) | \
                 _VAL2FLD(ISOUART_CTL_IFH_L_CMOSRCV_EN, ((uint8_t)enInputIfhLow == 0U) ? 0U : 1U) | \
                 _VAL2FLD(ISOUART_CTL_IFH_H_CMOSRCV_EN, ((uint8_t)enInputIfhHigh == 0U) ? 0U : 1U));
}

/*******************************************************************************
* Function Name: Cy_ISOUART_GetInterfaceStatus
****************************************************************************//**
*
* \brief
* Get the current state of the interface. Performs a single register read and decodes all fields.
* Hardware is not latched or cleared. Some fields in the structure are only meaningful in
* certain interface states (see \ref cy_stc_isouart_interface_status_t for details).
* \param base
* The base address for the iso UART.
* \param status
* Pointer to a status structure to be filled.
* \note
* This function is valid for both Host and Node.
*
*******************************************************************************/
void Cy_ISOUART_GetInterfaceStatus(ISOUART_Type* base, cy_stc_isouart_interface_status_t* status)
{
    CY_ASSERT_L1(NULL != base);
    CY_ASSERT_L1(NULL != status);

    uint32_t reg = base->INTERFACE_STATUS;

    CY_MISRA_DEVIATE_BLOCK_START('MISRA C-2012 Rule 10.8', 4, \
        'Casting uint32_t to enumeration types is safe as the register field values match the enumeration ranges.');
    status->interfaceState = (cy_en_isouart_interface_state_t)_FLD2VAL(ISOUART_INTERFACE_STATUS_INTERFACE_STATE, reg);
    status->requestState   = (cy_en_isouart_request_state_t)_FLD2VAL(ISOUART_INTERFACE_STATUS_REQUEST_STATE, reg);
    status->hostCommand    = (cy_en_isouart_host_command_t)_FLD2VAL(ISOUART_INTERFACE_STATUS_HOST_COMMAND, reg);
    status->replyState     = (cy_en_isouart_reply_state_t)_FLD2VAL(ISOUART_INTERFACE_STATUS_REPLY_STATE, reg);
    CY_MISRA_BLOCK_END('MISRA C-2012 Rule 10.8');
    status->txDeviceId     = (uint32_t)_FLD2VAL(ISOUART_INTERFACE_STATUS_DEVID_TRANSMITTING, reg);
    status->packetNumber   = (uint32_t)_FLD2VAL(ISOUART_INTERFACE_STATUS_PACKET_NUMBER, reg);
}

/*******************************************************************************
* Function Name: Cy_ISOUART_NodeGetNumMultiReadA
****************************************************************************//**
*
* \brief
* Get the number of reads from base address A after receiving multi-read command.
* \param base
* The base address for the iso UART.
* \return
* Number of reads from base address A.
* \note
* This function is valid only for Node.
*
*******************************************************************************/
uint32_t Cy_ISOUART_NodeGetNumMultiReadA(const ISOUART_Type* base)
{
    CY_ASSERT_L1(NULL != base);

    uint32_t config = base->IF_MULTI_READ_CFG;
    uint32_t num = _FLD2VAL(CY_ISOUART_IF_MULTI_READ_CFG_A_READ_NUM, config);
    bool subtract = _FLD2BOOL(CY_ISOUART_IF_MULTI_READ_CFG_A_SUBTRACT, config);

    return subtract ? num + 1U : num;
}

/*******************************************************************************
* Function Name: Cy_ISOUART_NodeGetNumMultiReadB
****************************************************************************//**
*
* \brief
* Get the number of reads from base address B after receiving multi-read command.
* \param base
* The base address for the iso UART.
* \return
* Number of reads from base address B.
* \note
* This function is valid only for Node.
*
*******************************************************************************/
uint32_t Cy_ISOUART_NodeGetNumMultiReadB(const ISOUART_Type* base)
{
    CY_ASSERT_L1(NULL != base);

    uint32_t config = base->IF_MULTI_READ_CFG;
    uint32_t num = _FLD2VAL(CY_ISOUART_IF_MULTI_READ_CFG_B_READ_NUM, config);
    bool subtract = _FLD2BOOL(CY_ISOUART_IF_MULTI_READ_CFG_B_SUBTRACT, config);

    return subtract ? num + 1U : num;
}

/*******************************************************************************
* Function Name: Cy_ISOUART_NodeGetInterfaceConfig
****************************************************************************//**
*
* \brief
* Get node interface configuration.
* \param base
* The base address for the iso UART.
* \param config
* Pointer to a configuration structure to be filled. See \ref cy_stc_isouart_node_interface_config_t
* \note
* This function is valid only for Node.
*
*******************************************************************************/
void Cy_ISOUART_NodeGetInterfaceConfig(const ISOUART_Type* base, cy_stc_isouart_node_interface_config_t* config)
{
    CY_ASSERT_L1(NULL != base);
    CY_ASSERT_L1(NULL != config);

    uint32_t reg = base->IF_CONFIG;

    config->nodeId = (uint8_t)_FLD2VAL(ISOUART_IF_CONFIG_NODE_ID, reg);
    config->blockId = (uint8_t)_FLD2VAL(ISOUART_IF_CONFIG_BLOCK_ID, reg);
    config->isFinalNode = _FLD2BOOL(ISOUART_IF_CONFIG_FN, reg);
}

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

#endif /* (CY_IP_M0S8ISOUART) */

/* [] END OF FILE */
