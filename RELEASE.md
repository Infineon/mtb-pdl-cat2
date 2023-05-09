# mtb-pdl-cat2 peripheral driver library v2.4.0

See the [README.md](./README.md) and the
[PDL API reference manual](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/index.html)
for a complete description of the Peripheral driver library.

## What's included?

### New Features

Support for new devices of the PSoC 4100S Plus 256KB device family:
- CY8C4128LQI-S446
- CY8C4128LQQ-S456
- CY8C4148LQI-S446
- CY8C4148LQQ-S456
- CY8C4546LQI-S476
- CY8C4546LQQ-S476
- CY8C4547LQI-S476
- CY8C4547LQQ-S476
- CY8C4548LQI-S486
- CY8C4548LQQ-S486

Support for new devices of the CCG7S device series:
- CYPD7199-40LDXS

Support for new devices of the WLC1 device series:
- WLC1250-68LQXQ

### Updated personalities

- CANFD-1.0, COMP-1.0, COUNTER-1.0, EZI2C-1.0, I2C-1.0, I2S-1.0, LPCOMP-1.0, 
  MXEZI2C-1.0, MXI2C-1.0, MXSPI-1.0, MXUART-1.0, PWM-1.0, QUADDEC-1.0,
  SAR-3.0, SPI-1.0, UART-1.0 - removed obsolete code.

- MXUART-1.0 - CTS Polarity and RTS Polarity depend on the parameter "Enable Flow Control".

### Updated drivers

- [USBPD (USB Power Delivery) 2.40](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__usbpd.html)
- [SCB (Serial Communication Block) 4.30](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__scb.html)
- [CAN FD 1.10.1](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__canfd.html)

## Defect fixes

See the Changelog section of each driver in [PDL API reference](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/modules.html) for all fixes and updates.

## Supported software and tools

This version of PDL was validated for compatibility with the following software and tools:

| Software and tools                                                            | Version      |
| :---                                                                          | :----        |
| ModusToolbox&trade;                                                           |  3.0.0       |
| [core library](https://github.com/Infineon/core-lib)                          |  1.3.1       |
| [device-db](https://github.com/Infineon/device-db)                            |  4.2.0       |
| CMSIS-Core(M)                                                                 |  5.8.0       |
| GCC compiler                                                                  | 10.3.1       |
| IAR compiler                                                                  |  9.30.1      |
| Arm&reg; compiler 6                                                           |  6.16        |

## More information

- [Peripheral driver library README.md](./README.md)

- [Peripheral driver library API reference manual](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/index.html)

- [ModusToolbox&trade; Software Environment, Quick Start Guide, Documentation, and Videos](https://www.infineon.com/cms/en/design-support/tools/sdk/modustoolbox-software)

- [ModusToolbox&trade; Device Configurator Tool Guide](https://documentation.infineon.com/html/modustoolbox-software/en/latest/tool-guide/ModusToolbox_Device_Configurator_Guide.html)

- [AN79953 - Getting started with PSoC&trade; 4](https://www.infineon.com/dgdl/Infineon-AN79953_Getting_Started_with_PSoC_4-ApplicationNotes-v21_00-EN.pdf?fileId=8ac78c8c7cdc391c017d07271fd64bc1&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-an_vanitylink)

- [PSoC&trade; 4 technical reference manuals](https://www.infineon.com/cms/en/search.html#!term=all&view=all&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-doc_search)

- [PSoC&trade; 4 datasheets](https://www.infineon.com/cms/en/search.html#!term=all&view=all&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-doc_search)

- [PMG1 device family](https://www.infineon.com/cms/en/product/universal-serial-bus-usb-power-delivery-controller/usb-c-and-power-delivery/ez-pd-pmg1-portfolio-high-voltage-mcus-usb-c-power-delivery/?utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-product_familieshttps://www.infineon.com/cms/en/product/universal-serial-bus-usb-power-delivery-controller/usb-c-and-power-delivery/ez-pd-pmg1-portfolio-high-voltage-mcus-usb-c-power-delivery/?utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-product_families)

---
© 2020-2023, Cypress Semiconductor Corporation (an Infineon company) or an affiliate of Cypress Semiconductor Corporation.
