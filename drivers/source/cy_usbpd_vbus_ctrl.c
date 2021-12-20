/***************************************************************************//**
* \file cy_usbpd_vbus_ctrl.c
* \version 1.30
*
* The source file of the USBPD VBUS Control driver.
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
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either expressed or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#include "cy_device.h"
#include "cy_device_headers.h"

#if (defined(CY_IP_MXUSBPD) || defined(CY_IP_M0S8USBPD))

#include "cy_usbpd_common.h"
#include "cy_usbpd_defines.h"
#include "cy_usbpd_vbus_ctrl.h"
#include "cy_usbpd_typec.h"

/* Default reference voltage on M0S8-USBPD IP. */
#define PD_ADC_DEFAULT_VDDD_VOLT_MV             (3300u)

/* Reference voltage in MX_PD IP (2.0 V) */
#define MX_PD_ADC_REF_VOLT_MV                   (2000u)

/* Bandgap voltage in millivolt. */
#define PD_ADC_BAND_GAP_VOLT_MV                 (1200u)

/* Maximum ADC count. */
#define PD_ADC_MAX_VALUE                        (0xFFu)

/* Number of ADC levels. */
#define PD_ADC_NUM_LEVELS                       (PD_ADC_MAX_VALUE + 1u)

/*
 * The minimum comparator output shall be limited to this value to guard
 * against ground bounce and other ADC noise.
 */
#define PD_ADC_LEVEL_MIN_THRESHOLD              (4u)

/*
 * The maximum comparator output shall be limited to this value to guard
 * against max level.
 */
#define PD_ADC_LEVEL_MAX_THRESHOLD              (254u)

/* Loop timeout count for ADC. */
#define PD_ADC_TIMEOUT_COUNT                    (200UL)

#define VBUS_MON_DIV_20P_VAL                    (10u)   /* Multiplied by 2. */
#define VBUS_MON_DIV_10P_VAL                    (20u)   /* Multiplied by 2. */
#define VBUS_MON_DIV_9P_VAL                     (22u)   /* Multiplied by 2. */
#define VBUS_MON_DIV_8P_VAL                     (25u)   /* Multiplied by 2. */
#define VBUS_MON_DIV_6P_VAL                     (33u)   /* Multiplied by 2. */

/* Maximum voltage in mV that can be measured with 20% divider. */
#define VBUS_DIV_20_PER_MAX_VOLT                (9000U)

/* PMG1S0 enable VBUS_IN resistor divider for TYPE-C VBUS monitoring using ADC. */
#define AMUX_ADC_PMG1S0_VBUS_IN_8P_EN_POS       (7)
#define AMUX_ADC_PMG1S0_VBUS_IN_20P_EN_POS      (4)

/* PMG1S0 switch to 20% divider option at the final MUX. */
#define AMUX_ADC_PMG1S0_VBUS_DIV_2OP_EN_POS     (14)

/* EA_IREF_GAIN TRIM setting location in SFLASH. */
#define EA_IREF_GAIN_PDAC       (*(volatile uint8_t *)(0x0ffff2a4))
#define EA_IREF_GAIN_NDAC       (*(volatile uint8_t *)(0x0ffff2a5))

/* AUTO_MODE control mask for PGDO_1_CFG and PGDO_PU_1_CFG registers. */
#define PGDO_1_CFG_AUTO_SEL_MASK       (PDSS_PGDO_1_CFG_SEL_CSA_OC |\
                                        PDSS_PGDO_1_CFG_SEL_SWAP_VBUS_LESS_5_MASK |\
                                        PDSS_PGDO_1_CFG_SEL_FILT2_MASK |\
                                        PDSS_PGDO_1_CFG_SEL_CC1_OCP |\
                                        PDSS_PGDO_1_CFG_SEL_CC2_OCP |\
                                        PDSS_PGDO_1_CFG_SEL_CC1_OVP |\
                                        PDSS_PGDO_1_CFG_SEL_CC2_OVP |\
                                        PDSS_PGDO_1_CFG_SEL_SBU1_OVP_MASK |\
                                        PDSS_PGDO_1_CFG_SEL_SBU2_OVP_MASK)

#define PGDO_PU_1_CFG_AUTO_SEL_MASK     (PGDO_1_CFG_AUTO_SEL_MASK)

#define AMUX_OV_VBUS_SEL_MASK           (0x00000208)
#if defined(CY_DEVICE_CCG3PA)
#define AMUX_OV_DIV_SEL_BIT_POS         (13)
#elif defined (CY_DEVICE_PMG1S3)
#define AMUX_OV_DIV_SEL_BIT_POS         (1)
#else
#define AMUX_OV_DIV_SEL_BIT_POS         (2)
#endif /* CY_DEVICE_CCG3PA */

#define AMUX_UV_VBUS_SEL_MASK           (0x00000044)
#if defined (CY_DEVICE_PMG1S3)
#define AMUX_UV_DIV_SEL_BIT_POS         (0)
#else
#define AMUX_UV_DIV_SEL_BIT_POS         (12)
#endif /* defined(CY_DEVICE_PMG1S3) */

#define VBUS_C_20_PER_DIV               (5)       /* 20% */
#define VBUS_C_10_PER_DIV               (10)      /* 10% */
#define VBUS_C_8_PER_DIV                (12)      /* APPROX 8% */

#if defined(CY_DEVICE_CCG3)

#define VBUS_P_NGDO_EN_LV_0 (1UL << 13)
#define VBUS_P_NGDO_EN_LV_1 (1UL << 12)
#define VBUS_C_NGDO_EN_LV_0 (1UL << 3)
#define VBUS_C_NGDO_EN_LV_1 (1UL << 4)

#define VBUS_P_PLDN_EN_LV_0 (1UL << 15)
#define VBUS_P_PLDN_EN_LV_1 (1UL << 14)
#define VBUS_C_PLDN_EN_LV_0 (1UL << 5)
#define VBUS_C_PLDN_EN_LV_1 (1UL << 6)

#endif /* defined(CY_DEVICE_CCG3) */

#if (defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S))

/* 
 * Bit position in T_VCONN field for slow turn ON and 
 * enable the slow charging of 20vconn switch 
 * for inrush current control
 */
#define PDSS_VCONN20_CTRL_T_VCONN_SLOW_TURN_ON_POS      (12u)

/* VCONN OCP trim */
#define VCONN_OCP_TRIM(port) (*(volatile uint8_t *)((0x0ffff13cu) + \
        (((uint32_t)(port)) * (0x32u))))

/* VCONN OCP offset on CC2 line in steps of 4.8mA */
#define VCONN_OCP_TRIM_CC2_OFFSET       (1u)

#endif /*(defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S)) */

void Cy_USBPD_SetRefgenVoltage(cy_stc_usbpd_context_t *context, uint8_t vrefSel, uint8_t vrefSelPos);

/*******************************************************************************
* Function Name: Cy_USBPD_Adc_Init
****************************************************************************//**
*
* Initializes the PD ADC block. This function enables the PD block and the registers
* required for ADC operation. It then calibrates the ADC to identify the VDDD voltage.
* It does not start any ADC operations.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param adcId
* ADC ID. \ref cy_en_usbpd_adc_id_t
*
* \return
* CY_USBPD_STAT_SUCCESS if operation is successful,
* CY_USBPD_STAT_BAD_PARAM if the context pointer or adcID is invalid.
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Adc_Init(cy_stc_usbpd_context_t *context, cy_en_usbpd_adc_id_t adcId)
{
    PPDSS_REGS_T pd = NULL;

    if((NULL == context)
#if !defined(CY_DEVICE_CCG3)
      || (CY_USBPD_ADC_ID_OVUV == adcId)
#endif
      )
    {
        return CY_USBPD_STAT_BAD_PARAM;
    }

    pd = context->base;

#if defined(CY_DEVICE_CCG3)
    uint16_t retVal = 0U;
#endif /* defined(CY_DEVICE_CCG3) */
#if defined (CY_DEVICE_CCG3PA)
    context->vbusDetachAdcInp = CY_USBPD_ADC_INPUT_AMUX_B;
    context->vbusDetachAdcId = CY_USBPD_ADC_ID_1;
#elif(defined (CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S))
    context->vbusDetachAdcInp = CY_USBPD_ADC_INPUT_AMUX_B;
    context->vbusDetachAdcId = CY_USBPD_ADC_ID_0;
#elif defined(CY_DEVICE_CCG3)
    context->vbusDetachAdcInp = CY_USBPD_ADC_INPUT_AMUX_A;
    context->vbusDetachAdcId = CY_USBPD_ADC_ID_1;
#else
    context->vbusDetachAdcInp = CY_USBPD_ADC_INPUT_AMUX_A;
    context->vbusDetachAdcId = CY_USBPD_ADC_ID_0;
#endif /* defined (CY_DEVICE_CCG3PA) */

#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S))
    /* Enable the ADC and power it down. */
    pd->adc_ctrl[adcId] = PDSS_ADC_CTRL_ADC_ISO_N | PDSS_ADC_CTRL_PD_LV;
#endif /* defined(CY_DEVICE_CCG3PA) */

#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_CCG6DF) || defined(CY_DEVICE_PMG1S3))
    /* Enable the ADC and power it down. */
    pd->adc_ctrl = PDSS_ADC_CTRL_ADC_ISO_N | PDSS_ADC_CTRL_PD_LV;
#endif /* (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_CCG6DF) || defined(CY_DEVICE_PMG1S3)) */

#if defined(CY_DEVICE_CCG3)
    context->adcRefVddd[adcId] = true;
    context->adcVdddMv[adcId]  = PD_ADC_DEFAULT_VDDD_VOLT_MV;

    /* On PMG1S2, we use 9.09 % of VBus for voltage measurements. */
    context->vbusMonDiv        = VBUS_MON_DIV_9P_VAL;

    if (adcId == CY_USBPD_ADC_ID_1)
    {
        /* Enable the ADC and power it down. */
        pd->adc2_ctrl = PDSS_ADC2_CTRL_ADC_ISO_N | PDSS_ADC2_CTRL_PD_LV;

    }
    else
    {
        /* Enable the ADC and power it down. */
        pd->adc1_ctrl = PDSS_ADC1_CTRL_ADC_ISO_N | PDSS_ADC1_CTRL_PD_LV;
    }

    /* Calibrate the ADC before starting. */
    retVal = Cy_USBPD_Adc_Calibrate (context, adcId);
    (void)retVal;
#else
    context->adcRefVddd[adcId] = false;
    context->adcVdddMv[adcId]  = MX_PD_ADC_REF_VOLT_MV;
#if defined(CY_DEVICE_PMG1S3)
    context->vbusMonDiv        = VBUS_MON_DIV_6P_VAL;
#else
    context->vbusMonDiv        = VBUS_MON_DIV_8P_VAL;
#endif /* defined(CY_DEVICE_PMG1S3) */
#endif /* defined(CY_DEVICE_CCG3) */

    return CY_USBPD_STAT_SUCCESS;
}

/*******************************************************************************
* Function Name: Cy_USBPD_Adc_VoltToLevel
****************************************************************************//**
*
* Converts the voltage in millivolts to ADC units. It takes a 16-bit voltage
* value in millivolts and returns the corresponding 8-bit ADC reading.
* This function does not perform any ADC operations.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param adcId
* ADC ID. \ref cy_en_usbpd_adc_id_t
*
* \param volt
* Voltage in millivolt
*
* \return uint8_t
* 8-bit ADC reading corresponding to Voltage.
*
*******************************************************************************/
uint8_t Cy_USBPD_Adc_VoltToLevel(cy_stc_usbpd_context_t *context, cy_en_usbpd_adc_id_t adcId, uint16_t volt)
{
    uint32_t threshold;

    threshold = (((uint32_t)volt * PD_ADC_NUM_LEVELS) / context->adcVdddMv[adcId]);

    if (threshold < PD_ADC_LEVEL_MIN_THRESHOLD)
    {
        threshold = PD_ADC_LEVEL_MIN_THRESHOLD;
    }
    if (threshold > PD_ADC_LEVEL_MAX_THRESHOLD)
    {
        threshold = PD_ADC_LEVEL_MAX_THRESHOLD;
    }

    return (uint8_t)threshold;
}

/*******************************************************************************
* Function Name: Cy_USBPD_Adc_LevelToVolt
****************************************************************************//**
*
* Converts the ADC units to voltage in millivolts.
* It takes an 8-bit ADC reading and returns the corresponding 16-bit voltage
* value in millivolts. This function does not perform any ADC operations.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param adcId
* ADC ID. \ref cy_en_usbpd_adc_id_t
*
* \param level
* The 8-bit ADC reading.
*
* \return uint16_t
* Returns voltage in mV.
*
*******************************************************************************/
uint16_t Cy_USBPD_Adc_LevelToVolt(cy_stc_usbpd_context_t *context, cy_en_usbpd_adc_id_t adcId, uint8_t level)
{
    uint16_t threshold;

    threshold = ((level * context->adcVdddMv[adcId]) / PD_ADC_NUM_LEVELS);

    return threshold;
}

/*******************************************************************************
* Function Name: Cy_USBPD_Adc_GetVbusVolt
****************************************************************************//**
*
* Converts the ADC level to VBUS voltage in millivolts.
* It takes an 8-bit ADC reading and returns the corresponding 16-bit voltage
* value in millivolts. This function does not perform any ADC operations.
* Cy_USBPD_Adc_Sample should be called to convert a divided version of VBus to
* ADC levels before calling this function.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param adcId
* ADC ID. \ref cy_en_usbpd_adc_id_t
*
* \param level
* The 8-bit ADC reading.
*
* \return uint16_t
* Returns voltage in mV.
*
*******************************************************************************/
uint16_t Cy_USBPD_Adc_GetVbusVolt(cy_stc_usbpd_context_t *context, cy_en_usbpd_adc_id_t adcId, uint8_t level)
{
    uint16_t result;

    /* We need to multiply PD_ADC_NUM_LEVELS by 2 as vbusMonDiv has been pre-multiplied by 2. */
    result = ((level * context->adcVdddMv[adcId] * context->vbusMonDiv)/(PD_ADC_NUM_LEVELS << 1u));
    return result;
}


/*******************************************************************************
* Function Name: Cy_USBPD_Vbus_SetDetachParams
****************************************************************************//**
*
* Select the comparator block and input setting used for VBus detach
* detection. PMG1 will use the selected settings to detect Type-C disconnection
* based on removal of the VBus power.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param adcId
* ADC ID. \ref cy_en_usbpd_adc_id_t
*
* \param adcInp
* ADC input source.
*
*******************************************************************************/
void Cy_USBPD_Vbus_SetDetachParams(cy_stc_usbpd_context_t *context, cy_en_usbpd_adc_id_t adcId, cy_en_usbpd_adc_input_t adcInp)
{
    context->vbusDetachAdcInp = adcInp;
    context->vbusDetachAdcId    = adcId;
}


/*******************************************************************************
* Function Name: Cy_USBPD_Adc_FreeRunCtrl
****************************************************************************//**
*
* Configures the ADC for comparator functionality with the requested
* threshold with no interrupts enabled.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param adcId
* ADC ID.
*
* \param input
* ADC input source.
*
* \param level
* Comparator level (8-bit ADC count)
*
* \return
* \ref cy_en_usbpd_status_t
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Adc_FreeRunCtrl(cy_stc_usbpd_context_t *context, cy_en_usbpd_adc_id_t adcId,
                                            cy_en_usbpd_adc_input_t input, uint8_t level)
{
    PPDSS_REGS_T pd = context->base;
#if defined(CY_IP_MXUSBPD)
    uint32_t regVal = (uint32_t)level;

    if (context->adcRefVddd[adcId])
    {
        regVal |= PDSS_ADC_CTRL_VREF_DAC_SEL;
    }
#endif /* defined(CY_IP_MXUSBPD) */

#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S))
    /* Disable interrupts. */
    pd->intr3_mask &= (uint32_t)(~((1UL << PDSS_INTR3_CMP_OUT_CHANGED_POS) << (uint8_t)adcId));
    pd->intr3 = ((1UL << PDSS_INTR3_CMP_OUT_CHANGED_POS) << (uint8_t)adcId);

    /* Configure ADC */
    pd->adc_ctrl[adcId] = regVal | PDSS_ADC_CTRL_ADC_ISO_N |
            (((uint32_t)input << PDSS_ADC_CTRL_VSEL_POS) & PDSS_ADC_CTRL_VSEL_MASK);
#endif /* defined(CY_DEVICE_CCG3PA) */

#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_CCG6DF) || defined(CY_DEVICE_PMG1S3))
    /* Disable interrupts. */
    pd->intr3_mask &= ~PDSS_INTR3_CMP_OUT_CHANGED;
    pd->intr3 = PDSS_INTR3_CMP_OUT_CHANGED;

    /* Configure ADC */
    pd->adc_ctrl = regVal | PDSS_ADC_CTRL_ADC_ISO_N |
            (((uint32_t)input << PDSS_ADC_CTRL_VSEL_POS) & PDSS_ADC_CTRL_VSEL_MASK);
#endif /* (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_CCG6DF) || defined(CY_DEVICE_PMG1S3)) */

#if defined(CY_DEVICE_CCG3)
    if (adcId == CY_USBPD_ADC_ID_0)
    {
        /* Disable interrupts. */
        pd->intr1_mask &= ~PDSS_INTR1_CMP_OUT1_CHANGED;
        pd->intr1 = PDSS_INTR1_CMP_OUT1_CHANGED;

        pd->adc1_ctrl = (uint32_t)level | PDSS_ADC1_CTRL_ADC_ISO_N |
            (((uint32_t)input << PDSS_ADC1_CTRL_VSEL_POS) & PDSS_ADC1_CTRL_VSEL_MASK);

    }
    else
    {
        /* Disable interrupts. */
        pd->intr1_mask &= ~PDSS_INTR1_CMP_OUT2_CHANGED;
        pd->intr1 = PDSS_INTR1_CMP_OUT2_CHANGED;

        pd->adc2_ctrl = (uint32_t)level | PDSS_ADC2_CTRL_ADC_ISO_N |
            (((uint32_t)input << PDSS_ADC2_CTRL_VSEL_POS) & PDSS_ADC2_CTRL_VSEL_MASK);
    }
#endif /* defined(CY_DEVICE_CCG3) */

    /* Only one ADC supported on some devices. */
    CY_UNUSED_PARAMETER(adcId);

    return CY_USBPD_STAT_SUCCESS;
}

/*******************************************************************************
* Function Name: Cy_USBPD_Adc_CompCtrl
****************************************************************************//**
*
* Configures the ADC for comparator functionality with
* the requested threshold, and registers a callback which shall be called
* when the comparator output changes.
* This function configures the ADC block as a comparator. The function takes
* the input to be configured and the ADC comparator threshold. It also takes a
* callback. If the callback is not NULL, then the threshold is configured and
* interrupts are enabled. If the callback is NULL, then the ADC / comparator
* is set to the low power state and interrupts are disabled.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param adcId
* ADC ID.
*
* \param input
* ADC input source.
*
* \param level
* Comparator level (8-bit ADC count)
*
* \param intCfg
* Interrupt configuration
*
* \param cb
* Callback to be called on interrupt.
*
*******************************************************************************/
void Cy_USBPD_Adc_CompCtrl(cy_stc_usbpd_context_t *context, cy_en_usbpd_adc_id_t adcId, cy_en_usbpd_adc_input_t input,
                            uint8_t level, cy_en_usbpd_adc_int_t intCfg, cy_cb_adc_events_t cb)
{
    PPDSS_REGS_T pd = context->base;
    uint32_t state;
    bool out;
    uint32_t regVal = (uint32_t)level;

    state = Cy_SysLib_EnterCriticalSection();

    context->adcCb[adcId] = cb;

#if defined(CY_IP_MXUSBPD)
    if (context->adcRefVddd[adcId])
    {
        regVal |= PDSS_ADC_CTRL_VREF_DAC_SEL;
    }
#endif /* defined(CY_IP_MXUSBPD) */

#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S))
    if (cb != NULL)
    {
        pd->intr3_cfg_adc_hs[adcId] &= ~PDSS_INTR3_CFG_ADC_HS_FILT_CFG_MASK;
        pd->intr3_cfg_adc_hs[adcId] |= ((uint32_t)intCfg << PDSS_INTR3_CFG_ADC_HS_FILT_CFG_POS);
        pd->adc_ctrl[adcId] = regVal | PDSS_ADC_CTRL_ADC_ISO_N |
            (((uint32_t)input << PDSS_ADC_CTRL_VSEL_POS) & PDSS_ADC_CTRL_VSEL_MASK);
        /* Delay 10us for the input selection to stabilize. */
        Cy_SysLib_DelayUs(10);

        /* Clear comparator interrupts. */
        pd->intr3 = ((1UL << PDSS_INTR3_CMP_OUT_CHANGED_POS) << (uint8_t)adcId);

        /* Enable comparator interrupts. */
        pd->intr3_mask |= ((1UL << PDSS_INTR3_CMP_OUT_CHANGED_POS) << (uint8_t)adcId);

        if ((pd->adc_ctrl[adcId] & PDSS_ADC_CTRL_CMP_OUT) != 0U)
        {
            out = true;
        }
        else
        {
            out = false;
       }
        if (((intCfg ==  CY_USBPD_ADC_INT_FALLING) && (out == false)) ||
            ((intCfg ==  CY_USBPD_ADC_INT_RISING) && (out == true)))
        {
            /* Raise an interrupt. */
            pd->intr3_set |= ((uint32_t)PDSS_INTR3_CMP_OUT_CHANGED_POS << (uint8_t)adcId);
        }
    }
    else
    {
        /* Revert register configuration. */
        pd->adc_ctrl[adcId] = PDSS_ADC_CTRL_ADC_ISO_N | PDSS_ADC_CTRL_PD_LV;

        /* Disable interrupts. */
        pd->intr3_mask &= (~((1UL << PDSS_INTR3_CMP_OUT_CHANGED_POS) << (uint8_t)adcId));
        pd->intr3 = ((1UL << PDSS_INTR3_CMP_OUT_CHANGED_POS) << (uint8_t)adcId);
    }
#endif /* defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S) */

#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_CCG6DF) || defined(CY_DEVICE_PMG1S3))
    if (cb != NULL)
    {
        pd->intr3_cfg_adc_hs = Cy_USBPD_MmioRegUpdateField(pd->intr3_cfg_adc_hs, (uint32_t)intCfg,
                PDSS_INTR3_CFG_ADC_HS_FILT_CFG_MASK, (uint8_t)PDSS_INTR3_CFG_ADC_HS_FILT_CFG_POS);
        pd->adc_ctrl = regVal | PDSS_ADC_CTRL_ADC_ISO_N |
                        (((uint32_t)input << PDSS_ADC_CTRL_VSEL_POS) & PDSS_ADC_CTRL_VSEL_MASK);
        /* Delay 10us for the input selection to stabilize. */
        Cy_SysLib_DelayUs(10);

        /* Clear comparator interrupts. */
        pd->intr3 = PDSS_INTR3_CMP_OUT_CHANGED;

        /* Enable comparator interrupts. */
        pd->intr3_mask |= PDSS_INTR3_CMP_OUT_CHANGED;

        if ((pd->adc_ctrl & PDSS_ADC_CTRL_CMP_OUT) != 0U)
        {
            out = true;
        }
        else
        {
            out = false;
        }
        if (((intCfg ==  CY_USBPD_ADC_INT_FALLING) && (out == false)) ||
            ((intCfg ==  CY_USBPD_ADC_INT_RISING) && (out == true)))
        {
            /* Raise an interrupt. */
            pd->intr3_set |= PDSS_INTR3_CMP_OUT_CHANGED;
        }
    }
    else
    {
        /* Revert register configuration. */
        pd->adc_ctrl = PDSS_ADC_CTRL_ADC_ISO_N | PDSS_ADC_CTRL_PD_LV;

        /* Disable interrupts. */
        pd->intr3_mask &= ~(PDSS_INTR3_CMP_OUT_CHANGED);
        pd->intr3 = PDSS_INTR3_CMP_OUT_CHANGED;
    }
#endif /* (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_CCG6DF) || defined(CY_DEVICE_PMG1S3)) */

#if defined(CY_DEVICE_CCG3)
    if (cb != NULL)
    {
        if (adcId == CY_USBPD_ADC_ID_0)
        {
            /* Load the interrupt configuration. */
            regVal = pd->intr_1_cfg & ~PDSS_INTR_1_CFG_ADC1_CFG_MASK;
            pd->intr_1_cfg = ((uint32_t)intCfg << PDSS_INTR_1_CFG_ADC1_CFG_POS) | regVal;

            pd->adc1_ctrl = (uint32_t)level | PDSS_ADC1_CTRL_ADC_ISO_N |
                (((uint32_t)input << PDSS_ADC1_CTRL_VSEL_POS) & PDSS_ADC1_CTRL_VSEL_MASK);
            Cy_SysLib_DelayUs(10);

            /* Clear comparator interrupts. */
            pd->intr1 = PDSS_INTR1_CMP_OUT1_CHANGED;

            /* Enable comparator interrupts. */
            pd->intr1_mask |= PDSS_INTR1_CMP_OUT1_CHANGED;
            if ((pd->adc1_ctrl & PDSS_ADC1_CTRL_CMP_OUT) != 0U)
            {
                out = true;
            }
            else
            {
                out = false;
            }

            if (((intCfg ==  CY_USBPD_ADC_INT_FALLING) && (out == false)) ||
                    ((intCfg ==  CY_USBPD_ADC_INT_RISING) && (out == true)))
            {
                /* Raise an interrupt. */
                pd->intr1_set |= PDSS_INTR1_CMP_OUT1_CHANGED;
            }
        }
        else
        {
            /* Load the interrupt configuration. */
            regVal = pd->intr_1_cfg & ~PDSS_INTR_1_CFG_ADC2_CFG_MASK;
            pd->intr_1_cfg = ((uint32_t)intCfg << PDSS_INTR_1_CFG_ADC2_CFG_POS) | regVal;

            pd->adc2_ctrl = (uint32_t)level | PDSS_ADC2_CTRL_ADC_ISO_N |
                (((uint32_t)input << PDSS_ADC2_CTRL_VSEL_POS) & PDSS_ADC2_CTRL_VSEL_MASK);
            Cy_SysLib_DelayUs(10);

            /* Clear comparator interrupts. */
            pd->intr1 = PDSS_INTR1_CMP_OUT2_CHANGED;

            /* Enable comparator interrupts. */
            pd->intr1_mask |= PDSS_INTR1_CMP_OUT2_CHANGED;

            if ((pd->adc2_ctrl & PDSS_ADC2_CTRL_CMP_OUT) != 0U)
            {
                out = true;
            }
            else
            {
                out = false;
            }

            if (((intCfg == CY_USBPD_ADC_INT_FALLING) && (out == false)) ||
                    ((intCfg == CY_USBPD_ADC_INT_RISING) && (out == true)))
            {
                /* Raise an interrupt. */
                pd->intr1_set |= PDSS_INTR1_CMP_OUT2_CHANGED;
            }
        }
    }
    else
    {
        if (adcId == CY_USBPD_ADC_ID_0)
        {
            /* Revert register configuration. */
            pd->adc1_ctrl = PDSS_ADC1_CTRL_ADC_ISO_N | PDSS_ADC1_CTRL_PD_LV;

            /* Disable interrupts. */
            pd->intr1_mask &= ~PDSS_INTR1_CMP_OUT1_CHANGED;
            pd->intr1 = PDSS_INTR1_CMP_OUT1_CHANGED;
        }
        else
        {
            /* Revert register configuration. */
            pd->adc2_ctrl = PDSS_ADC2_CTRL_ADC_ISO_N | PDSS_ADC2_CTRL_PD_LV;

            /* Disable interrupts. */
            pd->intr1_mask &= ~PDSS_INTR1_CMP_OUT2_CHANGED;
            pd->intr1 = PDSS_INTR1_CMP_OUT2_CHANGED;
        }
    }
#endif /* defined(CY_DEVICE_CCG3) */

    /* Only one ADC supported on some devices. */
    CY_UNUSED_PARAMETER(adcId);

    Cy_SysLib_ExitCriticalSection(state);
}

#if defined(CY_IP_MXUSBPD)
static void pd_adc_restore_intr(cy_stc_usbpd_context_t *context, cy_en_usbpd_adc_id_t adcId, uint32_t regAdcCtrl)
{
    PPDSS_REGS_T pd = context->base;
    uint32_t regVal;
    bool out;
    (void)adcId;

#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S))
    /* Revert register configuration. */
    pd->adc_ctrl[adcId] = regAdcCtrl;
    Cy_SysLib_DelayUs(10);

    pd->intr3 = ((uint32_t)PDSS_INTR3_CMP_OUT_CHANGED_POS << (uint8_t)adcId);

    if (((pd->intr3_mask & ((1UL << PDSS_INTR3_CMP_OUT_CHANGED_POS) << (uint8_t)adcId)) != 0U) &&
        ((regAdcCtrl & PDSS_ADC_CTRL_PD_LV) == 0U))
    {
        regVal = (pd->intr3_cfg_adc_hs[adcId] & PDSS_INTR3_CFG_ADC_HS_FILT_CFG_MASK) >>
            PDSS_INTR3_CFG_ADC_HS_FILT_CFG_POS;

        if ((pd->adc_ctrl[adcId] & PDSS_ADC_CTRL_CMP_OUT) != 0U)
        {
            out = true;
        }
        else
        {
            out = false;
        }

        if (((regVal == (uint32_t)CY_USBPD_ADC_INT_FALLING) && (out == false)) ||
            ((regVal == (uint32_t)CY_USBPD_ADC_INT_RISING) && (out == true)))
        {
            /* Raise an interrupt. */
            pd->intr3_set |= ((1UL << PDSS_INTR3_CMP_OUT_CHANGED_POS) << (uint8_t)adcId);
        }
    }
#endif /* (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S)) */

#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_CCG6DF) || defined(CY_DEVICE_PMG1S3))
    /* Revert register configuration. */
    pd->adc_ctrl = regAdcCtrl;
    Cy_SysLib_DelayUs(10);

    pd->intr3 = PDSS_INTR3_CMP_OUT_CHANGED;

    if (((pd->intr3_mask & PDSS_INTR3_CMP_OUT_CHANGED) != 0U) &&
        ((regAdcCtrl & PDSS_ADC_CTRL_PD_LV) == 0U))
    {
        regVal = (pd->intr3_cfg_adc_hs & PDSS_INTR3_CFG_ADC_HS_FILT_CFG_MASK) >>
            PDSS_INTR3_CFG_ADC_HS_FILT_CFG_POS;

        if ((pd->adc_ctrl & PDSS_ADC_CTRL_CMP_OUT) != 0U)
        {
            out = true;
        }
        else
        {
            out = false;
        }

        if (((regVal == (uint32_t)CY_USBPD_ADC_INT_FALLING) && (out == false)) ||
            ((regVal == (uint32_t)CY_USBPD_ADC_INT_RISING) && (out == true)))
        {
            /* Raise an interrupt. */
            pd->intr3_set |= PDSS_INTR3_CMP_OUT_CHANGED;
        }
    }
#endif /* (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_CCG6DF) || defined(CY_DEVICE_PMG1S3)) */
}
#endif /* defined(CY_IP_MXUSBPD) */

