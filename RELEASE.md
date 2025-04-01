# mtb-pdl-cat2 peripheral driver library v2.16.0

See the [README.md](./README.md) and the
[PDL API reference manual](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/index.html)
for a complete description of the Peripheral driver library.

## What's included?

### New Features
- Added CAN2.0B driver for the PSOC&trade; 4100S Plus devices.
Please refer to the [New drivers](#new-drivers) section below for details.
- Added support of PSOC&trade; 4100T Plus device family.
- Updated the USBPD driver to provide APIs to Enable/Disable the ilimit block for CFP devices.

### Updated personalities
- SAR 4.0: The 'Minimum Acquisition Time' parameter range is fixed: now the minimal valid value is 194ns.
- Added new CAN 1.0 personalities for PSOC&trade; 4100S Plus Platform.

### Updated drivers
- [SAR 2.60](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__sar.html)
- [SysClk 3.40](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__sysclk.html)
- [Cy_Flash 1.20](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__flash.html) - Enabled flash non-blocking write operations for devices with 1 flash array.
- [SCB 4.60](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__scb.html)
- [Cryptolite 1.40](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__cryptolite.html)
- [USBPD 2.110](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__usbpd.html)

### New drivers
- [CAN 1.0](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__can.html)

## Known Issues
* PDL flash write APIs are not supported when using ARM C6 Compiler in debug mode. Please use ARM C6 Compiler in release mode if flash write is needed.

## Defect fixes

See the Changelog section of each driver in [PDL API reference](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/modules.html) for all fixes and updates.

## Supported software and tools

This version of PDL was validated for compatibility with the following software and tools:

| Software and tools                                                            | Version      |
| :---                                                                          | :----        |
| ModusToolbox&trade;                                                           |  3.4.0       |
| [core library](https://github.com/Infineon/core-lib)                          |  1.4.3       |
| [device-db](https://github.com/Infineon/device-db)                            |  4.21.0      |
| CMSIS-Core(M)                                                                 |  5.8.0       |
| GCC compiler                                                                  |  11.3.1      |
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
