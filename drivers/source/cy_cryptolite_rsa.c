/***************************************************************************//**
* \file cy_cryptolite_rsa.c
* \version 1.30
*
* \brief
* This file implements various RSA functionalities such as
* calculating Montgomery and Barrett coefficients, performing
* modular exponentiations etc.
*
*******************************************************************************
* \copyright
* (c) (2021-2024), Cypress Semiconductor Corporation (an Infineon company) or
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
#include "cy_device.h"

#if (defined(CY_IP_M0S8CRYPTOLITE) && defined (CRYPTOLITE_VU_PRESENT))

#if defined(__cplusplus)
extern "C" {
#endif

#include "cy_cryptolite_rsa.h"
#include "cy_cryptolite_vu.h"

/*******************************************************************************
* Function Name: Cy_Cryptolite_Rsa_TestEven (for internal use)
****************************************************************************//**
*
* This function checks for Input number is Even or not
*
* \param dataPtr
* The pointer to data structure.
*
* \return
* TRUE  - Input number is Even number \n
* FALSE - Input number is Odd number
*
*******************************************************************************/
ATTRIBUTES_CRYPTOLITE_RSA __STATIC_FORCEINLINE bool Cy_Cryptolite_Rsa_TestEven (uint8_t* dataPtr)
{
    uint32_t* dataPtrUint32 = (uint32_t *)(void *)dataPtr;
    uint32_t  word  = *dataPtrUint32;

    return ((word & 1UL) == 0UL);
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Rsa_ClrBit (for internal use)
****************************************************************************//**
*
* This function clears the bit at given bit position in data structure.
*
* \param dataPtr
* The pointer to data structure.
*
* \param bitPos
* Bit position to be cleared in the given data structure.
*
* \return
* None
*
*******************************************************************************/
ATTRIBUTES_CRYPTOLITE_RSA __STATIC_FORCEINLINE void Cy_Cryptolite_Rsa_ClrBit (uint8_t* dataPtr, uint32_t bitPos)
{
    uint32_t byteOffset = bitPos >> 3UL;
    uint32_t bitOffset  = bitPos & 7UL;

    dataPtr[byteOffset] = dataPtr[byteOffset] & ((uint8_t)~(1UL << bitOffset));
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Rsa_SetBit (for internal use)
****************************************************************************//**
*
* This function sets the bit at given bit position in data structure.
*
* \param dataPtr
* The pointer to data structure.
*
* \param bitPos
* Bit position to be set in the given data structure.
*
* \return
* None
*
*******************************************************************************/
ATTRIBUTES_CRYPTOLITE_RSA __STATIC_FORCEINLINE void Cy_Cryptolite_Rsa_SetBit (uint8_t* dataPtr, uint32_t bitPos)
{
    uint32_t byteOffset = bitPos >> 3UL;
    uint32_t bitOffset  = bitPos & 7UL;

    dataPtr[byteOffset] = dataPtr[byteOffset] | ((uint8_t)(1UL << bitOffset));
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Rsa_GetBit (for internal use)
****************************************************************************//**
*
* This function gets the bit value at given bit position in data structure.
*
* \param dataPtr
* The pointer to data structure.
*
* \param bitPos
* Bit position for which the value need to be returned from the given data structure.
*
* \return
* uint32_t bit value either 0 or 1.
*
*******************************************************************************/
ATTRIBUTES_CRYPTOLITE_RSA __STATIC_FORCEINLINE uint32_t Cy_Cryptolite_Rsa_GetBit (uint8_t* dataPtr, uint32_t bitPos)
{
    uint32_t byteOffset = bitPos >> 3UL;
    uint32_t bitOffset  = bitPos & 7UL;

    return (((uint32_t)dataPtr[byteOffset] >> bitOffset) & 1UL);
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Rsa_Lsl (for internal use)
****************************************************************************//**
*
* This function logical shift left (LSL) operation.
*
* \param dataOutputPtr
* The pointer to output array.
*
* \param outputSize
* Size of the output array.
*
* \param dataInputPtr
* The pointer to input array.
*
* \param bitShift
* No of bits to be shifted in the given data structure.
*
* \return
* None
*
*******************************************************************************/
ATTRIBUTES_CRYPTOLITE_RSA static void Cy_Cryptolite_Rsa_Lsl (uint8_t* dataOutputPtr, uint32_t outputSize, uint8_t* dataInputPtr, uint32_t bitShift)
{
    uint32_t  wordShift            = bitShift >> 5UL;
    uint32_t  bitOffset            = bitShift & 31UL;
    uint32_t* dataOutputPtrUint32  = (uint32_t *) (void *)dataOutputPtr;
    uint32_t* dataInputPtrUint32   = (uint32_t *) (void *)dataInputPtr;
    uint32_t  saved = 0UL;
    uint32_t  word;

    while (dataOutputPtrUint32 < (((uint32_t *) (void *)dataOutputPtr) + wordShift))
    {
        *dataOutputPtrUint32++ = 0UL;
    }

    while (dataOutputPtrUint32 < (((uint32_t *) (void *)dataOutputPtr) + outputSize))
    {
        word = *dataInputPtrUint32++;
        *dataOutputPtrUint32++ = (word <<  bitOffset) | saved;
        saved = (word >> (32UL - wordShift));
    }
}

#if (!CY_CRYPTOLITE_RSA_PRE_CALC_COEFF_ENABLE)
/*******************************************************************************
* Function Name: Cy_Cryptolite_Rsa_Clr (for internal use)
****************************************************************************//**
*
* This function is used to clear the input data of given size.
*
* \param dataPtr
* The pointer to data structure.
*
* \param dataSize
* Size of data to be cleared in 32bit word in the given data structure.
*
* \return
* None
*
*******************************************************************************/
ATTRIBUTES_CRYPTOLITE_RSA static void Cy_Cryptolite_Rsa_Clr (uint8_t* dataPtr, uint32_t dataSize)
{
    uint32_t* dataPtrUint32 = (uint32_t *) (void *)dataPtr;
    uint32_t  i = 0UL;

    do
    {
        *dataPtrUint32++ = 0UL;
        i++;
    } while (i < dataSize);
}
#endif /*(!CY_CRYPTOLITE_RSA_PRE_CALC_COEFF_ENABLE) */

/*******************************************************************************
* Function Name: Cy_Cryptolite_Rsa_MontInvTransform (for internal use)
****************************************************************************//**
*
* This function transforms from Montgomery domain back to regular domain.
*
* \param base
* Cryptolite Base address.
*
* \param rsaSignaturePtr
* Pointer to RSA signature.
*
* \param p_t1_2n1
* Pointer to internal buffer.
*
* \param p_t2_2n1
* Pointer to internal buffer.
*
* \param key
* Pointer to Public key and RSA coefficients.
*
* \return
* None
*
*******************************************************************************/
ATTRIBUTES_CRYPTOLITE_RSA static void Cy_Cryptolite_Rsa_MontInvTransform (
    CRYPTOLITE_Type *base,
    uint8_t* rsaSignaturePtr,
    uint8_t* p_t1_2n1,
    uint8_t* p_t2_2n1,
    cy_stc_cryptolite_rsa_pub_key_t *key
)
{
    uint32_t bitSize = key->moduloLength;

    Cy_Cryptolite_Vu_Mul_Hw (base, rsaSignaturePtr, VU_BITS_TO_WORDS(bitSize), key->inverseModuloPtr, VU_BITS_TO_WORDS(bitSize), p_t1_2n1, VU_BITS_TO_WORDS(2UL*bitSize));
    Cy_Cryptolite_Vu_WaitForComplete (base);
    Cy_Cryptolite_Vu_Lsr_Hw (base, p_t1_2n1, VU_BITS_TO_WORDS(2UL*bitSize), bitSize, rsaSignaturePtr, VU_BITS_TO_WORDS(bitSize));
    Cy_Cryptolite_Vu_WaitForComplete (base);
    Cy_Cryptolite_Vu_Mul_Hw (base, rsaSignaturePtr, VU_BITS_TO_WORDS(bitSize), key->barretCoefPtr, VU_BITS_TO_WORDS(bitSize+1UL), p_t2_2n1, VU_BITS_TO_WORDS(2UL*bitSize));
    Cy_Cryptolite_Vu_WaitForComplete (base);
    Cy_Cryptolite_Vu_Lsr_Hw (base, p_t2_2n1, VU_BITS_TO_WORDS(2UL*bitSize), bitSize, rsaSignaturePtr, VU_BITS_TO_WORDS(bitSize));
    Cy_Cryptolite_Vu_WaitForComplete (base);
    Cy_Cryptolite_Vu_Mul_Hw (base, rsaSignaturePtr, VU_BITS_TO_WORDS(bitSize), key->moduloPtr, VU_BITS_TO_WORDS(bitSize), p_t2_2n1, VU_BITS_TO_WORDS(2UL*bitSize));
    Cy_Cryptolite_Vu_WaitForComplete (base);
    Cy_Cryptolite_Vu_Sub_Hw (base, p_t1_2n1, VU_BITS_TO_WORDS(bitSize+2UL), p_t2_2n1, VU_BITS_TO_WORDS(bitSize+2UL), p_t1_2n1, VU_BITS_TO_WORDS(bitSize+2UL));
    Cy_Cryptolite_Vu_WaitForComplete (base);
    /* Reduction. */
    Cy_Cryptolite_Vu_Cond_Sub_Hw (base, p_t1_2n1, VU_BITS_TO_WORDS(bitSize+2UL), key->moduloPtr, VU_BITS_TO_WORDS(bitSize), p_t2_2n1, VU_BITS_TO_WORDS(bitSize+1UL));
    Cy_Cryptolite_Vu_WaitForComplete (base);
    Cy_Cryptolite_Vu_Cond_Sub_Hw (base, p_t2_2n1, VU_BITS_TO_WORDS(bitSize+1UL), key->moduloPtr, VU_BITS_TO_WORDS(bitSize), rsaSignaturePtr, VU_BITS_TO_WORDS(bitSize));
    Cy_Cryptolite_Vu_WaitForComplete (base);
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Rsa_MontTransform (for internal use)
****************************************************************************//**
*
* This function performs transformation into Montgomery domain.
*
* \param base
* Cryptolite Base address.
*
* \param rsaSignaturePtr
* Pointer to RSA signature.
*
* \param p_t1_2n1
* Pointer to internal buffer.
*
* \param p_t2_2n1
* Pointer to internal buffer.
*
* \param key
* Pointer to Public key and RSA coefficients.
*
* \return
* None
*
*******************************************************************************/
ATTRIBUTES_CRYPTOLITE_RSA static void Cy_Cryptolite_Rsa_MontTransform (
    CRYPTOLITE_Type *base,
    uint8_t* rsaSignaturePtr,
    uint8_t* p_t1_2n1,
    uint8_t* p_t2_2n1,
    cy_stc_cryptolite_rsa_pub_key_t *key
)
{
    uint32_t bitSize = key->moduloLength;

    Cy_Cryptolite_Rsa_Lsl (p_t1_2n1, VU_BITS_TO_WORDS(2UL*bitSize), rsaSignaturePtr, bitSize);

    Cy_Cryptolite_Vu_Mul_Hw (base, rsaSignaturePtr, VU_BITS_TO_WORDS(bitSize), key->barretCoefPtr, VU_BITS_TO_WORDS(bitSize+1UL), p_t2_2n1, VU_BITS_TO_WORDS(2UL*bitSize));
    Cy_Cryptolite_Vu_WaitForComplete (base);
    Cy_Cryptolite_Vu_Lsr_Hw (base, p_t2_2n1, VU_BITS_TO_WORDS(2UL*bitSize), bitSize, rsaSignaturePtr, VU_BITS_TO_WORDS(bitSize));
    Cy_Cryptolite_Vu_WaitForComplete (base);
    Cy_Cryptolite_Vu_Mul_Hw (base, rsaSignaturePtr, VU_BITS_TO_WORDS(bitSize), key->moduloPtr, VU_BITS_TO_WORDS(bitSize), p_t2_2n1, VU_BITS_TO_WORDS(2UL*bitSize));
    Cy_Cryptolite_Vu_WaitForComplete (base);
    Cy_Cryptolite_Vu_Sub_Hw (base, p_t1_2n1, VU_BITS_TO_WORDS(bitSize+2UL), p_t2_2n1, VU_BITS_TO_WORDS(bitSize+2UL), p_t1_2n1, VU_BITS_TO_WORDS(bitSize+2UL));
    Cy_Cryptolite_Vu_WaitForComplete (base);
    /* Reduction. */
    Cy_Cryptolite_Vu_Cond_Sub_Hw (base, p_t1_2n1, VU_BITS_TO_WORDS(bitSize+2UL), key->moduloPtr, VU_BITS_TO_WORDS(bitSize), p_t2_2n1, VU_BITS_TO_WORDS(bitSize+1UL));
    Cy_Cryptolite_Vu_WaitForComplete (base);
    Cy_Cryptolite_Vu_Cond_Sub_Hw (base, p_t2_2n1, VU_BITS_TO_WORDS(bitSize+1UL), key->moduloPtr, VU_BITS_TO_WORDS(bitSize), rsaSignaturePtr, VU_BITS_TO_WORDS(bitSize));
    Cy_Cryptolite_Vu_WaitForComplete (base);
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Rsa_MontMul (for internal use)
****************************************************************************//**
*
* This function performs Multiplications in Montgomery domain.
*
* \param base
* Cryptolite Base address.
*
* \param destPtr
* Pointer to destination data structure.
*
* \param srcPtr1
* Pointer to source data structure.
*
* \param srcPtr2
* Pointer to source data structure.
*
* \param p_t1_2n1
* Pointer to internal buffer.
*
* \param p_t2_2n1
* Pointer to internal buffer.
*
* \param key
* Pointer to Public key and RSA coefficients.
*
* \return
* None
*
*******************************************************************************/
ATTRIBUTES_CRYPTOLITE_RSA static void Cy_Cryptolite_Rsa_MontMul (
    CRYPTOLITE_Type *base,
    uint8_t* destPtr,
    uint8_t* srcPtr1,
    uint8_t* srcPtr2,
    uint8_t* p_t1_2n1,
    uint8_t* p_t2_2n1,
    cy_stc_cryptolite_rsa_pub_key_t *key
)
{
    uint32_t bitSize = key->moduloLength;

    Cy_Cryptolite_Vu_Mul_Hw (base, srcPtr1, VU_BITS_TO_WORDS(bitSize), srcPtr2, VU_BITS_TO_WORDS(bitSize), p_t1_2n1, VU_BITS_TO_WORDS(2UL*bitSize));
    Cy_Cryptolite_Vu_WaitForComplete (base);
    Cy_Cryptolite_Vu_Mul_Hw (base, p_t1_2n1, VU_BITS_TO_WORDS(bitSize), key->rBarPtr, VU_BITS_TO_WORDS(bitSize), destPtr, VU_BITS_TO_WORDS(bitSize));
    Cy_Cryptolite_Vu_WaitForComplete (base);
    Cy_Cryptolite_Vu_Mul_Hw (base, destPtr, VU_BITS_TO_WORDS(bitSize), key->moduloPtr, VU_BITS_TO_WORDS(bitSize), p_t2_2n1, VU_BITS_TO_WORDS(2UL*bitSize));
    Cy_Cryptolite_Vu_WaitForComplete (base);
    Cy_Cryptolite_Vu_Add_Hw (base, p_t2_2n1, VU_BITS_TO_WORDS(2UL*bitSize), p_t1_2n1, VU_BITS_TO_WORDS(2UL*bitSize), p_t1_2n1, VU_BITS_TO_WORDS(2UL*bitSize+1UL));
    Cy_Cryptolite_Vu_WaitForComplete (base);
    /* Reduction */
    Cy_Cryptolite_Vu_Cond_Sub_Hw (base, &(p_t1_2n1[VU_BITS_TO_BYTES(bitSize)]), VU_BITS_TO_WORDS(bitSize+1UL), key->moduloPtr, VU_BITS_TO_WORDS(bitSize), destPtr, VU_BITS_TO_WORDS(bitSize));
    Cy_Cryptolite_Vu_WaitForComplete (base);
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Rsa_Exp (for internal use)
****************************************************************************//**
*
* This function perform modular exponentiation using coefficients.
*
* \param base
* Cryptolite Base address.
*
* \param processedDigestPtr
* Pointer to store decrypted digest data.
*
* \param rsaSignaturePtr
* Pointer to RSA signature.
*
* \param key
* Pointer to Public key and RSA coefficients.
*
* \return
* None
*
*******************************************************************************/
ATTRIBUTES_CRYPTOLITE_RSA static void Cy_Cryptolite_Rsa_Exp ( CRYPTOLITE_Type *base, uint8_t* processedDigestPtr,
                                    uint8_t* rsaSignaturePtr, cy_stc_cryptolite_rsa_pub_key_t *key)
{
    int         idx;
    uint8_t*    p;
    uint32_t    bitSize = key->moduloLength;
    uint32_t    expBitSize = key->pubExpLength;
    uint8_t*    localDigestPtr = processedDigestPtr;
    uint8_t*    p_t1_2n1  = &key->privateBuffer[0UL];
    uint8_t*    p_t2_2n1  = &key->privateBuffer[4UL*VU_BITS_TO_WORDS(2UL*bitSize+1UL)];
    uint8_t*    p_t3_n    = &key->privateBuffer[4UL*VU_BITS_TO_WORDS(2UL*bitSize+1UL) + 4UL*VU_BITS_TO_WORDS(2UL*bitSize+1UL)];

    /* Converting RSA Signature to Montgomery domain. */
    Cy_Cryptolite_Rsa_MontTransform (base, rsaSignaturePtr, p_t1_2n1, p_t2_2n1, key);

    /* Copying Converted RSA signature to output digest buffer. */
    Cy_Cryptolite_Vu_Mov_Hw (base, rsaSignaturePtr, VU_BITS_TO_WORDS(bitSize), processedDigestPtr, VU_BITS_TO_WORDS(bitSize));
    Cy_Cryptolite_Vu_WaitForComplete (base);

    if (expBitSize != 1UL)
    {
        for (idx = ((int)expBitSize-2); idx >= 0; idx--)
        {
            /* SHA digest extraction using Montgomery multiplications using public key and RSA signature. */
            Cy_Cryptolite_Rsa_MontMul (base, p_t3_n, processedDigestPtr, processedDigestPtr, p_t1_2n1, p_t2_2n1, key);

            if (0UL != Cy_Cryptolite_Rsa_GetBit (key->pubExpPtr, (uint32_t)idx))
            {
                Cy_Cryptolite_Rsa_MontMul (base, processedDigestPtr, p_t3_n, rsaSignaturePtr, p_t1_2n1, p_t2_2n1, key);
            }
            else
            {
                /* Update the processed digest pointer location. */
                p = p_t3_n; p_t3_n = processedDigestPtr; processedDigestPtr = p;
            }
        }

        /* If Montgomery based RSA signature pointer (processed digest pointer) is updated to new location
         * in previous operations, copy the updated data to local digest pointer.
         */
        if (localDigestPtr != processedDigestPtr)
        {
            Cy_Cryptolite_Vu_Mov_Hw (base, processedDigestPtr, VU_BITS_TO_WORDS(bitSize), localDigestPtr, VU_BITS_TO_WORDS(bitSize));
            Cy_Cryptolite_Vu_WaitForComplete (base);
        }
    }

    /* Converting extracted SHA digest from Montgomery domain to normal domain */
    Cy_Cryptolite_Rsa_MontInvTransform (base, localDigestPtr, p_t1_2n1, p_t2_2n1, key);
}
#if (!CY_CRYPTOLITE_RSA_PRE_CALC_COEFF_ENABLE)
/*******************************************************************************
* Function Name: Cy_Cryptolite_Rsa_Coeff (for internal use)
****************************************************************************//**
*
* This function is used to calculate Montgomery and Barrett coefficients.
*
* \param base
* Cryptolite Base address.
*
* \param key
* Pointer to Public key and RSA coefficients.
*
* \return
* None
*
*******************************************************************************/
ATTRIBUTES_CRYPTOLITE_RSA void Cy_Cryptolite_Rsa_Coeff (CRYPTOLITE_Type *base, cy_stc_cryptolite_rsa_pub_key_t *key)
{
    int         idx;
    uint8_t*    p;
    uint32_t    bitSize = key->moduloLength;
    uint8_t*    p_t1_n1    = &key->privateBuffer[0UL];
    uint8_t*    p_t2_n1    = &key->privateBuffer[4UL*VU_BITS_TO_WORDS(bitSize+1UL)];
    uint8_t*    p_dividend = p_t1_n1;
    uint32_t    sign;
    bool        even;

    Cy_Cryptolite_Rsa_Clr (p_dividend, VU_BITS_TO_WORDS(bitSize+1UL));
    Cy_Cryptolite_Rsa_SetBit (p_dividend, bitSize);
    Cy_Cryptolite_Rsa_Clr (key->barretCoefPtr, VU_BITS_TO_WORDS(bitSize+1UL));
    Cy_Cryptolite_Rsa_Clr (key->inverseModuloPtr, VU_BITS_TO_WORDS(bitSize+1UL));
    Cy_Cryptolite_Rsa_SetBit (key->inverseModuloPtr, 0UL);
    Cy_Cryptolite_Rsa_Clr (key->rBarPtr, VU_BITS_TO_WORDS(bitSize));
    Cy_Cryptolite_Vu_Sub_Hw (base, p_dividend, VU_BITS_TO_WORDS(bitSize+1UL), key->moduloPtr, VU_BITS_TO_WORDS(bitSize), p_dividend, VU_BITS_TO_WORDS(bitSize+1UL));
    Cy_Cryptolite_Vu_WaitForComplete (base);
    Cy_Cryptolite_Rsa_SetBit (key->barretCoefPtr, bitSize);
    /* Drop top bit */
    Cy_Cryptolite_Rsa_ClrBit (p_dividend, bitSize);
    Cy_Cryptolite_Vu_Lsl1_Hw (base, p_dividend, VU_BITS_TO_WORDS(bitSize+1UL), p_dividend, VU_BITS_TO_WORDS(bitSize+1UL));
    Cy_Cryptolite_Vu_WaitForComplete (base);
    for (idx = ((int)bitSize-1); idx >= 0; idx--)
    {
        Cy_Cryptolite_Vu_Sub_Hw (base, p_dividend, VU_BITS_TO_WORDS(bitSize+1UL), key->moduloPtr, VU_BITS_TO_WORDS(bitSize), p_t2_n1, VU_BITS_TO_WORDS(bitSize+1UL));
        Cy_Cryptolite_Vu_WaitForComplete (base);
        Cy_Cryptolite_Vu_Lsr1_Hw (base, key->rBarPtr, VU_BITS_TO_WORDS(bitSize), key->rBarPtr, VU_BITS_TO_WORDS(bitSize));
        Cy_Cryptolite_Vu_WaitForComplete (base);
        even = Cy_Cryptolite_Rsa_TestEven (key->inverseModuloPtr);
        if (even)
        {
            Cy_Cryptolite_Vu_Lsr1_Hw (base, key->inverseModuloPtr, VU_BITS_TO_WORDS(bitSize), key->inverseModuloPtr, VU_BITS_TO_WORDS(bitSize));
            Cy_Cryptolite_Vu_WaitForComplete (base);
        }
        else
        {
            Cy_Cryptolite_Vu_Add_Hw (base, key->inverseModuloPtr, VU_BITS_TO_WORDS(bitSize), key->moduloPtr, VU_BITS_TO_WORDS(bitSize), key->inverseModuloPtr,  VU_BITS_TO_WORDS(bitSize+1UL));
            Cy_Cryptolite_Vu_WaitForComplete (base);
            Cy_Cryptolite_Vu_Lsr1_Hw (base, key->inverseModuloPtr, VU_BITS_TO_WORDS(bitSize+1UL), key->inverseModuloPtr, VU_BITS_TO_WORDS(bitSize+1UL));
            Cy_Cryptolite_Vu_WaitForComplete (base);
            Cy_Cryptolite_Rsa_SetBit (key->rBarPtr, bitSize-1UL);
        }
        /* sign = (dividend < mod) */
        sign = Cy_Cryptolite_Rsa_GetBit (p_t2_n1, bitSize);
        if (0UL == sign)
        {
            Cy_Cryptolite_Rsa_SetBit (key->barretCoefPtr, (uint32_t)idx);
            p = p_t2_n1; p_t2_n1 = p_dividend; p_dividend = p;
        }
        /* Drop top bit */
        Cy_Cryptolite_Rsa_ClrBit (p_dividend, bitSize);
        Cy_Cryptolite_Vu_Lsl1_Hw (base, p_dividend, VU_BITS_TO_WORDS(bitSize+1UL), p_dividend, VU_BITS_TO_WORDS(bitSize+1UL));
        Cy_Cryptolite_Vu_WaitForComplete (base);
    }
}
#endif /*(!CY_CRYPTOLITE_RSA_PRE_CALC_COEFF_ENABLE) */

/*
 * This function is called from the boot loader to verify if the given signature
 * matches with the input hash (SHA digest) or not. RSA coefficient are first calculated
 * using public key modulus after which RSA exponentiation is performed.
 * RSA coefficient can be pre-calculated to save time for application validation.
 */
ATTRIBUTES_CRYPTOLITE_RSA cy_en_cryptolite_status_t Cy_Cryptolite_Rsa_Verify (
    CRYPTOLITE_Type *base,
    uint16_t digestLength,
    uint8_t *digest,
    uint8_t *rsaSignature,
    uint16_t rsaLength,
    cy_stc_cryptolite_rsa_pub_key_t *key,
    uint8_t *processedDigest
)
{
    uint32_t index;

    if ((NULL == base) || (NULL == digest) || (NULL == rsaSignature) || (NULL == key) || (NULL == processedDigest))
    {
        return CY_CRYPTOLITE_BAD_PARAMS;
    }

    if ((NULL == key->pubExpPtr) || (NULL == key->barretCoefPtr) || (NULL == key->inverseModuloPtr) ||
        (NULL == key->rBarPtr) || (NULL == key->moduloPtr) || (NULL == key->privateBuffer) ||
        (0UL == key->pubExpLength) || (0UL == key->moduloLength))
    {
        return CY_CRYPTOLITE_BAD_PARAMS;
    }

    if(VU_BITS_TO_BYTES(key->moduloLength) != rsaLength)
    {
        return  CY_CRYPTOLITE_BAD_PARAMS;
    }

#if (!CY_CRYPTOLITE_RSA_PRE_CALC_COEFF_ENABLE)
    /* Coefficient calculation. */
    if (true == key->calculateCoeff)
    {
        CRYPTOLITE_RSA_CALL_MAP(Cy_Cryptolite_Rsa_Coeff) (base, key);

        /* Set the Coefficients calculated flag to false as coefficients are calculated in previous step. */
        key->calculateCoeff = false;
    }
#endif /* (!CY_CRYPTOLITE_RSA_PRE_CALC_COEFF_ENABLE) */

    /* Modular exponent calculation using coefficients */
    Cy_Cryptolite_Rsa_Exp (base, processedDigest, rsaSignature, key);

    /*
     * Perform digest verification to validate the decrypted digest from RSA signature.
     * The output digest from SHA calculation is in BE format and output digest from
     * RSA decryption is in LE format. Hence, the processed digest is compared in reverse order
     * with input digest.
     */
    for(index = 0UL; index < digestLength; index++)
    {
        if(processedDigest[index] != (digest[digestLength - 1UL - index]))
        {
            return CY_CRYPTOLITE_RSA_VERIFY_FAILURE;
        }
    }
    return CY_CRYPTOLITE_SUCCESS;
}

#if defined(__cplusplus)
}
#endif

#endif /* (defined(CY_IP_M0S8CRYPTOLITE) && defined (CRYPTOLITE_VU_PRESENT)) */


/* [] END OF FILE */
