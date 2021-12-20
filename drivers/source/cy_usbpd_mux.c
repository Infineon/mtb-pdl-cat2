/***************************************************************************//**
* \file cy_usbpd_mux.c
* \version 1.30
*
* Provides implementation of MUX control functions for the USBPD IP.
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

#include "cy_usbpd_common.h"
#include "cy_usbpd_mux.h"

/*******************************************************************************
* Function Name: Cy_USBPD_Mux_ConfigDpDm
****************************************************************************//**
*
* This function configures the Dp/Dm MUX.
*
* \param context
* USBPD PDL Context pointer.
*
* \param conf
* DP/DM MUX Configuration
*
* \return
*  cy_en_usbpd_status_t 
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Mux_ConfigDpDm(cy_stc_usbpd_context_t *context, cy_en_usbpd_dpdm_mux_cfg_t conf)
{
#if defined(CY_DEVICE_CCG6)    
    PPDSS_REGS_T pd = context->base;

    /* Do nothing if the configuration is already correct. */
    if (context->curDpdmCfg == conf)
    {
        return CY_USBPD_STAT_SUCCESS;
    }
    context->curDpdmCfg = conf;

    if (conf == CY_USBPD_DPDM_MUX_CONN_NONE)
    {
        /* Disable pump first. PUMP[3] is for Port 0 */
        Cy_USBPD_Pump_Enable (context, (uint8_t)(2U));
        
        Cy_SysLib_DelayUs(2);

        /* Turn off all switches and isolate outputs. */
        pd->dpdm_ctrl &=
                        (
                        (PDSS_DPDM_CTRL_DPDM_T_DPDM_MASK) |
                        (PDSS_DPDM_CTRL_DCP_SHORT_DPDM_TOP |
                         PDSS_DPDM_CTRL_DCP_SHORT_DPDM_BOTTOM)
                        );
    }
    else
    {
        /* Enable the output switch and configure required bits. */
        pd->dpdm_ctrl = (pd->dpdm_ctrl & PDSS_DPDM_CTRL_DPDM_T_DPDM_MASK) |
            ((uint32_t)conf | PDSS_DPDM_CTRL_DPDM_ISO_N);
        Cy_SysLib_DelayUs(2);

        /* Now enable pump for slow ramp of signals. */
        Cy_USBPD_Pump_Enable (context, (uint8_t)(2U));
    }
    return CY_USBPD_STAT_SUCCESS;
#else
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(conf);
    return CY_USBPD_STAT_NOT_SUPPORTED;
#endif /* defined(CY_DEVICE_CCG6) */
}

/*******************************************************************************
* Function Name: Cy_USBPD_Mux_SbuSwitchConfigure
****************************************************************************//**
*
* This function configures the SBU Switches.
*
* \param context
* USBPD PDL Context pointer.
*
* \param sbu1State
* sbu switch 1 state
*
* \param sbu2State
* sbu switch 2 state
*
* \return
*  cy_en_usbpd_status_t 
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Mux_SbuSwitchConfigure(cy_stc_usbpd_context_t *context, 
                                                     cy_en_usbpd_sbu_switch_state_t sbu1State, 
                                                     cy_en_usbpd_sbu_switch_state_t sbu2State) 
{
    /* Used CCG6 macro instead CY_DEVICE_CCG6 here since SBU MUX
     * is not advertised for PMG1S1 device */
