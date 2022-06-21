#include "cy_device.h"
#include "cy_device_headers.h"

#if (defined(CY_IP_MXUSBPD) || defined(CY_IP_M0S8USBPD))

#include "cy_usbpd_defines.h"
#include "cy_usbpd_common.h"
#include "config_table.h"


const auto_config_t * get_auto_config(cy_stc_usbpd_context_t *context)
{
    /* QAC suppression 0310, 3305: The correct pointer alignment is ensured by the placement
     * of this table as per the linker script. */
    return (const auto_config_t *)context->cfg_table; /* PRQA S 0310, 3305 */
}

const pd_port_config_t * get_pd_port_config(cy_stc_usbpd_context_t *context)
{
    if(context->port==0u)
    {
    /* QAC suppression 0310, 3305: The correct pointer alignment is ensured by the placement
     * of this table as per the linker script. */
    return ((const pd_port_config_t *)((uint32_t)(const uint8_t *)(get_auto_config (context)) +
                get_auto_config(context)->port_0_config_offset));  /* PRQA S 0310, 3305 */
    }
    else
    {
    return ((const pd_port_config_t *)((uint32_t)(const uint8_t *)(get_auto_config (context)) +
                get_auto_config(context)->port_1_config_offset));
    }
}

#if (defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S) || defined(CY_DEVICE_CCG3PA))
auto_cfg_settings_t* pd_get_ptr_auto_cfg_tbl(cy_stc_usbpd_context_t *context)
{
    /* Update the Automotive charger settings from the configuration table*/
    return ((auto_cfg_settings_t *)((uint32_t)(const uint8_t *)(get_auto_config (context)) +
            get_auto_config(context)->auto_config_offset));
}
#endif /* (defined(CY_DEVICE_CCG7D) || defined(CY_DEVICE_CCG7S)- || defined(CY_DEVICE_CCG3PA)) */

#if CY_USE_CONFIG_TABLE
ovp_settings_t* pd_get_ptr_ovp_tbl(cy_stc_usbpd_context_t *context)
{
    /* Update the OVP settings from the configuration table. */
    return ((ovp_settings_t *)(uint32_t)((const uint8_t *)(get_auto_config (context)) +
                get_pd_port_config(context)->port_n_ovp_table_offset));
}

#if (BATTERY_CHARGING_ENABLE) || (QC_PPS_ENABLE)
cy_stc_legacy_charging_cfg_t* pd_get_ptr_chg_cfg_tbl(cy_stc_usbpd_context_t *context)
{
#if CCG_TYPE_A_PORT_ENABLE
    if (port == TYPE_A_PORT_ID)
    {
        /* Return parameters for TYPE-A port. */
        return pd_get_ptr_type_a_chg_cfg_tbl (0);
    }
#endif /* CCG_TYPE_A_PORT_ENABLE */

    /* Update the legacy charging parameters from the configuration table */
    return ((cy_stc_legacy_charging_cfg_t *)((uint32_t)(const uint8_t *)(get_auto_config (context)) +
            get_pd_port_config(context)->port_n_bch_table_offset));
}
#endif /* (BATTERY_CHARGING_ENABLE) || (QC_PPS_ENABLE) */

rcp_settings_t* pd_get_ptr_rcp_tbl(cy_stc_usbpd_context_t *context)
{
    /* Update the VBus OCP settings from the configuration table */
    return ((rcp_settings_t *)((uint32_t)(const uint8_t *)(get_auto_config (context)) +
                get_pd_port_config(context)->port_n_rcp_table_offset));
}
ocp_settings_t* pd_get_ptr_ocp_tbl(cy_stc_usbpd_context_t *context)
{
    /* Update the VBus OCP settings from the configuration table */
    return ((ocp_settings_t *)((uint32_t)(const uint8_t *)(get_auto_config (context)) +
                get_pd_port_config(context)->port_n_ocp_table_offset));
}

scp_settings_t* pd_get_ptr_scp_tbl(cy_stc_usbpd_context_t *context)
{
    /* Update the VBus SCP settings from the configuration table */
    return ((scp_settings_t *)((uint32_t)(const uint8_t *)(get_auto_config (context)) +
                get_pd_port_config(context)->port_n_scp_table_offset));
}

vconn_ocp_settings_t* pd_get_ptr_vconn_ocp_tbl(cy_stc_usbpd_context_t *context)
{
    /* Update the Vcon OCP settings from the configuration table */
    return ((vconn_ocp_settings_t *)((uint32_t)(const uint8_t *)(get_auto_config (context)) +
                get_pd_port_config(context)->port_n_vconn_ocp_table_offset));
}

otp_settings_t* pd_get_ptr_otp_tbl(cy_stc_usbpd_context_t *context)
{
    /* Update the OTP settings from the configuration table */
    return ((otp_settings_t *)((uint32_t)(const uint8_t *)(get_auto_config (context)) +
                get_pd_port_config(context)->port_n_otp_table_offset));
}

uvp_settings_t* pd_get_ptr_uvp_tbl(cy_stc_usbpd_context_t *context)
{
    /* Update the VBus UVP settings from the configuration table */
    return ((uvp_settings_t *)((uint32_t)(const uint8_t *)(get_auto_config (context)) +
                get_pd_port_config(context)->port_n_uvp_table_offset));
}
const cy_stc_pdstack_port_cfg_t* pd_get_ptr_pdstack_tbl(cy_stc_usbpd_context_t *context)
{
    return ((const cy_stc_pdstack_port_cfg_t *)((uint32_t)(const uint8_t *)(get_auto_config (context)) +
                    get_pd_port_config(context)->port_n_pd_table_offset));
}
#endif /* CY_USE_CONFIG_TABLE */

pwr_params_t* pd_get_ptr_pwr_tbl(cy_stc_usbpd_context_t *context)
{
    /* Update the power parameters from the configuration table */
    return ((pwr_params_t *)((uint32_t)(const uint8_t *)(get_auto_config (context)) +
                get_pd_port_config(context)->port_n_power_table_offset));
}

app_config_t* pd_get_ptr_app_tbl(cy_stc_usbpd_context_t *context)
{
    /* Update the power parameters from the configuration table */
    return ((app_config_t *)((uint32_t)(const uint8_t *)(get_auto_config (context)) +
                get_pd_port_config(context)->port_n_app_table_offset));
}

base_alt_cfg_settings_t* pd_get_ptr_base_alt_tbl(cy_stc_usbpd_context_t *context)
{
    return ((base_alt_cfg_settings_t *)((uint32_t)(const uint8_t *)(get_auto_config (context)) +
                get_pd_port_config(context)->port_n_base_alt_mode_table_offset));
}
#endif
