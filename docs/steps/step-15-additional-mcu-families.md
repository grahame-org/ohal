# Step 15 – Additional MCU Families and Models

**Goal:** Systematically expand platform coverage by adding new MCU families and additional
models within existing families, following the same pattern established in Steps 6, 8, and 10.
Each new family or model requires gathering hardware register information before any code is
written.

**Prerequisites:** [Step 3 (Conventional Commits / Merge Queue)](step-03-conventional-commits-merge-queue.md)
and [Step 12 (CI)](step-12-ci.md) must be in place so that each new family or model contribution
is delivered via a conventional commit PR and passes the required merge checks.
[Step 13 (Release Automation)](step-13-release-automation.md) and
[Step 14 (vcpkg)](step-14-vcpkg-package.md) must be in place so that each contribution is
included in the correct release and packaging flow.

## 15.1 New MCU Families

The table below lists candidate families, their architectures, and the GPIO register style.
Families are listed in suggested implementation order: start with 32-bit families that use a
register layout similar to STM32 (less mapping work), then move to more architecturally distinct
families.

| Family define          | MCU family                        | Architecture             | Register width | GPIO style                                         | Toolchain           |
| ---------------------- | --------------------------------- | ------------------------ | -------------- | -------------------------------------------------- | ------------------- |
| `OHAL_FAMILY_TI_MSPM0` | TI MSPM0 (e.g. MSPM0G3507)        | ARM Cortex-M0+           | 32-bit         | DOUT/DIN/DOE with set/clear/toggle registers       | `arm-none-eabi-g++` |
| `OHAL_FAMILY_NRF5`     | Nordic nRF5x (e.g. nRF52840)      | ARM Cortex-M4F           | 32-bit         | DIR/OUT/OUTSET/OUTCLR/IN + PIN_CNF array           | `arm-none-eabi-g++` |
| `OHAL_FAMILY_RP2040`   | Raspberry Pi RP2040               | ARM Cortex-M0+ dual-core | 32-bit         | SIO (GPIO_OUT/OE with set/clr/xor) + pad control   | `arm-none-eabi-g++` |
| `OHAL_FAMILY_PIC`      | Microchip PIC18 (e.g. PIC18F4550) | 8-bit Harvard RISC       | 8-bit          | PORT (input), LAT (output), TRIS (direction)       | Microchip XC8       |
| `OHAL_FAMILY_AVR`      | Microchip AVR (e.g. ATmega328P)   | AVR 8-bit RISC           | 8-bit          | DDR (direction), PORT (output), PIN (input/toggle) | `avr-g++`           |
| `OHAL_FAMILY_SAMD`     | Microchip SAM D (e.g. SAMD21G18)  | ARM Cortex-M0+           | 32-bit         | PORT with DIRSET/DIRCLR/OUTSET/OUTCLR/IN           | `arm-none-eabi-g++` |

### Inputs Required Before Implementing Each New Family

For each family, gather from the official datasheet or technical reference manual:

- Full list of supported models (for `family.hpp` validation)
- Base addresses for all GPIO ports
- Register layout and offsets for all GPIO control registers
- Per-pin field width (1 bit or 2 bits per pin, depending on register)
- Access type for each register (RO / WO / RW)
- Default pin state at power-on reset
- Which GPIO features are unsupported (affects `capabilities.hpp` specialisations)

### Implementation Checklist for Each New Family

1. Create `platforms/<family>/family.hpp` — `#error` if no model defined, then include model
   header(s).
2. Create `platforms/<family>/models/<model>/gpio.hpp` — partial specialisations of
   `ohal::gpio::Pin<Port, PinNum>` using `Register<Addr, T>` and `BitField<>`.
3. Create `platforms/<family>/models/<model>/capabilities.hpp` — specialise capability traits
   for the features this model supports.
4. Add the family to `platform.hpp`'s `#if … #elif` chain.
5. Update `platform.hpp`'s top-level `#error` message to list the new family flag.
6. Add host unit tests with mock registers (`tests/host/test_gpio_<model>.cpp`).
7. Add cross-compile CI job (see [Step 12](step-12-ci.md)).

No changes to peripheral interface headers under `include/ohal/` are required beyond
updating `include/ohal/platform.hpp`.

## 15.2 Additional Models Within Existing Families

When adding a new model to an already-supported family (e.g. STM32U073 alongside STM32U083):

1. Gather the register map differences between the new model and the existing model from the
   datasheet.
2. If the register layout is identical, create a thin `models/<new_model>/gpio.hpp` that
   `#include`s the shared register definitions from the existing model and adjusts only the
   differing addresses.
3. Add the new model to `family.hpp`'s model validation list.
4. Update the top-level `#error` message if the supported model list has changed significantly.
5. Add or extend host tests to cover the new model.

### Candidate Additional Models

| Family      | Additional models          | Notes                                                                           |
| ----------- | -------------------------- | ------------------------------------------------------------------------------- |
| STM32U0     | STM32U073, STM32U031       | Same GPIO register layout as STM32U083; different base addresses and pin counts |
| MSP430FR2XX | MSP430FR2353, MSP430FR2310 | Subset of MSP430FR2355 port count; same PxIN/PxOUT/PxDIR/PxREN layout           |
| TI_MSPM0    | MSPM0L1306                 | Fewer GPIO ports than MSPM0G3507; same register layout                          |

## 15.3 Cross-Platform Consistency Validation

After each new family or model is added:

- Build `tests/host/test_gpio_crossplatform.cpp` against all supported models in the same
  build configuration to verify the generic API contract is unchanged.
- Verify that calling an unsupported feature on the new model produces a `static_assert` with
  the correct error message prefix (`ohal:`).
- Confirm the new cross-compile CI job passes (see [Step 12](step-12-ci.md)).
- The contribution PR title must follow the conventional commit convention
  (see [Step 3](step-03-conventional-commits-merge-queue.md)), e.g.:

  ```text
  feat(nrf5): add nRF52840 GPIO partial specialisations
  feat(stm32u0): add STM32U073 model support
  ```