#if defined(CCG6)
    PPDSS_REGS_T pd = context->base; 
    uint32_t sbu1val;
    uint32_t sbu2val;
    uint32_t intstate;

    /* Check that state values are within allowed range. */
    if ((sbu1State >= CY_USBPD_SBU_MAX_STATE) || (sbu2State >= CY_USBPD_SBU_MAX_STATE))
    {
        /* Don't service the request. */
        return CY_USBPD_STAT_INVALID_ARGUMENT; 
    }

    intstate = Cy_SysLib_EnterCriticalSection();
    if ((sbu1State == CY_USBPD_SBU_NOT_CONNECTED) && (sbu2State == CY_USBPD_SBU_NOT_CONNECTED))
    {
        /* Turn off pump first and then disable all switches. */
        Cy_USBPD_Pump_Enable(context, (uint8_t)(1U));
        Cy_SysLib_DelayUs (2);

        pd->sbu20_sbu1_en_1_ctrl = 0;
        pd->sbu20_sbu2_en_1_ctrl = 0;
    }
    else
    {
        /* SBU1 connection. */
        /* Turn the switch off when OVP is detected on any of the CC or SBU lines. */
        sbu1val = PDSS_SBU20_SBU1_EN_1_CTRL_SEL_ON_OFF |
            PDSS_SBU20_SBU1_EN_1_CTRL_SEL_CC1_OVP | PDSS_SBU20_SBU1_EN_1_CTRL_SEL_CC2_OVP |
            PDSS_SBU20_SBU1_EN_1_CTRL_SBU1_SEL_SBU1_OVP | PDSS_SBU20_SBU1_EN_1_CTRL_SBU1_SEL_SBU2_OVP;

        switch (sbu1State)
        {
            case CY_USBPD_SBU_CONNECT_AUX1:
                sbu1val |= PDSS_SBU20_SBU1_EN_1_CTRL_AUXP_SBU1_EN_ON_VALUE;
                break;

            case CY_USBPD_SBU_CONNECT_AUX2:
                sbu1val |= PDSS_SBU20_SBU1_EN_1_CTRL_AUXN_SBU1_EN_ON_VALUE;
                break;

            case CY_USBPD_SBU_CONNECT_LSTX:
                sbu1val |= PDSS_SBU20_SBU1_EN_1_CTRL_LSTX_SBU1_EN_ON_VALUE;
                break;

            case CY_USBPD_SBU_CONNECT_LSRX:
                sbu1val |= PDSS_SBU20_SBU1_EN_1_CTRL_LSRX_SBU1_EN_ON_VALUE;
                break;

            default:
                sbu1val = 0;
                break;
        }

        /* SBU2 connection. */
        /* Turn the switch off when OVP is detected on any of the CC or SBU lines. */
        sbu2val = PDSS_SBU20_SBU2_EN_1_CTRL_SEL_ON_OFF |
            PDSS_SBU20_SBU2_EN_1_CTRL_SEL_CC1_OVP | PDSS_SBU20_SBU2_EN_1_CTRL_SEL_CC2_OVP |
            PDSS_SBU20_SBU2_EN_1_CTRL_SBU2_SEL_SBU1_OVP | PDSS_SBU20_SBU2_EN_1_CTRL_SBU2_SEL_SBU2_OVP;

        switch (sbu2State)
        {
            case CY_USBPD_SBU_CONNECT_AUX1:
                sbu2val |= PDSS_SBU20_SBU2_EN_1_CTRL_AUXP_SBU2_EN_ON_VALUE;
                break;

            case CY_USBPD_SBU_CONNECT_AUX2:
                sbu2val |= PDSS_SBU20_SBU2_EN_1_CTRL_AUXN_SBU2_EN_ON_VALUE;
                break;

            case CY_USBPD_SBU_CONNECT_LSTX:
                sbu2val |= PDSS_SBU20_SBU2_EN_1_CTRL_LSTX_SBU2_EN_ON_VALUE;
                break;

            case CY_USBPD_SBU_CONNECT_LSRX:
                sbu2val |= PDSS_SBU20_SBU2_EN_1_CTRL_LSRX_SBU2_EN_ON_VALUE;
                break;

            default:
                sbu2val = 0;
                break;
        }

        pd->sbu20_sbu1_en_1_ctrl = sbu1val;
        pd->sbu20_sbu2_en_1_ctrl = sbu2val;

        /* Provide some delay and turn the pump on. */
        Cy_SysLib_DelayUs (5);
        Cy_USBPD_Pump_Enable (context, (uint8_t)(1U));
    }

    /* Store SBU1 and SBU2 states. */
    context->sbu1State = sbu1State;
    context->sbu2State = sbu2State;

    Cy_SysLib_ExitCriticalSection(intstate);
    return CY_USBPD_STAT_SUCCESS;
