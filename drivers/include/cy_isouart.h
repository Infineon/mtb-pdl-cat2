/***************************************************************************//**
* \file cy_isouart.h
* \version 1.0
*
* \brief
* Provides an API declaration of the Isolated UART driver
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

/**
* \addtogroup group_isouart
* \{
*
* The iso UART is a point-to-point differential current edge-triggered UART interface for communication with the other
* battery management system (BMS) ICs.
*
* \note Supported on PSOC&trade; 4 HVPA SPM only.
*
* The functions and other declarations used in this driver are in cy_isouart.h.
* You can include cy_pdl.h to get access to all functions and declarations in the PDL.
*
* \section group_isouart_overview Overview
*
* The iso UART can be configured to operate in different modes: Master (Host) or Slave (Node).
* This protocol consists of a single master device and multiple slave devices. The master device can address the slave devices using
* node-id and block id.
* It provides galvanic isolation with UART-style frames (start bit, stop bit, 8 data bits, zero parity bits),
* and has high robustness against external noise.
*
* \subsection group_isouart_interfaces Physical Interfaces
*
* The iso UART communicates via two physical links: high side interface (IFH) and low side interface (IFL).
* Both high and low side interfaces can drive the differential lines (TX) and both have a differential receiver
* circuit (RX). This offers the necessary isolation to guarantee error-free communication between different
* modules in the battery system. The interface allowed to drive the bus lines is determined by the bus timing protocol.
*
* \subsection group_isouart_topologies Topologies
*
* The protocol supports up to 30 devices in a single chain using device IDs. This can be extended to more than 30
* devices using Block ID. Block ID can also be used to group devices and limit the number of devices responding to
* broadcast messages. There are two possible topologies: Master-on-Bottom (MOB) and Master-on-Top (MOT).
* These topologies can also be optionally wired as complete loops (Ring).
*
* \image html isouart_topologies.png "MOT, MOB and Ring topology" width=600px
*
* **Master-on-Top (MOT):**
* - Host positioned at the top of the chain
* - Host communicates via Low interface
* - Data stored in SRAM Low interface registers
*
* **Master-on-Bottom (MOB):**
* - Host positioned at the bottom of the chain
* - Host communicates via High interface
* - Data stored in SRAM High interface registers
*
* **Ring Topologies (MOT-Ring, MOB-Ring):**
* - Complete physical loops connecting the chain endpoints
* - Host receives node replies on both interfaces
* - Host's own transmitted commands received on the opposite interface
*
* Both high side (IFH) and low side (IFL) interfaces can act as transmitters and receivers. Nodes automatically detect
* the wake signal source and configure their internal multiplexer to receive commands from the appropriate interface
* while simultaneously forwarding frames through the chain via the opposite interface. When transmitting a response,
* the slave transmits on both High and Low interfaces simultaneously.
*
* \subsection group_isouart_commands Command Types
*
* The iso UART protocol supports six command types for Host-Node communication, with two main categories:
* write and read requests that differ primarily in Address/ID field content. All frames are hardware-interpreted
* and use 8-bit data with full CRC protection (no parity). The protocol tolerates frequency mismatch between
* Host and Node transmissions and supports broadcast operations.
*
* \image html isouart_commands.png "Possible Master to Slave communication commands" width=800px
*
* **Write Commands:** \n
* The Host can write data to Node registers or SRAM locations using several write command types: \n
* - **Write**: Writes 16-bit data to a specific address of one Node.
* - **Broadcast Write**: Writes 16-bit data to the same address of all Nodes in the chain simultaneously.
*
* When executing write commands, Nodes return Write Reply messages containing operation status information
* (CRC, status flags, error bits) stored in HIGH_INTERFACE_READ or LOW_INTERFACE_READ registers.
* Standard Write commands receive a response from the addressed Node, while Broadcast Write
* commands receive a response from the final Node in the chain. It is essential for broadcast write mode that the final Node in the chain
* must be properly marked with FN=1 (Final Node flag) during the enumeration process. If no Node has FN=1 set, Broadcast Write
* commands will not receive any Write Reply frame, resulting in a FRAME ERROR. This configuration is
* essential for proper broadcast communication functionality.
*
* **Read Commands:** \n
* The Host can read data from Node registers or SRAM locations using several read command types: \n
* - **Read**: Reads 16-bit data from a specific address of one Node.
* - **Broadcast Read**: Reads 16-bit data from the same address of all Nodes sequentially.
*   Nodes transmit sequentially starting from Node 1, with each Node switching to transmit mode while others monitor
*   the reply frames for synchronization. After each transmission completes, the next Node takes its turn.
* - **Multi-Read**: Reads multiple registers from one Node using pre-configured address ranges.
*   The multi-read configuration must be set via broadcast write before use.
* - **Broadcast Multi-Read**: Combines broadcast and multi-read functionality to read multiple registers
*   from all Nodes sequentially.
*
* **Two non-data notifications commands:**
* - **Wake-up Sequence**: Required before any command frame to wake all Nodes from sleep mode. Automatically sent
*   by Host on first enable or after reset/re-enable. For subsequent commands, wake sequence can be manually
*   triggered via software when needed.
* - **Emergency Mode (EMM)**: Alternating signal used only by Nodes for fault handling. Can be triggered via SW or external
*   trigger inputs. EMM also wakes devices from sleep and is automatically forwarded through the chain.
*
* \subsection group_isouart_sram SRAM
*
* The iso UART block contains the 128 x 32-bit SRAM, which serves as the primary memory for data storage and exchange.
* It is the only interface between the iso UART peripheral and the rest of the chip via CPU or DMA.
* The SRAM defaults to an unexpected state and must be initialized and divided before iso UART block is enabled.
*
* **SRAM Word Structure (32 bits):**
* - **Bits [15:0]**: Data field - stores the actual 16-bit payload data.
* - **Bits [24:16]**: Attributes field (9 bits) - stores control and status information.
* - **Bits [31:25]**: ECC field (7 bits) - stores error correction code for data integrity.
*
* The 128-word SRAM can be divided into two parts Low and High, with each part being dedicated to store data received
* from corresponding interface in certain receive modes.
*
* The iso UART provides three distinct receiver modes that determine how incoming frame data is processed and stored in SRAM:
* - **Register Mode**: Reply frames stored only in receive registers, no SRAM usage.
* - **Unpacked Mode**: Data field (16 bits) extracted and stored in SRAM using one row.
* - **Raw Mode**: Entire frame (ID, address, data, CRC) stored in SRAM using three rows per frame.
*
* In Unpacked and Raw modes, data is written to both SRAM and interface registers, with optional ECC calculation for enhanced data integrity protection.
* Register mode writes data only to interface registers without using SRAM.
*
* \section group_isouart_configuration Configuration Considerations
*
* \subsection group_isouart_config_device_configurator Device Configurator Setup
* The ModusToolbox Device Configurator provides a graphical user interface to configure the iso UART peripheral.
* This tool automatically generates initialization code and configuration structures based on your settings.
*
* To use the Device Configurator:
* 1. Open the ModusToolbox Device Configurator from your project.
* 2. Navigate to the "Peripherals" tab and under "Communication" group enable "Iso UART" block.
* 3. Choose the operation mode (Host or Node).
* 4. Configure basic parameters like topology, bit rate, and CRC policy.
* 5. Save the configuration to generate the initialization code.
*
* After Device Configurator is set up, the initialization structures, clocks, peripheral clock dividers, GPIOs,
* and other peripheral configuration code are auto-generated and placed in ../bsps/TARGET_APP_.../config directory files. \n
* Place the call of the cybsp_init() function before using any iso UART API functions
* to ensure initialization of all external resources required for the iso UART operation.
*
* \subsection group_isouart_config_host Host Configuration
*
* To set up the iso UART driver in the Host mode, provide the configuration parameters in the \ref cy_stc_isouart_host_config_t structure.
* Host configuration requires careful consideration of topology, receiver mode, CRC policy, block ID usage, and multi-read settings
* to match those configured on the nodes in the physical chain setup. To initialize the driver, call the \ref Cy_ISOUART_HostInit function
* providing a pointer to the populated \ref cy_stc_isouart_host_config_t structure.
* \snippet isouart_snippet.c snippet_Cy_ISOUART_ConfigHost
*
* \subsection group_isouart_config_node Node Configuration
* To set up the iso UART driver in the Node mode, provide the configuration parameters in the \ref cy_stc_isouart_node_config_t structure.
* Node configuration must be compatible with the host configuration, especially regarding CRC policy, Block ID usage, and multi-read settings.
* To initialize the driver, call \ref Cy_ISOUART_NodeInit function providing a pointer to the populated \ref cy_stc_isouart_node_config_t structure.
* \snippet isouart_snippet.c snippet_Cy_ISOUART_ConfigNode
*
* \subsection group_isouart_config_clock Clock Configuration
* The iso UART block requires proper configuration of high frequency clock (HFCLK) and peripheral dividers to operate at 24 MHz.
* This clock is configured using the \ref group_sysclk driver API.
* \snippet isouart_snippet.c snippet_Cy_ISOUART_ClockConfig
*
* \subsection group_isouart_config_interrupt Interrupt Configuration
* The iso UART can use interrupts to handle communication events. Configuration flow includes setting up the interrupt
* service routine (ISR) and enabling the appropriate interrupt sources.
* \snippet isouart_snippet.c snippet_Cy_ISOUART_IntConfig
*
* \subsection group_isouart_config_enable Enable Iso UART
* The iso UART must be enabled to operate. This includes enabling the peripheral and global interrupts.
* \snippet isouart_snippet.c snippet_Cy_ISOUART_EnableIsoUART
*
* \subsection group_isouart_config_multiread Multi-Read Configuration
* The multi-read feature allows reading multiple registers from a Node using pre-configured address ranges.
* To use this feature, the multi-read configuration must be set via broadcast write before use.
* \snippet isouart_snippet.c snippet_Cy_ISOUART_ConfigMultiRead
*
* \section group_isouart_common_use_cases Common Use Cases
*
* This section demonstrates a typical iso UART Host and Node implementation using the following configuration:
* - **Topology**: Master-on-Top
* - **Receiver Mode**: Unpacked
* - **Operation**: Sequenced mode
*
* \subsection group_isouart_host_configuration Host Use Case
* This section demonstrates how to configure a Host to communicate with Nodes in the chain.
* The Host can be configured using either auto-generated code from \ref group_isouart_config_device_configurator
* or manual configuration as described in \ref group_isouart_config_host, \ref group_isouart_config_clock,
* \ref group_isouart_config_interrupt, and \ref group_isouart_config_enable sections.
*
* **Enumeration Process** \n
* Before starting communication, the Host must enumerate each Node in the chain to assign unique IDs.
* This process is required at a startup and after a wake-up from Sleep mode, as Node-IDs are reset to 0.
* The Host sequentially assigns IDs by sending write commands to Device ID 0, starting from 1.
* Every enumerated Node starts forwarding messages which allow access to the next Node with ID 0.
* This continues until all Nodes receive consecutive IDs, with the final Node marked appropriately:
* \snippet isouart_snippet.c snippet_Cy_ISOUART_EnumerateNodes
*
* **Write Operations** \n
* The Host writes data to Node registers or SRAM using \ref Cy_ISOUART_HostSendWriteCommand. This function supports both individual
* Node writes and broadcast writes to all Nodes simultaneously (for broadcast mode, set the nodeId parameter to 63).
*
* After every write operation, Nodes return Write Reply frames containing operation status information such as CRC validation,
* error flags, and operation status. These replies are stored in HIGH_INTERFACE_READ or LOW_INTERFACE_READ registers.
* If any error bits are set in the reply (status, access, or address errors), the Host triggers the
* corresponding interrupts: \ref CY_ISOUART_INTR_MASTER_SLAVE_STATUS_ERROR, \ref CY_ISOUART_INTR_MASTER_SLAVE_ACCESS_ERROR,
* or \ref CY_ISOUART_INTR_MASTER_SLAVE_ADDRESS_ERROR. Additionally, if the CRC doesn't match the expected value, the Host triggers
* \ref CY_ISOUART_INTR_MASTER_CRC_ERROR interrupt.
*
* To process the write reply, there are two approaches for CRC validation. You can enable the
* \ref CY_ISOUART_INTR_MASTER_CRC_ERROR interrupt or, alternatively, you can use manual validation
* by reading the reply data with \ref Cy_ISOUART_HostReadLowInterface or \ref Cy_ISOUART_HostReadHighInterface
* (depending on your topology configuration) and then passing this value to \ref Cy_ISOUART_HostVerifyCRC
* to validate the CRC value.
* \snippet isouart_snippet.c snippet_Cy_ISOUART_HostWrite
*
* **Read Operations** \n
* The Host can read data from Node registers or SRAM locations using \ref Cy_ISOUART_HostSendReadCommand.
* Received data is stored in SRAM and can be retrieved using appropriate API functions based on the
* configured receiver mode: \ref Cy_ISOUART_HostReadSramUnpack for Unpacked mode, \ref Cy_ISOUART_HostReadSramRaw for Raw mode
* (returns complete packet with addr, nodeID, data, CRC fields, or use \ref Cy_ISOUART_ReadSramRow16 / \ref Cy_ISOUART_ReadSramRow32
* for direct data field access), or \ref Cy_ISOUART_HostReadLowInterface / \ref Cy_ISOUART_HostReadHighInterface for Register mode:
*
* \snippet isouart_snippet.c snippet_Cy_ISOUART_ReadNode
*
* \subsection group_isouart_node_configuration Node Use Case
* This section demonstrates how to configure a Node to receive and process commands from the Host.
* The Node can be configured using either auto-generated code from \ref group_isouart_config_device_configurator
* or manual configuration as described in \ref group_isouart_config_node, \ref group_isouart_config_clock,
* \ref group_isouart_config_interrupt, and \ref group_isouart_config_enable sections.
*
* **Trigger Interrupts Structures and Attributes Configuration** \n
*
* To properly handle Host commands, the Node must be configured to monitor-specific SRAM addresses
* and trigger actions based on received data. This requires configuring interrupt trigger structures and
* linking them to specific SRAM row attributes:
* \snippet isouart_snippet.c snippet_Cy_ISOUART_NodeIntrTrigStructConfig
* \note
* After configuring the trigger structure, ensure that:
* 1. Primary Access and other relevant interrupts are enabled in the interrupt mask.
* 2. Interrupt handler is properly configured (see next snippet).
* 3. Global interrupts are enabled before expecting interrupt triggers.
*
* **Implementing the Interrupt Handler** \n
*
* The Node's interrupt handler processes incoming write commands from the Host and executes appropriate responses.
* Note that proper interrupt configuration (as described in \ref group_isouart_config_interrupt) must be completed
* before this handler can function correctly:
* \snippet isouart_snippet.c snippet_Cy_ISOUART_NodeInterruptHandler
*
* **Maintaining Node Activity** \n
*
* The Node contains a 7-bit watchdog counter that counts downward from its maximum value and must be serviced
* regularly to prevent the device from entering Sleep mode. The watchdog operates with a 16ms resolution on a 24MHz clock,
* and so provide a maximum timeout of 2.03 seconds. This watchdog timer must be refreshed periodically
* before it reaches zero using \ref Cy_ISOUART_NodeSetWdtCount on the Node or using \ref Cy_ISOUART_HostSendWriteCommand on the Host side.
*
* Refreshing watchdog counter from Host:
* \snippet isouart_snippet.c snippet_Cy_ISOUART_HostWatchdogRefresh
* Refreshing watchdog counter on the Node:
* \snippet isouart_snippet.c snippet_Cy_ISOUART_NodeWatchdogRefresh
*
* \subsection group_isouart_sram_protection SRAM Address Protection
* The iso UART provides mechanisms to protect SRAM addresses on Nodes using \ref Cy_ISOUART_NodeConfigSramRowAttr.
* Two protection modes are available: \n
*
* **Invalid Address Protection** \n
* When a Node marks a SRAM address as invalid using \ref Cy_ISOUART_NodeConfigSramRowAttr, any Host read or write attempt
* to that address will be rejected. For write commands, the Node responds with the address error bit set in the reply frame,
* triggering the \ref CY_ISOUART_INTR_MASTER_SLAVE_ADDRESS_ERROR interrupt on the Host side.
*
* Node-side configuration:
* \snippet isouart_snippet.c snippet_Cy_ISOUART_InvalidAddressNode
*
* Host-side error handling:
* \snippet isouart_snippet.c snippet_Cy_ISOUART_InvalidAddressHost
*
* \note Interrupt \ref CY_ISOUART_INTR_MASTER_SLAVE_ADDRESS_ERROR must be enabled in the interrupt mask on the Host.
*
* **Read-Only Protection** \n
* When a Node marks a SRAM address as read-only using \ref Cy_ISOUART_NodeConfigSramRowAttr, write attempts from the Host
* will be rejected while read operations remain allowed. The Node responds with the access error bit set in the reply frame,
* triggering the \ref CY_ISOUART_INTR_MASTER_SLAVE_ACCESS_ERROR interrupt on the Host side.
*
* Node-side configuration:
* \snippet isouart_snippet.c snippet_Cy_ISOUART_ReadOnlyAccessNode
*
* Host-side error handling:
* \snippet isouart_snippet.c snippet_Cy_ISOUART_ReadOnlyAccessHost
*
* \note Interrupt \ref CY_ISOUART_INTR_MASTER_SLAVE_ACCESS_ERROR must be enabled in the interrupt mask on the Host.
*
* \section group_isouart_more_information More Information.
* See the Iso UART chapter of the device technical reference manual (TRM).
*
* \section group_isouart_changelog Changelog
*
* <table class="doxtable">
*   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
*   <tr>
*     <td>1.0</td>
*     <td>The initial version.</td>
*     <td></td>
*   </tr>
* </table>
*
* \defgroup group_isouart_macros Macros
*   \{
*       \defgroup group_isouart_macros_interrupt_masks Interrupt Masks
*       \defgroup group_isouart_macros_reserved_addresses Reserved Addresses
*   \}
* \defgroup group_isouart_functions Functions
*   \{
*       \defgroup group_isouart_common_functions Common Functions
*       \defgroup group_isouart_host_functions Host Functions
*       \defgroup group_isouart_node_functions Node Functions
*       \defgroup group_isouart_sram_functions SRAM Functions
*       \defgroup group_isouart_interrupt_functions Interrupt Functions
*   \}
* \defgroup group_isouart_data_structures Data Structures
* \defgroup group_isouart_enums Enumerated Types
*/


