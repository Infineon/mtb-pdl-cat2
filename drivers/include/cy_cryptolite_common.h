/***************************************************************************//**
* \file cy_cryptolite_common.h
* \version 1.0
*
* \brief
*  This file provides common constants and parameters for the Cryptolite driver.
*
*******************************************************************************
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

#if !defined(CY_CRYPTOLITE_COMMON_H)
#define CY_CRYPTOLITE_COMMON_H

#include "cy_device.h"

#if defined(CY_IP_M0S8CRYPTOLITE)

#include "cy_syslib.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**
* \addtogroup group_cryptolite_common
* \{
* Cryptolite Common.
* This file contains all the necessary Macros, Structure and Enum definition required
* by the cryptolite driver.
*
* \defgroup group_cryptolite_common_macros Macros
* \defgroup group_cryptolite_common_enums Enums
*
* */

/** \} group_cryptolite_common */

/**
* \addtogroup group_cryptolite_common_macros
* \{
*/

/** Driver major version */
#define CY_CRYPTOLITE_DRV_VERSION_MAJOR         1

/** Driver minor version */
#define CY_CRYPTOLITE_DRV_VERSION_MINOR         0

/** \} group_cryptolite_common_macros */

#if defined (CRYPTOLITE_AES_PRESENT) || defined (CY_DOXYGEN)
/**
* \addtogroup group_cryptolite_aes_macros
* \{
*/

/** Crypto AES block size (in bytes) */
#define CY_CRYPTOLITE_AES_BLOCK_SIZE          (16U)

/** Crypto AES_128 key size (in bytes) */
#define CY_CRYPTOLITE_AES_128_KEY_SIZE        (16U)

/** Crypto AES maximum key size (in bytes) */
#define CY_CRYPTOLITE_AES_MAX_KEY_SIZE        (CY_CRYPTOLITE_AES_128_KEY_SIZE)

/** Crypto AES maximum key size (in four-byte words) */
#define CY_CRYPTOLITE_AES_MAX_KEY_SIZE_U32    (uint32_t)(CY_CRYPTOLITE_AES_MAX_KEY_SIZE / 4UL)

/** AES block size in four-byte words */
#define CY_CRYPTOLITE_AES_BLOCK_SIZE_U32      (uint32_t)(CY_CRYPTOLITE_AES_BLOCK_SIZE / 4UL)

/** \} group_cryptolite_aes_macros */

#endif /* defined (CRYPTOLITE_AES_PRESENT) || defined (CY_DOXYGEN) */

#if defined (CRYPTOLITE_SHA_PRESENT) || defined (CY_DOXYGEN)
/**
* \addtogroup group_cryptolite_sha_macros
* \{
*/
/* Defines for the SHA algorithm */

/** PAD size for the SHA256(in bytes)    */
#define CY_CRYPTOLITE_SHA256_PAD_SIZE           (56UL)

/** Hash size for SHA256 mode (in bytes) */
#define CY_CRYPTOLITE_SHA256_DIGEST_SIZE        (32UL)
/** Maximal hash size for SHA modes (in bytes) */
#define CY_CRYPTOLITE_SHA_MAX_DIGEST_SIZE       (CY_CRYPTOLITE_SHA256_DIGEST_SIZE)

/** Block size for SHA256 mode (in bytes)   */
#define CY_CRYPTOLITE_SHA256_BLOCK_SIZE         (64UL)

/** Block size for SHA256 mode (in 4-bytes word)   */
#define CY_CRYPTOLITE_SHA256_BLOCK_SIZE_U32     (CY_CRYPTOLITE_SHA256_BLOCK_SIZE / 4UL)

/** Maximal block size for SHA modes (in bytes)   */
#define CY_CRYPTOLITE_SHA_MAX_BLOCK_SIZE        (CY_CRYPTOLITE_SHA256_BLOCK_SIZE)

/** Hash size for SHA256 mode (in bytes)  */
#define CY_CRYPTOLITE_SHA256_HASH_SIZE          (32UL)

/** Hash size for SHA256 mode (in 4-bytes word)  */
#define CY_CRYPTOLITE_SHA256_HASH_SIZE_U32      (CY_CRYPTOLITE_SHA256_HASH_SIZE / 4UL)

/** Maximal hash size for SHA modes (in bytes)   */
#define CY_CRYPTOLITE_SHA_MAX_HASH_SIZE         (CY_CRYPTO_SHA256_HASH_SIZE)

/** SHA message schedule function bit value. */
#define CY_CRYPTOLITE_MSG_SCH_CTLWD             (0UL << 28U)
/** SHA message process function bit value. */
#define CY_CRYPTOLITE_PROCESS_CTLWD             (1UL << 28U)

/** \} group_cryptolite_sha_macros */
#endif /* defined (CRYPTOLITE_SHA_PRESENT) || defined (CY_DOXYGEN) */

#if defined (CRYPTOLITE_VU_PRESENT) || defined (CY_DOXYGEN)
/**
* \addtogroup group_cryptolite_vu_macros
* \{
*/

