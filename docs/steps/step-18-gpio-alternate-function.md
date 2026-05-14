# Step 18 – GPIO Alternate Function Selection

**Goal:** Add `set_alternate_function(uint8_t af_number)` to `GpioPortPinImpl` and every
existing STM32U0 package-level `Pin<>` specialisation, enabling application code to select
the AF multiplexer setting for a pin without naming any platform register directly.

This step is a prerequisite for Step 20 (UART implementation) and any future peripheral that
requires pins to be configured in Alternate Function mode — I2C, SPI, TIM PWM outputs, etc.

**Prerequisites:** [Step 8 (STM32U0 GPIO)](step-08-stm32u0-gpio.md) must be complete:
`GpioPortPinImpl`, `GpioPortRegs`, and all package-level `Pin<>` specialisations must
already exist. The `Afrl` and `Afrh` register-type aliases inside `GpioPortRegs` are already
defined; this step consumes them.

---

## Inputs Required

### STM32U083 AFRL / AFRH register map

The following details are taken from the CMSIS device header `stm32u083xx.h`, which reflects
the silicon register map. No reference-manual look-up is required for the addresses or field
widths; however, the **valid alternate-function numbers (0–15) and their peripheral
assignments per pin** must be sourced from RM0503 Annex A ("Alternate function mapping") before
any test or application code can verify that a given AF value is wired correctly.

| Register | Offset from GPIO port base | Access | Description                                  |
| -------- | -------------------------- | ------ | -------------------------------------------- |
| `AFRL`   | `0x20`                     | RW     | Alternate-function low register — pins 0–7   |
| `AFRH`   | `0x24`                     | RW     | Alternate-function high register — pins 8–15 |

**Field layout (identical for AFRL and AFRH):** 4 bits per pin, packed sequentially from bit 0.

| Pin within register          | Bit range |
| ---------------------------- | --------- |
| Pin 0 (AFRL) / Pin 8 (AFRH)  | `[3:0]`   |
| Pin 1 (AFRL) / Pin 9 (AFRH)  | `[7:4]`   |
| Pin 2 (AFRL) / Pin 10 (AFRH) | `[11:8]`  |
| Pin 3 (AFRL) / Pin 11 (AFRH) | `[15:12]` |
| Pin 4 (AFRL) / Pin 12 (AFRH) | `[19:16]` |
| Pin 5 (AFRL) / Pin 13 (AFRH) | `[23:20]` |
| Pin 6 (AFRL) / Pin 14 (AFRH) | `[27:24]` |
| Pin 7 (AFRL) / Pin 15 (AFRH) | `[31:28]` |

Both `AFRL` and `AFRH` are already present in `GpioPortRegs<Base>` in
`include/ohal/platforms/stm32u0/models/stm32u083/gpio_impl.hpp`.

### RM0503 look-up required before implementation

> Before writing any test that validates a specific AF number, gather the following from
> **RM0503 Annex A (Alternate function mapping)**:
>
> - The AF number for **I2C1_SCL** on **PB8** (expected: AF4, but must be confirmed).
> - The AF number for **I2C1_SDA** on **PB9** (expected: AF4, but must be confirmed).
> - The AF number for **USART2_TX** on **PA2** (needed for Step 20).
> - The AF number for **USART2_RX** on **PA3** (needed for Step 20).
>
> These values drive the acceptance tests for this step.

---

## Implementation

### 1. Add `set_alternate_function` to `GpioPortPinImpl`

Modify `include/ohal/platforms/stm32u0/models/stm32u083/gpio_impl.hpp`.

The method selects between `AFRL` (pins 0–7) and `AFRH` (pins 8–15) using `if constexpr`, so
the compiler eliminates the untaken branch entirely.

```cpp
// Inside GpioPortPinImpl<PinNum, Regs> — add after the existing set_pull() method

/// Selects the alternate-function multiplexer setting for this pin.
///
/// Writes the 4-bit AF number to the correct field of AFRL (pins 0–7)
/// or AFRH (pins 8–15).  The effect is only meaningful when the pin
/// is also configured with set_mode(PinMode::AlternateFunction).
///
/// @param af_number  AF number in the range [0, 15].  Values correspond
///                   to AF0–AF15 in RM0503 Annex A.  Values outside
///                   [0, 15] produce undefined hardware behaviour.
static void set_alternate_function(uint8_t af_number) noexcept {
    static_assert(
        ohal::gpio::capabilities::supports_alternate_function<
            /* Port deduced by the surrounding specialisation */,
            PinNum>::value,
        "ohal: this pin does not support alternate function selection "
        "on the selected MCU package.");

    if constexpr (PinNum < 8U) {
        using AfField = ohal::core::BitField<
            typename Regs::Afrl,
            static_cast<uint8_t>(PinNum * 4U),
            4U,
            ohal::core::Access::ReadWrite>;
        AfField::write(af_number);
    } else {
        using AfField = ohal::core::BitField<
            typename Regs::Afrh,
            static_cast<uint8_t>((PinNum - 8U) * 4U),
            4U,
            ohal::core::Access::ReadWrite>;
        AfField::write(af_number);
    }
}
```