#if !defined(CY_ISOUART_H)
#define CY_ISOUART_H

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/
#include "cy_device.h"
#include "cy_syslib.h"
#include <stdint.h>
#include <stdbool.h>

#if defined(CY_IP_M0S8ISOUART)

#if defined(__cplusplus)
extern "C" {
#endif


/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define')                           */
/*****************************************************************************/
/**
* \addtogroup group_isouart_macros
* \{
*/

/** Driver major version */
#define CY_ISOUART_DRV_VERSION_MAJOR    1

/** Driver minor version */
#define CY_ISOUART_DRV_VERSION_MINOR    0

/** Iso UART driver ID */
#define CY_ISOUART_ID CY_PDL_DRV_ID(0x52U)

/** \} group_isouart_macros */


/** \addtogroup group_isouart_macros_interrupt_masks
* \{
*/

#define CY_ISOUART_INTR_WAKE_HIGH                    (ISOUART_INTR_WAKE_HIGH_Msk)                     /**< Wakeup Event on High Interface */
#define CY_ISOUART_INTR_WAKE_LOW                     (ISOUART_INTR_WAKE_LOW_Msk)                      /**< Wakeup Event on Low Interface */
#define CY_ISOUART_INTR_WDOG_CNT_TIMEOUT             (ISOUART_INTR_WDOG_CNT_TIMEOUT_Msk)              /**< Timeout of the watchdog counter, indicating that the interface is going to sleep mode */
#define CY_ISOUART_INTR_EMM                          (ISOUART_INTR_EMM_Msk)                           /**< The Emergency signal was transmitted by another part in the chain */
#define CY_ISOUART_INTR_REQUEST                      (ISOUART_INTR_REQUEST_Msk)                       /**< The host completed a request, with clean CRC (whether or not it was addressed to us) */
#define CY_ISOUART_INTR_REQUEST_MATCH                (ISOUART_INTR_REQUEST_MATCH_Msk)                 /**< The host completed a request, with device ID matching ours, with clean CRC */
#define CY_ISOUART_INTR_BC_WRITE                     (ISOUART_INTR_BC_WRITE_Msk)                      /**< The host completed a BC_WRITE request */
#define CY_ISOUART_INTR_BC_READ                      (ISOUART_INTR_BC_READ_Msk)                       /**< The host completed a BC_READ request */
#define CY_ISOUART_INTR_FRAME_ERR                    (ISOUART_INTR_FRAME_ERR_Msk)                     /**< Framing communication error */
#define CY_ISOUART_INTR_CRC_ERR                      (ISOUART_INTR_CRC_ERR_Msk)                       /**< CRC error */
#define CY_ISOUART_INTR_REG_ACCESS_P                 (ISOUART_INTR_REG_ACCESS_P_Msk)                  /**< Primary register specific access interrupt */
#define CY_ISOUART_INTR_REG_ACCESS_S                 (ISOUART_INTR_REG_ACCESS_S_Msk)                  /**< Secondary register specific access interrupt */
#define CY_ISOUART_INTR_UVD                          (ISOUART_INTR_UVD_Msk)                           /**< Under Voltage Detection  */
#define CY_ISOUART_INTR_ECC_ERR                      (ISOUART_INTR_ECC_ERR_Msk)                       /**< ECC Error */
#define CY_ISOUART_INTR_WRITE_COMPLETE               (ISOUART_INTR_WRITE_COMPLETE_Msk)                /**< A master transaction completed */
#if (0U != ISOUART_MASTER)
#define CY_ISOUART_INTR_MASTER_LOW_FRAME_RECEIVED    (ISOUART_INTR_MASTER_LOW_FRAME_RECEIVED_Msk)     /**< A frame was received at the LOW interface while not in SLAVE mode (Master mode only) */
#define CY_ISOUART_INTR_MASTER_HIGH_FRAME_RECEIVED   (ISOUART_INTR_MASTER_HIGH_FRAME_RECEIVED_Msk)    /**< A frame was received at the HIGH interface while not in SLAVE mode (Master mode only) */
#define CY_ISOUART_INTR_MASTER_LOW_SRAM_FULL         (ISOUART_INTR_MASTER_LOW_SRAM_FULL_Msk)          /**< The LOW interface filled the allocated local SRAM (Master mode only) */
#define CY_ISOUART_INTR_MASTER_HIGH_SRAM_FULL        (ISOUART_INTR_MASTER_HIGH_SRAM_FULL_Msk)         /**< The HIGH interface filled the allocated local SRAM (Master mode only) */
#define CY_ISOUART_INTR_MASTER_INTERNAL_ERROR        (ISOUART_INTR_MASTER_INTERNAL_ERROR_Msk)         /**< An internal error such as configuration problem occurred (Master mode only) */
#define CY_ISOUART_INTR_MASTER_CRC_ERROR             (ISOUART_INTR_MASTER_CRC_ERROR_Msk)              /**< A CRC value from slave didn't match expected value (Master mode only) */
#define CY_ISOUART_INTR_MASTER_SLAVE_STATUS_ERROR    (ISOUART_INTR_MASTER_SLAVE_STATUS_ERROR_Msk)     /**< The slave returned a reply frame with status error bit set (Master mode only) */
#define CY_ISOUART_INTR_MASTER_SLAVE_ACCESS_ERROR    (ISOUART_INTR_MASTER_SLAVE_ACCESS_ERROR_Msk)     /**< The slave returned a reply frame with access error bit set (Master mode only) */
#define CY_ISOUART_INTR_MASTER_SLAVE_ADDRESS_ERROR   (ISOUART_INTR_MASTER_SLAVE_ADDRESS_ERROR_Msk)    /**< The slave returned a reply frame with address error bit set (Master mode only) */

/** Master-specific interrupt masks combined */
#define CY_ISOUART_INTR_MASTER_ALL                   (CY_ISOUART_INTR_MASTER_LOW_FRAME_RECEIVED  | \
                                                      CY_ISOUART_INTR_MASTER_HIGH_FRAME_RECEIVED | \
                                                      CY_ISOUART_INTR_MASTER_LOW_SRAM_FULL       | \
                                                      CY_ISOUART_INTR_MASTER_HIGH_SRAM_FULL      | \
                                                      CY_ISOUART_INTR_MASTER_INTERNAL_ERROR      | \
                                                      CY_ISOUART_INTR_MASTER_CRC_ERROR           | \
                                                      CY_ISOUART_INTR_MASTER_SLAVE_STATUS_ERROR  | \
                                                      CY_ISOUART_INTR_MASTER_SLAVE_ACCESS_ERROR  | \
                                                      CY_ISOUART_INTR_MASTER_SLAVE_ADDRESS_ERROR)
