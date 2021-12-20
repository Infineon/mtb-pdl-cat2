/***************************************************************************//**
* \file cy_usbpd_typec.c
* \version 1.30
*
* The source file of the USBPD TypeC driver.
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

#include "cy_usbpd_defines.h"
#include "cy_usbpd_common.h"
#include "cy_usbpd_typec.h"
#include "cy_usbpd_vbus_ctrl.h"
#include "cy_usbpd_bch.h"
#include "cy_usbpd_hpd.h"

/* No of Rd rows and entries per row, in the thresholds LUT. */
#define RD_ROW_NO                       (3u)
#define RD_COL_WIDTH                    (4u)

#define AMUX_CTRL_EA_TOP_REFGEN_SEL7_EN         (0x02U)

/* PMG1S0 enable VBUS_IN resistor divider for TYPE-C VBUS monitoring using ADC. */
#define AMUX_ADC_PMG1S0_VBUS_IN_8P_EN_POS        (7)
#define AMUX_ADC_PMG1S0_VBUS_IN_20P_EN_POS       (4)

/* PMG1S0 discharge drive strength settings. */
#define PMG1S0_DISCHG_DS_VBUS_C_00               (1U)
#define PMG1S0_DISCHG_DS_VBUS_IN_00              (1U)

/* TRIM0_TX_TRIM register needs to be overwritten by firmware. */
#define TRIM0_TX_TRIM_VALUE_3A                  (2U)
#define TRIM0_TX_TRIM_VALUE_900MA               (0U)

/* The TRIM6 and TRIM3 registers needs to be overwritten by firmware. */
#define SILICON_TRIM6_V1P575_TRIM_VALUE         (3U)
#define SILICON_TRIM3_V0P55_TRIM_VALUE          (2U)

#if defined(CY_DEVICE_PMG1S3)
#define SFLASH_SCP_RCP_VREF_TRIM_HOT            (0x0FFFF430u)
#define SFLASH_SCP_RCP_VREF_TRIM_COLD           (0x0FFFF446u)
#define SFLASH_SCP_RCP_VREF_TRIM_ROOM           (0x0FFFF45Cu)
#endif /* defined(CY_DEVICE_PMG1S3) */

#if defined(CY_DEVICE_CCG6)
#define CLK_LF_FILT_NUM (2UL)
#endif /* defined(CY_DEVICE_CCG6) */

#if defined(CY_DEVICE_PMG1S3)
#define CLK_LF_FILT_NUM (4UL)
#define CLK_LF_FILT_SBU1 (1UL)
#define CLK_LF_FILT_SBU2 (2UL)
#endif /* defined(CY_DEVICE_PMG1S3) */

/* SBU comparator reference set voltage (130 + 107*10 = 1200 mV)*/
#define SBU_COMP_VREF_1_2_V            107UL
/**
 * Type C voltage thresholds (in mV) as per Section 4.11.3 of Type C
 * specification Rev1.
 */
static const uint8_t thresholds[4][4] =
{
    {PD_CMP_VSEL_0_2_V, PD_CMP_VSEL_1_77_V, 0, 0}, /* Rp USB default row. */
    {PD_CMP_VSEL_0_4_V, PD_CMP_VSEL_1_77_V, 0, 0}, /* Rp 1.5A row. */
    {PD_CMP_VSEL_0_8_V, PD_CMP_VSEL_2_6_V, 0, 0}, /* Rp 3A row. */
    {PD_CMP_VSEL_0_2_V, PD_CMP_VSEL_0_655_V, PD_CMP_VSEL_1_235_V, PD_CMP_VSEL_2_6_V}, /* RD row. */
};

void Cy_USBPD_Phy_VbusDetachCbk (
        void *context,
        bool  compOut)
{
    /* Do nothing. */
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(compOut);
}

/* Returns the current status on the CC line (rp_cc_status_t or rd_cc_status_t). */
static uint8_t Cy_USBPD_TypeC_GetRpRdStatus (
        cy_stc_usbpd_context_t *context, 
        uint8_t channel, 
        bool rd_idx)
{
#if (!CY_PD_SINK_ONLY)
    cy_stc_pd_dpm_config_t *dpmConfig = context->dpmGetConfig();
    bool     isSrc = true;
#endif /* (!CY_PD_SINK_ONLY) */

    PPDSS_REGS_T pd = context->base;
    uint32_t rval = 0;
    uint32_t temp = 0;
    uint8_t  out = 0;
    uint8_t index = (uint8_t)rd_idx;
    uint32_t value = 0;

#if (!CY_PD_SINK_ONLY)
    /* Set default output. */
    if (dpmConfig->curPortRole == CY_PD_PRT_ROLE_SOURCE)
    {
        out = (uint8_t)CY_PD_RP_OPEN;
    }
    else
#endif /* (!CY_PD_SINK_ONLY) */
    {
        out = ((uint8_t)CY_PD_RD_RA + index);
#if (!CY_PD_SINK_ONLY)
        isSrc = false;
#endif /* (!CY_PD_SINK_ONLY) */

#if PSVP_FPGA_ENABLE
        /* Set the RP SEL value based on rd_idx to ensure that the AFE uses the correct value. */
        if (rd_idx == true)
        {
            pd->cc_ctrl_0 = (pd->cc_ctrl_0 & ~PDSS_CC_CTRL_0_RP_MODE_MASK) | (1UL << PDSS_CC_CTRL_0_RP_MODE_POS);
        }
        else
        {
            pd->cc_ctrl_0 = (pd->cc_ctrl_0 & ~PDSS_CC_CTRL_0_RP_MODE_MASK);
        }
#endif /* PSVP_FPGA_ENABLE */
    }

    /* Connect both the Up/Dn comparators to the active CC line. */
    if (channel == CY_PD_CC_CHANNEL_2)
    {
        rval = (PDSS_CC_CTRL_0_CMP_DN_CC1V2 | PDSS_CC_CTRL_0_CMP_UP_CC1V2);

#if PSVP_FPGA_ENABLE
        rval |= PDSS_CC_CTRL_0_CC_1V2;
#endif /* PSVP_FPGA_ENABLE */
    }
#if PSVP_FPGA_ENABLE
    else
    {
        rval &= ~PDSS_CC_CTRL_0_CC_1V2;
    }
#endif /* PSVP_FPGA_ENABLE */

#if (!CY_PD_SINK_ONLY)
    if (isSrc)
    {
        /*
         * Set the threshold of the Dn comparator to Ra level and the Up
         * comparator to Rp open level.
         */
        rval |= (((uint32_t)thresholds[dpmConfig->srcCurLevelLive][0]) << PDSS_CC_CTRL_0_CMP_DN_VSEL_POS) |
            (((uint32_t)thresholds[dpmConfig->srcCurLevelLive][1]) << PDSS_CC_CTRL_0_CMP_UP_VSEL_POS);
    }
    else
#endif /* (!CY_PD_SINK_ONLY) */
    {
        /* Set the Dn comparator to vRdUSB and the Up comparator to vRd1.5A. */
        rval |= (((uint32_t)thresholds[RD_ROW_NO][index]) << PDSS_CC_CTRL_0_CMP_DN_VSEL_POS) |
            (((uint32_t)thresholds[RD_ROW_NO][index + 1U]) << PDSS_CC_CTRL_0_CMP_UP_VSEL_POS);
    }

    value = (uint32_t)(
            PDSS_CC_CTRL_0_CMP_DN_CC1V2 |
            PDSS_CC_CTRL_0_CMP_UP_CC1V2 |
#if PSVP_FPGA_ENABLE
            PDSS_CC_CTRL_0_CC_1V2 |
#endif /* PSVP_FPGA_ENABLE */
            PDSS_CC_CTRL_0_CMP_DN_VSEL_MASK |
            PDSS_CC_CTRL_0_CMP_UP_VSEL_MASK
            );

    temp = (uint32_t)(pd->cc_ctrl_0 & value);
    if (temp != rval)
    {
        pd->cc_ctrl_0 &= ~(value);
        pd->cc_ctrl_0 |= rval;

        /* Delay to allow references to settle. */
        Cy_SysLib_DelayUs (50);
    }

#if defined(CY_IP_MXUSBPD)

    temp = (uint32_t)(pd->intr1_status & (PDSS_INTR1_STATUS_VCMP_DN_STATUS | PDSS_INTR1_STATUS_VCMP_UP_STATUS));
    if ((temp & PDSS_INTR1_STATUS_VCMP_UP_STATUS) != 0U)
    {
#if (!CY_PD_SINK_ONLY)
        if (!isSrc)
#endif /* (!CY_PD_SINK_ONLY) */
        {
            out = ((uint8_t)CY_PD_RD_1_5A + index);
        }
    }
    else
    {
        if ((temp & PDSS_INTR1_STATUS_VCMP_DN_STATUS) != 0U)
        {
#if (!CY_PD_SINK_ONLY)
            if (isSrc)
            {
                out = (uint8_t)(CY_PD_RP_RD);
            }
            else
#endif /* (!CY_PD_SINK_ONLY) */
            {
                out = ((uint8_t)CY_PD_RD_USB + index);
            }
        }
#if (!CY_PD_SINK_ONLY)
        else
        {
            if (isSrc)
            {
                out = (uint8_t)(CY_PD_RP_RA);
            }
        }
#endif /* (!CY_PD_SINK_ONLY) */
    }

#elif defined(CY_IP_M0S8USBPD)

    temp = pd->status;
    if ((temp & PDSS_STATUS_VCMP_UP_STATUS) != 0U)
    {
#if (!CY_PD_SINK_ONLY)
        if (!isSrc)
#endif /* (!CY_PD_SINK_ONLY) */
        {
            out = ((uint8_t)CY_PD_RD_1_5A + index);
        }
    }
    else
    {
        if ((temp & PDSS_STATUS_VCMP_DN_STATUS) != 0U)
        {
#if (!CY_PD_SINK_ONLY)
            if (isSrc)
            {
                out = (uint8_t)(CY_PD_RP_RD);
            }
            else
#endif /* (!CY_PD_SINK_ONLY) */
            {
                out = ((uint8_t)CY_PD_RD_USB + index);
            }
        }
#if (!CY_PD_SINK_ONLY)
        else
        {
            if (isSrc)
            {
                out = (uint8_t)CY_PD_RP_RA;
            }
        }
#endif /* (!CY_PD_SINK_ONLY) */
    }

#else
    #error "Unsupported IP"
#endif

    return out;
}


#if SYS_DEEPSLEEP_ENABLE
static void Cy_USBPD_TypeC_PhyDetectCCRise (
        cy_stc_usbpd_context_t *context,
        bool rpConnected)
{
    PPDSS_REGS_T pd = context->base;
    cy_stc_pd_dpm_config_t *dpmConfig = context->dpmGetConfig();
    uint8_t threshold_row = RD_ROW_NO;

    /* Connect UP comparator to CC1 and down comparator to CC2 */
    pd->cc_ctrl_0 = PDSS_CC_CTRL_0_CMP_DN_CC1V2 |
        (pd->cc_ctrl_0 &
         ~(PDSS_CC_CTRL_0_CMP_UP_CC1V2 | PDSS_CC_CTRL_0_CMP_UP_VSEL_MASK | PDSS_CC_CTRL_0_CMP_DN_VSEL_MASK));

    if (rpConnected)
    {
        /* Set the comparator voltage references based on the active Rp level applied by us. */
        threshold_row = dpmConfig->srcCurLevelLive;
    }

    pd->cc_ctrl_0 |= ((thresholds[threshold_row][0]) << PDSS_CC_CTRL_0_CMP_UP_VSEL_POS) |
        ((thresholds[threshold_row][0]) << PDSS_CC_CTRL_0_CMP_DN_VSEL_POS);

#if defined(CY_IP_MXUSBPD)
    pd->intr1_cfg_vcmp_up_down_ls &= ~(PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_UP_CFG_MASK |
            PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_UP_FILT_EN |
            PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_UP_FILT_BYPASS |
            PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_DN_CFG_MASK |
            PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_DN_FILT_EN |
            PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_DN_FILT_BYPASS);
    pd->intr1_cfg_vcmp_up_down_ls |= (((CY_USBPD_ADC_INT_RISING) << PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_UP_CFG_POS) |
            ((CY_USBPD_ADC_INT_RISING) << PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_DN_CFG_POS) |
            (PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_UP_FILT_EN | PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_DN_FILT_EN));
#elif defined(CY_IP_M0S8USBPD)
    pd->intr_1_cfg &= ~(PDSS_INTR_1_CFG_VCMP_UP_CFG_MASK | PDSS_INTR_1_CFG_VCMP_DN_CFG_MASK);
    pd->intr_1_cfg |= ((CY_USBPD_ADC_INT_RISING) << PDSS_INTR_1_CFG_VCMP_UP_CFG_POS) |
        ((CY_USBPD_ADC_INT_RISING) << PDSS_INTR_1_CFG_VCMP_DN_CFG_POS);
#endif /* defined(CY_IP_M0S8USBPD) */

    Cy_SysLib_DelayUs (10);
    pd->intr1 = (PDSS_INTR1_VCMP_UP_CHANGED | PDSS_INTR1_VCMP_DN_CHANGED | PDSS_INTR1_VCMP_LA_CHANGED);
    pd->intr1_mask |= (PDSS_INTR1_VCMP_UP_CHANGED | PDSS_INTR1_VCMP_DN_CHANGED);

    /* If comparators have already triggered, then set any one interrupt to ensure device does not go into sleep. */
#if defined(CY_IP_MXUSBPD)
    if (pd->intr1_status & (PDSS_INTR1_STATUS_VCMP_UP_STATUS | PDSS_INTR1_STATUS_VCMP_DN_STATUS))
    {
        pd->intr1_set |= PDSS_INTR1_VCMP_UP_CHANGED;
    }
#elif defined(CY_IP_M0S8USBPD)
    if (pd->status & (PDSS_STATUS_VCMP_UP_STATUS | PDSS_STATUS_VCMP_DN_STATUS))
    {
        pd->intr1_set |= PDSS_INTR1_VCMP_UP_CHANGED;
    }
#endif /* CY_IP */
}

#endif /* SYS_DEEPSLEEP_ENABLE */

/*******************************************************************************
* Function Name: Cy_USBPD_Pump_Disable
****************************************************************************//**
* 
* Disable the charge pump
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \param index
* Charge pump index
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_Pump_Disable (
        cy_stc_usbpd_context_t *context, 
        uint8_t index)
{
#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))
    context->base->pump5v_ctrl[index] |= PDSS_PUMP5V_CTRL_PUMP5V_BYPASS_LV;
    Cy_SysLib_DelayUs (10);
    context->base->pump5v_ctrl[index]  = PDSS_PUMP5V_CTRL_DEFAULT;
#elif (defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S))
    CY_UNUSED_PARAMETER(index);
    context->base->pump5v_ctrl |= PDSS_PUMP5V_CTRL_PUMP5V_BYPASS_LV;
    Cy_SysLib_DelayUs (10);
    context->base->pump5v_ctrl = PDSS_PUMP5V_CTRL_DEFAULT;
#else /* CY_DEVICE_CCG3PA */
    CY_UNUSED_PARAMETER(index);
    context->base->pump_ctrl |= (PDSS_PUMP_CTRL_PD_PUMP | PDSS_PUMP_CTRL_BYPASS_LV);
#endif /* (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)) */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Pump_Enable
****************************************************************************//**
* 
* Enable the charge pump
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \param index
* Charge pump index
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_Pump_Enable (
        cy_stc_usbpd_context_t *context,
        uint8_t index)
{
#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))
    PPDSS_REGS_T pd = context->base;
    /* Enable the pump only if it is not already on. */
    if ((pd->pump5v_ctrl[index] & PDSS_PUMP5V_CTRL_PUMP5V_PUMP_EN) == 0u)
    {
        pd->pump5v_ctrl[index] = PDSS_PUMP5V_CTRL_PUMP5V_PUMP_EN | PDSS_PUMP5V_CTRL_PUMP5V_BYPASS_LV;
        Cy_SysLib_DelayUs (10);
        pd->pump5v_ctrl[index] = PDSS_PUMP5V_CTRL_PUMP5V_PUMP_EN;
        Cy_SysLib_DelayUs (40);
        pd->pump5v_ctrl[index] = PDSS_PUMP5V_CTRL_PUMP5V_PUMP_EN | PDSS_PUMP5V_CTRL_PUMP5V_DEL_PUMP_EN;
    }
#elif(defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S))
    /* Enable the pump only if it is not already on. */
    PPDSS_REGS_T pd = context->base;
    if ((pd->pump5v_ctrl & PDSS_PUMP5V_CTRL_PUMP5V_PUMP_EN) == 0u)
    {
       context->base->pump5v_ctrl = (PDSS_PUMP5V_CTRL_PUMP5V_PUMP_EN | PDSS_PUMP5V_CTRL_PUMP5V_BYPASS_LV);
    }
    CY_UNUSED_PARAMETER(index);
#else /* CY_DEVICE_CCG3PA */
    CY_UNUSED_PARAMETER(index);
    context->base->pump_ctrl &= ~(PDSS_PUMP_CTRL_PD_PUMP | PDSS_PUMP_CTRL_BYPASS_LV);
#endif /* (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)) */
}

/*******************************************************************************
* Function Name: Cy_USBPD_TypeC_Init
****************************************************************************//**
* 
* This function initializes the Type-C registers in the PD block.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* CY_USBPD_STAT_SUCCESS if operation is successful, 
* CY_USBPD_STAT_BAD_PARAM if the context pointer is invalid.
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_TypeC_Init (
        cy_stc_usbpd_context_t *context)
{
    PPDSS_REGS_T pd = NULL;

    if (context == NULL)
    {
        return CY_USBPD_STAT_BAD_PARAM;
    }

    pd = context->base;

    pd->cc_ctrl_0 &= ~(PDSS_CC_CTRL_0_HYST_MODE | PDSS_CC_CTRL_0_EN_HYST | PDSS_CC_CTRL_0_CMP_LA_VSEL_MASK);
    pd->cc_ctrl_0 |= (PDSS_CC_CTRL_0_CMP_LA_VSEL_CFG << PDSS_CC_CTRL_0_CMP_LA_VSEL_POS);

#if defined(CY_IP_MXUSBPD)

    pd->dpslp_ref_ctrl &= ~PDSS_DPSLP_REF_CTRL_PD_DPSLP;

    /*
     * Up/Down comparators filter will only be enabled before going to
     * deepsleep and disabled after coming out of deepsleep.
     */
    pd->intr1_cfg_vcmp_up_down_ls &= ~(PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_DN_FILT_EN |
                                       PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_UP_FILT_EN);

