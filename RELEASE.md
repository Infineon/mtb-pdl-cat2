# mtb-pdl-cat2 peripheral driver library v2.7.0

See the [README.md](./README.md) and the
[PDL API reference manual](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/index.html)
for a complete description of the Peripheral driver library.

## What's included?

### New Features
- Support for new MPNs devices of the PSoC 4100S MAX device family.

### Updated personalities

- USBPD 1.0 - Pwm Fix Freq Dith parameter for buck-boost controller is set to integer type,
fixed enumeration order of Pwm Dithering Type parameter.

### Updated drivers

- [USBPD 2.60](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__usbpd.html)
- [WDT 1.0.3](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__wdt.html)

## Known Issues

When WLC1115-68LQXQ or WLC1150-68LQXQ based project is compiled by IAR compiler the following warning  issued: "/builds/repo/bsp_csp/output/libs/COMPONENT_CAT2/mtb-pdl-cat2/drivers/source/cy_usbpd_vbus_ctrl.c",4052: dynamic initialization in unreachable code. This warning do not cause any system malfunction.

## Defect fixes

See the Changelog section of each driver in [PDL API reference](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/modules.html) for all fixes and updates.

## Supported software and tools

This version of PDL was validated for compatibility with the following software and tools:

| Software and tools                                                            | Version      |
| :---                                                                          | :----        |
| ModusToolbox&trade;                                                           |  3.1.0       |
| [core library](https://github.com/Infineon/core-lib)                          |  1.3.1       |
| [device-db](https://github.com/Infineon/device-db)                            |  4.6.0       |
| CMSIS-Core(M)                                                                 |  5.8.0       |
| GCC compiler                                                                  | 11.3.1       |
| IAR compiler                                                                  |  9.30.1      |
| Arm&reg; compiler 6                                                           |  6.16        |

## More information

- [Peripheral driver library README.md](./README.md)

- [Peripheral driver library API reference manual](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/index.html)

- [ModusToolbox&trade; Software Environment, Quick Start Guide, Documentation, and Videos](https://www.infineon.com/cms/en/design-support/tools/sdk/modustoolbox-software)

- [ModusToolbox&trade; Device Configurator Tool Guide](https://documentation.infineon.com/html/modustoolbox-software/en/latest/tool-guide/ModusToolbox_Device_Configurator_User_Guide.html)

- [AN79953 - Getting started with PSoC&trade; 4](https://www.infineon.com/dgdl/Infineon-AN79953_Getting_Started_with_PSoC_4-ApplicationNotes-v21_00-EN.pdf?fileId=8ac78c8c7cdc391c017d07271fd64bc1&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-an_vanitylink)

- [PSoC&trade; 4 technical reference manuals](https://www.infineon.com/cms/en/search.html#!term=all&view=all&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-doc_search)

- [PSoC&trade; 4 datasheets](https://www.infineon.com/cms/en/search.html#!term=all&view=all&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-doc_search)

- [PMG1 device family](https://www.infineon.com/cms/en/product/universal-serial-bus-usb-power-delivery-controller/usb-c-and-power-delivery/ez-pd-pmg1-portfolio-high-voltage-mcus-usb-c-power-delivery/?utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-product_families)

---
© 2020-2023, Cypress Semiconductor Corporation (an Infineon company) or an affiliate of Cypress Semiconductor Corporation.