#else
/** Master-specific interrupts not available in node-only configuration */
#define CY_ISOUART_INTR_MASTER_ALL                   (0U)
#endif /* (0U != ISOUART_MASTER) */

/** Combined iso UART interrupt mask */
#define CY_ISOUART_INTR_MASK_ALL                     (CY_ISOUART_INTR_WAKE_HIGH                  | \
                                                      CY_ISOUART_INTR_WAKE_LOW                   | \
                                                      CY_ISOUART_INTR_WDOG_CNT_TIMEOUT           | \
                                                      CY_ISOUART_INTR_EMM                        | \
                                                      CY_ISOUART_INTR_REQUEST                    | \
                                                      CY_ISOUART_INTR_REQUEST_MATCH              | \
                                                      CY_ISOUART_INTR_BC_WRITE                   | \
                                                      CY_ISOUART_INTR_BC_READ                    | \
                                                      CY_ISOUART_INTR_FRAME_ERR                  | \
                                                      CY_ISOUART_INTR_CRC_ERR                    | \
                                                      CY_ISOUART_INTR_REG_ACCESS_P               | \
                                                      CY_ISOUART_INTR_REG_ACCESS_S               | \
                                                      CY_ISOUART_INTR_UVD                        | \
                                                      CY_ISOUART_INTR_ECC_ERR                    | \
                                                      CY_ISOUART_INTR_WRITE_COMPLETE             | \
                                                      CY_ISOUART_INTR_MASTER_ALL)
/** \} group_isouart_macros_interrupt_masks */

/** \addtogroup group_isouart_macros_reserved_addresses
* \{
*/
#define CY_ISOUART_ADDR_GEN_DIAG                     (0x0BU)   /**< General diagnostic/status register */
#define CY_ISOUART_ADDR_MULTI_READ                   (0x31U)   /**< Multi-read data window (current read slot) */
#define CY_ISOUART_ADDR_MULTI_READ_CFG               (0x32U)   /**< Multi-read configuration (counts / bases) */
#define CY_ISOUART_ADDR_CONFIG                       (0x36U)   /**< Static configuration/status register */
#define CY_ISOUART_ADDR_WDOG_CNT                     (0x3DU)   /**< Watchdog counter register */
/** \} group_isouart_macros_reserved_addresses */

/** \cond INTERNAL */
/* Max value for the glitch filter counter value for the under voltage detection (UVD) */
#define CY_ISOUART_UVD_MAX_FILTER_LIMIT              (0x4B0UL)