#if defined(CY_DEVICE_CCG3PA)
    /* Disable filter on comparator 1 and comparator 2 outputs. */
    pd->intr3_cfg_adc_hs[0] &= ~(PDSS_INTR3_CFG_ADC_HS_FILT_EN);
    pd->intr3_cfg_adc_hs[1] &= ~(PDSS_INTR3_CFG_ADC_HS_FILT_EN);
#endif /* defined(CY_DEVICE_CCG3PA) */

#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))
    /* Disable filter on comparator output. */
    pd->intr3_cfg_adc_hs &= ~(PDSS_INTR3_CFG_ADC_HS_FILT_EN);
    pd->intr3_cfg_adc_hs |= PDSS_INTR3_CFG_ADC_HS_FILT_BYPASS;
#endif /* (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)) */

    /* Leave the CC pump enabled. */
    Cy_USBPD_Pump_Enable (context, 0);

#elif defined(CY_IP_M0S8USBPD)

    /*
     * Up/Down comparators filter will only be enabled before going to
     * deepsleep and disabled after coming out of deepsleep.
     */
    pd->intr_1_cfg &= ~(PDSS_INTR_1_CFG_VCMP_DN_FILT_EN | PDSS_INTR_1_CFG_VCMP_UP_FILT_EN);

    /* Disable filter on comparator 1 and comparator 2 outputs. */
    pd->intr_1_cfg &= ~(PDSS_INTR_1_CFG_CMP_OUT1_FILT_EN | PDSS_INTR_1_CFG_CMP_OUT2_FILT_EN);

    /* Leave the CC pump enabled. */
    pd->pump_ctrl &= ~(PDSS_PUMP_CTRL_PD_PUMP | PDSS_PUMP_CTRL_BYPASS_LV);

#else
    #error "Unsupported IP"
#endif

    return CY_USBPD_STAT_SUCCESS;
}

/*******************************************************************************
* Function Name: Cy_USBPD_TypeC_Start
****************************************************************************//**
* 
* This function starts the Type C Line comparators.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* CY_USBPD_STAT_SUCCESS if operation is successful, 
* CY_USBPD_STAT_BAD_PARAM if the context pointer is invalid.
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_TypeC_Start (
        cy_stc_usbpd_context_t *context)
{
    PPDSS_REGS_T pd = NULL;
#if defined(CY_IP_MXUSBPD)
    uint32_t regVal = 0;
#endif /* defined(CY_IP_MXUSBPD) */

    if (context == NULL)
    {
        return CY_USBPD_STAT_BAD_PARAM;
    }

    pd = context->base;

    /* Power up the block. */
    pd->cc_ctrl_0 &= ~PDSS_CC_CTRL_0_PWR_DISABLE;

#if defined(CY_IP_MXUSBPD)

    /* Enable PUMP */
    Cy_USBPD_Pump_Enable (context, 0);

    Cy_SysLib_DelayUs(50);

    pd->cc_ctrl_0  |= (PDSS_CC_CTRL_0_CMP_EN | PDSS_CC_CTRL_0_RX_EN);

    /* Use voltage reference from refgen to define CC tx_swing. */
    pd->cc_ctrl_1 |= PDSS_CC_CTRL_1_CC_VREF_1P1_SEL;

#if (defined(CY_DEVICE_CCG3PA) && (VBUS_IN_DISCHARGE_EN))
    /* Set VBUS_IN discharge threshold to 5.5V */

    pd->dischg_shv_ctrl[1] |= PDSS_DISCHG_SHV_CTRL_DISCHG_EN_CFG;

    /* Resistor div on VBUS IN already set to 10% in HW */
    /* Configure Reference for comparator. */
    regVal = pd->refgen_4_ctrl;
    regVal &= ~(PDSS_REFGEN_4_CTRL_SEL11_MASK);
    regVal |= (PDSS_VBUS_IN_REF_SEL_VAL << PDSS_REFGEN_4_CTRL_SEL11_POS);
    pd->refgen_4_ctrl = regVal;

    /* Enable Comparator. */
    pd->comp_ctrl[COMP_ID_VBUS_DISCHARGE] |= PDSS_COMP_CTRL_COMP_ISO_N;
#endif /* (defined(CY_DEVICE_CCG3PA) && (VBUS_IN_DISCHARGE_EN !=0) */

#if (defined(CY_DEVICE_PMG1S3) && (SBU_LEVEL_DETECT_EN != 0))

    pd->refgen_0_ctrl &= ~PDSS_REFGEN_0_CTRL_REFGEN_PD;
    pd->refgen_0_ctrl  |= PDSS_REFGEN_0_CTRL_REFGEN_VREFIN_SEL |
                          PDSS_REFGEN_0_CTRL_REFGEN_VREFIN_MON_SEL;

    /* Configure Reference for comparator. */
    regVal = pd->refgen_2_ctrl;
    regVal &= ~(PDSS_REFGEN_2_CTRL_SEL7_MASK);
    regVal |= (SBU_COMP_VREF_1_2_V << PDSS_REFGEN_2_CTRL_SEL7_POS);
    pd->refgen_2_ctrl = regVal;

    regVal = pd->refgen_3_ctrl;
    regVal &= ~(PDSS_REFGEN_3_CTRL_SEL8_MASK);
    regVal |= (SBU_COMP_VREF_1_2_V << PDSS_REFGEN_3_CTRL_SEL8_POS);
    pd->refgen_3_ctrl = regVal;

    /* Enable Comparator. */
    pd->comp_ctrl[CY_USBPD_VBUS_COMP_ID_P0_SBU1] |= PDSS_COMP_CTRL_COMP_ISO_N;
    pd->comp_ctrl[CY_USBPD_VBUS_COMP_ID_P0_SBU2] |= PDSS_COMP_CTRL_COMP_ISO_N;

    Cy_SysLib_DelayUs(10);

    /* Filter configuration. */
    pd->intr7_filter_cfg[0] = (PDSS_INTR7_FILTER_CFG_CLK_LF_FILT_BYPASS |
                    PDSS_INTR7_FILTER_CFG_CLK_LF_FILT_RESET |
                    ((uint32_t)CY_USBPD_VBUS_FILTER_CFG_POS_EN_NEG_EN << PDSS_INTR7_FILTER_CFG_FILT_CFG_POS));

    pd->intr7_filter_cfg[1] = (PDSS_INTR7_FILTER_CFG_CLK_LF_FILT_BYPASS |
                    PDSS_INTR7_FILTER_CFG_CLK_LF_FILT_RESET |
                    ((uint32_t)CY_USBPD_VBUS_FILTER_CFG_POS_EN_NEG_EN << PDSS_INTR7_FILTER_CFG_FILT_CFG_POS));


    /* Clear interrupt. */
    pd->intr7 = (1U << CLK_LF_FILT_SBU1);
    pd->intr7 = (1U << CLK_LF_FILT_SBU2);

    /* Enable Interrupt. */
    pd->intr7_mask |= (1U << CLK_LF_FILT_SBU1);
    pd->intr7_mask |= (1U << CLK_LF_FILT_SBU2);
#endif /* (defined(CY_DEVICE_PMG1S3) && (SBU_LEVEL_DETECT_EN != 0)) */

#if defined(CY_DEVICE_CCG3PA)
    pd->refgen_0_ctrl &= ~PDSS_REFGEN_0_CTRL_REFGEN_PD;
    pd->refgen_0_ctrl  |= PDSS_REFGEN_0_CTRL_REFGEN_VREFIN_SEL |
                          PDSS_REFGEN_0_CTRL_REFGEN_VREFIN_MON_SEL;

    /* Set SEL7 reference (used by EA shunt regulator during deep sleep) to 1.2V */
    pd->refgen_2_ctrl &= ~PDSS_REFGEN_2_CTRL_SEL7_MASK;
    pd->refgen_2_ctrl |= (107UL << PDSS_REFGEN_2_CTRL_SEL7_POS);

    /* Set SEL14 reference (used by cc_shvt block for tx_swing to 1.12 V. */
    pd->refgen_4_ctrl &= ~PDSS_REFGEN_4_CTRL_SEL14_MASK;
    pd->refgen_4_ctrl |= (4UL << PDSS_REFGEN_4_CTRL_SEL14_POS);

    /* Give some delay for references to settle. */
    Cy_SysLib_DelayUs (50);
#endif /* defined(CY_DEVICE_CCG3PA) */

    /* Disable and bypass all filters. */
    regVal = pd->intr1_cfg_cc1_cc2_ls;
    regVal &= ~(PDSS_INTR1_CFG_CC1_CC2_LS_CC1_FILT_EN | PDSS_INTR1_CFG_CC1_CC2_LS_CC2_FILT_EN);
    regVal |= (PDSS_INTR1_CFG_CC1_CC2_LS_CC1_FILT_BYPASS | PDSS_INTR1_CFG_CC1_CC2_LS_CC2_FILT_BYPASS);
    pd->intr1_cfg_cc1_cc2_ls = regVal;

    regVal = pd->intr1_cfg_vcmp_up_down_ls;
    regVal &= ~(PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_UP_FILT_EN | PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_DN_FILT_EN);
    regVal |= (PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_UP_FILT_BYPASS | PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_DN_FILT_BYPASS);
    pd->intr1_cfg_vcmp_up_down_ls = regVal;

    /* Set LA config for wakeup */
    pd->intr1_cfg |= PDSS_INTR1_CFG_VCMP_LA_CFG_MASK;

#if defined (CY_DEVICE_CCG6)
    /* Enable the filter associated with CC1/2 OVP detection. */
    pd->intr1_cfg_cc12_ovp_hs = (
            (4UL << PDSS_INTR1_CFG_CC12_OVP_HS_CC1_OVP_FILT_SEL_POS) |
            ((uint32_t)CY_USBPD_VBUS_FILTER_CFG_POS_EN_NEG_DIS << PDSS_INTR1_CFG_CC12_OVP_HS_CC1_OVP_FILT_CFG_POS) |
            (PDSS_INTR1_CFG_CC12_OVP_HS_CC1_OVP_DPSLP_MODE) |
            (4UL << PDSS_INTR1_CFG_CC12_OVP_HS_CC2_OVP_FILT_SEL_POS) |
            ((uint32_t)CY_USBPD_VBUS_FILTER_CFG_POS_EN_NEG_DIS << PDSS_INTR1_CFG_CC12_OVP_HS_CC2_OVP_FILT_CFG_POS) |
            (PDSS_INTR1_CFG_CC12_OVP_HS_CC2_OVP_DPSLP_MODE)
            );
    pd->intr1_cfg_cc12_ovp_hs |= (
            PDSS_INTR1_CFG_CC12_OVP_HS_CC1_OVP_FILT_EN |
            PDSS_INTR1_CFG_CC12_OVP_HS_CC2_OVP_FILT_EN
            );

    /* Clear and Enable CC1/2 OVP change interrupt. */
    pd->intr1       = (PDSS_INTR1_CC1_OVP_CHANGED | PDSS_INTR1_CC2_OVP_CHANGED);
    pd->intr1_mask |= (PDSS_INTR1_MASK_CC1_OVP_CHANGED_MASK | PDSS_INTR1_MASK_CC2_OVP_CHANGED_MASK);

    /* If OVP condition is present, set the interrupt again. */
    if ((pd->intr1_status & PDSS_INTR1_STATUS_CC1_OVP_FILT) != 0U)
    {
        pd->intr1_set |= PDSS_INTR1_SET_CC1_OVP_CHANGED;
    }
    if ((pd->intr1_status & PDSS_INTR1_STATUS_CC2_OVP_FILT) != 0U)
    {
        pd->intr1_set |= PDSS_INTR1_SET_CC2_OVP_CHANGED;
    }

    /* Enable the SBU OVP filters. */
    pd->intr3_cfg_sbu20_ovp_hs = (
            (4UL << PDSS_INTR3_CFG_SBU20_OVP_HS_SBU1_FILT_SEL_POS) |
            ((uint32_t)CY_USBPD_VBUS_FILTER_CFG_POS_EN_NEG_DIS << PDSS_INTR3_CFG_SBU20_OVP_HS_SBU1_FILT_CFG_POS) |
            (PDSS_INTR3_CFG_SBU20_OVP_HS_SBU1_DPSLP_MODE) |
            (4UL << PDSS_INTR3_CFG_SBU20_OVP_HS_SBU2_FILT_SEL_POS) |
            ((uint32_t)CY_USBPD_VBUS_FILTER_CFG_POS_EN_NEG_DIS << PDSS_INTR3_CFG_SBU20_OVP_HS_SBU2_FILT_CFG_POS) |
            (PDSS_INTR3_CFG_SBU20_OVP_HS_SBU2_DPSLP_MODE)
            );
    pd->intr3_cfg_sbu20_ovp_hs |= (
            PDSS_INTR3_CFG_SBU20_OVP_HS_SBU1_FILT_EN |
            PDSS_INTR3_CFG_SBU20_OVP_HS_SBU2_FILT_EN
            );

    /* Enable OVP detection on the SBU pins. */
    pd->intr3       = PDSS_INTR3_SBU1_SBU2_OVP_CHANGED_MASK;
    pd->intr3_mask |= PDSS_INTR3_SBU1_SBU2_OVP_CHANGED_MASK;

#if PMG1_V5V_CHANGE_DETECT
    /* Enable the V5V detect comparator. */
    pd->vconn20_ctrl |= PDSS_VCONN20_CTRL_EN_COMP;

    /* Enable the V5V detect filter. */
    pd->intr1_cfg     = (pd->intr1_cfg & ~(PDSS_INTR1_CFG_V5V_FILT_EN |
                PDSS_INTR1_CFG_V5V_CFG_MASK | PDSS_INTR1_CFG_V5V_FILT_BYPASS));
    pd->intr1_cfg    |= PDSS_INTR1_CFG_V5V_CFG_MASK;

    Cy_SysLib_DelayUs (10);

    pd->intr1_cfg    |= PDSS_INTR1_CFG_V5V_FILT_EN;

    /* Clear and enable the V5V change interrupt. */
    pd->intr1       = PDSS_INTR1_V5V_CHANGED;
    pd->intr1_mask |= PDSS_INTR1_MASK_V5V_CHANGED_MASK;
#endif /* PMG1_V5V_CHANGE_DETECT */

#endif /* defined (CY_DEVICE_CCG6) */

#elif defined(CY_IP_M0S8USBPD)

    /* Enable PUMP */
    pd->pump_ctrl &= ~(PDSS_PUMP_CTRL_PD_PUMP  | PDSS_PUMP_CTRL_BYPASS_LV);
    Cy_SysLib_DelayUs (50);

    /* Enable V5V change detect interrupt. */
    pd->intr1         = PDSS_INTR1_V5V_CHANGED;
    pd->intr1_mask   |= PDSS_INTR1_MASK_V5V_CHANGED_MASK;
    pd->pfet300_ctrl |= PDSS_PFET300_CTRL_EN_COMP;

    pd->cc_ctrl_0 |= (PDSS_CC_CTRL_0_CMP_EN | PDSS_CC_CTRL_0_RX_EN);

#endif

    return CY_USBPD_STAT_SUCCESS;
}

/*******************************************************************************
* Function Name: Cy_USBPD_TypeC_Stop
****************************************************************************//**
* 
* This function stops the Type C Line comparators.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* CY_USBPD_STAT_SUCCESS if operation is successful, 
* CY_USBPD_STAT_BAD_PARAM if the context pointer is invalid.
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_TypeC_Stop (
        cy_stc_usbpd_context_t *context)
{
    PPDSS_REGS_T pd = NULL;

    if (context == NULL)
    {
        return CY_USBPD_STAT_BAD_PARAM;
    }

    pd = context->base;

#if defined (CY_IP_MXUSBPD)

#if defined (CY_DEVICE_CCG6)
#if PMG1_V5V_CHANGE_DETECT
    /* Clear and disable the V5V change detect interrupt. */
    pd->intr1       = PDSS_INTR1_V5V_CHANGED;
    pd->intr1_mask &= ~PDSS_INTR1_MASK_V5V_CHANGED_MASK;

    /* Disable and bypass the V5V detect filter. */
    pd->intr1_cfg = (pd->intr1_cfg & ~(PDSS_INTR1_CFG_V5V_FILT_EN |
                PDSS_INTR1_CFG_V5V_CFG_MASK | PDSS_INTR1_CFG_V5V_FILT_BYPASS));

    Cy_SysLib_DelayUs (10);
    
    pd->intr1     = PDSS_INTR1_V5V_CHANGED;
#endif /* PMG1_V5V_CHANGE_DETECT */

    /* Clear and disable SBU OVP detect interrupts. */
    pd->intr3_mask &= ~(PDSS_INTR3_SBU1_SBU2_OVP_CHANGED_MASK);
    pd->intr3       = PDSS_INTR3_SBU1_SBU2_OVP_CHANGED_MASK;
    pd->intr3_cfg_sbu20_ovp_hs &= ~(
            PDSS_INTR3_CFG_SBU20_OVP_HS_SBU1_FILT_EN |
            PDSS_INTR3_CFG_SBU20_OVP_HS_SBU2_FILT_EN
            );

    /* Clear and disable CC1/2 OVP change interrupt. */
    pd->intr1       = (PDSS_INTR1_CC1_OVP_CHANGED | PDSS_INTR1_CC2_OVP_CHANGED);
    pd->intr1_mask &= ~(PDSS_INTR1_MASK_CC1_OVP_CHANGED_MASK | PDSS_INTR1_MASK_CC2_OVP_CHANGED_MASK);

    pd->intr1_cfg_cc12_ovp_hs &= ~(
            PDSS_INTR1_CFG_CC12_OVP_HS_CC1_OVP_FILT_EN |
            PDSS_INTR1_CFG_CC12_OVP_HS_CC2_OVP_FILT_EN
            );
