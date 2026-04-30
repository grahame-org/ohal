# Step 9 – First Non-ARM Platform: Microchip PIC18F4550 GPIO

**Goal:** Implement GPIO for a non-ARM MCU to prove the architecture delivers true cross-platform
consistency. The PIC18F4550 uses an 8-bit non-ARM Harvard RISC core, 8-bit Special Function
Registers, and a fundamentally different GPIO register layout from the STM32U0.

## Why PIC18F4550 Demonstrates the Architecture's Power

| Property           | STM32U083 (ARM Cortex-M0+)        | PIC18F4550 (Microchip PIC18)                        |
| ------------------ | --------------------------------- | --------------------------------------------------- |
| Architecture       | 32-bit ARM Cortex-M0+             | 8-bit Harvard RISC (non-ARM)                        |
| Toolchain (target) | `arm-none-eabi-g++`               | Microchip XC8                                       |
| Register width     | 32-bit (`uint32_t`)               | 8-bit (`uint8_t`)                                   |
| GPIO direction     | `MODER` (2 bits/pin)              | `TRIS` (1 bit/pin, **inverted**: 1=input, 0=output) |
| GPIO output        | `BSRR` (atomic set/reset, WO)     | `LAT` (output latch, read-write)                    |
| GPIO input         | `IDR` (RO)                        | `PORT` (reads actual pin state, RO)                 |
| Output type        | `OTYPER` (push-pull / open-drain) | Not supported — compile error                       |
| Output speed       | `OSPEEDR`                         | Not supported — compile error                       |
| Pull resistors     | `PUPDR`                           | Not supported in GPIO peripheral — compile error    |
| Alternate function | `AFRL`/`AFRH`                     | Not supported in GPIO peripheral — compile error    |

The same application-level API (`set()`, `clear()`, `toggle()`, `read_input()`, `set_mode()`)
works unchanged. Calls to unsupported features (`set_speed()`, `set_output_type()`, `set_pull()`)
produce a `static_assert` compile error with a helpful message.

## Inputs Required

- MCU family: PIC
- MCU model: PIC18F4550
- MCU architecture: 8-bit PIC18 (non-ARM, Harvard)
- Register width: 8 bits
- Compiler (target builds): Microchip XC8
- MCU peripheral features: GPIO with PORT (input), LAT (output), TRIS (direction); 1 bit per pin
- MCU peripheral register memory map (PIC18F4550 datasheet DS39632E):

| Register | Address | Access | Description                            |
| -------- | ------- | ------ | -------------------------------------- |
| PORTA    | `0xF80` | RO     | Pin input state — port A               |
| PORTB    | `0xF81` | RO     | Pin input state — port B               |
| PORTC    | `0xF82` | RO     | Pin input state — port C               |
| PORTD    | `0xF83` | RO     | Pin input state — port D               |
| PORTE    | `0xF84` | RO     | Pin input state — port E               |
| LATA     | `0xF89` | RW     | Output latch — port A                  |
| LATB     | `0xF8A` | RW     | Output latch — port B                  |
| LATC     | `0xF8B` | RW     | Output latch — port C                  |
| LATD     | `0xF8C` | RW     | Output latch — port D                  |
| LATE     | `0xF8D` | RW     | Output latch — port E                  |
| TRISA    | `0xF92` | RW     | Direction — port A (1=input, 0=output) |
| TRISB    | `0xF93` | RW     | Direction — port B                     |
| TRISC    | `0xF94` | RW     | Direction — port C                     |
| TRISD    | `0xF95` | RW     | Direction — port D                     |
| TRISE    | `0xF96` | RW     | Direction — port E                     |

## Sequence — "set GPIO pin high" on PIC18F4550

```mermaid
sequenceDiagram
    participant App as Application
    participant Pin as gpio::Pin&lt;PortA,5&gt;
    participant LAT as LATA (port A latch)

    App->>Pin: Pin::set()
    Pin->>LAT: set_bits(1u << 5)
    Note over LAT: Single 8-bit load-modify-store.<br/>Zero overhead vs. direct SFR access.
```

