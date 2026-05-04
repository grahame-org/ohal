#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083MC_GPIO_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083MC_GPIO_HPP

// Shared GPIO Pin<>/Port<> specialisations for STM32U083MC 80/81-pin packages:
//   STM32U083MCT (LQFP80) and STM32U083MCI (UFBGA81).
//
// Both packages share the following GPIO bonding:
//   GPIOA (bits 0–15), GPIOB (bits 0–15), GPIOC (bits 0–15),
//   GPIOD (bits 0–6 and 8–13; PD7, PD14, PD15 absent),
//   GPIOF (PF0–PF3 only).
// GPIOE bonding differs between packages:
//   MCT: PE7–PE9 only.
//   MCI: PE3, PE7–PE9.
// GPIOE is therefore NOT defined in this shared header; the per-package
// headers (stm32u083mct/gpio.hpp and stm32u083mci/gpio.hpp) provide the
// correct Pin<PortE>/Port<PortE> specialisations.
//
// This header is included by stm32u083mct/gpio.hpp and stm32u083mci/gpio.hpp.

#include "ohal/platforms/stm32u0/models/stm32u083/gpio_impl.hpp"

namespace ohal::gpio {

// Bonded-out ports with full pin ranges: GPIOA, GPIOB, GPIOC
template <uint8_t PinNum>
struct Pin<PortA, PinNum> : ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<
                                PinNum, ohal::platforms::stm32u0::stm32u083::GpioA> {};

template <uint8_t PinNum>
struct Pin<PortB, PinNum> : ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<
                                PinNum, ohal::platforms::stm32u0::stm32u083::GpioB> {};

template <uint8_t PinNum>
struct Pin<PortC, PinNum> : ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<
                                PinNum, ohal::platforms::stm32u0::stm32u083::GpioC> {};

// GPIOD: bits 0–6 and 8–13 are bonded out; PD7, PD14, PD15 are absent.
// The partial specialisation below covers all PD pins; explicit full specialisations
// for PD7, PD14, and PD15 below override the non-bonded pins with deleted members.
template <uint8_t PinNum>
struct Pin<PortD, PinNum> : ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<
                                PinNum, ohal::platforms::stm32u0::stm32u083::GpioD> {};

// GPIOF: only PF0–PF3 are bonded out on both MC packages.
// Bits 4–15 are absent; explicit full specialisations below override them.
template <uint8_t PinNum>
struct Pin<PortF, PinNum> : ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<
                                PinNum, ohal::platforms::stm32u0::stm32u083::GpioF> {};

template <>
struct Port<PortA> : ohal::platforms::stm32u0::stm32u083::GpioPortImpl<
                         ohal::platforms::stm32u0::stm32u083::GpioA> {};

template <>
struct Port<PortB> : ohal::platforms::stm32u0::stm32u083::GpioPortImpl<
                         ohal::platforms::stm32u0::stm32u083::GpioB> {};

template <>
struct Port<PortC> : ohal::platforms::stm32u0::stm32u083::GpioPortImpl<
                         ohal::platforms::stm32u0::stm32u083::GpioC> {};

template <>
struct Port<PortD> : ohal::platforms::stm32u0::stm32u083::GpioPortImpl<
                         ohal::platforms::stm32u0::stm32u083::GpioD> {};

template <>
struct Port<PortF> : ohal::platforms::stm32u0::stm32u083::GpioPortImpl<
                         ohal::platforms::stm32u0::stm32u083::GpioF> {};

} // namespace ohal::gpio

// ---------------------------------------------------------------------------
// Compile-time guards for non-bonded pins within partially-bonded ports.
//
// Pin<PortD, PinNum> and Pin<PortF, PinNum> cover all PinNum values via the
// partial specialisations above, but not every bit is physically bonded on
// the MC 80/81-pin packages.  The explicit full specialisations below override
// the partial spec for every non-bonded PortD and PortF pin with deleted
// member functions, producing a clear diagnostic for any attempt to use a pad
// that does not exist on these packages.
// ---------------------------------------------------------------------------

namespace ohal::gpio {

namespace detail {
/// Helper base for a GPIO pin that is not bonded out on the STM32U083MC packages.
/// All member functions are deleted so that any attempt to call them produces
/// a "call to deleted function" error pointing at the non-bonded pad.
struct NotBondedMc {
  static void set_mode(PinMode) noexcept = delete;           ///< pin not bonded out (MC)
  static void set_output_type(OutputType) noexcept = delete; ///< pin not bonded out (MC)
  static void set_speed(Speed) noexcept = delete;            ///< pin not bonded out (MC)
  static void set_pull(Pull) noexcept = delete;              ///< pin not bonded out (MC)
  static void set() noexcept = delete;                       ///< pin not bonded out (MC)
  static void clear() noexcept = delete;                     ///< pin not bonded out (MC)
  static Level read_input() noexcept = delete;               ///< pin not bonded out (MC)
  static Level read_output() noexcept = delete;              ///< pin not bonded out (MC)
  static void toggle() noexcept = delete;                    ///< pin not bonded out (MC)
};
} // namespace detail

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

// GPIOD: PD7, PD14, and PD15 are not bonded out on the MC packages.
// Bits 0–6 and 8–13 are bonded; PD7, PD14, PD15 are absent.
template <>
struct Pin<PortD, 7> : detail::NotBondedMc {}; ///< PD7  not bonded (MC)
template <>
struct Pin<PortD, 14> : detail::NotBondedMc {}; ///< PD14 not bonded (MC)
template <>
struct Pin<PortD, 15> : detail::NotBondedMc {}; ///< PD15 not bonded (MC)

// GPIOF: only PF0–PF3 are bonded out on the MC packages.
// Bits 4–15 are absent on these packages.
template <>
struct Pin<PortF, 4> : detail::NotBondedMc {}; ///< PF4  not bonded (MC)
template <>
struct Pin<PortF, 5> : detail::NotBondedMc {}; ///< PF5  not bonded (MC)
template <>
struct Pin<PortF, 6> : detail::NotBondedMc {}; ///< PF6  not bonded (MC)
template <>
struct Pin<PortF, 7> : detail::NotBondedMc {}; ///< PF7  not bonded (MC)
template <>
struct Pin<PortF, 8> : detail::NotBondedMc {}; ///< PF8  not bonded (MC)
template <>
struct Pin<PortF, 9> : detail::NotBondedMc {}; ///< PF9  not bonded (MC)
template <>
struct Pin<PortF, 10> : detail::NotBondedMc {}; ///< PF10 not bonded (MC)
template <>
struct Pin<PortF, 11> : detail::NotBondedMc {}; ///< PF11 not bonded (MC)
template <>
struct Pin<PortF, 12> : detail::NotBondedMc {}; ///< PF12 not bonded (MC)
template <>
struct Pin<PortF, 13> : detail::NotBondedMc {}; ///< PF13 not bonded (MC)
template <>
struct Pin<PortF, 14> : detail::NotBondedMc {}; ///< PF14 not bonded (MC)
template <>
struct Pin<PortF, 15> : detail::NotBondedMc {}; ///< PF15 not bonded (MC)

// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083MC_GPIO_HPP
