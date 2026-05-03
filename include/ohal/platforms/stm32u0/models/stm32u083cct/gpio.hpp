#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083CCT_GPIO_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083CCT_GPIO_HPP

// The STM32U083CCT (48-pin LQFP) shares the same GPIO register map and base
// addresses as the other STM32U083 packages.
// Bonded-out ports: GPIOA (bits 0–15), GPIOB (bits 0–15),
//                   GPIOC (bits 13–15 only), GPIOF (bits 0–3 only).
// GPIOD and GPIOE are not bonded out on this package.

#include "ohal/platforms/stm32u0/models/stm32u083/gpio.hpp"

// ---------------------------------------------------------------------------
// Package-specific compile-time guards for the STM32U083CCT (48-pin LQFP).
//
// The shared stm32u083/gpio.hpp provides Pin<PortC, PinNum> and
// Pin<PortF, PinNum> partial specialisations covering all bits 0–15.
// The CCT package only bonds out PC13–PC15 and PF0–PF3; all other bits are
// absent.  The explicit full specialisations below override the shared partial
// specs for the non-bonded bits with deleted member functions, so that code
// targeting a non-existent pad produces a clear compile-time diagnostic.
// ---------------------------------------------------------------------------

namespace ohal::gpio {

namespace detail {
/// Helper base for a GPIO pin that is not bonded out on the STM32U083CCT.
/// All member functions are deleted so that any attempt to call them produces
/// a "call to deleted function" error pointing at the non-bonded pad.
struct NotBondedCct {
  static void set_mode(PinMode) noexcept = delete;           ///< pin not bonded out on CCT
  static void set_output_type(OutputType) noexcept = delete; ///< pin not bonded out on CCT
  static void set_speed(Speed) noexcept = delete;            ///< pin not bonded out on CCT
  static void set_pull(Pull) noexcept = delete;              ///< pin not bonded out on CCT
  static void set() noexcept = delete;                       ///< pin not bonded out on CCT
  static void clear() noexcept = delete;                     ///< pin not bonded out on CCT
  static Level read_input() noexcept = delete;               ///< pin not bonded out on CCT
  static Level read_output() noexcept = delete;              ///< pin not bonded out on CCT
  static void toggle() noexcept = delete;                    ///< pin not bonded out on CCT
};
} // namespace detail

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

// GPIOC: only bits 13, 14, 15 are bonded out on the STM32U083CCT (LQFP48).
// Bits 0–12 are absent on this package; using them is a compile-time error.
template <>
struct Pin<PortC, 0> : detail::NotBondedCct {}; ///< PC0  not bonded (CCT)
template <>
struct Pin<PortC, 1> : detail::NotBondedCct {}; ///< PC1  not bonded (CCT)
template <>
struct Pin<PortC, 2> : detail::NotBondedCct {}; ///< PC2  not bonded (CCT)
template <>
struct Pin<PortC, 3> : detail::NotBondedCct {}; ///< PC3  not bonded (CCT)
template <>
struct Pin<PortC, 4> : detail::NotBondedCct {}; ///< PC4  not bonded (CCT)
template <>
struct Pin<PortC, 5> : detail::NotBondedCct {}; ///< PC5  not bonded (CCT)
template <>
struct Pin<PortC, 6> : detail::NotBondedCct {}; ///< PC6  not bonded (CCT)
template <>
struct Pin<PortC, 7> : detail::NotBondedCct {}; ///< PC7  not bonded (CCT)
template <>
struct Pin<PortC, 8> : detail::NotBondedCct {}; ///< PC8  not bonded (CCT)
template <>
struct Pin<PortC, 9> : detail::NotBondedCct {}; ///< PC9  not bonded (CCT)
template <>
struct Pin<PortC, 10> : detail::NotBondedCct {}; ///< PC10 not bonded (CCT)
template <>
struct Pin<PortC, 11> : detail::NotBondedCct {}; ///< PC11 not bonded (CCT)
template <>
struct Pin<PortC, 12> : detail::NotBondedCct {}; ///< PC12 not bonded (CCT)

// GPIOF: only bits 0, 1, 2, 3 are bonded out on the STM32U083CCT (LQFP48).
// Bits 4–15 are absent on this package; using them is a compile-time error.
template <>
struct Pin<PortF, 4> : detail::NotBondedCct {}; ///< PF4  not bonded (CCT)
template <>
struct Pin<PortF, 5> : detail::NotBondedCct {}; ///< PF5  not bonded (CCT)
template <>
struct Pin<PortF, 6> : detail::NotBondedCct {}; ///< PF6  not bonded (CCT)
template <>
struct Pin<PortF, 7> : detail::NotBondedCct {}; ///< PF7  not bonded (CCT)
template <>
struct Pin<PortF, 8> : detail::NotBondedCct {}; ///< PF8  not bonded (CCT)
template <>
struct Pin<PortF, 9> : detail::NotBondedCct {}; ///< PF9  not bonded (CCT)
template <>
struct Pin<PortF, 10> : detail::NotBondedCct {}; ///< PF10 not bonded (CCT)
template <>
struct Pin<PortF, 11> : detail::NotBondedCct {}; ///< PF11 not bonded (CCT)
template <>
struct Pin<PortF, 12> : detail::NotBondedCct {}; ///< PF12 not bonded (CCT)
template <>
struct Pin<PortF, 13> : detail::NotBondedCct {}; ///< PF13 not bonded (CCT)
template <>
struct Pin<PortF, 14> : detail::NotBondedCct {}; ///< PF14 not bonded (CCT)
template <>
struct Pin<PortF, 15> : detail::NotBondedCct {}; ///< PF15 not bonded (CCT)

// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083CCT_GPIO_HPP
