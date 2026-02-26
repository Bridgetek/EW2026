# EVE-MCU-Dev Ports

[Back](../README.md)

## Porting EVE-MCU-Dev

The EVE-MCU-Dev library is intended for simple porting to MCU and embedded environments. There are several common target types which are provided with this library:

| Port Name | Platform Macro | Port Directory | Variant | Test Status (1) |
| --- | --- | --- | --- | --- |
| Raspberry Pi Pico (SDK and VS Code) | `PLATFORM_RP2040` | [eve_arch_rpi](eve_arch_rpi/README.md#hardware-rp2040) | RP2040 | Pass |


- (1) Key for Test Status
  - Pass: Has been compiled with the latest toolchains and run on hardware across of EVE generations.
  - Under Test: Compiles with the latest toolchain but hardware testing is ongoing.

For most parts a generic variant of the device has been chosen. The specific device variant is shown in the table above.

Note: The Rasberrry Pi and BeagleBone Black ports are based on Linux builds and are suitable for most embedded Linux platforms by modifying the GPIO pins and device names to match the CPU and board package.