/* Macros for conditions used in CY_ASSERT calls */
#define CY_ISOUART_INTRMASK_VALID(mask)              (0UL == ((mask) & ~CY_ISOUART_INTR_MASK_ALL))
#define CY_ISOUART_IS_INDEX_EVEN(index)              (0U == (index & 0x01U))
#define CY_ISOUART_IS_BLOCK_ID_ENABLED(base)         (0U != _FLD2VAL(ISOUART_CTL_BLOCK_ID_EN, base->CTL))
#define CY_ISOUART_IS_RECEIVER_MODE_RAW(base)        (_FLD2VAL(ISOUART_MASTER_CFG_RECEIVE_MODE, base->MASTER_CFG) == CY_ISOUART_RECEIVER_MODE_RAW)
#define CY_ISOUART_IS_RECEIVER_MODE_UNPACKED(base)   (_FLD2VAL(ISOUART_MASTER_CFG_RECEIVE_MODE, base->MASTER_CFG) == CY_ISOUART_RECEIVER_MODE_UNPACKED)
#define CY_ISOUART_IS_TOPOLOGY_MOT(base)             ((_FLD2VAL(ISOUART_MASTER_CFG_HIGH_INTERFACE_MODE, base->MASTER_CFG) == CY_ISOUART_INTERFACE_MODE_NODE) && \
                                                      (_FLD2VAL(ISOUART_MASTER_CFG_LOW_INTERFACE_MODE, base->MASTER_CFG) == CY_ISOUART_INTERFACE_MODE_HOST))
#define CY_ISOUART_IS_TOPOLOGY_MOB(base)             ((_FLD2VAL(ISOUART_MASTER_CFG_HIGH_INTERFACE_MODE, base->MASTER_CFG) == CY_ISOUART_INTERFACE_MODE_HOST) && \
                                                      (_FLD2VAL(ISOUART_MASTER_CFG_LOW_INTERFACE_MODE, base->MASTER_CFG) == CY_ISOUART_INTERFACE_MODE_NODE))
#define CY_ISOUART_IS_TOPOLOGY_RING(base)            ((_FLD2VAL(ISOUART_MASTER_CFG_HIGH_INTERFACE_MODE, base->MASTER_CFG) == CY_ISOUART_INTERFACE_MODE_RECEIVER) || \
                                                      (_FLD2VAL(ISOUART_MASTER_CFG_LOW_INTERFACE_MODE, base->MASTER_CFG) == CY_ISOUART_INTERFACE_MODE_RECEIVER))
#define CY_ISOUART_RAW_MODE_SRAM_SIZE_VALID(base, highSramSize, lowSramSize) \
                                                     (!CY_ISOUART_IS_RECEIVER_MODE_RAW(base) || \
                                                      ((((highSramSize) % CY_ISOUART_NUM_OF_ROW_IN_RAW_MODE) == 0U) && \
                                                       (((lowSramSize) % CY_ISOUART_NUM_OF_ROW_IN_RAW_MODE) == 0U)))
#define CY_ISOUART_MOT_TOPOLOGY_SRAM_SIZE_VALID(base, lowSramSize) \
                                                     (!CY_ISOUART_IS_TOPOLOGY_MOT(base) || (lowSramSize != 0U))
#define CY_ISOUART_MOB_TOPOLOGY_SRAM_SIZE_VALID(base, highSramSize, lowSramSize) \
                                                     (!CY_ISOUART_IS_TOPOLOGY_MOB(base) || \
                                                      ((highSramSize != 0U) && (lowSramSize == 0U)))
#define CY_ISOUART_RING_TOPOLOGY_SRAM_SIZE_VALID(base, highSramSize, lowSramSize) \
                                                     (!CY_ISOUART_IS_TOPOLOGY_RING(base) || \
                                                      ((highSramSize != 0U) && (lowSramSize != 0U)))
#define CY_ISOUART_GET_INTERFACE_SRAM_SIZE(base, iSide) ((iSide == CY_ISOUART_IF_LOW) ? \
                                                         _FLD2VAL(ISOUART_MASTER_CFG_LOW_INTERFACE_SRAM_SIZE, base->MASTER_CFG) : \
                                                         _FLD2VAL(ISOUART_MASTER_CFG_HIGH_INTERFACE_SRAM_SIZE, base->MASTER_CFG))
#define CY_ISOUART_RAW_MODE_PACKET_NUM_VALID(base, iSide, packetNum) (((packetNum + 1U) * CY_ISOUART_NUM_OF_ROW_IN_RAW_MODE) <= \
                                                                       CY_ISOUART_GET_INTERFACE_SRAM_SIZE(base, iSide))
#define CY_ISOUART_UNPACKED_MODE_PACKET_NUM_VALID(base, iSide, packetNum) ((packetNum + CY_ISOUART_NUM_OF_ROW_IN_UNPACKED_MODE) <= \
                                                                            CY_ISOUART_GET_INTERFACE_SRAM_SIZE(base, iSide))
#define CY_ISOUART_INTERFACE_MODE_IS_NODE(base, iSide) ((iSide == CY_ISOUART_IF_LOW) ? \
                                                        (_FLD2VAL(ISOUART_MASTER_CFG_LOW_INTERFACE_MODE, base->MASTER_CFG) == CY_ISOUART_INTERFACE_MODE_NODE) : \
                                                        (_FLD2VAL(ISOUART_MASTER_CFG_HIGH_INTERFACE_MODE, base->MASTER_CFG) == CY_ISOUART_INTERFACE_MODE_NODE))
#define CY_ISOUART_BLOCK_ID_VALID(base, blockId)     (CY_ISOUART_MAX_BLOCK_ID >= blockId || !CY_ISOUART_IS_BLOCK_ID_ENABLED(base))
#define CY_ISOUART_NODE_ID_VALID(nodeId)       (CY_ISOUART_MAX_NODE_ID > nodeId) || (nodeId == CY_ISOUART_BROADCAST_NODE_ID)

/* Field definitions for multi-read configuration register */
#define CY_ISOUART_IF_MULTI_READ_CFG_A_READ_NUM_Pos     (0UL)       /**< Base address A - number of reads field position */
#define CY_ISOUART_IF_MULTI_READ_CFG_A_READ_NUM_Msk     (0xFUL)     /**< Base address A - number of reads field mask */
#define CY_ISOUART_IF_MULTI_READ_CFG_A_SUBTRACT_Pos     (4UL)       /**< Base address A - subtract field position */
#define CY_ISOUART_IF_MULTI_READ_CFG_A_SUBTRACT_Msk     (0x10UL)    /**< Base address A - subtract field mask */

#define CY_ISOUART_IF_MULTI_READ_CFG_B_READ_NUM_Pos     (5UL)       /**< Base address B - number of reads field position */
#define CY_ISOUART_IF_MULTI_READ_CFG_B_READ_NUM_Msk     (0xE0UL)    /**< Base address B - number of reads field mask */
#define CY_ISOUART_IF_MULTI_READ_CFG_B_SUBTRACT_Pos     (8UL)       /**< Base address B - subtract field position */
#define CY_ISOUART_IF_MULTI_READ_CFG_B_SUBTRACT_Msk     (0x100UL)   /**< Base address B - subtract field mask */
/** \endcond */

/*****************************************************************************/
/* Enumerations                                                              */
/*****************************************************************************/
/** \addtogroup group_isouart_enums
* \{
*/

/** Iso UART event types */
typedef enum
{
    CY_ISOUART_EVENT_NONE       = 0U, /**< No trigger (interrupt) */
    CY_ISOUART_EVENT_WRITE      = 1U, /**< Trigger (interrupt) on a write event */
    CY_ISOUART_EVENT_EARLY_READ = 2U, /**< Trigger (interrupt) on an early read event */
    CY_ISOUART_EVENT_LATE_READ  = 3U  /**< Trigger (interrupt) on a late read event */
} cy_en_isouart_event_t;

/** Interface side to be read. */
typedef enum
{
    CY_ISOUART_IF_LOW  = 0U, /**< Low Interface */
    CY_ISOUART_IF_HIGH = 1U, /**< High Interface */
} cy_en_isouart_interface_side_t;

/** Configure the CRC Polynomial. */
typedef enum
{
    CY_ISOUART_CRC8_SAE_J1850 = 0U, /**< CRC8 SAE J1850: z^8 + z^4 + z^3 + z^2 + 1 */
    CY_ISOUART_CRC8_H2F       = 1U, /**< CRC8 H2F 0x2F polynomial: x^8 + x^5 + x^3 + x^2 + x + 1 */
} cy_en_isouart_crc_policy_t;

/** Configure the Topology of iso UART chain. */
typedef enum
{
    CY_ISOUART_MOT      = 0U, /**< Host On Top */
    CY_ISOUART_MOB      = 1U, /**< Host On Bottom */
    CY_ISOUART_MOT_RING = 2U, /**< Host On Top with Ring configuration */
    CY_ISOUART_MOB_RING = 3U, /**< Host On Bottom with Ring configuration */
} cy_en_isouart_chain_topology_t;

/** Configure the data rate of the iso UART chain. */
typedef enum
{
    CY_ISOUART_HOST_BITRATE_2_1MBPS = 0U, /**< 2.1 Mbps */
    CY_ISOUART_HOST_BITRATE_3_0MBPS = 1U, /**< 3.0 Mbps */
} cy_en_isouart_host_bitrate_t;

