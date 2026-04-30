# Step 7 – Peripheral Abstraction Interface (GPIO first)

**Goal:** A generic `ohal::gpio::Pin<Port, PinNum>` that provides the full GPIO API without any
reference to register addresses.

**Inputs required:** None — register addresses come from the platform layer, already included via
`platform.hpp`.

## Key Design Decisions

- `Port` is a tag type (e.g., `ohal::gpio::PortA`), not an integer. This prevents silent errors
  from swapping port and pin number arguments.
- `PinNum` is a `uint8_t` NTTP (non-type template parameter). Out-of-range values (e.g., pin 16
  on an 8-pin port) are caught by `static_assert` inside the platform specialisation.
- The implementation body of each method is provided by **partial specialisation** in the
  platform-specific header. If no specialisation exists, the primary template fires a
  `static_assert`:

  ```text
  ohal: gpio::Pin is not implemented for the selected MCU.
  ```

- Some platforms do not support all GPIO features (e.g. PIC18 has no configurable output speed).
  Unsupported features are modelled through capability traits; calling an unsupported method
  produces a `static_assert` with a helpful message at compile time.

## Port Tag Types (generic)

```cpp
// include/ohal/gpio.hpp  (excerpt)
namespace ohal::gpio {
    struct PortA {};
    struct PortB {};
    struct PortC {};
    struct PortD {};
    struct PortE {};
    struct PortF {};
} // namespace ohal::gpio
```

## Enumerations (generic)

```cpp
namespace ohal::gpio {

enum class PinMode   : uint8_t { Input = 0, Output = 1, AlternateFunction = 2, Analog = 3 };
enum class OutputType: uint8_t { PushPull = 0, OpenDrain = 1 };
enum class Speed     : uint8_t { Low = 0, Medium = 1, High = 2, VeryHigh = 3 };
enum class Pull      : uint8_t { None = 0, Up = 1, Down = 2 };
enum class Level     : uint8_t { Low = 0, High = 1 };

} // namespace ohal::gpio
```

## Primary (Unimplemented) Template

```cpp
namespace ohal::gpio {

template <typename Port, uint8_t PinNum>
struct Pin {
    static_assert(sizeof(Port) == 0,
        "ohal: gpio::Pin is not implemented for the selected MCU. "
        "Ensure -DOHAL_FAMILY_* and -DOHAL_MODEL_* are set correctly.");
};

} // namespace ohal::gpio
```

## Capability Traits (generic primary templates)

Capability traits declare whether a particular feature is supported for a given (Port, Pin) pair.
Platform headers specialise these to `true_type` or `false_type`. The generic primary templates
default to `false_type` so that unimplemented platforms fail noisily.

```cpp
// include/ohal/core/capabilities.hpp  (excerpt)
namespace ohal::gpio::capabilities {

template <typename Port, uint8_t PinNum>
struct supports_output_type : std::false_type {};

template <typename Port, uint8_t PinNum>
struct supports_output_speed : std::false_type {};

template <typename Port, uint8_t PinNum>
struct supports_pull : std::false_type {};

template <typename Port, uint8_t PinNum>
struct supports_alternate_function : std::false_type {};

} // namespace ohal::gpio::capabilities
```
