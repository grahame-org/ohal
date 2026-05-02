# GPIO API reference

This page describes the complete public API exposed by `ohal::gpio`. All symbols live in the
`ohal::gpio` namespace; import them with `using namespace ohal::gpio;`.

## Port tag types

Port tags are empty structs used as the first template argument to `Pin<>`. Using a tag type rather
than an integer prevents silent transposition of the port and pin-number arguments.

```cpp
namespace ohal::gpio {
    struct PortA {};
    struct PortB {};
    struct PortC {};
    struct PortD {};
    struct PortE {};
    struct PortF {};
}
```

Not every MCU exposes all six ports. The platform specialisation for your MCU will fire a
`static_assert` if you instantiate a port that does not exist on the selected device.

## Enumerations

All enumerations have `uint8_t` as their underlying type so that their bit patterns match the
values written directly to hardware registers.

### `PinMode`

Controls the electrical function of a pin.

| Enumerator                   | Value | Description                                                        |
| ---------------------------- | ----- | ------------------------------------------------------------------ |
| `PinMode::Input`             | 0     | High-impedance input                                               |
| `PinMode::Output`            | 1     | General-purpose output (push-pull or open-drain, see `OutputType`) |
| `PinMode::AlternateFunction` | 2     | Pin routed to an on-chip peripheral (UART, SPI, …)                 |
| `PinMode::Analog`            | 3     | Analogue input / ADC (default on STM32 at power-on)                |

### `OutputType`

Selects the output driver topology when a pin is in Output or AlternateFunction mode.

| Enumerator              | Value | Description                                          |
| ----------------------- | ----- | ---------------------------------------------------- |
| `OutputType::PushPull`  | 0     | Active drive high and low                            |
| `OutputType::OpenDrain` | 1     | Active drive low only; high side released to pull-up |

Not supported on all MCUs. Check `capabilities::supports_output_type<Port, PinNum>` or refer to
[Supported MCUs](supported-mcus.md).

### `Speed`

Controls the slew rate of the output driver. Higher speeds allow faster signal edges but increase
electromagnetic emissions and current draw.

| Enumerator        | Value | Description       |
| ----------------- | ----- | ----------------- |
| `Speed::Low`      | 0     | Slowest slew rate |
| `Speed::Medium`   | 1     |                   |
| `Speed::High`     | 2     |                   |
| `Speed::VeryHigh` | 3     | Fastest slew rate |

Not supported on all MCUs. Check `capabilities::supports_output_speed<Port, PinNum>`.

### `Pull`

Enables the internal pull resistor on a pin.

| Enumerator   | Value | Description           |
| ------------ | ----- | --------------------- |
| `Pull::None` | 0     | No pull resistor      |
| `Pull::Up`   | 1     | Weak pull-up to VDD   |
| `Pull::Down` | 2     | Weak pull-down to GND |

Not supported on all MCUs. Check `capabilities::supports_pull<Port, PinNum>`.

### `Level`

Represents a logic level when reading or driving a pin.

| Enumerator    | Value | Description           |
| ------------- | ----- | --------------------- |
| `Level::Low`  | 0     | Logic low (near GND)  |
| `Level::High` | 1     | Logic high (near VDD) |

## `Pin<Port, PinNum>` template

> **Status — planned API:** The `Pin<>` primary template and the enumerations above are implemented
> and available today. The configuration and I/O methods described in this section
> (`set_mode`, `set_output_type`, `set_speed`, `set_pull`, `set`, `clear`, `toggle`,
> `read_input`, `read_output`) are part of the planned interface and will be provided by the
> STM32U083 platform specialisation in Step 8. Instantiating `Pin<>` before that specialisation
> exists produces a compile error (see below).

```cpp
template <typename Port, uint8_t PinNum>
struct Pin;
```

`Pin` is the central type in the GPIO API. It is a zero-size struct; all methods are `static`.
Platform headers provide a partial specialisation for each supported (Port, PinNum) pair. If no
specialisation exists for the selected MCU, instantiating `Pin<>` produces a compile error:

```text
ohal: gpio::Pin is not implemented for the selected MCU.
      Ensure -DOHAL_FAMILY_* and -DOHAL_MODEL_* are set correctly.
```

Typical usage is to create a type alias for each pin:

```cpp
using Led    = ohal::gpio::Pin<ohal::gpio::PortA, 5>;
using Button = ohal::gpio::Pin<ohal::gpio::PortC, 13>;
```

### Configuration methods

#### `set_mode(PinMode mode)`

Sets the electrical function of the pin.

```cpp
Led::set_mode(PinMode::Output);
Button::set_mode(PinMode::Input);
```

Available on all supported MCUs.

#### `set_output_type(OutputType type)`

Selects push-pull or open-drain output topology.

```cpp
Led::set_output_type(OutputType::PushPull);
```

Guarded by `supports_output_type`. Calling this method on an MCU that does not support it is a
compile error.

#### `set_speed(Speed speed)`

Sets the output slew rate.

```cpp
Led::set_speed(Speed::Low);
```

Guarded by `supports_output_speed`. Calling this method on an MCU that does not support it is a
compile error.

#### `set_pull(Pull pull)`

Enables or disables the internal pull resistor.

```cpp
Button::set_pull(Pull::Up);
```

Guarded by `supports_pull`. Calling this method on an MCU that does not support it is a compile
error.

### Output methods

#### `set()`