/** Configure how received data is processed. */
typedef enum
{
    CY_ISOUART_RECEIVER_MODE_REGISTER = 0U, /**< Register mode - Stored in respective RECEIVED register only. */
    CY_ISOUART_RECEIVER_MODE_UNPACKED = 1U, /**< Unpacked mode - Unpacked, with Data field being stored in local SRAM.  */
    CY_ISOUART_RECEIVER_MODE_RAW      = 2U, /**< Raw mode - Raw register values stored in local SRAM.
                                             * This allows for end (slave) to end (ASIL-D host) data integrity to be maintained.
                                             */
} cy_en_isouart_receiver_mode_t;

/** High-level interface state of the iso UART */
typedef enum
{
    CY_ISOUART_INTERFACE_STATE_SLEEP      = 0U, /**< Interface in SLEEP (low power, waiting for a wake sequence). */
    CY_ISOUART_INTERFACE_STATE_INIT       = 1U, /**< INIT: Wake detected, interface initializing. */
    CY_ISOUART_INTERFACE_STATE_IDLE       = 2U, /**< IDLE: Ready, no frame in progress. */
    CY_ISOUART_INTERFACE_STATE_REQUEST    = 3U, /**< REQUEST: Host request (SYNC through CRC of request). */
    CY_ISOUART_INTERFACE_STATE_REVERSE    = 4U, /**< REVERSE: Upstream device transmitting (node replying). */
    CY_ISOUART_INTERFACE_STATE_REPLY      = 5U, /**< REPLY: This device transmitting a reply. */
    CY_ISOUART_INTERFACE_STATE_BC_WAIT    = 6U, /**< BC_WAIT: Waiting for broadcast reply from downstream devices. */
    CY_ISOUART_INTERFACE_STATE_BC_REPLY   = 7U, /**< BC_REPLY: This device sending broadcast reply. */
    CY_ISOUART_INTERFACE_STATE_BC_REVERSE = 8U, /**< BC_REVERSE: Upstream sending broadcast reply. */
    CY_ISOUART_INTERFACE_STATE_EMM        = 9U, /**< EMM: Error management message being transmitted. */
} cy_en_isouart_interface_state_t;

/** Sub-state of a host request sequence.
 * \note Valid only when interface state is \ref CY_ISOUART_INTERFACE_STATE_REQUEST.
 */
typedef enum
{
    CY_ISOUART_REQUEST_STATE_BLOCK_ID  = 0U, /**< Sending SYNC frame / BLOCK ID (if enabled). */
    CY_ISOUART_REQUEST_STATE_DEVICE_ID = 1U, /**< Sending DEVICE ID frame. */
    CY_ISOUART_REQUEST_STATE_ADDR      = 2U, /**< Sending ADDRESS frame. */
    CY_ISOUART_REQUEST_STATE_DATA1     = 3U, /**< Sending DATA1 frame (high byte). */
    CY_ISOUART_REQUEST_STATE_DATA0     = 4U, /**< Sending DATA0 frame (low byte). */
    CY_ISOUART_REQUEST_STATE_CRC       = 5U, /**< Sending CRC frame. */
} cy_en_isouart_request_state_t;

/** Sub-state of a a reply frame sequence.
 * \note Valid only when interface state is one of:
 * \ref CY_ISOUART_INTERFACE_STATE_REPLY,
 * \ref CY_ISOUART_INTERFACE_STATE_BC_WAIT,
 * \ref CY_ISOUART_INTERFACE_STATE_BC_REPLY,
 * \ref CY_ISOUART_INTERFACE_STATE_BC_REVERSE,
 */
typedef enum
{
    CY_ISOUART_REPLY_STATE_REPLY_FRAME = 0U, /**< Sending REPLY frame. */
    CY_ISOUART_REPLY_STATE_ID_FRAME    = 1U, /**< Sending ID frame. */
    CY_ISOUART_REPLY_STATE_ADDR_FRAME  = 2U, /**< Sending ADDRESS frame. */
    CY_ISOUART_REPLY_STATE_DATA1_FRAME = 3U, /**< Sending DATA1 frame (high byte). */
    CY_ISOUART_REPLY_STATE_DATA0_FRAME = 4U, /**< Sending DATA0 frame (low byte). */
    CY_ISOUART_REPLY_STATE_CRC_FRAME   = 5U, /**< Sending CRC frame. */
} cy_en_isouart_reply_state_t;

/** Last host command type observed on the bus.
 * \note Valid when the interface state indicates a transmit phase for a slave:
 * \ref CY_ISOUART_INTERFACE_STATE_REVERSE,
 * \ref CY_ISOUART_INTERFACE_STATE_REPLY,
 * \ref CY_ISOUART_INTERFACE_STATE_BC_WAIT,
 * \ref CY_ISOUART_INTERFACE_STATE_BC_REPLY,
 * \ref CY_ISOUART_INTERFACE_STATE_BC_REVERSE.
 */
typedef enum
{
    CY_ISOUART_HOST_COMMAND_WRITE         = 0U, /**< Write command. */
    CY_ISOUART_HOST_COMMAND_BC_WRITE      = 1U, /**< Broadcast Write command. */
    CY_ISOUART_HOST_COMMAND_READ          = 2U, /**< Read command. */
    CY_ISOUART_HOST_COMMAND_MULTI_READ    = 3U, /**< Multi-Read command. */
    CY_ISOUART_HOST_COMMAND_BC_READ       = 4U, /**< Broadcast Read command. */
    CY_ISOUART_HOST_COMMAND_BC_MULTI_READ = 5U, /**< Broadcast Multi-Read command. */
} cy_en_isouart_host_command_t;

/** ECC error types */
typedef enum
{
    CY_ISOUART_ECC_NO_ERROR        = 0U,  /**< No ECC error. */
    CY_ISOUART_ECC_CORRECTED       = 1U,  /**< Corrected ECC error. */
    CY_ISOUART_ECC_NOT_CORRECTED   = 2U,  /**< Not corrected ECC error. */
} cy_en_isouart_ecc_error_t;

/** Wakeup source types */
typedef enum
{
    CY_ISOUART_WAKEUP_ISOUART = 0U, /**< Wake-up via iso UART */
    CY_ISOUART_WAKEUP_GPIO    = 1U, /**< Wake-up via GPIO */
} cy_en_isouart_wakeup_source_t;
/** \} group_isouart_enums */


/*****************************************************************************/
/* Data structures                                                           */
/*****************************************************************************/
/** \addtogroup group_isouart_data_structures
* \{
*/

#if (0U != ISOUART_MASTER)
/** Configure the iso UART in Host mode. */
typedef struct
{
    cy_en_isouart_chain_topology_t topology;               /**< Topology of iso UART chain */
    cy_en_isouart_host_bitrate_t   hostBitrate;            /**< The data rate of the iso UART chain */
    cy_en_isouart_receiver_mode_t  receiverMode;           /**< Receive mode */
    cy_en_isouart_crc_policy_t     crcPolicy;              /**< CRC Polynomial */
    bool                           enableBlockId;          /**< Whether enable Block ID */
    bool                           useWriteDoneTrig;       /**< Whether use trigger when a write transaction is complete */
    bool                           useLowSramFullTrig;     /**< Whether use trigger when data is full in Low side of SRAM */
    bool                           useHighSramFullTrig;    /**< Whether use trigger when data is full in High side of SRAM */
    bool                           enableMultiReadBC;      /**< Whether enable BC Multi Read */
    bool                           enableInternalLoopback; /**< Whether enable internal loopback */
} cy_stc_isouart_host_config_t;
#endif /* (0U != ISOUART_MASTER) */

/** Configure the iso UART in Node mode. */
typedef struct
{
    cy_en_isouart_crc_policy_t     crcPolicy;              /**< CRC Polynomial */
    bool                           enableBlockId;          /**< Whether enable Block ID */
    bool                           useTrig0Emm;            /**< Whether use Trigger input 0 for EMM */
    bool                           useTrig1Emm;            /**< Whether use Trigger input 1 for EMM */
    bool                           useTrig2Emm;            /**< Whether use Trigger input 2 for EMM */
    bool                           useTrig3Emm;            /**< Whether use Trigger input 3 for EMM */
    bool                           enableMultiReadBC;      /**< Whether enable BC Multi Read */
    uint8_t                        baseAddressA;           /**< Base address A for Multi Read */
    uint8_t                        baseAddressB;           /**< Base address B for Multi Read */
} cy_stc_isouart_node_config_t;

/** Iso UART frame structure. */
typedef struct
{
    uint8_t                        addr;                   /**< Address frame */
    uint8_t                        nodeId;                 /**< ID frame */
    uint16_t                       data;                   /**< Data frame */
    uint8_t                        crc;                    /**< CRC frame */
} cy_stc_isouart_raw_frame_t;

/** Iso UART interface status. */
typedef struct
{
    cy_en_isouart_interface_state_t interfaceState;        /**< Interface state */
    cy_en_isouart_request_state_t   requestState;          /**< Request sub-state (REQUEST state) */
    cy_en_isouart_host_command_t    hostCommand;           /**< Last host command (slave transmit phases) */
    cy_en_isouart_reply_state_t     replyState;            /**< Reply sub-state (slave transmit phases) */
    uint32_t                        txDeviceId;            /**< Device ID currently transmitting */
    uint32_t                        packetNumber;          /**< Current packet index */
} cy_stc_isouart_interface_status_t;