#elif defined(CY_DEVICE_CCG3)
    PPDSS_REGS_T pd = context->base; 
    uint32_t regval;
    uint32_t intstate;

    /* Check that state values are within allowed range. */
    if ((sbu1State > CY_USBPD_SBU_CONNECT_AUX2) || (sbu2State > CY_USBPD_SBU_CONNECT_AUX2))
    {
        /* Don't service the request. */
        return CY_USBPD_STAT_INVALID_ARGUMENT; 
    }

    intstate = Cy_SysLib_EnterCriticalSection();
    regval = pd->sbu_ctrl;

    /* Configure SBU1 switch. */
    if (sbu1State == CY_USBPD_SBU_NOT_CONNECTED)
    {
        /* SBU1 shall not be connected to AUX1/2. */
        regval &= ~(PDSS_SBU_CTRL_IN1_OUT1_EN | PDSS_SBU_CTRL_IN1_OUT2_EN);
    }
    else if (sbu1State == CY_USBPD_SBU_CONNECT_AUX1)
    {
        /* SBU1 shall be connected to AUX1 and not to AUX2. */
        regval |= PDSS_SBU_CTRL_IN1_OUT1_EN;
        regval &= ~PDSS_SBU_CTRL_IN1_OUT2_EN;
    }
    else
    {
        /* SBU1 shall be connected to AUX2 and not to AUX1. */
        regval |= PDSS_SBU_CTRL_IN1_OUT2_EN;
        regval &= ~PDSS_SBU_CTRL_IN1_OUT1_EN;
    }

    /* Configure SBU2 switch. */
    if (sbu2State == CY_USBPD_SBU_NOT_CONNECTED)
    {
        /* SBU2 shall not be connected to AUX1/2. */
        regval &= ~(PDSS_SBU_CTRL_IN2_OUT1_EN | PDSS_SBU_CTRL_IN2_OUT2_EN);
    }
    else if (sbu2State == CY_USBPD_SBU_CONNECT_AUX1)
    {
        /* SBU2 shall be connected to AUX1 and not to AUX2. */
        regval |= PDSS_SBU_CTRL_IN2_OUT1_EN;
        regval &= ~PDSS_SBU_CTRL_IN2_OUT2_EN;
    }
    else
    {
        /* SBU2 shall be connected to AUX2 and not to AUX1. */
        regval |= PDSS_SBU_CTRL_IN2_OUT2_EN;
        regval &= ~PDSS_SBU_CTRL_IN2_OUT1_EN;
    }

    /* Store SBU1 and SBU2 states. */
    /* Store SBU1 and SBU2 states. */
    context->sbu1State = sbu1State;
    context->sbu2State = sbu2State;

    /* Update the switch register. */
    pd->sbu_ctrl = regval;
    Cy_SysLib_ExitCriticalSection(intstate);
    return CY_USBPD_STAT_SUCCESS;