/*******************************************************************************
* Function Name: Cy_USBPD_Adc_CompSample
****************************************************************************//**
*
* Temporarily configures the comparator as requested and
* checks whether the input exceeds the specified digital level.
* This function restores the comparator to its previous state after operation. This
* is useful when the comparator is already configured to function with a certain
* input and level with interrupt and another reading needs to be done without
* having to re-configure the block after the sampling.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param adcId
* ADC ID.
*
* \param input
* ADC input source.
*
* \param level
* Value(8-bit ADC count) to compare the input voltage against.
*
* \return bool
* Returns true if voltage > level, false otherwise.
*
*******************************************************************************/
bool Cy_USBPD_Adc_CompSample(cy_stc_usbpd_context_t *context, cy_en_usbpd_adc_id_t adcId,
                             cy_en_usbpd_adc_input_t input, uint8_t level)
{
    PPDSS_REGS_T pd = context->base;
    uint32_t regAdcCtrl;
    uint32_t regVal = (uint32_t)level;
    uint32_t state;
    bool compOut = true;

#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_CCG6DF) || defined(CY_DEVICE_PMG1S3))
    /* adcId is irrelevant as only one ADC is supported. */
    CY_UNUSED_PARAMETER(adcId);
#endif /* (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_CCG6DF) || defined(CY_DEVICE_PMG1S3)) */

    /* Store previous configuration and disable interrupts. */
    state = Cy_SysLib_EnterCriticalSection();

#if defined(CY_IP_MXUSBPD)
    if (context->adcRefVddd[adcId])
    {
        regVal |= PDSS_ADC_CTRL_VREF_DAC_SEL;
    }

#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S))
    regAdcCtrl = pd->adc_ctrl[adcId];

    /* Configure the input and level. */
    pd->adc_ctrl[adcId] = regVal | PDSS_ADC_CTRL_ADC_ISO_N |
            (((uint32_t)input << PDSS_ADC_CTRL_VSEL_POS) & PDSS_ADC_CTRL_VSEL_MASK);

    /* Delay 10us for the input selection to stabilize. */
    Cy_SysLib_DelayUs(10);

    if ((pd->adc_ctrl[adcId] & PDSS_ADC_CTRL_CMP_OUT) != 0U)
    {
        compOut = false;
    }
#endif /* (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S)) */

#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_CCG6DF) || defined(CY_DEVICE_PMG1S3))
    regAdcCtrl = pd->adc_ctrl;

    /* Configure the input and level. */
    pd->adc_ctrl = regVal | PDSS_ADC_CTRL_ADC_ISO_N |
            (((uint32_t)input << PDSS_ADC_CTRL_VSEL_POS) & PDSS_ADC_CTRL_VSEL_MASK);

    /* Delay 10us for the input selection to stabilize. */
    Cy_SysLib_DelayUs(10);

    if ((pd->adc_ctrl & PDSS_ADC_CTRL_CMP_OUT) != 0U)
    {
        compOut = false;
    }
#endif /* (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_CCG6DF) || defined(CY_DEVICE_PMG1S3)) */

    pd_adc_restore_intr(context, adcId, regAdcCtrl);
#endif /* defined(CY_IP_MXUSBPD) */

#if defined(CY_DEVICE_CCG3)
    if (adcId == CY_USBPD_ADC_ID_0)
    {
        regAdcCtrl = pd->adc1_ctrl;

        /* Configure the input and level. */
        pd->adc1_ctrl = (uint32_t)level | PDSS_ADC1_CTRL_ADC_ISO_N |
            (((uint32_t)input << PDSS_ADC1_CTRL_VSEL_POS) & PDSS_ADC1_CTRL_VSEL_MASK);
        Cy_SysLib_DelayUs (10);

        if ((pd->adc1_ctrl & PDSS_ADC1_CTRL_CMP_OUT) != 0U)
        {
            compOut = false;
        }

        /* Revert register configuration. */
        pd->adc1_ctrl = regAdcCtrl;
        Cy_SysLib_DelayUs (10);

        pd->intr1 = PDSS_INTR1_CMP_OUT1_CHANGED;
        if (((pd->intr1_mask & PDSS_INTR1_CMP_OUT1_CHANGED) != 0U) &&
                ((regAdcCtrl & PDSS_ADC1_CTRL_PD_LV) == 0U))
        {
            regVal = (pd->intr_1_cfg & PDSS_INTR_1_CFG_ADC1_CFG_MASK) >> PDSS_INTR_1_CFG_ADC1_CFG_POS;

            if (
                    (((pd->adc1_ctrl & PDSS_ADC1_CTRL_CMP_OUT) != 0U) && (regVal == (uint32_t)CY_USBPD_ADC_INT_RISING)) ||
                    (((pd->adc1_ctrl & PDSS_ADC1_CTRL_CMP_OUT) == 0U) && (regVal == (uint32_t)CY_USBPD_ADC_INT_FALLING))
               )
            {
                /* Raise an interrupt. */
                pd->intr1_set |= PDSS_INTR1_CMP_OUT1_CHANGED;
            }
        }
    }
    else
    {
        regAdcCtrl = pd->adc2_ctrl;

        /* Configure the input and level. */
        pd->adc2_ctrl = ((uint32_t)level | PDSS_ADC2_CTRL_ADC_ISO_N |
                (((uint32_t)input << PDSS_ADC2_CTRL_VSEL_POS) & PDSS_ADC2_CTRL_VSEL_MASK));
        Cy_SysLib_DelayUs (10);

        if ((pd->adc2_ctrl & PDSS_ADC2_CTRL_CMP_OUT) != 0U)
        {
            compOut = false;
        }

        /* Revert register configuration. */
        pd->adc2_ctrl = regAdcCtrl;
        Cy_SysLib_DelayUs(10);

        pd->intr1 = PDSS_INTR1_CMP_OUT2_CHANGED;
        if (((pd->intr1_mask & PDSS_INTR1_CMP_OUT2_CHANGED) != 0U) &&
                ((regAdcCtrl & PDSS_ADC2_CTRL_PD_LV) == 0U))
        {
            regVal = (pd->intr_1_cfg & PDSS_INTR_1_CFG_ADC2_CFG_MASK) >> PDSS_INTR_1_CFG_ADC2_CFG_POS;

            if (
                    (((pd->adc2_ctrl & PDSS_ADC2_CTRL_CMP_OUT) != 0U) && (regVal == (uint32_t)CY_USBPD_ADC_INT_RISING)) ||
                    (((pd->adc2_ctrl & PDSS_ADC2_CTRL_CMP_OUT) == 0U) && (regVal == (uint32_t)CY_USBPD_ADC_INT_FALLING))
               )
            {
                /* Raise an interrupt. */
                pd->intr1_set |= PDSS_INTR1_CMP_OUT2_CHANGED;
            }
        }
    }
#endif /* defined(CY_DEVICE_CCG3) */

    Cy_SysLib_ExitCriticalSection(state);

    return compOut;
}

/*******************************************************************************
* Function Name: Cy_USBPD_Adc_GetCompStatus
****************************************************************************//**
*
* Gets the current comparator status.
* This function does not configure the ADC / comparator. It just returns the
* current state of the comparator. If true is returned, then the input voltage
* is greater than the reference and if false, the input voltage is lower than
* the reference.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param adcId
* ADC ID.
*
* \return bool
* Returns the comparator output.
*
*******************************************************************************/
bool Cy_USBPD_Adc_GetCompStatus(cy_stc_usbpd_context_t *context, cy_en_usbpd_adc_id_t adcId)
{
    PPDSS_REGS_T pd = context->base;

    CY_UNUSED_PARAMETER(adcId);

#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S))
    if ((pd->adc_ctrl[adcId] & PDSS_ADC_CTRL_CMP_OUT) != 0U)
    {
        return false;
    }
#endif /* (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S)) */

#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_CCG6DF) || defined(CY_DEVICE_PMG1S3))
    if ((pd->adc_ctrl & PDSS_ADC_CTRL_CMP_OUT) != 0U)
    {
        return false;
    }
#endif /* (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_CCG6DF) || defined(CY_DEVICE_PMG1S3)) */

#if defined(CY_DEVICE_CCG3)
    if (adcId == CY_USBPD_ADC_ID_0)
    {
        if ((pd->adc1_ctrl & PDSS_ADC1_CTRL_CMP_OUT) != 0U)
        {
            return false;
        }
    }
    else
    {
        if ((pd->adc2_ctrl & PDSS_ADC2_CTRL_CMP_OUT) != 0U)
        {
            return false;
        }
    }
#endif /* defined(CY_DEVICE_CCG3) */

    return true;
}

/*******************************************************************************
* Function Name: Cy_USBPD_Adc_Sample
****************************************************************************//**
*
* Samples the ADC. This function enables the ADC block to function as an ADC and
* returns the sample value in ADC units. This function disables any previously
* configured comparator interrupts / settings before sampling and restores them
* after the sampling is done. If any interrupt scenario happens across the sampling,
*  the information is lost.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param adcId
* ADC ID.
*
* \param input
* ADC input source.
*
* \return uint8_t
* Returns the ADC sampled value in counts.
*
*******************************************************************************/
uint8_t Cy_USBPD_Adc_Sample(cy_stc_usbpd_context_t *context, cy_en_usbpd_adc_id_t adcId, cy_en_usbpd_adc_input_t input)
{
    PPDSS_REGS_T pd = context->base;
    uint32_t state;
    uint32_t volatile timeout = 0;
    uint8_t level = 0;
    uint32_t regAdcCtrl;
    (void)adcId;

    /* Store previous configuration and disable interrupts. */
    state = Cy_SysLib_EnterCriticalSection();

#if defined(CY_IP_MXUSBPD)
#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S))
    regAdcCtrl = pd->adc_ctrl[adcId];

    /* Clear the SAR done interrupt. */
    pd->intr0 = (1UL << (PDSS_INTR0_SAR_DONE_POS + (uint32_t)adcId));

    /* Configure the input. */
    if (context->adcRefVddd[adcId])
    {
        pd->adc_ctrl[adcId] = PDSS_ADC_CTRL_VREF_DAC_SEL | PDSS_ADC_CTRL_ADC_ISO_N |
            (((uint32_t)input << PDSS_ADC_CTRL_VSEL_POS) & PDSS_ADC_CTRL_VSEL_MASK);
    }
    else
    {
        pd->adc_ctrl[adcId] = PDSS_ADC_CTRL_ADC_ISO_N | (((uint32_t)input << PDSS_ADC_CTRL_VSEL_POS) & PDSS_ADC_CTRL_VSEL_MASK);
    }
    pd->adc_sar_ctrl[adcId] |= PDSS_ADC_SAR_CTRL_SAR_EN;

    /* Wait for SAR done interrupt status or timeout. */
    while (timeout < PD_ADC_TIMEOUT_COUNT)
    {
        if ((pd->intr0 & (1UL << (PDSS_INTR0_SAR_DONE_POS + (uint32_t)adcId))) != 0UL)
        {
            break;
        }
        timeout++;
    }

    level = (uint8_t)((pd->adc_sar_ctrl[adcId] & PDSS_ADC_SAR_CTRL_SAR_OUT_MASK) >> PDSS_ADC_SAR_CTRL_SAR_OUT_POS);

    /* Clear the SAR done interrupt. */
    pd->intr0 = (1UL << (PDSS_INTR0_SAR_DONE_POS + (uint32_t)adcId));

#endif /* (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S)) */

#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_CCG6DF) || defined(CY_DEVICE_PMG1S3))
    /* Disable the ADC interrupt output to prevent it from getting affected during the sample operation. */
    pd->intr3_cfg_adc_hs &= ~(PDSS_INTR3_CFG_ADC_HS_FILT_EN | PDSS_INTR3_CFG_ADC_HS_FILT_BYPASS);
    Cy_SysLib_DelayUs (5);

    regAdcCtrl = pd->adc_ctrl;

    /* Clear the SAR done interrupt. */
    pd->intr0 = PDSS_INTR0_SAR_DONE;

    /* Configure the input. */
    if (context->adcRefVddd[adcId])
    {
        pd->adc_ctrl = PDSS_ADC_CTRL_VREF_DAC_SEL | PDSS_ADC_CTRL_ADC_ISO_N |
            (((uint32_t)input << PDSS_ADC_CTRL_VSEL_POS) & PDSS_ADC_CTRL_VSEL_MASK);
    }
    else
    {
        pd->adc_ctrl = PDSS_ADC_CTRL_ADC_ISO_N |
            (((uint32_t)input << PDSS_ADC_CTRL_VSEL_POS) & PDSS_ADC_CTRL_VSEL_MASK);
    }
    pd->adc_sar_ctrl |= PDSS_ADC_SAR_CTRL_SAR_EN;

    /* Wait for SAR done interrupt status or timeout. */
    while (timeout < PD_ADC_TIMEOUT_COUNT)
    {
        if ((pd->intr0 & PDSS_INTR0_SAR_DONE) != 0U)
        {
            break;
        }
        timeout++;
    }

    /* Delay required between SAR_EN bit to be cleared and value to be loaded. */
    Cy_SysLib_DelayUs(2);

    level = (uint8_t)((pd->adc_sar_ctrl & PDSS_ADC_SAR_CTRL_SAR_OUT_MASK) >> PDSS_ADC_SAR_CTRL_SAR_OUT_POS);

    /* Clear the SAR done interrupt. */
    pd->intr0 = PDSS_INTR0_SAR_DONE;
#endif /* (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_CCG6DF) || defined(CY_DEVICE_PMG1S3)) */

    pd_adc_restore_intr(context, adcId, regAdcCtrl);

#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_CCG6DF) || defined(CY_DEVICE_PMG1S3))
    /* Bypass the filter on the ADC output again. */
    pd->intr3_cfg_adc_hs |= PDSS_INTR3_CFG_ADC_HS_FILT_BYPASS;
#endif /* (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_CCG6DF) || defined(CY_DEVICE_PMG1S3)) */

#endif /* defined(CY_IP_MXUSBPD) */

#if defined(CY_DEVICE_CCG3)
    uint32_t rval;

    volatile uint32_t *adc_ctrl_reg_p;
    volatile uint32_t *adc_sar_ctrl_reg_p;
    uint32_t           adc_intr_mask;
    uint32_t           adc_intr_cfg;
    uint32_t           adc_intr_cfg_pos;

    if (adcId == CY_USBPD_ADC_ID_0)
    {
        adc_ctrl_reg_p     = &(pd->adc1_ctrl);
        adc_sar_ctrl_reg_p = &(pd->adc1_sar_ctrl);
        adc_intr_mask      = PDSS_INTR1_CMP_OUT1_CHANGED;
        adc_intr_cfg       = PDSS_INTR_1_CFG_ADC1_CFG_MASK;
        adc_intr_cfg_pos   = PDSS_INTR_1_CFG_ADC1_CFG_POS;
    }
    else
    {
        adc_ctrl_reg_p     = &(pd->adc2_ctrl);
        adc_sar_ctrl_reg_p = &(pd->adc2_sar_ctrl);
        adc_intr_mask      = PDSS_INTR1_CMP_OUT2_CHANGED;
        adc_intr_cfg       = PDSS_INTR_1_CFG_ADC2_CFG_MASK;
        adc_intr_cfg_pos   = PDSS_INTR_1_CFG_ADC2_CFG_POS;
    }

    /* Store previous configuration. */
    regAdcCtrl = *adc_ctrl_reg_p;

    /* Configure the input. */
    *adc_ctrl_reg_p = PDSS_ADC1_CTRL_ADC_ISO_N |
        (((uint32_t)input << PDSS_ADC1_CTRL_VSEL_POS) & PDSS_ADC1_CTRL_VSEL_MASK);
    *adc_sar_ctrl_reg_p |= PDSS_ADC1_SAR_CTRL_SAR_EN;

    /* Wait for sampling to complete or timeout. */
    while (timeout < PD_ADC_TIMEOUT_COUNT)
    {
        if ((*adc_sar_ctrl_reg_p & PDSS_ADC1_SAR_CTRL_SAR_EN) == 0U)
        {
            break;
        }
        timeout++;
    }

    /* Delay required between SAR_EN bit to be cleared and value to be loaded. */
    Cy_SysLib_DelayUs (2);

    level = (uint8_t)((*adc_sar_ctrl_reg_p & PDSS_ADC1_SAR_CTRL_SAR_OUT_MASK) >>
            PDSS_ADC1_SAR_CTRL_SAR_OUT_POS);

    /* Revert register configuration. */
    *adc_ctrl_reg_p = regAdcCtrl;
    Cy_SysLib_DelayUs (10);
    pd->intr1 = adc_intr_mask;

    if (((pd->intr1_mask & adc_intr_mask) != 0U) && ((regAdcCtrl & PDSS_ADC1_CTRL_PD_LV) == 0U))
    {
        rval = (pd->intr_1_cfg & adc_intr_cfg) >> adc_intr_cfg_pos;

        if (
                (((*adc_ctrl_reg_p & PDSS_ADC1_CTRL_CMP_OUT) != 0U) && (rval == (uint32_t)CY_USBPD_ADC_INT_RISING)) ||
                (((*adc_ctrl_reg_p & PDSS_ADC1_CTRL_CMP_OUT) == 0U) && (rval == (uint32_t)CY_USBPD_ADC_INT_FALLING))
           )
        {
            /* Raise an interrupt. */
            pd->intr1_set |= adc_intr_mask;
        }
    }
#endif /* defined(CY_DEVICE_CCG3) */

    Cy_SysLib_ExitCriticalSection(state);

    return level;
}

/*******************************************************************************
* Function Name: Cy_USBPD_Adc_Calibrate
****************************************************************************//**
*
* Calibrates the specified ADC for operation.
* This function calibrates the specified ADC by identifying the VDDD voltage
* for reference. It should be noted that by calling the function, the
* previously calculated threshold levels may have to be changed based on the
* VDDD reading. The VDDD level is calculated based on the bandgap voltage
* which is expected to be constant.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param adcId
* ADC ID.
*
* \return uint16_t
* Returns the VDDD value in mV after calibration.
*
*******************************************************************************/
uint16_t Cy_USBPD_Adc_Calibrate(cy_stc_usbpd_context_t *context, cy_en_usbpd_adc_id_t adcId)
{
    uint8_t  level;
    uint32_t threshold;

#if defined(CY_IP_MXUSBPD)
    if (context->adcRefVddd[adcId])
#endif /* defined(CY_IP_MXUSBPD) */
    {
        level = Cy_USBPD_Adc_Sample(context, adcId, CY_USBPD_ADC_INPUT_BANDGAP);

        /* Check for zero. If level came out as zero, then do not calculate. */
        if (level != 0U)
        {
            threshold = ((PD_ADC_BAND_GAP_VOLT_MV * PD_ADC_NUM_LEVELS) / (uint32_t)level);
            context->adcVdddMv[adcId] = (uint16_t)threshold;
        }
    }

    return context->adcVdddMv[adcId];
}

/*******************************************************************************
* Function Name: Cy_USBPD_Adc_SelectVref
****************************************************************************//**
*
* Selects the reference voltage used by the ADC block
* on the PMG1 device. A 2.0 V supply generated by the RefGen block in the PD
* IP is used as ADC reference by default. This API can be used to select
* the VDDD supply as the ADC reference. This is useful in cases where voltages
* greater than 2.0 V need to be measured.
*
* Note: Since the VDDD supply level could vary across time, the ADC volts
* per division value needs to be calibrated before taking any ADC readings.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param adcId
* ADC ID.
*
* \param vrefSel
* ADC reference selection.
*
* \return
* \ref cy_en_usbpd_status_t
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Adc_SelectVref(cy_stc_usbpd_context_t *context, cy_en_usbpd_adc_id_t adcId,
                                             cy_en_usbpd_adc_vref_t vrefSel)
{
#if defined(CY_IP_M0S8USBPD)
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(adcId);
    CY_UNUSED_PARAMETER(vrefSel);

    return CY_USBPD_STAT_INVALID_COMMAND;
#else
    PPDSS_REGS_T pd = context->base;
    uint16_t retVal = 0U;
    CY_UNUSED_PARAMETER(pd);

    if (vrefSel == CY_USBPD_ADC_VREF_VDDD)
    {
#if defined(CY_DEVICE_CCG3PA)
        /* Enable the ADC and power it down. */
        pd->adc_ctrl[adcId] |= PDSS_ADC_CTRL_VREF_DAC_SEL;
#endif /* defined(CY_DEVICE_CCG3PA) */

#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_CCG6DF) || defined(CY_DEVICE_PMG1S3))
        /* Enable the ADC and power it down. */
        pd->adc_ctrl |= PDSS_ADC_CTRL_VREF_DAC_SEL;
#endif /* (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_CCG6DF) || defined(CY_DEVICE_PMG1S3)) */

        context->adcRefVddd[adcId] = true;
        retVal = Cy_USBPD_Adc_Calibrate (context, adcId);
        CY_UNUSED_PARAMETER(retVal);
    }
    else
    {
#if defined(CY_DEVICE_CCG3PA)
        /* Enable the ADC and power it down. */
        pd->adc_ctrl[adcId] &= ~PDSS_ADC_CTRL_VREF_DAC_SEL;
#endif /* defined(CY_DEVICE_CCG3PA) */

#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_CCG6DF) || defined(CY_DEVICE_PMG1S3))
        /* Enable the ADC and power it down. */
        pd->adc_ctrl &= ~PDSS_ADC_CTRL_VREF_DAC_SEL;
#endif /* (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_CCG6DF) || defined(CY_DEVICE_PMG1S3)) */

        context->adcRefVddd[adcId] = false;
        context->adcVdddMv[adcId]  = MX_PD_ADC_REF_VOLT_MV;
    }

    return CY_USBPD_STAT_SUCCESS;
#endif /* CY_IP */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Adc_GetVbusLevel
****************************************************************************//**
*
* Gets the ADC level that corresponds to the actual
* voltage on vbus. It also takes into account the VBus monitor divider.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param adcId
* ADC ID.
*
* \param volt
* Voltage in 50mV units.
*
* \param per
* Percentage margin on the voltage.
*
* \return uint8_t
* Returns the ADC level.
*
*******************************************************************************/
uint8_t Cy_USBPD_Adc_GetVbusLevel(cy_stc_usbpd_context_t *context, cy_en_usbpd_adc_id_t adcId,
        uint16_t volt, int8_t per)
{
    int32_t threshold = ((int32_t)volt + (((int32_t)volt * (int32_t)per) / 100));

    /* Remove negative numbers. */
    if (threshold < 0)
    {
        threshold = 0;
    }

    /* Convert volts to ADC units. */
    return Cy_USBPD_Adc_VoltToLevel(context, adcId, (uint16_t)(2U * (uint32_t)threshold / context->vbusMonDiv));
}

/*******************************************************************************
* Function Name: Cy_USBPD_Vbus_Mon_SetDivider
****************************************************************************//**
*
* Specify the voltage division ratio between the voltage applied
* on VBUS_MON and the actual VBus voltage. The commonly used resistor divider
* ratio used is 1:10, giving a voltage division ratio of 1/11.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param divider
* Divider Ratio between VBUS_MON voltage and VBus voltage.
*
*******************************************************************************/
void Cy_USBPD_Vbus_Mon_SetDivider(cy_stc_usbpd_context_t *context, uint8_t divider)
{
    context->vbusMonDiv = divider;
}


/*******************************************************************************
* Function Name: Cy_USBPD_Adc_MeasureVbusIn
****************************************************************************//**
*
* Measure the current voltage on the VBus-in.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param adcId
* ADC ID.
*
* \param input
* ADC input source.
*
* \return uint16_t
* VBus voltage in mV units.
*
*******************************************************************************/
uint16_t Cy_USBPD_Adc_MeasureVbusIn(cy_stc_usbpd_context_t *context, cy_en_usbpd_adc_id_t adcId,
                                    cy_en_usbpd_adc_input_t input)
{
    uint16_t retVal;

#if defined(CY_DEVICE_CCG3PA)
    uint32_t state;
    PPDSS_REGS_T pd = context->base;
    uint8_t level;

    state = Cy_SysLib_EnterCriticalSection();

/* Select VBUS_IN for ADC. VBUS_C in case of flipped FET control */
#if PMG1_FLIPPED_FET_CTRL
    pd->amux_nhv_ctrl &= ~((1U << AMUX_ADC_PMG1S0_VBUS_IN_8P_EN_POS) |
        (1U << AMUX_ADC_PMG1S0_VBUS_IN_20P_EN_POS));
#else /* PMG1_FLIPPED_FET_CTRL */
    pd->amux_nhv_ctrl |= ((1U << AMUX_ADC_PMG1S0_VBUS_IN_8P_EN_POS) |
        (1U << AMUX_ADC_PMG1S0_VBUS_IN_20P_EN_POS));
#endif /* PMG1_FLIPPED_FET_CTRL */

    Cy_SysLib_DelayUs(20);

    /* Sample the VBus voltage using ADC. */
    level = Cy_USBPD_Adc_Sample(context, adcId, input);
    retVal = Cy_USBPD_Adc_GetVbusVolt(context, adcId, level);

    /*
     * NOTE: Currently accuracy of VBUS_IN measurement is required only for
     * direct feedback implementation. Other cases can remain using the
     * 8% divider. Switching to 20% divider takes considerably more time
     * and does not warrant locking the interrupts for that long.
     */
    /*
     * At low voltages, you may require higher accuracy. This can be done
     * by moving to a lower divider value of 20%. Do this only if required.
     */
    if (retVal < VBUS_DIV_20_PER_MAX_VOLT)
    {
        /* First set the AMUX to use 20% divider. */

        pd->amux_nhv_ctrl |= (1UL << AMUX_ADC_PMG1S0_VBUS_DIV_2OP_EN_POS);
        Cy_USBPD_Vbus_Mon_SetDivider(context, VBUS_MON_DIV_20P_VAL);
        Cy_SysLib_DelayUs(10);
        /* Now measure the voltage. */
        level = Cy_USBPD_Adc_Sample(context, adcId, input);
        retVal = Cy_USBPD_Adc_GetVbusVolt(context, adcId, level);

        /* Restore the divider value. */
        pd->amux_nhv_ctrl &= ~(1UL << AMUX_ADC_PMG1S0_VBUS_DIV_2OP_EN_POS);
        Cy_USBPD_Vbus_Mon_SetDivider(context, VBUS_MON_DIV_8P_VAL);
        Cy_SysLib_DelayUs(10);

        /*
         * NOTE: Currently ADC interrupt is not enabled when running when
         * sampling the VBUS. If this changes, the code has to be extended to
         * save and restore the status to prevent falsely triggering interrupts.
         */
    }

/* Deselect VBUS_IN for ADC. VBUS_C in case of flipped FET control */
#if PMG1_FLIPPED_FET_CTRL
    pd->amux_nhv_ctrl |= ((1UL << AMUX_ADC_PMG1S0_VBUS_IN_8P_EN_POS) |
        (1UL << AMUX_ADC_PMG1S0_VBUS_IN_20P_EN_POS));
#else /* PMG1_FLIPPED_FET_CTRL */
    pd->amux_nhv_ctrl &= ~((1UL << AMUX_ADC_PMG1S0_VBUS_IN_8P_EN_POS) |
        (1UL << AMUX_ADC_PMG1S0_VBUS_IN_20P_EN_POS));
#endif /* PMG1_FLIPPED_FET_CTRL */

    Cy_SysLib_DelayUs(20);

    Cy_SysLib_ExitCriticalSection(state);
#else
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(adcId);
    CY_UNUSED_PARAMETER(input);

    retVal = 0U;
#endif /* defined(CY_DEVICE_CCG3PA) */

    return retVal;
}

/*******************************************************************************
* Function Name: Cy_USBPD_Adc_MeasureVbus
****************************************************************************//**
*
* Measure the current voltage on the VBus-Supply.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param adcId
* ADC ID.
*
* \param input
* ADC input source.
*
* \return uint16_t
* VBus voltage in mV units.
*
*******************************************************************************/
uint16_t Cy_USBPD_Adc_MeasureVbus(cy_stc_usbpd_context_t *context, cy_en_usbpd_adc_id_t adcId,
                                  cy_en_usbpd_adc_input_t input)
{
    /* Sample the VBus voltage using ADC. */
    uint8_t level;
    uint16_t retVal;

    retVal = Cy_USBPD_Adc_Calibrate(context, adcId);
    level = Cy_USBPD_Adc_Sample(context, adcId, input);
    retVal = Cy_USBPD_Adc_GetVbusVolt(context, adcId, level);

    return retVal;
}

/*******************************************************************************
* Function Name: Cy_USBPD_Adc_IntrHandler
****************************************************************************//**
*
* ADC Comparator Interrupt Handler function.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
*******************************************************************************/
void Cy_USBPD_Adc_IntrHandler(cy_stc_usbpd_context_t *context)
{
    PPDSS_REGS_T pd = context->base;
    bool compOut = true;

#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S))
    if ((pd->intr3_masked & (1U << PDSS_INTR3_CMP_OUT_CHANGED_POS)) != 0U)
    {
        pd->intr3_mask &= ~(1U << PDSS_INTR3_CMP_OUT_CHANGED_POS);
        pd->intr3       =  (1U << PDSS_INTR3_CMP_OUT_CHANGED_POS);

        /* Check status. */
        compOut = true;
        if ((pd->adc_ctrl[CY_USBPD_ADC_ID_0] & PDSS_ADC_CTRL_CMP_OUT) != 0U)
        {
            compOut = false;
        }

        /* Report status. */
        if (context->adcCb[CY_USBPD_ADC_ID_0] != NULL)
        {
            context->adcCb[CY_USBPD_ADC_ID_0](context, compOut);
        }
    }

    if ((pd->intr3_masked & ((1U << PDSS_INTR3_CMP_OUT_CHANGED_POS) << CY_USBPD_ADC_ID_1)) != 0U)
    {
        pd->intr3_mask &= ~((1U << PDSS_INTR3_CMP_OUT_CHANGED_POS) << CY_USBPD_ADC_ID_1);
        pd->intr3       =  ((1U << PDSS_INTR3_CMP_OUT_CHANGED_POS) << CY_USBPD_ADC_ID_1);

        /* Check status. */
        compOut = true;
        if ((pd->adc_ctrl[CY_USBPD_ADC_ID_1] & PDSS_ADC_CTRL_CMP_OUT) != 0U)
        {
            compOut = false;
        }

        /* Report status. */
        if (context->adcCb[CY_USBPD_ADC_ID_1] != NULL)
        {
           context->adcCb[CY_USBPD_ADC_ID_1](context, compOut);
        }
    }
#elif (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_CCG6DF) || defined(CY_DEVICE_PMG1S3))
    if ((pd->intr3_masked & PDSS_INTR3_CMP_OUT_CHANGED) != 0U)
    {
        pd->intr3_mask &= ~PDSS_INTR3_CMP_OUT_CHANGED;
        pd->intr3       =  PDSS_INTR3_CMP_OUT_CHANGED;

        /* Check status. */
        compOut = true;
        if ((pd->adc_ctrl & PDSS_ADC_CTRL_CMP_OUT) != 0U)
        {
            compOut = false;
        }

        /* Report status. */
        if (context->adcCb[CY_USBPD_ADC_ID_0] != NULL)
        {
            context->adcCb[CY_USBPD_ADC_ID_0](context, compOut);
        }
    }
#elif (defined(CY_DEVICE_CCG3))
    if ((pd->intr1_masked & PDSS_INTR1_CMP_OUT1_CHANGED) != 0U)
    {
        /* Disable the interrupt. */
        pd->intr1_mask &= ~PDSS_INTR1_CMP_OUT1_CHANGED;

        /* Check status. */
        compOut = true;
        if ((pd->adc1_ctrl & PDSS_ADC1_CTRL_CMP_OUT) != 0U)
        {
            compOut = false;
        }

        /* Report status. */
        if (context->adcCb[CY_USBPD_ADC_ID_0] != NULL)
        {
            context->adcCb[CY_USBPD_ADC_ID_0](context, compOut);
        }
    }

    if ((pd->intr1_masked & PDSS_INTR1_CMP_OUT2_CHANGED) != 0U)
    {
        /* Disable the interrupt. */
        pd->intr1_mask &= ~PDSS_INTR1_CMP_OUT2_CHANGED;

        /* Check status. */
        compOut = true;
        if ((pd->adc2_ctrl & PDSS_ADC2_CTRL_CMP_OUT) != 0U)
        {
            compOut = false;
        }

        /* Report status. */
        if (context->adcCb[CY_USBPD_ADC_ID_1] != NULL)
        {
            context->adcCb[CY_USBPD_ADC_ID_1](context, compOut);
        }
    }
#endif /* (defined(CY_DEVICE_CCG3)) */
}


/*******************************************************************************
* Function Name: Cy_USBPD_V5V_IsSupplyOn
****************************************************************************//**
*
* Checks whether the 5V supply to provide VConn power is present.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \return bool
* Returns true if 5V supply is present, false otherwise.
*
*******************************************************************************/
bool Cy_USBPD_V5V_IsSupplyOn(cy_stc_usbpd_context_t *context)
{
    bool stat = false;

#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_CCG6DF) || defined(CY_DEVICE_PMG1S3))
    PPDSS_REGS_T pd = context->base;

    /* If V5V is not present, return error. */
    if ((pd->intr1_status & PDSS_INTR1_STATUS_V5V_STATUS) != 0U)
    {
        stat = true;
    }

#elif defined(CY_DEVICE_CCG3)
    PPDSS_REGS_T pd = context->base;

    /* If V5V is not present, return error. */
    if ((pd->status & PDSS_STATUS_V5V_STATUS) != 0U)
    {
        stat = true;
    }
#else
    CY_UNUSED_PARAMETER(context);
#endif /* defined(CY_DEVICE_CCG3) */

    return stat;
}


/*******************************************************************************
* Function Name: Cy_USBPD_Vconn_Enable
****************************************************************************//**
*
* Turns on Vconn for the specified channel.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param channel
* Channel index, where CC1 = 0, CC2 = 1.
*
* \return
* \ref cy_en_usbpd_status_t
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Vconn_Enable(cy_stc_usbpd_context_t *context, uint8_t channel)
{
#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_CCG6DF) || defined(CY_DEVICE_PMG1S3) || defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S))
    PPDSS_REGS_T pd = context->base;
    uint32_t regVal;

#if PMG1_V5V_CHANGE_DETECT
    /* If V5V is not present, return error. */
    if ((pd->intr1_status & PDSS_INTR1_STATUS_V5V_STATUS) == 0U)
    {
        return CY_USBPD_STAT_FAILURE;
    }
#endif /* PMG1_V5V_CHANGE_DETECT */

    /* Check whether VConn has already been turned ON. */
    if (Cy_USBPD_Vconn_IsPresent(context, channel))
    {
        return CY_USBPD_STAT_SUCCESS;
    }
   
#if (defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S))
    PPDSS_TRIMS_REGS_T trimRegs = context->trimsBase;
     /*
     * CDT 360166:
     * Enable VCONN SCP in HW cut off and retry mode initially
     * until VCONN switch is ON. This is to cut off sudden load on VCONN that 
     * can cause Regulator Inrush fault.
     * This protection is enabled by default to address faulty active cables.
     */
    pd->vconn20_ctrl |= ((uint32_t)1u << (7u + PDSS_VCONN20_CTRL_T_VCONN_POS));

    if (channel == CY_PD_CC_CHANNEL_1)
    {
        /* Revert OCP trim offset */
        REG_FIELD_UPDATE(trimRegs->trim_bb_20vconn_2, 
            PDSS_TRIM_BB_20VCONN_2_BB_20VCONN_OCP_TRIM, 
            VCONN_OCP_TRIM(context->port));

        /* Enable SC detection on CC1/2 pins. */
        pd->vconn20_ctrl &= ~PDSS_VCONN20_CTRL_EN_OCP_CC2;
        pd->vconn20_ctrl |= PDSS_VCONN20_CTRL_EN_OCP_CC1;
    }
    else
    {
        /* Set OCP trim offset for CC2 line only */
        REG_FIELD_UPDATE(trimRegs->trim_bb_20vconn_2, 
            PDSS_TRIM_BB_20VCONN_2_BB_20VCONN_OCP_TRIM, 
            (uint32_t)VCONN_OCP_TRIM(context->port) + (uint32_t)VCONN_OCP_TRIM_CC2_OFFSET);

        /* Enable SC detection on CC1/2 pins. */
        pd->vconn20_ctrl &= ~PDSS_VCONN20_CTRL_EN_OCP_CC1;
        pd->vconn20_ctrl |= PDSS_VCONN20_CTRL_EN_OCP_CC2;
    }