#endif /* defined (CY_DEVICE_CCG6) */

    /* Power down the block. */
    pd->cc_ctrl_0 |= PDSS_CC_CTRL_0_PWR_DISABLE;

    /* Disable PUMP */
    Cy_USBPD_Pump_Disable (context, 0);

    /* Turn off references. */
    pd->dpslp_ref_ctrl |= PDSS_DPSLP_REF_CTRL_PD_DPSLP;

#elif defined(CY_IP_M0S8USBPD)

    /* Clear and disable the V5V changed detect interrupt. */
    pd->intr1       = PDSS_INTR1_V5V_CHANGED;
    pd->intr1_mask &= ~PDSS_INTR1_MASK_V5V_CHANGED_MASK;

    /* Power down the block. */
    pd->cc_ctrl_0 |= PDSS_CC_CTRL_0_PWR_DISABLE;

    /* Disable PUMP */
    pd->pump_ctrl |= (PDSS_PUMP_CTRL_PD_PUMP  | PDSS_PUMP_CTRL_BYPASS_LV);

    /* Turn off references. */
    pd->dpslp_ref_ctrl |= PDSS_DPSLP_REF_CTRL_PD_DPSLP;

    /* Turn off comparators. */
    pd->cc_ctrl_0 &= ~(PDSS_CC_CTRL_0_CMP_EN | PDSS_CC_CTRL_0_RX_EN);

#endif /* defined(CY_IP_MXUSBPD) */

    return CY_USBPD_STAT_SUCCESS;
}

/*******************************************************************************
* Function Name: Cy_USBPD_TypeC_RdEnable
****************************************************************************//**
* 
* This function enables the Rd termination without initializing the block
* completely. This is used in Type-C fault handling use cases.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_TypeC_RdEnable (
        cy_stc_usbpd_context_t *context)
{
    uint32_t temp = 0;
    PPDSS_REGS_T pd = NULL;

    if (context == NULL)
    {
        return;
    }

    pd = context->base;

    /* Power up the block. */
    pd->cc_ctrl_0 &= ~PDSS_CC_CTRL_0_PWR_DISABLE;
    pd->cc_ctrl_0 |= (PDSS_CC_CTRL_0_CMP_EN | PDSS_CC_CTRL_0_RX_EN);

    /* Enable Rd on both CC lines. */
    temp = pd->cc_ctrl_0;
    temp |= (PDSS_CC_CTRL_0_RD_CC1_EN | PDSS_CC_CTRL_0_RD_CC1_DB_DIS);
    temp |= (PDSS_CC_CTRL_0_RD_CC2_EN | PDSS_CC_CTRL_0_RD_CC2_DB_DIS);
    temp &= ~PDSS_CC_CTRL_0_DFP_EN;

    pd->cc_ctrl_0 = temp;
}

/*******************************************************************************
* Function Name: Cy_USBPD_TypeC_SnkUpdateTrim
****************************************************************************//**
* 
* This function updates the tx trim settings when in the sink role. It must be
* called whenever an Rp change on the port partner side is detected.  This is
* required to meet the CC transmitter characteristics defined by the USB-PD
* specification.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* None
*
*******************************************************************************/
#if (!(CY_PD_SOURCE_ONLY))
void Cy_USBPD_TypeC_SnkUpdateTrim (
        cy_stc_usbpd_context_t *context)
{
#if defined(CY_IP_MXUSBPD)
#if defined (CY_DEVICE_CCG6)
    PPDSS_TRIMS_REGS_T trimRegs = NULL;
    cy_stc_pd_dpm_config_t *dpmConfig = NULL;

    if (context == NULL)
    {
        return;
    }

    trimRegs = context->trimsBase;
    dpmConfig = context->dpmGetConfig();

    if (dpmConfig->curPortRole == CY_PD_PRT_ROLE_SINK)
    {
        trimRegs->trim_cc_0 &= ~PDSS_TRIM_CC_0_TX_TRIM_MASK;

        if (dpmConfig->snkCurLevel == (uint8_t)CY_PD_RD_3A)
        {
            /* Use faster slew rate when 3A Rp is in use. */
            trimRegs->trim_cc_0 |= (TRIM0_TX_TRIM_VALUE_3A << PDSS_TRIM_CC_0_TX_TRIM_POS);
        }
    }
#else
    (void)context;
#endif /* defined (CY_DEVICE_CCG6) */
#elif defined(CY_IP_M0S8USBPD)

    PPDSS_REGS_T pd;
    cy_stc_pd_dpm_config_t *dpmConfig = NULL;

    if (context == NULL)
    {
        return;
    }

    pd = context->base;
    dpmConfig = context->dpmGetConfig();

    if (dpmConfig->curPortRole == CY_PD_PRT_ROLE_SINK)
    {
        pd->s8usbpd_trim_0 &= ~PDSS_S8USBPD_TRIM_0_TX_TRIM_MASK;
        if (dpmConfig->snkCurLevel == (uint8_t)CY_PD_RD_3A)
        {
            pd->s8usbpd_trim_0 |= (TRIM0_TX_TRIM_VALUE_3A << PDSS_S8USBPD_TRIM_0_TX_TRIM_POS);
        }
    }

#endif /* defined(CY_IP_M0S8USBPD) */
}
#endif /* (!(CY_PD_SOURCE_ONLY)) */

#if defined(CY_DEVICE_CCG3)
#define SFLASH_PDSS_PORT0_TRIM1_BASE            (0x0FFFF282u)
#define SFLASH_PDSS_PORT1_TRIM1_BASE            (0x0FFFF285u)
#elif defined(CY_DEVICE_PMG1S3)
#define SFLASH_PDSS_PORT0_TRIM1_BASE            (0x0FFFF410u)
#define SFLASH_PDSS_PORT1_TRIM1_BASE            (0x0FFFF510u)
#else
#define SFLASH_PDSS_PORT0_TRIM1_BASE            (0x0FFFF536u)
#define SFLASH_PDSS_PORT1_TRIM1_BASE            (0x0FFFF542u)
#endif /* defined(CY_DEVICE_CCG3) */

/*******************************************************************************
* Function Name: Cy_USBPD_TypeC_EnableRp
****************************************************************************//**
* 
* This function configures and enables Rp termination on the specified CC line.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \param channel
* Channel index, where CC1 = 0, CC2 = 1
*
* \param rpVal
* Rp Value
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_TypeC_EnableRp (
        cy_stc_usbpd_context_t *context, 
        uint8_t channel, 
        cy_en_pd_rp_term_t rpVal)
{
#if (!CY_PD_SINK_ONLY)
    PPDSS_REGS_T pd = NULL;
    uint32_t temp = 0;
    uint8_t rpMode = 0;
    uint8_t *pdss_rp_trim_db = (uint8_t *)SFLASH_PDSS_PORT0_TRIM1_BASE;

#if defined(CY_IP_MXUSBPD)
    PPDSS_TRIMS_REGS_T trimRegs = NULL;
#endif /* defined(CY_IP_MXUSBPD) */

#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3) || defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S))
    uint8_t cc_trim = 0;
#endif /* (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3) || defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S)) */

    if (context == NULL)
    {
        return;
    }

    pd = context->base;
    rpMode = (uint8_t)rpVal;

    /* Make sure the CC pump is enabled. */
    Cy_USBPD_Pump_Enable (context, 0);

#if defined(CY_IP_MXUSBPD)
#if defined(CY_DEVICE_PMG1S3)
    if (context->port != TYPEC_PORT_0_IDX)
    {
        pdss_rp_trim_db = (uint8_t *)SFLASH_PDSS_PORT1_TRIM1_BASE;
    }
#endif /* defined(CY_DEVICE_PMG1S3) */

    trimRegs = context->trimsBase;
    cc_trim  = pdss_rp_trim_db[rpVal];

    /* Set cc trim from sflash */
    if (channel == CY_PD_CC_CHANNEL_1)
    {
        trimRegs->trim_cc_1 = cc_trim;
    }
    else
    {
        trimRegs->trim_cc_2 = cc_trim;
    }

    trimRegs->trim_cc_0 &= ~PDSS_TRIM_CC_0_TX_TRIM_MASK;
    if (rpVal == CY_PD_RP_TERM_RP_CUR_3A)
    {
        /* Actual value in HW register for 3A Rp is (CY_PD_RP_TERM_RP_CUR_3A + 1) */
        rpMode++;

        /* Use faster slew rate when 3A Rp is in use. */
        trimRegs->trim_cc_0 |= (TRIM0_TX_TRIM_VALUE_3A << PDSS_TRIM_CC_0_TX_TRIM_POS);
    }
#elif defined(CY_IP_M0S8USBPD)
    pd->s8usbpd_trim_0 &= ~PDSS_S8USBPD_TRIM_0_TX_TRIM_MASK;

    pd->s8usbpd_trim_1 &= ~PDSS_S8USBPD_TRIM_1_RP_CC1_TRIM_MASK;
    pd->s8usbpd_trim_2 &= ~PDSS_S8USBPD_TRIM_2_RP_CC2_TRIM_MASK;

    /* Set cc trim from sflash */
    pd->s8usbpd_trim_1 |= pdss_rp_trim_db[rpVal];
    pd->s8usbpd_trim_2 |= pdss_rp_trim_db[rpVal];

    if (rpVal == CY_PD_RP_TERM_RP_CUR_3A)
    {
        /* Use faster slew rate when 3A Rp is in use. */
        pd->s8usbpd_trim_0 |= (TRIM0_TX_TRIM_VALUE_3A << PDSS_S8USBPD_TRIM_0_TX_TRIM_POS);

        /* Actual value in HW register for 3A Rp is (CY_PD_RP_TERM_RP_CUR_3A + 1) */
        rpMode++;
    }
#endif /* defined(CY_IP_MXUSBPD) */

    /* Set Rp mode and enable references for source operation. */
    temp = pd->cc_ctrl_0;
    temp &= ~PDSS_CC_CTRL_0_RP_MODE_MASK;
    temp |= ((uint32_t)rpMode << PDSS_CC_CTRL_0_RP_MODE_POS) | PDSS_CC_CTRL_0_DFP_EN;

    if (channel == CY_PD_CC_CHANNEL_1)
    {
        temp |= PDSS_CC_CTRL_0_RP_CC1_EN;
    }
    else
    {
        temp |= PDSS_CC_CTRL_0_RP_CC2_EN;
    }

    pd->cc_ctrl_0 = temp;

#else
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(channel);
    CY_UNUSED_PARAMETER(rpVal);
#endif /* (!CY_PD_SINK_ONLY) */
}

/*******************************************************************************
* Function Name: Cy_USBPD_TypeC_DisableRp
****************************************************************************//**
* 
* This function disables Rp termination on the specified CC line.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \param channel
* Channel index, where CC1 = 0, CC2 = 1
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_TypeC_DisableRp (
        cy_stc_usbpd_context_t *context, 
        uint8_t channel)
{
#if (!CY_PD_SINK_ONLY)
    PPDSS_REGS_T pd = NULL;
    if (context == NULL)
    {
        return;
    }

    pd = context->base;
    if (channel == CY_PD_CC_CHANNEL_1)
    {
        pd->cc_ctrl_0 &= ~PDSS_CC_CTRL_0_RP_CC1_EN;
    }
    else
    {
        pd->cc_ctrl_0 &= ~PDSS_CC_CTRL_0_RP_CC2_EN;
    }
#else
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(channel);
#endif /* (!CY_PD_SINK_ONLY) */
}

/*******************************************************************************
* Function Name: Cy_USBPD_TypeC_EnDpSlpRp
****************************************************************************//**
* 
* This function enables a resistive Rp termination (not accurate to match any
* Type-C current levels) which can be used to save power while there is no
* Type-C connection and the PMG1 device is in sleep.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_TypeC_EnDpSlpRp(
        cy_stc_usbpd_context_t *context)
{
#if (!CY_PD_SINK_ONLY)
    if (context != NULL)
    {
        context->base->cc_ctrl_1 |= PDSS_CC_CTRL_1_DS_ATTACH_DET_EN;
    }
#else
    CY_UNUSED_PARAMETER(context);
#endif /* (!CY_PD_SINK_ONLY) */
}

/*******************************************************************************
* Function Name: Cy_USBPD_TypeC_DisDpSlpRp
****************************************************************************//**
* 
* This function disables the resistive Rp termination so that the accurate
* current source based Rp termination can be applied.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_TypeC_DisDpSlpRp(
        cy_stc_usbpd_context_t *context)
{
#if (!CY_PD_SINK_ONLY)
    if (context != NULL)
    {
        context->base->cc_ctrl_1 &= ~PDSS_CC_CTRL_1_DS_ATTACH_DET_EN;
    }
#else
    CY_UNUSED_PARAMETER(context);
#endif /* (!CY_PD_SINK_ONLY) */
}

/*******************************************************************************
* Function Name: Cy_USBPD_TypeC_ChangeRp
****************************************************************************//**
*
* This function changes Rp. If port is in a connected state, Rp will be changed
* only on the active channel. If port is not connected then Rp gets updated on
* both CC channels.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \param rp
* Desired Rp value.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_TypeC_ChangeRp (cy_stc_usbpd_context_t *context, cy_en_pd_rp_term_t rp)
{
#if (!CY_PD_SINK_ONLY)
    cy_stc_pd_dpm_config_t *dpmConfig;
    
    if (context != NULL)
    {
        dpmConfig = context->dpmGetConfig();

        if(dpmConfig->curPortRole == CY_PD_PRT_ROLE_SOURCE)
        {
            dpmConfig->srcCurLevelLive = (uint8_t)rp;

            if (dpmConfig->connect == false)
            {
                Cy_USBPD_TypeC_EnableRp(context, CY_PD_CC_CHANNEL_1, rp);
                Cy_USBPD_TypeC_EnableRp(context, CY_PD_CC_CHANNEL_2, rp);
            }
            else
            {
                Cy_USBPD_TypeC_EnableRp(context, dpmConfig->polarity, rp);
            }

            /* Adding this delay for cc voltage to settle */
            Cy_SysLib_DelayUs(50);
        }
    }
#else
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(rp);
#endif /* (!CY_PD_SINK_ONLY) */
}

/*******************************************************************************
* Function Name: Cy_USBPD_TypeC_EnableDeadBatRd
****************************************************************************//**
* 
* This function enables the dead battery Rd termination.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_TypeC_EnableDeadBatRd (
        cy_stc_usbpd_context_t *context)
{
    PPDSS_REGS_T pd;
    uint32_t temp = 0;

    if (context != NULL)
    {
        pd = context->base;
        temp = pd->cc_ctrl_0;

        /* Re-enable dead battery Rd */
        temp &= ~(PDSS_CC_CTRL_0_RD_CC1_DB_DIS | PDSS_CC_CTRL_0_RD_CC2_DB_DIS);

        /* Remove trimmed Rd */
        temp &= ~(PDSS_CC_CTRL_0_RD_CC1_EN | PDSS_CC_CTRL_0_RD_CC2_EN);

        pd->cc_ctrl_0 = temp;
    }
}

/*******************************************************************************
* Function Name: Cy_USBPD_TypeC_EnableRd
****************************************************************************//**
* 
* This function enables the Rd termination on the specified CC line.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \param channel
* Channel index, where CC1 = 0, CC2 = 1
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_TypeC_EnableRd (
        cy_stc_usbpd_context_t *context,
        uint8_t channel)
{
    PPDSS_REGS_T pd = NULL;
    uint32_t temp = 0;

    if (context == NULL)
    {
        return;
    }

    pd = context->base;

    /* Disable PUMP */
    Cy_USBPD_Pump_Disable (context, 0);

    temp = pd->cc_ctrl_0;
    if (channel == CY_PD_CC_CHANNEL_1)
    {
        temp |= (PDSS_CC_CTRL_0_RD_CC1_EN | PDSS_CC_CTRL_0_RD_CC1_DB_DIS);
    }
    else
    {
        temp |= (PDSS_CC_CTRL_0_RD_CC2_EN | PDSS_CC_CTRL_0_RD_CC2_DB_DIS);
    }

    temp &= ~PDSS_CC_CTRL_0_DFP_EN;
    pd->cc_ctrl_0 = temp;
}

