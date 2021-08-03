# mtb-pdl-cat2 peripheral driver library v1.3.0

See the [README.md](./README.md) and the
[PDL API reference manual](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/index.html)
for a complete description of the peripheral driver library.

## What's included?

### New personalities

- CTBm (supported by ModusToolbox 2.3.1 or later)
- Comparator (supported by ModusToolbox 2.3.1 or later)
- DieTemp (supported by ModusToolbox 2.3.1 or later)
- Emulated EEPROM
- Follower (supported by ModusToolbox 2.3.1 or later)
- Low-Power Comparator
- OpAmp (supported by ModusToolbox 2.3.1 or later)
- Programmable Analog
- Segment LCD
- USBFS Device

### Updated personalities

- CAN FD - Fixed the name of the IRQ define.
- CSD - Added a check if Power personality is enabled. Fixed the typos in the description.
- Counter - Fixed the capture terminal display.
- DMAC - Fixed the typos in the description.
- ECO - Increased the startup timeout because ECO may take longer to become stable.
- EXTCLK - Set the minimum allowed frequency to 0 MHz.
- EXTREF - Corrected the maximum external frequency to 48 MHz
- HFCLK - Updated in accordance with the SysClk driver updates.
- I2C - Added a delay of the address-matching event parameter. Fixed the typos in the description.
- MSC - Improved the DMAC triggers configuration. Fixed the typos in the description.
- PIN - Corrected the name of Analog drive mode.
- PLL - Decreased the startup timeout to match the specification.
- Power - Added the AMUXBUS charge pump.
- SAR - Updated to support interconnection with CTBm and DieTemp (supported by ModusToolbox 2.3.1 or later).
- SPI - Fixed the typos in the description.
- Smart I/O - Fixed the issue in the selection of an instance.
- SysClocks - Fixed the ILO and ECO configurations. Removed unused code.
- UART - Fixed the typos in the description.
- USBPD - Updated the sense-resistance option parameters.
- WCO - Added a nominal frequency and startup delay.

### Added drivers

- [CTB (Continuous time block)](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__ctb.html)

- [Crypto](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__crypto.html)

- [LPCOMP (Low-power comparator)](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__lpcomp.html)

- [SegLCD (Segment LCD)](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__seglcd.html)

- [USBFS (USB Full-Speed Device)](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__usbfs.html)

### Updated drivers

- [CSD (CapSense Sigma Delta) 1.10](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__csd.html)

- [GPIO 1.20](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__gpio.html)

- [MSC (Multi-Sense Converter) 1.10](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__msc.html)

- [SAR (SAR ADC) 2.0](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__sar.html)

- [SCB 3.10](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__scb.html)

- [SysClk (System Clock) 2.0](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__sysclk.html)

- [SysPm (System Power Management) 2.0](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__syspm.html)

- [USBPD (USB Power Delivery) 1.10](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__usbpd.html)

### Drivers with patch version updates

- [CAN FD (CAN with Flexible Data-Rate) 1.0.1](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__canfd.html)

- [DMAC (Direct Memory Access Controller) 1.0.1](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__dmac.html)

- [SysLib (System Library) 2.10.2](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__syslib.html)

- [TCPWM (Timer Counter PWM) 1.10.1](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__tcpwm.html)

- [WDC 1.0.1](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/group__group__wdc.html)

## Known issues/limitations

For the PSoC P4000S, P4100S, P4100S Plus, and PSoC 4500S devices, the SCB in
Master mode may stick in the CY_SCB_I2C_MASTER_BUSY state, if the
Cy_SCB_I2C_MasterAbortWrite() or Cy_SCB_I2C_MasterAbortRead() functions are
called during an address transfer. The SCB instance re-initialization is
required to restore the instance operation.

## Defect fixes

See the Changelog section of each driver in the [PDL API reference](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/modules.html) for all fixes and updates.


## Supported software and tools

This version of PDL was validated for compatibility with the following software and tools:

| Software and tools                                                            | Version      |
| :---                                                                          | :----        |
| [core library](https://github.com/Infineon/core-lib)                          | 1.2.0        |
| CMSIS-Core(M)                                                                 | 5.4.0        |
| GCC compiler                                                                  | 9.3.1        |
| IAR compiler                                                                  | 8.42.2       |
| Arm&reg; compiler 6                                                           | 6.13         |

## More information

- [Peripheral driver library README.md](./README.md)

- [Peripheral driver library API reference manual](https://infineon.github.io/mtb-pdl-cat2/pdl_api_reference_manual/html/index.html)

- [ModusToolbox&trade; software environment, quick start guide, documentation, and videos](https://www.cypress.com/products/modustoolbox-software-environment)

- [ModusToolbox&trade; device configurator tool guide](https://www.cypress.com/ModusToolboxDeviceConfig)

- [AN79953 - Getting started with PSoC&trade; 4](https://www.cypress.com/an79953)

- [PSoC&trade; 4 technical reference manuals](https://www.cypress.com/search/all?f%5B0%5D=meta_type%3Atechnical_documents&f%5B1%5D=resource_meta_type%3A583&f%5B2%5D=field_related_products%3A1314)

- [PSoC&trade; 4 datasheets](https://www.cypress.com/search/all?f%5B0%5D=meta_type%3Atechnical_documents&f%5B1%5D=field_related_products%3A1297&f%5B2%5D=resource_meta_type%3A575)

- [PMG1 device family](http://www.cypress.com/PMG1)

---
© 2020-2021, Cypress Semiconductor Corporation (an Infineon company) or an affiliate of Cypress Semiconductor Corporation.
