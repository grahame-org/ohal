#ifndef OHAL_PLATFORMS_MSP430FR2XX_MODELS_MSP430FR2355_GPIO_HPP
#define OHAL_PLATFORMS_MSP430FR2XX_MODELS_MSP430FR2355_GPIO_HPP

#include <cstdint>
#include <type_traits>

#include "ohal/core/access.hpp"
#include "ohal/core/field.hpp"
#include "ohal/core/register.hpp"
#include "ohal/gpio.hpp"
#include "ohal/platforms/msp430fr2xx/models/msp430fr2355/constants.hpp"

namespace ohal::platforms::msp430fr2xx::msp430fr2355 {

// ---------------------------------------------------------------------------
// GPIO register addresses (SLASE54 register map, Table 9-4).
//
// The MSP430FR2355 GPIO registers are byte-wide and non-contiguous.
// Ports are grouped into pairs; within each pair the even port (P1, P3, P5)
// is at the even offset and the odd port (P2, P4, P6) is at odd+1.
//
// Register type offsets within each pair (from the pair base address):
//   IN   0x00 (even) / 0x01 (odd)
//   OUT  0x02 (even) / 0x03 (odd)
//   DIR  0x04 (even) / 0x05 (odd)
//   REN  0x06 (even) / 0x07 (odd)
//   [0x08/0x09 are reserved]
//   SEL0 0x0A (even) / 0x0B (odd)
//   SEL1 0x0C (even) / 0x0D (odd)
// ---------------------------------------------------------------------------

/// Base address of the Port 1 / Port 2 register pair.
inline constexpr uintptr_t kPortPair12Base = 0x0200U;
/// Base address of the Port 3 / Port 4 register pair.
inline constexpr uintptr_t kPortPair34Base = 0x0220U;
/// Base address of the Port 5 / Port 6 register pair.
inline constexpr uintptr_t kPortPair56Base = 0x0240U;

/// Sub-offset added to any register-type offset to reach the even-port register.
inline constexpr uintptr_t kEvenPortSub = 0x00U;
/// Sub-offset added to any register-type offset to reach the odd-port register.
inline constexpr uintptr_t kOddPortSub = 0x01U;

/// Offset of the PxIN register within a port pair (from the pair base).
inline constexpr uintptr_t kInPairOffset = 0x00U;
/// Offset of the PxOUT register within a port pair.
inline constexpr uintptr_t kOutPairOffset = 0x02U;
/// Offset of the PxDIR register within a port pair.
inline constexpr uintptr_t kDirPairOffset = 0x04U;
/// Offset of the PxREN register within a port pair.
inline constexpr uintptr_t kRenPairOffset = 0x06U;
/// Offset of the PxSEL0 register within a port pair.
inline constexpr uintptr_t kSel0PairOffset = 0x0AU;
/// Offset of the PxSEL1 register within a port pair.
inline constexpr uintptr_t kSel1PairOffset = 0x0CU;

/// Register types for one MSP430FR2355 8-bit GPIO port.
/// Parameterised on six independent 8-bit register addresses (each port has
/// distinct non-contiguous addresses — see SLASE54 register map).
template <uintptr_t InAddr, uintptr_t OutAddr, uintptr_t DirAddr, uintptr_t RenAddr,
          uintptr_t Sel0Addr, uintptr_t Sel1Addr>
struct GpioPortRegs {
  using In = core::Register<InAddr, uint8_t>;     ///< PxIN  (RO)
  using Out = core::Register<OutAddr, uint8_t>;   ///< PxOUT (RW)
  using Dir = core::Register<DirAddr, uint8_t>;   ///< PxDIR (RW)
  using Ren = core::Register<RenAddr, uint8_t>;   ///< PxREN (RW)
  using Sel0 = core::Register<Sel0Addr, uint8_t>; ///< PxSEL0 (RW)
  using Sel1 = core::Register<Sel1Addr, uint8_t>; ///< PxSEL1 (RW)
};

// Named register sets for each port built from the constants above.
using GpioP1Regs = GpioPortRegs<kPortPair12Base + kInPairOffset + kEvenPortSub,
                                kPortPair12Base + kOutPairOffset + kEvenPortSub,
                                kPortPair12Base + kDirPairOffset + kEvenPortSub,
                                kPortPair12Base + kRenPairOffset + kEvenPortSub,
                                kPortPair12Base + kSel0PairOffset + kEvenPortSub,
                                kPortPair12Base + kSel1PairOffset + kEvenPortSub>;
using GpioP2Regs = GpioPortRegs<
    kPortPair12Base + kInPairOffset + kOddPortSub, kPortPair12Base + kOutPairOffset + kOddPortSub,
    kPortPair12Base + kDirPairOffset + kOddPortSub, kPortPair12Base + kRenPairOffset + kOddPortSub,
    kPortPair12Base + kSel0PairOffset + kOddPortSub,
    kPortPair12Base + kSel1PairOffset + kOddPortSub>;
using GpioP3Regs = GpioPortRegs<kPortPair34Base + kInPairOffset + kEvenPortSub,
                                kPortPair34Base + kOutPairOffset + kEvenPortSub,
                                kPortPair34Base + kDirPairOffset + kEvenPortSub,
                                kPortPair34Base + kRenPairOffset + kEvenPortSub,
                                kPortPair34Base + kSel0PairOffset + kEvenPortSub,
                                kPortPair34Base + kSel1PairOffset + kEvenPortSub>;
using GpioP4Regs = GpioPortRegs<
    kPortPair34Base + kInPairOffset + kOddPortSub, kPortPair34Base + kOutPairOffset + kOddPortSub,
    kPortPair34Base + kDirPairOffset + kOddPortSub, kPortPair34Base + kRenPairOffset + kOddPortSub,
    kPortPair34Base + kSel0PairOffset + kOddPortSub,
    kPortPair34Base + kSel1PairOffset + kOddPortSub>;
using GpioP5Regs = GpioPortRegs<kPortPair56Base + kInPairOffset + kEvenPortSub,
                                kPortPair56Base + kOutPairOffset + kEvenPortSub,
                                kPortPair56Base + kDirPairOffset + kEvenPortSub,
                                kPortPair56Base + kRenPairOffset + kEvenPortSub,
                                kPortPair56Base + kSel0PairOffset + kEvenPortSub,
                                kPortPair56Base + kSel1PairOffset + kEvenPortSub>;
using GpioP6Regs = GpioPortRegs<
    kPortPair56Base + kInPairOffset + kOddPortSub, kPortPair56Base + kOutPairOffset + kOddPortSub,
    kPortPair56Base + kDirPairOffset + kOddPortSub, kPortPair56Base + kRenPairOffset + kOddPortSub,
    kPortPair56Base + kSel0PairOffset + kOddPortSub,
    kPortPair56Base + kSel1PairOffset + kOddPortSub>;

namespace detail {
/// Helper for a compile-time false that is dependent on a template parameter.
/// Used in static_assert to ensure the assertion fires only when the
/// containing function is actually called (instantiated), not when the
/// enclosing class template is instantiated.
template <typename>
struct dependent_false : std::false_type {};
} // namespace detail

/// Implements the ohal::gpio::Pin<Port, PinNum> API for one MSP430FR2355 GPIO port.
///
/// Parameterised on @p Regs so that host-side tests can inject a mock register set
/// (using ohal::test::MockRegister types) without modifying the real hardware header.
/// Production code uses GpioP1Regs…GpioP6Regs as the @p Regs argument.
///
/// @tparam PinNum  Zero-based pin number within the port (0–7).
/// @tparam Regs    A type whose nested type aliases (In, Out, Dir, Ren, Sel0, Sel1)
///                 satisfy the ohal::core::Register or MockRegister interface.
template <uint8_t PinNum, typename Regs>
struct GpioPortPinImpl {
  static_assert(PinNum < kPinCount, "ohal: MSP430FR2355 GPIO ports have pins 0-7 only.");