> **Note on the `static_assert` port parameter:** `GpioPortPinImpl` is a helper type
> parameterised on `PinNum` and `Regs` — it does not carry a port tag. The port tag is
> available in the surrounding `Pin<Port, PinNum>` partial specialisation that delegates
> to `GpioPortPinImpl`. Move the `static_assert` into the `Pin<>` specialisation wrapper
> rather than inside `GpioPortPinImpl`, so that the port tag is in scope.

### 2. Propagate through package specialisations

Every existing package-level `gpio.hpp` that defines `Pin<PortX, PinNum>` as a thin
delegation to `GpioPortPinImpl` will automatically expose `set_alternate_function` once
`GpioPortPinImpl` provides it. No per-package changes are required to the method body.

If any package wraps `GpioPortPinImpl` with additional `static_assert` guards on
`supports_alternate_function`, verify those guards compile cleanly after this change.

### 3. No changes required to `include/ohal/gpio.hpp`

The generic `Pin<>` primary template uses `static_assert(sizeof(Port) == 0, ...)` and is
never instantiated. The method is provided entirely through partial specialisations in the
platform layer, consistent with the existing `set_mode`, `set_pull`, etc. methods.

---

## Tests to Write (host)

Add test cases to the existing STM32U083 host-test files following the mock-register pattern
from [Step 11](step-11-unit-testing.md).

| Test                                        | Register written            | Expected value |
| ------------------------------------------- | --------------------------- | -------------- |
| `Pin<PortA, 0>::set_alternate_function(3)`  | `GPIOA_AFRL` bits `[3:0]`   | `3`            |
| `Pin<PortA, 5>::set_alternate_function(1)`  | `GPIOA_AFRL` bits `[23:20]` | `1`            |
| `Pin<PortA, 7>::set_alternate_function(15)` | `GPIOA_AFRL` bits `[31:28]` | `15`           |
| `Pin<PortB, 8>::set_alternate_function(4)`  | `GPIOB_AFRH` bits `[3:0]`   | `4`            |
| `Pin<PortB, 9>::set_alternate_function(4)`  | `GPIOB_AFRH` bits `[7:4]`   | `4`            |
| `Pin<PortB,15>::set_alternate_function(0)`  | `GPIOB_AFRH` bits `[31:28]` | `0`            |

Confirm that writing AF4 to PB8 and PB9 leaves all other bits in `GPIOB_AFRH` unchanged
(the mock register must be initialised to a known sentinel value before each test).

### Negative-compile tests

- Calling `set_alternate_function` on a pin whose `supports_alternate_function` specialisation
  is `false_type` must produce a `static_assert` failure. Add at least one such test for an
  unsupported pin on the smallest package that omits a port (e.g. a pin on a port that is not
  bonded out).

---

## Acceptance Criteria

1. `GpioPortPinImpl<5, GpioA>::set_alternate_function(1)` writes `1` to bits `[23:20]` of
   `GPIOA_AFRL` and leaves all other bits of `AFRL` and `AFRH` unchanged.
2. `GpioPortPinImpl<8, GpioB>::set_alternate_function(4)` writes `4` to bits `[3:0]` of
   `GPIOB_AFRH` and leaves all other bits unchanged.
3. `GpioPortPinImpl<9, GpioB>::set_alternate_function(4)` writes `4` to bits `[7:4]` of
   `GPIOB_AFRH` independently of the preceding test.
4. All existing host tests continue to pass (no regressions to `set_mode`, `set()`, `clear()`,
   etc.).
5. `bash lint.sh` reports no errors.

---

## `test_project` Conversion

After this step, replace the `HAL_GPIO_Init(GPIOB, ...)` call in `MX_GPIO_Init` for the
I2C1 SCL and SDA pins:

```cpp
// Before (ST HAL)
GPIO_InitTypeDef GPIO_InitStruct = { 0 };
GPIO_InitStruct.Pin       = I2C1_SCL_Pin | I2C1_SDA_Pin;   // PB8, PB9
GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
GPIO_InitStruct.Pull      = GPIO_NOPULL;
GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

// After (ohal) — AF number confirmed from RM0503 Annex A before writing this code
using Scl = ohal::gpio::Pin<ohal::gpio::PortB, 8U>;
using Sda = ohal::gpio::Pin<ohal::gpio::PortB, 9U>;
Scl::set_output_type(ohal::gpio::OutputType::OpenDrain);
Scl::set_pull(ohal::gpio::Pull::None);
Scl::set_speed(ohal::gpio::Speed::Low);
Scl::set_alternate_function(/* AF number from RM0503 */);
Scl::set_mode(ohal::gpio::PinMode::AlternateFunction);
Sda::set_output_type(ohal::gpio::OutputType::OpenDrain);
Sda::set_pull(ohal::gpio::Pull::None);
Sda::set_speed(ohal::gpio::Speed::Low);
Sda::set_alternate_function(/* AF number from RM0503 */);
Sda::set_mode(ohal::gpio::PinMode::AlternateFunction);
```

The `GPIO_InitTypeDef` variable, the `#include` of `stm32u0xx_hal_gpio.h` (if it was
included solely for this call), and the I2C-related BSP pin-name macros can be removed
once this conversion is complete.
