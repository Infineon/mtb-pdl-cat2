# MTB CAT2 Peripheral Driver Library v1.1.0

Please refer to the [README.md](./README.md) and the
[PDL API Reference Manual](https://cypresssemiconductorco.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/index.html)
for a complete description of the Peripheral Driver Library.

## What's Included?

### New Features

Add support of the following PMG1 device series (full device support available
starting from ModusToolbox 2.3):

* PMG1-S0
* PMG1-S1
* PMG1-S2
* PMG1-S3

### New Personalities

* USBPD 1.0

### Updated Personalities

* Counter - Minor enhancements of the input parameters check.
* SPI - Enhanced TX/RX FIFO Level default values.
* UART - Enhanced several default values and added new parameter for LIN Mode.
* PWM - Minor enhancements of the input parameters check.
* Quadrature Decoder - Minor enhancements of the input parameters check.
* SAR - Enhanced code generation for vrefSel field of the configuration structure.

### Added Drivers

* [USBPD 1.0 (USB Power Delivery)](https://cypresssemiconductorco.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__usbpd.html)

### Updated Drivers

* [SCB 3.0](https://cypresssemiconductorco.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__scb.html)
* [SysTick 1.20](https://cypresssemiconductorco.github.io/mtb-pdl-cat1/pdl_api_reference_manual/html/group__group__arm__system__timer.html)
* [SysLib 2.10](https://cypresssemiconductorco.github.io/mtb-pdl-cat1/pdl_api_reference_manual/html/group__group__syslib.html)

### Drivers with patch version updates

* [SAR 1.0](https://cypresssemiconductorco.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__sar.html)
* [Startup 1.10](https://cypresssemiconductorco.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__system__config.html)
* [WDT 1.0](https://cypresssemiconductorco.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__wdt.html)

## Known Issues/Limitations

No known issues

## Defect Fixes

See the Changelog section of each Driver in the [PDL API Reference](https://cypresssemiconductorco.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/modules.html) for all fixes and updates.

## Supported Software and Tools

This version of PDL was validated for compatibility with the following Software and Tools:

| Software and Tools                                                            | Version      |
| :---                                                                          | :----        |
| [Cypress Core Library](https://github.com/cypresssemiconductorco/core-lib)    | 1.1.4        |
| CMSIS-Core(M)                                                                 | 5.4.0        |
| GCC Compiler                                                                  | 9.3.1        |
| IAR Compiler                                                                  | 8.42.2       |
| ARM Compiler 6                                                                | 6.13         |

## More information

* [Peripheral Driver Library README.md](./README.md)
* [Peripheral Driver Library API Reference Manual](https://cypresssemiconductorco.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/index.html)
* [ModusToolbox Software Environment, Quick Start Guide, Documentation, and Videos](https://www.cypress.com/products/modustoolbox-software-environment)
* [ModusToolbox Device Configurator Tool Guide](https://www.cypress.com/ModusToolboxDeviceConfig)
* [AN79953 - Getting Started with PSoC® 4](https://www.cypress.com/an79953)
* [PSoC 4 Technical Reference Manuals](https://www.cypress.com/search/all?f%5B0%5D=meta_type%3Atechnical_documents&f%5B1%5D=resource_meta_type%3A583&f%5B2%5D=field_related_products%3A1314)
* [PSoC 4 MCU Datasheets](https://www.cypress.com/search/all?f%5B0%5D=meta_type%3Atechnical_documents&f%5B1%5D=field_related_products%3A1297&f%5B2%5D=resource_meta_type%3A575)
* [PMG1 Device Family](http://www.cypress.com/PMG1)
* [Cypress Semiconductor](http://www.cypress.com)

---
© Cypress Semiconductor Corporation, 2019-2021.