## `pic/family.hpp` Logic

```cpp
// platforms/pic/family.hpp
#ifndef OHAL_PLATFORMS_PIC_FAMILY_HPP
#define OHAL_PLATFORMS_PIC_FAMILY_HPP

#if !defined(OHAL_MODEL_PIC18F4550) /* … list all supported PIC models … */
  #error "ohal: No PIC model defined. " \
         "Pass -DOHAL_MODEL_PIC18F4550 (or another PIC model) to the compiler."
#endif

#if defined(OHAL_MODEL_PIC18F4550)
  #include "ohal/platforms/pic/models/pic18f4550/gpio.hpp"
  #include "ohal/platforms/pic/models/pic18f4550/capabilities.hpp"
#endif

#endif // OHAL_PLATFORMS_PIC_FAMILY_HPP
```

## Implementation Skeleton (`pic18f4550/gpio.hpp`)

Note the key differences from the STM32U083 specialisation:

- All registers are `uint8_t` wide.
- Direction is via the `TRIS` register: writing `0` makes the pin an output, `1` makes it an
  input. This is the **opposite** polarity from `PinMode::Output = 1`. The specialisation
  handles the inversion internally so the application API remains consistent.
- Output is written via `LAT`, not `PORT`, to avoid the classic PIC read-modify-write hazard.
- Input is read via `PORT`.

```cpp
// platforms/pic/models/pic18f4550/gpio.hpp
#ifndef OHAL_PLATFORMS_PIC_MODELS_PIC18F4550_GPIO_HPP
#define OHAL_PLATFORMS_PIC_MODELS_PIC18F4550_GPIO_HPP

#include <cstdint>
#include "ohal/core/register.hpp"
#include "ohal/core/field.hpp"
#include "ohal/gpio.hpp"
#include "ohal/platforms/pic/models/pic18f4550/capabilities.hpp"

namespace ohal::gpio {

// All PIC18F4550 GPIO registers are 8-bit.
// Register<Addr, uint8_t> is used throughout.

template <uint8_t PinNum>
struct Pin<PortA, PinNum> {
    static_assert(PinNum < 6u, "ohal: PIC18F4550 PORTA has pins 0-5 only.");

    // Direction: TRISA bit N. TRIS=0 -> output, TRIS=1 -> input (inverted vs PinMode enum).
    using TRIS_BIT = core::BitField<
        core::Register<0xF92u, uint8_t>, PinNum, 1u, core::Access::ReadWrite>;

    // Output latch: LATA bit N. Always write outputs here, never to PORT.
    using LAT_BIT = core::BitField<
        core::Register<0xF89u, uint8_t>, PinNum, 1u, core::Access::ReadWrite, Level>;

    // Input: PORTA bit N. Read the actual pin state here.
    using PORT_BIT = core::BitField<
        core::Register<0xF80u, uint8_t>, PinNum, 1u, core::Access::ReadOnly, Level>;

    // set_mode: PinMode::Output -> TRIS=0, PinMode::Input -> TRIS=1 (inverted polarity).
    static void set_mode(PinMode mode) noexcept {
        TRIS_BIT::write(mode == PinMode::Output ? 0u : 1u);
    }

    static void set()   noexcept { LAT_BIT::write(Level::High); }
    static void clear() noexcept { LAT_BIT::write(Level::Low);  }
    static Level read_input()  noexcept { return PORT_BIT::read(); }
    static Level read_output() noexcept { return LAT_BIT::read();  }
    static void toggle() noexcept {
        if (read_output() == Level::Low) set();
        else clear();
    }

    // Unsupported features: static_assert fires at call site using capability traits.
    static void set_output_type(OutputType) noexcept {
        static_assert(capabilities::supports_output_type<PortA, PinNum>::value,
            "ohal: PIC18F4550 GPIO does not support configurable output type.");
    }
    static void set_speed(Speed) noexcept {
        static_assert(capabilities::supports_output_speed<PortA, PinNum>::value,
            "ohal: PIC18F4550 GPIO does not support configurable output speed.");
    }
    static void set_pull(Pull) noexcept {
        static_assert(capabilities::supports_pull<PortA, PinNum>::value,
            "ohal: PIC18F4550 GPIO does not support pull configuration via the GPIO peripheral.");
    }
};

// Repeat for PortB ... PortE with their respective PORT/LAT/TRIS addresses and pin counts.
// PortB (8 pins): PORT=0xF81, LAT=0xF8A, TRIS=0xF93, PinNum < 8
// PortC (7 pins): PORT=0xF82, LAT=0xF8B, TRIS=0xF94, PinNum < 7
// PortD (8 pins): PORT=0xF83, LAT=0xF8C, TRIS=0xF95, PinNum < 8
// PortE (3 pins): PORT=0xF84, LAT=0xF8D, TRIS=0xF96, PinNum < 3

} // namespace ohal::gpio

#endif // OHAL_PLATFORMS_PIC_MODELS_PIC18F4550_GPIO_HPP
```

