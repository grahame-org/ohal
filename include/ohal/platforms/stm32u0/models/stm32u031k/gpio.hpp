#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U031K_GPIO_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U031K_GPIO_HPP

// Shared GPIO Pin<>/Port<> specialisations for STM32U031K 32-pin packages:
//   STM32U031K4U, STM32U031K6U, STM32U031K8U (UFQFPN32).
//
// Bonded-out ports:
//   GPIOA (bits 0-15), GPIOB (bits 0-1, 3-7),
//   GPIOC (bits 14-15 only), GPIOF (bits 2-3 only).
// GPIOD and GPIOE are not bonded out on this package.
//
// This header includes the shared STM32U083 GPIO register map (register addresses
// and GpioPortPinImpl/GpioPortImpl types are identical for STM32U031) and adds
// explicit full specialisations to mark non-bonded pins as deleted.

#include "ohal/platforms/stm32u0/models/stm32u083/gpio.hpp"

// ---------------------------------------------------------------------------
// Compile-time guards for non-bonded pins on STM32U031K 32-pin packages.
//
// stm32u083/gpio.hpp provides Pin<PortA/B/C/F, PinNum> partial specialisations
// covering all bits 0-15.  The K packages only bond out:
//   PB0-PB1, PB3-PB7 (PB2 and PB8-PB15 absent),
//   PC14-PC15 (PC0-PC13 absent),
//   PF2-PF3 (PF0-PF1 and PF4-PF15 absent).
// Explicit full specialisations below override the partial specs for every
// non-bonded pin with deleted member functions.
// ---------------------------------------------------------------------------

namespace ohal::gpio {

namespace detail {
/// Helper base for GPIO pins not bonded out on STM32U031K 32-pin packages.
struct NotBondedK {
  static void set_mode(PinMode) noexcept = delete;           ///< pin not bonded out (K)
  static void set_output_type(OutputType) noexcept = delete; ///< pin not bonded out (K)
  static void set_speed(Speed) noexcept = delete;            ///< pin not bonded out (K)
  static void set_pull(Pull) noexcept = delete;              ///< pin not bonded out (K)
  static void set() noexcept = delete;                       ///< pin not bonded out (K)
  static void clear() noexcept = delete;                     ///< pin not bonded out (K)
  static Level read_input() noexcept = delete;               ///< pin not bonded out (K)
  static Level read_output() noexcept = delete;              ///< pin not bonded out (K)
  static void toggle() noexcept = delete;                    ///< pin not bonded out (K)
};
} // namespace detail

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

// GPIOB: bits 2, 8-15 absent on STM32U031K 32-pin packages.
template <>
struct Pin<PortB, 2> : detail::NotBondedK {}; ///< PB2  not bonded (K)
template <>
struct Pin<PortB, 8> : detail::NotBondedK {}; ///< PB8  not bonded (K)
template <>
struct Pin<PortB, 9> : detail::NotBondedK {}; ///< PB9  not bonded (K)
template <>
struct Pin<PortB, 10> : detail::NotBondedK {}; ///< PB10 not bonded (K)
template <>
struct Pin<PortB, 11> : detail::NotBondedK {}; ///< PB11 not bonded (K)
template <>
struct Pin<PortB, 12> : detail::NotBondedK {}; ///< PB12 not bonded (K)
template <>
struct Pin<PortB, 13> : detail::NotBondedK {}; ///< PB13 not bonded (K)
template <>
struct Pin<PortB, 14> : detail::NotBondedK {}; ///< PB14 not bonded (K)
template <>
struct Pin<PortB, 15> : detail::NotBondedK {}; ///< PB15 not bonded (K)

// GPIOC: bits 0-13 absent on STM32U031K 32-pin packages.
template <>
struct Pin<PortC, 0> : detail::NotBondedK {}; ///< PC0  not bonded (K)
template <>
struct Pin<PortC, 1> : detail::NotBondedK {}; ///< PC1  not bonded (K)
template <>
struct Pin<PortC, 2> : detail::NotBondedK {}; ///< PC2  not bonded (K)
template <>
struct Pin<PortC, 3> : detail::NotBondedK {}; ///< PC3  not bonded (K)
template <>
struct Pin<PortC, 4> : detail::NotBondedK {}; ///< PC4  not bonded (K)
template <>
struct Pin<PortC, 5> : detail::NotBondedK {}; ///< PC5  not bonded (K)
template <>
struct Pin<PortC, 6> : detail::NotBondedK {}; ///< PC6  not bonded (K)
template <>
struct Pin<PortC, 7> : detail::NotBondedK {}; ///< PC7  not bonded (K)
template <>
struct Pin<PortC, 8> : detail::NotBondedK {}; ///< PC8  not bonded (K)
template <>
struct Pin<PortC, 9> : detail::NotBondedK {}; ///< PC9  not bonded (K)
template <>
struct Pin<PortC, 10> : detail::NotBondedK {}; ///< PC10 not bonded (K)
template <>
struct Pin<PortC, 11> : detail::NotBondedK {}; ///< PC11 not bonded (K)
template <>
struct Pin<PortC, 12> : detail::NotBondedK {}; ///< PC12 not bonded (K)
template <>
struct Pin<PortC, 13> : detail::NotBondedK {}; ///< PC13 not bonded (K)

// GPIOF: bits 0-1, 4-15 absent on STM32U031K 32-pin packages.
template <>
struct Pin<PortF, 0> : detail::NotBondedK {}; ///< PF0  not bonded (K)
template <>
struct Pin<PortF, 1> : detail::NotBondedK {}; ///< PF1  not bonded (K)
template <>
struct Pin<PortF, 4> : detail::NotBondedK {}; ///< PF4  not bonded (K)
template <>
struct Pin<PortF, 5> : detail::NotBondedK {}; ///< PF5  not bonded (K)
template <>
struct Pin<PortF, 6> : detail::NotBondedK {}; ///< PF6  not bonded (K)
template <>
struct Pin<PortF, 7> : detail::NotBondedK {}; ///< PF7  not bonded (K)
template <>
struct Pin<PortF, 8> : detail::NotBondedK {}; ///< PF8  not bonded (K)
template <>
struct Pin<PortF, 9> : detail::NotBondedK {}; ///< PF9  not bonded (K)
template <>
struct Pin<PortF, 10> : detail::NotBondedK {}; ///< PF10 not bonded (K)
template <>
struct Pin<PortF, 11> : detail::NotBondedK {}; ///< PF11 not bonded (K)
template <>
struct Pin<PortF, 12> : detail::NotBondedK {}; ///< PF12 not bonded (K)
template <>
struct Pin<PortF, 13> : detail::NotBondedK {}; ///< PF13 not bonded (K)
template <>
struct Pin<PortF, 14> : detail::NotBondedK {}; ///< PF14 not bonded (K)
template <>
struct Pin<PortF, 15> : detail::NotBondedK {}; ///< PF15 not bonded (K)

// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U031K_GPIO_HPP