/*******************************************************************************
* Function Name: Cy_USBPD_TypeC_DisableRd
****************************************************************************//**
* 
* This function disables the Rd termination on the specified CC line.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \param channel
* Channel index, where CC1 = 0, CC2 = 1
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_TypeC_DisableRd (
        cy_stc_usbpd_context_t *context, 
        uint8_t channel)
{
    PPDSS_REGS_T pd = NULL;
    uint32_t regVal = 0;

    if (context == NULL)
    {
        return;
    }

    pd = context->base;
    regVal = pd->cc_ctrl_0;

    if (channel == CY_PD_CC_CHANNEL_1)
    {
        regVal &= ~PDSS_CC_CTRL_0_RD_CC1_EN;
        regVal |= PDSS_CC_CTRL_0_RD_CC1_DB_DIS;
    }
    else
    {
        regVal &= ~PDSS_CC_CTRL_0_RD_CC2_EN;
        regVal |= PDSS_CC_CTRL_0_RD_CC2_DB_DIS;
    }

    pd->cc_ctrl_0 = regVal;
}

/*******************************************************************************
* Function Name: Cy_USBPD_TypeC_GetCCStatus
****************************************************************************//**
* 
* This function returns current status of both CC lines. The function
* identifies the current termination (Rp or Rd) applied by the PMG1 device and
* interprets the voltage on the CC line based on the current use case.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* Returns the value of the CC state.
*
*******************************************************************************/
cy_pd_cc_state_t Cy_USBPD_TypeC_GetCCStatus (
        cy_stc_usbpd_context_t *context)
{
    cy_stc_pd_dpm_config_t *dpmConfig = NULL;
    PPDSS_REGS_T pd = NULL;
    uint8_t polarity = 0;
    cy_pd_cc_state_t newState;
    uint8_t i = 0;

    pd = context->base;
    dpmConfig = context->dpmGetConfig();
    polarity = dpmConfig->polarity;

    /* If the CC TX/RX is busy, retain previously detected CC status. */
    newState = dpmConfig->ccOldStatus;
    if (
            (dpmConfig->attach == true) && 
            ((pd->status & (PDSS_STATUS_TX_BUSY | PDSS_STATUS_CC_DATA_VALID)) != 0U)
       )
    {
        return newState;
    }

#if (!CY_PD_SINK_ONLY)
    if (dpmConfig->curPortRole == CY_PD_PRT_ROLE_SOURCE)
    {
#if PSVP_FPGA_ENABLE
        /* On PSVP, do not scan the inactive CC line when there is a Type-C connection. */
        if (dpmConfig->attach == false)
        {
            newState.cc[1u - polarity] = Cy_USBPD_TypeC_GetRpRdStatus(context, 1u - polarity, 0);
        }
        newState.cc[polarity]     = Cy_USBPD_TypeC_GetRpRdStatus(context, polarity, 0);
#else
        /* Scan both CC lines: the active CC line should be scanned last. */
        newState.cc[1u - polarity] = Cy_USBPD_TypeC_GetRpRdStatus(context, 1u - polarity, 0);
        newState.cc[polarity]     = Cy_USBPD_TypeC_GetRpRdStatus(context, polarity, 0);

#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))
        if (context->ccOvpPending != 0U)
        {
            if (newState.cc[polarity] == (uint8_t)CY_PD_RP_OPEN)
            {
                /* Keep returning RP_RD status while OVP is active. */
                newState.cc[polarity] = (uint8_t)CY_PD_RP_RD;
            }
            else
            {
                /* Re-enable the Rp termination and mark OVP not active. */
                Cy_USBPD_TypeC_EnableRp(context, polarity, (cy_en_pd_rp_term_t)dpmConfig->srcCurLevelLive);
                context->ccOvpPending = 0U;
            }
        }
#endif /* (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)) */
#endif /* PSVP_FPGA_ENABLE */
    }
    else
#endif /* (!CY_PD_SINK_ONLY) */
    {
        if (dpmConfig->attach == true)
        {
            if (newState.cc[polarity] > (uint8_t)CY_PD_RD_USB)
            {
                newState.cc[polarity] = Cy_USBPD_TypeC_GetRpRdStatus(context, polarity, 1);
            }

            /* If CC line voltage is below the 1.5 A Rp threshold, do another check for presence of Rp. */
            if (newState.cc[polarity] <= (uint8_t)CY_PD_RD_USB)
            {
                newState.cc[polarity] = Cy_USBPD_TypeC_GetRpRdStatus(context, polarity, 0);
            }

            /* Only the active CC line needs to be scanned. */
            newState.cc[dpmConfig->revPol] = (uint8_t)CY_PD_RD_RA;
        }
        else
        {
            for (i = 0U; i < 2U; i++)
            {
                /* Scan CC[i] with threshold vRa and vRdUsb. */
                newState.cc[i] = Cy_USBPD_TypeC_GetRpRdStatus(context, i, 0);
                if (newState.cc[i] != (uint8_t)CY_PD_RD_RA)
                {
                    /* Scan CC[i] again with vRdusb and vRd1.5A to determine correct Rp value. */
                    newState.cc[i] = Cy_USBPD_TypeC_GetRpRdStatus(context, i, 1);
                }
            }
        }
    }

    return newState;
}

/*******************************************************************************
* Function Name: Cy_USBPD_TypeC_SetPolarity
****************************************************************************//**
* 
* This function sets the CC polarity for the receiver circuit.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \param polarity
* Type-C connection orientation: 0 when CC1 active and 1 when CC2 active.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_TypeC_SetPolarity (
        cy_stc_usbpd_context_t *context,
        uint8_t polarity)
{
    if (context != NULL)
    {
        if (polarity == 0U)
        {
            context->base->cc_ctrl_0 &= ~PDSS_CC_CTRL_0_CC_1V2;
        }
        else
        {
            context->base->cc_ctrl_0 |= PDSS_CC_CTRL_0_CC_1V2;
        }
    }
}

/*******************************************************************************
* Function Name: Cy_USBPD_TypeC_CfgAutoToggle
****************************************************************************//**
* 
* Enable/disable automatic hardware toggle operation as part of deep sleep cycle.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \param enable
* Whether automatic toggle is to be enabled.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_TypeC_CfgAutoToggle (
        cy_stc_usbpd_context_t *context, 
        bool enable)
{
#if CY_PD_HW_DRP_TOGGLE_ENABLE
    if (context != NULL)
    {
        /* Abort auto toggle if it is in progress. */
        Cy_USBPD_TypeC_AbortAutoToggle (context);
        context->autoToggleEn = enable;
    }
#else
    (void)context;
    (void)enable;
#endif /* CY_PD_HW_DRP_TOGGLE_ENABLE */
}

/*******************************************************************************
* Function Name: Cy_USBPD_TypeC_AbortAutoToggle
****************************************************************************//**
* 
* This function aborts any ongoing automatic DRP toggle operation.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_TypeC_AbortAutoToggle (
        cy_stc_usbpd_context_t *context)
{
    if (context != NULL)
    {
#if CY_PD_HW_DRP_TOGGLE_ENABLE
        if (context->autoToggleAct)
        {
            PPDSS_REGS_T pd = context->base;
            /* Disable the RP-RD toggle and the attach interrupt. */
            pd->rp_rd_cfg1 &= ~(PDSS_RP_RD_CFG1_HW_RP_RD_AUTO_TOGGLE | PDSS_RP_RD_CFG1_START_TOGGLE);
            pd->intr1_mask &= ~PDSS_INTR1_DRP_ATTACHED_DETECTED;

            context->autoToggleAct = false;
            context->typecStartPending = false;
        }
#endif /* CY_PD_HW_DRP_TOGGLE_ENABLE */
    }
}

/*******************************************************************************
* Function Name: Cy_USBPD_TypeC_IsAutoToggleActive
****************************************************************************//**
* 
* This function checks if automatic DRP toggle operation is active.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* Returns true if DRP toggle operation is active, otherwise false.
*
*******************************************************************************/
bool Cy_USBPD_TypeC_IsAutoToggleActive (
        cy_stc_usbpd_context_t *context)
{
    if (context == NULL)
    {
        return false;
    }
#if CY_PD_HW_DRP_TOGGLE_ENABLE
    /* Don't run through Type-C state machine if auto toggle is active or has just been stopped. */
    return (((bool)context->autoToggleAct) || ((bool)context->typecStartPending));
#else
    return false;
#endif /* CY_PD_HW_DRP_TOGGLE_ENABLE */
}

/*******************************************************************************
* Function Name: Cy_USBPD_TypeC_SMRestart
****************************************************************************//**
* 
* Restart Type-C state machine once auto toggle operation is complete. This is
* normally required when the port has just exited the auto DRP toggle stage,
* and the state machine needs to handle further operations.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_TypeC_SMRestart (
        cy_stc_usbpd_context_t *context)
{
#if ((SYS_DEEPSLEEP_ENABLE) && (CY_PD_HW_DRP_TOGGLE_ENABLE))
    if (context)
    {
        if (context->typecStartPending)
        {
            Cy_USBPD_Resume(context);
        }
    }
#else
    (void)context;
#endif /* ((SYS_DEEPSLEEP_ENABLE) && (CY_PD_HW_DRP_TOGGLE_ENABLE)) */
}

/*******************************************************************************
* Function Name: Cy_USBPD_SetReference
****************************************************************************//**
* 
* This function facilitates to set deep sleep/bandgap reference. This should be
* called for setting deep sleep reference just before entry into deep sleep
* state. Function should be called with band gap reference just after exit from
* deep sleep state.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \param flag
* If true signifies deep sleep reference, otherwise band gap reference.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_SetReference (
        cy_stc_usbpd_context_t *context,
        bool flag)
{
    if (context != NULL)
    {
#if (defined(CY_DEVICE_CCG3PA))
        PPDSS_REGS_T pd = context->base;
        if (flag == true)
        {
            /* Deep Sleep Reference requested */
            /*
             * Configure Refgen block to use Deepsleep Reference input instead of Bandgap
             * reference which is not available in deep sleep.
             */
            pd->refgen_0_ctrl &= ~(PDSS_REFGEN_0_CTRL_REFGEN_VREFIN_SEL |
                    PDSS_REFGEN_0_CTRL_REFGEN_VREFIN_MON_SEL);

            /* Switch to using refgen_2_ctrl SEL7 for EA shunt regulator reference. */
            pd->amux_ctrl |= AMUX_CTRL_EA_TOP_REFGEN_SEL7_EN;
        }
        else
        {
            /* Bandgap Reference requested */
            /* Switch to using bandgap for EA shunt regulator reference. */
            pd->amux_ctrl &= ~AMUX_CTRL_EA_TOP_REFGEN_SEL7_EN;

            /*
             * Configure Refgen block to use Bandgap Reference input instead of Deep sleep
             * reference.
             */
            pd->refgen_0_ctrl |= (PDSS_REFGEN_0_CTRL_REFGEN_VREFIN_SEL |
                    PDSS_REFGEN_0_CTRL_REFGEN_VREFIN_MON_SEL);
        }
#else
        (void)context;
        (void)flag;
#endif /* (defined(CY_DEVICE_CCG3PA))  */
    }
}

/*******************************************************************************
* Function Name: Cy_USBPD_DisableVsysReg
****************************************************************************//**
* 
* This function disables the internal VBus regulator.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_DisableVsysReg (
        cy_stc_usbpd_context_t *context)
{
    if (context != NULL)
    {
        /*
         * If system has external VDDD source, internal VBUS regulator shall
         * be turned off.
         */
#if (defined(CY_DEVICE_CCG3PA))
        context->base->vreg_vsys_ctrl &= ~(PDSS_VREG_VSYS_CTRL_VREG_EN);
#elif (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))
        context->base->vreg_vsys_ctrl &= ~(PDSS_VREG_VSYS_CTRL_VREG20_1_EN);
#elif (defined(CY_DEVICE_CCG3))
        context->base->vreg_vsys_ctrl &= ~(PDSS_VREG_VSYS_CTRL_VREG20_EN);
#endif /* (defined(CY_DEVICE_CCG3PA)) */
    }
}

/*******************************************************************************
* Function Name: Cy_USBPD_IsVsysUp
****************************************************************************//**
* 
* This function enables the VSYS detect comparator and checks whether VSYS
* supply is up. If VSYS supply is up, the interrupt handler will switch the
* device from VBUS powered state to VSYS powered state. This function should
* be called periodically from the app_task().
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return bool
* Returns true if Vsys is up.
*
*******************************************************************************/
bool Cy_USBPD_IsVsysUp (
        cy_stc_usbpd_context_t *context)
{
    uint32_t state = 0;
    bool retVal = false;
    PPDSS_REGS_T pd = NULL;

    if ((context == NULL) || (context->port != TYPEC_PORT_0_IDX))
    {
        return retVal;
    }

    pd = context->base;

#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))
    if ((pd->vreg_vsys_ctrl & PDSS_VREG_VSYS_CTRL_ENABLE_VDDD_SWITCH) == 0U)
    {
        /* Enable the VSYS detect comparator. */
        pd->comp_ctrl[CY_USBPD_VBUS_COMP_ID_VSYS_DET] |= PDSS_COMP_CTRL_COMP_ISO_N;
        pd->comp_ctrl[CY_USBPD_VBUS_COMP_ID_VSYS_DET] &= ~PDSS_COMP_CTRL_COMP_PD;

        /* Wait for a maximum of 200 us to allow VSYS detection to be completed. */
        Cy_SysLib_DelayUs (200);

        /* Disable interrupts while checking for status to prevent race conditions. */
        state = Cy_SysLib_EnterCriticalSection();
        if ((pd->vreg_vsys_ctrl & PDSS_VREG_VSYS_CTRL_ENABLE_VDDD_SWITCH) == 0U)
        {
            /* VSYS still not detected. Turn off the comparator again. */
            pd->comp_ctrl[CY_USBPD_VBUS_COMP_ID_VSYS_DET] |= PDSS_COMP_CTRL_COMP_PD;
        }
        else
        {
            retVal = true;
        }
        Cy_SysLib_ExitCriticalSection(state);
    }
#elif defined(CY_DEVICE_CCG3)
    if ((pd->ncell_status & PDSS_NCELL_STATUS_VSYS_STATUS) != 0U)
    {
        uint32_t regVal;
        uint16_t vsys_level;
        uint8_t  adc_reading;
        uint16_t value;

        state = Cy_SysLib_EnterCriticalSection();

        regVal = pd->uvov_ctrl;

        /* Disconnect the UV/OV resistor divider from ADFT so that VSYS can be measured. */
        pd->uvov_ctrl &= ~(PDSS_UVOV_CTRL_UVOV_ADFT_EN | PDSS_UVOV_CTRL_UVOV_ADFT_CTRL_MASK);
        Cy_SysLib_DelayUs (100);

        /* Enable VSYS/2 connection to ADFT. */
        pd->vsys_ctrl |= (PDSS_VSYS_CTRL_ADFT_EN | (1UL << PDSS_VSYS_CTRL_ADFT_SEL_POS));
        Cy_SysLib_DelayUs (100);

        /* Measure VSYS voltage using ADC. If VSYS is above 3.2 V, we can switch
           to VSYS power for the device.
           */
        value = Cy_USBPD_Adc_Calibrate (context, CY_USBPD_ADC_ID_0);
        adc_reading = Cy_USBPD_Adc_Sample (context, CY_USBPD_ADC_ID_0, CY_USBPD_ADC_INPUT_AMUX_A);
        vsys_level  = ((uint16_t)adc_reading * context->adcVdddMv[CY_USBPD_ADC_ID_0] * 2U) >> 8u; 

        /* Restore the original ADFT settings. */
        pd->vsys_ctrl &= ~(PDSS_VSYS_CTRL_ADFT_EN | PDSS_VSYS_CTRL_ADFT_SEL_MASK);
        Cy_SysLib_DelayUs (10);
        pd->uvov_ctrl = regVal;

        Cy_SysLib_ExitCriticalSection(state);

        if (vsys_level > 3200u)
        {
            retVal = true;
        }
        CY_UNUSED_PARAMETER(value);
    }
#else
    CY_UNUSED_PARAMETER(state);
    CY_UNUSED_PARAMETER(pd);
#endif /* defined(CY_DEVICE_CCG3) */

    return retVal;
}

/*******************************************************************************
* Function Name: Cy_USBPD_VsysCompStatus
****************************************************************************//**
* 
* Get the VSYS Comparator Status
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return bool
* Returns comparator status.
*
*******************************************************************************/
bool Cy_USBPD_VsysCompStatus (
        cy_stc_usbpd_context_t *context)
{
    if (context != NULL)
    {
#if (defined(CY_DEVICE_CCG6) || defined (CY_DEVICE_PMG1S3))
        /* Return the status of the vsys comparator. */
        if ((context->base->intr7_status & (CLK_LF_FILT_NUM << PDSS_INTR7_STATUS_FILT_8_POS)) != 0U)
        {
            return true;
        }
#elif (defined(CY_DEVICE_CCG3))
        if ((context->base->ncell_status & PDSS_NCELL_STATUS_VSYS_STATUS) != 0U)
        {
            return true;
        }
#endif
    }

    return false;
}

/*******************************************************************************
* Function Name: Cy_USBPD_SwitchVsysToVbus
****************************************************************************//**
* 
* This function switches over from VSYS VDDD switch to VBUS regulator
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_SwitchVsysToVbus (
        cy_stc_usbpd_context_t *context)
{
#if defined(CY_DEVICE_CCG6)
    PPDSS_REGS_T pd = NULL;

    if (context != NULL)
    {
        pd = context->base;

        /* Turn the VBus LDO ON and then turn off the VSYS-VDDD switch. */
        pd->vreg_vsys_ctrl |= PDSS_VREG_VSYS_CTRL_VREG20_1_EN;
        Cy_SysLib_DelayUs (120);
        pd->vreg_vsys_ctrl &= ~PDSS_VREG_VSYS_CTRL_ENABLE_VDDD_SWITCH;
    }
#elif defined(CY_DEVICE_CCG3)
    PPDSS_REGS_T pd = NULL;

    if (context != NULL)
    {
        pd = context->base;

        /* Turn the VBus LDO ON and then turn off the VSYS-VDDD switch. */
        pd->vreg_vsys_ctrl |= PDSS_VREG_VSYS_CTRL_VREG20_EN;
        Cy_SysLib_DelayUs (120);
        pd->vreg_vsys_ctrl &= ~PDSS_VREG_VSYS_CTRL_ENABLE_VDDD_SWITCH;
    }
#else
    (void)context;
#endif /* defined(CY_DEVICE_CCG6 */
}