/** Vector unit multiplication operation opcode. */
#define CY_CRYPTOLITE_VU_OPCODE_MUL                 (0UL)
/** Vector unit addition operation opcode. */
#define CY_CRYPTOLITE_VU_OPCODE_ADD                 (1UL)
/** Vector unit subtraction operation opcode. */
#define CY_CRYPTOLITE_VU_OPCODE_SUB                 (2UL)
/** Vector unit XOR operation opcode. */
#define CY_CRYPTOLITE_VU_OPCODE_XOR                 (3UL)
/** Vector unit long integer multiplication operation opcode. */
#define CY_CRYPTOLITE_VU_OPCODE_XMUL                (4UL)
/** Vector unit right shift by one bit operation opcode. */
#define CY_CRYPTOLITE_VU_OPCODE_LSR1                (5UL)
/** Vector unit left shift by one bit operation opcode. */
#define CY_CRYPTOLITE_VU_OPCODE_LSL1                (6UL)
/** Vector unit left shift operation on a given value opcode. */
#define CY_CRYPTOLITE_VU_OPCODE_LSR                 (7UL)
/** Vector unit conditional operation operation opcode. */
#define CY_CRYPTOLITE_VU_OPCODE_COND_SUB            (8UL)
/** Vector unit move/copy operation opcode. */
#define CY_CRYPTOLITE_VU_OPCODE_MOV                 (9UL)

/** Vector unit opcode bit position. */
#define CY_CRYPTOLITE_VU_BIT_POS_OPCODE             (28U)
/** Vector unit destination/result bit position. */
#define CY_CRYPTOLITE_VU_BIT_POS_DEST_OPERAND       (16U)
/** Vector unit arithmetic/logic operation first operand bit position. */
#define CY_CRYPTOLITE_VU_BIT_POS_OPERAND1           (8U)
/** Vector unit arithmetic/logic operation second operand bit position. */
#define CY_CRYPTOLITE_VU_BIT_POS_OPERAND0           (0U)

/** \} group_cryptolite_vu_macros */
#endif /* defined (CRYPTOLITE_VU_PRESENT) || defined (CY_DOXYGEN) */

#if defined(CRYPTOLITE_TRNG_PRESENT) || defined(CY_DOXYGEN)
/**
* \addtogroup group_cryptolite_trng_macros
* \{
*/

/** TRNG health monitor DAS bit stream selection. */
#define CY_CRYPTOLITE_TRNG_BITSTREAM_DAS            (0UL)
/** TRNG health monitor RED bit stream selection. */
#define CY_CRYPTOLITE_TRNG_BITSTREAM_RED            (1UL)
/** TRNG health monitor TR bit stream selection. */
#define CY_CRYPTOLITE_TRNG_BITSTREAM_TR             (2UL)

/** TRNG Adaptive Proportion (AP) test enable bit. */
#define CY_CRYPTOLITE_TRNG_AP_EN                    (1UL)
/** TRNG Repetition Count test enable bit. */
#define CY_CRYPTOLITE_TRNG_RC_EN                    (1UL)

/** \} group_cryptolite_trng_macros */

#endif /* defined(CRYPTOLITE_TRNG_PRESENT) || define(CY_DOXYGEN) */


/** \cond INTERNAL */

/* Width of the Crypto hardware registers values in bits */
#define CY_CRYPTOLITE_HW_REGS_WIDTH             (32UL)

/* Calculates the actual size in bytes of the bits value */
#define CY_CRYPTOLITE_BYTE_SIZE_OF_BITS(x)      (uint32_t)(((uint32_t)(x) + 7U) >> 3U)

/* Calculates the actual size in 32-bit words of the bits value */
#define CY_CRYPTOLITE_WORD_SIZE_OF_BITS(x)      (uint32_t)(((uint32_t)(x) + 31U) >> 5U)

/** \endcond */


/**
* \addtogroup group_cryptolite_common_macros
* \{
*/
/** Crypto Driver PDL ID */
#define CY_CRYPTOLITE_ID                        CY_PDL_DRV_ID(0x74U)

/** \} group_cryptolite_common_macros */
/**
* \addtogroup group_cryptolite_common_enums
* \{
*/

/** Crypto block errors */
typedef enum
{
    /** Operation completed successfully */
    CY_CRYPTOLITE_SUCCESS             = 0x00U,

    /** A hardware error occurred */
    CY_CRYPTOLITE_BUS_ERROR           = CY_CRYPTOLITE_ID | CY_PDL_STATUS_ERROR | 0x01U,

    /** Crypto operation parameters are incorrect */
    CY_CRYPTOLITE_BAD_PARAMS          = CY_CRYPTOLITE_ID | CY_PDL_STATUS_ERROR | 0x02U,

    /** Crypto operation is invalid **/
    CY_CRYPTOLITE_INVALID_OPERATION   = CY_CRYPTOLITE_ID | CY_PDL_STATUS_ERROR | 0x03U,

    /** Cryptolite HW block is busy **/
    CY_CRYPTOLITE_HW_BUSY             = CY_CRYPTOLITE_ID | CY_PDL_STATUS_WARNING | 0x04U,

    /** Cryptolite TRNG HW block AP or RC detected **/
    CY_CRYPTOLITE_TRNG_AP_RC_DETECTED = CY_CRYPTOLITE_ID | CY_PDL_STATUS_ERROR | 0x05U,

    /** Unknown error */
    CY_CRYPTOLITE_UNKNOWN             = CY_CRYPTOLITE_ID | CY_PDL_STATUS_ERROR | 0xFFU
} cy_en_cryptolite_status_t;

