#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083CCI_GPIO_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083CCI_GPIO_HPP

// The STM32U083CCI (48-pin UFQFPN) shares the same GPIO register map and base
// addresses as the other STM32U083 packages.
// Bonded-out ports: GPIOA (bits 0–15), GPIOB (bits 0–15),
//                   GPIOC (bits 13–15 only), GPIOF (bits 0–3 only).
// GPIOD and GPIOE are not bonded out on this package.

#include "ohal/platforms/stm32u0/models/stm32u083/gpio.hpp"

// ---------------------------------------------------------------------------
// Package-specific compile-time guards for the STM32U083CCI (48-pin UFQFPN).
//
// The shared stm32u083/gpio.hpp provides Pin<PortC, PinNum> and
// Pin<PortF, PinNum> partial specialisations covering all bits 0–15.
// The CCI package only bonds out PC13–PC15 and PF0–PF3; all other bits are
// absent.  The explicit full specialisations below override the shared partial
// specs for the non-bonded bits with deleted member functions, so that code
// targeting a non-existent pad produces a clear compile-time diagnostic.
// ---------------------------------------------------------------------------

namespace ohal::gpio {

namespace detail {
/// Helper base for a GPIO pin that is not bonded out on the STM32U083CCI.
/// All member functions are deleted so that any attempt to call them produces
/// a "call to deleted function" error pointing at the non-bonded pad.
struct NotBondedCci {
  static void set_mode(PinMode) noexcept = delete;           ///< pin not bonded out on CCI
  static void set_output_type(OutputType) noexcept = delete; ///< pin not bonded out on CCI
  static void set_speed(Speed) noexcept = delete;            ///< pin not bonded out on CCI
  static void set_pull(Pull) noexcept = delete;              ///< pin not bonded out on CCI
  static void set() noexcept = delete;                       ///< pin not bonded out on CCI
  static void clear() noexcept = delete;                     ///< pin not bonded out on CCI
  static Level read_input() noexcept = delete;               ///< pin not bonded out on CCI
  static Level read_output() noexcept = delete;              ///< pin not bonded out on CCI
  static void toggle() noexcept = delete;                    ///< pin not bonded out on CCI
};
} // namespace detail

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

// GPIOC: only bits 13, 14, 15 are bonded out on the STM32U083CCI (UFQFPN48).
// Bits 0–12 are absent on this package; using them is a compile-time error.
template <>
struct Pin<PortC, 0> : detail::NotBondedCci {}; ///< PC0  not bonded (CCI)
template <>
struct Pin<PortC, 1> : detail::NotBondedCci {}; ///< PC1  not bonded (CCI)
template <>
struct Pin<PortC, 2> : detail::NotBondedCci {}; ///< PC2  not bonded (CCI)
template <>
struct Pin<PortC, 3> : detail::NotBondedCci {}; ///< PC3  not bonded (CCI)
template <>
struct Pin<PortC, 4> : detail::NotBondedCci {}; ///< PC4  not bonded (CCI)
template <>
struct Pin<PortC, 5> : detail::NotBondedCci {}; ///< PC5  not bonded (CCI)
template <>
struct Pin<PortC, 6> : detail::NotBondedCci {}; ///< PC6  not bonded (CCI)
template <>
struct Pin<PortC, 7> : detail::NotBondedCci {}; ///< PC7  not bonded (CCI)
template <>
struct Pin<PortC, 8> : detail::NotBondedCci {}; ///< PC8  not bonded (CCI)
template <>
struct Pin<PortC, 9> : detail::NotBondedCci {}; ///< PC9  not bonded (CCI)
template <>
struct Pin<PortC, 10> : detail::NotBondedCci {}; ///< PC10 not bonded (CCI)
template <>
struct Pin<PortC, 11> : detail::NotBondedCci {}; ///< PC11 not bonded (CCI)
template <>
struct Pin<PortC, 12> : detail::NotBondedCci {}; ///< PC12 not bonded (CCI)

// GPIOF: only bits 0, 1, 2, 3 are bonded out on the STM32U083CCI (UFQFPN48).
// Bits 4–15 are absent on this package; using them is a compile-time error.
template <>
struct Pin<PortF, 4> : detail::NotBondedCci {}; ///< PF4  not bonded (CCI)
template <>
struct Pin<PortF, 5> : detail::NotBondedCci {}; ///< PF5  not bonded (CCI)
template <>
struct Pin<PortF, 6> : detail::NotBondedCci {}; ///< PF6  not bonded (CCI)
template <>
struct Pin<PortF, 7> : detail::NotBondedCci {}; ///< PF7  not bonded (CCI)
template <>
struct Pin<PortF, 8> : detail::NotBondedCci {}; ///< PF8  not bonded (CCI)
template <>
struct Pin<PortF, 9> : detail::NotBondedCci {}; ///< PF9  not bonded (CCI)
template <>
struct Pin<PortF, 10> : detail::NotBondedCci {}; ///< PF10 not bonded (CCI)
template <>
struct Pin<PortF, 11> : detail::NotBondedCci {}; ///< PF11 not bonded (CCI)
template <>
struct Pin<PortF, 12> : detail::NotBondedCci {}; ///< PF12 not bonded (CCI)
template <>
struct Pin<PortF, 13> : detail::NotBondedCci {}; ///< PF13 not bonded (CCI)
template <>
struct Pin<PortF, 14> : detail::NotBondedCci {}; ///< PF14 not bonded (CCI)
template <>
struct Pin<PortF, 15> : detail::NotBondedCci {}; ///< PF15 not bonded (CCI)

// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083CCI_GPIO_HPP
