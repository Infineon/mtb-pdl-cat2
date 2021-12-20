/***************************************************************************//**
* \file cy_usbpd_bch.c
* \version 1.30
*
* Provides implementation of legacy battery charging support functions using
* the USBPD IP.
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

#include "cy_device.h"
#include "cy_device_headers.h"

#if (defined(CY_IP_MXUSBPD) || defined(CY_IP_M0S8USBPD))

#include <cy_usbpd_defines.h>
#include <cy_usbpd_bch.h>
#include <cy_usbpd_common.h>

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_Phy_Init
****************************************************************************//**
*
* This function initializes the charger detect block and registers a callback
* function with the charger detect PHY layer.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \param bcPhyCbk
* Battery Charging Event Callback function pointer. Must be non-NULL.
*
* \return
* \ref cy_en_usbpd_status_t
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Bch_Phy_Init(cy_stc_usbpd_context_t *context, cy_cb_bc_phy_events_t bcPhyCbk)
{
    PPDSS_REGS_T pd;
    cy_en_usbpd_status_t stat = CY_USBPD_STAT_BAD_PARAM;

    if ((context != NULL) && (context->port < NO_OF_TYPEC_PORTS))
    {
        if (bcPhyCbk != NULL)
        {
            pd = context->base;
            context->bcPhyCbk = bcPhyCbk;

#if defined(CY_IP_MXUSBPD)
            pd->intr9_cfg_bch_det[0] &= ~(PDSS_INTR9_CFG_BCH_DET_BCH_DET_COMP0_FILT_EN |
                    PDSS_INTR9_CFG_BCH_DET_BCH_DET_COMP0_CFG_MASK |
                    PDSS_INTR9_CFG_BCH_DET_BCH_DET_COMP0_FILT_SEL_MASK |
                    PDSS_INTR9_CFG_BCH_DET_BCH_DET_COMP1_FILT_EN  |
                    PDSS_INTR9_CFG_BCH_DET_BCH_DET_COMP1_CFG_MASK |
                    PDSS_INTR9_CFG_BCH_DET_BCH_DET_COMP1_FILT_SEL_MASK);
            stat = CY_USBPD_STAT_SUCCESS;
#elif defined(CY_IP_M0S8USBPD)
            pd->intr3_cfg_0 &= ~(PDSS_INTR3_CFG_0_CHGDET_FILT_EN |
                    PDSS_INTR3_CFG_0_CHGDET_CFG_MASK |
                    PDSS_INTR3_CFG_0_CHGDET_FILT_SEL_MASK);
            stat = CY_USBPD_STAT_SUCCESS;
#else
            stat = CY_USBPD_STAT_NOT_SUPPORTED;
#endif /* defined(CY_IP_MXUSBPD) */
        }
    }

    return stat;
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_Phy_En
****************************************************************************//**
*
* This function enables the charging block hardware. The Cy_USBPD_Bch_Phy_Init
* function should previously have been called to initialize the block.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* \ref cy_en_usbpd_status_t
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Bch_Phy_En(cy_stc_usbpd_context_t *context)
{
    cy_en_usbpd_status_t stat = CY_USBPD_STAT_NOT_SUPPORTED;

#if defined(CY_IP_MXUSBPD)
    PPDSS_REGS_T pd = context->base ;

    /* Enable Charger detect block */
    pd->bch_det_0_ctrl[0] = PDSS_BCH_DET_0_CTRL_EN_CHGDET;
    pd->bch_det_1_ctrl[0] = PDSS_BCH_DET_1_CTRL_CHGDET_ISO_N;

#if (defined(CY_DEVICE_CCG6))
    /* Connect the charger detect block to the D+/D- signals on the appropriate side of the Type-C connector. */
    if ((context->dpmGetConfig())->polarity != 0u)
    {
        pd->dpdm_ctrl = (pd->dpdm_ctrl & ~((uint32_t)PDSS_DPDM_CTRL_DPDM_T_DPDM_MASK)) |
            (2UL << PDSS_DPDM_CTRL_DPDM_T_DPDM_POS);
    }
    else
    {
        pd->dpdm_ctrl = (pd->dpdm_ctrl & ~((uint32_t)PDSS_DPDM_CTRL_DPDM_T_DPDM_MASK)) |
            (8UL << PDSS_DPDM_CTRL_DPDM_T_DPDM_POS);
    }
#endif /* (defined(CY_DEVICE_CCG6)) */

#if !QC_AFC_CHARGING_DISABLED
#if defined(CY_DEVICE_CCG3PA)
    pd->ctrl |= (1UL << PDSS_CTRL_AFC_ENABLED_POS);
#else
    pd->ctrl |= (uint32_t)PDSS_CTRL_AFC_ENABLED;
#endif /* defined(CY_DEVICE_CCG3PA) */
#endif /* !QC_AFC_CHARGING_DISABLED */

    Cy_SysLib_DelayUs(50);
    stat = CY_USBPD_STAT_SUCCESS;

#elif defined(CY_IP_M0S8USBPD)
    PPDSS_REGS_T pd = context->base;

    pd->chgdet_ctrl_0 = PDSS_CHGDET_CTRL_0_EN_CHGDET;
    pd->chgdet_ctrl_1 = PDSS_CHGDET_CTRL_1_CHGDET_ISO_N;
    stat = CY_USBPD_STAT_SUCCESS;
#else
    CY_UNUSED_PARAMETER(context);
#endif /* CY_IP */

    return stat;
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_Phy_Dis
****************************************************************************//**
*
* This function disables the charging block hardware in the USB-PD IP.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* \ref cy_en_usbpd_status_t
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Bch_Phy_Dis(cy_stc_usbpd_context_t *context)
{
    cy_en_usbpd_status_t stat = CY_USBPD_STAT_NOT_SUPPORTED;

#if defined(CY_IP_MXUSBPD)
    PPDSS_REGS_T pd = context->base;

    /* Disable and clear all interrupts */
    pd->intr9_mask &= (uint32_t)(~(BCH_PORT_0_CMP1_2_INTR_MASK << 1u));
    pd->intr9 = (uint32_t)(BCH_PORT_0_CMP1_2_INTR_MASK << 1u);

    /* Power down the charger detect block. */
    pd->bch_det_1_ctrl[0] = 0;
    pd->bch_det_0_ctrl[0] = PDSS_BCH_DET_0_CTRL_PD;

#if (defined(CY_DEVICE_CCG6))
    /* Isolate the charge detect block from the DP DM lines */
    pd->dpdm_ctrl &= ~((uint32_t)(PDSS_DPDM_CTRL_DPDM_T_DPDM_MASK | PDSS_DPDM_CTRL_DCP_SHORT_DPDM_TOP |
            PDSS_DPDM_CTRL_DCP_SHORT_DPDM_BOTTOM));
#endif /* (defined(CY_DEVICE_CCG6)) */

#if !QC_AFC_CHARGING_DISABLED
#if defined(CY_DEVICE_CCG3PA)
    pd->ctrl &= ~(1UL << PDSS_CTRL_AFC_ENABLED_POS);
#else
    pd->ctrl &= ~(PDSS_CTRL_AFC_ENABLED);
#endif /* defined(CY_DEVICE_CCG3PA) */
#endif /* !QC_AFC_CHARGING_DISABLED  */

    stat = CY_USBPD_STAT_SUCCESS;

#elif defined(CY_IP_M0S8USBPD)
    PPDSS_REGS_T pd = context->base;

    /* Disable and clear charger detect interrupts. */
    pd->intr3_mask &= ~(PDSS_INTR3_MASK_CHGDET_CHANGED_MASK);
    pd->intr3 = PDSS_INTR3_CHGDET_CHANGED;

    /* Disable the charger detect block. */
    pd->chgdet_ctrl_1 = PDSS_CHGDET_CTRL_1_DEFAULT;
    pd->chgdet_ctrl_0 = PDSS_CHGDET_CTRL_0_DEFAULT;

    stat = CY_USBPD_STAT_SUCCESS;
#else
    CY_UNUSED_PARAMETER(context);
#endif /* CY_IP */

    return stat;
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_Phy_ConfigSrcTerm
****************************************************************************//**
*
* This function applies the desired source terminations on D+/D- lines.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \param chargerTerm
* Charger Terminations as per the required protocol
*
* \return
* \ref cy_en_usbpd_status_t
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Bch_Phy_ConfigSrcTerm(cy_stc_usbpd_context_t *context,
        cy_en_usbpd_bch_src_term_t chargerTerm)
{
    cy_en_usbpd_status_t status = CY_USBPD_STAT_SUCCESS;

#if (defined(CY_IP_MXUSBPD) && (!CY_PD_SINK_ONLY))
    PPDSS_REGS_T pd = context->base;

    /* Remove any existing terminations. */
    (void)Cy_USBPD_Bch_Phy_RemoveTerm(context);

#if (defined(CY_DEVICE_CCG6))
    uint32_t regval = pd->dpdm_ctrl & ~PDSS_DPDM_CTRL_DPDM_T_DPDM_MASK;

    /* Make sure DP/DM connection to SYS side is disabled in all modes other than SDP/CDP. */
    if ((chargerTerm != CHGB_SRC_TERM_CDP) && (chargerTerm != CHGB_SRC_TERM_SDP))
    {
        regval &= ~(PDSS_DPDM_CTRL_DPDM_ISO_N | PDSS_DPDM_CTRL_DPDM_SWITCH_CTRL_MASK);
    }

    /* Make sure that charger detect block is connected to the correct set of D+/D- lines. */
    if ((context->dpmGetConfig())->polarity != 0u)
    {
        pd->dpdm_ctrl = regval | (uint32_t)(2UL << PDSS_DPDM_CTRL_DPDM_T_DPDM_POS);
    }
    else
    {
        pd->dpdm_ctrl = regval | (uint32_t)(8UL << PDSS_DPDM_CTRL_DPDM_T_DPDM_POS);
    }

    Cy_SysLib_DelayUs(10);
#endif /* (CY_DEVICE_CCG6) */

    switch(chargerTerm)
    {
        case CHGB_SRC_TERM_APPLE_1A:
            pd->bch_det_1_ctrl[0] |= ((1UL << PDSS_BCH_DET_1_CTRL_CHGDET_APPLE_MODE_DP_POS) |
                (2UL << PDSS_BCH_DET_1_CTRL_CHGDET_APPLE_MODE_DM_POS) |
                PDSS_BCH_DET_1_CTRL_CHGDET_APP_DET);
            pd->bch_det_0_ctrl[0] |= (PDSS_BCH_DET_0_CTRL_VDM_SRC_EN);
            break;

        case CHGB_SRC_TERM_APPLE_2_1A:
            pd->bch_det_1_ctrl[0] |= ((2UL << PDSS_BCH_DET_1_CTRL_CHGDET_APPLE_MODE_DP_POS) |
                (1UL << PDSS_BCH_DET_1_CTRL_CHGDET_APPLE_MODE_DM_POS) |
                PDSS_BCH_DET_1_CTRL_CHGDET_APP_DET);
            pd->bch_det_0_ctrl[0] |= (PDSS_BCH_DET_0_CTRL_VDP_SRC_EN);
            break;

        case CHGB_SRC_TERM_APPLE_2_4A:
            pd->bch_det_1_ctrl[0] |= ((2UL << PDSS_BCH_DET_1_CTRL_CHGDET_APPLE_MODE_DP_POS) |
                (2UL << PDSS_BCH_DET_1_CTRL_CHGDET_APPLE_MODE_DM_POS) |
                 PDSS_BCH_DET_1_CTRL_CHGDET_APP_DET);
            pd->bch_det_0_ctrl[0] |= (PDSS_BCH_DET_0_CTRL_VDP_SRC_EN |
                    PDSS_BCH_DET_0_CTRL_VDM_SRC_EN);
            break;

#if (!QC_AFC_CHARGING_DISABLED)
        case CHGB_SRC_TERM_QC:
        case CHGB_SRC_TERM_AFC:
            pd->bch_det_0_ctrl[0] |= (PDSS_BCH_DET_0_CTRL_RDM_PD_EN |
                PDSS_BCH_DET_0_CTRL_RDAT_LKG_DP_EN);
            break;
#endif /* (!QC_AFC_CHARGING_DISABLED) */

        case CHGB_SRC_TERM_DCP:
            pd->bch_det_0_ctrl[0] |= (PDSS_BCH_DET_0_CTRL_DCP_EN | PDSS_BCH_DET_0_CTRL_RDAT_LKG_DP_EN);
            break;

        case CHGB_SRC_TERM_SDP:
            pd->bch_det_0_ctrl[0] |= (PDSS_BCH_DET_0_CTRL_RDP_PD_EN | PDSS_BCH_DET_0_CTRL_RDM_PD_EN);
            break;

        case CHGB_SRC_TERM_CDP:
            pd->bch_det_0_ctrl[0] |= (PDSS_BCH_DET_0_CTRL_RDP_PD_EN | PDSS_BCH_DET_0_CTRL_RDM_PD_EN | PDSS_BCH_DET_0_CTRL_IDP_SNK_EN);
            break;

        default:
            status = CY_USBPD_STAT_BAD_PARAM;
            break;
    }

#elif (defined(CY_IP_M0S8USBPD) && (!CY_PD_SINK_ONLY))

    PPDSS_REGS_T pd = context->base;

    /* Remove any existing terminations. */
    (void)Cy_USBPD_Bch_Phy_RemoveTerm(context);

    switch(chargerTerm)
    {
        case CHGB_SRC_TERM_APPLE_1A:
            pd->chgdet_ctrl_1 |= ((1UL << PDSS_CHGDET_CTRL_1_CHGDET_APPLE_MODE_DP_POS) |
                    (2UL << PDSS_CHGDET_CTRL_1_CHGDET_APPLE_MODE_DM_POS) |
                    PDSS_CHGDET_CTRL_1_CHGDET_APP_DET);
            pd->chgdet_ctrl_0 |= PDSS_CHGDET_CTRL_0_VDM_SRC_EN;
            break;

        case CHGB_SRC_TERM_APPLE_2_1A:
            pd->chgdet_ctrl_1 |= ((2UL << PDSS_CHGDET_CTRL_1_CHGDET_APPLE_MODE_DP_POS) |
                    (1UL << PDSS_CHGDET_CTRL_1_CHGDET_APPLE_MODE_DM_POS) |
                    PDSS_CHGDET_CTRL_1_CHGDET_APP_DET);
            pd->chgdet_ctrl_0 |= PDSS_CHGDET_CTRL_0_VDP_SRC_EN;
            break;

        case CHGB_SRC_TERM_APPLE_2_4A:
            pd->chgdet_ctrl_1 |= ((2UL << PDSS_CHGDET_CTRL_1_CHGDET_APPLE_MODE_DP_POS) |
                    (2UL << PDSS_CHGDET_CTRL_1_CHGDET_APPLE_MODE_DM_POS) |
                    PDSS_CHGDET_CTRL_1_CHGDET_APP_DET);
            pd->chgdet_ctrl_0 |= PDSS_CHGDET_CTRL_0_VDP_SRC_EN | PDSS_CHGDET_CTRL_0_VDM_SRC_EN;
            break;

        case CHGB_SRC_TERM_DCP:
            pd->chgdet_ctrl_0 |= (PDSS_CHGDET_CTRL_0_DCP_EN | PDSS_CHGDET_CTRL_0_RDAT_LKG_DP_EN);
            break;

        case CHGB_SRC_TERM_SDP:
            pd->chgdet_ctrl_0 |= (PDSS_CHGDET_CTRL_0_RDP_PD_EN | PDSS_CHGDET_CTRL_0_RDM_PD_EN);
            break;

        case CHGB_SRC_TERM_CDP:
            pd->chgdet_ctrl_0 |= PDSS_CHGDET_CTRL_0_IDP_SNK_EN;
            break;

        default:
            status = CY_USBPD_STAT_BAD_PARAM;
            break;
    }
#else
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(chargerTerm);
    status = CY_USBPD_STAT_NOT_SUPPORTED;
#endif /* CY_IP */

    return status;
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_Phy_ConfigSnkTerm
****************************************************************************//**
*
* This function applies the desired sink terminations on D+/D- lines.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \param chargerTerm
* Sink Terminations to be applied.
*
* \return
* \ref cy_en_usbpd_status_t
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Bch_Phy_ConfigSnkTerm(cy_stc_usbpd_context_t *context,
        cy_en_usbpd_bch_snk_term_t chargerTerm)
{
    cy_en_usbpd_status_t status = CY_USBPD_STAT_SUCCESS;

#if defined(CY_IP_MXUSBPD)
    PPDSS_REGS_T pd = context->base ;

#ifdef CY_DEVICE_CCG6
    /* Connect the charger detect block to the D+/D- pins from the active side of the Type-C connector. */
    if ((context->dpmGetConfig())->polarity != 0u)
    {
        pd->dpdm_ctrl = (pd->dpdm_ctrl & ~((uint32_t)PDSS_DPDM_CTRL_DPDM_T_DPDM_MASK)) |
            (2UL << PDSS_DPDM_CTRL_DPDM_T_DPDM_POS);
    }
    else
    {
        pd->dpdm_ctrl = (pd->dpdm_ctrl & ~((uint32_t)PDSS_DPDM_CTRL_DPDM_T_DPDM_MASK)) |
            (8UL << PDSS_DPDM_CTRL_DPDM_T_DPDM_POS);
    }
    Cy_SysLib_DelayUs(10);
#endif /* CY_DEVICE_CCG6 */

    switch(chargerTerm)
    {
        case CHGB_SINK_TERM_SPD:
            /* Pull up on D+ */
            pd->bch_det_0_ctrl[0] |= PDSS_BCH_DET_0_CTRL_RDP_PU_EN;
            break;

        case CHGB_SINK_TERM_PCD:
            /* Connect VDP_SRC and IDM_SINK. */
            pd->bch_det_0_ctrl[0] |= (PDSS_BCH_DET_0_CTRL_VDP_SRC_EN | PDSS_BCH_DET_0_CTRL_IDM_SNK_EN);
            break;

        case CHGB_SINK_TERM_SCD:
            /* Connect VDM_SRC and IDP_SINK. */
            pd->bch_det_0_ctrl[0] |= (PDSS_BCH_DET_0_CTRL_VDM_SRC_EN | PDSS_BCH_DET_0_CTRL_IDP_SNK_EN);
            break;

#if (!QC_AFC_CHARGING_DISABLED)
        case CHGB_SINK_TERM_AFC:
        case CHGB_SINK_TERM_QC_5V:
            /* 0.6 on D+, D- HiZ */
            pd->bch_det_0_ctrl[0] |= PDSS_BCH_DET_0_CTRL_VDP_SRC_EN;
            /* Remove other terms */
            pd->bch_det_0_ctrl[0] &= ~ (PDSS_BCH_DET_0_CTRL_RDP_PU_EN |
                    PDSS_BCH_DET_0_CTRL_RDM_PU_EN |
                    PDSS_BCH_DET_0_CTRL_VDM_SRC_EN |
                    PDSS_BCH_DET_0_CTRL_IDM_SNK_EN);
            break;
        case CHGB_SINK_TERM_QC_9V:
            /* 3.3V on D+, 0.6 on D- */
            pd->bch_det_0_ctrl[0] |= PDSS_BCH_DET_0_CTRL_RDP_PU_EN | PDSS_BCH_DET_0_CTRL_VDM_SRC_EN ;

            /* Remove other terms */
            pd->bch_det_0_ctrl[0] &= ~ ( PDSS_BCH_DET_0_CTRL_RDM_PU_EN |
                                   PDSS_BCH_DET_0_CTRL_RDM_PD_EN |
                                   PDSS_BCH_DET_0_CTRL_VDP_SRC_EN |
                                   PDSS_BCH_DET_0_CTRL_IDM_SNK_EN);
            break;
        case CHGB_SINK_TERM_QC_12V:            /* 0.6 on D+, 0.6 on D- */
            pd->bch_det_0_ctrl[0] |= (PDSS_BCH_DET_0_CTRL_VDP_SRC_EN | PDSS_BCH_DET_0_CTRL_VDM_SRC_EN);

            /* Remove other terms */
            pd->bch_det_0_ctrl[0] &= ~ (PDSS_BCH_DET_0_CTRL_RDP_PU_EN |
                                    PDSS_BCH_DET_0_CTRL_RDM_PU_EN);
            break;
        case CHGB_SINK_TERM_QC_20V:
            /* 3.3V on D+, 3.3V on D- */
            pd->bch_det_0_ctrl[0] |= (PDSS_BCH_DET_0_CTRL_RDP_PU_EN | PDSS_BCH_DET_0_CTRL_RDM_PU_EN);

            /* Remove other terms */
            pd->bch_det_0_ctrl[0] &= ~ ( PDSS_BCH_DET_0_CTRL_VDM_SRC_EN |
                                    PDSS_BCH_DET_0_CTRL_VDP_SRC_EN);
            break;
#endif /* (!QC_AFC_CHARGING_DISABLED) */

        case CHGB_SINK_TERM_APPLE:
            (void)Cy_USBPD_Bch_Phy_RemoveTerm(context);
            break;

        default:
            status = CY_USBPD_STAT_BAD_PARAM;
            break;
    }

#elif defined(CY_IP_M0S8USBPD)

    PPDSS_REGS_T pd = context->base ;

    switch(chargerTerm)
    {
        case CHGB_SINK_TERM_SPD:
            pd->chgdet_ctrl_0 |= PDSS_CHGDET_CTRL_0_RDP_PU_EN;
            break;

        case CHGB_SINK_TERM_PCD:
            pd->chgdet_ctrl_0 |= PDSS_CHGDET_CTRL_0_VDP_SRC_EN | PDSS_CHGDET_CTRL_0_IDM_SNK_EN;
            break;

        case CHGB_SINK_TERM_SCD:
            pd->chgdet_ctrl_0 |= PDSS_CHGDET_CTRL_0_VDM_SRC_EN | PDSS_CHGDET_CTRL_0_IDP_SNK_EN;
            break;

        case CHGB_SINK_TERM_APPLE:
            (void)Cy_USBPD_Bch_Phy_RemoveTerm(context);
            break;

        default:
            status = CY_USBPD_STAT_BAD_PARAM;
            break;
    }
#else
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(chargerTerm);
    status = CY_USBPD_STAT_NOT_SUPPORTED;
#endif /* CY_IP */

    return status;
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_Phy_RemoveTerm
****************************************************************************//**
*
* This function removes all terminations/voltage sources from D+/D- lines.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* \ref cy_en_usbpd_status_t
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Bch_Phy_RemoveTerm(cy_stc_usbpd_context_t *context)
{
    cy_en_usbpd_status_t status = CY_USBPD_STAT_SUCCESS;

#if defined(CY_IP_MXUSBPD)
    PPDSS_REGS_T pd = context->base;

    pd->bch_det_0_ctrl[0] &= ~(PDSS_BCH_DET_0_CTRL_IDP_SNK_EN |
            PDSS_BCH_DET_0_CTRL_IDM_SNK_EN |
            PDSS_BCH_DET_0_CTRL_VDP_SRC_EN |
            PDSS_BCH_DET_0_CTRL_VDM_SRC_EN |
            PDSS_BCH_DET_0_CTRL_IDP_SRC_EN |
            PDSS_BCH_DET_0_CTRL_DCP_EN |
            PDSS_BCH_DET_0_CTRL_RDM_PD_EN |
            PDSS_BCH_DET_0_CTRL_RDM_PU_EN |
            PDSS_BCH_DET_0_CTRL_RDP_PD_EN |
            PDSS_BCH_DET_0_CTRL_RDP_PU_EN |
            PDSS_BCH_DET_0_CTRL_RDAT_LKG_DP_EN |
            PDSS_BCH_DET_0_CTRL_RDAT_LKG_DM_EN );
    pd->bch_det_1_ctrl[0] = PDSS_BCH_DET_1_CTRL_CHGDET_ISO_N;

#if (defined(CY_DEVICE_CCG6))
    /* Make sure D+/D- short is removed. */
    pd->dpdm_ctrl &= ~(PDSS_DPDM_CTRL_DCP_SHORT_DPDM_BOTTOM | PDSS_DPDM_CTRL_DCP_SHORT_DPDM_BOTTOM);
#endif /* (defined(CY_DEVICE_CCG6)) */

#elif defined(CY_IP_M0S8USBPD)
    PPDSS_REGS_T pd = context->base;

    pd->chgdet_ctrl_0 &= ~(PDSS_CHGDET_CTRL_0_IDP_SNK_EN |
            PDSS_CHGDET_CTRL_0_IDM_SNK_EN |
            PDSS_CHGDET_CTRL_0_VDP_SRC_EN |
            PDSS_CHGDET_CTRL_0_VDM_SRC_EN |
            PDSS_CHGDET_CTRL_0_IDP_SRC_EN |
            PDSS_CHGDET_CTRL_0_DCP_EN |
            PDSS_CHGDET_CTRL_0_RDM_PD_EN |
            PDSS_CHGDET_CTRL_0_RDM_PU_EN |
            PDSS_CHGDET_CTRL_0_RDP_PD_EN |
            PDSS_CHGDET_CTRL_0_RDP_PU_EN |
            PDSS_CHGDET_CTRL_0_RDAT_LKG_DP_EN |
            PDSS_CHGDET_CTRL_0_RDAT_LKG_DM_EN);
    pd->chgdet_ctrl_1 &= ~(PDSS_CHGDET_CTRL_1_CHGDET_APP_DET |
            PDSS_CHGDET_CTRL_1_CHGDET_APPLE_MODE_DM_MASK |
            PDSS_CHGDET_CTRL_1_CHGDET_APPLE_MODE_DP_MASK);
#else
    CY_UNUSED_PARAMETER(context);
    status = CY_USBPD_STAT_NOT_SUPPORTED;
#endif /* CY_IP */

    return status;
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_Phy_Config_Comp
****************************************************************************//**
*
* This function configures and enables one of the charger detect comparators.
* This can be used for a quick comparator state check or to configure the PHY to
* provide an interrupt callback when the comparator state changes
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \param compIdx
* Comparator index(0 or 1).
*
* \param pInput
* Positive input signal selection
*
* \param mInput
* Negative input signal selection
*
* \param vref
* Vref value to be used when either input selection is vref.
*
* \param edge
* Edge selection. If edge is selected, then callback registered in \ref
* Cy_USBPD_Bch_Phy_Init will be called when event fires. If edge is no
* interrupt, then this function will revert the comparator to its previous
* settings after comparison is done.
*
* \return Comparator output value.
*
*******************************************************************************/
bool Cy_USBPD_Bch_Phy_Config_Comp(cy_stc_usbpd_context_t *context,
                                  uint8_t compIdx,
                                  cy_en_usbpd_bch_comp_pinput_t pInput,
                                  cy_en_usbpd_bch_comp_ninput_t mInput,
                                  cy_en_usbpd_bch_vref_t vref,
                                  cy_en_usbpd_bch_comp_edge_t edge)
{
#if defined(CY_IP_MXUSBPD)
    PPDSS_REGS_T pd = context->base ;
    uint32_t volatile *bch_ctrl_p;
    uint32_t volatile *cfg_bch_det_p;
    uint32_t regVal;
    uint32_t temp_bch_ctrl;
    uint32_t temp_intr_cfg;
    uint32_t intr_mask = (uint32_t)(BCH_PORT_0_CMP1_INTR_MASK << compIdx);
    uint32_t cfg_clr_mask;
    uint32_t cfg_pos;
    uint32_t intr_state;
    uint32_t filt_bypass_bit_mask;
    uint32_t cfg_set_mask;
    bool result = false;

    intr_state = Cy_SysLib_EnterCriticalSection();

    cfg_bch_det_p = &pd->intr9_cfg_bch_det[0];

    if (compIdx == 0u)
    {
        bch_ctrl_p = &pd->bch_det_0_ctrl[0];
        cfg_clr_mask = PDSS_INTR9_CFG_BCH_DET_BCH_DET_COMP0_FILT_EN |
            PDSS_INTR9_CFG_BCH_DET_BCH_DET_COMP0_CFG_MASK |
            PDSS_INTR9_CFG_BCH_DET_BCH_DET_COMP0_FILT_SEL_MASK |
            PDSS_INTR9_CFG_BCH_DET_BCH_DET_COMP0_FILT_RESET;
        filt_bypass_bit_mask = PDSS_INTR9_CFG_BCH_DET_BCH_DET_COMP0_FILT_BYPASS;
        cfg_set_mask = PDSS_INTR9_CFG_BCH_DET_BCH_DET_COMP0_FILT_EN |
            (2UL << PDSS_INTR9_CFG_BCH_DET_BCH_DET_COMP0_FILT_SEL_POS);

        if (edge == CHGB_COMP_EDGE_FALLING)
        {
            cfg_set_mask |= PDSS_INTR9_CFG_BCH_DET_BCH_DET_COMP0_FILT_RESET;
        }
        cfg_pos = PDSS_INTR9_CFG_BCH_DET_BCH_DET_COMP0_CFG_POS;

    }
    else
    {
        bch_ctrl_p = &pd->bch_det_1_ctrl[0];
        cfg_clr_mask = PDSS_INTR9_CFG_BCH_DET_BCH_DET_COMP1_FILT_EN  |
                   PDSS_INTR9_CFG_BCH_DET_BCH_DET_COMP1_CFG_MASK |
                   PDSS_INTR9_CFG_BCH_DET_BCH_DET_COMP1_FILT_SEL_MASK |
                   PDSS_INTR9_CFG_BCH_DET_BCH_DET_COMP1_FILT_RESET;
        filt_bypass_bit_mask = PDSS_INTR9_CFG_BCH_DET_BCH_DET_COMP1_FILT_BYPASS;
        cfg_set_mask = PDSS_INTR9_CFG_BCH_DET_BCH_DET_COMP1_FILT_EN |
            (2UL << PDSS_INTR9_CFG_BCH_DET_BCH_DET_COMP1_FILT_SEL_POS);

        if (edge == CHGB_COMP_EDGE_FALLING)
        {
            cfg_set_mask |= PDSS_INTR9_CFG_BCH_DET_BCH_DET_COMP1_FILT_RESET;
        }
        cfg_pos = PDSS_INTR9_CFG_BCH_DET_BCH_DET_COMP1_CFG_POS;
    }

    temp_bch_ctrl = *bch_ctrl_p;
    temp_intr_cfg = *cfg_bch_det_p;

    regVal = temp_bch_ctrl;
    regVal &= ~(PDSS_BCH_DET_0_CTRL_CMP1_INN_SEL_MASK |
                PDSS_BCH_DET_0_CTRL_CMP1_INP_SEL_MASK |
                PDSS_BCH_DET_0_CTRL_CMP1_VREF_SEL_MASK |
                PDSS_BCH_DET_0_CTRL_CMP1_OFFSET_SEL_MASK |
                PDSS_BCH_DET_0_CTRL_CMP1_OFFSET_EN);
    regVal |= (((uint32_t)mInput << PDSS_BCH_DET_0_CTRL_CMP1_INN_SEL_POS) |
               ((uint32_t)pInput << PDSS_BCH_DET_0_CTRL_CMP1_INP_SEL_POS) |
               ((uint32_t)vref << PDSS_BCH_DET_0_CTRL_CMP1_VREF_SEL_POS) |
               (uint32_t)PDSS_BCH_DET_0_CTRL_EN_COMP1_CHGDET);

    if (vref == CHGB_VREF_0_325V)
    {
        /* The actual voltage needs to be 425 mV. Enable +100mV offset to get the desired voltage. */
        regVal |= ((4UL << PDSS_BCH_DET_0_CTRL_CMP1_OFFSET_SEL_POS) |
                (uint32_t)PDSS_BCH_DET_0_CTRL_CMP1_OFFSET_EN);
    }

    /* Enable comparator */
    *bch_ctrl_p = regVal;
    Cy_SysLib_DelayUs(10);

    if ((pd->intr9_status_0 & intr_mask) != 0u)
    {
        result = true;
    }

    /* Enable Interrupt and check if condition already exists */
    if(edge == CHGB_COMP_NO_INTR)
    {
        *bch_ctrl_p = temp_bch_ctrl;
        *cfg_bch_det_p &= ~cfg_clr_mask;
        *cfg_bch_det_p = temp_intr_cfg;
        Cy_SysLib_DelayUs(10);
    }
    else
    {
        /* Configure edge detection. */
        *cfg_bch_det_p &= ~(cfg_clr_mask | filt_bypass_bit_mask);
        *cfg_bch_det_p |= ((uint32_t)edge << cfg_pos);
        *cfg_bch_det_p |= cfg_set_mask;

        /* Enable comparator interrupts. */
        pd->intr9_mask |= intr_mask;
    }

    /* Clear comparator interrupt. */
    pd->intr9 = intr_mask;

    Cy_SysLib_ExitCriticalSection(intr_state);
    return result;

#elif defined(CY_IP_M0S8USBPD)
    PPDSS_REGS_T pd = context->base ;
    bool result = false;
    uint32_t intr_state, regVal;

    CY_UNUSED_PARAMETER(compIdx);

    intr_state = Cy_SysLib_EnterCriticalSection();

    /* Disable all comparator interrupt configuration. */
    pd->intr3_cfg_0 &= ~(PDSS_INTR3_CFG_0_CHGDET_FILT_SEL_MASK |
            PDSS_INTR3_CFG_0_CHGDET_FILT_EN |
            PDSS_INTR3_CFG_0_CHGDET_CFG_MASK);

    pd->chgdet_ctrl_0 &= ~(PDSS_CHGDET_CTRL_0_CMP_INN_SEL_MASK |
            PDSS_CHGDET_CTRL_0_CMP_INP_SEL_MASK |
            PDSS_CHGDET_CTRL_0_VREF_SEL_MASK |
            PDSS_CHGDET_CTRL_0_CMP_OFFSET_SEL_MASK |
            PDSS_CHGDET_CTRL_0_CMPP_OFFSET_EN |
            PDSS_CHGDET_CTRL_0_EN_COMP_CHGDET);

    /* Clear any previous comparator interrupts. */
    pd->intr3 = PDSS_INTR3_CHGDET_CHANGED;

    /* Configure the comparator inputs. */
    regVal = pd->chgdet_ctrl_0;
    regVal |= (((uint32_t)mInput << PDSS_CHGDET_CTRL_0_CMP_INN_SEL_POS) |
            ((uint32_t)pInput << PDSS_CHGDET_CTRL_0_CMP_INP_SEL_POS) |
            ((uint32_t)vref << PDSS_CHGDET_CTRL_0_VREF_SEL_POS) |
            (uint32_t)PDSS_CHGDET_CTRL_0_EN_COMP_CHGDET);
    if (vref == CHGB_VREF_0_325V)
    {
        regVal |= ((5UL << PDSS_CHGDET_CTRL_0_CMP_OFFSET_SEL_POS) |
                (uint32_t)PDSS_CHGDET_CTRL_0_CMPP_OFFSET_EN);
    }

    /* Enable the comparator and apply a delay for checking. */
    pd->chgdet_ctrl_0 = regVal;
    Cy_SysLib_DelayUs(10);

    if ((pd->ncell_status & PDSS_NCELL_STATUS_CHGDET_STATUS) != 0u)
    {
        result = true;
    }

    if (edge != CHGB_COMP_NO_INTR)
    {
        /* Enable charger detect comparator interrupt. */
        pd->intr3_mask |= PDSS_INTR3_MASK_CHGDET_CHANGED_MASK;

        /* Enable detection of desired edge with 2 LF cycles of filtering. */
        pd->intr3_cfg_0 |= (PDSS_INTR3_CFG_0_CHGDET_FILT_EN |
                (2UL << PDSS_INTR3_CFG_0_CHGDET_FILT_SEL_POS) |
                ((uint32_t)edge << PDSS_INTR3_CFG_0_CHGDET_CFG_POS));
    }

    Cy_SysLib_ExitCriticalSection(intr_state);
    return result;

#else
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(compIdx);
    CY_UNUSED_PARAMETER(pInput);
    CY_UNUSED_PARAMETER(mInput);
    CY_UNUSED_PARAMETER(vref);
    CY_UNUSED_PARAMETER(edge);

    return false;
#endif /* CY_IP */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_Phy_DisableComp
****************************************************************************//**
*
* This function disables one of the charger detect comparators.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \param compIdx
* Comparator index.
*
* \return
* \ref cy_en_usbpd_status_t
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Bch_Phy_DisableComp(cy_stc_usbpd_context_t *context,
        uint8_t compIdx )
{
#if defined(CY_IP_MXUSBPD)
    PPDSS_REGS_T pd = context->base;

    if(compIdx == 0u)
    {
        pd->bch_det_0_ctrl[0] &= ~PDSS_BCH_DET_0_CTRL_EN_COMP1_CHGDET;
        pd->intr9_mask &= ~(BCH_PORT_0_CMP1_INTR_MASK);
        pd->intr9 = BCH_PORT_0_CMP1_INTR_MASK;
        pd->intr9_cfg_bch_det[0] &= ~PDSS_INTR9_CFG_BCH_DET_BCH_DET_COMP0_FILT_EN;
    }
    else
    {
        pd->bch_det_1_ctrl[0] &= ~PDSS_BCH_DET_1_CTRL_EN_COMP2_CHGDET;
        pd->intr9_mask &= ~(BCH_PORT_0_CMP2_INTR_MASK);
        pd->intr9 = BCH_PORT_0_CMP2_INTR_MASK;
        pd->intr9_cfg_bch_det[0] &= ~PDSS_INTR9_CFG_BCH_DET_BCH_DET_COMP1_FILT_EN;
    }
#elif defined(CY_IP_M0S8USBPD)
    PPDSS_REGS_T pd = context->base;

    CY_UNUSED_PARAMETER(compIdx);
    pd->chgdet_ctrl_0 &= ~(PDSS_CHGDET_CTRL_0_EN_COMP_CHGDET);
    pd->intr3          = PDSS_INTR3_CHGDET_CHANGED;
    pd->intr3_mask    &= ~(PDSS_INTR3_MASK_CHGDET_CHANGED_MASK);
    pd->intr3_cfg_0   &= ~(PDSS_INTR3_CFG_0_CHGDET_FILT_EN);
#else
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(compIdx);
#endif /* CY_IP */

    return CY_USBPD_STAT_SUCCESS;
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_Phy_Isolate_DpDm
****************************************************************************//**
*
* This function isolates the charge detect block from D+/D- lines.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* \ref cy_en_usbpd_status_t
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Bch_Phy_Isolate_DpDm(cy_stc_usbpd_context_t *context)
{
#if (defined(CY_DEVICE_CCG6))
    PPDSS_REGS_T pd = context->base ;

    /* Isolate charge detect block from DP and DM lines */
    pd->dpdm_ctrl &= ~((uint32_t)PDSS_DPDM_CTRL_DPDM_T_DPDM_MASK);

    return CY_USBPD_STAT_SUCCESS;
#else
    CY_UNUSED_PARAMETER(context);
    return CY_USBPD_STAT_NOT_SUPPORTED;
#endif /* (defined(CY_DEVICE_CCG6)) */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_Phy_CompResult
****************************************************************************//**
*
* This function checks and returns the comparator output.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \param compIdx
* Comparator index.
*
* \return
* Comparator Output
*
*******************************************************************************/
bool Cy_USBPD_Bch_Phy_CompResult(cy_stc_usbpd_context_t *context, uint8_t compIdx)
{
    bool retVal = false;

#if defined(CY_IP_MXUSBPD)
    PPDSS_REGS_T pd = context->base ;

    if(compIdx == 0u)
    {
        if ((pd->intr9_status_0 & (uint32_t)(BCH_PORT_0_CMP1_INTR_MASK)) != 0u)
        {
            retVal = true;
        }
    }
    else
    {
        if ((pd->intr9_status_0 & (uint32_t)(BCH_PORT_0_CMP2_INTR_MASK)) != 0u)
        {
            retVal = true;
        }
    }
#elif defined(CY_IP_M0S8USBPD)
    PPDSS_REGS_T pd = context->base ;

    CY_UNUSED_PARAMETER(compIdx);
    if ((pd->ncell_status & PDSS_NCELL_STATUS_CHGDET_STATUS) != 0u)
    {
        retVal = true;
    }
#else
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(compIdx);
#endif /* CY_IP */

    return retVal;
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_ApplyDpPd
****************************************************************************//**
*
* This function applies pulldown on D+.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* \ref cy_en_usbpd_status_t
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Bch_ApplyDpPd(cy_stc_usbpd_context_t *context)
{
#if defined(CY_IP_MXUSBPD)
    context->base->bch_det_0_ctrl[0] |= PDSS_BCH_DET_0_CTRL_RDP_PD_EN;
#elif defined(CY_IP_M0S8USBPD)
    context->base->chgdet_ctrl_0 |= PDSS_CHGDET_CTRL_0_RDP_PD_EN;
#else
    CY_UNUSED_PARAMETER(context);
#endif /* CY_IP */

    return CY_USBPD_STAT_SUCCESS;
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_RemoveDpPd
****************************************************************************//**
*
* This function removes pulldown on D+.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* \ref cy_en_usbpd_status_t
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Bch_RemoveDpPd(cy_stc_usbpd_context_t *context)
{
#if defined(CY_IP_MXUSBPD)
    context->base->bch_det_0_ctrl[0] &= ~PDSS_BCH_DET_0_CTRL_RDP_PD_EN;
#elif defined(CY_IP_M0S8USBPD)
    context->base->chgdet_ctrl_0 &= ~PDSS_CHGDET_CTRL_0_RDP_PD_EN;
#else
    CY_UNUSED_PARAMETER(context);
#endif /* CY_IP */

    return CY_USBPD_STAT_SUCCESS;
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_ApplyRdatLkgDp
****************************************************************************//**
*
* This function applies RDAT_LKG resistance on D+.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
*  None
*
*******************************************************************************/
void Cy_USBPD_Bch_ApplyRdatLkgDp(cy_stc_usbpd_context_t *context)
{
#if defined(CY_IP_MXUSBPD)
    context->base->bch_det_0_ctrl[0] |= PDSS_BCH_DET_0_CTRL_RDAT_LKG_DP_EN;
#elif defined(CY_IP_M0S8USBPD)
    context->base->chgdet_ctrl_0 |= PDSS_CHGDET_CTRL_0_RDAT_LKG_DP_EN;
#else
    CY_UNUSED_PARAMETER(context);
#endif /* CY_IP */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_ApplyRdatLkgDm
****************************************************************************//**
*
* This function applies RDAT_LKG resistance on D-.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_Bch_ApplyRdatLkgDm(cy_stc_usbpd_context_t *context)
{
#if defined(CY_IP_MXUSBPD)
    context->base->bch_det_0_ctrl[0] |= PDSS_BCH_DET_0_CTRL_RDAT_LKG_DM_EN;
#elif defined(CY_IP_M0S8USBPD)
    context->base->chgdet_ctrl_0 |= PDSS_CHGDET_CTRL_0_RDAT_LKG_DM_EN;
#else
    CY_UNUSED_PARAMETER(context);
#endif /* CY_IP */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_RemoveRdatLkgDp
****************************************************************************//**
*
* This function removes RDAT_LKG resistance on D+.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
*  None
*
*******************************************************************************/
void Cy_USBPD_Bch_RemoveRdatLkgDp(cy_stc_usbpd_context_t *context)
{
#if defined(CY_IP_MXUSBPD)
    context->base->bch_det_0_ctrl[0] &= ~PDSS_BCH_DET_0_CTRL_RDAT_LKG_DP_EN;
#elif defined(CY_IP_M0S8USBPD)
    context->base->chgdet_ctrl_0 &= ~PDSS_CHGDET_CTRL_0_RDAT_LKG_DP_EN;
#else
    CY_UNUSED_PARAMETER(context);
#endif /* CY_IP */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_RemoveRdatLkgDm
****************************************************************************//**
*
* This function removes RDAT_LKG resistance on D-.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
*  None
*
*******************************************************************************/
void Cy_USBPD_Bch_RemoveRdatLkgDm(cy_stc_usbpd_context_t *context)
{
#if defined(CY_IP_MXUSBPD)
    context->base->bch_det_0_ctrl[0] &= ~PDSS_BCH_DET_0_CTRL_RDAT_LKG_DM_EN;
#elif defined(CY_IP_M0S8USBPD)
    context->base->chgdet_ctrl_0 &= ~~PDSS_CHGDET_CTRL_0_RDAT_LKG_DM_EN;
#else
    CY_UNUSED_PARAMETER(context);
#endif /* CY_IP */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_Apply_AppleTermDp
* **************************************************************************//**
*
* This function applies Apple source terminations on D+.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \param appleTermId
* Apple termination ID
*
* \return
*  None
*
*******************************************************************************/
void Cy_USBPD_Bch_Apply_AppleTermDp(cy_stc_usbpd_context_t *context,
                                       cy_en_usbpd_bch_src_term_t appleTermId)
{
#if defined(CY_IP_MXUSBPD)
    PPDSS_REGS_T pd = context->base ;

    switch(appleTermId)
    {
        case CHGB_SRC_TERM_APPLE_1A:
            pd->bch_det_1_ctrl[0] |= (uint32_t)(1UL << PDSS_BCH_DET_1_CTRL_CHGDET_APPLE_MODE_DP_POS);
            break;
        case CHGB_SRC_TERM_APPLE_2_1A:
            pd->bch_det_1_ctrl[0] |= (uint32_t)(2UL << PDSS_BCH_DET_1_CTRL_CHGDET_APPLE_MODE_DP_POS);
            break;
        case CHGB_SRC_TERM_APPLE_2_4A:
            pd->bch_det_1_ctrl[0] |= (uint32_t)(2UL << PDSS_BCH_DET_1_CTRL_CHGDET_APPLE_MODE_DP_POS);
            break;
        default:
            Cy_SysLib_DelayUs (1);
            break;
    }
#elif defined(CY_IP_M0S8USBPD)
    PPDSS_REGS_T pd = context->base ;

    switch(appleTermId)
    {
        case CHGB_SRC_TERM_APPLE_1A:
            pd->chgdet_ctrl_1 |= (uint32_t)(1UL << PDSS_CHGDET_CTRL_1_CHGDET_APPLE_MODE_DP_POS);
            break;
        case CHGB_SRC_TERM_APPLE_2_1A:
            pd->chgdet_ctrl_1 |= (uint32_t)(2UL << PDSS_CHGDET_CTRL_1_CHGDET_APPLE_MODE_DP_POS);
            break;
        case CHGB_SRC_TERM_APPLE_2_4A:
            pd->chgdet_ctrl_1 |= (uint32_t)(2UL << PDSS_CHGDET_CTRL_1_CHGDET_APPLE_MODE_DP_POS);
            break;
        default:
            Cy_SysLib_DelayUs (1);
            break;
    }
#else
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(appleTermId);
#endif /* CY_IP */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_Apply_AppleTermDm
* **************************************************************************//**
*
* This function applies Apple source terminations on D-.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \param appleTermId
* Apple termination ID
*
* \return
*  None
*
*******************************************************************************/
void Cy_USBPD_Bch_Apply_AppleTermDm(cy_stc_usbpd_context_t *context,
                                       cy_en_usbpd_bch_src_term_t appleTermId)
{
#if defined(CY_IP_MXUSBPD)
    PPDSS_REGS_T pd = context->base ;

    switch(appleTermId)
    {
        case CHGB_SRC_TERM_APPLE_1A:
            pd->bch_det_1_ctrl[0] |= (2u << PDSS_BCH_DET_1_CTRL_CHGDET_APPLE_MODE_DM_POS);
            break;
        case CHGB_SRC_TERM_APPLE_2_1A:
            pd->bch_det_1_ctrl[0] |= (1u << PDSS_BCH_DET_1_CTRL_CHGDET_APPLE_MODE_DM_POS);
            break;
        case CHGB_SRC_TERM_APPLE_2_4A:
            pd->bch_det_1_ctrl[0] |= (2u << PDSS_BCH_DET_1_CTRL_CHGDET_APPLE_MODE_DM_POS);
            break;
        default:
            Cy_SysLib_DelayUs (1);
            break;
    }
#elif defined(CY_IP_M0S8USBPD)
    PPDSS_REGS_T pd = context->base ;

    switch(appleTermId)
    {
        case CHGB_SRC_TERM_APPLE_1A:
            pd->chgdet_ctrl_1 |= (2u << PDSS_CHGDET_CTRL_1_CHGDET_APPLE_MODE_DM_POS);
            break;
        case CHGB_SRC_TERM_APPLE_2_1A:
            pd->chgdet_ctrl_1 |= (1u << PDSS_CHGDET_CTRL_1_CHGDET_APPLE_MODE_DM_POS);
            break;
        case CHGB_SRC_TERM_APPLE_2_4A:
            pd->chgdet_ctrl_1 |= (2u << PDSS_CHGDET_CTRL_1_CHGDET_APPLE_MODE_DM_POS);
            break;
        default:
            Cy_SysLib_DelayUs (1);
            break;
    }
#else
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(appleTermId);
#endif /* CY_IP */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_Apply_VdmSrc
****************************************************************************//**
*
* This function applies VDM_SRC on DM.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
*  None
*
*******************************************************************************/
void Cy_USBPD_Bch_Apply_VdmSrc(cy_stc_usbpd_context_t *context)
{
#if defined(CY_IP_MXUSBPD)
    context->base->bch_det_0_ctrl[0] |= PDSS_BCH_DET_0_CTRL_VDM_SRC_EN;
#elif defined(CY_IP_M0S8USBPD)
    context->base->chgdet_ctrl_0 |= PDSS_CHGDET_CTRL_0_VDM_SRC_EN;
#else
    CY_UNUSED_PARAMETER(context);
#endif /* CY_IP */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_Remove_VdmSrc
****************************************************************************//**
*
* This function removes VDM_SRC on DM.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
*  None
*
*******************************************************************************/
void Cy_USBPD_Bch_Remove_VdmSrc(cy_stc_usbpd_context_t *context)
{
#if defined(CY_IP_MXUSBPD)
    context->base->bch_det_0_ctrl[0] &= ~PDSS_BCH_DET_0_CTRL_VDM_SRC_EN;
#elif defined(CY_IP_M0S8USBPD)
    context->base->chgdet_ctrl_0 &= ~PDSS_CHGDET_CTRL_0_VDM_SRC_EN;
#else
    CY_UNUSED_PARAMETER(context);
#endif /* CY_IP */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_Is_VdmSrc_On
****************************************************************************//**
*
* Function to check whether VDM_SRC supply is enabled.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* True if VDM_SRC supply is ON, false otherwise.
*
*******************************************************************************/
bool Cy_USBPD_Bch_Is_VdmSrc_On(cy_stc_usbpd_context_t *context)
{
#if defined(CY_IP_MXUSBPD)
    return ((bool)((context->base->bch_det_0_ctrl[0] & PDSS_BCH_DET_0_CTRL_VDM_SRC_EN) != 0UL));
#elif defined(CY_IP_M0S8USBPD)
    return ((bool)((context->base->chgdet_ctrl_0 & PDSS_CHGDET_CTRL_0_VDM_SRC_EN) != 0UL));
#else
    CY_UNUSED_PARAMETER(context);
    return false;
#endif /* CY_IP */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_Remove_AppleSrcDp
****************************************************************************//**
*
* This function removes Apple source terminations from D+
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
*  None
*
*******************************************************************************/
void Cy_USBPD_Bch_Remove_AppleSrcDp(cy_stc_usbpd_context_t *context)
{
#if defined(CY_IP_MXUSBPD)
    context->base->bch_det_1_ctrl[0] &= ~(PDSS_BCH_DET_1_CTRL_CHGDET_APPLE_MODE_DP_MASK);
#elif defined(CY_IP_M0S8USBPD)
    context->base->chgdet_ctrl_1 &= ~(PDSS_CHGDET_CTRL_1_CHGDET_APPLE_MODE_DP_MASK);
#else
    CY_UNUSED_PARAMETER(context);
#endif /* CY_IP */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_Remove_AppleSrcDm
****************************************************************************//**
*
* This function removes Apple source terminations from D-
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_Bch_Remove_AppleSrcDm(cy_stc_usbpd_context_t *context)
{
#if defined(CY_IP_MXUSBPD)
    context->base->bch_det_1_ctrl[0] &= ~(PDSS_BCH_DET_1_CTRL_CHGDET_APPLE_MODE_DM_MASK);
#elif defined(CY_IP_M0S8USBPD)
    context->base->chgdet_ctrl_1 &= ~(PDSS_CHGDET_CTRL_1_CHGDET_APPLE_MODE_DM_MASK);
#else
    CY_UNUSED_PARAMETER(context);
#endif /* CY_IP */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_Enable_AppleDet
****************************************************************************//**
*
* This function enables Apple Brick ID detection.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
*  None
*
*******************************************************************************/
void Cy_USBPD_Bch_Enable_AppleDet(cy_stc_usbpd_context_t *context)
{
#if defined(CY_IP_MXUSBPD)
    context->base->bch_det_1_ctrl[0] |= PDSS_BCH_DET_1_CTRL_CHGDET_APP_DET;
#elif defined(CY_IP_M0S8USBPD)
    context->base->chgdet_ctrl_1 |= PDSS_CHGDET_CTRL_1_CHGDET_APP_DET;
#else
    CY_UNUSED_PARAMETER(context);
#endif /* CY_IP */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_Disable_AppleDet
****************************************************************************//**
*
* This function disables Apple Brick ID detection.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_Bch_Disable_AppleDet(cy_stc_usbpd_context_t *context)
{
#if defined(CY_IP_MXUSBPD)
    context->base->bch_det_1_ctrl[0] &= ~PDSS_BCH_DET_1_CTRL_CHGDET_APP_DET;
#elif defined(CY_IP_M0S8USBPD)
    context->base->chgdet_ctrl_1 &= ~PDSS_CHGDET_CTRL_1_CHGDET_APP_DET;
#else
    CY_UNUSED_PARAMETER(context);
#endif /* CY_IP */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_Phy_DpStat
****************************************************************************//**
*
* This function checks status of DP line.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* True if DP is high, false otherwise.
*
*******************************************************************************/
bool Cy_USBPD_Bch_Phy_DpStat(cy_stc_usbpd_context_t *context)
{
#if defined(CY_IP_MXUSBPD)
    PPDSS_REGS_T pd = context->base ;
#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))
    uint8_t chgb_id = 0;
    if ((pd->intr9_status_1 & (1UL << (PDSS_INTR9_STATUS_1_QCOM_RCVR_DP_STATUS_POS + chgb_id))) != 0UL)
#elif (defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S))
    if ((pd->intr9_status_1 & (PDSS_INTR9_STATUS_1_QCOM_RCVR_DP_STATUS)) != 0u)
#endif /* CCGx */
    {
        return true;
    }
    else
    {
        return false;
    }
#else
    CY_UNUSED_PARAMETER(context);
    return false;
#endif /* CY_IP */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_Phy_DmStat
****************************************************************************//**
*
* This function checks status of DM line.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* True if DM is high, false otherwise.
*
*******************************************************************************/
bool Cy_USBPD_Bch_Phy_DmStat(cy_stc_usbpd_context_t *context)
{
#if defined(CY_IP_MXUSBPD)
    PPDSS_REGS_T pd = context->base ;
#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))
    uint8_t chgb_id = 0;
    if ((pd->intr9_status_1 & (1UL << (PDSS_INTR9_STATUS_1_QCOM_RCVR_DM_STATUS_POS + chgb_id))) != 0UL)
#elif (defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S))
     if ((pd->intr9_status_1 & (PDSS_INTR9_STATUS_1_QCOM_RCVR_DM_STATUS)) != 0u)
#endif /* CCGx */
    {
        return true;
    }
    else
    {
        return false;
    }
#else
    CY_UNUSED_PARAMETER(context);
    return false;
#endif /* CY_IP */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_Phy_Config_DeepSleep
****************************************************************************//**
*
* This function configures the charger detect block for deepsleep entry.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_Bch_Phy_Config_DeepSleep(cy_stc_usbpd_context_t *context)
{
#if defined(CY_IP_MXUSBPD)
    PPDSS_REGS_T pd = context->base ;

    /* Configuring both edge detection. */
    pd->intr9_cfg_bch_det[0] |= (PDSS_INTR9_CFG_BCH_DET_QCOM_RCVR_DM_CFG_MASK |
            PDSS_INTR9_CFG_BCH_DET_QCOM_RCVR_DP_CFG_MASK);

#if (defined(CY_DEVICE_CCG3PA))
    pd->intr9_mask |= ((1UL << PDSS_INTR9_QCOM_RCVR_DM_CHANGED_POS) |
                (1UL << PDSS_INTR9_QCOM_RCVR_DP_CHANGED_POS));
#else
    pd->intr9_mask |= (PDSS_INTR9_QCOM_RCVR_DM_CHANGED |
            PDSS_INTR9_QCOM_RCVR_DP_CHANGED);
#endif /* (defined(CY_DEVICE_CCG3PA)) */

#else
    CY_UNUSED_PARAMETER(context);
#endif /* CY_IP */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_Phy_Config_Wakeup
****************************************************************************//**
*
* This function configures the charger detect block after deepsleep exit.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_Bch_Phy_Config_Wakeup(cy_stc_usbpd_context_t *context)
{
    /* Nothing to do. */
    (void)context;
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_QcSrcInit
****************************************************************************//**
*
* This function initialize QC source block.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* cy_en_usbpd_status_t
*
*******************************************************************************/    
cy_en_usbpd_status_t Cy_USBPD_Bch_QcSrcInit(cy_stc_usbpd_context_t *context)
{
    /* Nothing to do for now */
    CY_UNUSED_PARAMETER(context);
    return CY_USBPD_STAT_SUCCESS;
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_QcSrcStop
****************************************************************************//**
*
* This function deselect the QC source block.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* cy_en_usbpd_status_t
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Bch_QcSrcStop(cy_stc_usbpd_context_t *context)
{
    /* Nothing to do for now */
    CY_UNUSED_PARAMETER(context);
    return CY_USBPD_STAT_SUCCESS;
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_QcSrcMasterSenseEn
****************************************************************************//**
*
* This function enable master for hardware to start sensing the pulses on Dplus
* and Dminus.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* cy_en_usbpd_status_t
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Bch_QcSrcMasterSenseEn(cy_stc_usbpd_context_t *context)
{
#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)) 
    PPDSS_REGS_T pd = context->base;
    
#if defined(CY_DEVICE_PMG1S3)
    pd->qc3_chrger_ctrl |= PDSS_QC3_CHRGER_CTRL_QC3_0_CHG_EN;
#else
    pd->qc3_chrger_ctrl[0] |= PDSS_QC3_CHRGER_CTRL_QC3_0_CHG_EN;
#endif /* defined(CY_DEVICE_PMG1S3) */
  
    return CY_USBPD_STAT_SUCCESS;
    
#else
    CY_UNUSED_PARAMETER(context);
    return CY_USBPD_STAT_NOT_SUPPORTED;
#endif /* (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))  */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_QcSrcMasterSenseDis
****************************************************************************//**
*
* This function disable master for hardware to stop sensing the pulses on Dplus
* and Dminus.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* cy_en_usbpd_status_t
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Bch_QcSrcMasterSenseDis(cy_stc_usbpd_context_t *context)
{
#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)) 
    PPDSS_REGS_T pd = context->base;

#if defined(CY_DEVICE_PMG1S3)
    pd->qc3_chrger_ctrl &= ~PDSS_QC3_CHRGER_CTRL_QC3_0_CHG_EN;
#else
    pd->qc3_chrger_ctrl[0] &= ~PDSS_QC3_CHRGER_CTRL_QC3_0_CHG_EN;
#endif /* defined(CY_DEVICE_PMG1S3) */

    return CY_USBPD_STAT_SUCCESS;

#else
    CY_UNUSED_PARAMETER(context);
    return CY_USBPD_STAT_NOT_SUPPORTED;
#endif /* (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))  */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_QcSrcContModeStart
****************************************************************************//**
*
* This function configures the QC3 block, enable QC interrupts and enables 
* master for DP and DM sensing.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* cy_en_usbpd_status_t
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Bch_QcSrcContModeStart(cy_stc_usbpd_context_t *context)
{
#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)) 
    PPDSS_REGS_T pd = context->base;
    uint32_t regval;
    volatile uint32_t *qc3_chrger_ctrl = NULL;
    volatile uint32_t *afc_hs_filter_cfg = NULL;
    
    context->bcQcPulseCount = 0;
    
#if defined(CY_DEVICE_PMG1S3)
    qc3_chrger_ctrl = &pd->qc3_chrger_ctrl;
    afc_hs_filter_cfg = &pd->afc_hs_filter_cfg;
#else
    qc3_chrger_ctrl = &pd->qc3_chrger_ctrl[0];
    afc_hs_filter_cfg = &pd->afc_hs_filter_cfg[0];
#endif /* defined(CY_DEVICE_PMG1S3) */    
    
    /*
     * Enable pulse count and filter. DP filter should look for rising edge
     * and DM filter should look for falling edge.
     */
    *qc3_chrger_ctrl |= PDSS_QC3_CHRGER_CTRL_QC3_0_CHG_EN;

    /* Clear the DP/DM Count. */
    *qc3_chrger_ctrl |= PDSS_QC3_CHRGER_CTRL_READ_DPDM_COUNT;
    while (((*qc3_chrger_ctrl & PDSS_QC3_CHRGER_CTRL_READ_DPDM_COUNT) != 0u)) {}

    regval = *afc_hs_filter_cfg;

    regval &= ~(PDSS_AFC_HS_FILTER_CFG_DP_FILT_SEL_MASK |
        PDSS_AFC_HS_FILTER_CFG_DM_FILT_SEL_MASK | PDSS_AFC_HS_FILTER_CFG_DP_FILT_EN
        | PDSS_AFC_HS_FILTER_CFG_DM_FILT_EN |
        PDSS_AFC_HS_FILTER_CFG_DP_FILT_RESET);
    *afc_hs_filter_cfg = regval;

    regval |= ((QC3_DP_DM_PULSE_FILTER_CLOCK_SEL <<
        PDSS_AFC_HS_FILTER_CFG_DP_FILT_SEL_POS) | (QC3_DP_DM_PULSE_FILTER_CLOCK_SEL
        << PDSS_AFC_HS_FILTER_CFG_DM_FILT_SEL_POS) | PDSS_AFC_HS_FILTER_CFG_DM_FILT_RESET |
            (PDSS_AFC_HS_FILTER_CFG_DP_FILT_EN | PDSS_AFC_HS_FILTER_CFG_DM_FILT_EN));

    *afc_hs_filter_cfg = regval;

    /* Enable D+/D- pulse interrupts */
    pd->intr6 = QC3_PORT_0_DP_DM_PULSE_MASK;
    pd->intr6_mask |= QC3_PORT_0_DP_DM_PULSE_MASK;
  
    return CY_USBPD_STAT_SUCCESS;

#else
    CY_UNUSED_PARAMETER(context);
    return CY_USBPD_STAT_NOT_SUPPORTED;
#endif /* (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))  */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_QcSrcContModeStop
****************************************************************************//**
*
* This function disables QC3 block and interrupts, and also disables the DP/DM
* master sensing.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* cy_en_usbpd_status_t
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Bch_QcSrcContModeStop(cy_stc_usbpd_context_t *context)
{
#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)) 
    PPDSS_REGS_T pd = context->base;

    context->bcQcPulseCount = 0;
#if defined(CY_DEVICE_PMG1S3)
    pd->qc3_chrger_ctrl &= ~PDSS_QC3_CHRGER_CTRL_QC3_0_CHG_EN;
#else
    pd->qc3_chrger_ctrl[0] &= ~PDSS_QC3_CHRGER_CTRL_QC3_0_CHG_EN;
#endif /* defined(CY_DEVICE_PMG1S3) */

    /* Disable D+/D- pulse interrupts */
    pd->intr6_mask &= ~(QC3_PORT_0_DP_DM_PULSE_MASK);
    pd->intr6 = QC3_PORT_0_DP_DM_PULSE_MASK;

    return CY_USBPD_STAT_SUCCESS;

#else
    CY_UNUSED_PARAMETER(context);
    return CY_USBPD_STAT_NOT_SUPPORTED;
#endif /* (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))  */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_AfcSrcInit
****************************************************************************//**
*
* This function enables the AFC source clock, filters and other configurations.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* cy_en_usbpd_status_t
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Bch_AfcSrcInit(cy_stc_usbpd_context_t *context)
{
#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)) 
    PPDSS_REGS_T pd = context->base;
    volatile uint32_t *afc_1_ctrl = NULL;
    volatile uint32_t *afc_2_ctrl = NULL;
    volatile uint32_t *afc_hs_filter_cfg = NULL;
    uint32_t regval = 0;
    
#if defined(CY_DEVICE_PMG1S3)
    afc_1_ctrl = &pd->afc_1_ctrl;
    afc_2_ctrl = &pd->afc_2_ctrl;
    afc_hs_filter_cfg = &pd->afc_hs_filter_cfg;
#else
    afc_1_ctrl = &pd->afc_1_ctrl[0];
    afc_2_ctrl = &pd->afc_2_ctrl[0];
    afc_hs_filter_cfg = &pd->afc_hs_filter_cfg[0];
#endif /* defined(CY_DEVICE_PMG1S3) */
    
    regval = *afc_1_ctrl;

    regval &= ~(PDSS_AFC_1_CTRL_TX_RESET |
            PDSS_AFC_1_CTRL_NO_OF_BYTES_TX_MASK);
    regval |= PDSS_AFC_1_CTRL_UPDATE_TXCLK;

    *afc_1_ctrl = regval;


    regval = *afc_2_ctrl;
    /* Update UI clock cycle count for 125kHz clock. */
    regval &= ~PDSS_AFC_2_CTRL_UI_COUNT_MASK;
    regval |= (QC3_DP_DM_PULSE_FILTER_CLOCK_SEL <<
        PDSS_AFC_2_CTRL_UI_COUNT_POS);
    *afc_2_ctrl = regval;

    /* Enable filter. */
    *afc_hs_filter_cfg &= ~(PDSS_AFC_HS_FILTER_CFG_DM_FILT_SEL_MASK);
    *afc_hs_filter_cfg |= (PDSS_AFC_HS_FILTER_CFG_DM_FILT_EN);

    return CY_USBPD_STAT_SUCCESS;

#else
    CY_UNUSED_PARAMETER(context);
    return CY_USBPD_STAT_NOT_SUPPORTED;
#endif /* (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))  */
}

#define CHGB_AFC_INTR4_PORT0_ALL_INTR_BIT_MASK      (0x11111001)

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_AfcSrcStart
****************************************************************************//**
*
* This function starts the AFC source state machine operations. Cy_USBPD_Bch_AfcSrcInit
* function must be called before this function call.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* cy_en_usbpd_status_t
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Bch_AfcSrcStart(cy_stc_usbpd_context_t *context)
{
#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)) 
    PPDSS_REGS_T pd = context->base;
    volatile uint32_t *afc_opcode_ctrl = NULL;
    
#if defined(CY_DEVICE_PMG1S3)
    afc_opcode_ctrl = &pd->afc_opcode_ctrl;
#else
    afc_opcode_ctrl = &pd->afc_opcode_ctrl[0];
#endif /* defined(CY_DEVICE_PMG1S3) */    
    
    /* Set opcode for afc source operation */
    *afc_opcode_ctrl = AFC_SOURCE_OPCODE | (1UL << 29u);
    
    Cy_SysLib_DelayUs (10);

    /* Enable Interrupts */ 
    pd->intr4 = (PDSS_INTR4_AFC_PING_RECVD | PDSS_INTR4_AFC_SM_IDLE |
                 PDSS_INTR4_AFC_TIMEOUT | PDSS_INTR4_AFC_RX_RESET |
                 PDSS_INTR4_UPDATE_PING_PONG | PDSS_INTR4_AFC_ERROR);

    pd->intr4_mask |= ((PDSS_INTR4_AFC_SM_IDLE) |
                (PDSS_INTR4_UPDATE_PING_PONG) |
                (PDSS_INTR4_AFC_RX_RESET));
    
    context->bcAfcRxIdx = 0;

    /* Start operation */
    *afc_opcode_ctrl |= PDSS_AFC_OPCODE_CTRL_OP_CODE_START;

    return CY_USBPD_STAT_SUCCESS;
    
#else
    CY_UNUSED_PARAMETER(context);
    return CY_USBPD_STAT_NOT_SUPPORTED;
#endif /* (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))  */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_AfcSrcStop
****************************************************************************//**
*
* This function stops the AFC source state machine operations.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* cy_en_usbpd_status_t
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Bch_AfcSrcStop(cy_stc_usbpd_context_t *context)
{
#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)) 
    PPDSS_REGS_T pd = context->base;

    uint32_t intr_mask = 0;

    intr_mask = (PDSS_INTR4_AFC_PING_RECVD | PDSS_INTR4_AFC_SM_IDLE |
                PDSS_INTR4_AFC_TIMEOUT | PDSS_INTR4_AFC_RX_RESET |
                PDSS_INTR4_UPDATE_PING_PONG | PDSS_INTR4_AFC_ERROR);
    
    /* Disable Interrupts */
    pd->intr4_mask &= ~(intr_mask);
    pd->intr4 = intr_mask;

    return CY_USBPD_STAT_SUCCESS;
    
#else
    CY_UNUSED_PARAMETER(context);
    return CY_USBPD_STAT_NOT_SUPPORTED;
#endif /* (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))  */    
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_AfcLoadTxData
****************************************************************************//**
*
* This function load the USBPD context AFC tx data into AFC ping pong buffer.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_Bch_AfcLoadTxData(cy_stc_usbpd_context_t *context)
{
#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)) 
    uint16_t data = 0;
    PPDSS_REGS_T pd = context->base;

    if(context->bcAfcTxIdx < context->bcAfcTxSize)
    {
        data = context->bcAfcTxBuf[context->bcAfcTxIdx++];

        if(context->bcAfcTxIdx < context->bcAfcTxSize)
        {
            data |= (uint16_t)(context->bcAfcTxBuf[context->bcAfcTxIdx++]) << 8u;
        }
#if defined(CY_DEVICE_PMG1S3)
        pd->afc_ping_pong = data;
#else
        pd->afc_ping_pong[0] = data;
#endif /* defined(CY_DEVICE_PMG1S3) */

        Cy_SysLib_DelayUs(2);
    }
    
#else
    CY_UNUSED_PARAMETER(context);
#endif /* (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))  */  
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_Set_AfcTxData
****************************************************************************//**
*
* This function transmits AFC data buffer provided.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \param dataPtr
* Transmit data buffer pointer
*
* \param count
* Transmit data count
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_Bch_Set_AfcTxData(cy_stc_usbpd_context_t *context, uint8_t* dataPtr, uint8_t count)
{
#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)) 
    PPDSS_REGS_T pd = context->base;
    uint32_t regval;
    uint8_t index = 0;
    volatile uint32_t *afc_1_ctrl = NULL;
    
    context->bcAfcTxSize = count;
    context->bcAfcTxIdx = 0;
    
#if defined(CY_DEVICE_PMG1S3)
    afc_1_ctrl = &pd->afc_1_ctrl;
#else
    afc_1_ctrl = &pd->afc_1_ctrl[0];
#endif /* defined(CY_DEVICE_PMG1S3) */

    regval = *afc_1_ctrl & ~PDSS_AFC_1_CTRL_NO_OF_BYTES_TX_MASK;

    regval |= (uint32_t)count << PDSS_AFC_1_CTRL_NO_OF_BYTES_TX_POS;

    *afc_1_ctrl = regval;
    
    /* Copy data into AFC tx buffer */
    for (index = 0; index < count; index++)
    {
        context->bcAfcTxBuf[index]= *(dataPtr++);
    }

    Cy_USBPD_Bch_AfcLoadTxData(context);
    
#else
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(dataPtr);
    CY_UNUSED_PARAMETER(count);
#endif /* (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))  */  
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_AfcSinkInit
****************************************************************************//**
*
* This function initialize the AFC clock, filters and enables block for sink 
* operations. 
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* cy_en_usbpd_status_t
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Bch_AfcSinkInit(cy_stc_usbpd_context_t *context)
{
#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)) 
    PPDSS_REGS_T pd = context->base;
    volatile uint32_t *afc_1_ctrl = NULL;
    volatile uint32_t *afc_2_ctrl = NULL;
    volatile uint32_t *afc_hs_filter_cfg = NULL;
    
#if defined(CY_DEVICE_PMG1S3)
    afc_1_ctrl = &pd->afc_1_ctrl;
    afc_2_ctrl = &pd->afc_2_ctrl;
    afc_hs_filter_cfg = &pd->afc_hs_filter_cfg;
#else
    afc_1_ctrl = &pd->afc_1_ctrl[0];
    afc_2_ctrl = &pd->afc_2_ctrl[0];
    afc_hs_filter_cfg = &pd->afc_hs_filter_cfg[0];
#endif /* defined(CY_DEVICE_PMG1S3) */

    *afc_1_ctrl &= ~(PDSS_AFC_1_CTRL_TX_RESET |
                     PDSS_AFC_1_CTRL_NO_OF_BYTES_TX_MASK);

    *afc_1_ctrl |= PDSS_AFC_1_CTRL_UPDATE_TXCLK;

    /* Update UI clock cycle count for 125kHz clock. */
    *afc_2_ctrl &= ~PDSS_AFC_2_CTRL_UI_COUNT_MASK;
    *afc_2_ctrl |= (QC3_DP_DM_PULSE_FILTER_CLOCK_SEL <<
                    PDSS_AFC_2_CTRL_UI_COUNT_POS);

    *afc_2_ctrl |= PDSS_AFC_2_CTRL_DISABLE_SYNC_ON_RX_BY_4_UI;

    /* Enable filter. */
    *afc_hs_filter_cfg &= ~(PDSS_AFC_HS_FILTER_CFG_DM_FILT_SEL_MASK);
    *afc_hs_filter_cfg |= (PDSS_AFC_HS_FILTER_CFG_DM_FILT_EN);

    /* Enable Clock */
    pd->ctrl |= PDSS_CTRL_AFC_ENABLED;

    return CY_USBPD_STAT_SUCCESS;

#else
    CY_UNUSED_PARAMETER(context);
    return CY_USBPD_STAT_NOT_SUPPORTED;
#endif /* (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))  */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_AfcSinkStartPing
****************************************************************************//**
*
* This function sends a ping request to AFC source.
* operations. 
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* cy_en_usbpd_status_t
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Bch_AfcSinkStartPing(cy_stc_usbpd_context_t *context)
{
#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)) 
    PPDSS_REGS_T pd = context->base;
    volatile uint32_t *afc_opcode_ctrl = NULL;
    
#if defined(CY_DEVICE_PMG1S3)
    afc_opcode_ctrl = &pd->afc_opcode_ctrl;
#else
    afc_opcode_ctrl = &pd->afc_opcode_ctrl[0];
#endif /* defined(CY_DEVICE_PMG1S3) */

    /* Set opcode for afc sink operation */
    *afc_opcode_ctrl = AFC_SINK_OPCODE_PING;

    /* Enable Interrupts */
    pd->intr4 = CHGB_AFC_INTR4_PORT0_ALL_INTR_BIT_MASK;

    pd->intr4_mask |= (PDSS_INTR4_AFC_SM_IDLE
                        | PDSS_INTR4_UPDATE_PING_PONG
                        | PDSS_INTR4_AFC_RX_RESET);

    context->bcAfcRxIdx = 0;

    /* Start operation */
    *afc_opcode_ctrl |= PDSS_AFC_OPCODE_CTRL_OP_CODE_START;

    return CY_USBPD_STAT_SUCCESS;

#else
    CY_UNUSED_PARAMETER(context);
    return CY_USBPD_STAT_NOT_SUPPORTED;
#endif /* (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))  */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_AfcSinkStart
****************************************************************************//**
*
* This function configures the AFC state machine, interrupts and stats the AFC
* sink operations. 
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* cy_en_usbpd_status_t
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Bch_AfcSinkStart(cy_stc_usbpd_context_t *context)
{
#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)) 
    PPDSS_REGS_T pd = context->base;
    volatile uint32_t *afc_opcode_ctrl = NULL; 

    /* Set opcode for afc sink operation */
#if defined(CY_DEVICE_PMG1S3)
    afc_opcode_ctrl = &pd->afc_opcode_ctrl;
#else
    afc_opcode_ctrl = &pd->afc_opcode_ctrl[0];
#endif

    /* Set opcode for afc sink operation */
    *afc_opcode_ctrl = AFC_SINK_OPCODE;
    
    pd->intr4 = CHGB_AFC_INTR4_PORT0_ALL_INTR_BIT_MASK;
    
    /* Enable Interrupts */
    pd->intr4_mask = ( PDSS_INTR4_AFC_SM_IDLE | PDSS_INTR4_UPDATE_PING_PONG |
                        /* Not used. Errors are handled in AFC sink state machine */
#if BC_AFC_SINK_ERROR_INT_ENABLE
                       PDSS_INTR4_AFC_ERROR | PDSS_INTR4_AFC_TIMEOUT |
#endif /* BC_AFC_SINK_ERROR_INT_ENABLE */
                       PDSS_INTR4_AFC_PING_RECVD | PDSS_INTR4_AFC_RX_RESET);

    context->bcAfcRxIdx = 0U;

    /* Start operation */
    *afc_opcode_ctrl |= PDSS_AFC_OPCODE_CTRL_OP_CODE_START;

    return CY_USBPD_STAT_SUCCESS;

#else
    CY_UNUSED_PARAMETER(context);
    return CY_USBPD_STAT_NOT_SUPPORTED;
#endif /* (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))  */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_AfcSinkStop
****************************************************************************//**
*
* This function disables all AFC interrupt.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* cy_en_usbpd_status_t
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Bch_AfcSinkStop(cy_stc_usbpd_context_t *context)
{
#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)) 
    PPDSS_REGS_T pd = context->base;
    
    /* Disable Interrupts */
    pd->intr4_mask = 0;

    pd->intr4 = CHGB_AFC_INTR4_PORT0_ALL_INTR_BIT_MASK;

    return CY_USBPD_STAT_SUCCESS;

#else
    CY_UNUSED_PARAMETER(context);
    return CY_USBPD_STAT_NOT_SUPPORTED;
#endif /* (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))  */
}    

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_QC3_IntrHandler
****************************************************************************//**
*
* This function handles the QC3 interrupts.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_Bch_QC3_IntrHandler(cy_stc_usbpd_context_t *context)
{
#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))     
    PPDSS_REGS_T pd = context->base;
    uint32_t dp_pulse, dm_pulse;
    uint32_t regval;
    volatile uint32_t *qc3_chrger_ctrl = NULL; 

    /* Clear interrupts */
    pd->intr6 = QC3_PORT_0_DP_DM_PULSE_MASK;

#if defined(CY_DEVICE_PMG1S3)
    qc3_chrger_ctrl = &pd->qc3_chrger_ctrl;
#else
    qc3_chrger_ctrl = &pd->qc3_chrger_ctrl[0];
#endif /* defined(CY_DEVICE_PMG1S3) */

    /* Read count */
    *qc3_chrger_ctrl |= PDSS_QC3_CHRGER_CTRL_READ_DPDM_COUNT;
    while((*qc3_chrger_ctrl & PDSS_QC3_CHRGER_CTRL_READ_DPDM_COUNT) != 0UL) {}
    regval = *qc3_chrger_ctrl;

    dp_pulse = (regval & PDSS_QC3_CHRGER_CTRL_DP_PULSE_COUNT_MASK)
    >> PDSS_QC3_CHRGER_CTRL_DP_PULSE_COUNT_POS;
    dm_pulse = (regval & PDSS_QC3_CHRGER_CTRL_DM_PULSE_COUNT_MASK)
                    >> PDSS_QC3_CHRGER_CTRL_DM_PULSE_COUNT_POS;
    context->bcQcPulseCount += ((int32_t)dp_pulse - (int32_t)dm_pulse);

    /* Report an QC continuous event. */
    if (context->bcPhyCbk != NULL)
    {
        context->bcPhyCbk(context, BC_EVT_QC_CONT);
    }

#else
    CY_UNUSED_PARAMETER(context);
#endif /* (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))  */    
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_AfcPingPong_IntrHandler
****************************************************************************//**
*
* This function handles the AFC ping pong interrupts and copies data into context
* buffer. 
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_Bch_AfcPingPong_IntrHandler(cy_stc_usbpd_context_t *context)
{
#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)) 
    PPDSS_REGS_T pd = context->base;
    volatile uint32_t *afc_sm_status = NULL;
    volatile uint32_t *afc_ping_pong = NULL;

#if defined(CY_DEVICE_PMG1S3)
    afc_sm_status = &pd->afc_sm_status;
    afc_ping_pong = &pd->afc_ping_pong;
#else
    afc_sm_status = &pd->afc_sm_status[0];
    afc_ping_pong = &pd->afc_ping_pong[0];
#endif /* defined(CY_DEVICE_PMG1S3) */

    if(((*afc_sm_status & PDSS_AFC_SM_STATUS_CURR_OP_CODE_MASK)
         >> PDSS_AFC_SM_STATUS_CURR_OP_CODE_POS) == AFC_TX_DATA_S_OPCODE)
    {
        /* TX going on */
        Cy_USBPD_Bch_AfcLoadTxData(context);
    }
    else
    {
        /* RX going on */
        if(context->bcAfcRxIdx < AFC_MAX_BYTES)
        {
            context->bcAfcRxBuf[context->bcAfcRxIdx++] = CY_USBPD_DWORD_GET_BYTE0(*afc_ping_pong);
        }
        
        if(context->bcAfcRxIdx < AFC_MAX_BYTES)            
        {
            context->bcAfcRxBuf[context->bcAfcRxIdx++] = CY_USBPD_DWORD_GET_BYTE1(*afc_ping_pong);
        }

#if (!QC_SRC_AFC_CHARGING_DISABLED)
        /* For AFC source, only 1 byte will be received so creating this event here.
         * Later when AFC sink is to be implemented then we need to put a check if source
         * only then create event
         */
        if (context->bcPhyCbk != NULL)
        {
            context->bcPhyCbk(context, BC_EVT_AFC_MSG_RCVD);
        }
#endif /* (!QC_SRC_AFC_CHARGING_DISABLED) */
    }
    
#else
    CY_UNUSED_PARAMETER(context);
#endif /* (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))  */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_AfcIdle_IntrHandler
****************************************************************************//**
*
* This function handles the AFC idle interrupt.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_Bch_AfcIdle_IntrHandler(cy_stc_usbpd_context_t *context)
{
#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)) 
    PPDSS_REGS_T pd = context->base;
    uint32_t event;
    
    if((pd->intr4 & (PDSS_INTR4_AFC_ERROR | PDSS_INTR4_AFC_TIMEOUT)) != 0UL)
    {
        event = BC_EVT_AFC_MSG_SEND_FAIL;
    }
    else
    {
        event = BC_EVT_AFC_MSG_SENT;
    }
    
    if (context->bcPhyCbk != NULL)
    {
        context->bcPhyCbk(context, event);
    }
    
#else
    CY_UNUSED_PARAMETER(context);
#endif /* (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))  */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_AfcReset_IntrHandler
****************************************************************************//**
*
* This function handles the AFC reset interrupt.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_Bch_AfcReset_IntrHandler(cy_stc_usbpd_context_t *context)
{
    /* Report reset received event. */
    if (context->bcPhyCbk != NULL)
    {
        context->bcPhyCbk(context, BC_EVT_AFC_RESET_RCVD);
    }
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_Intr0Handler
****************************************************************************//**
*
* This function handles the Battery Charging interrupts related to AFC and QC.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_Bch_Intr0Handler(cy_stc_usbpd_context_t *context)
{
#if (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3)) 
    PPDSS_REGS_T pd = context->base;

    if(pd->intr4_masked != 0u)
    {
        if ((pd->intr4_masked & (PDSS_INTR4_AFC_PING_RECVD)) != 0u)
        {
            pd->intr4 = PDSS_INTR4_AFC_PING_RECVD;
            /* Nothing to handle here */
        }
        if ((pd->intr4_masked & (PDSS_INTR4_UPDATE_PING_PONG)) != 0u)
        {
            Cy_USBPD_Bch_AfcPingPong_IntrHandler (context);
            pd->intr4 = PDSS_INTR4_UPDATE_PING_PONG;
        }
        if ((pd->intr4_masked & (PDSS_INTR4_AFC_SM_IDLE)) != 0u)
        {
            Cy_USBPD_Bch_AfcIdle_IntrHandler(context);
            pd->intr4 = PDSS_INTR4_AFC_SM_IDLE;
        }
        if ((pd->intr4_masked & (PDSS_INTR4_AFC_RX_RESET)) != 0u)
        {
            Cy_USBPD_Bch_AfcReset_IntrHandler(context);
            pd->intr4 = PDSS_INTR4_AFC_RX_RESET;
        }
    }

    if(pd->intr6_masked != 0u)
    {
        if ((pd->intr6_masked & QC3_PORT_0_DP_DM_PULSE_MASK) != 0u)
        {
            Cy_USBPD_Bch_QC3_IntrHandler(context);
        }
    }
    
#else
    CY_UNUSED_PARAMETER(context);
#endif /* (defined(CY_DEVICE_CCG3PA) || defined(CY_DEVICE_CCG6) || defined(CY_DEVICE_PMG1S3))  */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Bch_Intr1Handler
****************************************************************************//**
*
* This function handles the Battery Charging interrupts.
*
* \param context
* Pointer to the context structure \ref cy_stc_usbpd_context_t.
*
* \return
* None
*
*******************************************************************************/
void Cy_USBPD_Bch_Intr1Handler(cy_stc_usbpd_context_t *context)
{
    PPDSS_REGS_T pd = context->base ;

#if defined(CY_IP_MXUSBPD)
    if ((pd->intr9_masked & BCH_PORT_0_CMP1_INTR_MASK) != 0u)
    {
        /* Disable and clear interrupts. */
        pd->intr9_mask &= ~BCH_PORT_0_CMP1_INTR_MASK;
        pd->intr9 = BCH_PORT_0_CMP1_INTR_MASK;

        /* Report an CMP1 fire event. */
        if (context->bcPhyCbk != NULL)
        {
            context->bcPhyCbk(context, BC_EVT_CMP1_FIRE);
        }
    }

    if ((pd->intr9_masked & BCH_PORT_0_CMP2_INTR_MASK) != 0u)
    {
        /* Disable and clear interrupts. */
        pd->intr9_mask &= ~BCH_PORT_0_CMP2_INTR_MASK;
        pd->intr9 = BCH_PORT_0_CMP2_INTR_MASK;

        /* Report an CMP2 fire event. */
        if (context->bcPhyCbk != NULL)
        {
            context->bcPhyCbk(context, BC_EVT_CMP2_FIRE);
        }
    }

#if (NO_OF_BC_PORTS == 2)
    if ((pd->intr9_masked & BCH_PORT_1_CMP1_INTR_MASK) != 0u)
    {
        /* Disable and clear interrupts. */
        pd->intr9_mask &= ~BCH_PORT_1_CMP1_INTR_MASK;
        pd->intr9 = BCH_PORT_1_CMP1_INTR_MASK;

        /* Report an CMP1 fire event. */
        if (context->bcPhyCbk != NULL)
        {
            context->bcPhyCbk(context, BC_EVT_CMP1_FIRE);
        }
    }

    if ((pd->intr9_masked & BCH_PORT_1_CMP2_INTR_MASK) != 0u)
    {
        /* Disable and clear interrupts. */
        pd->intr9_mask &= ~BCH_PORT_1_CMP2_INTR_MASK;
        pd->intr9 = BCH_PORT_1_CMP2_INTR_MASK;

        /* Report an CMP2 fire event. */
        if (context->bcPhyCbk != NULL)
        {
            context->bcPhyCbk(context, BC_EVT_CMP2_FIRE);
        }
    }
#endif /* (NO_OF_BC_PORTS == 2) */

#elif defined(CY_IP_M0S8USBPD)

    if ((pd->intr3_masked & PDSS_INTR3_MASKED_CHGDET_CHANGED_MASKED) != 0u)
    {
        /* Disable and clear interrupt. */
        pd->intr3_mask &= ~PDSS_INTR3_MASK_CHGDET_CHANGED_MASK;
        pd->intr3 = PDSS_INTR3_CHGDET_CHANGED;

        /* Report comparator fired. */
        if (context->bcPhyCbk != NULL)
        {
            context->bcPhyCbk(context, BC_EVT_CMP1_FIRE);
        }
    }

#else
    CY_UNUSED_PARAMETER(context);
#endif /* CY_IP */
}

#endif /* (defined(CY_IP_MXUSBPD) || defined(CY_IP_M0S8USBPD)) */

/* [] END OF FILE */
