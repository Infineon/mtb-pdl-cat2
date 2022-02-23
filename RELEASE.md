# mtb-pdl-cat2 peripheral driver library v1.5.0

See the [README.md](./README.md) and the
[PDL API reference manual](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/index.html)
for a complete description of the Peripheral driver library.

## What's included?

### Updated personalities

- SCB (UART, I2C, SPI, EZI2C) - Corrected SCB personalities that did offer interfaces, that can not be used.
- USBFS -  The DMA endpoint management modes for the USBFS driver, both manual/automatic, have been implemented.
- DMAC - The DMA personality reserves Hardware Abstraction Layer (HAL) resources.
- Pin, Port-Intrinsic - Added port-level configuration option.

### Added drivers

- [CryptoLite 1.0](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__cryptolite.html)

### Updated drivers

- [USBPD (USB Power Delivery) 1.30](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__usbpd.html)

- [CAN FD 1.10](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__canfd.html)

- [SCB 4.0](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__scb.html)

- [GPIO 2.0](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__gpio.html)

- [SAR 2.20](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__sar.html)

- [SysClk (System Clock) 2.20](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__sysclk.html)

- [SYSPM 3.0](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__syspm.html)

- [USBFS (USB Full-Speed Device) 1.10](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__usbfs__dev__drv.html)

### Drivers with patch-version updates

- Updated patch version for all drivers because updated the paths to the code snippets.

## Known Issues

* Keep the Enable CSDADC parameter of the CSD personality disabled (default state) because at the moment, the CSDADC middleware does not support PSoC™ 4 families and thus is not present in the Library manager. Enabling the parameter leads to build errors.
* Check manually for newer versions of enabled personalities in the Device Configurator and upgrade them if needed because the personality migration mechanism does not work.

## Defect fixes

See the Changelog section of each driver in [PDL API reference](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/modules.html) for all fixes and updates.

## Supported software and tools

This version of PDL was validated for compatibility with the following software and tools:

| Software and tools                                                            | Version      |
| :---                                                                          | :----        |
| ModusToolbox&#8482;                                                           |  2.3.0       |
| [core library](https://github.com/Infineon/core-lib)                          |  1.2.0       |
| CMSIS-Core(M)                                                                 |  5.4.0       |
| GCC compiler                                                                  | 10.3.1       |
| IAR compiler                                                                  |  8.42.2      |
| Arm&reg; compiler 6                                                           |  6.13        |

## More information

- [Peripheral driver library README.md](./README.md)

- [Peripheral driver library API reference manual](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/index.html)

- [ModusToolbox&trade; software environment, Quick Start guide, documentation, and videos](https://www.cypress.com/products/modustoolbox-software-environment)

- [ModusToolbox&trade; Device Configurator guide](https://www.cypress.com/ModusToolboxDeviceConfig)

- [AN79953 - Getting started with PSoC&trade; 4](https://www.cypress.com/an79953)

- [PSoC&trade; 4 technical reference manuals](https://www.cypress.com/search/all?f%5B0%5D=meta_type%3Atechnical_documents&f%5B1%5D=resource_meta_type%3A583&f%5B2%5D=field_related_products%3A1314)

- [PSoC&trade; 4 datasheets](https://www.cypress.com/search/all?f%5B0%5D=meta_type%3Atechnical_documents&f%5B1%5D=field_related_products%3A1297&f%5B2%5D=resource_meta_type%3A575)

- [PMG1 device family](http://www.cypress.com/PMG1)

---
© 2020-2021, Cypress Semiconductor Corporation (an Infineon company) or an affiliate of Cypress Semiconductor Corporation.