/** ECC status structure */
typedef struct
{
    cy_en_isouart_ecc_error_t errorType;    /**< ECC error type. */
    uint32_t                  syndrome;     /**< ECC error syndrome. */
} cy_stc_isouart_ecc_status_t;

/** Interface node configuration structure */
typedef struct
{
    uint8_t nodeId;         /**< Address (ID) of the node. */
    uint8_t blockId;        /**< Block ID assigned to the node (Only useful when blockid feature is enabled). */
    bool    isFinalNode;    /**< Final node flag. */
} cy_stc_isouart_node_interface_config_t;
/** \} group_isouart_data_structures */

/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/
/**
* \addtogroup group_isouart_common_functions
* \{
*/

__STATIC_INLINE void Cy_ISOUART_Enable(ISOUART_Type* base);
__STATIC_INLINE void Cy_ISOUART_Disable(ISOUART_Type* base);

void Cy_ISOUART_ConfigIORouting(ISOUART_Type* base,
                                bool enInputIflLow,
                                bool enInputIflHigh,
                                bool enInputIfhLow,
                                bool enInputIfhHigh);

__STATIC_INLINE void Cy_ISOUART_ConfigureUvd(ISOUART_Type* base, bool isEnable, uint32_t filterLimit);
__STATIC_INLINE void Cy_ISOUART_ResetInterface(ISOUART_Type* base);
void Cy_ISOUART_GetInterfaceStatus(ISOUART_Type* base, cy_stc_isouart_interface_status_t* status);
/** \} group_isouart_common_functions */

/**
* \addtogroup group_isouart_sram_functions
* \{
*/
void Cy_ISOUART_NodeConfigSramRowAttr(ISOUART_Type* base,
                                     uint32_t index,
                                     uint8_t trigIntId,
                                     bool isInvalid,
                                     bool isReadOnly);

void Cy_ISOUART_HostSetSramSize(ISOUART_Type* base, uint32_t lowSramSize, uint32_t highSramSize);
volatile uint32_t * Cy_ISOUART_HostGetSramBaseAddr(ISOUART_Type* base, cy_en_isouart_interface_side_t iSide);
uint16_t Cy_ISOUART_ReadSramRow16(ISOUART_Type* base, uint32_t index);
uint32_t Cy_ISOUART_ReadSramRow32(ISOUART_Type* base, uint32_t index);
void Cy_ISOUART_WriteSramRow16(ISOUART_Type* base, uint32_t index, uint16_t data);
void Cy_ISOUART_WriteSramRow32(ISOUART_Type* base, uint32_t index, uint32_t data);
void Cy_ISOUART_HostReadSramRaw(ISOUART_Type* base, cy_en_isouart_interface_side_t iSide, uint32_t packetNum, cy_stc_isouart_raw_frame_t* frame);
uint16_t Cy_ISOUART_HostReadSramUnpack(ISOUART_Type* base, cy_en_isouart_interface_side_t iSide, uint32_t packetNum);
__STATIC_INLINE void Cy_ISOUART_EnableEcc(ISOUART_Type* base);
__STATIC_INLINE void Cy_ISOUART_DisableEcc(ISOUART_Type* base);
void Cy_ISOUART_EnableEccInjection(ISOUART_Type* base, uint32_t index, uint32_t parity);
void Cy_ISOUART_DisableEccInjection(ISOUART_Type* base);
void Cy_ISOUART_GetEccStatus(const ISOUART_Type* base, cy_stc_isouart_ecc_status_t* status);
/** \} group_isouart_sram_functions */

/**
* \addtogroup group_isouart_host_functions
* \{
*/

#if (0U != ISOUART_MASTER)

void Cy_ISOUART_HostInit(ISOUART_Type* base, const cy_stc_isouart_host_config_t* config);
void Cy_ISOUART_HostEnableWakeSequence(ISOUART_Type* base, uint32_t wakeCmdDelay);
__STATIC_INLINE uint8_t Cy_ISOUART_HostReadLowInterface(const ISOUART_Type* base);
__STATIC_INLINE uint8_t Cy_ISOUART_HostReadHighInterface(const ISOUART_Type* base);

void Cy_ISOUART_HostSendWriteCommand(ISOUART_Type* base, uint32_t blockId, uint32_t nodeId, uint32_t address, uint16_t data);
void Cy_ISOUART_HostSendReadCommand(ISOUART_Type* base, uint32_t blockId, uint32_t nodeId, uint32_t address);
void Cy_ISOUART_HostSendManualCommand(ISOUART_Type* base, uint8_t data);
bool Cy_ISOUART_HostVerifyCRC(uint8_t frameData);

#endif /* (0U != ISOUART_MASTER) */
/** \} group_isouart_host_functions */


/**
* \addtogroup group_isouart_node_functions
* \{
*/
void Cy_ISOUART_NodeInit(ISOUART_Type* base,  const cy_stc_isouart_node_config_t* config);
void Cy_ISOUART_NodeConfigTriggerInt(ISOUART_Type* base,
                                 uint8_t trigIntId,
                                 cy_en_isouart_event_t trigSrc,
                                 cy_en_isouart_event_t primIntSrc,
                                 cy_en_isouart_event_t secIntSrc);
__STATIC_INLINE void Cy_ISOUART_NodeSetWdtCount(ISOUART_Type* base, uint16_t counter);
__STATIC_INLINE uint16_t Cy_ISOUART_NodeGetWdtCount(const ISOUART_Type* base);
__STATIC_INLINE void Cy_ISOUART_NodeTriggerEMM(ISOUART_Type* base);
__STATIC_INLINE uint8_t Cy_ISOUART_NodeGetClkCounter(ISOUART_Type* base);
__STATIC_INLINE cy_en_isouart_wakeup_source_t Cy_ISOUART_NodeGetWakeupSource(const ISOUART_Type* base);
__STATIC_INLINE bool Cy_ISOUART_NodeGetMasterTopology(const ISOUART_Type* base);
uint32_t Cy_ISOUART_NodeGetNumMultiReadA(const ISOUART_Type* base);
uint32_t Cy_ISOUART_NodeGetNumMultiReadB(const ISOUART_Type* base);
void Cy_ISOUART_NodeGetInterfaceConfig(const ISOUART_Type* base, cy_stc_isouart_node_interface_config_t* config);
/** \} group_isouart_node_functions */

/**
* \addtogroup group_isouart_interrupt_functions
* \{
*/

__STATIC_INLINE void        Cy_ISOUART_ClearInterrupt(ISOUART_Type* base, uint32_t intrMask);
__STATIC_INLINE uint32_t    Cy_ISOUART_GetInterruptStatus(const ISOUART_Type* base);
__STATIC_INLINE void        Cy_ISOUART_SetInterrupt(ISOUART_Type* base, uint32_t intrMask);
__STATIC_INLINE uint32_t    Cy_ISOUART_GetInterruptMask(const ISOUART_Type* base);
__STATIC_INLINE void        Cy_ISOUART_SetInterruptMask(ISOUART_Type* base, uint32_t intrMask);
__STATIC_INLINE uint32_t    Cy_ISOUART_GetInterruptStatusMasked(const ISOUART_Type* base);

__STATIC_INLINE uint8_t Cy_ISOUART_NodeGetIntrAddrPrim(const ISOUART_Type* base);
__STATIC_INLINE cy_en_isouart_event_t Cy_ISOUART_NodeGetIntrEventPrim(const ISOUART_Type* base);
__STATIC_INLINE uint8_t Cy_ISOUART_NodeGetIntrAddrSec(const ISOUART_Type* base);
__STATIC_INLINE cy_en_isouart_event_t Cy_ISOUART_NodeGetIntrEventSec(const ISOUART_Type* base);

/** \} group_isouart_interrupt_functions */

/*****************************************************************************/
/*  In-line Function Implementation                                          */
/*****************************************************************************/

/**
* \addtogroup group_isouart_common_functions
* \{
*/

/*******************************************************************************
* Function Name: Cy_ISOUART_Enable
****************************************************************************//**
*
* \brief
* Enables the iso UART block.
* \param base
* The base address for the iso UART.
* \note
* Some registers are cleared when the iso UART block is disabled.
* For detail, refer to the device reference manual.
* \note
* This function is valid for both Host and Node.
*
*******************************************************************************/
__STATIC_INLINE void Cy_ISOUART_Enable(ISOUART_Type* base)
{
    CY_ASSERT_L1(NULL != base);

    CY_REG32_CLR_SET(base->CTL, ISOUART_CTL_ENABLED, 1U);
}

/*******************************************************************************
* Function Name: Cy_ISOUART_Disable
****************************************************************************//**
*
* \brief
* Disables the iso UART block.
* \param base
* The base address for the iso UART.
* \note
* Some registers are cleared when the iso UART block is disabled.
* For detail, refer to the device reference manual.
* \note
* This function is valid for both Host and Node.
*
*******************************************************************************/
__STATIC_INLINE void Cy_ISOUART_Disable(ISOUART_Type* base)
{
    CY_ASSERT_L1(NULL != base);

    CY_REG32_CLR_SET(base->CTL, ISOUART_CTL_ENABLED, 0U);
}