/*******************************************************************************
* Function Name: Cy_USBPD_SwitchVbusToVsys
****************************************************************************//**
* 
* This function switches over from VBus Regulator to VSYS VDDD switch.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_SwitchVbusToVsys (
        cy_stc_usbpd_context_t *context)
{
#if defined(CY_DEVICE_CCG6)
    PPDSS_REGS_T pd = NULL;

    if (context != NULL)
    {
        pd = context->base;

        pd->vreg_vsys_ctrl |= PDSS_VREG_VSYS_CTRL_ENABLE_VDDD_SWITCH;
        Cy_SysLib_DelayUs (100);
        pd->vreg_vsys_ctrl &= ~PDSS_VREG_VSYS_CTRL_VREG20_1_EN;
    }
#elif defined(CY_DEVICE_CCG3)
    PPDSS_REGS_T pd = NULL;

    if (context != NULL)
    {
        pd = context->base;

        pd->vreg_vsys_ctrl |= PDSS_VREG_VSYS_CTRL_ENABLE_VDDD_SWITCH;
        Cy_SysLib_DelayUs (100);
        pd->vreg_vsys_ctrl &= ~PDSS_VREG_VSYS_CTRL_VREG20_EN;
    }
#else
    CY_UNUSED_PARAMETER(context);
#endif /* defined(CY_DEVICE_CCG6) */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Init
****************************************************************************//**
* 
* This function initializes the PDSS IP with necessary clock and interrupt
* handlers.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \param port
* Port index
*
* \param ptrPdBaseReg
* Base register address for PD block
* 
* \param ptrPdTrimsBaseReg
* Base register address for TRIMs
*
* \param ptrUsbpdConfig
* USBPD block configuration structure pointer
*
* \param dpmGetConfigCbk
* DPM status function callback 
*
* \return
* CY_USBPD_STAT_SUCCESS if operation is successful. 
* CY_USBPD_STAT_BAD_PARAM if the context pointer is invalid.
* CY_USBPD_STAT_FAILURE if the operation failed.
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Init(
        cy_stc_usbpd_context_t *context,
        uint8_t port,
        void *ptrPdBaseReg,
        void *ptrPdTrimsBaseReg,
        cy_stc_usbpd_config_t *ptrUsbpdConfig,
        cy_cb_pd_dpm_get_config_t dpmGetConfigCbk)
{
    cy_en_usbpd_status_t status;
    PPDSS_REGS_T pd = NULL;
#if defined(CY_DEVICE_CCG3)
    uint32_t tmp;
#endif /* defined(CY_DEVICE_CCG3) */
#if defined(CY_DEVICE_PMG1S3)
    uint8_t vref = 0u;
#endif /* defined(CY_DEVICE_PMG1S3) */

    /* Validate the parameters. */
    if (
            (context == NULL) || (ptrPdBaseReg == NULL) ||
#if (!defined(CY_DEVICE_CCG3))
            /* PMG1S2 does not have a separate trim register space. */
            (ptrPdTrimsBaseReg == NULL) ||
#endif /* (!defined(CY_DEVICE_CCG3)) */
            (ptrUsbpdConfig == NULL) ||
            (dpmGetConfigCbk == NULL)
       )
    {
        return CY_USBPD_STAT_BAD_PARAM;
    }

    /* Initialize the context members. */
    context->port = port;
    context->base = ptrPdBaseReg;
    context->trimsBase = ptrPdTrimsBaseReg;
    context->usbpdConfig = ptrUsbpdConfig;
    context->dpmGetConfig = dpmGetConfigCbk;

    pd = context->base;

    /* Enable the PD block. */
    pd->ctrl |= PDSS_CTRL_IP_ENABLED;

    /* Power up the block. */
    pd->cc_ctrl_0 &= ~PDSS_CC_CTRL_0_PWR_DISABLE;

    /* Turn off PHY deepsleep. References require 100us to stabilize. */
    pd->dpslp_ref_ctrl &= ~PDSS_DPSLP_REF_CTRL_PD_DPSLP;

    /*
     * Enable deep-sleep current reference outputs.
     */
    pd->dpslp_ref_ctrl |= PDSS_DPSLP_REF_CTRL_IGEN_EN;
    Cy_SysLib_DelayUs(100);

#if defined(CY_DEVICE_CCG3)
    /* Settings as recommended by design */
    pd->vreg_ctrl |= PDSS_VREG_CTRL_VREG_ISO_N;
    pd->vsys_ctrl |= PDSS_VSYS_CTRL_VSYS_ISO_N;

    /* Set trim settings. */
    tmp = pd->s8usbpd_trim_6 & ~PDSS_S8USBPD_TRIM_6_V1P575_TRIM_MASK;
    tmp |= SILICON_TRIM6_V1P575_TRIM_VALUE;
    pd->s8usbpd_trim_6 = tmp;

    tmp = pd->s8usbpd_trim_3 & ~PDSS_S8USBPD_TRIM_3_V0P55_TRIM_MASK;
    tmp |= SILICON_TRIM3_V0P55_TRIM_VALUE;
    pd->s8usbpd_trim_3 = tmp;

    /*
     * Clearing of TX_TRIM field is enough for Rp = 1.5A or Default termination
     * at initialization time. Later TX_TRIM could be updated for Rp = 3A
     * termination.
     */
    pd->s8usbpd_trim_0 &= (~(PDSS_S8USBPD_TRIM_0_TX_TRIM_MASK));

    /*
     * The UVP comparator shall be reserved for VBUS internal divider. We expect to
     * use a 1/11 fraction of VBus for voltage measurements. Using 0.74 V as the voltage
     * reference, this results in a VBus equivalent voltage of 8.14 V.
     *
     * Rounding to the nearest voltage setting from RISH-183, we need to use UV_IN value
     * of 22. The same value is being loaded into OV_IN until it is corrected as part
     * of OVP configuration.
     */
    tmp = pd->uvov_ctrl & ~(PDSS_UVOV_CTRL_UV_IN_MASK | PDSS_UVOV_CTRL_OV_IN_MASK |
            PDSS_UVOV_CTRL_UVOV_ADFT_CTRL_MASK | PDSS_UVOV_CTRL_PD_UVOV);
    tmp |= ((22UL << PDSS_UVOV_CTRL_UV_IN_POS) | (22UL << PDSS_UVOV_CTRL_OV_IN_POS) |
            PDSS_UVOV_CTRL_UVOV_ISO_N | PDSS_UVOV_CTRL_UVOV_ADFT_EN |
            (1UL << PDSS_UVOV_CTRL_UVOV_ADFT_CTRL_POS));
    pd->uvov_ctrl = tmp;

    /*
     * CDT 264973: The clock divider to drive the charge pump should be the lowest
     * (fastest clock) so that the gate drivers are turned on quickly. This setting
     * corresponds to the timing specs for the device.
     */
    pd->ngdo_ctrl_0 &= ~(PDSS_NGDO_CTRL_0_NGDO_VBUS_C_CLKSEL_LV_MASK | PDSS_NGDO_CTRL_0_NGDO_VBUS_P_CLKSEL_LV_MASK);

    /*
     * Bypass the synchronizer for fault detection in CLK_LF domain, so that
     * spacing takes effect right after fault detection.
     */
    pd->ngdo_ctrl_p |= PDSS_NGDO_CTRL_P_BYPASS_2DFF;
    pd->ngdo_ctrl_c |= PDSS_NGDO_CTRL_C_BYPASS_2DFF;
#endif /* defined(CY_DEVICE_CCG3) */

#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))
    /*
     * The Deep Sleep voltage reference is used as input to the refgen block at all times.
     */
    pd->refgen_0_ctrl &= ~PDSS_REFGEN_0_CTRL_REFGEN_PD;
    pd->refgen_1_ctrl  = 0x1C143616; /* SEL0=0x16(420 mV), SEL1=0x36(740 mV), SEL2=0x14(400 mV), SEL3=0x1C(480 mV) */
    pd->refgen_2_ctrl  = 0x003D6B5F; /* SEL4=0x5F(1.15 V), SEL5=0x6B(1.27 V), SEL6=0x3D(810 mV), SEL7=0x00(200 mV) */
    pd->refgen_3_ctrl  = 0x00003D14; /* SEL8=0x14(400 mV), SEL9=0x3D(810 mV), SEL10=0x00(200 mV) */
    pd->refgen_4_ctrl  = 0x00000900; /* SEL11=0x00(450 mV), SEL12=0x00(650 mV), SEL13=0x04(2.0 V), SEL14=0x04(1.12 V) */

#if defined(CY_DEVICE_PMG1S3)
    /* Set the threshold for SCP-RCP block to work */
    vref = (uint8_t)((*((volatile uint8_t *)(SFLASH_SCP_RCP_VREF_TRIM_HOT + (uint16_t)(port * 0x100u))) + 
                      *((volatile uint8_t *)(SFLASH_SCP_RCP_VREF_TRIM_COLD + (uint16_t)(port * 0x100u)))) / 2U);
//    vref = *((volatile uint8_t *)SFLASH_SCP_RCP_VREF_TRIM_ROOM);  /* In case room trim is available */
    pd->refgen_2_ctrl  = Cy_USBPD_MmioRegUpdateField(pd->refgen_2_ctrl, (uint32_t)vref,
                PDSS_REFGEN_2_CTRL_SEL5_MASK, (uint8_t)PDSS_REFGEN_2_CTRL_SEL5_POS);
#endif /* defined(CY_DEVICE_PMG1S3) */

    /* Give some delay for references to settle. */
    Cy_SysLib_DelayUs (50);

    /* Enable the VSYS_DET comparator. Corresponding LF filter also needs to be enabled.
       Look for both positive and negative edge interrupts.
       */
    /* Set the current reference used for VConn Over-Current detection. */
    pd->reg_control = ((pd->reg_control & ~PDSS_REG_CONTROL_T_REG_CONTROL_MASK) |
            (2u << PDSS_REG_CONTROL_T_REG_CONTROL_POS));

    if (*((volatile uint8_t *)SFLASH_VCONN_TRIM_ENABLE_ADDR) == 0u)
    {
#if defined(CY_DEVICE_CCG6)
        /* If optimal VConn OCP trims were not available in SFLASH, override with default value. */
        PPDSS_TRIMS_REGS_T trimRegs = (PPDSS_TRIMS_REGS_T)context->trimsBase;
        trimRegs->trim_vconn20_2 = 0x08u;
#endif /* defined(CY_DEVICE_CCG6) */
    }

    if (context->port == TYPEC_PORT_0_IDX)
    {
        /* Enable interrupt on either positive or negative edge from the VSYS_DET comparator. */
        pd->comp_ctrl[CY_USBPD_VBUS_COMP_ID_VSYS_DET] |= PDSS_COMP_CTRL_COMP_ISO_N;
        pd->comp_ctrl[CY_USBPD_VBUS_COMP_ID_VSYS_DET] &= ~PDSS_COMP_CTRL_COMP_PD;
#if defined(CY_DEVICE_CCG6)
        pd->intr7_filter_cfg[1] = (PDSS_INTR7_FILTER_CFG_CLK_LF_FILT_BYPASS |
                PDSS_INTR7_FILTER_CFG_CLK_LF_FILT_RESET |
                ((uint32_t)CY_USBPD_VBUS_FILTER_CFG_POS_EN_NEG_EN << PDSS_INTR7_FILTER_CFG_FILT_CFG_POS));
#endif /* defined(CY_DEVICE_CCG6) */
#if defined(CY_DEVICE_PMG1S3)
        pd->intr7_filter_cfg[2] = (PDSS_INTR7_FILTER_CFG_CLK_LF_FILT_BYPASS |
                PDSS_INTR7_FILTER_CFG_CLK_LF_FILT_RESET |
                ((uint32_t)CY_USBPD_VBUS_FILTER_CFG_POS_EN_NEG_EN << PDSS_INTR7_FILTER_CFG_FILT_CFG_POS));
#endif /* defined(CY_DEVICE_PMG1S3) */

        /* Need to disable the bypass on VSYS good comparator. */
        pd->reg_control |= PDSS_REG_CONTROL_BYPASS_VSYS_GOOD_ACC;

        /* Wait for some time and check VSYS status. */
        Cy_SysLib_Delay (1);
        if ((pd->intr7_status & (CLK_LF_FILT_NUM << PDSS_INTR7_STATUS_FILT_8_POS)) == 0U)
        {
            /* Notify application layer about absence of VSYS supply. */
            if (context->supplyChangeCbk != NULL)
            {
                context->supplyChangeCbk (context, CY_USBPD_SUPPLY_VSYS, false);
            }
            Cy_SysLib_Delay (1);

            /* VSYS is not present, disable VSYS switch. */
            pd->vreg_vsys_ctrl &= ~PDSS_VREG_VSYS_CTRL_ENABLE_VDDD_SWITCH;

            /* Leave the VSYS detect comparator disabled. We will enable this periodically to check for VSYS up. */
            pd->comp_ctrl[CY_USBPD_VBUS_COMP_ID_VSYS_DET] |= PDSS_COMP_CTRL_COMP_PD;
        }
        else
        {
            /* Notify application about presence of VSYS supply. */
            if (context->supplyChangeCbk != NULL)
            {
                context->supplyChangeCbk (context, CY_USBPD_SUPPLY_VSYS, true);
            }
        }

        /* Enable interrupt for VSYS detection. */
        pd->intr7_mask |= (CLK_LF_FILT_NUM << PDSS_INTR7_CLK_LF_EDGE_CHANGED_POS);
    }

    /* Enable HF Clk as source for Fault protection filters. */
    pd->ctrl |= PDSS_CTRL_SEL_CLK_FILTER;


#if defined(CY_DEVICE_CCG6)     
    /* amux_nhv[4]/amux_nhv[3] should be set for proper connection.
       amux_nhv[3]/amux_nhv[2] is used to select 8% divider instead of 10% divider.
     */
    pd->amux_nhv_ctrl |= (3UL << 3UL);
#else
    /* amux_nhv[5]/amux_nhv[3] should be set for proper connection.
       amux_nhv[2] is used to select 8% divider instead of 6% divider.
     */    
    pd->amux_nhv_ctrl |= (1UL << 3) | (1UL << 2);
#endif
    /*
     * Connect 10% of VBUS_C to VBUS_MON comparator input. This has to be done at start to prevent errors on
     * the pct10 feedback lines.
     */
    pd->amux_denfet_ctrl |= PDSS_AMUX_DENFET_CTRL_SEL;
#endif /* defined(CY_DEVICE_CCG6) */

#if defined(CY_DEVICE_CCG3PA)

    /* Enable HF Clk as source for Fault protection filters. */
    pd->ctrl |= PDSS_CTRL_SEL_CLK_FILTER;

    /* Update Refgen setting */
    pd->refgen_0_ctrl &= ~PDSS_REFGEN_0_CTRL_REFGEN_PD;
    pd->refgen_0_ctrl  |= PDSS_REFGEN_0_CTRL_REFGEN_VREFIN_SEL |
        PDSS_REFGEN_0_CTRL_REFGEN_VREFIN_MON_SEL;

    /*
     * Silicon workaround. There seems to be a floating node for SR comparator
     * voltage reference and needs to be always turned ON and set AMP2 gain to
     * 150. These numbers are provided by the design team.
     */
    /*
     * The LSCSA block has better accuracy when the chopper logic is enabled.
     * But this can be done only for *A revision of silicon or above.
     * Also load the discharge drive strength values.
     */
    pd->refgen_0_ctrl |= (PDSS_REFGEN_0_CTRL_REFGEN_CLK_SEL);

    /*
    * In this case (applications like Power bank), VBUS_IN node of silicon is connected
    * to TYPE-C VBUS. Therefore, select VBUS_IN resistor divider to monitor TYPE-C VBUS
    */
    pd->amux_nhv_ctrl |= ((1U << AMUX_ADC_PMG1S0_VBUS_IN_8P_EN_POS) |
                            (1U << AMUX_ADC_PMG1S0_VBUS_IN_20P_EN_POS));

    pd->dischg_shv_ctrl[0] = ((pd->dischg_shv_ctrl[0] & ~PDSS_DISCHG_SHV_CTRL_DISCHG_DS_MASK) |
                                (PMG1S0_DISCHG_DS_VBUS_C_00 << PDSS_DISCHG_SHV_CTRL_DISCHG_DS_POS));
    pd->dischg_shv_ctrl[1] = ((pd->dischg_shv_ctrl[1] & ~PDSS_DISCHG_SHV_CTRL_DISCHG_DS_MASK) |
                                (PMG1S0_DISCHG_DS_VBUS_IN_00 << PDSS_DISCHG_SHV_CTRL_DISCHG_DS_POS));

#endif /* defined(CY_DEVICE_CCG3PA) */

    /* Initialize ADCs. */
    status = Cy_USBPD_Adc_Init (context, CY_USBPD_ADC_ID_0);

#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG3))
    /* PMG1S0 and PMG1S2 have two ADCs per PD block. */
    status = Cy_USBPD_Adc_Init (context, CY_USBPD_ADC_ID_1);
#endif /* (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG3)) */

    (void)status;

    return CY_USBPD_STAT_SUCCESS;
}