## Capability Specialisations (`pic18f4550/capabilities.hpp`)

```cpp
// platforms/pic/models/pic18f4550/capabilities.hpp
#ifndef OHAL_PLATFORMS_PIC_MODELS_PIC18F4550_CAPABILITIES_HPP
#define OHAL_PLATFORMS_PIC_MODELS_PIC18F4550_CAPABILITIES_HPP

#include "ohal/core/capabilities.hpp"

namespace ohal::gpio::capabilities {

// PIC18F4550 GPIO does not support output type, speed, pull, or alternate function.
// The generic primary templates (all false_type) already handle this correctly —
// no additional specialisations are required for unsupported features.
// Supported features that deviate from the default false must be specialised here.
// (PIC18F4550 has no such deviations for GPIO.)

} // namespace ohal::gpio::capabilities

#endif // OHAL_PLATFORMS_PIC_MODELS_PIC18F4550_CAPABILITIES_HPP
```

## Approach

1. Add `platforms/pic/family.hpp` — validates model.
2. Add `platforms/pic/models/pic18f4550/gpio.hpp` — partial specialisations using `uint8_t`
   registers with PORT/LAT/TRIS layout.
3. Add `platforms/pic/models/pic18f4550/capabilities.hpp` — all GPIO capability traits remain
   `false_type` (default); no specialisations needed.
4. Application code using `ohal::gpio::Pin<PortA, 5>` for basic `set()`/`clear()`/`toggle()`
   compiles unchanged.
5. Application code calling `set_speed()` or `set_output_type()` on a PIC18F4550 target fails
   with a clear compile-time error message.
6. Run the same `set()`/`clear()`/`toggle()`/`read_input()` host tests against PIC18F4550 mock
   registers (8-bit `mock_memory` array).

## Tests to Write (host)

- `Pin<PortA, 2>::set()` calls `LAT_BIT::write(Level::High)` — verifies bit 2 of LATA mock slot
  is set.
- `Pin<PortA, 2>::clear()` — verifies bit 2 of LATA mock slot is cleared.
- `Pin<PortA, 2>::set_mode(PinMode::Output)` — verifies bit 2 of TRISA mock slot is cleared
  (TRIS=0 for output).
- `Pin<PortA, 2>::set_mode(PinMode::Input)` — verifies bit 2 of TRISA mock slot is set (TRIS=1
  for input).
- `Pin<PortA, 2>::read_input()` — reads PORTA mock slot, returns correct `Level`.
- `Pin<PortA, 2>::set_speed(Speed::High)` fails to compile with message
  `PIC18F4550 GPIO does not support configurable output speed` (negative-compile test).
