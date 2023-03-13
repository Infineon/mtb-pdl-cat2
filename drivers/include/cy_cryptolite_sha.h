/***************************************************************************//**
* \file cy_cryptolite_sha.h
* \version 1.20
*
* \brief
*  This file provides common constants and parameters for the Cryptolite driver.
*
*******************************************************************************
* \copyright
* (c) (2021-2022), Cypress Semiconductor Corporation (an Infineon company) or
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
#if !defined (CY_CRYPTOLITE_SHA_H)
#define CY_CRYPTOLITE_SHA_H

#include "cy_device.h"

#if (defined (CY_IP_M0S8CRYPTOLITE) && defined (CRYPTOLITE_SHA_PRESENT))  || defined (CY_DOXYGEN)

#include "cy_syslib.h"

#if defined(__cplusplus)
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include "cy_sysint.h"
#include "cy_cryptolite_common.h"

/**
* \addtogroup group_cryptolite_sha
* \{
* Secure Hash Algorithm (SHA)
*
* SHA generates an almost-unique 256-bit (32-byte) signature for a text. The 
* applications of SHA include hash tables, integrity verification, challenge 
* handshake authentication, digital signatures, etc.
*
* \defgroup group_cryptolite_sha_macros Macros
* \defgroup group_cryptolite_sha_data_structures Structures
* \defgroup group_cryptolite_sha_functions Functions
*
* */

/**
* \addtogroup group_cryptolite_sha_functions
* \{
*/

/*******************************************************************************
* Function Name: Cy_Cryptolite_Sha_Init
****************************************************************************//**
*
* The function to initialize the SHA256 operation.
*
* \param base
* The pointer to the Cryptolite instance.
*
* \param cfContext
* The pointer to the \ref cy_stc_cryptolite_sha_context_t structure that stores all
* internal variables for Cryptolite driver.
*
* \return
* \ref cy_en_cryptolite_status_t
*
* \funcusage
* \snippet cryptolite_sut.c snippet_myCryptoliteShaUse2
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Sha_Init(CRYPTOLITE_Type *base,
                                        cy_stc_cryptolite_sha_context_t *cfContext);

/*******************************************************************************
* Function Name: Cy_Cryptolite_Sha_Start
****************************************************************************//**
*
* Initializes the initial Hash vector.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param cfContext
* The pointer to the \ref cy_stc_cryptolite_sha_context_t structure that stores all
* internal variables for Cryptolite driver.
*
* \return
* \ref cy_en_cryptolite_status_t
*
* \funcusage
* \snippet cryptolite_sut.c snippet_myCryptoliteShaUse2
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Sha_Start(CRYPTOLITE_Type *base,
                                        cy_stc_cryptolite_sha_context_t *cfContext);

/*******************************************************************************
* Function Name: Cy_Cryptolite_Sha_Update
****************************************************************************//**
*
* Performs the SHA256 calculation on one message.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param message
* The pointer to the message whose Hash is being computed.
*
* \param messageSize
* The size of the message whose Hash is being computed.
*
* \param cfContext
* The pointer to the \ref cy_stc_cryptolite_sha_context_t structure that stores all
* internal variables for Cryptolite driver.
*
* \return
* \ref cy_en_cryptolite_status_t
* 
* \note There is no alignment or size restriction for message buffer, However providing
* a four byte aligned buffer with size in multiple of \ref CY_CRYPTOLITE_SHA256_BLOCK_SIZE,
* will result in best execution time.
*
* \funcusage
* \snippet cryptolite_sut.c snippet_myCryptoliteShaUse2
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Sha_Update(CRYPTOLITE_Type *base,
                                                    uint8_t const *message,
                                                    uint32_t  messageSize,
                                                    cy_stc_cryptolite_sha_context_t *cfContext);

/*******************************************************************************
* Function Name: Cy_Cryptolite_Sha_Finish
****************************************************************************//**
*
* Completes the SHA256 calculation.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param digest
* The pointer to the calculated Hash digest.
*
* \param cfContext
* The pointer to the \ref cy_stc_cryptolite_sha_context_t structure that stores all
* internal variables for Cryptolite driver.
*
* \return
* \ref cy_en_cryptolite_status_t
*
* \funcusage
* \snippet cryptolite_sut.c snippet_myCryptoliteShaUse2
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Sha_Finish(CRYPTOLITE_Type *base,
                               uint8_t *digest,
                               cy_stc_cryptolite_sha_context_t *cfContext);

/*******************************************************************************
* Function Name: Cy_Cryptolite_Sha_Free
****************************************************************************//**
*
* Clears the used memory and context data.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param cfContext
* The pointer to the \ref cy_stc_cryptolite_sha_context_t structure that stores all
* internal variables for Cryptolite driver.
*
* \return
* \ref cy_en_cryptolite_status_t
*
* \funcusage
* \snippet cryptolite_sut.c snippet_myCryptoliteShaUse2
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Sha_Free(CRYPTOLITE_Type *base,
                                                    cy_stc_cryptolite_sha_context_t *cfContext);


/*******************************************************************************
* Function Name: Cy_Cryptolite_Sha
****************************************************************************//**
*
* This function performs the SHA256 Hash function.
* Provide the required parameters and the pointer
* to the context structure when making this function call.
* It is independent of the previous Crypto state because it already contains
* preparation, calculation, and finalization steps.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param message
* The pointer to a message whose hash value is being computed.
*
* \param messageSize
* The size of a message in bytes.
*
* \param digest
* The pointer to the hash digest.
*
* \param cfContext
* The pointer to the \ref cy_stc_cryptolite_sha_context_t structure that stores all
* internal variables for Cryptolite driver.
*
* \return
* \ref cy_en_cryptolite_status_t
*
* \note There is no alignment or size restriction for message buffer, However providing
* a four byte aligned buffer with size in multiple of \ref CY_CRYPTOLITE_SHA256_BLOCK_SIZE,
* will result in best execution time.
*
* \funcusage
* \snippet cryptolite_sut.c snippet_myCryptoliteShaUse
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Sha(CRYPTOLITE_Type *base,
                                        uint8_t const *message,
                                        uint32_t  messageSize,
                                        uint8_t *digest,
                                        cy_stc_cryptolite_sha_context_t *cfContext);

/** \} group_cryptolite_sha_functions */
/** \} group_cryptolite_sha */

#if defined(__cplusplus)
}
#endif

#endif /* (defined (CY_IP_M0S8CRYPTOLITE) && defined (CRYPTOLITE_SHA_PRESENT)) || defined (CY_DOXYGEN) */

#endif /* #if !defined (CY_CRYPTOLITE_SHA_H) */

/* [] END OF FILE */