/** \} group_cryptolite_common_enums */

#if defined (CRYPTOLITE_AES_PRESENT) || defined (CY_DOXYGEN)
/**
* \addtogroup group_cryptolite_aes_data_structures
* \{
*/
/** Structure for storing the AES descriptor.
* Pointer stored in this must be 4B aligned. This structure is used for
* Cryptolite IP only.
*/
typedef struct
{
    /** \cond INTERNAL */
    /** AES key pointer*/
    uint32_t keyAddr;
    /** AES source/plaintext pointer */
    uint32_t srcAddr;
    /** AES dest/plaintext pointer */
    uint32_t dstAddr;
    /** \endcond */
} cy_stc_cryptolite_aes_desc_t;

/** Structure for storing the AES context.
* All fields for this structure are internal. Firmware never reads or
* writes these values. Firmware allocates the structure and provides the
* address of the structure to the driver in the function calls. Firmware must
* ensure that the defined instance of this structure remains in the scope
* while the driver is in use.
*/

typedef struct
{
    /** \cond INTERNAL */
    /** AES key */
    uint32_t key[CY_CRYPTOLITE_AES_MAX_KEY_SIZE_U32];
    /** AES descriptor */
    cy_stc_cryptolite_aes_desc_t desc;
    /** \endcond */
} cy_stc_cryptolite_aes_context_t;

/** \} group_cryptolite_aes_data_structures */
#endif /* defined (CRYPTOLITE_AES_PRESENT) || defined (CY_DOXYGEN) */

#if defined (CRYPTOLITE_SHA_PRESENT) || defined (CY_DOXYGEN)
/**
* \addtogroup group_cryptolite_sha_data_structures
* \{
*/
/** The Cryptolite SHA256 IP descriptor structure.
* All fields for the structure are internal. Firmware never reads or
* writes these values.
*/
typedef struct sha_struct_t {
    /** \cond INTERNAL */
   uint32_t data0;
   uint32_t data1;
   uint32_t data2;
   /** \endcond */
} cy_stc_cryptolite_sha_desc_t;

/** The structure for storing the SHA256 context.
* All fields for the context structure are internal. Firmware never reads or
* writes these values. Firmware allocates the structure and provides the
* address of the structure to the driver in the function calls. Firmware must
* ensure that the defined instance of this structure remains in scope
* while the drive is in use.
*/
typedef struct
{
    /** \cond INTERNAL */
    uint32_t msgBlock[CY_CRYPTOLITE_SHA256_BLOCK_SIZE_U32];
    uint32_t hash[CY_CRYPTOLITE_SHA256_HASH_SIZE_U32];
    uint32_t message_schedule[CY_CRYPTOLITE_SHA256_BLOCK_SIZE];
    uint8_t *message;
    uint32_t messageSize;
    uint32_t msgIdx;
    /** Operation data descriptors */
    cy_stc_cryptolite_sha_desc_t message_schedule_struct;
    cy_stc_cryptolite_sha_desc_t message_process_struct;
    /** \endcond */
} cy_stc_cryptolite_sha_context_t;

/** \} group_cryptolite_sha_data_structures */
#endif /* defined (CRYPTOLITE_SHA_PRESENT) || defined (CY_DOXYGEN) */

#if defined (CRYPTOLITE_VU_PRESENT) || defined (CY_DOXYGEN)
/**
* \addtogroup group_cryptolite_vu_data_structures
* \{
*/
/** The Cryptolite Vector Unit (VU) IP descriptor structure.
* All fields for the structure are internal. Firmware never reads or
* writes these values.
*/
typedef struct {
    /** opcode and size(s) */
    uint32_t data0;     
    /** Source 0 */
    uint32_t data1;
    /** Source 1 */
    uint32_t data2;
    /** Destination */
    uint32_t data3;
} cy_stc_cryptolite_vu_desc_t;

/** \} group_cryptolite_vu_data_structures */
#endif /* defined (CRYPTOLITE_VU_PRESENT) || defined (CY_DOXYGEN) */
/** \} group_cryptolite_data_structures */

#if defined(__cplusplus)
}
#endif

#endif /* defined(CY_IP_M0S8CRYPTOLITE) */

#endif /* !defined(CY_CRYPTOLITE_COMMON_H) */


/* [] END OF FILE */
