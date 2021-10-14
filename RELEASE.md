# mtb-pdl-cat2 peripheral driver library v1.4.0

See the [README.md](./README.md) and the
[PDL API reference manual](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/index.html)
for a complete description of the Peripheral driver library.

## What's included?
### Updated personalities

- Follower - Updated the Output Connection list.
- MSC - Minor updates.
- OpAmp - Updated the Output Connection list
- WDT - Added the check to ensure the clock source (LFCLK) is enabled.
- SYSTICK - Added the Reload Value field to configure the interrupt interval. The configuration is applied by the Cy_SysTick_Init() call in generated code.
- SAR - The trigMode generation is fixed, rangeIntrEn and satIntrEn generation is added.
- SmartIO - Fixed the SmartIO_HW generation.
- SysAnalog - Removed redundant parameter "Store Config in Flash".

### Updated drivers

- [DMAC 1.10](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__dmac.html)

- [SAR (SAR ADC) 2.10](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__sar.html)

- [SCB 3.20](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__scb.html)

- [SysClk (System Clock) 2.10](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__sysclk.html)

- [SysLib (System Library) 2.20](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__syslib.html)

- [SYSPM 2.10](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__syspm.html)

- [USBPD (USB Power Delivery) 1.20](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__usbpd.html)

### Drivers with patch-version updates

- [SYSTICK 1.20.1](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__arm__system__timer.html)

## Known issues/limitations
The DMA personality does not reserve Hardware Abstraction Layer (HAL) resources.
So, HAL may reserve DMA resources already reserved by the DMA personality and
corrupt the DMA resources configuration. To avoid the issue, use exclusively
either personality or HAL for the DMA configuration.

## Defect fixes

See the Changelog section of each driver in [PDL API reference](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/modules.html) for all fixes and updates.

## Supported software and tools

This version of PDL was validated for compatibility with the following software and tools:

| Software and tools                                                            | Version      |
| :---                                                                          | :----        |
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