  // Bit-field descriptors — 1 bit per pin.
  using DirBit = core::BitField<typename Regs::Dir, PinNum, 1U, core::Access::ReadWrite>;
  using OutBit =
      core::BitField<typename Regs::Out, PinNum, 1U, core::Access::ReadWrite, gpio::Level>;
  using InBit = core::BitField<typename Regs::In, PinNum, 1U, core::Access::ReadOnly, gpio::Level>;
  using RenBit = core::BitField<typename Regs::Ren, PinNum, 1U, core::Access::ReadWrite>;
  using Sel0Bit = core::BitField<typename Regs::Sel0, PinNum, 1U, core::Access::ReadWrite>;
  using Sel1Bit = core::BitField<typename Regs::Sel1, PinNum, 1U, core::Access::ReadWrite>;

  /// set_mode maps PinMode values to MSP430 GPIO register writes:
  ///   Input             → DIR=0, SEL0=0, SEL1=0 (GPIO input)
  ///   Output            → DIR=1, SEL0=0, SEL1=0 (GPIO output)
  ///   AlternateFunction → SEL0=1, SEL1=0 (primary AF; DIR set by peripheral)
  ///   Analog            → treated as Input (MSP430 has no distinct Analog mode;
  ///                       ADC channel selection is done through the ADC module)
  static void set_mode(gpio::PinMode mode) noexcept {
    if (mode == gpio::PinMode::Output) {
      DirBit::write(static_cast<uint8_t>(1U));
      Sel0Bit::write(static_cast<uint8_t>(0U));
      Sel1Bit::write(static_cast<uint8_t>(0U));
    } else if (mode == gpio::PinMode::AlternateFunction) {
      // Primary peripheral function: SEL1:SEL0 = 0b01. Direction is
      // controlled by the selected peripheral, not written here.
      Sel0Bit::write(static_cast<uint8_t>(1U));
      Sel1Bit::write(static_cast<uint8_t>(0U));
    } else { // PinMode::Input or PinMode::Analog (treated as GPIO input on MSP430)
      DirBit::write(static_cast<uint8_t>(0U));
      Sel0Bit::write(static_cast<uint8_t>(0U));
      Sel1Bit::write(static_cast<uint8_t>(0U));
    }
  }

