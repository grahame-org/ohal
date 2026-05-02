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

// ---------------------------------------------------------------------------
// Primary (unimplemented) Port template
// ---------------------------------------------------------------------------

/// Generic GPIO port type for operating on multiple pins simultaneously.
/// Platform headers provide specialisations that implement the full
/// API for a specific MCU.
///
/// Use this type when you need to update several pins on the same port in a
/// single atomic bus transaction (e.g. H-bridge state transitions). For
/// single-pin operations, use Pin<> instead.
///
/// If no specialisation is available for the selected Port tag (because the
/// MCU family or model is not yet supported) compilation fails with a
/// descriptive static_assert.
///
/// All methods accept a @p uint16_t bitmask.  Bit n corresponds to pin n
/// within the port.  Ports are at most 16 pins wide on all supported MCUs.
///
/// @tparam PortTag  A port tag type (e.g. PortA).
template <typename PortTag>
struct Port {
  /// Drive every pin whose bit is set in @p mask high in a single bus
  /// transaction.  Pins whose bit is clear in @p mask are unaffected.
  static void set(uint16_t mask);

  /// Drive every pin whose bit is set in @p mask low in a single bus
  /// transaction.  Pins whose bit is clear in @p mask are unaffected.
  static void clear(uint16_t mask);

  /// Drive the pins in @p set_mask high and the pins in @p clear_mask low.
  /// On platforms that provide a dedicated set/reset register (e.g. STM32
  /// BSRR) this compiles to a single store instruction — no intermediate
  /// hardware state is visible.  On platforms without such a register the
  /// implementation performs two writes and a brief intermediate state
  /// exists between them.
  static void write(uint16_t set_mask, uint16_t clear_mask);

  static_assert(sizeof(PortTag) == 0,
                "ohal: gpio::Port is not implemented for the selected MCU. "
                "Ensure -DOHAL_FAMILY_* and -DOHAL_MODEL_* are set correctly.");
};

} // namespace ohal::gpio

#endif // OHAL_GPIO_HPP