#elif (defined(CY_DEVICE_PMG1S3))
    PPDSS_REGS_T pd = context->base; 
    uint32_t regIndexLs;
    uint32_t regIndexAux;
    uint32_t intstate;

    /* Check that state values are within allowed range. */
    if ((sbu1State >= CY_USBPD_SBU_MAX_STATE) || (sbu2State >= CY_USBPD_SBU_MAX_STATE))
    {
        /* Don't service the request. */
        return CY_USBPD_STAT_INVALID_ARGUMENT; 
    }

    intstate = Cy_SysLib_EnterCriticalSection();

    /* SBU Connections for PMG1S3:
     * sbu_new_ctrl[0] - LSTX and LSRX
     * sbu_new_ctrl[1] - AUX1 and AUX2
     * IN1 - SBU1
     * IN2 - SBU2
     * OUT1 - LSTX/AUX1(AUXP)
     * OUT2 - LSRX/AUX2(AUXN)
     * */
    regIndexLs  = pd->sbu_new_ctrl[0];
    regIndexAux = pd->sbu_new_ctrl[1];

    switch (sbu1State)
    {
        case CY_USBPD_SBU_CONNECT_AUX1:
            /* SBU1 shall be connected to AUX1 and not to AUX2. */
            regIndexAux |= PDSS_SBU_NEW_CTRL_SBU_NEW_IN1_OUT1_EN;
            regIndexAux &= ~PDSS_SBU_NEW_CTRL_SBU_NEW_IN1_OUT2_EN;

            /* Clear the LS connections if any */
            regIndexLs &= ~(PDSS_SBU_NEW_CTRL_SBU_NEW_IN1_OUT1_EN | PDSS_SBU_NEW_CTRL_SBU_NEW_IN1_OUT2_EN);
            break;

        case CY_USBPD_SBU_CONNECT_AUX2:
            regIndexAux |= PDSS_SBU_NEW_CTRL_SBU_NEW_IN1_OUT2_EN;
            regIndexAux &= ~PDSS_SBU_NEW_CTRL_SBU_NEW_IN1_OUT1_EN;

            /* Clear the LS connections if any */
            regIndexLs &= ~(PDSS_SBU_NEW_CTRL_SBU_NEW_IN1_OUT1_EN | PDSS_SBU_NEW_CTRL_SBU_NEW_IN1_OUT2_EN);
            break;

        case CY_USBPD_SBU_CONNECT_LSTX:
            regIndexLs |= PDSS_SBU_NEW_CTRL_SBU_NEW_IN1_OUT1_EN;
            regIndexLs &= ~PDSS_SBU_NEW_CTRL_SBU_NEW_IN1_OUT2_EN;

            /* Clear the Aux connections if any */
            regIndexAux &= ~(PDSS_SBU_NEW_CTRL_SBU_NEW_IN1_OUT1_EN | PDSS_SBU_NEW_CTRL_SBU_NEW_IN1_OUT2_EN);
            break;

        case CY_USBPD_SBU_CONNECT_LSRX:
            regIndexLs |= PDSS_SBU_NEW_CTRL_SBU_NEW_IN1_OUT2_EN;
            regIndexLs &= ~PDSS_SBU_NEW_CTRL_SBU_NEW_IN1_OUT1_EN;

            /* Clear the Aux connections if any */
            regIndexAux &= ~(PDSS_SBU_NEW_CTRL_SBU_NEW_IN1_OUT1_EN | PDSS_SBU_NEW_CTRL_SBU_NEW_IN1_OUT2_EN);
            break;

        default:
            /* Clear the Aux/LS connections if any */
            regIndexAux &= ~(PDSS_SBU_NEW_CTRL_SBU_NEW_IN1_OUT1_EN | PDSS_SBU_NEW_CTRL_SBU_NEW_IN1_OUT2_EN);
            regIndexLs &= ~(PDSS_SBU_NEW_CTRL_SBU_NEW_IN1_OUT1_EN | PDSS_SBU_NEW_CTRL_SBU_NEW_IN1_OUT2_EN);
            break;
    }

    switch (sbu2State)
    {
        case CY_USBPD_SBU_CONNECT_AUX1:
            /* SBU2 shall be connected to AUX1 and not to AUX2. */
            regIndexAux |= PDSS_SBU_NEW_CTRL_SBU_NEW_IN2_OUT1_EN;
            regIndexAux &= ~PDSS_SBU_NEW_CTRL_SBU_NEW_IN2_OUT2_EN;

            /* Clear the LS connections if any */
            regIndexLs &= ~(PDSS_SBU_NEW_CTRL_SBU_NEW_IN2_OUT1_EN | PDSS_SBU_NEW_CTRL_SBU_NEW_IN2_OUT2_EN);
            break;

        case CY_USBPD_SBU_CONNECT_AUX2:
            regIndexAux |= PDSS_SBU_NEW_CTRL_SBU_NEW_IN2_OUT2_EN;
            regIndexAux &= ~PDSS_SBU_NEW_CTRL_SBU_NEW_IN2_OUT1_EN;

            /* Clear the LS connections if any */
            regIndexLs &= ~(PDSS_SBU_NEW_CTRL_SBU_NEW_IN2_OUT1_EN | PDSS_SBU_NEW_CTRL_SBU_NEW_IN2_OUT2_EN);
            break;

        case CY_USBPD_SBU_CONNECT_LSTX:
            regIndexLs |= PDSS_SBU_NEW_CTRL_SBU_NEW_IN2_OUT1_EN;
            regIndexLs &= ~PDSS_SBU_NEW_CTRL_SBU_NEW_IN2_OUT2_EN;

            /* Clear the Aux connections if any */
            regIndexAux &= ~(PDSS_SBU_NEW_CTRL_SBU_NEW_IN2_OUT1_EN | PDSS_SBU_NEW_CTRL_SBU_NEW_IN2_OUT2_EN);
            break;

        case CY_USBPD_SBU_CONNECT_LSRX:
            regIndexLs |= PDSS_SBU_NEW_CTRL_SBU_NEW_IN2_OUT2_EN;
            regIndexLs &= ~PDSS_SBU_NEW_CTRL_SBU_NEW_IN2_OUT1_EN;

            /* Clear the Aux connections if any */
            regIndexAux &= ~(PDSS_SBU_NEW_CTRL_SBU_NEW_IN2_OUT1_EN | PDSS_SBU_NEW_CTRL_SBU_NEW_IN2_OUT2_EN);
            break;

        default:
            /* Clear the Aux/LS connections if any */
            regIndexAux &= ~(PDSS_SBU_NEW_CTRL_SBU_NEW_IN2_OUT1_EN | PDSS_SBU_NEW_CTRL_SBU_NEW_IN2_OUT2_EN);
            regIndexLs &= ~(PDSS_SBU_NEW_CTRL_SBU_NEW_IN2_OUT1_EN | PDSS_SBU_NEW_CTRL_SBU_NEW_IN2_OUT2_EN);
            break;
    }

    /* Store SBU1 and SBU2 states. */
    context->sbu1State = sbu1State;
    context->sbu2State = sbu2State;

    /* Update the switch register. */
    pd->sbu_new_ctrl[0] = regIndexLs;
    pd->sbu_new_ctrl[1] = regIndexAux;

    Cy_SysLib_ExitCriticalSection(intstate);
    return CY_USBPD_STAT_SUCCESS;
