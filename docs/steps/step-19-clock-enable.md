# Step 19 – Peripheral Clock Enable (`ohal::clock`)

**Goal:** Implement `ohal::clock::Enable<Peripheral>::enable()` and
`ohal::clock::Enable<Peripheral>::disable()` for the STM32U083 peripherals needed by
`test_project`. This replaces all `__HAL_RCC_*_CLK_ENABLE()` macro calls with zero-overhead
ohal calls that write the correct bit in the correct RCC register.

This step is the focused implementation of the clock-enable pattern described in
[Step 16 section 16.2](step-16-additional-peripherals.md), scoped to the peripherals
actually used by `test_project`. The broader `ohal::clock` work (oscillator/PLL selection,
clock-tree configuration) is covered separately in [Step 22](step-22-rcc-clock-tree-systick.md).

**Prerequisites:** [Step 8 (STM32U0 GPIO)](step-08-stm32u0-gpio.md) must be complete so
that the `PortA`/`PortB`/`PortC`/`PortF` tag types exist as the peripheral keys.
[Step 3 (Conventional Commits)](step-03-conventional-commits-merge-queue.md) must be in
place so that the new `clock.hpp` lands via a `feat(clock):` commit.

---

## Inputs Required

### RCC register map (STM32U083)

The following register offsets and bit positions are taken directly from the CMSIS device
header `stm32u083xx.h` (comments reproduce the "Address offset" annotations from that file).
No additional reference-manual look-up is required for **these specific fields**; however, the
**enable-then-read-back requirement** (whether software must read the register back after
setting an enable bit to guarantee the clock is stable before accessing peripheral registers)
must be confirmed from **RM0503** before writing production code that depends on it.

**RCC base address:** `0x40021000`

#### IOPENR — I/O Port Clock Enable Register (offset `0x4C`)

Enables the AHB clock for each GPIO port. Write-1-to-set; write-0-to-clear.
A read-back after writing 1 is recommended by the ST HAL for `__HAL_RCC_GPIOx_CLK_ENABLE`;
whether this is architecturally required is to be confirmed from RM0503.

| Bit | Name    | Peripheral |
| --- | ------- | ---------- |
| 0   | GPIOAEN | GPIOA      |
| 1   | GPIOBEN | GPIOB      |
| 2   | GPIOCEN | GPIOC      |
| 3   | GPIODEN | GPIOD      |
| 4   | GPIOEEN | GPIOE      |
| 5   | GPIOFEN | GPIOF      |

#### APBENR1 — APB1 Peripheral Clock Enable Register (offset `0x58`)

| Bit | Name      | Peripheral |
| --- | --------- | ---------- |
| 0   | TIM2EN    | TIM2       |
| 1   | TIM3EN    | TIM3       |
| 4   | TIM6EN    | TIM6       |
| 5   | TIM7EN    | TIM7       |
| 7   | LPUART2EN | LPUART2    |
| 17  | USART2EN  | USART2     |
| 20  | LPUART1EN | LPUART1    |
| 21  | I2C1EN    | I2C1       |
| 28  | PWREN     | PWR        |
| 31  | LPTIM1EN  | LPTIM1     |

#### APBENR2 — APB2 Peripheral Clock Enable Register (offset `0x60`)

| Bit | Name     | Peripheral |
| --- | -------- | ---------- |
| 0   | SYSCFGEN | SYSCFG     |
| 11  | TIM1EN   | TIM1       |
| 12  | SPI1EN   | SPI1       |
| 14  | USART1EN | USART1     |
| 16  | TIM15EN  | TIM15      |
| 17  | TIM16EN  | TIM16      |

### RM0503 look-up required before implementation

> Before writing production code or tests:
>
> 1. **Read-back requirement** — confirm from RM0503 section "Peripheral clock enabling" (or
>    equivalent) whether a dummy read of the enable register is required after setting an enable
>    bit to ensure the clock propagates before a subsequent register access. The ST HAL macros
>    include a `tmpreg = READ_BIT(RCC->IOPENR, ...)` step for this purpose; determine whether
>    this is a silicon errata workaround, an architecture requirement, or a defensive pattern.
> 2. **Reset registers** — if `ohal::clock::Reset<Peripheral>::reset()` is to be implemented
>    in this step, gather the corresponding `IOPRSTR`, `AHBRSTR`, `APBRSTR1`, `APBRSTR2`
>    offsets and bit positions from RM0503. (Reset is not required for the test_project
>    conversion but is a natural companion to `Enable`.)

---

## Approach

### Generic interface (`include/ohal/clock.hpp`)

