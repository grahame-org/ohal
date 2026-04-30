# Step 6 – MCU Family and Model Selection Mechanism

**Goal:** A `platform.hpp` that validates the MCU defines and includes the correct platform header,
and a skeleton for the first platform (STM32U0 / STM32U083).

## Inputs Required for STM32U083

- MCU family: STM32U0
- MCU model: STM32U083
- Peripheral: GPIO
- Register base addresses (STM32U083 Reference Manual RM0503):
  - GPIOA base: `0x42020000`
  - GPIOB base: `0x42020400`
  - GPIOC base: `0x42020800`
  - GPIOD base: `0x42020C00`
  - GPIOE base: `0x42021000`
  - GPIOF base: `0x42021400`
- Register offsets within each GPIO block (identical across all STM32U0 GPIO ports):
  - `MODER`    offset `0x00` — pin mode (Input / Output / AF / Analog)
  - `OTYPER`   offset `0x04` — output type (Push-Pull / Open-Drain)
  - `OSPEEDR`  offset `0x08` — output speed
  - `PUPDR`    offset `0x0C` — pull-up / pull-down
  - `IDR`      offset `0x10` — input data register (RO)
  - `ODR`      offset `0x14` — output data register
  - `BSRR`     offset `0x18` — bit set/reset register (WO)
  - `LCKR`     offset `0x1C` — configuration lock register
  - `AFRL`     offset `0x20` — alternate function low (pins 0–7)
  - `AFRH`     offset `0x24` — alternate function high (pins 8–15)

## `platform.hpp` Logic

```cpp
// include/ohal/platform.hpp
#ifndef OHAL_PLATFORM_HPP
#define OHAL_PLATFORM_HPP

#if !defined(OHAL_FAMILY_STM32U0) && \
    !defined(OHAL_FAMILY_TI_MSPM0) && \
    !defined(OHAL_FAMILY_PIC)
  #error "ohal: No MCU family defined. " \
         "Pass one of -DOHAL_FAMILY_STM32U0, -DOHAL_FAMILY_TI_MSPM0, " \
         "-DOHAL_FAMILY_PIC to the compiler."
#endif

#if defined(OHAL_FAMILY_STM32U0)
  #include "ohal/platforms/stm32u0/family.hpp"
#elif defined(OHAL_FAMILY_TI_MSPM0)
  #include "ohal/platforms/ti_mspm0/family.hpp"
#elif defined(OHAL_FAMILY_PIC)
  #include "ohal/platforms/pic/family.hpp"
#endif

#endif // OHAL_PLATFORM_HPP
```

## `stm32u0/family.hpp` Logic

```cpp
// platforms/stm32u0/family.hpp
#ifndef OHAL_PLATFORMS_STM32U0_FAMILY_HPP
#define OHAL_PLATFORMS_STM32U0_FAMILY_HPP

#if !defined(OHAL_MODEL_STM32U083) && \
    !defined(OHAL_MODEL_STM32U073) /* … list all U0 models … */
  #error "ohal: No STM32U0 model defined. " \
         "Pass -DOHAL_MODEL_STM32U083 (or another U0 model) to the compiler."
#endif

#if defined(OHAL_MODEL_STM32U083)
  #include "ohal/platforms/stm32u0/models/stm32u083/gpio.hpp"
  #include "ohal/platforms/stm32u0/models/stm32u083/timer.hpp"
  #include "ohal/platforms/stm32u0/models/stm32u083/uart.hpp"
  #include "ohal/platforms/stm32u0/models/stm32u083/capabilities.hpp"
// … other models …
#endif

#endif // OHAL_PLATFORMS_STM32U0_FAMILY_HPP
```

## Tests to Write (Step 11)

- Compiling with no defines produces the "No MCU family defined" error.
- Compiling with `OHAL_FAMILY_STM32U0` but no model produces the "No STM32U0 model defined"
  error.
- Compiling with `OHAL_FAMILY_STM32U0` + `OHAL_MODEL_STM32U083` succeeds.
- Compiling with an invalid family produces the unknown-family error.