#endif /* (defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S)) */

    /* Turn on the VConn switch. */
    if (channel == CY_PD_CC_CHANNEL_1)
    {
        regVal = pd->vconn20_cc1_switch_1_ctrl;
        regVal |= PDSS_VCONN20_CC1_SWITCH_1_CTRL_SEL_ON_OFF |
            PDSS_VCONN20_CC1_SWITCH_1_CTRL_EN_SWITCH_CC1_ON_VALUE |
            PDSS_VCONN20_CC1_SWITCH_1_CTRL_SEL_CC1_OVP |
            PDSS_VCONN20_CC1_SWITCH_1_CTRL_SEL_CC2_OVP;
        pd->vconn20_cc1_switch_1_ctrl = regVal;

        /* Reset edge detector. */
        pd->vconn20_cc1_switch_1_ctrl |= PDSS_VCONN20_CC1_SWITCH_1_CTRL_RST_EDGE_DET;
        pd->vconn20_cc1_switch_1_ctrl &= ~PDSS_VCONN20_CC1_SWITCH_1_CTRL_RST_EDGE_DET;
    }
    else
    {
        regVal = pd->vconn20_cc2_switch_1_ctrl;
        regVal |= PDSS_VCONN20_CC2_SWITCH_1_CTRL_SEL_ON_OFF |
            PDSS_VCONN20_CC2_SWITCH_1_CTRL_EN_SWITCH_CC2_ON_VALUE |
            PDSS_VCONN20_CC2_SWITCH_1_CTRL_SEL_CC1_OVP |
            PDSS_VCONN20_CC2_SWITCH_1_CTRL_SEL_CC2_OVP;
        pd->vconn20_cc2_switch_1_ctrl = regVal;

        /* Reset edge detector. */
        pd->vconn20_cc2_switch_1_ctrl |= PDSS_VCONN20_CC2_SWITCH_1_CTRL_RST_EDGE_DET;
        pd->vconn20_cc2_switch_1_ctrl &= ~PDSS_VCONN20_CC2_SWITCH_1_CTRL_RST_EDGE_DET;
    }

    /* Turn on the VConn pump. */
    regVal = pd->vconn20_pump_en_1_ctrl;
    regVal |= PDSS_VCONN20_PUMP_EN_1_CTRL_SEL_ON_OFF | PDSS_VCONN20_PUMP_EN_1_CTRL_EN_VCONN20_PUMP_ON_VALUE |
        PDSS_VCONN20_PUMP_EN_1_CTRL_SEL_CC1_OVP | PDSS_VCONN20_PUMP_EN_1_CTRL_SEL_CC2_OVP;
    pd->vconn20_pump_en_1_ctrl = regVal;

#if (defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S))
     /* 
     * Turn ON & enable the slow charging of 20vconn switch 
     * for inrush current control.
     */
    Cy_SysLib_DelayUs(100);

    /* Turn on VCONN slow switch */
    pd->vconn20_ctrl |= ((uint32_t)1u << (PDSS_VCONN20_CTRL_T_VCONN_SLOW_TURN_ON_POS +
        PDSS_VCONN20_CTRL_T_VCONN_POS));

    /* Start firmware VCONN OCP and SCP handling after VCONN switch output is stable */   
#endif /* (defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S)) */

    return CY_USBPD_STAT_SUCCESS;

#elif defined(CY_DEVICE_CCG3)
    PPDSS_REGS_T pd = context->base;

    if (channel == CY_PD_CC_CHANNEL_1)
    {
        pd->pfet300_ctrl |= PDSS_PFET300_CTRL_EN_SWITCH_CC1;
    }
    else
    {
        pd->pfet300_ctrl |= PDSS_PFET300_CTRL_EN_SWITCH_CC2;
    }

    return CY_USBPD_STAT_SUCCESS;
#else
    /* No VConn support in PMG1S0. */
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(channel);

    return CY_USBPD_STAT_NOT_SUPPORTED;
#endif /* defined(CY_DEVICE_CCG3) */
}


/*******************************************************************************
* Function Name: Cy_USBPD_Vconn_Disable
****************************************************************************//**
*
* Turns Off Vconn for the specified channel.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param channel
* Channel index, where CC1 = 0, CC2 = 1.
*
* \return
* \ref cy_en_usbpd_status_t
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Vconn_Disable(cy_stc_usbpd_context_t *context, uint8_t channel)
{
#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_CCG6DF) || defined(CY_DEVICE_PMG1S3) || defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S))

    PPDSS_REGS_T pd = context->base;
    uint32_t regVal;

    /* Check whether VConn has already been turned OFF. */
    if (!Cy_USBPD_Vconn_IsPresent(context, channel))
    {
        return CY_USBPD_STAT_SUCCESS;
    }

    /* PMG1S3 */
#if defined(CY_DEVICE_PMG1S3)
    /* Gate pull up needs to be disabled. */
    pd->vconn20_ctrl &= ~(0x01UL << (0x05UL + PDSS_VCONN20_CTRL_T_VCONN_POS));
#elif (defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S))
    /* Turn off VCONN slow switch*/
    pd->vconn20_ctrl &= ~((uint32_t)1u << (PDSS_VCONN20_CTRL_T_VCONN_SLOW_TURN_ON_POS + PDSS_VCONN20_CTRL_T_VCONN_POS));
#endif /* defined(CY_DEVICE_PMG1S3) */

    /* Turn off the VConn pump. */
    regVal = pd->vconn20_pump_en_1_ctrl;
    regVal &= ~(PDSS_VCONN20_PUMP_EN_1_CTRL_SEL_ON_OFF | PDSS_VCONN20_PUMP_EN_1_CTRL_EN_VCONN20_PUMP_ON_VALUE |
            PDSS_VCONN20_PUMP_EN_1_CTRL_SEL_CC1_OVP | PDSS_VCONN20_PUMP_EN_1_CTRL_SEL_CC2_OVP);
    pd->vconn20_pump_en_1_ctrl = regVal;

    /* Adding a small delay. */
    Cy_SysLib_DelayUs (10);

    /* Turn off the VConn switch. */
    if (channel == CY_PD_CC_CHANNEL_1)
    {
        regVal = pd->vconn20_cc1_switch_1_ctrl;
        regVal &= ~(PDSS_VCONN20_CC1_SWITCH_1_CTRL_SEL_ON_OFF | PDSS_VCONN20_CC1_SWITCH_1_CTRL_EN_SWITCH_CC1_ON_VALUE |
                PDSS_VCONN20_CC1_SWITCH_1_CTRL_SEL_CC1_OVP);
        pd->vconn20_cc1_switch_1_ctrl = regVal;

        /* Reset edge detector. */
        pd->vconn20_cc1_switch_1_ctrl |= PDSS_VCONN20_CC1_SWITCH_1_CTRL_RST_EDGE_DET;
        pd->vconn20_cc1_switch_1_ctrl &= ~PDSS_VCONN20_CC1_SWITCH_1_CTRL_RST_EDGE_DET;
    }
    else
    {
        regVal = pd->vconn20_cc2_switch_1_ctrl;
        regVal &= ~(PDSS_VCONN20_CC2_SWITCH_1_CTRL_SEL_ON_OFF | PDSS_VCONN20_CC2_SWITCH_1_CTRL_EN_SWITCH_CC2_ON_VALUE |
                PDSS_VCONN20_CC2_SWITCH_1_CTRL_SEL_CC2_OVP);
        pd->vconn20_cc2_switch_1_ctrl = regVal;

        /* Reset edge detector. */
        pd->vconn20_cc2_switch_1_ctrl |= PDSS_VCONN20_CC2_SWITCH_1_CTRL_RST_EDGE_DET;
        pd->vconn20_cc2_switch_1_ctrl &= ~PDSS_VCONN20_CC2_SWITCH_1_CTRL_RST_EDGE_DET;
    }

    return CY_USBPD_STAT_SUCCESS;

#elif defined(CY_DEVICE_CCG3)
    PPDSS_REGS_T pd = context->base;

    if (channel == CY_PD_CC_CHANNEL_1)
    {
        pd->pfet300_ctrl &= ~PDSS_PFET300_CTRL_EN_SWITCH_CC1;
    }
    else
    {
        pd->pfet300_ctrl &= ~PDSS_PFET300_CTRL_EN_SWITCH_CC2;
    }

    return CY_USBPD_STAT_SUCCESS;
#else
    /* No VConn support in PMG1S0. */
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(channel);

    return CY_USBPD_STAT_NOT_SUPPORTED;
#endif /* defined(CY_DEVICE_CCG3) */
}


/*******************************************************************************
* Function Name: Cy_USBPD_Vconn_GatePullUp_Enable
****************************************************************************//**
*
* Enable the gate pull up to limit the inrush current. This function needs to
* be called after at least 500 us of \ref Cy_USBPD_Vconn_Enable
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_Vconn_GatePullUp_Enable(cy_stc_usbpd_context_t *context)
{
#if defined(CY_DEVICE_PMG1S3)
    PPDSS_REGS_T pd = context->base;
    /* Gate pull up needs to be enabled after 500 us of VConn Switch Enable to
     * limit inrush current. */

    pd->vconn20_ctrl |= (0x01UL << (0x05UL + PDSS_VCONN20_CTRL_T_VCONN_POS));
#else
    (void)context;
#endif /* defined(CY_DEVICE_PMG1S3) */
}


/*******************************************************************************
* Function Name: Cy_USBPD_Vconn_IsPresent
****************************************************************************//**
*
* This function checks the status of the VConn.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param channel
* Channel index, where CC1 = 0, CC2 = 1.
*
* \return bool
* Returns true if Vconn is turned on, false otherwise.
*
*******************************************************************************/
bool Cy_USBPD_Vconn_IsPresent(cy_stc_usbpd_context_t *context, uint8_t channel)
{
    bool retVal = false;

#if defined(CY_DEVICE_CCG3PA)
    /* No internal VConn support on PMG1S0. */
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(channel);
#endif /* defined(CY_DEVICE_CCG3PA) */

#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_CCG6DF) || defined(CY_DEVICE_PMG1S3) || defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S))
    PPDSS_REGS_T pd = context->base;

    /* Check whether the V5V -> VConn Switch is ON. */
    if (channel == CY_PD_CC_CHANNEL_1)
    {
        retVal = ((pd->vconn20_cc1_switch_1_ctrl & PDSS_VCONN20_CC1_SWITCH_1_CTRL_SEL_ON_OFF) != 0U);
    }
    else
    {
        retVal = ((pd->vconn20_cc2_switch_1_ctrl & PDSS_VCONN20_CC2_SWITCH_1_CTRL_SEL_ON_OFF) != 0U);
    }
#endif /* (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_CCG6DF) || defined(CY_DEVICE_PMG1S3) || defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S)) */

#if defined(CY_DEVICE_CCG3)
    PPDSS_REGS_T pd = context->base;

    if (channel == CY_PD_CC_CHANNEL_1)
    {
        retVal = ((pd->pfet300_ctrl & PDSS_PFET300_CTRL_EN_SWITCH_CC1) != 0U);
    }
    else
    {
        retVal = ((pd->pfet300_ctrl & PDSS_PFET300_CTRL_EN_SWITCH_CC2) != 0U);
    }
#endif /* defined(CY_DEVICE_CCG3) */

    return retVal;
}

/*******************************************************************************
* Function Name: Cy_USBPD_VbusDiv_To_AMuxDiscon
****************************************************************************//**
*
* Disconnect the internal VBus divider from AMUX bus.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \return bool
* Return true if disconnected, false if disconnection not allowed.
*
*******************************************************************************/
bool Cy_USBPD_VbusDiv_To_AMuxDiscon(cy_stc_usbpd_context_t *context)
{
    CY_UNUSED_PARAMETER(context);

    return false;
}

/* The FRS comparator threshold should be set to 0.52V = 2. */
#define CMP_FS_VSEL_VALUE       (2)

/*
 * Swap CTRL default settings for FRS receive. This settings are based on 5Mhz clock
 * to the block.
 */
#define FRS_RX_SWAP_CTRL1_DFLT_VAL      ((175u << PDSS_SWAP_CTRL1_PULSE_MIN_POS)| \
                                         (650u << PDSS_SWAP_CTRL1_PULSE_MAX_POS)|\
                                         (PDSS_SWAP_CTRL1_RESET_SWAP_STATE))

#define FRS_RX_SWAP_CTRL2_DFLT_VAL      ((50u << PDSS_SWAP_CTRL2_GLITCH_WIDTH_LOW_POS) | \
                                         (1u << PDSS_SWAP_CTRL2_GLITCH_WIDTH_HIGH_POS))

#define FRS_RX_SWAP_CTRL3_DFLT_VAL      (160u << PDSS_SWAP_CTRL3_STABLE_LOW_POS)

#define FRS_RX_SWAP_CTRL5_DFLT_VAL      (750u << PDSS_SWAP_CTRL5_LONG_LOW_POS)

/*******************************************************************************
* Function Name: Cy_USBPD_Vbus_FrsRxEnable
****************************************************************************//**
*
* Enables the fast role swap receive functionality. Callback
* registered in Cy_USBPD_Phy_Init will be called when FRS signal is received.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \return bool
* Returns true if success, otherwise returns false.
*
*******************************************************************************/
bool Cy_USBPD_Vbus_FrsRxEnable(cy_stc_usbpd_context_t *context)
{
#if (CY_PD_REV3_ENABLE && CY_PD_FRS_RX_ENABLE)
    cy_stc_pd_dpm_config_t* dpmConfig = context->dpmGetConfig();
    PPDSS_REGS_T pd = context->base;
    uint32_t regVal;

    /* Set the VBus detach comparator threshold to Vsafe5V */
    uint8_t level = Cy_USBPD_Adc_GetVbusLevel (context, pd_hal_get_vbus_detach_adc(),
            CY_PD_VSAFE_5V, CY_PD_VSAFE_5V_FRS_SWAP_RX_MARGIN);

    /* Enable VSAFE5V comparator */
    Cy_USBPD_Adc_FreeRunCtrl(context, pd_hal_get_vbus_detach_adc(), pd_hal_get_vbus_detach_input(), level);

    /* Configure CC line voltage thresholds to detect frs signal */
    regVal  = (pd->cc_ctrl_1 & ~(PDSS_CC_CTRL_1_CMP_FS_VSEL_MASK | PDSS_CC_CTRL_1_CMP_FS_CC1V2));
    regVal |= (CMP_FS_VSEL_VALUE << PDSS_CC_CTRL_1_CMP_FS_VSEL_POS);

    /* See if cc polarity need update */
    if(dpmConfig->polarity == CY_PD_CC_CHANNEL_2)
    {
        regVal |= PDSS_CC_CTRL_1_CMP_FS_CC1V2;
    }
    pd->cc_ctrl_1 = regVal;

    /*
     * Set the vsafe5v comp signal source:
     * Using VBUS_MON. Also clear Rx Swap Done status.
     */
    pd->swap_ctrl0 = (pd_hal_get_vbus_detach_adc() << PDSS_SWAP_CTRL0_SWAPR_SOURCE_SEL_POS) |
        (1U << PDSS_SWAP_CTRL0_RX_SWAP_SOURCE_POS) |
        PDSS_SWAP_CTRL0_CLR_RX_SWAP_DONE;

    /* Now configure the Swap controller */
    pd->swap_ctrl1 = FRS_RX_SWAP_CTRL1_DFLT_VAL;
    pd->swap_ctrl2 = FRS_RX_SWAP_CTRL2_DFLT_VAL;
    pd->swap_ctrl3 = FRS_RX_SWAP_CTRL3_DFLT_VAL;
    pd->swap_ctrl5 = FRS_RX_SWAP_CTRL5_DFLT_VAL;

    /* Let thresholds settle */
    Cy_SysLib_DelayUs(10);

    /* Take swap controller out of reset */
    pd->swap_ctrl1 &= ~PDSS_SWAP_CTRL1_RESET_SWAP_STATE;

    /* Enabling the FR pulse and disconnect interrupts. FET switching will only happen on pulse. */
    pd->intr2 = PDSS_INTR2_SWAP_RCVD | PDSS_INTR2_SWAP_DISCONNECT;
    pd->intr2_mask |= PDSS_INTR2_SWAP_RCVD | PDSS_INTR2_SWAP_DISCONNECT;

    pd->intr1 = PDSS_INTR1_VSWAP_VBUS_LESS_5_DONE;

    /* Enable the swap controller */
    pd->swap_ctrl0 &= ~PDSS_SWAP_CTRL0_CLR_RX_SWAP_DONE;
    pd->swap_ctrl0 |= PDSS_SWAP_CTRL0_SWAP_ENABLED;

    /* Ensure that the FET switching happens based on SWAP IRQ and VBUS Detect. */
    pd->debug_cc_0 |= PDSS_DEBUG_CC_0_VBUS_C_SWAP_SOURCE_SEL | PDSS_DEBUG_CC_0_VBUS_P_SWAP_SOURCE_SEL;

#if defined(CY_DEVICE_CCG6)

    /* Set the sink fet OFF settings as per current HW, and enable LF filter 0 for auto FET switching. */
    regVal = pd->pgdo_1_cfg;
    regVal |= ((1U << PDSS_PGDO_1_CFG_SEL_SWAP_VBUS_LESS_5_POS) | PDSS_PGDO_1_CFG_AUTO_MODE);
    pd->pgdo_1_cfg  = regVal;

    /* Ensure that source FET is off to start with, and then configure it for turn ON on FR trigger. */
    Cy_USBPD_Vbus_GdrvPfetOff (context, false);

    /* PGDO_PU_1_CFG register is used for Provider FET */
    regVal = pd->pgdo_pu_1_cfg;
    regVal |= (PDSS_PGDO_PU_1_CFG_PGDO_PU_EN_LV_ON_VALUE |
            (1U << PDSS_PGDO_PU_1_CFG_SEL_SWAP_VBUS_LESS_5_POS) |
            PDSS_PGDO_PU_1_CFG_AUTO_MODE);
    pd->pgdo_pu_1_cfg = regVal;

    /* We can use the ISINK control of the gate driver to slowly enable the provider path instead of
     * doing an instantaneous turn-on. Programming 0x120 value to the ID SINK counter in bypass mode. */

    /* First clear the current ISINK counter value. */
    pd->pgdo_pd_isnk_cfg &= ~(PDSS_PGDO_PD_ISNK_CFG_VALUE_MASK | PDSS_PGDO_PD_ISNK_CFG_VALUE_1_MASK |
            PDSS_PGDO_PD_ISNK_CFG_STRONG_EN);
    Cy_SysLib_DelayUs (50);
    pd->pgdo_pd_isnk_cfg |= PDSS_PGDO_PD_ISNK_CFG_LOAD_INIT_VALUE;
    Cy_SysLib_DelayUs (100);
    while ((pd->pgdo_pd_isnk_cfg & PDSS_PGDO_PD_ISNK_CFG_LOAD_INIT_VALUE) != 0);

    pd->pgdo_pd_isnk_cfg &= ~PDSS_PGDO_PD_ISNK_CFG_STRONG_EN;
    pd->pgdo_pd_isnk_cfg |= (0x20 << PDSS_PGDO_PD_ISNK_CFG_VALUE_POS) |
        (0x04U << PDSS_PGDO_PD_ISNK_CFG_VALUE_1_POS) |
        PDSS_PGDO_PD_ISNK_CFG_BYPASS_PD_ISNK;

#endif /* CY_DEVICE_CCG6 */
#else
    CY_UNUSED_PARAMETER(context);
#endif /* (CY_PD_REV3_ENABLE && CY_PD_FRS_RX_ENABLE) */

    return true;
}


/*******************************************************************************
* Function Name: Cy_USBPD_Vbus_FrsRxDisable
****************************************************************************//**
*
* Disables the fast role swap receive functionality.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \return bool
* Returns true if success, otherwise returns false.
*
*******************************************************************************/
bool Cy_USBPD_Vbus_FrsRxDisable(cy_stc_usbpd_context_t *context)
{
#if (CY_PD_REV3_ENABLE && CY_PD_FRS_RX_ENABLE)
    PPDSS_REGS_T pd = context->base;

    Cy_USBPD_Adc_CompCtrl (context, pd_hal_get_vbus_detach_adc(), CY_USBPD_ADC_INPUT_AMUX_A, 0, CY_USBPD_ADC_INT_DISABLED, NULL);

    /* Disable the swap controller */
    pd->swap_ctrl1 |= PDSS_SWAP_CTRL1_RESET_SWAP_STATE;
    pd->swap_ctrl0 &= ~PDSS_SWAP_CTRL0_SWAP_ENABLED;

    /* Disable and clear frs receive interrupts */
    pd->intr1_mask &= ~(PDSS_INTR1_VSWAP_VBUS_LESS_5_DONE);
    pd->intr1 = (PDSS_INTR1_VSWAP_VBUS_LESS_5_DONE);

    pd->intr2_mask &= ~(PDSS_INTR2_SWAP_RCVD | PDSS_INTR2_SWAP_DISCONNECT);
    pd->intr2 = (PDSS_INTR2_SWAP_RCVD | PDSS_INTR2_SWAP_DISCONNECT);

#if defined(CY_DEVICE_CCG6)
    /* Clear the switch on SWAP bits in the gate driver registers. */
    pd->pgdo_1_cfg &= ~PDSS_PGDO_1_CFG_SEL_SWAP_VBUS_LESS_5_MASK;
    pd->pgdo_2_cfg &= ~(1U << PDSS_PGDO_2_CFG_LS_SOURCE_SEL_POS);

    /* Use PGDO_PU_1_CFG register for Provider FET */
    pd->pgdo_pu_1_cfg &= ~PDSS_PGDO_PU_1_CFG_SEL_SWAP_VBUS_LESS_5_MASK;
    pd->pgdo_pu_2_cfg &= ~(1U << PDSS_PGDO_PU_2_CFG_LS_SOURCE_SEL_POS);

    /* Make sure auto-mode is turned off on both the provider and consumer gate drivers. */
    if (((pd->pgdo_pu_1_cfg & PGDO_PU_1_CFG_AUTO_SEL_MASK) == 0) && (pd->pgdo_pu_2_cfg == 0))
    {
        Cy_USBPD_Vbus_GdrvRstEdgeDet(context, true);
        pd->pgdo_pu_1_cfg &= ~(PDSS_PGDO_PU_1_CFG_AUTO_MODE);
    }

    if (((pd->pgdo_1_cfg & PGDO_1_CFG_AUTO_SEL_MASK) == 0) && (pd->pgdo_2_cfg == 0))
    {
        Cy_USBPD_Vbus_GdrvRstEdgeDet(context, false);
        pd->pgdo_1_cfg &= ~(PDSS_PGDO_1_CFG_AUTO_MODE);
    }
#endif /*CY_DEVICE_CCG6 */

    /* Restore FET switch condition to default. */
    pd->debug_cc_0 &= ~(PDSS_DEBUG_CC_0_VBUS_C_SWAP_SOURCE_SEL | PDSS_DEBUG_CC_0_VBUS_P_SWAP_SOURCE_SEL);
#else
    CY_UNUSED_PARAMETER(context);
#endif /* (CY_PD_REV3_ENABLE && CY_PD_FRS_RX_ENABLE) */

    return true;
}

/*******************************************************************************
* Function Name: Cy_USBPD_Vbus_FrsRx_IntrHandler
****************************************************************************//**
*
* Fast Role Swap Receive Interrupt Handler function.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
*******************************************************************************/
void Cy_USBPD_Vbus_FrsRx_IntrHandler(cy_stc_usbpd_context_t *context)
{
#if (CY_PD_REV3_ENABLE && CY_PD_FRS_RX_ENABLE)
    PPDSS_REGS_T pd = context->base;
    uint32_t regVal;

 #if defined(CY_DEVICE_CCG6)
    /*
       Clear the AUTO mode on the FETs without affecting their current state. We can
       only do this by assuming that the consumer FET is OFF and the provider FET is ON.
       Please note that it is not possible to handle OV/OC errors while going through
       the Fast Role Transition.
     */
    pd->pgdo_1_cfg &= ~(PDSS_PGDO_1_CFG_PGDO_EN_LV_OFF_VALUE | PDSS_PGDO_1_CFG_PGDO_EN_LV_ON_VALUE);
    pd->pgdo_1_cfg &= ~PDSS_PGDO_1_CFG_AUTO_MODE;
    Cy_USBPD_Vbus_GdrvRstEdgeDet(context, false);
    /* Use PGDO_PU_1_CFG register for Provider FET */
    pd->pgdo_pu_1_cfg &= ~PDSS_PGDO_PU_1_CFG_PGDO_PU_EN_LV_OFF_VALUE;
    pd->pgdo_pu_1_cfg |= PDSS_PGDO_PU_1_CFG_PGDO_PU_EN_LV_ON_VALUE;
    Cy_SysLib_DelayUs (1);

    Cy_USBPD_Vbus_GdrvRstEdgeDet(context, true);

    /* Use PGDO_PU_1_CFG register for Provider FET */
    regVal = pd->pgdo_pu_1_cfg;
    regVal &= ~(PDSS_PGDO_PU_1_CFG_AUTO_MODE | PDSS_PGDO_PU_1_CFG_PGDO_PU_IN_LV_ON_VALUE);
    regVal |= PDSS_PGDO_PU_1_CFG_SEL_ON_OFF;
    pd->pgdo_pu_1_cfg = regVal;
#endif /* defined(CY_DEVICE_CCG6) */

    /* Remember the fact that the provider FET is on so that a subsequent
     * soft start does not cause supply to be turned off.
     */
    context->vbusPfetOn = true;

    Cy_SysLib_DelayUs (1);
    Cy_USBPD_Vbus_FrsRxDisable(context);
#else
    CY_UNUSED_PARAMETER(context);
#endif /* (CY_PD_REV3_ENABLE && CY_PD_FRS_RX_ENABLE) */
}

#if defined(CY_DEVICE_PMG1S3)
#define FRS_TX_GPIO_PORT_PIN_P0 (0x21)
#define FRS_TX_GPIO_PORT_PIN_P1 (0x26)
#endif /* defined(CY_DEVICE_PMG1S3) */

/* DEBUG_CC_1 Register Setting */
#define PDSS_TX_STOP_ON_SWAP_MASK       (1u << 26u)

/* Swap CTRL settings for FRS TX */
#define FRS_TX_IRQ_WIDTH                (900u)
#define FRS_TX_SWAP_CTRL1_DFLT_VAL      (FRS_TX_IRQ_WIDTH | PDSS_SWAPT_CTRL1_DEFAULT_LEVEL |    \
                                         (2u << PDSS_SWAPT_CTRL1_COMMAND_POS) |                 \
                                         PDSS_SWAPT_CTRL1_RESET_SWAPT_STATE)
/*******************************************************************************
* Function Name: Cy_USBPD_Vbus_FrsTxEnable
****************************************************************************//**
*
* Enables the fast role swap transmit functionality. Callback
* registered in Cy_USBPD_Phy_Init will be called when FRS signal is received.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \return bool
* Returns true if success, otherwise returns false.
*
*******************************************************************************/
bool Cy_USBPD_Vbus_FrsTxEnable(cy_stc_usbpd_context_t *context)
{
#if (CY_PD_REV3_ENABLE && CY_PD_FRS_TX_ENABLE && defined(CY_DEVICE_PMG1S3))
    cy_stc_pd_dpm_config_t* dpmConfig = context->dpmGetConfig();
    PPDSS_REGS_T pd = context->base;
    
    /* Configure FRS TX source */
    if(context->port == TYPEC_PORT_0_IDX)
    {
        /* Configuring a GPIO for trigering FRS signal */
        Cy_GPIO_SetDrivemode(Cy_GPIO_PortToAddr(FRS_TX_GPIO_PORT_PIN_P0 >> 4),
                (FRS_TX_GPIO_PORT_PIN_P0 & 0x0Fu), CY_GPIO_DM_HIGHZ);
        Cy_GPIO_SetHSIOM(Cy_GPIO_PortToAddr(FRS_TX_GPIO_PORT_PIN_P0 >> 4),
                (FRS_TX_GPIO_PORT_PIN_P0 & 0x0Fu), (en_hsiom_sel_t)P2_1_USBPD0_SWAPT_IN);
    }
#if PMG1_PD_DUALPORT_ENABLE
    if (context->port == TYPEC_PORT_1_IDX)
    {
        /* Configuring a GPIO for trigering FRS signal */
        Cy_GPIO_SetDrivemode(Cy_GPIO_PortToAddr(FRS_TX_GPIO_PORT_PIN_P1 >> 4),
                (FRS_TX_GPIO_PORT_PIN_P0 & 0x0Fu), CY_GPIO_DM_HIGHZ);
        Cy_GPIO_SetHSIOM(Cy_GPIO_PortToAddr(FRS_TX_GPIO_PORT_PIN_P1 >> 4),
                (FRS_TX_GPIO_PORT_PIN_P0 & 0x0Fu), (en_hsiom_sel_t)P2_6_USBPD1_SWAPT_IN);
    }
#endif /* PMG1_PD_DUALPORT_ENABLE */

    /* Configure for Auto FRS signal transmitting */
    pd->debug_cc_1 &= ~(PDSS_DEBUG_CC_1_PFET300_PULLDN_EN_CC1 |
                PDSS_DEBUG_CC_1_PFET300_PULLDN_EN_CC2 |
                PDSS_DEBUG_CC_1_SWAPT_TO_CC1_EN |
                PDSS_DEBUG_CC_1_SWAPT_TO_CC2_EN);

    /* Enable TX discard on swap */
    pd->debug_cc_1 |= PDSS_TX_STOP_ON_SWAP_MASK;

    pd->swap_ctrl0 = PDSS_SWAP_CTRL0_SWAP_ENABLED;
    pd->swap_ctrl0 |= 1UL;

    pd->swap_ctrl0 |= PDSS_SWAP_CTRL0_SWAPT_POLARITY;
    pd->swapt_ctrl1 = FRS_TX_SWAP_CTRL1_DFLT_VAL;

    /* Enable necessary interrupts */
    pd->intr2 = PDSS_INTR2_SWAP_COMMAND_DONE;
    pd->intr2_mask |= PDSS_INTR2_SWAP_COMMAND_DONE;

    /* This delay is needed otherwise swap TX indefinitely short the cc line */
    Cy_SysLib_DelayUs(10);

    /* Enable the swap tx  */
    pd->swapt_ctrl1 &= ~PDSS_SWAPT_CTRL1_RESET_SWAPT_STATE;

    /* Set cc polarity for pulldowns */
   if(dpmConfig->polarity == CY_PD_CC_CHANNEL_2)
   {
        pd->debug_cc_1 |= (PDSS_DEBUG_CC_1_SWAPT_TO_CC2_EN);
   }
   else
   {
        pd->debug_cc_1 |= (PDSS_DEBUG_CC_1_SWAPT_TO_CC1_EN);
   }
#else
    CY_UNUSED_PARAMETER(context);
#endif /* (CY_PD_REV3_ENABLE && CY_PD_FRS_TX_ENABLE && defined(CY_DEVICE_PMG1S3)) */

    return true;
}

/*******************************************************************************
* Function Name: Cy_USBPD_Vbus_FrsTxDisable
****************************************************************************//**
*
* Disables the fast role swap transmit functionality.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \return bool
* Returns true if success, otherwise returns false.
*
*******************************************************************************/
bool Cy_USBPD_Vbus_FrsTxDisable(cy_stc_usbpd_context_t *context)
{
#if (CY_PD_REV3_ENABLE && CY_PD_FRS_TX_ENABLE && defined(CY_DEVICE_PMG1S3))
    PPDSS_REGS_T pd = context->base;

    /* Disable the swap controller */
    pd->swapt_ctrl1 = PDSS_SWAPT_CTRL1_RESET_SWAPT_STATE;
    pd->swap_ctrl0 &= ~PDSS_SWAP_CTRL0_SWAP_ENABLED;

    /* Clear and disable frs receive interrupts */
    pd->intr2_mask &= ~PDSS_INTR2_SWAP_COMMAND_DONE;
    pd->intr2       = PDSS_INTR2_SWAP_COMMAND_DONE;

    /* Disable pulldown */
    pd->debug_cc_1 &= ~(PDSS_DEBUG_CC_1_SWAPT_TO_CC1_EN |
                PDSS_DEBUG_CC_1_SWAPT_TO_CC2_EN);

#else
    CY_UNUSED_PARAMETER(context);
#endif /* (CY_PD_REV3_ENABLE && CY_PD_FRS_TX_ENABLE && defined(CY_DEVICE_PMG1S3)) */

    return true;
}


/*******************************************************************************
* Function Name: Cy_USBPD_Vbus_GdrvRstEdgeDet
****************************************************************************//**
*
* Resets the gate driver edge detector to clear any fault state.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param pgdoType
* Flag indicating the gate driver to be cleared, true for P_CTRL
* and false for C_CTRL.
*
*******************************************************************************/
void Cy_USBPD_Vbus_GdrvRstEdgeDet(cy_stc_usbpd_context_t *context, bool pgdoType)
{
    PPDSS_REGS_T pd = context->base;

    (void)pd;

#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6))
    /* True indicates PGDO with internal pull up. */
    if (pgdoType)
    {
        /* Reset edge detector. */
        pd->pgdo_pu_1_cfg |= PDSS_PGDO_PU_1_CFG_RST_EDGE_DET;
        pd->pgdo_pu_1_cfg &= ~PDSS_PGDO_PU_1_CFG_RST_EDGE_DET;
    }
    else
    {
        /* Reset edge detector. */
        pd->pgdo_1_cfg |= PDSS_PGDO_1_CFG_RST_EDGE_DET;
        pd->pgdo_1_cfg &= ~PDSS_PGDO_1_CFG_RST_EDGE_DET;
    }
#elif defined(CY_DEVICE_CCG3)
    if (pgdoType)
    {
        pd->ngdo_ctrl_p |= PDSS_NGDO_CTRL_P_RST_EDGE_DET;
        pd->ngdo_ctrl_p &= ~PDSS_NGDO_CTRL_P_RST_EDGE_DET;
    }
    else
    {
        pd->ngdo_ctrl_c |= PDSS_NGDO_CTRL_C_RST_EDGE_DET;
        pd->ngdo_ctrl_c &= ~PDSS_NGDO_CTRL_C_RST_EDGE_DET;
    }
#else
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(pgdoType);
#endif /* CY_DEVICE */
}