Drives the pin high. Implemented using a hardware atomic set mechanism (e.g., BSRR on STM32) where
available, so it is safe to call from interrupt context without additional masking.

```cpp
Led::set();
```

#### `clear()`

Drives the pin low. Implemented using a hardware atomic clear mechanism where available.

```cpp
Led::clear();
```

#### `toggle()`

Inverts the current output state.

```cpp
Led::toggle();
```

### Input methods

#### `read_input() → Level`

Reads the actual logic level present on the pin (from the input data register).

```cpp
if (Button::read_input() == Level::Low) { /* button pressed */ }
```

#### `read_output() → Level`

Reads the currently programmed output level (from the output latch, not the pin). Useful for
implementing `toggle()` and for verifying the output state without relying on pin feedback.

```cpp
Level current = Led::read_output();
```

## `Port<PortTag>` template

> **Status — planned API:** The `Port<>` primary template is defined in `ohal/gpio.hpp` and
> available today. The `set()`, `clear()`, and `write()` methods described in this section are
> part of the planned interface and will be provided by the STM32U083 platform specialisation in
> Step 8. Instantiating `Port<>` before that specialisation exists produces a compile error.

```cpp
template <typename PortTag>
struct Port;
```

`Port` is the multi-pin companion to `Pin<>`. It is a zero-size struct; all methods are
`static`. Use it when you need to update several pins on the same port atomically — for example,
when driving an H-bridge, a parallel bus, or a shift register latch. For single-pin operations
use `Pin<>` instead.

Platform headers provide a partial specialisation for each supported port tag. If no
specialisation exists for the selected MCU, instantiating `Port<>` produces a compile error:

```text
ohal: gpio::Port is not implemented for the selected MCU.
      Ensure -DOHAL_FAMILY_* and -DOHAL_MODEL_* are set correctly.
```

All three methods accept a `uint16_t` bitmask. Bit _n_ corresponds to pin _n_ within the port.
Ports are at most 16 pins wide on all supported MCUs.

Typical usage:

```cpp
using namespace ohal::gpio;

// Set PA4 and PA7 high, PA5 and PA6 low — all in one bus transaction.
Port<PortA>::write((1U << 4) | (1U << 7), (1U << 5) | (1U << 6));
```

### `set(uint16_t mask)`

Drives every pin whose bit is set in `mask` high in a single bus transaction. Pins whose bit is
clear in `mask` are unaffected.

```cpp
Port<PortA>::set((1U << 4) | (1U << 7));  // PA4 and PA7 → HIGH; other pins unchanged
```

### `clear(uint16_t mask)`

Drives every pin whose bit is set in `mask` low in a single bus transaction. Pins whose bit is
clear in `mask` are unaffected.

```cpp
Port<PortA>::clear((1U << 5) | (1U << 6));  // PA5 and PA6 → LOW; other pins unchanged
```

### `write(uint16_t set_mask, uint16_t clear_mask)`

Atomically drives the pins in `set_mask` high and the pins in `clear_mask` low. On platforms
that provide a dedicated set/reset register (e.g. STM32 BSRR) this compiles to a **single store
instruction** with no intermediate hardware state. On platforms without such a register the
implementation performs two separate writes.

```cpp
// Forward: AHi=1, ALo=0, BHi=0, BLo=1
Port<PortA>::write((1U << 4) | (1U << 7), (1U << 5) | (1U << 6));
```

If a pin number appears in both `set_mask` and `clear_mask` the behaviour is platform-defined.
Keep the masks disjoint to write portable code.

## Capability traits

Capability traits are `std::bool_constant` specialisations that indicate at compile time which
features a given (Port, PinNum) pair supports on the selected MCU.

```cpp
namespace ohal::gpio::capabilities {
    template <typename Port, uint8_t PinNum>
    struct supports_output_type     : /* std::true_type or std::false_type */ {};

    template <typename Port, uint8_t PinNum>
    struct supports_output_speed    : /* std::true_type or std::false_type */ {};

    template <typename Port, uint8_t PinNum>
    struct supports_pull            : /* std::true_type or std::false_type */ {};

    template <typename Port, uint8_t PinNum>
    struct supports_alternate_function : /* std::true_type or std::false_type */ {};
}
```

The primary templates default to `std::false_type`. Platform headers specialise them to
`std::true_type` for the features they support.

### Querying a capability in application code

```cpp
#include <ohal/core/capabilities.hpp>

static_assert(
    ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 5>::value,
    "PA5 must support pull configuration on the selected MCU"
);
```

This is useful in generic middleware that must be portable but requires a minimum feature set.

## Platform-specific notes

### STM32U083

- `set()` and `clear()` write to the BSRR register (write-only, atomic). Each call is a single
  32-bit store; no read-modify-write occurs.
- `set_mode()`, `set_output_type()`, `set_speed()`, and `set_pull()` use read-modify-write on the
  corresponding configuration registers. These operations are not atomic with respect to interrupts
  that modify the same register. Mask interrupts at the call site if required.
- All four capability traits are `true` for every port/pin on the STM32U083.

### PIC18F4550 (planned)

- `set()` and `clear()` write to the `LAT` register (output latch), not `PORT`, to avoid the
  classic PIC read-modify-write hazard on the input register.
- `set_mode()` writes to the `TRIS` register with inverted polarity (`TRIS=0` means output).
- `set_output_type()`, `set_speed()`, and `set_pull()` are not supported — calling them is a
  compile error.