  static void set() noexcept { OutBit::write(gpio::Level::High); }
  static void clear() noexcept { OutBit::write(gpio::Level::Low); }
  [[nodiscard]] static gpio::Level read_input() noexcept { return InBit::read(); }
  [[nodiscard]] static gpio::Level read_output() noexcept { return OutBit::read(); }

  static void toggle() noexcept {
    if (read_output() == gpio::Level::Low) {
      set();
    } else {
      clear();
    }
  }

  /// set_pull: enable PxREN and use PxOUT to select up/down.
  ///
  /// **Important:** On MSP430FR2355, `PxOUT` controls both the output driven level
  /// (when the pin is an output) and the pull resistor direction (when the pin is an
  /// input with PxREN set). Calling `set_pull()` while the pin is still configured as
  /// an output will change the pin's driven level as a side effect. Always call
  /// `set_mode(PinMode::Input)` before `set_pull()`.
  static void set_pull(gpio::Pull pull) noexcept {
    if (pull == gpio::Pull::None) {
      RenBit::write(static_cast<uint8_t>(0U));
    } else {
      // PxOUT bit N: 1 = pull-up, 0 = pull-down (only meaningful when PxREN=1 and PxDIR=0).
      OutBit::write(pull == gpio::Pull::Up ? gpio::Level::High : gpio::Level::Low);
      RenBit::write(static_cast<uint8_t>(1U));
    }
  }

  // Unsupported features: static_assert fires when the function is called.
  // dependent_false<Regs>::value is a template-dependent false expression,
  // ensuring the assertion is only evaluated (and fires) at the call site.
  static void set_output_type(gpio::OutputType /*output_type*/) noexcept {
    static_assert(detail::dependent_false<Regs>::value,
                  "ohal: MSP430FR2355 GPIO does not support configurable output type.");
  }
  static void set_speed(gpio::Speed /*speed*/) noexcept {
    static_assert(detail::dependent_false<Regs>::value,
                  "ohal: MSP430FR2355 GPIO does not support configurable output speed.");
  }
};

} // namespace ohal::platforms::msp430fr2xx::msp430fr2355

namespace ohal::gpio {

// Pin<PortA…PortF, PinNum> partial specialisations delegate to GpioPortPinImpl.
// PortA → P1 (GpioP1Regs), PortB → P2 (GpioP2Regs), PortC → P3 (GpioP3Regs),
// PortD → P4 (GpioP4Regs), PortE → P5 (GpioP5Regs), PortF → P6 (GpioP6Regs).

template <uint8_t PinNum>
struct Pin<PortA, PinNum> : platforms::msp430fr2xx::msp430fr2355::GpioPortPinImpl<
                                PinNum, platforms::msp430fr2xx::msp430fr2355::GpioP1Regs> {};

template <uint8_t PinNum>
struct Pin<PortB, PinNum> : platforms::msp430fr2xx::msp430fr2355::GpioPortPinImpl<
                                PinNum, platforms::msp430fr2xx::msp430fr2355::GpioP2Regs> {};

template <uint8_t PinNum>
struct Pin<PortC, PinNum> : platforms::msp430fr2xx::msp430fr2355::GpioPortPinImpl<
                                PinNum, platforms::msp430fr2xx::msp430fr2355::GpioP3Regs> {};

template <uint8_t PinNum>
struct Pin<PortD, PinNum> : platforms::msp430fr2xx::msp430fr2355::GpioPortPinImpl<
                                PinNum, platforms::msp430fr2xx::msp430fr2355::GpioP4Regs> {};

template <uint8_t PinNum>
struct Pin<PortE, PinNum> : platforms::msp430fr2xx::msp430fr2355::GpioPortPinImpl<
                                PinNum, platforms::msp430fr2xx::msp430fr2355::GpioP5Regs> {};

template <uint8_t PinNum>
struct Pin<PortF, PinNum> : platforms::msp430fr2xx::msp430fr2355::GpioPortPinImpl<
                                PinNum, platforms::msp430fr2xx::msp430fr2355::GpioP6Regs> {};

} // namespace ohal::gpio

#endif // OHAL_PLATFORMS_MSP430FR2XX_MODELS_MSP430FR2355_GPIO_HPP
