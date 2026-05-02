# Supported MCUs

This page lists the MCU families and models that ohal currently supports, along with their
implementation status and the compiler toolchain required to build for each target.

## How to read the status column

| Status      | Meaning                                                              |
| ----------- | -------------------------------------------------------------------- |
| ✅ Complete | Register map and `Pin<>` specialisations are implemented and tested. |
| 🔧 Partial  | Register map headers exist; `Pin<>` specialisations are in progress. |
| 📋 Planned  | Included in the roadmap; implementation has not started.             |

## STM32U0 family — `OHAL_FAMILY_STM32U0`

ARM Cortex-M0+ family from STMicroelectronics. 32-bit registers. Toolchain: `arm-none-eabi-g++`.

| Model define           | Device    | GPIO ports           | Status     |
| ---------------------- | --------- | -------------------- | ---------- |
| `OHAL_MODEL_STM32U083` | STM32U083 | PA–PF (16 pins each) | 🔧 Partial |
| `OHAL_MODEL_STM32U073` | STM32U073 | PA–PF                | 📋 Planned |

**Supported GPIO features on STM32U083:**

| Feature                               | Supported                         |
| ------------------------------------- | --------------------------------- |
| `set_mode()`                          | 🔧 Partial (register map defined) |
| `set_output_type()`                   | 🔧 Partial                        |
| `set_speed()`                         | 🔧 Partial                        |
| `set_pull()`                          | 🔧 Partial                        |
| `set()` / `clear()` (atomic via BSRR) | 🔧 Partial                        |
| `read_input()`                        | 🔧 Partial                        |
| `toggle()`                            | 🔧 Partial                        |

### Selecting an STM32U0 model

```cmake
target_compile_definitions(my_app PRIVATE
    OHAL_FAMILY_STM32U0
    OHAL_MODEL_STM32U083
)
```

```sh
arm-none-eabi-g++ -DOHAL_FAMILY_STM32U0 -DOHAL_MODEL_STM32U083 -std=c++17 ...
```

## PIC family — `OHAL_FAMILY_PIC`

Microchip PIC18 8-bit Harvard RISC family. 8-bit registers (`uint8_t`). Toolchain: Microchip XC8.

| Model define            | Device     | GPIO ports | Status     |
| ----------------------- | ---------- | ---------- | ---------- |
| `OHAL_MODEL_PIC18F4550` | PIC18F4550 | PA–PE      | 📋 Planned |

**Supported GPIO features on PIC18F4550:**

| Feature                                | Supported                                         |
| -------------------------------------- | ------------------------------------------------- |
| `set_mode()`                           | 📋 Planned (via TRIS register, inverted polarity) |
| `set_output_type()`                    | ❌ Not available — compile error                  |
| `set_speed()`                          | ❌ Not available — compile error                  |
| `set_pull()`                           | ❌ Not available — compile error                  |
| `set()` / `clear()` (via LAT register) | 📋 Planned                                        |
| `read_input()` (via PORT register)     | 📋 Planned                                        |
| `toggle()`                             | 📋 Planned                                        |

## TI MSPM0 family — `OHAL_FAMILY_TI_MSPM0`

ARM Cortex-M0+ family from Texas Instruments. 32-bit registers. Toolchain: `arm-none-eabi-g++`.

| Model define            | Device     | GPIO ports | Status     |
| ----------------------- | ---------- | ---------- | ---------- |
| `OHAL_MODEL_MSPM0G3507` | MSPM0G3507 | —          | 📋 Planned |

## Future families

The following families are on the long-term roadmap. No implementation has started.

| Family define        | MCU family                       | Architecture               |
| -------------------- | -------------------------------- | -------------------------- |
| `OHAL_FAMILY_NRF5`   | Nordic nRF5x (e.g. nRF52840)     | ARM Cortex-M4F             |
| `OHAL_FAMILY_RP2040` | Raspberry Pi RP2040              | ARM Cortex-M0+ (dual-core) |
| `OHAL_FAMILY_AVR`    | Microchip AVR (e.g. ATmega328P)  | AVR 8-bit RISC             |
| `OHAL_FAMILY_SAMD`   | Microchip SAM D (e.g. SAMD21G18) | ARM Cortex-M0+             |

## Invalid define combinations

Passing conflicting or missing defines causes a compile error. Common mistakes:

| Defines passed                                  | Error                                  |
| ----------------------------------------------- | -------------------------------------- |
| (none)                                          | `ohal: No MCU family defined.`         |
| `OHAL_FAMILY_STM32U0` only                      | `ohal: No STM32U0 model defined.`      |
| `OHAL_FAMILY_STM32U0` + `OHAL_MODEL_PIC18F4550` | `ohal: Multiple MCU families defined.` |
| `OHAL_FAMILY_STM32U0` + `OHAL_FAMILY_PIC`       | `ohal: Multiple MCU families defined.` |