#else
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(sbu1State);
    CY_UNUSED_PARAMETER(sbu2State);
    return CY_USBPD_STAT_NOT_SUPPORTED;
#endif /* defined(CY_DEVICE_CCG6) */    
}

/*******************************************************************************
* Function Name: Cy_USBPD_Mux_AuxTermConfigure
****************************************************************************//**
*
* This function configures resistor values for AUX1 and AUX2.
*
* \param context
* USBPD PDL Context pointer.
*
* \param aux1Config
* Aux Resistor 1 state
*
* \param aux2Config
* Aux Resistor 2 state
*
* \return
*  cy_en_usbpd_status_t 
*
*******************************************************************************/
cy_en_usbpd_status_t Cy_USBPD_Mux_AuxTermConfigure(cy_stc_usbpd_context_t *context,
                                   cy_en_usbpd_aux_resistor_config_t aux1Config,
                                   cy_en_usbpd_aux_resistor_config_t aux2Config) 
{
#if (defined(CY_DEVICE_CCG3) || defined(CY_DEVICE_PMG1S3))
    PPDSS_REGS_T pd = context->base;
    uint32_t regval = pd->sbu_ctrl;

    /* Check if resistor values passed are correct. */
    if ((aux1Config > CY_USBPD_AUX_1_470K_PD_RESISTOR) ||
            (((aux2Config < CY_USBPD_AUX_2_100K_PU_RESISTOR) && (aux2Config != CY_USBPD_AUX_NO_RESISTOR))
             || (aux2Config > CY_USBPD_AUX_MAX_RESISTOR_CONFIG)))
    {
        /* Wrong configuration. */
        return CY_USBPD_STAT_INVALID_ARGUMENT;
    }

    /* Check if no resistor configuration for AUX1. */
    if (aux1Config == CY_USBPD_AUX_NO_RESISTOR)
    {
        regval &= ~(PDSS_SBU_CTRL_OUT1_1MEG_EN_PU | PDSS_SBU_CTRL_OUT1_100K_EN_PD
                | PDSS_SBU_CTRL_OUT1_470K_EN_PD);
    }
    else
    {
        /* Enable the requested resistor. */
        regval |= (uint32_t)(1UL << (uint32_t)aux1Config);
    }

    /* Check if no resistor configuration for AUX2. */
    if (aux2Config == CY_USBPD_AUX_NO_RESISTOR)
    {
        regval &= ~(PDSS_SBU_CTRL_OUT2_100K_EN_PU | PDSS_SBU_CTRL_OUT2_4P7MEG_EN_PD
                | PDSS_SBU_CTRL_OUT2_1MEG_EN_PD);
    }
    else
    {
        /* Enable the requested resistor. */
        regval |= (uint32_t)(1UL << (uint32_t)aux2Config);
    }

    /* Store the configuration. */
    context->aux1Config = aux1Config;
    context->aux2Config = aux2Config;

    /* Update the resistor. */
    pd->sbu_ctrl = regval;
    return CY_USBPD_STAT_SUCCESS;
#else
    CY_UNUSED_PARAMETER(context);
    CY_UNUSED_PARAMETER(aux1Config);
    CY_UNUSED_PARAMETER(aux2Config);
    return CY_USBPD_STAT_NOT_SUPPORTED;
#endif /* defined(CY_DEVICE_CCG3) || defined(CY_DEVICE_PMG1S3) */   
}

#endif /* (defined(CY_IP_MXUSBPD) || defined(CY_IP_M0S8USBPD)) */

/* [] END OF FILE */
