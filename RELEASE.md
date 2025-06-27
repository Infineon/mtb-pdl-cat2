# mtb-pdl-cat2 peripheral driver library v2.17.0

See the [README.md](./README.md) and the
[PDL API reference manual](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/index.html)
for a complete description of the Peripheral driver library.

## What's included?

### New Features
- Added support for PSOC&trade; 4 HVPA 144K device family.
- Added new drivers for the PSOC&trade; 4 HVPA devices.
- Added the mtb_stdlib_stubs.c file with weak stub implementations for GCC_ARM nosys library.
Please refer to the [New drivers](#new-drivers) section below for details.

### Updated personalities
- Enabled multiple personalities on PSOC&trade; 4 HVPA Platform: PUMPCLK, HVSS, SYSTICK.
- Added new personalities for PSOC&trade; 4 HVPA Platform: PACSS, AGC, Current, Voltage, On-Die temperature,
External temperature, GNDREF, HPBGR, Input multiplexor, Temp.

### Updated drivers
- [Cy_Flash 1.30](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__flash.html)
- [SysLib 3.30.1](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__syslib.html)
- [LIN 1.10](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__lin.html)
- [SysClk 3.50](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__sysclk.html)
- [SysFault 1.10](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__sysfault.html)

### New drivers
- [DSADC 1.0](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__dsadc.html)

## Defect fixes

See the Changelog section of each driver in [PDL API reference](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/modules.html) for all fixes and updates.

## Supported software and tools

This version of PDL was validated for compatibility with the following software and tools:

| Software and tools                                                            | Version      |
| :---                                                                          | :----        |
| ModusToolbox&trade;                                                           |  3.5.0       |
| [core library](https://github.com/Infineon/core-lib)                          |  1.4.4       |
| [device-db](https://github.com/Infineon/device-db)                            |  4.26.0      |
| CMSIS-Core(M)                                                                 |  5.8.2       |
| GCC compiler                                                                  |  14.2.1      |
| IAR compiler                                                                  |  9.50.2      |
| Arm&reg; compiler 6                                                           |  6.22.0      |

## More information

- [Peripheral driver library README.md](./README.md)

- [Peripheral driver library API reference manual](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/index.html)

- [ModusToolbox&trade; Software Environment, Quick Start Guide, Documentation, and Videos](https://www.infineon.com/cms/en/design-support/tools/sdk/modustoolbox-software)

- [ModusToolbox&trade; Device Configurator Tool Guide](https://documentation.infineon.com/html/modustoolbox-software/en/latest/tool-guide/ModusToolbox_Device_Configurator_User_Guide.html)

- [AN79953 - Getting started with PSOC&trade; 4](https://www.infineon.com/dgdl/Infineon-AN79953_Getting_Started_with_PSoC_4-ApplicationNotes-v21_00-EN.pdf?fileId=8ac78c8c7cdc391c017d07271fd64bc1&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-an_vanitylink)

- [PSOC&trade; 4 technical reference manuals](https://documentation.infineon.com/psoc4/docs/hup1702048028817)

- [PSOC&trade; 4 datasheets](https://documentation.infineon.com/psoc4/docs/qqs1702048028479)

- [PMG1 device family](https://www.infineon.com/cms/en/product/universal-serial-bus-usb-power-delivery-controller/usb-c-and-power-delivery/ez-pd-pmg1-portfolio-high-voltage-mcus-usb-c-power-delivery/?utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-product_families)

- [CCGxF_CFP device family](https://www.infineon.com/cms/en/product/universal-serial-bus/usb-c-power-delivery-controllers)

---
© 2020-2025, Cypress Semiconductor Corporation (an Infineon company) or an affiliate of Cypress Semiconductor Corporation.
