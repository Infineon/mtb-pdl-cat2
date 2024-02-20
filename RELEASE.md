# mtb-pdl-cat2 peripheral driver library v2.8.0

See the [README.md](./README.md) and the
[PDL API reference manual](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/index.html)
for a complete description of the Peripheral driver library.

## What's included?

### New Features

- New MPNs of CCG7S Device Family were added.

- Updated Cryptolite driver to add support for:
    - SHA-384 and SHA-512
    - RSA signature verification using the provided public key
    
- USBPD Driver:
    - Corrected REFGEN control settings for PMG1S3
    - Updated configuration table space for Dock solutions
    - Updates related to HPD handling for PMG1S3

- SCB Driver:
    - I2C: Interrupt logic updated to handle the slave not ready condition
    - Provided new function Cy_SCB_I2C_SlaveSendAckNack() that the user application can send ACK/NACK and configure buffer

### Updated personalities

- MXEZI2C, MXI2C, MXSPI, MXUART, SPI, UART - Resolved issue with empty scb connections

### Updated drivers

- [USBPD 2.70](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__usbpd.html)
- [Cryptolite 1.30](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__cryptolite.html)
- [SCB 4.50](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__scb.html)

## Known Issues

When WLC1115-68LQXQ or WLC1150-68LQXQ based project is compiled by IAR compiler the following warning  issued: "/builds/repo/bsp_csp/output/libs/COMPONENT_CAT2/mtb-pdl-cat2/drivers/source/cy_usbpd_vbus_ctrl.c",4052: dynamic initialization in unreachable code. This warning do not cause any system malfunction.

## Defect fixes

See the Changelog section of each driver in [PDL API reference](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/modules.html) for all fixes and updates.

## Supported software and tools

This version of PDL was validated for compatibility with the following software and tools:

| Software and tools                                                            | Version      |
| :---                                                                          | :----        |
| ModusToolbox&trade;                                                           |  3.1.0       |
| [core library](https://github.com/Infineon/core-lib)                          |  1.4.1       |
| [device-db](https://github.com/Infineon/device-db)                            |  4.11.0      |
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
© 2020-2024, Cypress Semiconductor Corporation (an Infineon company) or an affiliate of Cypress Semiconductor Corporation.