/*******************************************************************************
* Function Name: Cy_USBPD_Vbus_RemoveIntrnlFbDiv
****************************************************************************//**
*
* Removes internal feedback resistor divider.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
*******************************************************************************/
void Cy_USBPD_Vbus_RemoveIntrnlFbDiv(cy_stc_usbpd_context_t *context)
{
#if defined(CY_DEVICE_CCG3PA)
    PPDSS_REGS_T pd = context->base;
    pd->ea_ctrl |= PDSS_EA_CTRL_RES_DIV_BYPASS;
#else
    CY_UNUSED_PARAMETER(context);
#endif /* defined(CY_DEVICE_CCG3PA) */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Vbus_GdrvIsSnkFetOn
****************************************************************************//**
*
* Check whether the Sink FET has been turned on.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \return bool
* Returns true if the sink path is on to allow system to charge.
*
*******************************************************************************/
bool Cy_USBPD_Vbus_GdrvIsSnkFetOn (cy_stc_usbpd_context_t *context)
{
    return (context->vbusCfetOn);
}

/*******************************************************************************
* Function Name: Cy_USBPD_Vbus_NgdoEqCtrl
****************************************************************************//**
*
* Enable/Disable the equalizer between the Gates and Source/Drain of the Gate driver circuit. 
* Use this API after turning off Gate-1 during Pfet turn Off sequence. This is only 
* applicable to PMG1S3 device.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param value
* Gate-1 control value.
* False: Disable the equalizer.
* True: Enable the equalizer.
*
*******************************************************************************/
void Cy_USBPD_Vbus_NgdoEqCtrl(cy_stc_usbpd_context_t *context, bool value)
{
#if defined(CY_DEVICE_PMG1S3)    
    PPDSS_REGS_T pd = context->base;
    
    pd->v2_ngdo_0_ctrl &= ~PDSS_V2_NGDO_0_CTRL_NGDO_GRV_EN;
    if(value == true)
    {
        pd->v2_ngdo_1_ctrl |= (1UL << 23) | (1UL << 8);
    }
    else
    {
        pd->v2_ngdo_1_ctrl &= ~((1UL << 23) | (1UL << 8));
    }
#else
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(value);
#endif /* defined(CY_DEVICE_PMG1S3) */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Vbus_NgdoG1Ctrl
****************************************************************************//**
*
* Control the Gate-1 on the VBUS-C side of the Gate driver circuit. This is only 
* applicable to PMG1S3 device.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param value
* Gate-1 control value.
* False: Gate-1 is pulled down to turn the FET off.
* True: Gate-1 is allowed to charge for turning on the FET.
*
*******************************************************************************/
void Cy_USBPD_Vbus_NgdoG1Ctrl(cy_stc_usbpd_context_t *context, bool value)
{
#if defined(CY_DEVICE_PMG1S3)    
    PPDSS_REGS_T pd = context->base;
    
    if(value == true)
    {
        pd->v2_ngdo_1_ctrl |= PDSS_V2_NGDO_1_CTRL_V2_NGDO_EN_G1_CHRG;
    }
    else
    {
        pd->v2_ngdo_1_ctrl &= ~PDSS_V2_NGDO_1_CTRL_V2_NGDO_EN_G1_CHRG;
    }
#else
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(value);
#endif /* defined(CY_DEVICE_PMG1S3) */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Vbus_NgdoSetDriveStrength
****************************************************************************//**
*
* Set the drive strength of the producer FET using the internal gate driver.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param value
* The drive strength value with bit step size (current) as 150nA. 
* Max value allowed is 4200nA (0x1C).
*
*******************************************************************************/
void Cy_USBPD_Vbus_NgdoSetDriveStrength(cy_stc_usbpd_context_t *context, uint8_t value)
{
#if defined(CY_DEVICE_PMG1S3)
    PPDSS_REGS_T pd = context->base;
    
    /* Never set a higher value than maximum */
    if(value > 0x1Cu)
    {
        value = 0x1Cu; 
    }

    /* Clear the NGDO slew control field */
    pd->v2_ngdo_0_ctrl &= ~PDSS_V2_NGDO_0_CTRL_V2_NGDO_SLEW_CTRL_MASK;
    /* Set the value for NGDO slew control */ 
    pd->v2_ngdo_0_ctrl |= ((uint32_t)value << PDSS_V2_NGDO_0_CTRL_V2_NGDO_SLEW_CTRL_POS);

#else
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(value);
#endif /* defined(CY_DEVICE_PMG1S3) */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Vbus_GdrvPfetOn
****************************************************************************//**
*
* Turn on producer FET using the internal gate driver or dedicated GPIO.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param turnOnSeq
* Turn On sequence On PMG1S2, this bit selects which FET turns on first.
*  - 0: FET 0 turns on first.
*  - 1: FET 1 turns on first.
*
*******************************************************************************/
void Cy_USBPD_Vbus_GdrvPfetOn(cy_stc_usbpd_context_t *context, bool turnOnSeq)
{
#if (!CY_PD_SINK_ONLY)
    PPDSS_REGS_T pd = context->base;
    uint32_t regVal = 0;
    uint32_t state = 0;

    CY_UNUSED_PARAMETER(regVal);
    CY_UNUSED_PARAMETER(state);
    CY_UNUSED_PARAMETER(pd);
    CY_UNUSED_PARAMETER(turnOnSeq);

#if defined(CY_DEVICE_PMG1S3)
    pd->v2_ngdo_0_ctrl |= (PDSS_V2_NGDO_0_CTRL_NGDO_ISO_N | PDSS_V2_NGDO_0_CTRL_NGDO_EN_LV);
    Cy_SysLib_DelayUs(20);
    pd->v2_ngdo_0_ctrl |= PDSS_V2_NGDO_0_CTRL_NGDO_CP_EN;
    Cy_SysLib_DelayUs(20);
    pd->v2_ngdo_0_ctrl |= PDSS_V2_NGDO_0_CTRL_KEEP_OFF_DISABLE;
    Cy_SysLib_DelayUs(20);
    pd->v2_ngdo_0_ctrl |= PDSS_V2_NGDO_0_CTRL_NGDO_GRV_EN;
    
    context->vbusPfetOn = true;
#endif /* defined(CY_DEVICE_PMG1S3) */

#if defined(CY_DEVICE_CCG6)

    /* We can turn sink FET OFF if source FET is being turned ON. */
    Cy_USBPD_Vbus_GdrvCfetOff(context, turnOnSeq);

    /* Reset the edge detector. */
    Cy_USBPD_Vbus_GdrvRstEdgeDet(context, true);

    state = Cy_SysLib_EnterCriticalSection();

#if !SOFT_START_ENABLE
    /* Enable the pull-down for the FET. */
    pd->pgdo_pd_isnk_cfg = PDSS_PGDO_PD_ISNK_CFG_DEFAULT;
#endif /* SOFT_START_ENABLE */

    context->vbusPfetOn = true;

    /* Turn on the fet by setting ENABLE_ON bit and clearing IN_ON bit. */
    regVal = pd->pgdo_pu_1_cfg;
    regVal |= (PDSS_PGDO_PU_1_CFG_SEL_ON_OFF | PDSS_PGDO_PU_1_CFG_PGDO_PU_EN_LV_ON_VALUE);
    regVal &= ~PDSS_PGDO_PU_1_CFG_PGDO_PU_IN_LV_ON_VALUE;
    pd->pgdo_pu_1_cfg = regVal;

    Cy_SysLib_ExitCriticalSection (state);
#endif /* defined(CY_DEVICE_CCG6) */

#if defined(CY_DEVICE_CCG3)
    /* We can turn sink FET OFF if source FET is being turned ON. */
    Cy_USBPD_Vbus_GdrvCfetOff(context, turnOnSeq);

    state = Cy_SysLib_EnterCriticalSection();

    /* Assuming use of dual N-Channel FETs here. */

    /* Turn-the first FET ON. */
    regVal = pd->ngdo_ctrl_0;
    if (!turnOnSeq)
    {
        regVal &= ~VBUS_P_PLDN_EN_LV_0;
        regVal |= VBUS_P_NGDO_EN_LV_0;
    }
    else
    {
        regVal &= ~VBUS_P_PLDN_EN_LV_1;
        regVal |= VBUS_P_NGDO_EN_LV_1;
    }

    pd->ngdo_ctrl_0 = regVal;

    /* Now turn the second FET ON after a delay. */
    Cy_SysLib_DelayUs (10);

    if (!turnOnSeq)
    {
        regVal &= ~VBUS_P_PLDN_EN_LV_1;
        regVal |= VBUS_P_NGDO_EN_LV_1;
    }
    else
    {
        regVal &= ~VBUS_P_PLDN_EN_LV_0;
        regVal |= VBUS_P_NGDO_EN_LV_0;
    }

    pd->ngdo_ctrl_0 = regVal;
    context->vbusPfetOn = true;

    /* Reset the edge detector to make sure FETs turn ON. */
    Cy_USBPD_Vbus_GdrvRstEdgeDet (context, true);

    Cy_SysLib_ExitCriticalSection (state);
#endif /* defined(CY_DEVICE_CCG3) */
#else
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(turnOnSeq);
#endif /* (!CY_PD_SINK_ONLY) */
}


/*******************************************************************************
* Function Name: Cy_USBPD_Vbus_GdrvPfetOff
****************************************************************************//**
*
* Turn OFF producer FET using the internal gate driver or dedicated GPIO.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param turnOffSeq
* Turn Off sequence On PMG1S2, this bit selects which FET turns on first.
*  - 0: FET 0 turns on first.
*  - 1: FET 1 turns on first.
*
*******************************************************************************/
void Cy_USBPD_Vbus_GdrvPfetOff(cy_stc_usbpd_context_t *context, bool turnOffSeq)
{
#if (!CY_PD_SINK_ONLY)
    PPDSS_REGS_T pd = context->base;

    CY_UNUSED_PARAMETER(pd);
    CY_UNUSED_PARAMETER(turnOffSeq);

#if defined(CY_DEVICE_PMG1S3)
    context->vbusPfetOn = false;

    pd->v2_ngdo_0_ctrl &= ~PDSS_V2_NGDO_0_CTRL_NGDO_GRV_EN;
    Cy_SysLib_DelayUs(20);
    pd->v2_ngdo_0_ctrl &= ~PDSS_V2_NGDO_0_CTRL_NGDO_CP_EN;
    Cy_SysLib_DelayUs(20);    
    pd->v2_ngdo_0_ctrl &= ~(PDSS_V2_NGDO_0_CTRL_NGDO_ISO_N | PDSS_V2_NGDO_0_CTRL_NGDO_EN_LV);
    Cy_SysLib_DelayUs(20);
    pd->v2_ngdo_0_ctrl&= ~(PDSS_V2_NGDO_0_CTRL_NGDO_FAULT_RCP_EN | PDSS_V2_NGDO_0_CTRL_NGDO_FAULT_SCP_EN | PDSS_V2_NGDO_0_CTRL_NGDO_FAULT_VBUS_OV_EN);
#endif /* defined(CY_DEVICE_PMG1S3) */

#if defined(CY_DEVICE_CCG6)

#if !SOFT_START_ENABLE
    if (context->vbusPfetOn)
    {
        /* Disable the pull-down on the FET. */
        pd->pgdo_pd_isnk_cfg &= ~PDSS_PGDO_PD_ISNK_CFG_STRONG_EN;
    }
#endif /* SOFT_START_ENABLE */

    context->vbusPfetOn = false;

    /* Disable the FET first. */
    pd->pgdo_pu_1_cfg &= ~(PDSS_PGDO_PU_1_CFG_PGDO_PU_EN_LV_ON_VALUE | PDSS_PGDO_PU_1_CFG_SEL_ON_OFF |
            PDSS_PGDO_PU_1_CFG_AUTO_MODE);
    Cy_SysLib_DelayUs (10);

    /* Disable all auto mode configuration. */
    pd->pgdo_pu_2_cfg  = 0;
    pd->pgdo_pu_1_cfg &= ~(PGDO_PU_1_CFG_AUTO_SEL_MASK | PDSS_PGDO_PU_1_CFG_AUTO_MODE);
    Cy_SysLib_DelayUs (10);

    /* Program PGDO back to its default state. */
    pd->pgdo_pu_1_cfg &= ~(PDSS_PGDO_PU_1_CFG_SEL_ON_OFF
            | PDSS_PGDO_PU_1_CFG_PGDO_PU_EN_LV_ON_VALUE | PDSS_PGDO_PU_1_CFG_PGDO_PU_EN_LV_OFF_VALUE
            | PDSS_PGDO_PU_1_CFG_PGDO_PU_IN_LV_ON_VALUE | PDSS_PGDO_PU_1_CFG_PGDO_PU_IN_LV_OFF_VALUE);

    /*
     * The edge detector may have triggered later than previous fault.
     * In this case, this needs to be cleared.
     */
    Cy_USBPD_Vbus_GdrvRstEdgeDet(context, true);
#endif /* defined(CY_DEVICE_CCG6) */

#if defined(CY_DEVICE_CCG3)
    uint32_t regval = pd->ngdo_ctrl_0;
    uint32_t state;

    state = Cy_SysLib_EnterCriticalSection ();

    /* Turn-off the first FET. */
    if (!turnOffSeq)
    {
        regval &= ~VBUS_P_NGDO_EN_LV_0;
        regval |= VBUS_P_PLDN_EN_LV_0;
    }
    else
    {
        regval &= ~VBUS_P_NGDO_EN_LV_1;
        regval |= VBUS_P_PLDN_EN_LV_1;
    }

    pd->ngdo_ctrl_0 = regval;

    /* Turn-off the second FET after a delay. */
    Cy_SysLib_DelayUs (10);

    regval &= ~(VBUS_P_NGDO_EN_LV_0 | VBUS_P_NGDO_EN_LV_1);
    regval |= (VBUS_P_PLDN_EN_LV_0 | VBUS_P_PLDN_EN_LV_1);
    pd->ngdo_ctrl_0 = regval;

    /* Disable auto mode and reset the edge detector. */
    pd->ngdo_ctrl_p &= ~(PDSS_NGDO_CTRL_P_AUTO_MODE | PDSS_NGDO_CTRL_P_SEL_SWAP_VBUS_LESS_5);
    Cy_USBPD_Vbus_GdrvRstEdgeDet (context, true);

    context->vbusPfetOn = false;
    Cy_SysLib_ExitCriticalSection (state);
#endif /* defined(CY_DEVICE_CCG3) */
#else
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(turnOffSeq);
#endif /* (!CY_PD_SINK_ONLY) */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Vbus_GdrvCfetOn
****************************************************************************//**
*
* Turn on consumer FET using the internal gate driver or dedicated GPIO.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param turnOnSeq
* Turn On sequence On PMG1S2, this bit selects which FET turns on first.
*  - 0: FET 0 turns on first.
*  - 1: FET 1 turns on first.
*
*******************************************************************************/
void Cy_USBPD_Vbus_GdrvCfetOn(cy_stc_usbpd_context_t *context, bool turnOnSeq)
{
#if (PSVP_FPGA_ENABLE)
    /* No implementation necessary on PSVP. */
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(turnOnSeq);
#else
#if defined(CY_DEVICE_CCG6)
    PPDSS_REGS_T pd = context->base;
    uint32_t regVal;
    uint32_t mask;
    (void)turnOnSeq;

#if (!CY_PD_SINK_ONLY)
    /* We can turn source FET OFF when turning sink FET ON. */
    Cy_USBPD_Vbus_GdrvPfetOff(context, turnOnSeq);
#endif /* (!CY_PD_SINK_ONLY) */

    /* Reset the edge detector. */
    Cy_USBPD_Vbus_GdrvRstEdgeDet(context, false);

    mask = Cy_SysLib_EnterCriticalSection ();

    /* Make sure the PD port is idle before enabling the FET. */
    while ((pd->status & (PDSS_STATUS_RX_BUSY | PDSS_STATUS_CC_DATA_VALID)) != 0U)
    {

    }

#if !SOFT_START_ENABLE
    /* Enable the pull-down for the FET. */
    pd->pgdo_pd_isnk_cfg = PDSS_PGDO_PD_ISNK_CFG_DEFAULT;
#endif /* SOFT_START_ENABLE */

    context->vbusCfetOn = true;

    /* Turn on the fet by setting ENABLE_ON bit. */
    regVal = pd->pgdo_1_cfg;
    regVal |= (PDSS_PGDO_1_CFG_SEL_ON_OFF | PDSS_PGDO_1_CFG_PGDO_EN_LV_ON_VALUE);
    pd->pgdo_1_cfg |= regVal;

    Cy_SysLib_ExitCriticalSection (mask);

#elif defined(CY_DEVICE_CCG3PA)

    PPDSS_REGS_T pd = context->base;
    uint32_t regVal;

#if !SOFT_START_ENABLE
    /* Enable strong pull-down for turning the FET ON */
    pd->pgdo_pd_isnk_cfg |= PDSS_PGDO_PD_ISNK_CFG_STRONG_EN;
#endif /* SOFT_START_ENABLE */

    context->vbusCfetOn = true;
    
    /*
     * The edge detector may have triggered later than previous fault.
     * In this case, this needs to be cleared before the FET can be
     * turned ON.
     */
    Cy_USBPD_Vbus_GdrvRstEdgeDet(context, true);

    if(turnOnSeq)
    {
        /* Turn on the fet by setting ENABLE_ON bit and clearing IN_ON bit. */
        regVal = pd->pgdo_pu_1_cfg;
        regVal &= ~PDSS_PGDO_PU_1_CFG_PGDO_PU_IN_LV_ON_VALUE;
        regVal |= (PDSS_PGDO_PU_1_CFG_SEL_ON_OFF | PDSS_PGDO_PU_1_CFG_PGDO_PU_EN_LV_ON_VALUE);
        pd->pgdo_pu_1_cfg = regVal;
    }
    else
    {
        regVal = pd->pgdo_1_cfg;
        /* Turn on the fet by setting ENABLE_ON bit. */
        regVal |= (PDSS_PGDO_1_CFG_SEL_ON_OFF | PDSS_PGDO_1_CFG_PGDO_EN_LV_ON_VALUE);
        pd->pgdo_1_cfg = regVal;
    }

#elif defined(CY_DEVICE_CCG3)

    PPDSS_REGS_T pd = context->base;
    uint32_t regval;
    uint32_t mask;

#if (!CY_PD_SINK_ONLY)
    /* Disable the provider FET before enabling consumer FET. */
    Cy_USBPD_Vbus_GdrvPfetOff(context, turnOnSeq);
#endif /* (!CY_PD_SINK_ONLY) */

    mask = Cy_SysLib_EnterCriticalSection();

    regval = pd->ngdo_ctrl_0;

    /* Turn the first FET ON. */
    if (!turnOnSeq)
    {
        regval &= ~VBUS_C_PLDN_EN_LV_0;
        regval |= VBUS_C_NGDO_EN_LV_0;
    }
    else
    {
        regval &= ~VBUS_C_PLDN_EN_LV_1;
        regval |= VBUS_C_NGDO_EN_LV_1;
    }

    pd->ngdo_ctrl_0 = regval;

    /* Turn the second FET ON after a delay. */
    Cy_SysLib_DelayUs (10);

    regval &= ~(VBUS_C_PLDN_EN_LV_0 | VBUS_C_PLDN_EN_LV_1);
    regval |= VBUS_C_NGDO_EN_LV_0 | VBUS_C_NGDO_EN_LV_1;
    pd->ngdo_ctrl_0 = regval;

    /* Reset the edge detector. */
    Cy_USBPD_Vbus_GdrvRstEdgeDet(context, false);

    context->vbusCfetOn = true;

    Cy_SysLib_ExitCriticalSection (mask);

#else

    /* TBD: Implement for PMG1-S3. */
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(turnOnSeq);

#endif /* CY_DEVICE */
#endif /* PSVP_FPGA_ENABLE */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Vbus_GdrvCfetOff
****************************************************************************//**
*
* Turn OFF consumer FET using the internal gate driver or dedicated GPIO.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param turnOffSeq
* Turn Off sequence On PMG1S2, this bit selects which FET turns on first.
*  - 0: FET 0 turns on first.
*  - 1: FET 1 turns on first.
*
*******************************************************************************/
void Cy_USBPD_Vbus_GdrvCfetOff(cy_stc_usbpd_context_t *context, bool turnOffSeq)
{
#if (PSVP_FPGA_ENABLE)
    /* No implementation necessary on PSVP. */
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(turnOffSeq);
#else
#if defined(CY_DEVICE_CCG6)
    PPDSS_REGS_T pd = context->base;

    /* Legacy parameter - Not used. */
    (void)turnOffSeq;

    /* Disable all auto mode configuration. */
    pd->pgdo_2_cfg  = 0;
    pd->pgdo_1_cfg &= ~(PGDO_1_CFG_AUTO_SEL_MASK | PDSS_PGDO_1_CFG_AUTO_MODE);

    /* Program PGDO back to its default (OFF) state. */
    pd->pgdo_1_cfg &= ~(PDSS_PGDO_1_CFG_SEL_ON_OFF |
            PDSS_PGDO_1_CFG_PGDO_EN_LV_ON_VALUE | PDSS_PGDO_1_CFG_PGDO_EN_LV_OFF_VALUE);

    context->vbusCfetOn = false;

#elif defined(CY_DEVICE_CCG3PA)

    PPDSS_REGS_T pd = context->base;
    uint32_t regVal;

    if(turnOffSeq)
    {
        regVal = pd->pgdo_pu_1_cfg;
        regVal &= ~(PDSS_PGDO_PU_1_CFG_SEL_ON_OFF
                    | PDSS_PGDO_PU_1_CFG_PGDO_PU_EN_LV_ON_VALUE | PDSS_PGDO_PU_1_CFG_PGDO_PU_EN_LV_OFF_VALUE
                    | PDSS_PGDO_PU_1_CFG_PGDO_PU_IN_LV_ON_VALUE | PDSS_PGDO_PU_1_CFG_PGDO_PU_IN_LV_OFF_VALUE);
        regVal |= PDSS_PGDO_PU_1_CFG_PGDO_PU_IN_LV_OFF_VALUE | PDSS_PGDO_PU_1_CFG_PGDO_PU_EN_LV_OFF_VALUE;
        pd->pgdo_pu_1_cfg = regVal;
    }
    else
    {
        /* Program PGDO_PU back to its default (OFF) state. */
        pd->pgdo_1_cfg &= ~(PDSS_PGDO_1_CFG_SEL_ON_OFF
            | PDSS_PGDO_1_CFG_PGDO_EN_LV_ON_VALUE | PDSS_PGDO_1_CFG_PGDO_EN_LV_OFF_VALUE);
    }
    /*
     * The edge detector may have triggered later than previous fault.
     * In this case, this needs to be cleared.
     */
    Cy_USBPD_Vbus_GdrvRstEdgeDet(context, true);

    /* Make sure the soft start of the FET is disabled. */
    if (context->vbusCfetOn)
    {
        pd->pgdo_pd_isnk_cfg |= PDSS_PGDO_PD_ISNK_CFG_STRONG_EN;
        context->vbusCfetOn = false;
    }

#elif defined(CY_DEVICE_CCG3)

    PPDSS_REGS_T pd = context->base;
    uint32_t regval;
    uint32_t mask;

    mask = Cy_SysLib_EnterCriticalSection();

    regval = pd->ngdo_ctrl_0;

    /* Turn OFF the first FET. */
    if (!turnOffSeq)
    {
        regval &= ~VBUS_C_NGDO_EN_LV_0;
        regval |= VBUS_C_PLDN_EN_LV_0;
    }
    else
    {
        regval &= ~VBUS_C_NGDO_EN_LV_1;
        regval |= VBUS_C_PLDN_EN_LV_1;
    }

    pd->ngdo_ctrl_0 = regval;

    /* Turn the second FET off after a delay. */
    Cy_SysLib_DelayUs(10);

    regval &= ~(VBUS_C_NGDO_EN_LV_0 | VBUS_C_NGDO_EN_LV_1);
    regval |= VBUS_C_PLDN_EN_LV_0 | VBUS_C_PLDN_EN_LV_1;
    pd->ngdo_ctrl_0 = regval;

    /* Disable auto mode and reset the edge detector. */
    pd->ngdo_ctrl_c &= ~(PDSS_NGDO_CTRL_C_AUTO_MODE | PDSS_NGDO_CTRL_C_SEL_SWAP_VBUS_LESS_5);
    Cy_USBPD_Vbus_GdrvRstEdgeDet(context, false);

    context->vbusCfetOn = false;

    Cy_SysLib_ExitCriticalSection(mask);

#else

    /* TBD: Implement for PMG1-S3. */
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(turnOffSeq);

#endif /* CY_DEVICE */
#endif /* PSVP_FPGA_ENABLE */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Vbus_NgdoOn
****************************************************************************//**
*
* Turns on the FET using the internal NGDO. This API is only applicable to PMG1S3.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param pfet
* Flag indicating whether the NGDO is connected to Provider FET
*  - 0: Connected to Consumer FET.
*  - 1: Connected to Provider FET.
*
*******************************************************************************/
void Cy_USBPD_Vbus_NgdoOn (cy_stc_usbpd_context_t *context, bool pfet)
{
#if defined(CY_DEVICE_PMG1S3)
    PPDSS_REGS_T pd = context->base;

    pd->v2_ngdo_0_ctrl |= (PDSS_V2_NGDO_0_CTRL_NGDO_ISO_N | PDSS_V2_NGDO_0_CTRL_NGDO_EN_LV);
    Cy_SysLib_DelayUs(20);
    pd->v2_ngdo_0_ctrl |= PDSS_V2_NGDO_0_CTRL_NGDO_CP_EN;
    Cy_SysLib_DelayUs(20);
    pd->v2_ngdo_0_ctrl |= PDSS_V2_NGDO_0_CTRL_KEEP_OFF_DISABLE;
    Cy_SysLib_DelayUs(20);
    pd->v2_ngdo_0_ctrl |= PDSS_V2_NGDO_0_CTRL_NGDO_GRV_EN;
    
    if(pfet == true)
    {
        context->vbusPfetOn = true;
    }
    else
    {
        context->vbusCfetOn = true;
    }
#else
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(pfet);
#endif /* defined(CY_DEVICE_PMG1S3) */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Vbus_NgdoOff
****************************************************************************//**
*
* Turns off the FET using the internal NGDO. This API is only applicable to PMG1S3.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param pfet
* Flag indicating whether the NGDO is connected to Provider FET
*  - 0: Connected to Consumer FET.
*  - 1: Connected to Provider FET.
*
*******************************************************************************/
void Cy_USBPD_Vbus_NgdoOff (cy_stc_usbpd_context_t *context, bool pfet)
{
#if defined(CY_DEVICE_PMG1S3)
    PPDSS_REGS_T pd = context->base;

    if(pfet == true)
    {
        context->vbusPfetOn = false;
    }
    else
    {
        context->vbusCfetOn = false;
    }
    pd->v2_ngdo_0_ctrl &= ~PDSS_V2_NGDO_0_CTRL_NGDO_GRV_EN;
    Cy_SysLib_DelayUs(20);
    pd->v2_ngdo_0_ctrl &= ~PDSS_V2_NGDO_0_CTRL_NGDO_CP_EN;
    Cy_SysLib_DelayUs(20);    
    pd->v2_ngdo_0_ctrl &= ~(PDSS_V2_NGDO_0_CTRL_NGDO_ISO_N | PDSS_V2_NGDO_0_CTRL_NGDO_EN_LV);
    Cy_SysLib_DelayUs(20);
    pd->v2_ngdo_0_ctrl&= ~(PDSS_V2_NGDO_0_CTRL_NGDO_FAULT_RCP_EN | PDSS_V2_NGDO_0_CTRL_NGDO_FAULT_SCP_EN | PDSS_V2_NGDO_0_CTRL_NGDO_FAULT_VBUS_OV_EN);
#else
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(pfet);
#endif /* defined(CY_DEVICE_PMG1S3) */
}

/** Vbus discharge drive strength at high voltages for PMG1S1. */
#define DISCHG_DRIVE_STRENGTH_VBUS_HI_REVA      (0x03UL)

/*******************************************************************************
* Function Name: Cy_USBPD_Vbus_DischargeOn
****************************************************************************//**
*
* Turn on the internal VBus discharge path.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
*******************************************************************************/
void Cy_USBPD_Vbus_DischargeOn(cy_stc_usbpd_context_t *context)
{
#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S))
    PPDSS_REGS_T pd = context->base;

#if !PMG1_FLIPPED_FET_CTRL
    /* This is VBUS_C discharge path. */
    pd->dischg_shv_ctrl[0] |= (PDSS_DISCHG_SHV_CTRL_DISCHG_EN | PDSS_DISCHG_SHV_CTRL_DISCHG_EN_CFG);
#else /* PMG1_FLIPPED_FET_CTRL */
    pd->dischg_shv_ctrl[1] |= (PDSS_DISCHG_SHV_CTRL_DISCHG_EN | PDSS_DISCHG_SHV_CTRL_DISCHG_EN_CFG);
#endif /* PMG1_FLIPPED_FET_CTRL */

#if VBUS_IN_DISCHARGE_EN
    Cy_USBPD_VbusIn_DischargeOn(context);
#endif /* VBUS_IN_DISCHARGE_EN */

#elif (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))
    PPDSS_REGS_T pd = context->base;
    uint32_t regVal;

    /* Enable the VBus discharge circuit. */
    regVal = pd->dischg_shv_ctrl;
    regVal &= ~PDSS_DISCHG_SHV_CTRL_DISCHG_DS_MASK;
    regVal |= (PDSS_DISCHG_SHV_CTRL_DISCHG_EN | PDSS_DISCHG_SHV_CTRL_DISCHG_EN_CFG |
              (DISCHG_DRIVE_STRENGTH_VBUS_HI_REVA << PDSS_DISCHG_SHV_CTRL_DISCHG_DS_POS));
    pd->dischg_shv_ctrl = regVal;

#elif (defined(CY_DEVICE_CCG3))

    PPDSS_REGS_T pd = context->base;
    pd->vbus_ctrl |= PDSS_VBUS_CTRL_DISCHG_EN;

#endif /* CY_DEVICE */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Vbus_DischargeOff
****************************************************************************//**
*
* Turn off the internal VBus discharge path.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
*******************************************************************************/
void Cy_USBPD_Vbus_DischargeOff(cy_stc_usbpd_context_t *context)
{
#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S))
    PPDSS_REGS_T pd = context->base;

#if !PMG1_FLIPPED_FET_CTRL
#if defined(CY_DEVICE_CCG7D)
    pd->dischg_shv_ctrl[0] = ((pd->dischg_shv_ctrl[0] & ~PDSS_DISCHG_SHV_CTRL_DISCHG_EN) |
        PDSS_DISCHG_SHV_CTRL_DISCHG_EN_CFG);
#else
    pd->dischg_shv_ctrl[0] = ((PDSS->dischg_shv_ctrl[0] & ~PDSS_DISCHG_SHV_CTRL_DISCHG_EN) |
        PDSS_DISCHG_SHV_CTRL_DISCHG_EN_CFG);
#endif /* defined(CY_DEVICE_CCG7D) */
#else  /* PMG1_FLIPPED_FET_CTRL */
#if defined(CY_DEVICE_CCG7D)
    pd->dischg_shv_ctrl[1] = ((pd->dischg_shv_ctrl[1] & ~PDSS_DISCHG_SHV_CTRL_DISCHG_EN) |
        PDSS_DISCHG_SHV_CTRL_DISCHG_EN_CFG);
#else /* defined(CY_DEVICE_CCG7D) */
    pd->dischg_shv_ctrl[1] = ((PDSS->dischg_shv_ctrl[1] & ~PDSS_DISCHG_SHV_CTRL_DISCHG_EN) |
        PDSS_DISCHG_SHV_CTRL_DISCHG_EN_CFG);
#endif /* defined(CY_DEVICE_CCG7D) */
#endif /* PMG1_FLIPPED_FET_CTRL */

#if VBUS_IN_DISCHARGE_EN
    Cy_USBPD_VbusIn_DischargeOff(context);
#endif /* VBUS_IN_DISCHARGE_EN */

#elif (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))

    PPDSS_REGS_T pd = context->base;

    /* Disable the discharge circuit. */
    pd->dischg_shv_ctrl &= ~PDSS_DISCHG_SHV_CTRL_DISCHG_EN;

#elif (defined(CY_DEVICE_CCG3))

    PPDSS_REGS_T pd = context->base;
    pd->vbus_ctrl &= ~PDSS_VBUS_CTRL_DISCHG_EN;

#endif /* CY_DEVICE */
}

/*******************************************************************************
* Function Name: Cy_USBPD_VbusIn_DischargeOn
****************************************************************************//**
*
* Turn on the internal VBUS_IN discharge path.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
*******************************************************************************/
void Cy_USBPD_VbusIn_DischargeOn(cy_stc_usbpd_context_t *context)
{
#if VBUS_IN_DISCHARGE_EN
    PPDSS_REGS_T pd = context->base;
#if !PMG1_FLIPPED_FET_CTRL
#if VBUS_IN_DISCH_BELOW_5V_EN
    pd->dischg_shv_ctrl[1] |= (PDSS_DISCHG_SHV_CTRL_DISCHG_EN | PDSS_DISCHG_SHV_CTRL_DISCHG_EN_CFG);
#else /* !VBUS_IN_DISCH_BELOW_5V_EN */
    /* Only discharge to 5V. */
    pd->dischg_shv_ctrl[1] &= ~(PDSS_DISCHG_SHV_CTRL_DISCHG_EN_CFG);
    /* Enable the comparator. */
    pd->comp_ctrl[CY_USBPD_VBUS_COMP_ID_VBUS_DISCHARGE] &= ~PDSS_COMP_CTRL_COMP_PD;
#endif /* VBUS_IN_DISCH_BELOW_5V_EN */
#else /* PMG1_FLIPPED_FET_CTRL */
    pd->dischg_shv_ctrl[0] |= (PDSS_DISCHG_SHV_CTRL_DISCHG_EN | PDSS_DISCHG_SHV_CTRL_DISCHG_EN_CFG);
#endif /* PMG1_FLIPPED_FET_CTRL */
#endif /* VBUS_IN_DISCHARGE_EN */
    CY_UNUSED_PARAMETER(context);
}


/*******************************************************************************
* Function Name: Cy_USBPD_VbusIn_DischargeOff
****************************************************************************//**
*
* Turn off the internal VBUS_IN discharge path.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
*******************************************************************************/
void Cy_USBPD_VbusIn_DischargeOff(cy_stc_usbpd_context_t *context)
{
#if VBUS_IN_DISCHARGE_EN
    PPDSS_REGS_T pd = context->base;
#if !PMG1_FLIPPED_FET_CTRL
#if !VBUS_IN_DISCH_BELOW_5V_EN
    pd->comp_ctrl[CY_USBPD_VBUS_COMP_ID_VBUS_DISCHARGE] |= PDSS_COMP_CTRL_COMP_PD;
#endif /* !VBUS_IN_DISCH_BELOW_5V_EN */
    pd->dischg_shv_ctrl[1] = ((pd->dischg_shv_ctrl[1] & ~PDSS_DISCHG_SHV_CTRL_DISCHG_EN) |
        PDSS_DISCHG_SHV_CTRL_DISCHG_EN_CFG);
#else /* PMG1_FLIPPED_FET_CTRL */
    pd->dischg_shv_ctrl[0] = ((pd->dischg_shv_ctrl[0] & ~PDSS_DISCHG_SHV_CTRL_DISCHG_EN) |
        PDSS_DISCHG_SHV_CTRL_DISCHG_EN_CFG);
#endif /* PMG1_FLIPPED_FET_CTRL */
#endif /* VBUS_IN_DISCHARGE_EN */
    CY_UNUSED_PARAMETER(context);
}

/*******************************************************************************
* Function Name: Cy_USBPD_Fault_FetAutoModeEnable
****************************************************************************//**
*
* Enable automatic hardware control on a gate driver due to a specified
* comparator and output filter. Does not affect automatic control of the gate
* driver by other comparators of filters.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param pctrl
* Flag indicating the type of gate driver to be controlled, true for
* P_CTRL and false for C_CTRL.
*
* \param filterIndex
* \ref cy_en_usbpd_vbus_filter_id_t enumerated type for various faults
*
*******************************************************************************/
void Cy_USBPD_Fault_FetAutoModeEnable(cy_stc_usbpd_context_t *context, bool pctrl, cy_en_usbpd_vbus_filter_id_t filterIndex)
{
#if ((defined(CY_DEVICE_CCG3PA)) || (defined(CY_DEVICE_CCG6)) || (defined(CY_DEVICE_PMG1S3)))
    PPDSS_REGS_T pd = context->base;

#if defined(CY_DEVICE_CCG3PA)
    uint32_t regVal;
    /* Provider FET. */
    if (pctrl)
    {
        regVal = pd->pgdo_pu_1_cfg;
        /* Enable auto mode. */
        regVal |= PDSS_PGDO_PU_1_CFG_AUTO_MODE;
        /* Program the off value to turn off the PFET. */
        regVal &= ~PDSS_PGDO_PU_1_CFG_PGDO_PU_EN_LV_OFF_VALUE;
        regVal |= PDSS_PGDO_PU_1_CFG_PGDO_PU_IN_LV_OFF_VALUE;
        pd->pgdo_pu_1_cfg = regVal;
        /* Select source. */
        pd->pgdo_pu_2_cfg |= 1UL << (uint8_t)filterIndex;
    }
    /* Consumer FET. */
    else
    {
        regVal = pd->pgdo_1_cfg;
        /* Enable auto mode. */
        regVal |= PDSS_PGDO_1_CFG_AUTO_MODE;
        /* Program the off value to turn off the PFET. */
        regVal &= ~PDSS_PGDO_1_CFG_PGDO_EN_LV_OFF_VALUE;
        pd->pgdo_1_cfg = regVal;
        /* Select source. */
        pd->pgdo_2_cfg |= 1UL << (uint8_t)filterIndex;
    }
#elif defined (CY_DEVICE_CCG6)
    uint32_t regVal;
    /* Provider FET. */
    if (pctrl)
    {
        /* Use PGDO_PU_1_CFG register for Provider FET */
        regVal = pd->pgdo_pu_1_cfg;

        /* Enable auto mode. */
        regVal |= PDSS_PGDO_PU_1_CFG_AUTO_MODE;

        /* Program the off value to turn off the PFET. */
        regVal &= ~PDSS_PGDO_PU_1_CFG_PGDO_PU_EN_LV_OFF_VALUE;
        pd->pgdo_pu_1_cfg = regVal;

        if (filterIndex == CY_USBPD_VBUS_FILTER_ID_HSCSA_SCP)
        {
            pd->pgdo_pu_3_cfg |= PDSS_PGDO_PU_3_CFG_PGDO_EN_SCP_LV;
        }
        else if (filterIndex == CY_USBPD_VBUS_FILTER_ID_HSCSA_RCP)
        {
            pd->pgdo_pu_3_cfg |= PDSS_PGDO_PU_3_CFG_PGDO_EN_RCP_LV | PDSS_PGDO_PU_3_CFG_PGDO_EN_VBUS_OV_LV;
        }
        else if (filterIndex == CY_USBPD_VBUS_FILTER_ID_HSCSA_OCP)
        {
            pd->pgdo_pu_1_cfg |= PDSS_PGDO_PU_1_CFG_SEL_CSA_OC;
        }
        else
        {
            pd->pgdo_pu_2_cfg |= 1UL << (uint8_t)filterIndex;
        }
    }
    /* Consumer FET. */
    else
    {
        regVal = pd->pgdo_1_cfg;
        /* Enable auto mode. */
        regVal |= PDSS_PGDO_1_CFG_AUTO_MODE;
        /* Program the off value to turn off the PFET. */
        regVal &= ~PDSS_PGDO_1_CFG_PGDO_EN_LV_OFF_VALUE;
        pd->pgdo_1_cfg = regVal;
        /* Select source. */
        pd->pgdo_2_cfg |= 1UL << (uint8_t)filterIndex;
    }
    
#elif defined(CY_DEVICE_PMG1S3)
    /* Provider FET. */
    if (pctrl)
    {
        if (filterIndex == CY_USBPD_VBUS_FILTER_ID_HSCSA_SCP)
        {
            pd->v2_ngdo_0_ctrl |= (PDSS_V2_NGDO_0_CTRL_NGDO_FAULT_SCP_EN);
        }
        else if (filterIndex == CY_USBPD_VBUS_FILTER_ID_HSCSA_RCP)
        {
            pd->v2_ngdo_0_ctrl |= (PDSS_V2_NGDO_0_CTRL_NGDO_FAULT_RCP_EN);
        }
        else
        {
            /* Do nothing */
        }
    }
    /* Consumer FET. */
    else
    {
        /* Do nothing. */
    }
#endif

#elif defined(CY_DEVICE_CCG3)

#if (VBUS_UVP_ENABLE || VBUS_OVP_ENABLE)
    PPDSS_REGS_T pd = context->base;
    uint32_t regval;

    if (pctrl)
    {
#if VBUS_UVP_ENABLE
        if (filterIndex == CY_USBPD_VBUS_FILTER_ID_UV)
        {
            regval = pd->ngdo_ctrl_p;

            regval |= PDSS_NGDO_CTRL_P_AUTO_MODE | PDSS_NGDO_CTRL_P_SEL_UV;
            regval |= PDSS_NGDO_CTRL_P_PULLDN_EN_LV_OFF_VALUE_MASK;
            regval &= ~PDSS_NGDO_CTRL_P_EN_LV_OFF_VALUE_MASK;

            pd->ngdo_ctrl_p = regval;

            pd->intr3_cfg_0 = (pd->intr3_cfg_0 & ~PDSS_INTR3_CFG_0_UV_NEG_FILT_SEL_MASK) |
                (0x03 << PDSS_INTR3_CFG_0_UV_NEG_FILT_SEL_POS) |
                PDSS_INTR3_CFG_0_UV_NEG_FILT_EN;
        }
#endif /* VBUS_UVP_ENABLE */

#if VBUS_OVP_ENABLE
        if (filterIndex == CY_USBPD_VBUS_FILTER_ID_OV)
        {
            regval = pd->ngdo_ctrl_p;

            regval |= PDSS_NGDO_CTRL_P_AUTO_MODE | PDSS_NGDO_CTRL_P_SEL_OV;
            regval |= PDSS_NGDO_CTRL_P_PULLDN_EN_LV_OFF_VALUE_MASK;
            regval &= ~PDSS_NGDO_CTRL_P_EN_LV_OFF_VALUE_MASK;

            pd->ngdo_ctrl_p = regval;

            pd->intr3_cfg_0 = (pd->intr3_cfg_0 & ~PDSS_INTR3_CFG_0_OV_POS_FILT_SEL_MASK) |
                (0x03 << PDSS_INTR3_CFG_0_OV_POS_FILT_SEL_POS) |
                PDSS_INTR3_CFG_0_OV_POS_FILT_EN;
        }
#endif /* VBUS_OVP_ENABLE */
    }
    else
    {
#if VBUS_UVP_ENABLE
        if (filterIndex == CY_USBPD_VBUS_FILTER_ID_UV)
        {
            regval = pd->ngdo_ctrl_c;

            regval |= PDSS_NGDO_CTRL_C_AUTO_MODE | PDSS_NGDO_CTRL_C_SEL_UV;
            regval |= PDSS_NGDO_CTRL_C_PULLDN_EN_LV_OFF_VALUE_MASK;
            regval &= ~PDSS_NGDO_CTRL_C_EN_LV_OFF_VALUE_MASK;

            pd->ngdo_ctrl_c = regval;

            pd->intr3_cfg_0 = (pd->intr3_cfg_0 & ~PDSS_INTR3_CFG_0_UV_NEG_FILT_SEL_MASK) |
                (0x03 << PDSS_INTR3_CFG_0_UV_NEG_FILT_SEL_POS) |
                PDSS_INTR3_CFG_0_UV_NEG_FILT_EN;
        }
#endif /* VBUS_UVP_ENABLE */

#if VBUS_OVP_ENABLE
        if (filterIndex == CY_USBPD_VBUS_FILTER_ID_OV)
        {
            regval = pd->ngdo_ctrl_c;

            regval |= PDSS_NGDO_CTRL_C_AUTO_MODE | PDSS_NGDO_CTRL_C_SEL_OV;
            regval |= PDSS_NGDO_CTRL_C_PULLDN_EN_LV_OFF_VALUE_MASK;
            regval &= ~PDSS_NGDO_CTRL_C_EN_LV_OFF_VALUE_MASK;

            pd->ngdo_ctrl_c = regval;

            pd->intr3_cfg_0 = (pd->intr3_cfg_0 & ~PDSS_INTR3_CFG_0_OV_POS_FILT_SEL_MASK) |
                (0x03 << PDSS_INTR3_CFG_0_OV_POS_FILT_SEL_POS) |
                PDSS_INTR3_CFG_0_OV_POS_FILT_EN;
        }
#endif /* VBUS_OVP_ENABLE */
    }
#endif /* ((VBUS_OVP_ENABLE) || (VBUS_UVP_ENABLE)) */

#endif /* CY_DEVICE */

    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(pctrl);
    CY_UNUSED_PARAMETER(filterIndex);
}


/*******************************************************************************
* Function Name: Cy_USBPD_Fault_FetAutoModeDisable
****************************************************************************//**
*
* Disable automatic hardware control on a gate driver due to a specified
 * comparator and output filter. Does not affect automatic control of the gate
 * driver by other comparators of filters.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param pctrl
* Flag indicating the type of gate driver to be controlled, true for
* P_CTRL and false for C_CTRL.
*
* \param filterIndex
* \ref cy_en_usbpd_vbus_filter_id_t enumerated type for various faults
*
*******************************************************************************/
void Cy_USBPD_Fault_FetAutoModeDisable(cy_stc_usbpd_context_t *context, bool pctrl, cy_en_usbpd_vbus_filter_id_t filterIndex)
{
#if ((defined(CY_DEVICE_CCG3PA)) || (defined(CY_DEVICE_CCG6)) || (defined(CY_DEVICE_PMG1S3)))
    PPDSS_REGS_T pd = context->base;

    (void)pd;
#if defined(CY_DEVICE_CCG3PA)
    uint32_t regValCfg1;
    uint32_t regValCfg2;

    if (pctrl)
    {
        /* Remove source. */
        regValCfg1 = pd->pgdo_pu_1_cfg;
        regValCfg2 = pd->pgdo_pu_2_cfg;

        regValCfg2 &= ~(1UL << (uint8_t)filterIndex);

        /* Disable Auto mode if no other source is active. */
        if ((regValCfg2 == 0U) && ((regValCfg1 & PGDO_PU_1_CFG_AUTO_SEL_MASK) == 0U))
        {
           regValCfg1 &= ~PDSS_PGDO_PU_1_CFG_AUTO_MODE;
        }
        pd->pgdo_pu_2_cfg = regValCfg2;
        pd->pgdo_pu_1_cfg = regValCfg1;
    }
    else
    {
        /* Remove source. */
        regValCfg1 = pd->pgdo_1_cfg;
        regValCfg2 = pd->pgdo_2_cfg;

        regValCfg2 &= ~(1UL << (uint8_t)filterIndex);

        if ((regValCfg2 == 0U) && ((regValCfg1 & PGDO_1_CFG_AUTO_SEL_MASK) == 0U))
        {
            regValCfg1 &= ~PDSS_PGDO_1_CFG_AUTO_MODE;
        }
        pd->pgdo_2_cfg = regValCfg2;
        pd->pgdo_1_cfg = regValCfg1;
    }
#elif defined(CY_DEVICE_CCG6)
    if (pctrl)
    {
        /* Use PGDO_PU_1_CFG register for Provider FET */

        /* Remove source. */
        if (filterIndex == CY_USBPD_VBUS_FILTER_ID_HSCSA_SCP)
        {
            pd->pgdo_pu_3_cfg &= ~PDSS_PGDO_PU_3_CFG_PGDO_EN_SCP_LV;
        }
        else if(filterIndex == CY_USBPD_VBUS_FILTER_ID_HSCSA_RCP)
        {
            pd->pgdo_pu_3_cfg &= ~(PDSS_PGDO_PU_3_CFG_PGDO_EN_RCP_LV | PDSS_PGDO_PU_3_CFG_PGDO_EN_VBUS_OV_LV);
        }
        else if (filterIndex == CY_USBPD_VBUS_FILTER_ID_HSCSA_OCP)
        {
            pd->pgdo_pu_1_cfg &= ~PDSS_PGDO_PU_1_CFG_SEL_CSA_OC;
        }
        else
        {
            pd->pgdo_pu_2_cfg &= ~(1UL << (uint8_t)filterIndex);
        }

        /* Disable Auto mode if no other source is active. */
        if ((pd->pgdo_pu_2_cfg == 0U) && ((pd->pgdo_pu_1_cfg & PGDO_1_CFG_AUTO_SEL_MASK) == 0U))
        {
            pd->pgdo_pu_1_cfg &= ~PDSS_PGDO_1_CFG_AUTO_MODE;
        }
    }
    else
    {
        /* Remove source. */
        pd->pgdo_2_cfg &= ~(1UL << (uint8_t)filterIndex);
        /* Disable Auto mode if no other source is active. */
        if ((pd->pgdo_2_cfg == 0U) && ((pd->pgdo_1_cfg & PGDO_1_CFG_AUTO_SEL_MASK) == 0U))
        {
            pd->pgdo_1_cfg &= ~PDSS_PGDO_1_CFG_AUTO_MODE;
        }
    }
#elif defined(CY_DEVICE_PMG1S3)
    /* Provider FET. */
    if (pctrl)
    {
        if (filterIndex == CY_USBPD_VBUS_FILTER_ID_HSCSA_SCP)
        {
            pd->v2_ngdo_0_ctrl &= ~(PDSS_V2_NGDO_0_CTRL_NGDO_FAULT_SCP_EN);
        }
        else if (filterIndex == CY_USBPD_VBUS_FILTER_ID_HSCSA_RCP)
        {
            pd->v2_ngdo_0_ctrl &= ~(PDSS_V2_NGDO_0_CTRL_NGDO_FAULT_RCP_EN);
        }
        else
        {
            /* Do nothing */
        }
    }
    /* Consumer FET. */
    else
    {
        /* Do nothing. */
    }
#endif /* CY_DEVICE */

#elif defined(CY_DEVICE_CCG3)

#if (VBUS_UVP_ENABLE || VBUS_OVP_ENABLE)
    PPDSS_REGS_T pd = context->base;
    uint32_t regval;

    if (pctrl)
    {
#if VBUS_UVP_ENABLE
        if (filterIndex == CY_USBPD_VBUS_FILTER_ID_UV)
        {
            regval = pd->ngdo_ctrl_p;

            regval &= ~PDSS_NGDO_CTRL_P_SEL_UV;
            if ((regval & (PDSS_NGDO_CTRL_P_SEL_OV | PDSS_NGDO_CTRL_P_SEL_OC)) == 0)
            {
                regval &= ~PDSS_NGDO_CTRL_P_AUTO_MODE;
            }

            pd->ngdo_ctrl_p = regval;
        }
#endif /* VBUS_UVP_ENABLE */

#if VBUS_OVP_ENABLE
        if (filterIndex == CY_USBPD_VBUS_FILTER_ID_OV)
        {
            regval = pd->ngdo_ctrl_p;

            regval &= ~PDSS_NGDO_CTRL_P_SEL_OV;
            if ((regval & (PDSS_NGDO_CTRL_P_SEL_UV | PDSS_NGDO_CTRL_P_SEL_OC)) == 0)
            {
                regval &= ~PDSS_NGDO_CTRL_P_AUTO_MODE;
            }

            pd->ngdo_ctrl_p = regval;
        }
#endif /* VBUS_OVP_ENABLE */
    }
    else
    {
#if VBUS_UVP_ENABLE
        if (filterIndex == CY_USBPD_VBUS_FILTER_ID_UV)
        {
            regval = pd->ngdo_ctrl_c;

            regval &= ~PDSS_NGDO_CTRL_C_SEL_UV;
            if ((regval & (PDSS_NGDO_CTRL_C_SEL_OV | PDSS_NGDO_CTRL_C_SEL_OC)) == 0)
            {
                regval &= ~PDSS_NGDO_CTRL_C_AUTO_MODE;
            }

            pd->ngdo_ctrl_c = regval;
        }
#endif /* VBUS_UVP_ENABLE */

#if VBUS_OVP_ENABLE
        if (filterIndex == CY_USBPD_VBUS_FILTER_ID_OV)
        {
            regval = pd->ngdo_ctrl_c;

            regval &= ~PDSS_NGDO_CTRL_C_SEL_OV;
            if ((regval & (PDSS_NGDO_CTRL_C_SEL_UV | PDSS_NGDO_CTRL_C_SEL_OC)) == 0)
            {
                regval &= ~PDSS_NGDO_CTRL_C_AUTO_MODE;
            }

            pd->ngdo_ctrl_c = regval;
        }
#endif /* VBUS_OVP_ENABLE */
    }
#endif /* (VBUS_UVP_ENABLE || VBUS_OVP_ENABLE) */

#endif /* CY_DEVICE */

    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(pctrl);
    CY_UNUSED_PARAMETER(filterIndex);
}


/*******************************************************************************
* Function Name: Cy_USBPD_Fault_Vconn_OcpEnable
****************************************************************************//**
*
* Enable Over-Current detection on the VConn power source.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param cb
* Callback function to be called on fault detection.
*
*******************************************************************************/
void Cy_USBPD_Fault_Vconn_OcpEnable(cy_stc_usbpd_context_t *context, cy_cb_vbus_fault_t cb)
{
#if (defined (VCONN_OCP_ENABLE) && (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)))
    PPDSS_REGS_T pd = context->base;
    uint32_t state;
    uint32_t regVal, ccInt;

    state = Cy_SysLib_EnterCriticalSection();

    /* Store the parameters. */
    context->vconnOcpCbk = cb;

    /* Configure the filter on the appropriate CC line. */
    pd->intr1_cfg_cc12_ocp_hs = 0;
    regVal = 0;

    if (context->dpmGetConfig()->polarity == CY_PD_CC_CHANNEL_1)
    {
        /* Enable filter with positive edge detection with 4 HF clock cycle period. */
        regVal = (
                ((uint32_t)CY_USBPD_VBUS_FILTER_CFG_POS_EN_NEG_DIS << PDSS_INTR1_CFG_CC12_OCP_HS_CC2_OCP_FILT_CFG_POS) |
                (4UL << PDSS_INTR1_CFG_CC12_OCP_HS_CC2_OCP_FILT_SEL_POS) |
                PDSS_INTR1_CFG_CC12_OCP_HS_CC2_OCP_DPSLP_MODE |
                PDSS_INTR1_CFG_CC12_OCP_HS_CC2_OCP_FILT_EN
                );
        ccInt = PDSS_INTR1_CC2_OCP_CHANGED;

        /* Enable OC detection on CC2 pin. */
        pd->vconn20_ctrl |= PDSS_VCONN20_CTRL_EN_OCP_CC2;
    }
    else
    {
        regVal = (
                ((uint32_t)CY_USBPD_VBUS_FILTER_CFG_POS_EN_NEG_DIS << PDSS_INTR1_CFG_CC12_OCP_HS_CC1_OCP_FILT_CFG_POS) |
                (4UL << PDSS_INTR1_CFG_CC12_OCP_HS_CC1_OCP_FILT_SEL_POS) |
                PDSS_INTR1_CFG_CC12_OCP_HS_CC1_OCP_DPSLP_MODE |
                PDSS_INTR1_CFG_CC12_OCP_HS_CC1_OCP_FILT_EN
                );
        ccInt = PDSS_INTR1_CC1_OCP_CHANGED;

        /* Enable OC detection on CC1 pin. */
        pd->vconn20_ctrl |= PDSS_VCONN20_CTRL_EN_OCP_CC1;
    }

    /* Set the VConn OCP configuration. */
    pd->intr1_cfg_cc12_ocp_hs = regVal;

    /* Enable the VCONN OCP trim current and comparator */
    pd->vconn20_ctrl |= ((1UL << 6) | (1UL << 10));

    /* Clear and enable the interrupt associated with OC detection on CC lines. */
    pd->intr1 = (PDSS_INTR1_CC1_OCP_CHANGED | PDSS_INTR1_CC2_OCP_CHANGED);

    /* Enable the interrupt. */
    pd->intr1_mask |= ccInt;

    Cy_SysLib_ExitCriticalSection (state);

#else
    /* Avoid compiler warnings about unused arguments. */
    CY_UNUSED_PARAMETER(context);
    (void)(cy_cb_vbus_fault_t)cb;
#endif /* (defined (VCONN_OCP_ENABLE) && (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))) */
}


/*******************************************************************************
* Function Name: Cy_USBPD_Fault_Vconn_OcpDisable
****************************************************************************//**
*
* Disable Over-Current detection on the VConn power source.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
*******************************************************************************/
void Cy_USBPD_Fault_Vconn_OcpDisable(cy_stc_usbpd_context_t *context)
{
#if defined(VCONN_OCP_ENABLE)
#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))
    PPDSS_REGS_T pd = context->base;

    /* Disable OC detection on CC pins. */
    pd->vconn20_ctrl &= ~(PDSS_VCONN20_CTRL_EN_OCP_CC1 | PDSS_VCONN20_CTRL_EN_OCP_CC2);
    /* Disable the VCONN OCP trim current and comparator */
    pd->vconn20_ctrl &= ~((1UL << 6) | (1UL << 10));

    /* Disable the filter block associated with CC12 OC detection. */
    pd->intr1_cfg_cc12_ocp_hs = 0;

    /* Disable and clear the CC12 OC changed interrupt. */
    pd->intr1_mask &= ~(PDSS_INTR1_CC1_OCP_CHANGED | PDSS_INTR1_CC2_OCP_CHANGED);
    pd->intr1       = (PDSS_INTR1_CC1_OCP_CHANGED | PDSS_INTR1_CC2_OCP_CHANGED);

    /* Clear the stored configuration. */
    context->vconnOcpCbk = NULL;
#endif /* (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)) */
#endif /* defined(VCONN_OCP_ENABLE) */

    CY_UNUSED_PARAMETER(context);
}


/*******************************************************************************
* Function Name: Cy_USBPD_Fault_Vconn_OcpIntrHandler
****************************************************************************//**
*
* * VCONN OCP fault interrupt handler function.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
*******************************************************************************/
void Cy_USBPD_Fault_Vconn_OcpIntrHandler(cy_stc_usbpd_context_t *context)
{
#if defined(VCONN_OCP_ENABLE)
#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))
    PPDSS_REGS_T pd = context->base;
    uint32_t cfgMask, cfgPos;
    uint32_t statusMask;

    /* Disable the interrupt to start with. */
    pd->intr1_mask &= ~(PDSS_INTR1_CC1_OCP_CHANGED | PDSS_INTR1_CC2_OCP_CHANGED);

    /* Identify the register fields of interest. */
    if (context->dpmGetConfig()->polarity == CY_PD_CC_CHANNEL_1)
    {
        cfgMask  = PDSS_INTR1_CFG_CC12_OCP_HS_CC2_OCP_FILT_CFG_MASK;
        cfgPos   = PDSS_INTR1_CFG_CC12_OCP_HS_CC2_OCP_FILT_CFG_POS;
        statusMask = PDSS_INTR1_STATUS_CC2_OCP_FILT;
    }
    else
    {
        cfgMask  = PDSS_INTR1_CFG_CC12_OCP_HS_CC1_OCP_FILT_CFG_MASK;
        cfgPos   = PDSS_INTR1_CFG_CC12_OCP_HS_CC1_OCP_FILT_CFG_POS;
        statusMask = PDSS_INTR1_STATUS_CC1_OCP_FILT;
    }

    /* If positive edge interrupt: Current exceeded positive threshold. */
    if (((pd->intr1_cfg_cc12_ocp_hs & cfgMask) >> cfgPos) == (uint32_t)CY_USBPD_VBUS_FILTER_CFG_POS_EN_NEG_DIS)
    {
        /* Look for negative edge of comparator. */
        pd->intr1_cfg_cc12_ocp_hs = Cy_USBPD_MmioRegUpdateField(pd->intr1_cfg_cc12_ocp_hs,
                (uint32_t)CY_USBPD_VBUS_FILTER_CFG_POS_DIS_NEG_EN, cfgMask, (uint8_t)cfgPos);

        /* If the negative edge happened already, queue the interrupt. */
        if ((pd->intr1_status & statusMask) == 0U)
        {
            if (context->dpmGetConfig()->polarity == CY_PD_CC_CHANNEL_1)
            {
                pd->intr1_set = PDSS_INTR1_SET_CC2_OCP_CHANGED;
            }
            else
            {
                pd->intr1_set = PDSS_INTR1_SET_CC1_OCP_CHANGED;
            }
        }

        (void)context->vconnOcpCbk (context, true);
    }
    /* If negative edge interrupt: Current is back within limit. */
    else
    {
        bool retval = context->vconnOcpCbk (context, false);
        if (retval == true)
        {
            /* Look for positive edge of comparator. */
            pd->intr1_cfg_cc12_ocp_hs = Cy_USBPD_MmioRegUpdateField(pd->intr1_cfg_cc12_ocp_hs,
                    (uint32_t)CY_USBPD_VBUS_FILTER_CFG_POS_EN_NEG_DIS, cfgMask, (uint8_t)cfgPos);

            /* If the positive edge happened already, queue the interrupt. */
            if ((pd->intr1_status & statusMask) != 0U)
            {
                if (context->dpmGetConfig()->polarity == CY_PD_CC_CHANNEL_1)
                {
                    pd->intr1_set = PDSS_INTR1_SET_CC2_OCP_CHANGED;
                }
                else
                {
                    pd->intr1_set = PDSS_INTR1_SET_CC1_OCP_CHANGED;
                }
            }
        }
    }

    /* Enable interrupt. */
    if (context->dpmGetConfig()->polarity == CY_PD_CC_CHANNEL_1)
    {
        pd->intr1_mask |= PDSS_INTR1_CC2_OCP_CHANGED;
    }
    else
    {
        pd->intr1_mask |= PDSS_INTR1_CC1_OCP_CHANGED;
    }

#endif /* (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)) */
#endif /* defined(VCONN_OCP_ENABLE) */
    CY_UNUSED_PARAMETER(context);
}

/* Function to gradually modify a refgen output voltage so as to prevent perturbations on other outputs. */
void Cy_USBPD_SetRefgenVoltage(cy_stc_usbpd_context_t *context, uint8_t vrefSel, uint8_t vrefSelPos)
{
#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))
    PPDSS_REGS_T pd = context->base;
    uint32_t vrefOldVal = 0;
    uint32_t mask = 0xFF;
    uint8_t  pos = 0;
    volatile uint32_t *regAddr = &pd->refgen_4_ctrl;
    uint32_t state;

    state = Cy_SysLib_EnterCriticalSection ();
    switch (vrefSelPos)
    {
        case 0:
        case 1:
        case 2:
        case 3:
            regAddr = &(pd->refgen_1_ctrl);
            pos      = (vrefSelPos * 8U);
            break;

        case 4:
        case 5:
        case 6:
        case 7:
            regAddr = &(pd->refgen_2_ctrl);
            pos      = (vrefSelPos - 4U) * 8U;
            break;

        case 8:
        case 9:
        case 10:
            regAddr = &(pd->refgen_3_ctrl);
            pos      = (vrefSelPos - 8U) * 8U;
            break;

        default:
            /* References in REFGEN_4 are three bits wide. */
            pos      = (vrefSelPos - 12U) * 3U;
            mask     = 0x07U;
            break;
    }

    vrefOldVal = (*regAddr >> pos) & mask;
    if (vrefSel > vrefOldVal)
    {
        while ((vrefSel - vrefOldVal) > 20U)
        {
            vrefOldVal += 20U;
            *regAddr  = (*regAddr & ~(mask << pos)) | ((vrefOldVal << pos) & (mask << pos));
            Cy_SysLib_DelayUs(10);
        }
    }
    else
    {
        while ((vrefOldVal - vrefSel) > 20U)
        {
            vrefOldVal -= 20U;
            *regAddr  = (*regAddr & ~(mask << pos)) | ((vrefOldVal << pos) & (mask << pos));
            Cy_SysLib_DelayUs(10);
        }
    }

    *regAddr  = (*regAddr & ~(mask << pos)) | (((uint32_t)vrefSel << pos) & (mask << pos));
    Cy_SysLib_ExitCriticalSection (state);
#else
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(vrefSel);
    CY_UNUSED_PARAMETER(vrefSelPos);
#endif /* (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)) */
}

#if VBUS_OVP_ENABLE
/* OVP min reference voltage in mV. */
#define OVP_REF_VOLT_MIN            (200u)

/* Minimum voltage for VREF8 in mV. */
#define OVP_REF8_VOLT_MIN           (130u)

/* OVP reference voltage step size in mV. */
#define OVP_REF_VOLT_STEP           (10u)

/* Min OVP detection level for VBus. */
#define VBUS_OVP_DETECT_MIN         (6000u)

#if (defined(CY_DEVICE_PMG1S3) && CY_PD_EPR_ENABLE)
/* VBUS max voltage in mV. */
#define VBUS_VOLT_MAX               (30000u)
#else
/* VBUS max voltage in mV. */
#define VBUS_VOLT_MAX               (24000u)
#endif /* defined(CY_DEVICE_PMG1S3) */

/* Max. VREF setting. */
#define VREF_MAX_SETTING            (199u)

#define MAX_OVP_DEBOUNCE_CYCLES         (0x20u)
#endif /* VBUS_OVP_ENABLE */
/*******************************************************************************
* Function Name: Cy_USBPD_Fault_Vbus_OvpEnable
****************************************************************************//**
*
* Enable Over Voltage Protection (OVP) control using the internal UV-OV block.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param volt
* Contract Voltage in mV units.
*
* \param cb
* Callback function to be called on fault detection.
*
* \param pctrl
* Flag indicating the type of gate driver to be controlled, true for
* P_CTRL and false for C_CTRL.
*
*******************************************************************************/
void Cy_USBPD_Fault_Vbus_OvpEnable(cy_stc_usbpd_context_t *context, uint16_t volt, cy_cb_vbus_fault_t cb, bool pctrl)
{
#if VBUS_OVP_ENABLE
    PPDSS_REGS_T pd = context->base;
    uint32_t regVal;
    uint16_t threshold;
    uint8_t filterSel;

#if defined(CY_IP_MXUSBPD)
    uint32_t divPos = AMUX_OV_DIV_SEL_BIT_POS;
    uint16_t vref;
#endif /* defined(CY_IP_MXUSBPD) */

    filterSel = (context->usbpdConfig->vbusOvpConfig->debounce + 1) / 2;
    filterSel = CY_USBPD_GET_MIN (filterSel, MAX_OVP_DEBOUNCE_CYCLES);

    /* Clear AUTO MODE OVP detect to avoid false auto off during reference change */
    if (context->usbpdConfig->vbusOvpConfig->mode == CY_USBPD_VBUS_OVP_MODE_UVOV_AUTOCTRL)
    {
        Cy_USBPD_Fault_FetAutoModeDisable (context, pctrl, CY_USBPD_VBUS_FILTER_ID_OV);
    }

    /* Store OVP parameters. */
    context->vbusOvpCbk = cb;

    /* Calculate required VBUS for OVP. */
    threshold = volt + ((volt * context->usbpdConfig->vbusOvpConfig->threshold) / 100);

#if defined(CY_IP_MXUSBPD)

    /* Cap the maximum voltage to be measured. */
    if (threshold > VBUS_VOLT_MAX)
        threshold = VBUS_VOLT_MAX;
    /* Make sure threshold is above the minimum trip point to avoid false triggers. */
    if (threshold < VBUS_OVP_DETECT_MIN)
        threshold = VBUS_OVP_DETECT_MIN;

    regVal = pd->amux_nhv_ctrl;

#if defined(CY_DEVICE_CCG3PA)
#if !PMG1_FLIPPED_FET_CTRL
    /* Choose VBUS_C as source for AMUX. */
     regVal &= ~(AMUX_OV_VBUS_SEL_MASK);
#else
    /* As TYPE C VBUS is connected to VBUS_IN node, choose VBUS_IN as source. */
    regVal |= AMUX_OV_VBUS_SEL_MASK;
#endif /* PMG1_FLIPPED_FET_CTRL */
#endif /* defined(CY_DEVICE_CCG3PA) */

    /*
     * Internal divider to be used depends on VBUS to be measured.
     * On PMG1S0: for VBUS < VBUS_DIV_20_PER_MAX_VOLT, use 20% divider and for greater, use 8% divider.
     * On PMG1S1: Always use 8% divider.
     * On PMG1S3: Use 6% divider.
     */

#if defined(CY_DEVICE_CCG3PA)
    if (threshold <= VBUS_DIV_20_PER_MAX_VOLT)
    {
        vref = threshold / VBUS_C_20_PER_DIV;

        /* Connect output to 20% */
        regVal |= (1U << divPos);
    }
    else
#endif /* defined(CY_DEVICE_CCG3PA) */
    {

        {
#if defined(CY_DEVICE_PMG1S3)
            /* Get 6% of threshold. */
            vref = (threshold * 6) / 100;
#else            
            /* Get 8% of threshold. */
            vref = (threshold * 10) / 125;
#endif /* defined(CY_DEVICE_PMG1S3) */
        }

        /* Connect output to divider */
        regVal &= ~(1U << divPos);
    }

    pd->amux_nhv_ctrl = regVal;

    /* Calculate the actual reference voltage. Cap the value to the max. supported. */
    vref = ((vref - OVP_REF_VOLT_MIN) / OVP_REF_VOLT_STEP);
    if (vref > VREF_MAX_SETTING)
        vref = VREF_MAX_SETTING;

    /* Program reference voltage for OV comparator. */

    {
#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))
        Cy_USBPD_SetRefgenVoltage (context, vref, CY_USBPD_VREF_VBUS_OV);
#else
        /* OV comparator for Port 0 uses VREF[3]. */
        regVal = pd->refgen_1_ctrl;
        regVal &= ~(PDSS_REFGEN_1_CTRL_SEL3_MASK);
        regVal |= (vref << PDSS_REFGEN_1_CTRL_SEL3_POS);
        pd->refgen_1_ctrl = regVal;
#endif /* (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)) */
    }

    /* Turn on comparator. */
    pd->comp_ctrl[CY_USBPD_VBUS_COMP_ID_OV] |= PDSS_COMP_CTRL_COMP_ISO_N;
    pd->comp_ctrl[CY_USBPD_VBUS_COMP_ID_OV] &= ~PDSS_COMP_CTRL_COMP_PD;

    Cy_SysLib_DelayUs(10);

    /* Filter configuration. */
    pd->intr5_filter_cfg[CY_USBPD_VBUS_FILTER_ID_OV] &= ~PDSS_INTR5_FILTER_CFG_FILT_EN;
    regVal = pd->intr5_filter_cfg[CY_USBPD_VBUS_FILTER_ID_OV] & ~(PDSS_INTR5_FILTER_CFG_FILT_CFG_MASK
            | PDSS_INTR5_FILTER_CFG_FILT_SEL_MASK | PDSS_INTR5_FILTER_CFG_DPSLP_MODE |
            PDSS_INTR5_FILTER_CFG_FILT_BYPASS | PDSS_INTR5_FILTER_CFG_FILT_RESET);

    /* Set filter clock cycles if filter is required. */
    if (filterSel)
    {
        /* Subtracting 1 from filter clock cycle value as 0 translates to 1-2 clock cycles. */
        regVal |= (((filterSel - 1) & 0x1F) << PDSS_INTR5_FILTER_CFG_FILT_SEL_POS);

        /* Set edge detection. */
        regVal |= (CY_USBPD_VBUS_FILTER_CFG_POS_EN_NEG_DIS << PDSS_INTR5_FILTER_CFG_FILT_CFG_POS);
        regVal |= PDSS_INTR5_FILTER_CFG_FILT_EN | PDSS_INTR5_FILTER_CFG_DPSLP_MODE;
    }
    else
    {
        regVal |= PDSS_INTR5_FILTER_CFG_FILT_BYPASS;
    }

    pd->intr5_filter_cfg[CY_USBPD_VBUS_FILTER_ID_OV] = regVal;

    /* Clear interrupt. */
    pd->intr5 = (1U << CY_USBPD_VBUS_FILTER_ID_OV);

    /* Enable Interrupt. */
    pd->intr5_mask |= (1U << CY_USBPD_VBUS_FILTER_ID_OV);

    /* Enable Auto mode. */
    if (context->usbpdConfig->vbusOvpConfig->mode == CY_USBPD_VBUS_OVP_MODE_UVOV_AUTOCTRL)
    {
        Cy_USBPD_Fault_FetAutoModeEnable (context, pctrl, CY_USBPD_VBUS_FILTER_ID_OV);
    }

#elif defined(CY_IP_M0S8USBPD)

/*
 *  Input ladder voltages, code limits and step sizes from Table 26 of the
 *  HardIP BROS (001-98391).
 */
#define UVOV_LADDER_BOT     (2750u)
#define UVOV_LADDER_MID     (9000u)
#define UVOV_LADDER_TOP     (21500u)
#define UVOV_CODE_BOT       (0)
#define UVOV_CODE_MID       (25u)
#define UVOV_CODE_TOP       (50u)
#define UVOV_CODE_MAX       (63u)
#define UVOV_CODE_6V0       (13u)
#define UVOV_LO_STEP_SZ     (250u)
#define UVOV_HI_STEP_SZ     (500u)

    uint8_t level;

    /*
     * Calculate OVP comparator threshold setting.
     */
    if (threshold < UVOV_LADDER_BOT)
    {
        level = UVOV_CODE_BOT;
    }
    else if (threshold > UVOV_LADDER_TOP)
    {
        level = UVOV_CODE_TOP;
    }
    else if (threshold <= UVOV_LADDER_MID)
    {
        level = (uint8_t)((threshold - UVOV_LADDER_BOT) / UVOV_LO_STEP_SZ);
    }
    else
    {
        level = (uint8_t)(((threshold - UVOV_LADDER_MID) / UVOV_HI_STEP_SZ) + UVOV_CODE_MID);
    }

    /* Clear OVP positive edge notification. */
    pd->intr3 = PDSS_INTR3_POS_OV_CHANGED;

    /* Configure the UVOV block. */
    regVal = pd->uvov_ctrl & ~(PDSS_UVOV_CTRL_OV_IN_MASK | PDSS_UVOV_CTRL_PD_UVOV);
    regVal |= PDSS_UVOV_CTRL_UVOV_ISO_N;
    pd->uvov_ctrl = (level << PDSS_UVOV_CTRL_OV_IN_POS) | regVal;

    if (context->usbpdConfig->vbusOvpConfig->mode == CY_USBPD_VBUS_OVP_MODE_UVOV_AUTOCTRL)
    {
        Cy_USBPD_Fault_FetAutoModeEnable (context, pctrl, CY_USBPD_VBUS_FILTER_ID_OV);
        Cy_SysLib_DelayUs (10);
    }

    /* If the OV_DET output is already high, flag it. */
    if (pd->ncell_status & PDSS_NCELL_STATUS_OV_STATUS)
    {
        pd->intr3_set |= PDSS_INTR3_POS_OV_CHANGED;
    }

    /* Enable OVP positive edge detection. */
    pd->intr3_mask |= PDSS_INTR3_POS_OV_CHANGED;

#endif /* CY_IP */
#else
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(volt);
    (void)(cy_cb_vbus_fault_t)cb;
    CY_UNUSED_PARAMETER(pctrl);
#endif /* VBUS_OVP_ENABLE */
}


/*******************************************************************************
* Function Name: Cy_USBPD_Fault_Vbus_OvpDisable
****************************************************************************//**
*
* Disable Over Voltage Protection (OVP) control.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param pctrl
* Flag indicating the type of gate driver to be controlled, true for
* P_CTRL and false for C_CTRL.
*
*******************************************************************************/
void Cy_USBPD_Fault_Vbus_OvpDisable(cy_stc_usbpd_context_t *context, bool pctrl)
{
#if VBUS_OVP_ENABLE
#if defined(CY_IP_MXUSBPD)
    PPDSS_REGS_T pd = context->base;
    uint32_t state;
    uint16_t vref;

    state = Cy_SysLib_EnterCriticalSection();

    /* Clear AUTO MODE OVP detect. */
    if (context->usbpdConfig->vbusOvpConfig->mode == CY_USBPD_VBUS_OVP_MODE_UVOV_AUTOCTRL)
    {
        Cy_USBPD_Fault_FetAutoModeDisable (context, pctrl, CY_USBPD_VBUS_FILTER_ID_OV);
    }

    /* Disable comparator. */
    pd->comp_ctrl[CY_USBPD_VBUS_COMP_ID_OV] &= ~PDSS_COMP_CTRL_COMP_ISO_N;
    pd->comp_ctrl[CY_USBPD_VBUS_COMP_ID_OV] |= PDSS_COMP_CTRL_COMP_PD;

     /* Disable filter. */
    pd->intr5_filter_cfg[CY_USBPD_VBUS_FILTER_ID_OV] &= ~(PDSS_INTR5_FILTER_CFG_FILT_EN |
        PDSS_INTR5_FILTER_CFG_FILT_CFG_MASK | PDSS_INTR5_FILTER_CFG_FILT_SEL_MASK |
        PDSS_INTR5_FILTER_CFG_FILT_BYPASS | PDSS_INTR5_FILTER_CFG_DPSLP_MODE);

    /* Disable and clear OV interrupts. */
    pd->intr5_mask &= ~(1U << CY_USBPD_VBUS_FILTER_ID_OV);
    pd->intr5 = 1U << CY_USBPD_VBUS_FILTER_ID_OV;

    /* Connect OV comparator input to 20% / 10% of VBus. */
    pd->amux_nhv_ctrl |= (1U << AMUX_OV_DIV_SEL_BIT_POS);

    /* Restore reference voltage for OV comparator to 6V equivalent. */
#if defined(CY_DEVICE_CCG3PA)
    vref = VBUS_OVP_DETECT_MIN / VBUS_C_20_PER_DIV;
#else /* defined (CY_DEVICE_CCG6) */
    vref = VBUS_OVP_DETECT_MIN / VBUS_C_10_PER_DIV;
#endif /* defined(CY_DEVICE_CCG3PA)  */
    vref = (vref - OVP_REF_VOLT_MIN) / OVP_REF_VOLT_STEP;

    {
        /* OV comparator on port 0 uses VREF[3]. */
        pd->refgen_1_ctrl = (pd->refgen_1_ctrl & ~PDSS_REFGEN_1_CTRL_SEL3_MASK) | (vref << PDSS_REFGEN_1_CTRL_SEL3_POS);
    }

    /* Clear callback. */
   context->vbusOvpCbk = NULL;

    Cy_SysLib_ExitCriticalSection(state);

#elif defined(CY_IP_M0S8USBPD)

    PPDSS_REGS_T pd = context->base;
    uint32_t regval;

    Cy_USBPD_Fault_FetAutoModeDisable (context, pctrl, CY_USBPD_VBUS_FILTER_ID_OV);

    /* Disable the UVOV block, and set the OV threshold to 6V by default. */
    regval = pd->uvov_ctrl;
    regval = ((regval & ~PDSS_UVOV_CTRL_OV_IN_MASK) | (UVOV_CODE_6V0 << PDSS_UVOV_CTRL_OV_IN_POS));
    pd->uvov_ctrl = regval;

    /* Disable and clear UVOV interrupts. */
    pd->intr3_mask &= ~PDSS_INTR3_POS_OV_CHANGED;
    pd->intr3 = PDSS_INTR3_POS_OV_CHANGED;

    /* Clear callback. */
   context->vbusOvpCbk = NULL;

#endif /* CY_IP */
#else
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(pctrl);
#endif /* VBUS_OVP_ENABLE */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Fault_Vbus_OvpIntrHandler
****************************************************************************//**
*
* VBUS OVP fault interrupt handler function.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
*******************************************************************************/
void Cy_USBPD_Fault_Vbus_OvpIntrHandler(cy_stc_usbpd_context_t *context)
{
#if VBUS_OVP_ENABLE
    PPDSS_REGS_T pd = context->base;

#if defined(CY_IP_MXUSBPD)
    /* Disable and clear OV interrupt. */
    pd->intr5_mask &= ~(1U << CY_USBPD_VBUS_FILTER_ID_OV);
    pd->intr5 = 1U << CY_USBPD_VBUS_FILTER_ID_OV;

    /* Invoke OVP callback. */
    if (context->vbusOvpCbk != NULL)
    {
        context->vbusOvpCbk(context, true);
    }
#elif defined(CY_IP_M0S8USBPD)
    /* Disable and clear UVOV interrupts. */
    pd->intr3_mask &= ~PDSS_INTR3_POS_OV_CHANGED;
    pd->intr3 = PDSS_INTR3_POS_OV_CHANGED;

    /* Invoke OVP callback. */
    if (context->vbusOvpCbk != NULL)
    {
        context->vbusOvpCbk(context, true);
    }
#endif /* CY_IP */
#else
    CY_UNUSED_PARAMETER(context);
#endif /* VBUS_OVP_ENABLE */
}


#if VBUS_UVP_ENABLE
/* UVP min reference voltage in mV. */
#define UVP_REF_VOLT_MIN            (200)

/* UVP min reference voltage on Port #1. */
#define UVP_REF_VOLT_MIN_P1         (130)

/* UVP Reference voltage step size in mV. */
#define UVP_REF_VOLT_STEP           (10)

/*
 * Minimum supported voltage for UVP. Any voltage lower may cause system to
 * not work as expected; the block references can get affected. This is now
 * limited to 3.15V.
 */
#define UVP_MIN_VOLT                (3100)

#define MAX_UVP_DEBOUNCE_CYCLES     (0x20u)
#endif /* VBUS_UVP_ENABLE */

/*******************************************************************************
* Function Name: Cy_USBPD_Fault_Vbus_UvpEnable
****************************************************************************//**
*
* Enable Under Voltage Protection (UVP) control using the internal UV-OV block.
* UVP is only expected to be used while PD-port is the power source.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param volt
* Contract Voltage in mV units.
*
* \param cb
* Callback function to be called on fault detection.
*
* \param pctrl
* Flag indicating the type of gate driver to be controlled, true for
* P_CTRL and false for C_CTRL.
*
*******************************************************************************/
void Cy_USBPD_Fault_Vbus_UvpEnable(cy_stc_usbpd_context_t *context, uint16_t volt, cy_cb_vbus_fault_t cb, bool pctrl)
{
#if VBUS_UVP_ENABLE && defined(CY_IP_MXUSBPD)
    uint16_t threshold, vref;
    uint32_t regVal = 0;
    PPDSS_REGS_T pd = context->base;
    uint8_t filterSel;

    filterSel = (context->usbpdConfig->vbusUvpConfig->debounce + 1) / 2;
    filterSel = CY_USBPD_GET_MIN (filterSel, MAX_UVP_DEBOUNCE_CYCLES);

    Cy_USBPD_Fault_FetAutoModeDisable (context, pctrl, CY_USBPD_VBUS_FILTER_ID_UV);

    /* Set up UVP callback. */
    context->vbusUvpCbk = cb;

    /* Calculate required VBUS for UVP. */
    threshold = ((volt * context->usbpdConfig->vbusUvpConfig->threshold) / 100);

    /* Ensure that we are within the limits. */
    if (threshold < UVP_MIN_VOLT)
    {
        threshold = UVP_MIN_VOLT;
    }

    regVal = pd->amux_nhv_ctrl;

#if defined(CY_DEVICE_CCG3PA)

#if !PMG1_FLIPPED_FET_CTRL
    /* Choose VBUS_C as source for AMUX. */
    regVal &= ~(AMUX_UV_VBUS_SEL_MASK);
#else
    /* Choose VBUS_IN as source for AMUX. */
    regVal |= AMUX_UV_VBUS_SEL_MASK;
#endif /* PMG1_FLIPPED_FET_CTRL */

    /*
     * Internal divider to be used depends on VBUS to be measured. For VBUS <
     * VBUS_DIV_20_PER_MAX_VOLT, use 20% divider and for greater, use 10% divider.
     */
    if (threshold <= VBUS_DIV_20_PER_MAX_VOLT)
    {
        vref = threshold / VBUS_C_20_PER_DIV;
        /* Connect output to 20% */
        regVal |= (1U << AMUX_UV_DIV_SEL_BIT_POS);
    }
    else
    {
        vref = threshold / VBUS_C_10_PER_DIV;
        /* Connect output to 10% */
        regVal &= ~(1U << AMUX_UV_DIV_SEL_BIT_POS);
    }
#endif /* defined(CY_DEVICE_CCG3PA) */

#if (defined(CY_DEVICE_CCG6))
    /* Use 10% of VBus as input to UVP comparator. */
    regVal &= 0xFFFFFFFD;

    vref = threshold / VBUS_C_10_PER_DIV;
#elif defined(CY_DEVICE_PMG1S3)
    /* Use 8% of VBus as input to UVP comparator. */
    regVal &= ~(1U << AMUX_UV_DIV_SEL_BIT_POS);

    vref = threshold / VBUS_C_8_PER_DIV;
#endif /* (defined(CY_DEVICE_CCG6)) */

    pd->amux_nhv_ctrl = regVal;

    /* Program reference voltage for UV comparator. */
    vref = ((vref - UVP_REF_VOLT_MIN) / UVP_REF_VOLT_STEP);
    regVal = pd->refgen_1_ctrl;
    regVal &= ~(PDSS_REFGEN_1_CTRL_SEL2_MASK);
    regVal |= (vref << PDSS_REFGEN_1_CTRL_SEL2_POS);
    pd->refgen_1_ctrl = regVal;

    /* Turn on comparator. */
    pd->comp_ctrl[CY_USBPD_VBUS_COMP_ID_UV] |= PDSS_COMP_CTRL_COMP_ISO_N;
    pd->comp_ctrl[CY_USBPD_VBUS_COMP_ID_UV] &= ~PDSS_COMP_CTRL_COMP_PD;

    Cy_SysLib_DelayUs(10);

    /* Filter configuration. */
    pd->intr5_filter_cfg[CY_USBPD_VBUS_FILTER_ID_UV] &= ~PDSS_INTR5_FILTER_CFG_FILT_EN;
    /* Reset filter to 1. */
    regVal = pd->intr5_filter_cfg[CY_USBPD_VBUS_FILTER_ID_UV] & ~(PDSS_INTR5_FILTER_CFG_FILT_CFG_MASK
            | PDSS_INTR5_FILTER_CFG_FILT_SEL_MASK | PDSS_INTR5_FILTER_CFG_DPSLP_MODE |
            PDSS_INTR5_FILTER_CFG_FILT_BYPASS);
    regVal |= PDSS_INTR5_FILTER_CFG_FILT_RESET;

    /* Set filter clock cycles if filter is required. */
    if (filterSel)
    {
        /* Subtracting 1 from filter clock cycle value as 0 translates to 1-2 clock cycles. */
        regVal |= (((filterSel - 1) & 0x1F) << PDSS_INTR5_FILTER_CFG_FILT_SEL_POS);
        /* Set edge detection. */
        regVal |= (CY_USBPD_VBUS_FILTER_CFG_POS_DIS_NEG_EN << PDSS_INTR5_FILTER_CFG_FILT_CFG_POS);
        regVal |= PDSS_INTR5_FILTER_CFG_FILT_EN | PDSS_INTR5_FILTER_CFG_DPSLP_MODE;
    }
    else
    {
        regVal |= PDSS_INTR5_FILTER_CFG_FILT_BYPASS;
    }

    pd->intr5_filter_cfg[CY_USBPD_VBUS_FILTER_ID_UV] = regVal;

    /* Clear interrupt. */
    pd->intr5 = (1U << CY_USBPD_VBUS_FILTER_ID_UV);

    /* Enable Interrupt. */
    pd->intr5_mask |= (1U << CY_USBPD_VBUS_FILTER_ID_UV);

    /* Handle Auto mode. */
    Cy_USBPD_Fault_FetAutoModeEnable (context, pctrl, CY_USBPD_VBUS_FILTER_ID_UV);
#else
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(pctrl);
    CY_UNUSED_PARAMETER(volt);
    (void)(cy_cb_vbus_fault_t)cb;
#endif /* VBUS_UVP_ENABLE && CY_IP */
}


/*******************************************************************************
* Function Name: Cy_USBPD_Fault_Vbus_UvpDisable
****************************************************************************//**
*
* Disable Under Voltage Protection (UVP) control.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param pctrl
* Flag indicating the type of gate driver to be controlled, true for
* P_CTRL and false for C_CTRL.
*
*******************************************************************************/
void Cy_USBPD_Fault_Vbus_UvpDisable(cy_stc_usbpd_context_t *context, bool pctrl)
{
#if VBUS_UVP_ENABLE && defined(CY_IP_MXUSBPD)
    PPDSS_REGS_T pd = context->base;
    uint32_t state = 0;

    state = Cy_SysLib_EnterCriticalSection ();

    /* Clear AUTO MODE OVP detect. */
    Cy_USBPD_Fault_FetAutoModeDisable (context, pctrl, CY_USBPD_VBUS_FILTER_ID_UV);

    /* Disable comparator. */
    pd->comp_ctrl[CY_USBPD_VBUS_COMP_ID_UV] &= ~PDSS_COMP_CTRL_COMP_ISO_N;
    pd->comp_ctrl[CY_USBPD_VBUS_COMP_ID_UV] |= PDSS_COMP_CTRL_COMP_PD;

    /* Disable filter. */
    pd->intr5_filter_cfg[CY_USBPD_VBUS_FILTER_ID_UV] &= ~(PDSS_INTR5_FILTER_CFG_FILT_EN |
        PDSS_INTR5_FILTER_CFG_FILT_CFG_MASK | PDSS_INTR5_FILTER_CFG_FILT_SEL_MASK |
        PDSS_INTR5_FILTER_CFG_FILT_BYPASS);

    /* Disable and clear UV interrupts. */
    pd->intr5_mask &= ~(1U << CY_USBPD_VBUS_FILTER_ID_UV);
    pd->intr5 = 1U << CY_USBPD_VBUS_FILTER_ID_UV;

    /* Clear callback. */
    context->vbusUvpCbk = NULL;

    Cy_SysLib_ExitCriticalSection (state);
#else
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(pctrl);
#endif /* VBUS_UVP_ENABLE && CY_IP */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Fault_Vbus_UvpIntrHandler
****************************************************************************//**
*
* VBUS UVP fault interrupt handler function.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
*******************************************************************************/
void Cy_USBPD_Fault_Vbus_UvpIntrHandler(cy_stc_usbpd_context_t *context)
{
#if VBUS_UVP_ENABLE && defined(CY_IP_MXUSBPD)
    PPDSS_REGS_T pd = context->base;

    /* Disable and clear UV interrupt. */
    pd->intr5_mask &= ~(1U << CY_USBPD_VBUS_FILTER_ID_UV);
    pd->intr5 = 1U << CY_USBPD_VBUS_FILTER_ID_UV;

    /* Invoke UVP callback. */
    if (context->vbusUvpCbk != NULL)
    {
        context->vbusUvpCbk(context, false);
    }
#else
    CY_UNUSED_PARAMETER(context);
#endif /* VBUS_UVP_ENABLE && CY_IP */
}

#if defined(VBUS_OCP_ENABLE)
/* Maximum limit on the VBUS current (in 10mA units) as required by compliance. */
#define VBUS_MAX_CURRENT                    (550u)

#define VBUS_OCP_MODE_EXT                   (0u)  /**< OCP through external hardware. */
#define VBUS_OCP_MODE_INT                   (1u)  /**< Internal OCP without software debounce or hardware gate control. */
#define VBUS_OCP_MODE_INT_AUTOCTRL          (2u)  /**< Internal OCP with hardware gate control. */
#define VBUS_OCP_MODE_INT_SW_DB             (3u)  /**< Internal OCP with software debounce. */
#define VBUS_OCP_MODE_POLLING               (4u)  /**< Solution level polling based OCP. */

#if defined(CY_DEVICE_CCG6)
#define SFLASH_OCP_TRIM_AV50_1A_HOT         (0x0FFFF402u)
#define SFLASH_OCP_TRIM_AV20_1A_HOT         (0x0FFFF409u)
#define SFLASH_OCP_TRIM_AV50_1A_COLD        (0x0FFFF412u)
#define SFLASH_OCP_TRIM_AV20_1A_COLD        (0x0FFFF417u)

#define SFLASH_OCP_TRIM_TBL_1A_OFFSET       (0u)
#define SFLASH_OCP_TRIM_TBL_3A_OFFSET       (2u)
#endif /* defined(CY_DEVICE_CCG6) */

#define SFLASH_OCP_TRIM_ROOM_FLAG           (0x0FFFF48Au)
#define SFLASH_OCP_TRIM_5V_2A_ROOM          (0x0FFFF45Eu)
#define SFLASH_OCP_TRIM_5V_2A_HOT           (0x0FFFF432u)
#define SFLASH_OCP_TRIM_5V_2A_COLD          (0x0FFFF448u)
#define SFLASH_OCP_TRIM_TBL_2A_OFFSET       (0u)
#define SFLASH_OCP_TRIM_TBL_4A_OFFSET       (2u)
#endif /* defined(VBUS_OCP_ENABLE) */

/*******************************************************************************
* Function Name: Cy_USBPD_Fault_Vbus_OcpEnable
****************************************************************************//**
*
* Enable Over Current Protection (OCP) control using the internal Current Sense Amplifier.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param current
* Contract current in mA units.
*
* \param cb
* Callback function to be called on fault detection.
*
*******************************************************************************/
void Cy_USBPD_Fault_Vbus_OcpEnable(cy_stc_usbpd_context_t *context, uint32_t current, cy_cb_vbus_fault_t cb)
{
#if (defined(VBUS_OCP_ENABLE) && (defined(CY_DEVICE_PMG1S3) || defined(CY_DEVICE_CCG6)))
    PPDSS_REGS_T pd = context->base;
    uint32_t vsense = 0;
    uint32_t vrefSel = 0;
    uint32_t state = 0;
    uint32_t regval = 0;
#if defined(CY_DEVICE_CCG6)
    uint8_t gainSel = 0;
#endif /* defined(CY_DEVICE_CCG6) */
    uint8_t filterSel = 0;
#if defined(CY_DEVICE_PMG1S3)
    PPDSS_TRIMS_REGS_T trimRegs = context->trimsBase;
#endif /* defined(CY_DEVICE_PMG1S3) */

#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))
    /* Never set current less than 100mA */
    if (current < 10u)
    {
        current = 10u;
    }
#endif /* CY_DEVICE */ 

    /* Set up OCP callback. */
    context->vbusOcpCbk = cb;

    /* Calculate required current for OCP. */
    vsense = current + ((current * context->usbpdConfig->vbusOcpConfig->threshold) / 100u);

    if(vsense > VBUS_MAX_CURRENT)
    {
        vsense = VBUS_MAX_CURRENT;
    }

#if defined(CY_DEVICE_CCG6)
    /*
     * The voltage reference to be applied on the CSA output stage is determined
     * based on data collected during device test and programmed into the SFLASH.
     *
     * Reference data is provided for current settings of 1.3 A, 2.6 A, 3.9 A, 5.2 A and 6.5 A.
     * The following code uses the 1.3 A and 3.9 A settings to plot a line which maps current values
     * to the corresponding reference voltages. We then pick the value of reference voltage corresponding
     * to the current limit from the line.
     */

    const uint8_t *sflash_ocp_trim_hot;
    const uint8_t *sflash_ocp_trim_cold;
    uint32_t i = 0;
    int c = 0;

    /* Set the gain based on the selected Rsense value. */
    if (context->usbpdConfig->vbusOcpConfig->senseRes == 10u)
    {
        sflash_ocp_trim_hot  = (const uint8_t *)(SFLASH_OCP_TRIM_AV20_1A_HOT + (((uint32_t)context->port) << 7));
        sflash_ocp_trim_cold = (const uint8_t *)(SFLASH_OCP_TRIM_AV20_1A_COLD + (((uint32_t)context->port) << 7));

        /* Interpolating the OCP current trim value using line equation: vref_sel = vsense*m + c */
        i = (
                (
                 (uint32_t)sflash_ocp_trim_hot[SFLASH_OCP_TRIM_TBL_3A_OFFSET] +
                 (uint32_t)sflash_ocp_trim_cold[SFLASH_OCP_TRIM_TBL_3A_OFFSET]
                ) -
                (
                 (uint32_t)sflash_ocp_trim_hot[SFLASH_OCP_TRIM_TBL_1A_OFFSET] +
                 (uint32_t)sflash_ocp_trim_cold[SFLASH_OCP_TRIM_TBL_1A_OFFSET]
                )
            ) >> 1u;

        c = ((int32_t)sflash_ocp_trim_hot[SFLASH_OCP_TRIM_TBL_1A_OFFSET] +
                (int32_t)sflash_ocp_trim_cold[SFLASH_OCP_TRIM_TBL_1A_OFFSET] - (int32_t)i) / 2;

        vsense  = (vsense * 2u) / 1u;
        vrefSel = ((i * vsense) / 520u);
        if (c < 0)
        {
            int32_t temp = -c;

            vrefSel -= (uint32_t)temp;
        }
        else
        {
            vrefSel += (uint32_t)c;
        }

        gainSel = 0x09u;        /* Constant Gain = 20 (Av1:010, bw: 01) */
    }
    else
    {
        sflash_ocp_trim_hot  = (const uint8_t *)(SFLASH_OCP_TRIM_AV50_1A_HOT + (((uint32_t)context->port) << 7));
        sflash_ocp_trim_cold = (const uint8_t *)(SFLASH_OCP_TRIM_AV50_1A_COLD + (((uint32_t)context->port) << 7));

        /* Interpolating the OCP current trim value using line equation: vref_sel = vsense*m + c */
        i = (
                (
                 (uint32_t)sflash_ocp_trim_hot[SFLASH_OCP_TRIM_TBL_3A_OFFSET] +
                 (uint32_t)sflash_ocp_trim_cold[SFLASH_OCP_TRIM_TBL_3A_OFFSET]
                ) -
                (
                 (uint32_t)sflash_ocp_trim_hot[SFLASH_OCP_TRIM_TBL_1A_OFFSET] +
                 (uint32_t)sflash_ocp_trim_cold[SFLASH_OCP_TRIM_TBL_1A_OFFSET]
                )
            ) >> 1u;

        c = ((int32_t)sflash_ocp_trim_hot[SFLASH_OCP_TRIM_TBL_1A_OFFSET] +
                (int32_t)sflash_ocp_trim_cold[SFLASH_OCP_TRIM_TBL_1A_OFFSET] - (int32_t)i) / 2;
        vsense   = (vsense * 5u) >> 1u;
        vrefSel = ((i * vsense) / 650u);
        if (c < 0)
        {
            int32_t temp = -c;

            vrefSel -= (uint32_t)temp;
        }
        else
        {
            vrefSel += (uint32_t)c;
        }

        gainSel = 0x16u;        /* Constant gain = 50 (Av1:101, bw: 10) */
    }

    if(context->usbpdConfig->vbusOcpConfig->mode == VBUS_OCP_MODE_INT_AUTOCTRL)
    {
        filterSel = (context->usbpdConfig->vbusOvpConfig->debounce + 1u) / 2u;
        if(filterSel > 31u)
        {
            filterSel = 31u;
        }
    }
    state = Cy_SysLib_EnterCriticalSection();

    if(context->usbpdConfig->vbusOcpConfig->mode == VBUS_OCP_MODE_INT_AUTOCTRL)
    {
        Cy_USBPD_Fault_FetAutoModeDisable(context, true, CY_USBPD_VBUS_FILTER_ID_HSCSA_OCP);
    }

    Cy_USBPD_SetRefgenVoltage (context, (uint8_t)vrefSel, (uint8_t)CY_USBPD_VREF_CSA_OCP);

    regval = pd->csa_scp_0_ctrl;

    /* Bring the CSA out of power-down, enable outputs and clear the gain setting. */
    regval &= ~(PDSS_CSA_SCP_0_CTRL_PD_CSA | PDSS_CSA_SCP_0_CTRL_AV1_MASK | PDSS_CSA_SCP_0_CTRL_BW_MASK);
    regval |= PDSS_CSA_SCP_0_CTRL_SEL_OUT_D | PDSS_CSA_SCP_0_CTRL_CSA_ISO_N;

    /* Set the gain and bandwidth based on requirement. */
    regval |= ((((uint32_t)gainSel >> 2UL) & 0x07UL) << PDSS_CSA_SCP_0_CTRL_AV1_POS);
    regval |= ((gainSel & 0x03UL) << PDSS_CSA_SCP_0_CTRL_BW_POS);

    pd->csa_scp_0_ctrl = regval;

#elif defined(CY_DEVICE_PMG1S3)
    const uint8_t *sflash_ocp_trim_hot;
    const uint8_t *sflash_ocp_trim_cold;
#if defined(USE_OCP_ROOM_TRIMS)  
    const uint8_t *sflash_ocp_trim_room;
#endif /* defined(USE_OCP_ROOM_TRIMS) */ 
    uint32_t i = 0;
    int c = 0;
    int temp;

#if defined(USE_OCP_ROOM_TRIMS)  
    if (*((volatile uint8_t *)(SFLASH_OCP_TRIM_ROOM_FLAG + (uint16_t)(context->port * 0x100u))) == 1u)
    {
        sflash_ocp_trim_room  = (const uint8_t *)(SFLASH_OCP_TRIM_5V_2A_ROOM + (uint16_t)(context->port * 0x100u));
        
        i  = (uint32_t)sflash_ocp_trim_room[SFLASH_OCP_TRIM_TBL_4A_OFFSET] - 
             (uint32_t)sflash_ocp_trim_room[SFLASH_OCP_TRIM_TBL_2A_OFFSET];

        /* Take the difference of currents used. 4.8A - 2.4A */
        c = 240;
        
        vrefSel = sflash_ocp_trim_room[SFLASH_OCP_TRIM_TBL_2A_OFFSET];
        
        temp = (int32_t)vsense - 240;
        temp = (temp * (int32_t)i) / c;
        if (temp < 0)
        {
            int32_t value = -temp;
            vrefSel -= (uint32_t)value;
        }
        else
        {
            vrefSel += (uint32_t)temp;
        }
    }
    else
#endif /* USE_OCP_ROOM_TRIMS */ 
    {
        sflash_ocp_trim_hot  = (const uint8_t *)(SFLASH_OCP_TRIM_5V_2A_HOT + (uint16_t)(context->port * 0x100u));
        sflash_ocp_trim_cold = (const uint8_t *)(SFLASH_OCP_TRIM_5V_2A_COLD + (uint16_t)(context->port * 0x100u));
        
       /* Interpolating the OCP current trim value using line equation: vref_sel = vsense*m + c */
        i = (
                (
                 (uint32_t)sflash_ocp_trim_hot[SFLASH_OCP_TRIM_TBL_4A_OFFSET] +
                 (uint32_t)sflash_ocp_trim_cold[SFLASH_OCP_TRIM_TBL_4A_OFFSET]
                ) -
                (
                 (uint32_t)sflash_ocp_trim_hot[SFLASH_OCP_TRIM_TBL_2A_OFFSET] +
                 (uint32_t)sflash_ocp_trim_cold[SFLASH_OCP_TRIM_TBL_2A_OFFSET]
                )
            ) >> 1u;
            
        /* Take the difference of currents used. 4.8A - 2.4A */
        c = 240;
        
        vrefSel = (((uint32_t)sflash_ocp_trim_hot[SFLASH_OCP_TRIM_TBL_2A_OFFSET] + (uint32_t)sflash_ocp_trim_cold[SFLASH_OCP_TRIM_TBL_2A_OFFSET]) >> 1U);
        temp = (int32_t)vsense - 240;
        temp = (temp * (int32_t)i) / c;
        if (temp < 0)
        {
            int32_t value = -temp;
            vrefSel -= (uint32_t)value;
        }
        else
        {
            vrefSel += (uint32_t)temp;
        }
    }
    
    state = Cy_SysLib_EnterCriticalSection();
    
    /* Store the calculated trims in the OCP Trims Register */
    trimRegs->trim_scp_rcp_2 = (uint32_t)vrefSel;
    
    /* Power-Up the SCP-RCP hard-IP */
    pd->scp_rcp_0_ctrl &= ~PDSS_SCP_RCP_0_CTRL_SCP_RCP_PD;
    Cy_SysLib_DelayUs(10);
    /* Power-Up the OCP block */
    pd->scp_rcp_0_ctrl &= ~PDSS_SCP_RCP_0_CTRL_SCP_RCP_PD_OCP;
    Cy_SysLib_DelayUs(10);
    pd->scp_rcp_0_ctrl |= PDSS_SCP_RCP_0_CTRL_SCP_RCP_ISO_N;
    
#endif /* CY_DEVICE */

    /* Configuring the CSA output filter. */
    regval = pd->intr13_cfg_csa_scp_hs;
    regval &= ~(PDSS_INTR13_CFG_CSA_SCP_HS_CSA_OCP_FILT_EN | PDSS_INTR13_CFG_CSA_SCP_HS_CSA_OCP_FILT_CFG_MASK |
            PDSS_INTR13_CFG_CSA_SCP_HS_CSA_OCP_FILT_SEL_MASK | PDSS_INTR13_CFG_CSA_SCP_HS_CSA_OCP_FILT_RESET |
            PDSS_INTR13_CFG_CSA_SCP_HS_CSA_OCP_FILT_BYPASS);

    if (context->usbpdConfig->vbusOcpConfig->mode == VBUS_OCP_MODE_INT_AUTOCTRL)
    {
        /* Configure the filter based on debounce parameter from config table. */
        regval |= ((uint32_t)CY_USBPD_VBUS_FILTER_CFG_POS_EN_NEG_DIS << PDSS_INTR13_CFG_CSA_SCP_HS_CSA_OCP_FILT_CFG_POS);
        regval |= ((uint32_t)filterSel << PDSS_INTR13_CFG_CSA_SCP_HS_CSA_OCP_FILT_SEL_POS) |
            PDSS_INTR13_CFG_CSA_SCP_HS_CSA_OCP_FILT_EN |
            PDSS_INTR13_CFG_CSA_SCP_HS_CSA_OCP_DPSLP_MODE;
    }
    else
    {
        /* Bypassing filter is causing false OC triggers. Leaving filter enabled with a 5 us debounce period. */
        regval |= ((uint32_t)CY_USBPD_VBUS_FILTER_CFG_POS_EN_NEG_DIS << PDSS_INTR13_CFG_CSA_SCP_HS_CSA_OCP_FILT_CFG_POS);
        regval |= (10UL << PDSS_INTR13_CFG_CSA_SCP_HS_CSA_OCP_FILT_SEL_POS) | PDSS_INTR13_CFG_CSA_SCP_HS_CSA_OCP_FILT_EN |
            PDSS_INTR13_CFG_CSA_SCP_HS_CSA_OCP_DPSLP_MODE;
    }
    pd->intr13_cfg_csa_scp_hs = regval;

    /* Clear and enable the OC detect interrupt. */
    pd->intr13 = PDSS_INTR13_CSA_OCP_CHANGED;
    pd->intr13_mask |= PDSS_INTR13_MASK_CSA_OCP_CHANGED_MASK;

    if (context->usbpdConfig->vbusOcpConfig->mode == VBUS_OCP_MODE_INT_AUTOCTRL)
    {
        Cy_USBPD_Fault_FetAutoModeEnable(context, true, CY_USBPD_VBUS_FILTER_ID_HSCSA_OCP);
    }

    Cy_SysLib_ExitCriticalSection(state);
#endif /* defined(VBUS_OCP_ENABLE) */
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(current);
    (void)(cy_cb_vbus_fault_t)cb;
}

/*******************************************************************************
* Function Name: Cy_USBPD_Fault_Vbus_OcpDisable
****************************************************************************//**
*
* Disables the Over Current Protection (OCP)
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param pctrl
* Flag indicating the type of gate driver to be controlled, true for
* P_CTRL and false for C_CTRL.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_Fault_Vbus_OcpDisable(cy_stc_usbpd_context_t *context, bool pctrl)
{
#if (defined(VBUS_OCP_ENABLE))
#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))
    PPDSS_REGS_T pd = context->base;
    uint32_t regval;
    uint32_t state;

    state = Cy_SysLib_EnterCriticalSection();

    /* Deregister the callback. */
    context->vbusOcpCbk = NULL;

#if defined(CY_DEVICE_CCG6)
    /* Default settings and power down. */
    pd->csa_scp_0_ctrl = (pd->csa_scp_0_ctrl & ~PDSS_CSA_SCP_0_CTRL_CSA_ISO_N) | PDSS_CSA_SCP_0_CTRL_PD_CSA;
#elif defined(CY_DEVICE_PMG1S3)
    /* Default settings and power down. */
    pd->scp_rcp_0_ctrl |= PDSS_SCP_RCP_0_CTRL_SCP_RCP_PD_OCP;
#endif /* CY_DEVICE */

    /* Disable and clear interrupts. */
    pd->intr13_mask &= ~(PDSS_INTR13_CSA_OCP_CHANGED);
    pd->intr13 = PDSS_INTR13_CSA_OCP_CHANGED;

    /* Disable the filter. */
    regval = pd->intr13_cfg_csa_scp_hs;
    regval &= ~(PDSS_INTR13_CFG_CSA_SCP_HS_CSA_OCP_FILT_EN | PDSS_INTR13_CFG_CSA_SCP_HS_CSA_OCP_FILT_CFG_MASK |
            PDSS_INTR13_CFG_CSA_SCP_HS_CSA_OCP_FILT_SEL_MASK | PDSS_INTR13_CFG_CSA_SCP_HS_CSA_OCP_FILT_RESET |
            PDSS_INTR13_CFG_CSA_SCP_HS_CSA_OCP_FILT_BYPASS);
    pd->intr13_cfg_csa_scp_hs = regval;

    if (context->usbpdConfig->vbusOcpConfig->mode == VBUS_OCP_MODE_INT_AUTOCTRL)
    {
        /* Turn-off auto mode. */
        Cy_USBPD_Fault_FetAutoModeDisable(context, pctrl, CY_USBPD_VBUS_FILTER_ID_HSCSA_OCP);
    }
    else
    {
        /* APP: The caller needs to make sure to stop the debounce timer. */
        //timer_stop(port, PD_OCP_DEBOUNCE_TIMER);
    }

    Cy_SysLib_ExitCriticalSection(state);
#endif /* defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3) */
#endif /* defined(VBUS_OCP_ENABLE) */
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(pctrl);
}

/*******************************************************************************
* Function Name: Cy_USBPD_Fault_Vbus_OcpIntrHandler
****************************************************************************//**
*
* VBUS OCP fault interrupt handler function.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_Fault_Vbus_OcpIntrHandler(cy_stc_usbpd_context_t *context)
{
#if (defined(VBUS_OCP_ENABLE))
#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))
    PPDSS_REGS_T pd = context->base;
    /*
     * CCG6 uses INTR13.CSA_OCP_CHANGED interrupt to indicate OCP condition.
     */
    if ((pd->intr13_masked & PDSS_INTR13_CSA_OCP_CHANGED) != 0u)
    {
#if (defined(CY_DEVICE_CCG6))
        if (context->usbpdConfig->vbusOcpConfig->mode == VBUS_OCP_MODE_INT_AUTOCTRL)
        {
            /* Disable and clear the interrupt. */
            pd->intr13_mask &= ~PDSS_INTR13_MASK_CSA_OCP_CHANGED_MASK;
            pd->intr13 = PDSS_INTR13_CSA_OCP_CHANGED;

            /* Make sure the provider path is turned off. This should already have been done by hardware. */
            /* Invoke OCP callback. 
             * APP: The application shouldn't start a debounce timer for this mode. */
            if (context->vbusOcpCbk != NULL)
            {
                (void)context->vbusOcpCbk(context, true);
            }
        }
        else
#endif /* CY_DEVICE_CCG6 */
        {
            /* If positive edge interrupt. */
            if (((pd->intr13_cfg_csa_scp_hs & PDSS_INTR13_CFG_CSA_SCP_HS_CSA_OCP_FILT_CFG_MASK) >> PDSS_INTR13_CFG_CSA_SCP_HS_CSA_OCP_FILT_CFG_POS) == (uint32_t)CY_USBPD_VBUS_FILTER_CFG_POS_EN_NEG_DIS)
            {
                /* Clear and disable interrupt. */
                pd->intr13_mask &= ~PDSS_INTR13_MASK_CSA_OCP_CHANGED_MASK;
                pd->intr13 = PDSS_INTR13_CSA_OCP_CHANGED;

                /* Look for negative edge of comparator. */
                pd->intr13_cfg_csa_scp_hs = Cy_USBPD_MmioRegUpdateField(pd->intr13_cfg_csa_scp_hs, (uint32_t)CY_USBPD_VBUS_FILTER_CFG_POS_DIS_NEG_EN,
                        PDSS_INTR13_CFG_CSA_SCP_HS_CSA_OCP_FILT_CFG_MASK, (uint8_t)PDSS_INTR13_CFG_CSA_SCP_HS_CSA_OCP_FILT_CFG_POS);

                /* Enable interrupt. */
                pd->intr13_mask |= PDSS_INTR13_MASK_CSA_OCP_CHANGED_MASK;

                /* Invoke OCP callback. 
                 * APP: The application should start a debounce timer when the compout is set to true. */
                if (context->vbusOcpCbk != NULL)
                {
                    (void)context->vbusOcpCbk(context, true);
                }

                /* If the negative edge has already happened, raise an interrupt. */
                if ((pd->intr13_status & PDSS_INTR13_STATUS_CSA_OCP_FILT) == 0u)
                {
                    pd->intr13_set = PDSS_INTR13_SET_CSA_OCP_CHANGED;
                }
            }
            /* If negative edge interrupt. */
            else
            {
                bool retval = true;

                /* Clear the interrupt. */
                pd->intr13 = PDSS_INTR13_CSA_OCP_CHANGED;

                /* Invoke OCP callback. 
                 * APP: The application should start a debounce timer when the compout is set to true. */
                if (context->vbusOcpCbk != NULL)
                {
                    /* APP: The callback is expected to return true, if the
                     * debounce timer was running, otherwise false. */
                    retval = context->vbusOcpCbk(context, false);
                }
                else
                {
                    /* Disable the interrupt in this case. */
                    retval = false;
                }

                /* debouncing was still going on. */
                if (retval == true)
                {
                    /* Look for positive edge of comparator. */
                    pd->intr13_cfg_csa_scp_hs = Cy_USBPD_MmioRegUpdateField(pd->intr13_cfg_csa_scp_hs,
                            (uint32_t)CY_USBPD_VBUS_FILTER_CFG_POS_EN_NEG_DIS, PDSS_INTR13_CFG_CSA_SCP_HS_CSA_OCP_FILT_CFG_MASK, (uint8_t)PDSS_INTR13_CFG_CSA_SCP_HS_CSA_OCP_FILT_CFG_POS);

                    /* If the positive edge has already happened, raise an interrupt. */
                    if ((pd->intr13_status & PDSS_INTR13_STATUS_CSA_OCP_FILT) != 0u)
                    {
                        pd->intr13_set = PDSS_INTR13_SET_CSA_OCP_CHANGED;
                    }
                }
                else
                {
                    /* Disable the interrupt here as it is no longer of interest. */
                    pd->intr13_mask &= ~PDSS_INTR13_MASK_CSA_OCP_CHANGED_MASK;
                }
            }
        }
    }
#endif /* defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3) */
#endif /* defined(VBUS_OCP_ENABLE) */
    CY_UNUSED_PARAMETER(context);
}

/*******************************************************************************
* Function Name: Cy_USBPD_Fault_CcOvp_IntrHandler
****************************************************************************//**
*
* CC OVP fault interrupt handler function.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
*******************************************************************************/
void Cy_USBPD_Fault_CcOvp_IntrHandler(cy_stc_usbpd_context_t *context)
{
#if defined(CY_DEVICE_CCG6)
    PPDSS_REGS_T pd = context->base;

    /* No need to check current status as we only enable positive edge on the interrupt. */
    if (context->ccSbuOvpCbk != NULL)
    {
        /* Passing false to indicate CC1/2 fault. */
        (void)context->ccSbuOvpCbk (context, false);
    }

    /* Re-enable the interrupts. */
    pd->intr1_mask |= (PDSS_INTR1_MASK_CC1_OVP_CHANGED_MASK | PDSS_INTR1_MASK_CC2_OVP_CHANGED_MASK);
#else
    CY_UNUSED_PARAMETER(context);
#endif /* defined(CY_DEVICE_CCG6) */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Fault_SbuOvp_IntrHandler
****************************************************************************//**
*
* SBU OVP fault interrupt handler function.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
*******************************************************************************/
void Cy_USBPD_Fault_SbuOvp_IntrHandler(cy_stc_usbpd_context_t *context)
{
#if defined(CY_DEVICE_CCG6)
    PPDSS_REGS_T pd = context->base;

    /* Clear the interrupt. */
    pd->intr3 = PDSS_INTR3_SBU1_SBU2_OVP_CHANGED_MASK;

    /* We don't need to check status as only positive edge triggered interrupts are enabled. */
    if (context->ccSbuOvpCbk != NULL)
    {
        /* Parameter true indicates SBU fault. */
        (void)context->ccSbuOvpCbk (context, true);
    }
#else
    CY_UNUSED_PARAMETER(context);
#endif /* defined(CY_DEVICE_CCG6) */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Vbus_V5vChangeDetectHandler
****************************************************************************//**
*
* VCONN 5V supply change interrupt handler function.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
*******************************************************************************/
void Cy_USBPD_Vbus_V5vChangeDetectHandler(cy_stc_usbpd_context_t *context)
{
#if PMG1_V5V_CHANGE_DETECT
    PPDSS_REGS_T pd = context->base;
    bool temp = false;

    /* Clear and disable the interrupt. */
    pd->intr1 = PDSS_INTR1_V5V_CHANGED;
    pd->intr1_mask &= ~PDSS_INTR1_MASK_V5V_CHANGED_MASK;

    if (context->supplyChangeCbk != NULL)
    {
        /* Store the current status of the V5V supply */
#if defined(CY_IP_MXUSBPD)
        temp = ((pd->intr1_status & PDSS_INTR1_STATUS_V5V_STATUS) != 0);
#elif defined(CY_IP_M0S8USBPD)
        temp = ((pd->status & PDSS_STATUS_V5V_STATUS) != 0);
#endif

        /* Notify application layer about absence of VSYS supply. */
        context->supplyChangeCbk (context, CY_USBPD_SUPPLY_V5V, temp);
    }

    /* Re-enable the interrupt. */
    pd->intr1_mask |= PDSS_INTR1_MASK_V5V_CHANGED_MASK;
#else
    CY_UNUSED_PARAMETER(context);
#endif /* PMG1_V5V_CHANGE_DETECT */
}


/** \cond DOXYGEN_HIDE */
void Cy_USBPD_Vbus_LoadChangeISR_En(cy_stc_usbpd_context_t *context, uint32_t cur, 
                                   uint8_t filter, cy_cb_usbpd_vbus_load_chg_t cbk)
{
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(cur);
    CY_UNUSED_PARAMETER(filter);
    (void)(cy_cb_usbpd_vbus_load_chg_t)cbk;
}
/** \endcond */

#ifdef VBUS_RCP_ENABLE
#define RCP_GAIN_VALUE                  (100u)
#define SFLASH_RCP_TRIM_VBUS_OV         (0x0FFFF411u)
#define SFLASH_RCP_TRIM_CSA_HOT         (0x0FFFF410u)
#define SFLASH_RCP_TRIM_CSA_COLD        (0x0FFFF41Cu)

#define RCP_REF_VOLT_MIN                (130U)
#define RCP_REF_VOLT_STEP               (10U)
#define RCP_REF_MAX_STEP                (199U)

#define SFLASH_RCP_TRIM_ROOM_FLAG           (0x0FFFF48Bu)
#define SFLASH_RCP_TRIM_400MA_ROOM          (0x0FFFF464u)
#define SFLASH_RCP_TRIM_400MA_HOT           (0x0FFFF438u)
#define SFLASH_RCP_TRIM_400MA_COLD          (0x0FFFF44Eu) 
#define SFLASH_RCP_TRIM_VBUS_OV_5P4V_ROOM   (0x0FFFF481u)
#define SFLASH_RCP_TRIM_VBUS_OV_5P4V_HOT    (0x0FFFF473u)
#define SFLASH_RCP_TRIM_VBUS_OV_5P4V_COLD   (0x0FFFF47Au)
#endif /* VBUS_RCP_ENABLE */

/*******************************************************************************
* Function Name: Cy_USBPD_Fault_Vbus_RcpEnable
****************************************************************************//**
*
* Enable Reverse Current Protection (RCP).
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param volt
* Contract Voltage in mV units.
*
* \param cb
* Callback function to be called on fault detection.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_Fault_Vbus_RcpEnable(cy_stc_usbpd_context_t *context, uint16_t volt, cy_cb_vbus_fault_t cb)
{
#if (defined(VBUS_RCP_ENABLE) && (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)))
    PPDSS_REGS_T pd = context->base;
    uint32_t regval = 0;
    uint32_t vref = 0;
#if defined(CY_DEVICE_CCG6)    
    uint16_t trim_rcp_offset_value = 0;
#endif /* defined(CY_DEVICE_CCG6) */
    uint32_t state = 0;
#if defined(CY_DEVICE_PMG1S3)
    PPDSS_TRIMS_REGS_T trimRegs = context->trimsBase;
    uint32_t x1, x2, y1, y2, x = 0UL;
    int32_t c = 0;
#endif /* defined(CY_DEVICE_PMG1S3) */

    /* Register RCP Callback. */
    context->vbusRcpCbk = cb;
    state = Cy_SysLib_EnterCriticalSection();
    
#if defined(CY_DEVICE_CCG6)
    /* Enable pull-up of P_CTRL on VBus side when RCP detected. */
    pd->pgdo_pu_3_cfg |= (PDSS_PGDO_PU_3_CFG_PGDO_EN_RCP_LV | PDSS_PGDO_PU_3_CFG_PGDO_EN_VBUS_OV_LV);

    Cy_USBPD_Fault_FetAutoModeDisable (context, true, CY_USBPD_VBUS_FILTER_ID_HSCSA_RCP);

    /** CSA RCP detection configuration based on current measurement **/
    regval = pd->csa_rcp_0_ctrl;
    /* Power up the CSA block. */
    regval &= ~PDSS_CSA_RCP_0_CTRL_PD_CSA;
    /* Default operational settings. */
    regval |= PDSS_CSA_RCP_0_CTRL_SEL_OUT_D | PDSS_CSA_RCP_0_CTRL_CSA_ISO_N;
    /* Clear the gain and bw settings. */
    regval &= ~(PDSS_CSA_RCP_0_CTRL_AV1_MASK | PDSS_CSA_RCP_0_CTRL_BW_MASK);

    /* Write out CSA_RCP_0_CTRL to enable the block and outputs. */
    pd->csa_rcp_0_ctrl = regval;

    /* Set CSA gain of 100. */
    regval |=  0x07U;
    /* Set analog bandwidth. */
    regval |= (0x03UL << PDSS_CSA_RCP_0_CTRL_BW_POS);

    /* Write out CSA_RCP_0_CTRL. */
    pd->csa_rcp_0_ctrl = regval;

    /*
     * Configure Reference for RCP CSA comparator:
     * We use the average of the HOT and COLD RCP trims taken from SFLASH.
     */
    vref = ((uint32_t)(*((const uint8_t *) (SFLASH_RCP_TRIM_CSA_HOT))) + (uint32_t)(*((const uint8_t *) (SFLASH_RCP_TRIM_CSA_COLD))) ) / 2UL;
    Cy_USBPD_SetRefgenVoltage (context, (uint8_t)vref, (uint8_t)CY_USBPD_VREF_RCP_CSA);
    Cy_SysLib_DelayUs (50U);

    /* Enable the RCP CSA comparator */
    pd->csa_rcp_1_ctrl |= PDSS_CSA_RCP_1_CTRL_CSA_EN_LV;
    Cy_SysLib_DelayUs (100U);
    PDSS_TRIMS->trim_csa_rcp1_4 |= (1U << 7);

    /* Bypassing filter may cause false rc triggers. Leaving filter enabled with a 5 us debounce period. */
    regval = pd->intr13_cfg_csa_rcp_hs;
    regval &= ~(PDSS_INTR13_CFG_CSA_RCP_HS_CSA_OUT_FILT_EN | PDSS_INTR13_CFG_CSA_RCP_HS_CSA_OUT_FILT_CFG_MASK |
            PDSS_INTR13_CFG_CSA_RCP_HS_CSA_OUT_FILT_SEL_MASK | PDSS_INTR13_CFG_CSA_RCP_HS_CSA_OUT_FILT_RESET |
            PDSS_INTR13_CFG_CSA_RCP_HS_CSA_OUT_FILT_BYPASS);

    /* Set edge detection. */
    regval |= ((uint32_t)CY_USBPD_VBUS_FILTER_CFG_POS_EN_NEG_DIS << PDSS_INTR13_CFG_CSA_RCP_HS_CSA_OUT_FILT_CFG_POS);
    regval |= PDSS_INTR13_CFG_CSA_RCP_HS_CSA_OUT_FILT_EN | PDSS_INTR13_CFG_CSA_RCP_HS_CSA_OUT_DPSLP_MODE;
    pd->intr13_cfg_csa_rcp_hs = regval;

    /* Clear and enable the RCP CSA Detect interrupt. */
    pd->intr13 = PDSS_INTR13_CSA_OUT_CHANGED;
    pd->intr13_mask |= PDSS_INTR13_MASK_CSA_OUT_CHANGED_MASK;

    /** RCP detection configuration based on different RCP COMP **/
    /* Enable the RCP comparator */

    /* Set the RCP trim offset value based on the source voltage: Increment the
     * offset for a 3V rise in voltage. */
    if (volt > 5000U)
    {
        trim_rcp_offset_value = (uint16_t)PDSS_TRIMS->trim_csa_rcp1_2 + ((volt - 5000U) / 3000U);
        if (trim_rcp_offset_value > 7U)
        {
            trim_rcp_offset_value = 7U;
        }

        PDSS_TRIMS->trim_csa_rcp1_2 = (uint8_t)trim_rcp_offset_value;
    }

    PDSS_TRIMS->trim_csa_rcp1_4 &= ~(1U << 5);      /* Reset the latch */
    pd->csa_rcp_1_ctrl |= PDSS_CSA_RCP_1_CTRL_RCP_EN_LV;
    Cy_SysLib_DelayUs (20U);
    PDSS_TRIMS->trim_csa_rcp1_4 |= (1U << 5);        /* Enable the latch. */

    /* Bypassing filter may cause false rc triggers. Leaving filter enabled with a 5 us debounce period. */
    regval = pd->intr13_cfg_csa_rcp_hs;
    regval &= ~(PDSS_INTR13_CFG_CSA_RCP_HS_CSA_COMP_OUT_FILT_EN |
            PDSS_INTR13_CFG_CSA_RCP_HS_CSA_COMP_OUT_FILT_CFG_MASK |
            PDSS_INTR13_CFG_CSA_RCP_HS_CSA_COMP_OUT_FILT_SEL_MASK |
            PDSS_INTR13_CFG_CSA_RCP_HS_CSA_COMP_OUT_FILT_RESET |
            PDSS_INTR13_CFG_CSA_RCP_HS_CSA_COMP_OUT_FILT_BYPASS);

    /* Set edge detection. */
    regval |= ((uint32_t)CY_USBPD_VBUS_FILTER_CFG_POS_EN_NEG_DIS << PDSS_INTR13_CFG_CSA_RCP_HS_CSA_COMP_OUT_FILT_CFG_POS);
    regval |= PDSS_INTR13_CFG_CSA_RCP_HS_CSA_COMP_OUT_FILT_EN | PDSS_INTR13_CFG_CSA_RCP_HS_CSA_COMP_OUT_DPSLP_MODE;
    pd->intr13_cfg_csa_rcp_hs = regval;

    /* Clear and enable the RCP Comparator Detect interrupt. */
    pd->intr13 = PDSS_INTR13_CSA_COMP_OUT_CHANGED;
    pd->intr13_mask |= PDSS_INTR13_MASK_CSA_COMP_OUT_CHANGED_MASK;

    /**** RCP detection configuration based on VBUS OV detect ***/

    /* Reset the latch. */
    PDSS_TRIMS->trim_csa_rcp1_4 &= ~(1U << 6);

    if (volt <= 9000U)
    {
        /* If the source voltage is less than or equal to 9.0 V, we use the 20% Vbus divider for comparison. */
        pd->csa_rcp_1_ctrl &= ~(1UL << 12);

        /* If the source voltage is less than 5.5 V, we can use the trim data provided in SFLASH. */
        if (volt <= 5500U)
        {
            vref = *((const uint8_t *)(SFLASH_RCP_TRIM_VBUS_OV));
        }
        else
        {
            /* Vref calculated theoretically by adding 20% threshold to VBUS and then dividing it by 10 */
            vref = (uint32_t)((((volt * 11UL) / 50UL) - RCP_REF_VOLT_MIN) / RCP_REF_VOLT_STEP);
        }
    }
    else if (volt <= 15000U)
    {
        /* Use 10% VBUS divider for greater than 9V contract */
        pd->csa_rcp_1_ctrl |= (1UL << 12);

        /* Vref calculated theoretically by adding 20% threshold to VBUS and then dividing it by 10 */
        vref = (uint32_t)((((volt * 11UL) / 100UL) - RCP_REF_VOLT_MIN) / RCP_REF_VOLT_STEP);
    }
    else
    {
        /* Do nothing. */
    }

    if (volt <= 15000U)
    {
        if (vref > RCP_REF_MAX_STEP)
        {
            vref = RCP_REF_MAX_STEP;
        }
        /* Program the reference voltage. */
        Cy_USBPD_SetRefgenVoltage (context, (uint8_t)vref, (uint8_t)CY_USBPD_VREF_RCP_OVP);

        /* Enable the VBUS OV Detect comparator */
        pd->csa_rcp_1_ctrl |= PDSS_CSA_RCP_1_CTRL_RCP_VBUS_OVP_EN_LV;
        Cy_SysLib_DelayUs (20U);
        PDSS_TRIMS->trim_csa_rcp1_4 |= (1U << 6);

        /* Bypassing filter may cause false triggers. Leaving filter enabled with a 5 us debounce period. */
        regval = pd->intr13_cfg_csa_rcp_ovp_hs;
        regval &= ~(PDSS_INTR13_CFG_CSA_RCP_OVP_HS_CSA_VBUS_OVP_FILT_EN |
                PDSS_INTR13_CFG_CSA_RCP_OVP_HS_CSA_VBUS_OVP_FILT_CFG_MASK |
                PDSS_INTR13_CFG_CSA_RCP_OVP_HS_CSA_VBUS_OVP_FILT_SEL_MASK |
                PDSS_INTR13_CFG_CSA_RCP_OVP_HS_CSA_VBUS_OVP_FILT_RESET |
                PDSS_INTR13_CFG_CSA_RCP_OVP_HS_CSA_VBUS_OVP_FILT_BYPASS);

        /* Set edge detection. */
        regval |= ((uint32_t)CY_USBPD_VBUS_FILTER_CFG_POS_EN_NEG_DIS << PDSS_INTR13_CFG_CSA_RCP_OVP_HS_CSA_VBUS_OVP_FILT_CFG_POS);
        regval |= PDSS_INTR13_CFG_CSA_RCP_OVP_HS_CSA_VBUS_OVP_FILT_EN |
            PDSS_INTR13_CFG_CSA_RCP_OVP_HS_CSA_VBUS_OVP_DPSLP_MODE;
        pd->intr13_cfg_csa_rcp_ovp_hs = regval;

        /* Clear and enable the RCP VBUS OVP Detect interrupt. */
        pd->intr13 = PDSS_INTR13_CSA_VBUS_OVP_CHANGED;
        pd->intr13_mask |= PDSS_INTR13_MASK_CSA_VBUS_OVP_CHANGED_MASK;
    }
#elif defined (CY_DEVICE_PMG1S3)
    /* Disable FET automode */
    Cy_USBPD_Fault_FetAutoModeDisable (context, true, CY_USBPD_VBUS_FILTER_ID_HSCSA_RCP);

#if defined(USE_RCP_ROOM_TRIMS)    
    if (*((volatile uint8_t *)(SFLASH_RCP_TRIM_ROOM_FLAG + (uint16_t)(context->port * 0x100u))) == 1u)
    {
        vref = *((volatile uint8_t *)(SFLASH_RCP_TRIM_400MA_ROOM + 1UL + (uint16_t)(context->port * 0x100u)));
    }
    else
#endif /* defined(USE_RCP_ROOM_TRIMS) */
    {
        vref = (uint8_t)((*((volatile uint8_t *)(SFLASH_RCP_TRIM_400MA_HOT + 1UL + (uint16_t)(context->port * 0x100u))) + 
                        (*((volatile uint8_t *)(SFLASH_RCP_TRIM_400MA_COLD + 1UL  + (uint16_t)(context->port * 0x100u))))) / 2U);
    }
    
    /* Copy the trim value from SFLASH into the SCP Trims Register */
    trimRegs->trim_scp_rcp_1 = (uint32_t)vref;
#if defined(USE_RCP_ROOM_TRIMS)
    if (*((volatile uint8_t *)(SFLASH_RCP_TRIM_ROOM_FLAG + (uint16_t)(context->port * 0x100u))) == 1u)
    {
        if(volt < 6000U)
        {
            /* Set the VBUS divider to 33% */
            pd->scp_rcp_1_ctrl &= ~(1UL << 23);
            pd->scp_rcp_1_ctrl |= (1UL << 24);
            vref = *((volatile uint8_t *)(SFLASH_RCP_TRIM_VBUS_OV_5P4V_ROOM + (uint16_t)(context->port * 0x100u)));
        }
        else if(volt < 20000U)
        {
            /* Set the VBUS divider to 10% */
            pd->scp_rcp_1_ctrl &= ~(1UL << 24);
            pd->scp_rcp_1_ctrl |= (1UL << 23);

            y1 = *((volatile uint8_t *)(SFLASH_RCP_TRIM_VBUS_OV_5P4V_ROOM + 1UL + (uint16_t)(context->port * 0x100u)));
            y2 = *((volatile uint8_t *)(SFLASH_RCP_TRIM_VBUS_OV_5P4V_ROOM + 2UL + (uint16_t)(context->port * 0x100u)));
            x1 = 6000U;
            x2 = 20000U;
        }
        else
        {
            /* Set the VBUS divider to 6% */
            pd->scp_rcp_1_ctrl &= ~((1UL << 24) | (1UL << 23));
            
            y1 = *((volatile uint8_t *)(SFLASH_RCP_TRIM_VBUS_OV_5P4V_ROOM + 3UL + (uint16_t)(context->port * 0x100u)));
            y2 = *((volatile uint8_t *)(SFLASH_RCP_TRIM_VBUS_OV_5P4V_ROOM + 4UL + (uint16_t)(context->port * 0x100u)));
            x1 = 20100U;
            x2 = 24000U;
        }
    }
    else
#endif /* defined(USE_RCP_ROOM_TRIMS) */
    {
        if(volt < 6000U)
        {
            /* Set the VBUS divider to 33% */
            pd->scp_rcp_1_ctrl &= ~(1UL << 23);
            pd->scp_rcp_1_ctrl |= (1UL << 24);
            vref = (uint8_t)((*((volatile uint8_t *)(SFLASH_RCP_TRIM_VBUS_OV_5P4V_HOT + (uint16_t)(context->port * 0x100u))) + 
                             (*((volatile uint8_t *)(SFLASH_RCP_TRIM_VBUS_OV_5P4V_COLD + (uint16_t)(context->port * 0x100u))))) / 2U);
        }
        else if(volt < 20000U)
        {
            /* Set the VBUS divider to 10% */
            pd->scp_rcp_1_ctrl &= ~(1UL << 24);
            pd->scp_rcp_1_ctrl |= (1UL << 23);

            y1 = (uint8_t)((*((volatile uint8_t *)(SFLASH_RCP_TRIM_VBUS_OV_5P4V_HOT + 1UL + (uint16_t)(context->port * 0x100u))) + 
                           (*((volatile uint8_t *)(SFLASH_RCP_TRIM_VBUS_OV_5P4V_COLD + 1UL  + (uint16_t)(context->port * 0x100u))))) / 2U);
            y2 = (uint8_t)((*((volatile uint8_t *)(SFLASH_RCP_TRIM_VBUS_OV_5P4V_HOT + 2UL + (uint16_t)(context->port * 0x100u))) + 
                           (*((volatile uint8_t *)(SFLASH_RCP_TRIM_VBUS_OV_5P4V_COLD + 2UL  + (uint16_t)(context->port * 0x100u))))) / 2U);
            x1 = 6000U;
            x2 = 20000U;
        }
        else
        {
            /* Set the VBUS divider to 6% */
            pd->scp_rcp_1_ctrl &= ~((1UL << 24) | (1UL << 23));

            y1 = (uint8_t)((*((volatile uint8_t *)(SFLASH_RCP_TRIM_VBUS_OV_5P4V_HOT + 3UL + (uint16_t)(context->port * 0x100u))) + 
                           (*((volatile uint8_t *)(SFLASH_RCP_TRIM_VBUS_OV_5P4V_COLD + 3UL  + (uint16_t)(context->port * 0x100u))))) / 2U);
            y2 = (uint8_t)((*((volatile uint8_t *)(SFLASH_RCP_TRIM_VBUS_OV_5P4V_HOT + 4UL + (uint16_t)(context->port * 0x100u))) + 
                           (*((volatile uint8_t *)(SFLASH_RCP_TRIM_VBUS_OV_5P4V_COLD + 4UL  + (uint16_t)(context->port * 0x100u))))) / 2U);
            x1 = 20100U;
            x2 = 24000U;
        }
    }
    
    if(volt >= 6000U)
    {
        int32_t value = 0;
        /* VBUS calculated theoretically by adding 10% threshold to VBUS */
        x = (uint32_t)((volt * 11UL) / 10UL);
        
        value = ((int32_t)y2 - (int32_t)y1) * (int32_t)x1;

        c = ((int32_t)y1 - (value / ((int32_t)x2 - (int32_t)x1)));
        
        vref = (uint32_t)(((y2 - y1)*x)/(x2 - x1));
        if (c < 0)
        {
            int temp = -c;
            vref -= (uint32_t)temp;
        }
        else
        {
            vref += (uint32_t)c;
        }
    }
    
    /* Program the reference voltage. */
    Cy_USBPD_SetRefgenVoltage (context, (uint8_t)vref, (uint8_t)CY_USBPD_VREF_RCP_OVP);
    
    /* Power-Up the SCP-RCP hard-IP */
    pd->scp_rcp_0_ctrl &= ~PDSS_SCP_RCP_0_CTRL_SCP_RCP_PD;
    Cy_SysLib_DelayUs(10);
    /* Power-Up the RCP block */
    pd->scp_rcp_0_ctrl &= ~PDSS_SCP_RCP_0_CTRL_SCP_RCP_PD_RCP; 
    Cy_SysLib_DelayUs(10);
    pd->scp_rcp_0_ctrl |= PDSS_SCP_RCP_0_CTRL_SCP_RCP_ISO_N | PDSS_SCP_RCP_0_CTRL_SCP_RCP_VBUS_OVP_EN;
  
    /* Remove the RCP latch from Reset mode and put it in sample mode*/
    pd->scp_rcp_0_ctrl &= ~PDSS_SCP_RCP_0_CTRL_SCP_RCP_T_RCP_RST_LATCH_N;
    Cy_SysLib_DelayUs(10);
    pd->scp_rcp_0_ctrl |= PDSS_SCP_RCP_0_CTRL_SCP_RCP_T_RCP_RST_LATCH_N;
    
    /* Bypassing filter may cause false rc triggers. Leaving filter enabled with a 5 us debounce period. */
    regval = pd->intr13_cfg_csa_rcp_hs;
    regval &= ~(PDSS_INTR13_CFG_CSA_RCP_HS_CSA_OUT_FILT_EN | PDSS_INTR13_CFG_CSA_RCP_HS_CSA_OUT_FILT_CFG_MASK |
            PDSS_INTR13_CFG_CSA_RCP_HS_CSA_OUT_FILT_SEL_MASK | PDSS_INTR13_CFG_CSA_RCP_HS_CSA_OUT_FILT_RESET |
            PDSS_INTR13_CFG_CSA_RCP_HS_CSA_OUT_FILT_BYPASS | 
            PDSS_INTR13_CFG_CSA_RCP_HS_CSA_COMP_OUT_FILT_EN |
            PDSS_INTR13_CFG_CSA_RCP_HS_CSA_COMP_OUT_FILT_CFG_MASK |
            PDSS_INTR13_CFG_CSA_RCP_HS_CSA_COMP_OUT_FILT_SEL_MASK |
            PDSS_INTR13_CFG_CSA_RCP_HS_CSA_COMP_OUT_FILT_RESET |
            PDSS_INTR13_CFG_CSA_RCP_HS_CSA_COMP_OUT_FILT_BYPASS);          
    /* Set edge detection. */
    regval |= ((uint32_t)CY_USBPD_VBUS_FILTER_CFG_POS_EN_NEG_DIS << PDSS_INTR13_CFG_CSA_RCP_HS_CSA_OUT_FILT_CFG_POS);
    regval |= PDSS_INTR13_CFG_CSA_RCP_HS_CSA_OUT_FILT_EN | PDSS_INTR13_CFG_CSA_RCP_HS_CSA_OUT_DPSLP_MODE;
    regval |= ((uint32_t)CY_USBPD_VBUS_FILTER_CFG_POS_EN_NEG_DIS << PDSS_INTR13_CFG_CSA_RCP_HS_CSA_COMP_OUT_FILT_CFG_POS);
    regval |= PDSS_INTR13_CFG_CSA_RCP_HS_CSA_COMP_OUT_FILT_EN | PDSS_INTR13_CFG_CSA_RCP_HS_CSA_COMP_OUT_DPSLP_MODE;   
    pd->intr13_cfg_csa_rcp_hs = regval;
    
    /* Bypassing filter may cause false triggers. Leaving filter enabled with a 5 us debounce period. */
    regval = pd->intr13_cfg_csa_rcp_ovp_hs;
    regval &= ~(PDSS_INTR13_CFG_CSA_RCP_OVP_HS_CSA_VBUS_OVP_FILT_EN |
            PDSS_INTR13_CFG_CSA_RCP_OVP_HS_CSA_VBUS_OVP_FILT_CFG_MASK |
            PDSS_INTR13_CFG_CSA_RCP_OVP_HS_CSA_VBUS_OVP_FILT_SEL_MASK |
            PDSS_INTR13_CFG_CSA_RCP_OVP_HS_CSA_VBUS_OVP_FILT_RESET |
            PDSS_INTR13_CFG_CSA_RCP_OVP_HS_CSA_VBUS_OVP_FILT_BYPASS);
    /* Set edge detection. */
    regval |= ((uint32_t)CY_USBPD_VBUS_FILTER_CFG_POS_EN_NEG_DIS << PDSS_INTR13_CFG_CSA_RCP_OVP_HS_CSA_VBUS_OVP_FILT_CFG_POS);
    regval |= PDSS_INTR13_CFG_CSA_RCP_OVP_HS_CSA_VBUS_OVP_FILT_EN |
        PDSS_INTR13_CFG_CSA_RCP_OVP_HS_CSA_VBUS_OVP_DPSLP_MODE;
    pd->intr13_cfg_csa_rcp_ovp_hs = regval;    
    
    /* Clear and enable the RCP Detect interrupt. */
    pd->intr13 = (PDSS_INTR13_CSA_OUT_CHANGED | PDSS_INTR13_CSA_COMP_OUT_CHANGED | PDSS_INTR13_CSA_VBUS_OVP_CHANGED);
    pd->intr13_mask |= (PDSS_INTR13_MASK_CSA_OUT_CHANGED_MASK | PDSS_INTR13_MASK_CSA_COMP_OUT_CHANGED_MASK | PDSS_INTR13_MASK_CSA_VBUS_OVP_CHANGED_MASK);
#endif /* CY_DEVICE */

    /* Auto FET turn off mode. */
    Cy_USBPD_Fault_FetAutoModeEnable (context, true, CY_USBPD_VBUS_FILTER_ID_HSCSA_RCP);
    Cy_SysLib_ExitCriticalSection (state);
    CY_UNUSED_PARAMETER(volt);
#else
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(volt);
    (void)(cy_cb_vbus_fault_t)cb;
#endif /* (defined(VBUS_RCP_ENABLE) && defined(CY_DEVICE_CCG6)) */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Fault_Vbus_RcpDisable
****************************************************************************//**
*
* Disable Reverse Current Protection (RCP) control.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_Fault_Vbus_RcpDisable(cy_stc_usbpd_context_t *context)
{
#if (defined(VBUS_RCP_ENABLE) && (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)))
    PPDSS_REGS_T pd = context->base;
    uint32_t state;
#if defined(CY_DEVICE_CCG6)    
    uint32_t regval = 0;
#endif /* defined(CY_DEVICE_CCG6) */
    state = Cy_SysLib_EnterCriticalSection ();

    /* Deregister the callback. */
    context->vbusRcpCbk = NULL;

#if defined(CY_DEVICE_CCG6)
    /* Power down the CSA RCP block */
    pd->csa_rcp_0_ctrl |= PDSS_CSA_RCP_0_CTRL_PD_CSA;
    pd->csa_rcp_0_ctrl &= ~(PDSS_CSA_RCP_0_CTRL_SEL_OUT_D | PDSS_CSA_RCP_0_CTRL_CSA_ISO_N);

    pd->intr13_mask &= ~(PDSS_INTR13_MASK_CSA_OUT_CHANGED_MASK | PDSS_INTR13_MASK_CSA_COMP_OUT_CHANGED_MASK |
            PDSS_INTR13_MASK_CSA_VBUS_OVP_CHANGED_MASK);
            
    regval |= (PDSS_CSA_RCP_1_CTRL_CSA_EN_LV | PDSS_CSA_RCP_1_CTRL_RCP_EN_LV | PDSS_CSA_RCP_1_CTRL_RCP_VBUS_OVP_EN_LV);
    pd->csa_rcp_1_ctrl &= ~regval;
    PDSS_TRIMS->trim_csa_rcp1_4 &= ~((1U << 5) | (1U << 6) | (1U << 7));
    
#elif defined(CY_DEVICE_PMG1S3)
    pd->intr13_mask &= ~(PDSS_INTR13_MASK_CSA_OUT_CHANGED_MASK | PDSS_INTR13_MASK_CSA_COMP_OUT_CHANGED_MASK |
            PDSS_INTR13_MASK_CSA_VBUS_OVP_CHANGED_MASK);
    pd->scp_rcp_0_ctrl |= PDSS_SCP_RCP_0_CTRL_SCP_RCP_PD_RCP; 
    pd->scp_rcp_0_ctrl &= ~(PDSS_SCP_RCP_0_CTRL_SCP_RCP_ISO_N | PDSS_SCP_RCP_0_CTRL_SCP_RCP_VBUS_OVP_EN | PDSS_SCP_RCP_0_CTRL_SCP_RCP_T_RCP_RST_LATCH_N);
#endif /* CY_DEVICE */

    /* Clear the interrupts. */
    pd->intr13 = (PDSS_INTR13_CSA_OUT_CHANGED | PDSS_INTR13_CSA_COMP_OUT_CHANGED | PDSS_INTR13_CSA_VBUS_OVP_CHANGED);

    /* Remove RCP as source. */
    Cy_USBPD_Fault_FetAutoModeDisable (context, true, CY_USBPD_VBUS_FILTER_ID_HSCSA_RCP);
    Cy_SysLib_ExitCriticalSection (state);
#else
    CY_UNUSED_PARAMETER(context);
#endif /* (defined(VBUS_RCP_ENABLE) && defined(CY_DEVICE_CCG6)) */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Fault_Vbus_RcpIntrHandler
****************************************************************************//**
*
* VBUS RCP fault interrupt handler function.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_Fault_Vbus_RcpIntrHandler(cy_stc_usbpd_context_t *context)
{
#if (defined(VBUS_RCP_ENABLE) && (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)))
    PPDSS_REGS_T pd = context->base;

    /*
     * RCP interrupt handling for CCG6.
     * Provider FET would have been turned off. We only need to clear the interrupt and start recovery.
     */
    if (
            ((pd->intr13_masked & (PDSS_INTR13_MASKED_CSA_OUT_CHANGED_MASKED)) != 0U) ||
            ((pd->intr13_masked & (PDSS_INTR13_MASKED_CSA_COMP_OUT_CHANGED_MASKED)) != 0U) ||
            ((pd->intr13_masked & (PDSS_INTR13_MASKED_CSA_VBUS_OVP_CHANGED_MASKED)) != 0U)
       )
    {
#if defined(CY_DEVICE_CCG6)
        /* Make sure P_CTRL does not get pulled up on Vin side. */
        pd->pgdo_pu_3_cfg = (PDSS_PGDO_PU_3_CFG_PGDO_EN_RCP_LV | PDSS_PGDO_PU_3_CFG_PGDO_EN_VBUS_OV_LV);
#endif /* defined(CY_DEVICE_CCG6) */

        /* Clear and disable all RCP related interrupts. */
        pd->intr13_mask &= ~(PDSS_INTR13_MASKED_CSA_OUT_CHANGED_MASKED |
                PDSS_INTR13_MASKED_CSA_COMP_OUT_CHANGED_MASKED |
                PDSS_INTR13_MASKED_CSA_VBUS_OVP_CHANGED_MASKED);
        pd->intr13 = (PDSS_INTR13_MASKED_CSA_OUT_CHANGED_MASKED |
                PDSS_INTR13_MASKED_CSA_COMP_OUT_CHANGED_MASKED |
                PDSS_INTR13_MASKED_CSA_VBUS_OVP_CHANGED_MASKED);
#if defined(CY_DEVICE_CCG6)
        /* Reset PGDO edge detector logic. */
        Cy_USBPD_Vbus_GdrvRstEdgeDet(context, true);
#endif /* defined(CY_DEVICE_CCG6) */

        /* Invoke RCP callback. */
        if (context->vbusRcpCbk != NULL)
        {
            (void)context->vbusRcpCbk(context, true);
        }
    }
#else
    CY_UNUSED_PARAMETER(context);
#endif /* (defined(VBUS_RCP_ENABLE) && defined(CY_DEVICE_CCG6)) */
}

/** \cond DOXYGEN_HIDE */
uint16_t Cy_USBPD_Vbus_MeasureCur(cy_stc_usbpd_context_t *context)
{
    CY_UNUSED_PARAMETER(context);
    return 0U;
}
/** \endcond */

#if defined(VBUS_SCP_ENABLE)

#define SCP_CUR_TRIP_6A                     (600u)
#define SCP_CUR_TRIP_10A                    (1000u)

#if defined(CY_DEVICE_CCG6)
/* CCG6: Location of SCP configuration data. */
#define SFLASH_SCP_TRIM_AV50_6A             (0x0FFFF407u)
#define SFLASH_SCP_TRIM_AV50_10A            (0x0FFFF408u)
#define SFLASH_SCP_TRIM_AV20_6A             (0x0FFFF40Eu)
#define SFLASH_SCP_TRIM_AV20_10A            (0x0FFFF40Fu)
#endif /* defined(CY_DEVICE_CCG6) */

#if defined(CY_DEVICE_PMG1S3)
#define SFLASH_SCP_TRIM_ROOM_FLAG           (0x0FFFF48Au)
#define SFLASH_SCP_TRIM_5V_6A_ROOM          (0x0FFFF462u)
#define SFLASH_SCP_TRIM_5V_6A_HOT           (0x0FFFF436u)
#define SFLASH_SCP_TRIM_5V_6A_COLD          (0x0FFFF44Cu)
#define SFLASH_SCP_TRIM_5V_6A_HOT_OFFSET    (0u)
#define SFLASH_SCP_TRIM_5V_10A_HOT_OFFSET   (1u)
#define SFLASH_SCP_TRIM_5V_6A_COLD_OFFSET   (22u)
#define SFLASH_SCP_TRIM_5V_10A_COLD_OFFSET  (23u)
#endif /* defined(CY_DEVICE_PMG1S3) */

#endif /* (defined(VBUS_SCP_ENABLE) && defined(CY_DEVICE_CCG6)) */

/*******************************************************************************
* Function Name: Cy_USBPD_Fault_Vbus_ScpEnable
****************************************************************************//**
*
* Enable Short Circuit Protection (SCP).
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param current
* Contract current in mA units.
*
* \param cb
* Callback function to be called on fault detection.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_Fault_Vbus_ScpEnable(cy_stc_usbpd_context_t *context, uint32_t current, cy_cb_vbus_fault_t cb)
{
#if (defined(VBUS_SCP_ENABLE) && (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)))
    PPDSS_REGS_T pd = context->base;
    uint32_t regval = 0U;
    uint32_t vsense = 0U;
#if defined(CY_DEVICE_CCG6)
    uint8_t debounce = 0U;
#endif /* defined(CY_DEVICE_CCG6) */
    uint32_t state = 0U;
    const uint8_t *sflash_scp_trim;
#if defined(CY_DEVICE_PMG1S3)
    const uint8_t *sflash_scp_trim_room_flag;
    PPDSS_TRIMS_REGS_T trimRegs = NULL;
#endif /* defined CY_DEVICE_PMG1S3 */

    /* Register SCP Callback. */
    context->vbusScpCbk = cb;

#if defined(CY_DEVICE_CCG6)
    debounce = context->usbpdConfig->vbusScpConfig->debounce;
    /* Divide debounce time in us by 5 to get glitch filter setting. */
    debounce /= 5U;
    if (debounce > 7U)
    {
        debounce = 7U;
    }

    if (context->usbpdConfig->vbusScpConfig->senseRes == 10U)
    {
        /* The SCP limit can be set to 6A/10A. The setting is chosen at the application level. */
        if (current <= SCP_CUR_TRIP_6A)
        {
            sflash_scp_trim = (const uint8_t *)(SFLASH_SCP_TRIM_AV20_6A);
        }
        else
        {
            sflash_scp_trim = (const uint8_t *)(SFLASH_SCP_TRIM_AV20_10A);
        }
    }
    else
    {
        if(current <= SCP_CUR_TRIP_6A)
        {
            sflash_scp_trim = (const uint8_t *)(SFLASH_SCP_TRIM_AV50_6A);
        }
        else
        {
            sflash_scp_trim = (const uint8_t *)(SFLASH_SCP_TRIM_AV50_10A);
        }
    }

    vsense = *sflash_scp_trim;

    state = Cy_SysLib_EnterCriticalSection ();

    Cy_USBPD_Fault_FetAutoModeDisable (context, true, CY_USBPD_VBUS_FILTER_ID_HSCSA_SCP);

    /* Configure Reference for comparator. */
    Cy_USBPD_SetRefgenVoltage (context, (uint8_t)vsense, (uint8_t)CY_USBPD_VREF_CSA_SCP);

    /* Make sure the CSA block is enabled. */
    pd->csa_scp_0_ctrl &= ~(PDSS_CSA_SCP_0_CTRL_PD_CSA | PDSS_CSA_SCP_0_CTRL_BW_MASK);
    pd->csa_scp_0_ctrl |= (PDSS_CSA_SCP_0_CTRL_SEL_OUT_D | PDSS_CSA_SCP_0_CTRL_CSA_ISO_N |
            (3UL << PDSS_CSA_SCP_0_CTRL_BW_POS));

    /* Enable pull-up of P_CTRL on V_IN side when SCP detected. */
    pd->pgdo_pu_3_cfg |= PDSS_PGDO_PU_3_CFG_PGDO_EN_SCP_LV;

    /*
     * Sets the same gain of 15 for SCP, removes latch from reset and sets glitch filter duration.
     * Reset the latch and then enable it in sample mode.
     */
    pd->csa_scp_1_ctrl = 0x00U;
    Cy_SysLib_DelayUs (10U);
    pd->csa_scp_1_ctrl = (0x04UL | ((uint32_t)debounce << 3));
#endif /* CY_DEVICE_CCG6 */

#if defined(CY_DEVICE_PMG1S3)
    sflash_scp_trim_room_flag = (const uint8_t *)(SFLASH_SCP_TRIM_ROOM_FLAG + (uint16_t)(context->port * 0x100u));
    trimRegs = context->trimsBase;
    /* Select the SCP trip point to 6A or 10A */
    if(current >= SCP_CUR_TRIP_10A)
    {

#if defined(USE_SCP_ROOM_TRIMS)
        /* Check if room trims are available */
        if(*sflash_scp_trim_room_flag == (uint8_t)1U)
        {
            sflash_scp_trim = (const uint8_t *)(SFLASH_SCP_TRIM_5V_6A_ROOM + 1u + (uint16_t)(context->port * 0x100u));
            vsense = *sflash_scp_trim;
        }
        else
#else
    (void)sflash_scp_trim_room_flag; 
#endif /* USE_SCP_ROOM_TRIMS */
        {
            sflash_scp_trim = (const uint8_t *)(SFLASH_SCP_TRIM_5V_6A_HOT);
            /* Take the average of the hot and the cold trims */
            vsense = (uint8_t)((sflash_scp_trim[SFLASH_SCP_TRIM_5V_10A_HOT_OFFSET] + sflash_scp_trim[SFLASH_SCP_TRIM_5V_10A_COLD_OFFSET]) / 2U);
        }
        pd->scp_rcp_0_ctrl |= PDSS_SCP_RCP_0_CTRL_SCP_RCP_T_SEL_SCP_10A;
    }
    else
    {
 #if defined(USE_SCP_ROOM_TRIMS)         
        if(*sflash_scp_trim_room_flag == (uint8_t)1U)
        {
            sflash_scp_trim = (const uint8_t *)(SFLASH_SCP_TRIM_5V_6A_ROOM + (uint16_t)(context->port * 0x100u));
            vsense = *sflash_scp_trim;
        }
        else
#else
    (void)sflash_scp_trim_room_flag; 
#endif /* USE_SCP_ROOM_TRIMS) */ 
        {
            sflash_scp_trim = (const uint8_t *)(SFLASH_SCP_TRIM_5V_6A_HOT);
            /* Take the average of the hot and the cold trims */
            vsense = (uint8_t)((sflash_scp_trim[SFLASH_SCP_TRIM_5V_6A_HOT_OFFSET] + sflash_scp_trim[SFLASH_SCP_TRIM_5V_6A_COLD_OFFSET]) / 2U);
        }
        pd->scp_rcp_0_ctrl &= ~PDSS_SCP_RCP_0_CTRL_SCP_RCP_T_SEL_SCP_10A;
    }
    
    /* Copy the trim value from SFLASH into the SCP Trims Register */
    trimRegs->trim_scp_rcp_0 = (uint32_t)vsense;

    /* Power-Up the SCP-RCP hard-IP */
    pd->scp_rcp_0_ctrl &= ~PDSS_SCP_RCP_0_CTRL_SCP_RCP_PD;
    Cy_SysLib_DelayUs(10);
    /* Power-Up the SCP block */
    pd->scp_rcp_0_ctrl &= ~PDSS_SCP_RCP_0_CTRL_SCP_RCP_PD_SCP; 
    Cy_SysLib_DelayUs(10);
    pd->scp_rcp_0_ctrl |= PDSS_SCP_RCP_0_CTRL_SCP_RCP_ISO_N;
  
    /* Remove the SCP latch from Reset mode and put it in sample mode*/
    pd->scp_rcp_0_ctrl &= ~PDSS_SCP_RCP_0_CTRL_SCP_RCP_T_SCP_RST_LATCH_N;
    Cy_SysLib_DelayUs(10);
    pd->scp_rcp_0_ctrl |= PDSS_SCP_RCP_0_CTRL_SCP_RCP_T_SCP_RST_LATCH_N;
#endif /* (CY_DEVICE_PMG1S3) */

    /* Bypass the filter to speed up interrupt trigger. */
    regval = pd->intr13_cfg_csa_scp_hs;
    regval &= ~(PDSS_INTR13_CFG_CSA_SCP_HS_CSA_SCP_FILT_EN | PDSS_INTR13_CFG_CSA_SCP_HS_CSA_SCP_FILT_CFG_MASK |
            PDSS_INTR13_CFG_CSA_SCP_HS_CSA_SCP_FILT_SEL_MASK | PDSS_INTR13_CFG_CSA_SCP_HS_CSA_SCP_FILT_RESET |
            PDSS_INTR13_CFG_CSA_SCP_HS_CSA_SCP_FILT_BYPASS);

    /* Set edge detection. */
    regval |= ((uint32_t)CY_USBPD_VBUS_FILTER_CFG_POS_EN_NEG_DIS << PDSS_INTR13_CFG_CSA_SCP_HS_CSA_SCP_FILT_CFG_POS);
    regval |= PDSS_INTR13_CFG_CSA_SCP_HS_CSA_SCP_FILT_BYPASS;

    pd->intr13_cfg_csa_scp_hs = regval;

    /* Clear and enable the SC Detect interrupt. */
    pd->intr13 = PDSS_INTR13_CSA_SCP_CHANGED;
    pd->intr13_mask |= PDSS_INTR13_MASK_CSA_SCP_CHANGED_MASK;

    /* Auto FET turn off mode. */
    Cy_USBPD_Fault_FetAutoModeEnable (context, true, CY_USBPD_VBUS_FILTER_ID_HSCSA_SCP);

    Cy_SysLib_ExitCriticalSection (state);
#else
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(current);
    (void)(cy_cb_vbus_fault_t)cb;
#endif /* (defined(VBUS_SCP_ENABLE) && (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))) */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Fault_Vbus_ScpDisable
****************************************************************************//**
*
* Disable Short Circuit Protection (SCP) control.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_Fault_Vbus_ScpDisable(cy_stc_usbpd_context_t *context)
{
#if (defined(VBUS_SCP_ENABLE) && (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)))
    PPDSS_REGS_T pd = context->base;
    uint32_t state;

    state = Cy_SysLib_EnterCriticalSection ();

    /* Deregister the callback. */
    context->vbusScpCbk = NULL;
#if defined(CY_DEVICE_CCG6)
    /* Power down the CSA SCP block */
    pd->csa_scp_0_ctrl |= PDSS_CSA_SCP_0_CTRL_PD_CSA;
    pd->csa_scp_0_ctrl &= ~(PDSS_CSA_SCP_0_CTRL_SEL_OUT_D | PDSS_CSA_SCP_0_CTRL_CSA_ISO_N);
#endif /* defined(CY_DEVICE_CCG6) */

#if defined(CY_DEVICE_PMG1S3)
    /* Power down the SCP block */
    pd->scp_rcp_0_ctrl |= PDSS_SCP_RCP_0_CTRL_SCP_RCP_PD_SCP; 
    /* Put the latch in Reset mode */
    pd->csa_scp_0_ctrl &= ~(PDSS_SCP_RCP_0_CTRL_SCP_RCP_ISO_N | PDSS_SCP_RCP_0_CTRL_SCP_RCP_T_SCP_RST_LATCH_N);
#endif /* defined(CY_DEVICE_PMG1S3) */

    /* Disable and clear interrupt. */
    pd->intr13 = PDSS_INTR13_CSA_SCP_CHANGED;
    pd->intr13_mask &= ~PDSS_INTR13_MASK_CSA_SCP_CHANGED_MASK;

    /* Disable FET turn off on SCP fault. */
    Cy_USBPD_Fault_FetAutoModeDisable (context, true, CY_USBPD_VBUS_FILTER_ID_HSCSA_SCP);
    Cy_SysLib_ExitCriticalSection (state);
#else
    CY_UNUSED_PARAMETER(context);
#endif /* (defined(VBUS_SCP_ENABLE) && (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))) */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Fault_Vbus_ScpIntrHandler
****************************************************************************//**
*
* VBUS SCP fault interrupt handler function.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_Fault_Vbus_ScpIntrHandler(cy_stc_usbpd_context_t *context)
{
#if (defined(VBUS_SCP_ENABLE) && (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)))
    PPDSS_REGS_T pd = context->base;
#if defined(CY_DEVICE_CCG6)
    /*
     * SCP interrupt handling for CCG6.
     * Provider FET would have been turned off. We only need to clear the interrupt and start recovery.
     */
    /* Make sure P_CTRL does not get pulled up on VBus side. */
    pd->pgdo_pu_3_cfg = PDSS_PGDO_PU_3_CFG_PGDO_EN_SCP_LV;

    /* Clear and disable interrupt. */
    pd->intr13_mask &= ~(PDSS_INTR13_MASKED_CSA_SCP_CHANGED_MASKED);
    pd->intr13 = PDSS_INTR13_MASKED_CSA_SCP_CHANGED_MASKED;

    /* Reset PGDO edge detector logic. */
    Cy_USBPD_Vbus_GdrvRstEdgeDet(context, true);
#endif /* defined(CY_DEVICE_CCG6) */

#if defined(CY_DEVICE_PMG1S3)
    /* Clear and disable interrupt. */
    pd->intr13_mask &= ~(PDSS_INTR13_MASKED_CSA_SCP_CHANGED_MASKED);
    pd->intr13 = PDSS_INTR13_MASKED_CSA_SCP_CHANGED_MASKED;
#endif /* defined(CY_DEVICE_PMG1S3) */

    /* Invoke SCP callback. */
    if (context->vbusScpCbk != NULL)
    {
        (void)context->vbusScpCbk(context, true);
    }
#else
    CY_UNUSED_PARAMETER(context);
#endif /* defined(VBUS_SCP_ENABLE) && (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)) */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Fault_CcSbuSetCB
****************************************************************************//**
*
* Register a callback function for CC/SBU OVP faults.
*
* \param context
* The pointer to the context structure \ref cy_stc_usbpd_context_t allocated
* by the user. The structure is used during the USBPD operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \param cb
* Callback function to be called on fault detection.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_Fault_CcSbuSetCB(cy_stc_usbpd_context_t *context, cy_cb_vbus_fault_t cb)
{
    context->ccSbuOvpCbk = cb;
}

/** \cond DOXYGEN_HIDE */
void Cy_USBPD_Fault_CcOvp_SetPending(cy_stc_usbpd_context_t *context)
{
    CY_UNUSED_PARAMETER(context);
}

void Cy_USBPD_Fault_Vbus_SetCsaRsense(cy_stc_usbpd_context_t *context, uint8_t rsense)
{
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(rsense);
}

bool Cy_USBPD_Vbus_CompGetStatus(cy_stc_usbpd_context_t *context, cy_en_usbpd_vbus_filter_id_t id, bool isFiltered)
{
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(id);
    CY_UNUSED_PARAMETER(isFiltered);

    return false;
}

/** \endcond */

#endif /* (defined(CY_IP_MXUSBPD) || defined(CY_IP_M0S8USBPD)) */

/* [] END OF FILE */