/*******************************************************************************
* Function Name: Cy_USBPD_PrepareDeepSleep
****************************************************************************//**
* 
* This function configures the PD block for deepsleep entry.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \param voltage
*  VBUS comparator voltage for detach
*
* \param percentage
* Percentage threshold for VBUS detach comparator
* 
* \return
* Returns true if deepsleep is possible and configured or if the block was not
* enabled, otherwise false.
*
*******************************************************************************/
bool Cy_USBPD_PrepareDeepSleep (
        cy_stc_usbpd_context_t *context,
        uint16_t voltage,
        int8_t percentage)
{
#if SYS_DEEPSLEEP_ENABLE
    cy_stc_pd_dpm_config_t *dpmConfig = NULL;
    PPDSS_REGS_T pd = NULL;
#if (!(CY_PD_SOURCE_ONLY))
    uint8_t level = 0;
#endif /* (!(CY_PD_SOURCE_ONLY)) */
#if (!CY_PD_SINK_ONLY)
    uint32_t volatile status = 0;
#endif /* (!CY_PD_SINK_ONLY) */
#if defined(CY_IP_MXUSBPD)
    uint32_t regVal = 0;
#endif /* defined(CY_IP_MXUSBPD) */

    if (context == NULL)
    {
        return false;
    }

    dpmConfig = context->dpmGetConfig();
    pd = context->base;

    if (!(dpmConfig->dpmEnabled))
    {
        return true;
    }

    if (dpmConfig->connect == true)
    {
        /* Set LA comparator for wakeup. */
        pd->intr1 = PDSS_INTR1_VCMP_LA_CHANGED;
        pd->intr1_mask |= PDSS_INTR1_VCMP_LA_CHANGED;

#if (!CY_PD_SINK_ONLY)
        if (dpmConfig->curPortRole == CY_PD_PRT_ROLE_SOURCE)
        {
            if (dpmConfig->attachedDev == CY_PD_DEV_AUD_ACC)
            {
                Cy_USBPD_TypeC_PhyDetectCCRise (context, true);
            }
            else
            {
#if defined(CY_IP_MXUSBPD)
                regVal = pd->intr1_cfg_vcmp_up_down_ls;
                regVal &= ~(PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_UP_CFG_MASK |
                        PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_UP_FILT_EN);
                regVal |= (CY_USBPD_ADC_INT_RISING) << PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_UP_CFG_POS;
                pd->intr1_cfg_vcmp_up_down_ls = regVal;
#else
                pd->intr_1_cfg &= ~PDSS_INTR_1_CFG_VCMP_UP_CFG_MASK;
                pd->intr_1_cfg |= (CY_USBPD_ADC_INT_RISING) << PDSS_INTR_1_CFG_VCMP_UP_CFG_POS;
#endif /* CY_IP */

                pd->intr1 = PDSS_INTR1_VCMP_UP_CHANGED;
                pd->intr1_mask |= PDSS_INTR1_VCMP_UP_CHANGED;

                /* If the comparator has already triggered, set the interrupt and return. */
                if (
#if defined(CY_IP_MXUSBPD)
                        (pd->intr1_status & PDSS_INTR1_STATUS_VCMP_UP_STATUS)
#else
                        (pd->status & PDSS_STATUS_VCMP_UP_STATUS)
#endif /* CY_IP */
                   )
                {
                    pd->intr1_set |= PDSS_INTR1_VCMP_UP_CHANGED;
                }
            }
        }
        else
#endif /* (!CY_PD_SINK_ONLY) */
        {
#if (!(CY_PD_SOURCE_ONLY))
#if defined(CY_IP_MXUSBPD)
            regVal = pd->intr1_cfg_vcmp_up_down_ls;
            regVal &= ~ (PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_UP_FILT_EN |
                    PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_DN_FILT_EN);
            /* Set up/down for both edges */
            regVal |=  (PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_UP_CFG_MASK |
                    PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_DN_CFG_MASK);
            pd->intr1_cfg_vcmp_up_down_ls = regVal;
#else
            /* Set the Up comparator on Active CC line to signal a detach based on the Rp level. */
            pd->intr_1_cfg &= ~ (PDSS_INTR_1_CFG_VCMP_UP_CFG_MASK | PDSS_INTR_1_CFG_VCMP_DN_CFG_MASK);
            pd->intr_1_cfg |= (CY_USBPD_ADC_INT_BOTH << PDSS_INTR_1_CFG_VCMP_UP_CFG_POS) |
                (CY_USBPD_ADC_INT_BOTH << PDSS_INTR_1_CFG_VCMP_DN_CFG_POS);
#endif /* CY_IP */

            pd->intr1 = PDSS_INTR1_VCMP_UP_CHANGED | PDSS_INTR1_VCMP_DN_CHANGED;
            pd->intr1_mask |= PDSS_INTR1_VCMP_UP_CHANGED | PDSS_INTR1_VCMP_DN_CHANGED;

            /* Check old cc status if different then */
            if(dpmConfig->ccOldStatus.state != Cy_USBPD_TypeC_GetCCStatus(context).state)
            {
                /* Fire anyone interrupt to wakeup */
                pd->intr1_set |= PDSS_INTR1_VCMP_UP_CHANGED;
                return true;
            }

#if CY_PD_REV3_ENABLE
            if (dpmConfig->frRxEnLive == false)
#endif /* CY_PD_REV3_ENABLE */
            {
                /* Set the VBus detach comparator as per current detach threshold. */
                level = Cy_USBPD_Adc_GetVbusLevel (context, 
                        context->vbusDetachAdcId,
                        voltage,
                        percentage
                        );

                /*
                 * The following call will also check if the comparator has
                 * triggered and set the interrupt.
                 */
                Cy_USBPD_Adc_CompCtrl (context, 
                        context->vbusDetachAdcId, 
                        context->vbusDetachAdcInp, 
                        level,
                        CY_USBPD_ADC_INT_RISING,
                        Cy_USBPD_Phy_VbusDetachCbk);
            }
#endif /* (!(CY_PD_SOURCE_ONLY)) */
        }
    }
    else
    {
#if ((CY_PD_HW_DRP_TOGGLE_ENABLE) || (!CY_PD_SINK_ONLY))
        uint8_t cc1_edge = CY_USBPD_ADC_INT_RISING;
        uint8_t cc2_edge = CY_USBPD_ADC_INT_RISING;
#endif /* ((CY_PD_HW_DRP_TOGGLE_ENABLE) || (!CY_PD_SINK_ONLY)) */

#if CY_PD_HW_DRP_TOGGLE_ENABLE
        /* Don't do anything if auto toggle is already running. */
        if (context->autoToggleAct)
        {
            return true;
        }

        /* If connection interrupt just occurred, set an interrupt to cause the device to wake immediately. */
        if (context->typecStartPending)
        {
            pd->intr1_mask |= PDSS_INTR1_MASK_DRP_ATTACHED_DETECTED_MASK;
            pd->intr1_set |= PDSS_INTR1_DRP_ATTACHED_DETECTED;
            return true;
        }
#endif /* CY_PD_HW_DRP_TOGGLE_ENABLE */

        /* Connect UP comparator to CC1 and down comparator to CC2 */
        pd->cc_ctrl_0 = ((pd->cc_ctrl_0 & ~PDSS_CC_CTRL_0_CMP_UP_CC1V2) |
                PDSS_CC_CTRL_0_CMP_DN_CC1V2);

#if (!CY_PD_SINK_ONLY)
        if (dpmConfig->curPortRole == CY_PD_PRT_ROLE_SOURCE)
        {
#if defined(CY_IP_MXUSBPD)
            /*
             * If Ra is present on only 1 CC line, then set the Up comparator
             * on the Ra line for a rising edge as per the Rp level. Check if
             * the comparator has already triggered, then set the interrupt and
             * return.
             *
             * Set the Dn comparator on the other line for a falling edge as
             * per the Rp level. Check if the comparator has already triggered,
             * then set the interrupt and return.
             *
             * Otherwise,
             *
             * Set the Up comparator on CC1 for a falling edge as per the Rp
             * level. Set the Dn comparator on CC2 for a falling edge as per
             * the Rp level. If the comparators have already triggered, then
             * set the respective interrupt and return.
             */

            /* Set threshold to Ra level to check if Ra is present on single CC line. */
            pd->cc_ctrl_0 &= ~(PDSS_CC_CTRL_0_CMP_UP_VSEL_MASK | PDSS_CC_CTRL_0_CMP_DN_VSEL_MASK);
            pd->cc_ctrl_0 |= ((thresholds[dpmConfig->srcCurLevelLive][0]) << PDSS_CC_CTRL_0_CMP_UP_VSEL_POS) |
                ((thresholds[dpmConfig->srcCurLevelLive][0]) << PDSS_CC_CTRL_0_CMP_DN_VSEL_POS);
            Cy_SysLib_DelayUs(10);

            status = pd->intr1_status;

            /* Apply resistor based Rp and remove current source Rp */
            pd->cc_ctrl_1 |= PDSS_CC_CTRL_1_DS_ATTACH_DET_EN;
            pd->cc_ctrl_0 &= ~(PDSS_CC_CTRL_0_RP_CC1_EN | PDSS_CC_CTRL_0_RP_CC2_EN);

            if ((status & (PDSS_INTR1_STATUS_VCMP_UP_STATUS | PDSS_INTR1_STATUS_VCMP_DN_STATUS))
                    == PDSS_INTR1_STATUS_VCMP_DN_STATUS)
            {
                cc1_edge = CY_USBPD_ADC_INT_FALLING;
            }
            else if ((status & (PDSS_INTR1_STATUS_VCMP_UP_STATUS | PDSS_INTR1_STATUS_VCMP_DN_STATUS))
                    == PDSS_INTR1_STATUS_VCMP_UP_STATUS)
            {
                cc2_edge = CY_USBPD_ADC_INT_FALLING;
            }
            Cy_SysLib_DelayUs(10);

            /* Configure and enable the CC1/CC2 detect filter. */
            pd->intr1_cfg_cc1_cc2_ls &= ~ (PDSS_INTR1_CFG_CC1_CC2_LS_CC1_CFG_MASK |
                    PDSS_INTR1_CFG_CC1_CC2_LS_CC2_CFG_MASK |
                    PDSS_INTR1_CFG_CC1_CC2_LS_CC1_FILT_EN |
                    PDSS_INTR1_CFG_CC1_CC2_LS_CC2_FILT_EN |
                    PDSS_INTR1_CFG_CC1_CC2_LS_CC1_FILT_BYPASS |
                    PDSS_INTR1_CFG_CC1_CC2_LS_CC2_FILT_BYPASS);
            pd->intr1_cfg_cc1_cc2_ls |= (((cc1_edge) << PDSS_INTR1_CFG_CC1_CC2_LS_CC1_CFG_POS) |
                        ((cc2_edge) << PDSS_INTR1_CFG_CC1_CC2_LS_CC2_CFG_POS) |
                    (PDSS_INTR1_CFG_CC1_CC2_LS_CC1_FILT_EN | PDSS_INTR1_CFG_CC1_CC2_LS_CC2_FILT_EN));

            pd->intr1 = (PDSS_INTR1_VCMP_UP_CHANGED |
                    PDSS_INTR1_VCMP_DN_CHANGED |
                    PDSS_INTR1_VCMP_LA_CHANGED |
                    PDSS_INTR1_CC1_CHANGED |
                    PDSS_INTR1_CC2_CHANGED);
            pd->intr1_mask |= (PDSS_INTR1_CC1_CHANGED | PDSS_INTR1_CC2_CHANGED);
            status = pd->intr1_status;

            /* If the comparators have  already triggered, then set the interrupts and return. */
            if(((cc1_edge == CY_USBPD_ADC_INT_RISING) && ((status & PDSS_INTR1_STATUS_CC1_STATUS) != 0U)) ||
                    ((cc1_edge == CY_USBPD_ADC_INT_FALLING) && ((status & PDSS_INTR1_STATUS_CC1_STATUS) == 0U)) ||
                    ((cc2_edge == CY_USBPD_ADC_INT_RISING) && ((status & PDSS_INTR1_STATUS_CC2_STATUS) != 0U)) ||
                    ((cc2_edge == CY_USBPD_ADC_INT_FALLING) && ((status & PDSS_INTR1_STATUS_CC2_STATUS) == 0U)))
            {
                /* Fire anyone to wakeup*/
                pd->intr1_set |= PDSS_INTR1_CC1_CHANGED;
                return true;
            }
#else
            /*
             * If Ra is present on only 1 CC line, then set the Up comparator
             * on the Ra line for a rising edge as per the Rp level. Check if
             * the comparator has already triggered, then set the interrupt and
             * return.
             *
             * Set the Dn comparator on the other line for a falling edge as
             * per the Rp level. Check if the comparator has already triggered,
             * then set the interrupt and return.
             *
             * Otherwise,
             *
             * Set the Up comparator on CC1 for a falling edge as per the Rp
             * level. Set the Dn comparator on CC2 for a falling edge as per
             * the Rp level. If the comparators have already triggered, then
             * set the respective interrupt and return.
             */

            /* Connect  UP comparator to CC1 */
            pd->cc_ctrl_0 &= ~PDSS_CC_CTRL_0_CMP_UP_CC1V2;

            /* Connect Down comparator to CC2 */
            pd->cc_ctrl_0 |=  PDSS_CC_CTRL_0_CMP_DN_CC1V2;

            /* Set threshold to Ra level to check if Ra is present on single CC line. */
            pd->cc_ctrl_0 &= ~(PDSS_CC_CTRL_0_CMP_UP_VSEL_MASK | PDSS_CC_CTRL_0_CMP_DN_VSEL_MASK);
            pd->cc_ctrl_0 |= ((thresholds[dpmConfig->srcCurLevelLive][0]) << PDSS_CC_CTRL_0_CMP_UP_VSEL_POS) |
                ((thresholds[dpmConfig->srcCurLevelLive][0]) << PDSS_CC_CTRL_0_CMP_DN_VSEL_POS);
            Cy_SysLib_DelayUs(10);

            status = pd->status;

            cc1_edge = CY_USBPD_ADC_INT_DISABLED;
            cc2_edge = CY_USBPD_ADC_INT_DISABLED;

            /* Apply resistor based Rp and remove current source Rp */
            pd->cc_ctrl_1 |= PDSS_CC_CTRL_1_DS_ATTACH_DET_EN;
            pd->cc_ctrl_0 &= ~(PDSS_CC_CTRL_0_RP_CC1_EN | PDSS_CC_CTRL_0_RP_CC2_EN);
            pd->intr_1_cfg &= ~(PDSS_INTR_1_CFG_CC1_CFG_MASK | PDSS_INTR_1_CFG_CC2_CFG_MASK |
                    PDSS_INTR_1_CFG_VCMP_UP_CFG_MASK | PDSS_INTR_1_CFG_VCMP_DN_CFG_MASK);

            pd->cc_ctrl_0 &= ~(PDSS_CC_CTRL_0_CMP_UP_VSEL_MASK | PDSS_CC_CTRL_0_CMP_DN_VSEL_MASK);

            if ((status & (PDSS_STATUS_VCMP_UP_STATUS | PDSS_STATUS_VCMP_DN_STATUS)) == PDSS_STATUS_VCMP_DN_STATUS)
            {
                cc1_edge = CY_USBPD_ADC_INT_RISING;
                cc2_edge = CY_USBPD_ADC_INT_FALLING;

                /* Set threshold at Ra level on CC1 and Rd level on CC2. */
                pd->cc_ctrl_0 |= ((thresholds[dpmConfig->srcCurLevelLive][0]) << PDSS_CC_CTRL_0_CMP_UP_VSEL_POS) |
                    (PD_CMP_VSEL_1_77_V << PDSS_CC_CTRL_0_CMP_DN_VSEL_POS);
            }
            else if ((status & (PDSS_STATUS_VCMP_UP_STATUS | PDSS_STATUS_VCMP_DN_STATUS)) == PDSS_STATUS_VCMP_UP_STATUS)
            {
                cc1_edge = CY_USBPD_ADC_INT_FALLING;
                cc2_edge = CY_USBPD_ADC_INT_RISING;

                /* Set threshold at Rd level on CC1 and Ra level on CC2. */
                pd->cc_ctrl_0 |= (PD_CMP_VSEL_1_77_V << PDSS_CC_CTRL_0_CMP_UP_VSEL_POS) |
                    ((thresholds[dpmConfig->srcCurLevelLive][0]) << PDSS_CC_CTRL_0_CMP_DN_VSEL_POS);
            }
            else
            {
                cc1_edge = CY_USBPD_ADC_INT_FALLING;
                cc2_edge = CY_USBPD_ADC_INT_FALLING;

                /* Set threshold at Rd level on both CC1 and CC2. */
                pd->cc_ctrl_0 |= (PD_CMP_VSEL_1_77_V << PDSS_CC_CTRL_0_CMP_UP_VSEL_POS) |
                    (PD_CMP_VSEL_1_77_V << PDSS_CC_CTRL_0_CMP_DN_VSEL_POS);
            }
            Cy_SysLib_DelayUs(10);

            pd->intr_1_cfg |= ((cc1_edge) << PDSS_INTR_1_CFG_VCMP_UP_CFG_POS) |
                ((cc2_edge) << PDSS_INTR_1_CFG_VCMP_DN_CFG_POS);
            pd->intr1 = (PDSS_INTR1_VCMP_UP_CHANGED | PDSS_INTR1_VCMP_DN_CHANGED |
                    PDSS_INTR1_VCMP_LA_CHANGED | PDSS_INTR1_CC1_CHANGED | PDSS_INTR1_CC2_CHANGED);
            pd->intr1_mask |= (PDSS_INTR1_CC1_CHANGED | PDSS_INTR1_CC2_CHANGED |
                    PDSS_INTR1_VCMP_UP_CHANGED | PDSS_INTR1_VCMP_DN_CHANGED);

            /* If the comparators have  already triggered, then set the interrupts and return. */
            if (
                    ((cc1_edge == CY_USBPD_ADC_INT_RISING) && ((pd->status & PDSS_STATUS_VCMP_UP_STATUS) != 0)) ||
                    ((cc1_edge == CY_USBPD_ADC_INT_FALLING) && ((pd->status & PDSS_STATUS_VCMP_UP_STATUS) == 0)) ||
                    ((cc2_edge == CY_USBPD_ADC_INT_RISING) && ((pd->status & PDSS_STATUS_VCMP_DN_STATUS) != 0)) ||
                    ((cc2_edge == CY_USBPD_ADC_INT_FALLING) && ((pd->status & PDSS_STATUS_VCMP_DN_STATUS) == 0))
               )
            {
                /* Fire anyone to wakeup*/
                pd->intr1_set |= PDSS_INTR1_VCMP_UP_CHANGED;
                return true;
            }
#endif /* CY_IP */
        }
        else
#endif /* (!CY_PD_SINK_ONLY) */
        {
            Cy_USBPD_TypeC_PhyDetectCCRise (context, false);

            /* Auto toggle cannot be started in Unattached.SNK state. */
            return true;
        }

#if CY_PD_HW_DRP_TOGGLE_ENABLE
        /* We can only use auto toggle when we don't already have an Ra connected. */
        if(
                (dpmConfig->toggle == true) &&
                (context->autoToggleEn != 0U) &&
                ((cc1_edge == CY_USBPD_ADC_INT_RISING) && (cc2_edge == CY_USBPD_ADC_INT_RISING))
          )
        {
            if (context->typecEventsCbk)
            {
                context->typecEventsCbk (context->pdStackContext, CY_PD_TYPEC_EVT_STOP_FSM);
            }

            /* Make sure trimmed Rp/Rd are disabled. */
            pd->cc_ctrl_0 &= 0xFC300000;

            pd->intr1_mask &= ~(PDSS_INTR1_VCMP_UP_CHANGED |
                    PDSS_INTR1_VCMP_DN_CHANGED |
                    PDSS_INTR1_CC1_CHANGED |
                    PDSS_INTR1_CC2_CHANGED |
                    PDSS_INTR1_VCMP_LA_CHANGED);
            pd->intr1 = (PDSS_INTR1_VCMP_UP_CHANGED |
                    PDSS_INTR1_VCMP_DN_CHANGED |
                    PDSS_INTR1_VCMP_LA_CHANGED |
                    PDSS_INTR1_CC1_CHANGED |
                    PDSS_INTR1_CC2_CHANGED);

            pd->rp_rd_cfg2 &= ~(PDSS_RP_RD_CFG2_OVERRIDE_HW_REF_CTRL |
                                PDSS_RP_RD_CFG2_VCMP_CC_OVERRIDE |
                                PDSS_RP_RD_CFG2_CC1_ATTACH_VALUE |
                                PDSS_RP_RD_CFG2_CC2_ATTACH_VALUE);
            /* Need to use firmware override of references to get expected voltage on the CC line. */
            pd->rp_rd_cfg2 |= PDSS_RP_RD_CFG2_VCMP_CC_OVERRIDE | PDSS_RP_RD_CFG2_OVERRIDE_HW_REF_CTRL;

            pd->rp_rd_cfg1 &= ~(PDSS_RP_RD_CFG1_TOGGLE_PERIOD_MASK |
                    PDSS_RP_RD_CFG1_BASE_HIGH_WIDTH_MASK |
                    PDSS_RP_RD_CFG1_CONTINUE_PREV);

#if (TIMER_TYPE == TIMER_TYPE_WDT)
            /* If WDT timer is being used, we can use a calibrated period setting. */
            pd->rp_rd_cfg1 |= (
                    ((PDSS_CY_PD_DRP_TOGGLE_PERIOD_MS * timer_get_multiplier()) << PDSS_RP_RD_CFG1_TOGGLE_PERIOD_POS) |
                    ((PDSS_DRP_HIGH_PERIOD_MS * timer_get_multiplier()) << PDSS_RP_RD_CFG1_BASE_HIGH_WIDTH_POS) |
                    PDSS_RP_RD_CFG1_HW_RP_RD_AUTO_TOGGLE |
                    PDSS_RP_RD_CFG1_RESET_COUNT
                    );
#else
            /* SYSTICK is used and LF CLK calibration is not done. */
            pd->rp_rd_cfg1 |= (
                    (PDSS_CY_PD_DRP_TOGGLE_PERIOD_VAL << PDSS_RP_RD_CFG1_TOGGLE_PERIOD_POS) |
                    (PDSS_DRP_HIGH_PERIOD_VAL << PDSS_RP_RD_CFG1_BASE_HIGH_WIDTH_POS) |
                    PDSS_RP_RD_CFG1_HW_RP_RD_AUTO_TOGGLE |
                    PDSS_RP_RD_CFG1_RESET_COUNT
                    );
#endif /* TIMER_TYPE */

            while(pd->rp_rd_cfg1 & PDSS_RP_RD_CFG1_RESET_COUNT);

            /* Setting cc1/cc2 filters */
            /* Configure filter clock cycles and disable the filter. */
            pd->intr1_cfg_cc1_cc2_ls &= ~(PDSS_INTR1_CFG_CC1_CC2_LS_CC1_FILT_SEL_MASK |
                    PDSS_INTR1_CFG_CC1_CC2_LS_CC2_FILT_SEL_MASK |
                    PDSS_INTR1_CFG_CC1_CC2_LS_CC1_FILT_EN |
                    PDSS_INTR1_CFG_CC1_CC2_LS_CC2_FILT_EN |
                    PDSS_INTR1_CFG_CC1_CC2_LS_CC1_FILT_RESET |
                    PDSS_INTR1_CFG_CC1_CC2_LS_CC2_FILT_RESET |
                    PDSS_INTR1_CFG_CC1_CC2_LS_CC1_FILT_BYPASS |
                    PDSS_INTR1_CFG_CC1_CC2_LS_CC2_FILT_BYPASS);

            pd->intr1_cfg_cc1_cc2_ls |= ((PDSS_CC_FILT_CYCLES << PDSS_INTR1_CFG_CC1_CC2_LS_CC1_FILT_SEL_POS) |
                    (PDSS_CC_FILT_CYCLES << PDSS_INTR1_CFG_CC1_CC2_LS_CC2_FILT_SEL_POS) |
                    (PDSS_INTR1_CFG_CC1_CC2_LS_CC1_FILT_EN | PDSS_INTR1_CFG_CC1_CC2_LS_CC2_FILT_EN));

            /*
             * Connect  up comparator to CC1 and down comparator to CC2. Also,
             * Set the up comparator on CC1 and the down comparator on CC2 for
             * rising threshold of 0.2V.
             */
            pd->cc_ctrl_0 = ((pd->cc_ctrl_0 & ~(PDSS_CC_CTRL_0_CMP_UP_CC1V2 |
                    PDSS_CC_CTRL_0_CMP_UP_VSEL_MASK | PDSS_CC_CTRL_0_CMP_DN_VSEL_MASK)) |
                    (PDSS_CC_CTRL_0_CMP_EN | PDSS_CC_CTRL_0_CMP_DN_CC1V2));

            /* Setting up/dn filters */
            pd->intr1_cfg_vcmp_up_down_ls &= ~(PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_UP_FILT_SEL_MASK |
                    PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_DN_FILT_SEL_MASK |
                    PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_UP_FILT_EN |
                    PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_DN_FILT_EN |
                    PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_UP_FILT_RESET |
                    PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_DN_FILT_RESET |
                    PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_UP_FILT_BYPASS |
                    PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_DN_FILT_BYPASS);
            pd->intr1_cfg_vcmp_up_down_ls |= (
                    (PDSS_CC_FILT_CYCLES << PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_UP_FILT_SEL_POS) |
                    (PDSS_CC_FILT_CYCLES << PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_DN_FILT_SEL_POS) |
                    (PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_UP_FILT_EN |
                     PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_DN_FILT_EN));

            /* Auto toggle is active. Prevent other changes from happening. */
            context->autoToggleAct = true;

            pd->intr1 = PDSS_INTR1_DRP_ATTACHED_DETECTED;
            pd->intr1_mask |= PDSS_INTR1_DRP_ATTACHED_DETECTED;

            pd->rp_rd_cfg1 |= PDSS_RP_RD_CFG1_START_TOGGLE;
        }
        else
        {
            /*
               Leave auto toggle disabled for now. It will get re-enabled when
               the Type-C state machine enters Unattached.SRC again.
             */
            context->autoToggleEn = false;
        }
#endif /* CY_PD_HW_DRP_TOGGLE_ENABLE */
    }

    return true;
#else /* !SYS_DEEPSLEEP_ENABLE */
    (void)context;
    (void)voltage;
    (void)percentage;
    return false;
#endif /* SYS_DEEPSLEEP_ENABLE */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Resume
****************************************************************************//**
* 
* This function configures all PD blocks supported by the device after exiting
* deepsleep.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* Returns true if successful, otherwise false
*
*******************************************************************************/
bool Cy_USBPD_Resume(
        cy_stc_usbpd_context_t *context)
{
#if SYS_DEEPSLEEP_ENABLE
    PPDSS_REGS_T pd = NULL;
    cy_stc_pd_dpm_config_t *dpmConfig = NULL;
    uint32_t regVal = 0;
    uint32_t mask = 0;

    if (context == NULL)
    {
        return false;
    }

    pd = context->base;
    dpmConfig = context->dpmGetConfig();

    if (!(dpmConfig->dpmEnabled))
    {
        return false;
    }

    /* Clear and disable all deepsleep interrupts. */
    mask = (PDSS_INTR1_VCMP_UP_CHANGED | PDSS_INTR1_VCMP_DN_CHANGED | PDSS_INTR1_VCMP_LA_CHANGED |
            PDSS_INTR1_CC1_CHANGED | PDSS_INTR1_CC2_CHANGED);
    pd->intr1_mask &= ~mask;
    pd->intr1       = mask;

#if (!(CY_PD_SOURCE_ONLY))
    if ((dpmConfig->connect == true) && (dpmConfig->curPortRole == CY_PD_PRT_ROLE_SINK))
    {
#if CY_PD_REV3_ENABLE
        if (dpmConfig->frRxEnLive == false)
#endif /* CY_PD_REV3_ENABLE */
        {
            /* Disable the detach detection comparator. */
            Cy_USBPD_Adc_CompCtrl (context, 
                    context->vbusDetachAdcId, 
                    CY_USBPD_ADC_INPUT_AMUX_A,
                    0,
                    CY_USBPD_ADC_INT_DISABLED,
                    NULL);
        }
    }
#else
    if (0)
    {

    }
#endif /* (!(CY_PD_SOURCE_ONLY)) */
    else if(dpmConfig->connect == false)
    {
#if ((!(CY_PD_SOURCE_ONLY)) && (!CY_PD_SINK_ONLY))
        if (dpmConfig->toggle == true)
        {
#if CY_PD_HW_DRP_TOGGLE_ENABLE
            if ((context->autoToggleEn != 0U) || (context->typecStartPending != 0U))
            {
                if (context->autoToggleAct)
                {
                    /*
                       Auto toggle is active and attach has not been detected. Do nothing.
                     */
                    return;
                }

                /* Type-C restart will be done below if required. */
                context->typecStartPending = false;

                /*
                   Type-C state machine will not be up-to-date when DRP toggle is being done by hardware.
                   We need to jump to the right Type-C state to resume operation.
                 */
                if ((pd->rp_rd_cfg1 & PDSS_RP_RD_CFG1_HW_RP_RD_AUTO_TOGGLE) != 0U)
                {
                    if (pd->status & PDSS_STATUS_RP_RD_STATUS)
                    {
                        Cy_USBPD_TypeC_EnableRp(port, CY_PD_CC_CHANNEL_1, (cy_en_pd_rp_term_t)dpmConfig->srcCurLevelLive);
                        Cy_USBPD_TypeC_EnableRp(port, CY_PD_CC_CHANNEL_2, (cy_en_pd_rp_term_t)dpmConfig->srcCurLevelLive);

                        /* Disable the deep sleep Rp. */
                        pd->cc_ctrl_1 &= ~PDSS_CC_CTRL_1_DS_ATTACH_DET_EN;

                        dpmConfig->curPortRole = CY_PD_PRT_ROLE_SOURCE;
                        dpmConfig->curPortType = PRT_TYPE_DFP;
                        if (context->typecEventsCbk)
                        {
                            context->typecEventsCbk(context->pdStackContext, CY_PD_TYPEC_EVT_UNATTACHED_SRC);
                        }
                    }
                    else
                    {
                        Cy_USBPD_TypeC_EnableRd(context, CY_PD_CC_CHANNEL_1);
                        Cy_USBPD_TypeC_EnableRd(context, CY_PD_CC_CHANNEL_2);

                        dpmConfig->curPortRole = CY_PD_PRT_ROLE_SINK;
                        dpmConfig->curPortType = PRT_TYPE_UFP;

                        if (context->typecEventsCbk)
                        {
                            context->typecEventsCbk(context->pdStackContext, CY_PD_TYPEC_EVT_UNATTACHED_SNK);
                        }
                    }

                    /* Disable the RP-RD toggle. Rp or Rd needs to be applied before this. */
                    pd->rp_rd_cfg1 &= ~(PDSS_RP_RD_CFG1_HW_RP_RD_AUTO_TOGGLE | PDSS_RP_RD_CFG1_START_TOGGLE);
                    pd->intr1_mask &= ~PDSS_INTR1_DRP_ATTACHED_DETECTED;

                    /* Leave auto toggle disabled until enabled again by Type-C module. */
                    context->autoToggleEn = false;

                    /* typec_gen_entry_event disables scanning so re-enable it */
                    dpmConfig->skipScan = false;

                    /* Type-C start is required to enable RX. */
                    Cy_USBPD_TypeC_Start(context);
                }
            }
#endif /* CY_PD_HW_DRP_TOGGLE_ENABLE */
        }
#endif /* ((!(CY_PD_SOURCE_ONLY)) && (!CY_PD_SINK_ONLY)) */

#if (!CY_PD_SINK_ONLY)
        if (dpmConfig->curPortRole == CY_PD_PRT_ROLE_SOURCE)
        {
            /* Re-enable the trimmed Rp. */
            Cy_USBPD_TypeC_EnableRp(context, CY_PD_CC_CHANNEL_1, (cy_en_pd_rp_term_t)dpmConfig->srcCurLevelLive);
            Cy_USBPD_TypeC_EnableRp(context, CY_PD_CC_CHANNEL_2, (cy_en_pd_rp_term_t)dpmConfig->srcCurLevelLive);
        }

        /* Disable the deep sleep Rp. */
        pd->cc_ctrl_1 &= ~PDSS_CC_CTRL_1_DS_ATTACH_DET_EN;

#endif /* (!CY_PD_SINK_ONLY) */

#if defined(CY_IP_MXUSBPD)
        /* Disable cc1/cc2/up/down filters */
        regVal = pd->intr1_cfg_cc1_cc2_ls;
        regVal &= ~(PDSS_INTR1_CFG_CC1_CC2_LS_CC1_FILT_EN |
                PDSS_INTR1_CFG_CC1_CC2_LS_CC2_FILT_EN);
        regVal |= PDSS_INTR1_CFG_CC1_CC2_LS_CC1_FILT_BYPASS |
            PDSS_INTR1_CFG_CC1_CC2_LS_CC2_FILT_BYPASS;
        pd->intr1_cfg_cc1_cc2_ls = regVal;

        regVal = pd->intr1_cfg_vcmp_up_down_ls;
        regVal &= ~(PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_UP_FILT_EN |
                PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_DN_FILT_EN);
        regVal |= (PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_UP_FILT_BYPASS |
                PDSS_INTR1_CFG_VCMP_UP_DOWN_LS_VCMP_DN_FILT_BYPASS);
        pd->intr1_cfg_vcmp_up_down_ls = regVal;
#else
        CY_UNUSED_PARAMETER(regVal);
#endif /* defined(CY_IP_MXUSBPD) */
    }

    return true;
#else
    CY_UNUSED_PARAMETER(context);
    return false;
#endif /* SYS_DEEPSLEEP_ENABLE */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Intr1Handler
****************************************************************************//**
* 
* Handle all wake-up interrupt sources.
* INTR1/INTR3/INTR5 etc are mapped to the wake-up interrupt vector.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_Intr1Handler (
        cy_stc_usbpd_context_t *context)
{
    PPDSS_REGS_T pd = context->base;
    uint32_t intrCause = pd->intr1_masked;
#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))

#if VBUS_RCP_ENABLE
    uint32_t mask = 0u;
#endif /* VBUS_RCP_ENABLE */

#if VBUS_SCP_ENABLE
    /*
     * SCP interrupt handling.
     * Provider FET would have been turned off. We only need to clear the interrupt and start recovery.
     */
    if ((pd->intr13_masked & (PDSS_INTR13_MASKED_CSA_SCP_CHANGED_MASKED)) != 0U)
    {
        Cy_USBPD_Fault_Vbus_ScpIntrHandler(context);
    }
#endif /* VBUS_SCP_ENABLE */

#if VBUS_RCP_ENABLE
    mask = (PDSS_INTR13_MASKED_CSA_OUT_CHANGED_MASKED) | (PDSS_INTR13_MASKED_CSA_COMP_OUT_CHANGED_MASKED) | (PDSS_INTR13_MASKED_CSA_VBUS_OVP_CHANGED_MASKED);
    /*
     * RCP interrupt handling.
     * Provider FET would have been turned off. We only need to clear the interrupt and start recovery.
     */
    if ((pd->intr13_masked & mask) != 0U)
    {
        Cy_USBPD_Fault_Vbus_RcpIntrHandler(context);
    }
#endif /* VBUS_RCP_ENABLE */

#endif /* defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3) */

    /*
     * This routine expects all interrupts which are triggered to be disabled
     * once they are fired, otherwise it can cause problems.
     */
    if (intrCause != 0U)
    {
        /* Clear the CC1 and CC2 changed interrupts. */
        if ((intrCause & PDSS_INTR1_CC1_CHANGED) != 0U)
        {
            pd->intr1_mask &= ~PDSS_INTR1_CC1_CHANGED;
            pd->intr1       = PDSS_INTR1_CC1_CHANGED;
        }

        if ((intrCause & PDSS_INTR1_CC2_CHANGED) != 0U)
        {
            pd->intr1_mask &= ~PDSS_INTR1_CC2_CHANGED;
            pd->intr1       = PDSS_INTR1_CC2_CHANGED;
        }

#if PMG1_HPD_RX_ENABLE
        if ((intrCause & PDSS_INTR1_HPDIN_CHANGED) != 0U)
        {
            pd->intr1_mask &= ~PDSS_INTR1_HPDIN_CHANGED;
            pd->intr1 = PDSS_INTR1_HPDIN_CHANGED;
            Cy_USBPD_Hpd_Intr1Handler(context);
        }
#endif /* PMG1_HPD_RX_ENABLE */

#if (!CY_PD_SINK_ONLY)
#if (CY_PD_REV3_ENABLE && CY_PD_FRS_RX_ENABLE)
        if ((intrCause & PDSS_INTR1_VSWAP_VBUS_LESS_5_DONE) != 0U)
        {
            pd->intr1_mask &= ~PDSS_INTR1_VSWAP_VBUS_LESS_5_DONE;
            pd->intr1 = PDSS_INTR1_VSWAP_VBUS_LESS_5_DONE;
            Cy_USBPD_Vbus_FrsRx_IntrHandler(context);
        }
#endif /* (CY_PD_REV3_ENABLE && CY_PD_FRS_RX_ENABLE) */
#endif /* (!CY_PD_SINK_ONLY) */

#if ((SYS_DEEPSLEEP_ENABLE) && (CY_PD_HW_DRP_TOGGLE_ENABLE))
        if ((intrCause & PDSS_INTR1_DRP_ATTACHED_DETECTED) != 0U)
        {
            pd->intr1_mask &= ~PDSS_INTR1_DRP_ATTACHED_DETECTED;
            pd->intr1 = PDSS_INTR1_DRP_ATTACHED_DETECTED;

            /* Auto toggle has been stopped. Also, mark Type-C restart pending. */
            context->autoToggleAct = false;
            context->typecStartPending = true;
        }
#endif /* ((SYS_DEEPSLEEP_ENABLE) && (CY_PD_HW_DRP_TOGGLE_ENABLE)) */

#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))
#if VCONN_OCP_ENABLE
        if ((intrCause & (PDSS_INTR1_CC1_OCP_CHANGED | PDSS_INTR1_CC2_OCP_CHANGED)) != 0U)
        {
            pd->intr1_mask &= ~(PDSS_INTR1_CC1_OCP_CHANGED | PDSS_INTR1_CC2_OCP_CHANGED);
            pd->intr1 = (PDSS_INTR1_CC1_OCP_CHANGED | PDSS_INTR1_CC2_OCP_CHANGED);
            Cy_USBPD_Fault_Vconn_OcpIntrHandler(context);
        }
#endif /* VCONN_OCP_ENABLE */

#if defined(CY_DEVICE_CCG6)
        /* CC1/2 OVP (VBus short) handler. */
        if ((intrCause & (PDSS_INTR1_CC1_OVP_CHANGED | PDSS_INTR1_CC2_OVP_CHANGED)) != 0U)
        {
            pd->intr1_mask &= ~(PDSS_INTR1_CC1_OVP_CHANGED | PDSS_INTR1_CC2_OVP_CHANGED);
            pd->intr1 = (PDSS_INTR1_CC1_OVP_CHANGED | PDSS_INTR1_CC2_OVP_CHANGED);
            Cy_USBPD_Fault_CcOvp_IntrHandler(context);
        }

#if PMG1_V5V_CHANGE_DETECT
        if ((intrCause & PDSS_INTR1_MASK_V5V_CHANGED_MASK) != 0U)
        {
            pd->intr1 = PDSS_INTR1_MASK_V5V_CHANGED_MASK;
            Cy_USBPD_Vbus_V5vChangeDetectHandler (context);
        }
#endif /* PMG1_V5V_CHANGE_DETECT */
#endif /* defined (CY_DEVICE_CCG6) */
#endif /* defined (CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3) */

#if defined(CY_DEVICE_CCG3)
        if ((intrCause & (PDSS_INTR1_CMP_OUT1_CHANGED | PDSS_INTR1_CMP_OUT2_CHANGED)) != 0U)
        {
            Cy_USBPD_Adc_IntrHandler(context);
            pd->intr1 = (PDSS_INTR1_CMP_OUT1_CHANGED | PDSS_INTR1_CMP_OUT2_CHANGED);
        }

#if PMG1_V5V_CHANGE_DETECT
        if ((intrCause & PDSS_INTR1_MASK_V5V_CHANGED_MASK) != 0U)
        {
            pd->intr1 = PDSS_INTR1_MASK_V5V_CHANGED_MASK;
            Cy_USBPD_Vbus_V5vChangeDetectHandler (context);
        }
#endif /* PMG1_V5V_CHANGE_DETECT */
#endif /* defined(CY_DEVICE_CCG3) */

        if ((intrCause & PDSS_INTR1_VCMP_LA_CHANGED) != 0U)
        {
            /* Disable the interrupt. */
            pd->intr1_mask &= ~PDSS_INTR1_VCMP_LA_CHANGED;
            pd->intr1 = PDSS_INTR1_VCMP_LA_CHANGED;
        }

        if ((intrCause & PDSS_INTR1_VCMP_UP_CHANGED) != 0U)
        {
            /* Disable the interrupt. */
            pd->intr1_mask &= ~PDSS_INTR1_VCMP_UP_CHANGED;
            pd->intr1 = PDSS_INTR1_VCMP_UP_CHANGED;
        }

        if ((intrCause & PDSS_INTR1_VCMP_DN_CHANGED) != 0U)
        {
            /* Disable the interrupt. */
            pd->intr1_mask &= ~PDSS_INTR1_VCMP_DN_CHANGED;
            pd->intr1 = PDSS_INTR1_VCMP_DN_CHANGED;
        }

        /* Clear all handled interrupts */
        pd->intr1 = intrCause ; 
    }

#if defined(CY_DEVICE_CCG3PA)
    if ((pd->intr3_masked & (1u << PDSS_INTR3_CMP_OUT_CHANGED_POS)) != 0U)
    {
        Cy_USBPD_Adc_IntrHandler(context);
    }

    if ((pd->intr3_masked & ((1u << PDSS_INTR3_CMP_OUT_CHANGED_POS) << CY_USBPD_ADC_ID_1)) != 0U)
    {
        Cy_USBPD_Adc_IntrHandler(context);
    }
#endif /* defined(CY_DEVICE_CCG3PA) */

#if defined(CY_DEVICE_CCG3)
    if ((pd->intr3_masked & PDSS_INTR3_VSYS_CHANGED) != 0U)
    {
        /* Clear the interrupt. */
        pd->intr3 = PDSS_INTR3_VSYS_CHANGED;

        /* Notify application about change in VSYS supply status. */
        if (context->supplyChangeCbk != NULL)
        {
            context->supplyChangeCbk (context, CY_USBPD_SUPPLY_VSYS,
                    ((pd->ncell_status & PDSS_NCELL_STATUS_VSYS_STATUS) != 0U));
        }
    }

    if ((pd->intr3_masked & PDSS_INTR3_POS_CSA_CHANGED) != 0U)
    {
        /* Clear and disable the interrupt first. */
        pd->intr3_mask &= ~PDSS_INTR3_POS_CSA_CHANGED;
        pd->intr3 = PDSS_INTR3_POS_CSA_CHANGED;

#if VBUS_OCP_ENABLE
        Cy_USBPD_Fault_Vbus_OcpIntrHandler(context);
#endif /* VBUS_OCP_ENABLE */
    }

    if ((pd->intr3_masked & PDSS_INTR3_NEG_CSA_CHANGED) != 0U)
    {
        /* Clear and disable the interrupt first. */
        pd->intr3_mask &= ~PDSS_INTR3_NEG_CSA_CHANGED;
        pd->intr3 = PDSS_INTR3_NEG_CSA_CHANGED;

#if VBUS_OCP_ENABLE
        Cy_USBPD_Fault_Vbus_OcpIntrHandler(context);
#endif /* VBUS_OCP_ENABLE */
    }

    if ((pd->intr3_masked & PDSS_INTR3_POS_OV_CHANGED) != 0U)
    {
        /* Disable and clear the OV interrupt. */
        pd->intr3_mask &= ~PDSS_INTR3_POS_OV_CHANGED;
        pd->intr3 = PDSS_INTR3_POS_OV_CHANGED;

#if VBUS_OVP_ENABLE
        Cy_USBPD_Fault_Vbus_OvpIntrHandler(context);
#endif /* VBUS_OVP_ENABLE */
    }

    if ((pd->intr3_masked & PDSS_INTR3_POS_UV_CHANGED) != 0U)
    {
        /* Disable and clear the UV interrupt. */
        pd->intr3_mask &= ~PDSS_INTR3_POS_UV_CHANGED;
        pd->intr3 = PDSS_INTR3_POS_UV_CHANGED;

#if VBUS_OVP_ENABLE
        Cy_USBPD_Fault_Vbus_UvpIntrHandler(context);
#endif /* VBUS_OVP_ENABLE */
    }
#endif /* defined(CY_DEVICE_CCG3) */

#if defined(CY_IP_MXUSBPD)
    if (pd->intr5_masked != 0U)
    {
#if VBUS_OVP_ENABLE
        if ((pd->intr5_masked & (1u << CY_USBPD_VBUS_FILTER_ID_OV)) != 0U)
        {
            Cy_USBPD_Fault_Vbus_OvpIntrHandler(context);
        }
#endif /* VBUS_OVP_ENABLE */

#if (VBUS_UVP_ENABLE)
        if ((pd->intr5_masked & (1UL << CY_USBPD_VBUS_FILTER_ID_UV)) != 0U)
        {
            Cy_USBPD_Fault_Vbus_UvpIntrHandler(context);
        }
#endif /* (VBUS_UVP_ENABLE) */
    }
#endif /* defined(CY_IP_MXUSBPD) */

#if (defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))
    if (pd->intr7_masked != 0U)
    {
        uint32_t value = pd->intr7;
        /* Clear all interrupts. */
        pd->intr7 = value;

        /* VSYS detection interrupt. */
        if ((pd->intr7_status & (CLK_LF_FILT_NUM << PDSS_INTR7_STATUS_FILT_8_POS)) != 0U)
        {
            /* VSYS is now present: Enable switch and disable regulator. */
            pd->vreg_vsys_ctrl |= PDSS_VREG_VSYS_CTRL_ENABLE_VDDD_SWITCH;
            Cy_SysLib_DelayUs (100);

            pd->vreg_vsys_ctrl &= ~PDSS_VREG_VSYS_CTRL_VREG20_1_EN;

            /* Notify application about presence of VSYS supply. */
            if (context->supplyChangeCbk != NULL)
            {
                context->supplyChangeCbk (context, CY_USBPD_SUPPLY_VSYS, true);
            }
        }
        else
        {
            /* Disable the VSYS-VDDD Switch. */
            pd->vreg_vsys_ctrl &= ~PDSS_VREG_VSYS_CTRL_ENABLE_VDDD_SWITCH;

            /* Notify application about absence of VSYS supply. */
            if (context->supplyChangeCbk != NULL)
            {
                context->supplyChangeCbk (context, CY_USBPD_SUPPLY_VSYS, false);
            }
        }

#if (defined(CY_DEVICE_PMG1S3) && (SBU_LEVEL_DETECT_EN != 0))
        if (((pd->intr7_status & (CLK_LF_FILT_SBU1 << PDSS_INTR7_STATUS_FILT_8_POS)) != 0U) ||
            ((pd->intr7_status & (CLK_LF_FILT_SBU2 << PDSS_INTR7_STATUS_FILT_8_POS)) != 0U))
        {
            bool sbu1Detect = ((pd->intr7_status & (CLK_LF_FILT_SBU1 << PDSS_INTR7_STATUS_FILT_8_POS)) != 0);
            bool sbu2Detect = ((pd->intr7_status & (CLK_LF_FILT_SBU2 << PDSS_INTR7_STATUS_FILT_8_POS)) != 0);

            if(context->sbuDetectCb != NULL)
            {
                context->sbuDetectCb(context, sbu1Detect, sbu2Detect);
            }
        }

#endif /* (defined(CY_DEVICE_PMG1S3) && (SBU_LEVEL_DETECT_EN != 0))) */
    }
#endif /* defined (CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3) */

#if BATTERY_CHARGING_ENABLE
#if (defined(CY_DEVICE_CCG3PA) && (!QC_AFC_CHARGING_DISABLED))
    if ((pd->intr9_masked & (PDSS_INTR9_QCOM_RCVR_DM_CHANGED_MASK | PDSS_INTR9_QCOM_RCVR_DP_CHANGED_MASK)) != 0u)
    {
        pd->intr9_mask &= ~(PDSS_INTR9_QCOM_RCVR_DM_CHANGED_MASK |
                PDSS_INTR9_QCOM_RCVR_DP_CHANGED_MASK);
        pd->intr9 = PDSS_INTR9_QCOM_RCVR_DM_CHANGED_MASK |
            PDSS_INTR9_QCOM_RCVR_DP_CHANGED_MASK;
    }
#elif (defined(CY_DEVICE_CCG6) && (!QC_AFC_CHARGING_DISABLED))
    if ((pd->intr9_masked &
                (PDSS_INTR9_MASKED_QCOM_RCVR_DM_CHANGED_MASKED |PDSS_INTR9_MASKED_QCOM_RCVR_DP_CHANGED_MASKED)
        ) != 0u)
    {
        pd->intr9_mask &= ~(PDSS_INTR9_MASK_QCOM_RCVR_DM_CHANGED_MASK |
                PDSS_INTR9_MASK_QCOM_RCVR_DP_CHANGED_MASK);
        pd->intr9 = PDSS_INTR9_QCOM_RCVR_DM_CHANGED |
            PDSS_INTR9_QCOM_RCVR_DP_CHANGED;
    }
#elif (defined(CY_DEVICE_PMG1S3) && (!QC_AFC_CHARGING_DISABLED))
    if ((pd->intr9_masked & (PDSS_INTR9_MASK_QCOM_RCVR_DM_CHANGED_MASK | PDSS_INTR9_MASK_QCOM_RCVR_DP_CHANGED_MASK)) != 0u)
    {
        pd->intr9_mask &= ~(PDSS_INTR9_MASK_QCOM_RCVR_DM_CHANGED_MASK |
                PDSS_INTR9_MASK_QCOM_RCVR_DP_CHANGED_MASK);
        pd->intr9 = PDSS_INTR9_MASK_QCOM_RCVR_DM_CHANGED_MASK |
            PDSS_INTR9_MASK_QCOM_RCVR_DP_CHANGED_MASK;
    }
#endif /* (defined(CY_DEVICE_PMG1S3) && (!QC_AFC_CHARGING_DISABLED)) */

#if defined(CY_IP_MXUSBPD)
    if ((pd->intr9_masked & (BCH_PORT_0_CMP1_INTR_MASK | BCH_PORT_0_CMP2_INTR_MASK)) != 0u)
    {
        Cy_USBPD_Bch_Intr1Handler(context);
    }
#if (NO_OF_BC_PORTS == 2)
    if ((pd->intr9_masked & (BCH_PORT_1_CMP1_INTR_MASK | BCH_PORT_1_CMP2_INTR_MASK)) != 0u)
    {
        Cy_USBPD_Bch_Intr1Handler(context);
    }
#endif /* (NO_OF_BC_PORTS == 2) */
#elif defined(CY_IP_M0S8USBPD)
    if ((pd->intr3_masked & PDSS_INTR3_MASKED_CHGDET_CHANGED_MASKED) != 0u)
    {
        Cy_USBPD_Bch_Intr1Handler(context);
    }
#endif /* CY_IP */

#endif /* BATTERY_CHARGING_ENABLE */

#if (defined (CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))

#if defined(CY_DEVICE_CCG6)
    /* SBU1/2 OVP change interrupt. */
    if ((pd->intr3_masked & PDSS_INTR3_SBU1_SBU2_OVP_CHANGED_MASK) != 0U)
    {
        Cy_USBPD_Fault_SbuOvp_IntrHandler (context);
    }
#endif /* defined (CY_DEVICE_CCG6) */

    /* Comparator (ADC) output change interrupt. */
    if ((pd->intr3_masked & PDSS_INTR3_CMP_OUT_CHANGED) != 0U)
    {
        Cy_USBPD_Adc_IntrHandler(context);
    }
#if VBUS_OCP_ENABLE
    /*
     * INTR13.CSA_OCP_CHANGED interrupt is used to indicate OCP condition.
     */
    if ((pd->intr13_masked & PDSS_INTR13_CSA_OCP_CHANGED) != 0U)
    {
        Cy_USBPD_Fault_Vbus_OcpIntrHandler(context);
    }
#endif /* VBUS_OCP_ENABLE */

#endif /* defined (CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3) */
}

/*******************************************************************************
* Function Name: Cy_USBPD_SetSupplyChange_EvtCb
****************************************************************************//**
* 
* Register a callback that can be used for notification of power supply changes.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \param cb
* Callback function pointer
*
* \return
* CY_USBPD_STAT_SUCCESS if operation is successful, 
* CY_USBPD_STAT_BAD_PARAM if the context pointer or cb is invalid.
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_SetSupplyChange_EvtCb(
        cy_stc_usbpd_context_t *context,
        cy_usbpd_supply_change_cbk_t cb)
{
    if (
            (context == NULL) || (cb == NULL)
       )
    {
        return CY_USBPD_STAT_BAD_PARAM;
    }

    context->supplyChangeCbk = cb;

    return CY_USBPD_STAT_SUCCESS;
}

/*******************************************************************************
* Function Name: Cy_USBPD_SetEvtCb
****************************************************************************//**
* 
* Register a callback that can be used for notification of USBPD driver events.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \param cb
* Callback function pointer
*
* \return
* CY_USBPD_STAT_SUCCESS if operation is successful, 
* CY_USBPD_STAT_BAD_PARAM if the context pointer or cb is invalid.
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_SetEvtCb(
        cy_stc_usbpd_context_t *context,
        cy_usbpd_evt_cbk_t cb)
{
    if (
            (context == NULL) || (cb == NULL)
       )
    {
        return CY_USBPD_STAT_BAD_PARAM;
    }

    context->usbpdEventsCbk = cb;

    return CY_USBPD_STAT_SUCCESS;
}

/*******************************************************************************
* Function Name: Cy_USBPD_SetTypeCEvtCb
****************************************************************************//**
* 
* Register a callback that can be used for notification of Type C driver events.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \param callerContext
* Pointer to the caller context
*
* \param cb
* Callback function pointer
*
* \return
* CY_USBPD_STAT_SUCCESS if operation is successful, 
* CY_USBPD_STAT_BAD_PARAM if the context pointer or cb is invalid.
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_SetTypeCEvtCb(
        cy_stc_usbpd_context_t *context,
        void *callerContext,
        cy_usbpd_typec_evt_cbk_t cb)
{
    if (
            (context == NULL) || (cb == NULL)
       )
    {
        return CY_USBPD_STAT_BAD_PARAM;
    }

    context->typecEventsCbk = cb;
    context->pdStackContext = callerContext;

    return CY_USBPD_STAT_SUCCESS;
}

/*******************************************************************************
* Function Name: Cy_USBPD_SetSbuLevelDetect_EvtCb
****************************************************************************//**
*
* Register a callback that can be used for notification of SBU level detection.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \param cb
* Callback function pointer
*
* \return
* CY_USBPD_STAT_SUCCESS if operation is successful,
* CY_USBPD_STAT_BAD_PARAM if the context pointer or cb is invalid.
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_SetSbuLevelDetect_EvtCb(
        cy_stc_usbpd_context_t *context,
        cy_cb_sbu_level_detect_t cb)
{
    if (
            (context == NULL) || (cb == NULL)
       )
    {
        return CY_USBPD_STAT_BAD_PARAM;
    }

    context->sbuDetectCb = cb;

    return CY_USBPD_STAT_SUCCESS;
}

#endif /* (defined(CY_IP_MXUSBPD) || defined(CY_IP_M0S8USBPD)) */


/* [] END OF FILE */
