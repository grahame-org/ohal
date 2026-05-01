#ifndef OHAL_GPIO_HPP
#define OHAL_GPIO_HPP

#include <cstdint>

namespace ohal::gpio {

// ---------------------------------------------------------------------------
// Port tag types
// ---------------------------------------------------------------------------

/// Tag types for GPIO ports.  Use these as the first template argument to
/// Pin<>.  Empty structs act as strong types and prevent silent transposition
/// of port and pin-number arguments.
struct PortA {};
struct PortB {};
struct PortC {};
struct PortD {};
struct PortE {};
struct PortF {};

// ---------------------------------------------------------------------------
// Enumerations
// ---------------------------------------------------------------------------

/// GPIO pin direction / function mode.
enum class PinMode : uint8_t {
  Input = 0,
  Output = 1,
  AlternateFunction = 2,
  Analog = 3,
};

/// GPIO output driver type.
enum class OutputType : uint8_t {
  PushPull = 0,
  OpenDrain = 1,
};

/// GPIO output switching speed.
enum class Speed : uint8_t {
  Low = 0,
  Medium = 1,
  High = 2,
  VeryHigh = 3,
};

/// GPIO internal resistor configuration.
enum class Pull : uint8_t {
  None = 0,
  Up = 1,
  Down = 2,
};

/// Logic level for reading or driving a GPIO pin.
enum class Level : uint8_t {
  Low = 0,
  High = 1,
};

// ---------------------------------------------------------------------------
// Primary (unimplemented) Pin template
// ---------------------------------------------------------------------------

/// Generic GPIO pin type.  Platform headers provide partial specialisations
/// that implement the full API for a specific MCU.
///
/// If no specialisation is available for the selected (Port, PinNum) pair
/// (because the MCU family or model is not yet supported) compilation fails
/// with a descriptive static_assert.
///
/// @tparam Port    A port tag type (e.g. PortA).  Strong typing prevents
///                 silent transposition of port and pin arguments.
/// @tparam PinNum  Zero-based pin number within the port (uint8_t NTTP).
///                 Out-of-range values are caught by static_assert inside the
///                 platform specialisation.
template <typename Port, uint8_t PinNum>
struct Pin {
  static_assert(sizeof(Port) == 0, "ohal: gpio::Pin is not implemented for the selected MCU. "
                                   "Ensure -DOHAL_FAMILY_* and -DOHAL_MODEL_* are set correctly.");
};

} // namespace ohal::gpio

#endif // OHAL_GPIO_HPP