/*******************************************************************************
* Function Name: Cy_ISOUART_ConfigureUvd
****************************************************************************//**
*
* \brief
* Sets up the under voltage detection (UVD) control.
* \param base
* The base address for the iso UART.
* \param isEnable
* If true, enables the UVD.
* \param filterLimit
* The glitch filter counter limit for the UVD. Each count corresponds to 1/clk_hf seconds.
* Max value is 0x4B0, the default value is 0x2D0.
* \note
* This function is valid for both Host and Node.
*
*******************************************************************************/
__STATIC_INLINE void Cy_ISOUART_ConfigureUvd(ISOUART_Type* base, bool isEnable, uint32_t filterLimit)
{
    CY_ASSERT_L1(NULL != base);
    CY_ASSERT_L2(filterLimit <= CY_ISOUART_UVD_MAX_FILTER_LIMIT);

    base->UVD_CTL = _VAL2FLD(ISOUART_UVD_CTL_FILT_LIMIT, filterLimit) | \
                    _VAL2FLD(ISOUART_UVD_CTL_ENABLE, (isEnable ? 1U : 0U));
}

/*******************************************************************************
* Function Name: Cy_ISOUART_ResetInterface
****************************************************************************//**
*
* \brief
* Reset the interface when the interface gets stuck (i.e. when a node fails to respond).
* After resetting, the followings occur:
*  - Host clears the MASTER_START register to prepare for another transaction.
*    The internal regulator is disabled. It's re-enabled when next command is sent.
*  - Node moves the interface back to IDLE state and the internal regulator is disabled.
*    Need wakeup sequence reception again to wakeup iso UART interface.
* \param base
* The base address for the iso UART.
* \note
* This function is valid for both Host and Node.
*
*******************************************************************************/
__STATIC_INLINE void Cy_ISOUART_ResetInterface(ISOUART_Type* base)
{
    CY_ASSERT_L1(NULL != base);

    CY_REG32_CLR_SET(base->CTL, ISOUART_CTL_RESET_INTERFACE, 1U);
}
/** \} group_isouart_common_functions */

/**
* \addtogroup group_isouart_sram_functions
* \{
*/

/*******************************************************************************
* Function Name: Cy_ISOUART_EnableEcc
****************************************************************************//**
*
* \brief
* Enable ECC for iso UART SRAM.
* \param base
* The base address for the iso UART.
* \note
* This function is valid for both Host and Node.
*
*******************************************************************************/
__STATIC_INLINE void Cy_ISOUART_EnableEcc(ISOUART_Type* base)
{
    CY_ASSERT_L1(NULL != base);

    CY_REG32_CLR_SET(base->ECC_CTL, ISOUART_ECC_CTL_ECC_EN, 1U);
}

/*******************************************************************************
* Function Name: Cy_ISOUART_DisableEcc
****************************************************************************//**
*
* \brief
* Disable ECC for iso UART SRAM.
* \param base
* The base address for the iso UART.
* \note
* This function is valid for both Host and Node.
*
*******************************************************************************/
__STATIC_INLINE void Cy_ISOUART_DisableEcc(ISOUART_Type* base)
{
    CY_ASSERT_L1(NULL != base);

    CY_REG32_CLR_SET(base->ECC_CTL, ISOUART_ECC_CTL_ECC_EN, 0U);
}

/** \} group_isouart_sram_functions */

/**
* \addtogroup group_isouart_host_functions
* \{
*/

#if (0U != ISOUART_MASTER)

/*******************************************************************************
* Function Name: Cy_ISOUART_HostReadLowInterface
****************************************************************************//**
*
* \brief
* Reads the latest frame data from the Low interface.
* \param base
* The base address for the iso UART.
* \return
* The latest frame data from the Low interface.
* \note
* This function is valid only for Host.
* \funcusage
* \snippet isouart_snippet.c snippet_Cy_ISOUART_HostWrite
*
*******************************************************************************/
__STATIC_INLINE uint8_t Cy_ISOUART_HostReadLowInterface(const ISOUART_Type* base)
{
    CY_ASSERT_L1(NULL != base);

    return (uint8_t)_FLD2VAL(ISOUART_LOW_INTERFACE_READ_READ_LOW_DATA, base->LOW_INTERFACE_READ);
}

/*******************************************************************************
* Function Name: Cy_ISOUART_HostReadHighInterface
****************************************************************************//**
*
* \brief
* Reads the latest frame data from the High interface.
* \param base
* The base address for the iso UART.
* \return
* The latest frame data from the High interface.
* \note
* This function is valid only for Host.
*
*******************************************************************************/
__STATIC_INLINE uint8_t Cy_ISOUART_HostReadHighInterface(const ISOUART_Type* base)
{
    CY_ASSERT_L1(NULL != base);

    return (uint8_t)_FLD2VAL(ISOUART_HIGH_INTERFACE_READ_READ_HIGH_DATA, base->HIGH_INTERFACE_READ);
}

#endif /* (0U != ISOUART_MASTER) */
/** \} group_isouart_host_functions */

/**
* \addtogroup group_isouart_node_functions
* \{
*/

/*******************************************************************************
* Function Name: Cy_ISOUART_NodeSetWdtCount
****************************************************************************//**
*
* \brief
* Updates Wdt down counter by SW. The Wdt is normally fed by host via iso UART
* write command.
* This function is not mandatory to be called in normal case.
* \param base
* The base address for the iso UART.
* \param counter
* New counter value.
* \note
* This function is valid only for Node.
* \funcusage
* \snippet isouart_snippet.c snippet_Cy_ISOUART_NodeWatchdogRefresh
*
*******************************************************************************/
__STATIC_INLINE void Cy_ISOUART_NodeSetWdtCount(ISOUART_Type* base, uint16_t counter)
{
    CY_ASSERT_L1(NULL != base);

    base->IF_WDOG_CNT = (uint32_t)counter;
}

/*******************************************************************************
* Function Name: Cy_ISOUART_NodeGetWdtCount
****************************************************************************//**
*
* \brief
* Get the current Wdt counter value.
* \param base
* The base address for the iso UART.
* \return
* Counter value.
* \note
* This function is valid only for Node.
*
*******************************************************************************/
__STATIC_INLINE uint16_t Cy_ISOUART_NodeGetWdtCount(const ISOUART_Type* base)
{
    CY_ASSERT_L1(NULL != base);

    return (uint16_t)_FLD2VAL(ISOUART_IF_WDOG_CNT_WDOG_CNT, base->IF_WDOG_CNT);
}

/*******************************************************************************
* Function Name: Cy_ISOUART_NodeTriggerEMM
****************************************************************************//**
*
* \brief
* Triggers emergency mode (EMM) signal transmission on the chain.
* \param base
* The base address for the iso UART.
* \note
* This function is valid only for Node.
* \warning
* If called during active communication, it could cause a bus clash.
*
*******************************************************************************/
__STATIC_INLINE void Cy_ISOUART_NodeTriggerEMM(ISOUART_Type* base)
{
    CY_ASSERT_L1(NULL != base);

    CY_REG32_CLR_SET(base->CTL, ISOUART_CTL_EMERGENCY, 1U);
}

/*******************************************************************************
* Function Name: Cy_ISOUART_NodeGetClkCounter
****************************************************************************//**
*
* \brief
* Returns the raw clock counter captured at the last sync frame.
* This value is the number of peripheral clock cycles for the sync frame.
* Divide the returned value by 8 to obtain peripheral clock cycles per bit.
* \param base
* The base address for the iso UART.
* \return
* Raw clock counter value.
* \note
* This function is valid only for Node.
*
*******************************************************************************/
__STATIC_INLINE uint8_t Cy_ISOUART_NodeGetClkCounter(ISOUART_Type* base)
{
    CY_ASSERT_L1(NULL != base);

    return (uint8_t)base->CLK_STATUS;
}

/*******************************************************************************
* Function Name: Cy_ISOUART_NodeGetWakeupSource
****************************************************************************//**
*
* \brief
* Get the last wakeup source of the node.
* \param base
* The base address for the iso UART.
* \return
* Last wakeup source. See \ref cy_en_isouart_wakeup_source_t.
* \note
* This function is valid only for Node.
* \funcusage
* \snippet isouart_snippet.c snippet_Cy_ISOUART_GetWakeupSource
*
*******************************************************************************/
__STATIC_INLINE cy_en_isouart_wakeup_source_t Cy_ISOUART_NodeGetWakeupSource(const ISOUART_Type* base)
{
    CY_ASSERT_L1(NULL != base);

    CY_MISRA_DEVIATE_LINE('MISRA C-2012 Rule 10.8', 'The field values match the enumeration.');
    return (cy_en_isouart_wakeup_source_t)_FLD2VAL(ISOUART_IF_GEN_DIAG_GPIO_WAKEUP, base->IF_GEN_DIAG);
}