```cpp
#ifndef OHAL_CLOCK_HPP
#define OHAL_CLOCK_HPP

#include <cstdint>

namespace ohal::clock {

/// Enables the peripheral bus clock for @p Peripheral.
///
/// Platform specialisations provide the correct register address and bit.
/// If no specialisation exists for the selected (family, peripheral) pair,
/// compilation fails with a descriptive static_assert.
///
/// @tparam Peripheral  A peripheral tag type.  For GPIO ports, use the
///                     existing ohal::gpio::PortA … PortF tag types.
///                     For other peripherals, use the tag types defined
///                     in their respective interface headers.
template <typename Peripheral>
struct Enable {
    static void enable() noexcept {
        static_assert(sizeof(Peripheral) == 0,
            "ohal: clock::Enable is not implemented for this peripheral "
            "on the selected MCU. Ensure -DOHAL_FAMILY_* and "
            "-DOHAL_MODEL_* are set correctly.");
    }
    static void disable() noexcept {
        static_assert(sizeof(Peripheral) == 0,
            "ohal: clock::Enable is not implemented for this peripheral "
            "on the selected MCU.");
    }
};

} // namespace ohal::clock

#endif // OHAL_CLOCK_HPP
```

### Platform specialisations (`include/ohal/platforms/stm32u0/models/stm32u083/clock.hpp`)

Implement a helper type `ClockEnableImpl<Reg, Bit>` that writes a single bit in a single
register. Each `Enable<>` specialisation is a one-liner delegating to `ClockEnableImpl`.

```cpp
// include/ohal/platforms/stm32u0/models/stm32u083/clock.hpp  (excerpt)
#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_CLOCK_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_CLOCK_HPP

#include <cstdint>
#include "ohal/clock.hpp"
#include "ohal/core/field.hpp"
#include "ohal/core/register.hpp"
#include "ohal/gpio.hpp"

namespace ohal::platforms::stm32u0::stm32u083 {

// RCC base address (stm32u083xx.h, Table 2)
inline constexpr uintptr_t kRccBase = 0x4002'1000U;

// RCC register offsets (stm32u083xx.h struct RCC_TypeDef)
inline constexpr uintptr_t kIopenrOffset  = 0x4CU; ///< I/O port clock enable
inline constexpr uintptr_t kApbenr1Offset = 0x58U; ///< APB1 peripheral clock enable
inline constexpr uintptr_t kApbenr2Offset = 0x60U; ///< APB2 peripheral clock enable

/// Helper: single-bit clock enable/disable in one RCC register.
template <uintptr_t RegAddr, uint8_t Bit>
struct ClockEnableImpl {
    using Reg = ohal::core::Register<RegAddr>;
    static void enable()  noexcept { Reg::write(Reg::read() | (1U << Bit)); }
    static void disable() noexcept { Reg::write(Reg::read() & ~(1U << Bit)); }
};

} // namespace ohal::platforms::stm32u0::stm32u083

// Explicit specialisations of ohal::clock::Enable<> for STM32U083 peripherals
namespace ohal::clock {

// GPIO port clocks (IOPENR)
template <> struct Enable<ohal::gpio::PortA>
    : ohal::platforms::stm32u0::stm32u083::ClockEnableImpl<
        ohal::platforms::stm32u0::stm32u083::kRccBase +
        ohal::platforms::stm32u0::stm32u083::kIopenrOffset, 0U> {};

template <> struct Enable<ohal::gpio::PortB>
    : ohal::platforms::stm32u0::stm32u083::ClockEnableImpl<
        ohal::platforms::stm32u0::stm32u083::kRccBase +
        ohal::platforms::stm32u0::stm32u083::kIopenrOffset, 1U> {};

template <> struct Enable<ohal::gpio::PortC>
    : ohal::platforms::stm32u0::stm32u083::ClockEnableImpl<
        ohal::platforms::stm32u0::stm32u083::kRccBase +
        ohal::platforms::stm32u0::stm32u083::kIopenrOffset, 2U> {};

template <> struct Enable<ohal::gpio::PortF>
    : ohal::platforms::stm32u0::stm32u083::ClockEnableImpl<
        ohal::platforms::stm32u0::stm32u083::kRccBase +
        ohal::platforms::stm32u0::stm32u083::kIopenrOffset, 5U> {};

// APB1 peripheral clocks
// (tag types Usart2, I2c1, Pwr — define these in clock.hpp or each peripheral's header)
template <> struct Enable</* Usart2 tag */>
    : ohal::platforms::stm32u0::stm32u083::ClockEnableImpl<
        ohal::platforms::stm32u0::stm32u083::kRccBase +
        ohal::platforms::stm32u0::stm32u083::kApbenr1Offset, 17U> {};

template <> struct Enable</* I2c1 tag */>
    : ohal::platforms::stm32u0::stm32u083::ClockEnableImpl<
        ohal::platforms::stm32u0::stm32u083::kRccBase +
        ohal::platforms::stm32u0::stm32u083::kApbenr1Offset, 21U> {};

template <> struct Enable</* Pwr tag */>
    : ohal::platforms::stm32u0::stm32u083::ClockEnableImpl<
        ohal::platforms::stm32u0::stm32u083::kRccBase +
        ohal::platforms::stm32u0::stm32u083::kApbenr1Offset, 28U> {};

// APB2 peripheral clocks
template <> struct Enable</* Syscfg tag */>
    : ohal::platforms::stm32u0::stm32u083::ClockEnableImpl<
        ohal::platforms::stm32u0::stm32u083::kRccBase +
        ohal::platforms::stm32u0::stm32u083::kApbenr2Offset, 0U> {};

} // namespace ohal::clock

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_CLOCK_HPP
```