/*******************************************************************************
* Function Name: Cy_ISOUART_NodeGetMasterTopology
****************************************************************************//**
*
* \brief
* Get the current master topology configuration.
* \param base
* The base address for the iso UART.
* \return
* Current master topology configuration.
* - true  : The topology is MOT (Master On Top).
* - false : The topology is MOB (Master On Bottom).
* \note
* This function is valid only for Node. \n
* This function does not distinguish between ring and non-ring topologies.
* It only indicates whether the master is positioned at the top or bottom of the chain.
* \funcusage
* \snippet isouart_snippet.c snippet_Cy_ISOUART_GetMasterTopology
*
*******************************************************************************/
__STATIC_INLINE bool Cy_ISOUART_NodeGetMasterTopology(const ISOUART_Type* base)
{
    CY_ASSERT_L1(NULL != base);

    return (_FLD2BOOL(ISOUART_IF_GEN_DIAG_MOT_MOB_N, base->IF_GEN_DIAG));
}

/** \} group_isouart_node_functions */

/**
* \addtogroup group_isouart_interrupt_functions
* \{
*/

/*******************************************************************************
* Function Name: Cy_ISOUART_ClearInterrupt
****************************************************************************//**
*
* \brief
* Clears the specified iso UART interrupt.
* \param base
* The base address for the iso UART.
* \param intrMask
* The bitmask of statuses to clear.
* Select one or more values from \ref group_isouart_macros_interrupt_masks and "OR" them together.
* \note
* This function is valid for both Host and Node.
* \funcusage
* \snippet isouart_snippet.c snippet_Cy_ISOUART_Isr_Handler
*
*******************************************************************************/
__STATIC_INLINE void Cy_ISOUART_ClearInterrupt(ISOUART_Type* base, uint32_t intrMask)
{
    CY_ASSERT_L2(CY_ISOUART_INTRMASK_VALID(intrMask));

    base->INTR = intrMask & CY_ISOUART_INTR_MASK_ALL;
    /* Dummy read for buffered writes. */
    (void) base->INTR;
}

/*******************************************************************************
* Function Name: Cy_ISOUART_GetInterruptStatus
****************************************************************************//**
*
* \brief
* Get the specified iso UART interrupt status.
* \param base
* The base address for the iso UART.
* \return
* The bit field determines which iso UART interrupt is set.
* To find out occurred interrupt, use \ref group_isouart_macros_interrupt_masks.
* \note
* This function is valid for both Host and Node.
* \funcusage
* \snippet isouart_snippet.c snippet_Cy_ISOUART_Isr_Handler
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_ISOUART_GetInterruptStatus(const ISOUART_Type* base)
{
    return (base->INTR & CY_ISOUART_INTR_MASK_ALL);
}

/*******************************************************************************
* Function Name: Cy_ISOUART_SetInterrupt
****************************************************************************//**
*
* \brief
* Software triggers the specified iso UART interrupt.
* \param base
* The base address for the iso UART.
* \param intrMask
* The bit field determines which interrupt will be triggered.
* Select one or more values from \ref group_isouart_macros_interrupt_masks and "OR" them together.
* \note
* This function is valid for both Host and Node. \n
* General use is to set the interrupt source to check if HW operates correctly or to use it as a SW interrupt.
* \funcusage
* \snippet isouart_snippet.c snippet_Cy_ISOUART_Interrupt
*
*******************************************************************************/
__STATIC_INLINE void Cy_ISOUART_SetInterrupt(ISOUART_Type* base, uint32_t intrMask)
{
    CY_ASSERT_L2(CY_ISOUART_INTRMASK_VALID(intrMask));

    base->INTR_SET = intrMask & CY_ISOUART_INTR_MASK_ALL;
}

/*******************************************************************************
* Function Name: Cy_ISOUART_GetInterruptMask
****************************************************************************//**
*
* \brief
* Get the specified iso UART interrupt mask.
* \param base
* The base address for the iso UART.
* \return
* The bit field determines which status changes can cause an interrupt.
* To find out which interrupts are enabled, use \ref group_isouart_macros_interrupt_masks.
* \note
* This function is valid for both Host and Node.
* \funcusage
* \snippet isouart_snippet.c snippet_Cy_ISOUART_Interrupt
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_ISOUART_GetInterruptMask(const ISOUART_Type* base)
{
    return (base->INTR_MASK & CY_ISOUART_INTR_MASK_ALL);
}

/*******************************************************************************
* Function Name: Cy_ISOUART_SetInterruptMask
****************************************************************************//**
*
* \brief
* Enables the specified iso UART interrupt mask.
* \param base
* The base address for the iso UART.
* \param intrMask
* The bit field determines which status changes can cause an interrupt.
* Select one or more values from \ref group_isouart_macros_interrupt_masks and "OR" them together.
* \note
* This function is valid for both Host and Node.
* \funcusage
* \snippet isouart_snippet.c snippet_Cy_ISOUART_Interrupt
*
*******************************************************************************/
__STATIC_INLINE void Cy_ISOUART_SetInterruptMask(ISOUART_Type* base, uint32_t intrMask)
{
    CY_ASSERT_L2(CY_ISOUART_INTRMASK_VALID(intrMask));

    base->INTR_MASK = intrMask & CY_ISOUART_INTR_MASK_ALL;
}

/*******************************************************************************
* Function Name: Cy_ISOUART_GetInterruptStatusMasked
****************************************************************************//**
*
* \brief
* Get the specified iso UART interrupt masked (enabled) status.
* \param base
* The base address for the iso UART.
* \return
* Bitwise AND of the interrupt request and mask registers.
* To find out the state of the interrupts status masked, use \ref group_isouart_macros_interrupt_masks.
* \note
* This function is valid for both Host and Node.
* \funcusage
* \snippet isouart_snippet.c snippet_Cy_ISOUART_Isr_Handler
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_ISOUART_GetInterruptStatusMasked(const ISOUART_Type* base)
{
    return (base->INTR_MASKED & CY_ISOUART_INTR_MASK_ALL);
}

/*******************************************************************************
* Function Name: Cy_ISOUART_NodeGetIntrAddrPrim
****************************************************************************//**
*
* \brief
* Get the address of request that triggered the interrupt \ref CY_ISOUART_INTR_REG_ACCESS_P.
* \param base
* The base address for the iso UART.
* \return
* The address value.
* \note
* This function is valid only for Node.
*
*******************************************************************************/
__STATIC_INLINE uint8_t Cy_ISOUART_NodeGetIntrAddrPrim(const ISOUART_Type* base)
{
    CY_ASSERT_L1(NULL != base);

    return ((uint8_t)_FLD2VAL(ISOUART_INTR_REG_ACCESS_P_REG_ADDR, base->INTR_REG_ACCESS_P));
}

/*******************************************************************************
* Function Name: Cy_ISOUART_NodeGetIntrEventPrim
****************************************************************************//**
*
* \brief
* Get the event that triggered the interrupt \ref CY_ISOUART_INTR_REG_ACCESS_P.
* \param base
* The base address for the iso UART.
* \return
* The event value \ref cy_en_isouart_event_t.
* \note
* This function is valid only for Node.
*
*******************************************************************************/
__STATIC_INLINE cy_en_isouart_event_t Cy_ISOUART_NodeGetIntrEventPrim(const ISOUART_Type* base)
{
    CY_ASSERT_L1(NULL != base);

    CY_MISRA_DEVIATE_LINE('MISRA C-2012 Rule 10.8', 'The field values match the enumeration.');
    return ((cy_en_isouart_event_t)_FLD2VAL(ISOUART_INTR_REG_ACCESS_P_REG_EVENT, base->INTR_REG_ACCESS_P));
}

/*******************************************************************************
* Function Name: Cy_ISOUART_NodeGetIntrAddrSec
****************************************************************************//**
*
* \brief
* Get the address of request that triggered the interrupt \ref CY_ISOUART_INTR_REG_ACCESS_S.
* \param base
* The base address for the iso UART.
* \return
* The address value.
* \note
* This function is valid only for Node.
*
*******************************************************************************/
__STATIC_INLINE uint8_t Cy_ISOUART_NodeGetIntrAddrSec(const ISOUART_Type* base)
{
    CY_ASSERT_L1(NULL != base);

    return ((uint8_t)_FLD2VAL(ISOUART_INTR_REG_ACCESS_S_REG_ADDR, base->INTR_REG_ACCESS_S));
}

/*******************************************************************************
* Function Name: Cy_ISOUART_NodeGetIntrEventSec
****************************************************************************//**
*
* \brief
* Get the event that triggered the interrupt \ref CY_ISOUART_INTR_REG_ACCESS_S.
* \param base
* The base address for the iso UART.
* \return
* The event value \ref cy_en_isouart_event_t.
* \note
* This function is valid only for Node.
*
*******************************************************************************/
__STATIC_INLINE cy_en_isouart_event_t Cy_ISOUART_NodeGetIntrEventSec(const ISOUART_Type* base)
{
    CY_ASSERT_L1(NULL != base);

    CY_MISRA_DEVIATE_LINE('MISRA C-2012 Rule 10.8', 'The field values match the enumeration.');
    return ((cy_en_isouart_event_t)_FLD2VAL(ISOUART_INTR_REG_ACCESS_S_REG_EVENT, base->INTR_REG_ACCESS_S));
}

/** \} group_isouart_interrupt_functions */

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

#endif /*(CY_IP_M0S8ISOUART) */

#endif /* !defined(CY_ISOUART_H) */

/** \} group_isouart */

/* [] END OF FILE */