> **Tag type decision:** The peripheral tag types for non-GPIO peripherals (Usart2, I2c1, Pwr,
> Syscfg) must be defined somewhere. The recommended location is a new
> `include/ohal/platforms/stm32u0/models/stm32u083/peripherals.hpp` header that lists all
> peripheral instance tag types for this model. This header is then included by
> `stm32u083/clock.hpp`, `stm32u083/uart.hpp`, etc. Decide and document the tag-type location
> before writing the specialisations.

### Package wrapper

Add a thin wrapper in each package directory (e.g.
`include/ohal/platforms/stm32u0/models/stm32u083rct/clock.hpp`) that simply `#include`s the
shared `stm32u083/clock.hpp`.

### Wire into `ohal.hpp`

Add `#include <ohal/clock.hpp>` to `include/ohal/ohal.hpp` so that
`clock::Enable<>` is available to any consumer who includes the single top-level header.

---

## Tests to Write (host)

Add a new test file `tests/host/test_clock_stm32u083rct.cpp` (or add to an existing
STM32U083 test file). Use the `MockRegister` infrastructure from
[Step 11](step-11-unit-testing.md).

| Test                                                | Register checked | Bit checked | Action |
| --------------------------------------------------- | ---------------- | ----------- | ------ |
| `Enable<PortA>::enable()` writes bit 0 of IOPENR    | IOPENR (mock)    | 0           | Set    |
| `Enable<PortB>::enable()` writes bit 1 of IOPENR    | IOPENR (mock)    | 1           | Set    |
| `Enable<PortC>::enable()` writes bit 2 of IOPENR    | IOPENR (mock)    | 2           | Set    |
| `Enable<PortF>::enable()` writes bit 5 of IOPENR    | IOPENR (mock)    | 5           | Set    |
| `Enable<PortA>::disable()` clears bit 0 of IOPENR   | IOPENR (mock)    | 0           | Clear  |
| `Enable<Usart2>::enable()` writes bit 17 of APBENR1 | APBENR1 (mock)   | 17          | Set    |
| `Enable<I2c1>::enable()` writes bit 21 of APBENR1   | APBENR1 (mock)   | 21          | Set    |
| `Enable<Pwr>::enable()` writes bit 28 of APBENR1    | APBENR1 (mock)   | 28          | Set    |
| `Enable<Syscfg>::enable()` writes bit 0 of APBENR2  | APBENR2 (mock)   | 0           | Set    |

Each test must confirm that only the target bit changes; all other bits in the register must
retain their pre-test values (initialise mock to `0xFFFFFFFF` and verify after `disable()`,
or initialise to `0x00000000` and verify after `enable()`).

### Negative-compile test

- `Enable<SomeUnsupportedTag>::enable()` must fire the primary-template `static_assert`.

---

## Acceptance Criteria

1. `clock::Enable<ohal::gpio::PortA>::enable()` sets bit 0 in the register at
   address `0x40021000 + 0x4C` and modifies no other bit.
2. `clock::Enable<ohal::gpio::PortF>::enable()` sets bit 5 in that same register.
3. `clock::Enable<Usart2>::enable()` sets bit 17 in the register at
   address `0x40021000 + 0x58`.
4. `clock::Enable<Syscfg>::enable()` sets bit 0 in the register at
   address `0x40021000 + 0x60`.
5. All `disable()` calls clear their respective bits without affecting other bits.
6. All existing host tests continue to pass.
7. `bash lint.sh` reports no errors.

---

## `test_project` Conversion

After this step, replace the clock-enable calls scattered through `main.cpp` and
`stm32u0xx_hal_msp.cpp`:

```cpp
// Before — in MX_GPIO_Init (main.cpp)
__HAL_RCC_GPIOC_CLK_ENABLE();
__HAL_RCC_GPIOF_CLK_ENABLE();
__HAL_RCC_GPIOA_CLK_ENABLE();
__HAL_RCC_GPIOB_CLK_ENABLE();

// After
ohal::clock::Enable<ohal::gpio::PortC>::enable();
ohal::clock::Enable<ohal::gpio::PortF>::enable();
ohal::clock::Enable<ohal::gpio::PortA>::enable();
ohal::clock::Enable<ohal::gpio::PortB>::enable();

// Before — in HAL_MspInit (stm32u0xx_hal_msp.cpp)
__HAL_RCC_PWR_CLK_ENABLE();
__HAL_RCC_SYSCFG_CLK_ENABLE();

// After
ohal::clock::Enable<ohal::platforms::stm32u0::stm32u083::Pwr>::enable();
ohal::clock::Enable<ohal::platforms::stm32u0::stm32u083::Syscfg>::enable();
```

Once all `__HAL_RCC_*_CLK_ENABLE()` calls in `test_project` are replaced, remove the
`#include "stm32u0xx_hal_rcc.h"` line from any file that no longer requires it.
