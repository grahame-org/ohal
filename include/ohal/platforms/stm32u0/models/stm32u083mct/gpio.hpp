#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083MCT_GPIO_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083MCT_GPIO_HPP

// GPIO Pin<>/Port<> specialisations for the STM32U083MCT (80-pin LQFP).
//
// GPIOA/B/C/D/F specialisations are inherited from the shared MC header.
// GPIOE: only PE7, PE8, PE9 are bonded out on the 80-pin LQFP package.
// PE3 is not bonded on this package (available on UFBGA81 only).

#include "ohal/platforms/stm32u0/models/stm32u083mc/gpio.hpp"

namespace ohal::gpio {

// GPIOE: PE7–PE9 are bonded out on the STM32U083MCT (LQFP80).
// The partial specialisation below covers all PE pins via the implementation;
// explicit full specialisations for non-bonded pins override with deleted members.
template <uint8_t PinNum>
struct Pin<PortE, PinNum> : ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<
                                PinNum, ohal::platforms::stm32u0::stm32u083::GpioE> {};

template <>
struct Port<PortE> : ohal::platforms::stm32u0::stm32u083::GpioPortImpl<
                         ohal::platforms::stm32u0::stm32u083::GpioE> {};

} // namespace ohal::gpio

// ---------------------------------------------------------------------------
// Compile-time guards for non-bonded GPIOE pins on the STM32U083MCT (LQFP80).
// Only PE7, PE8, PE9 are bonded out; all other PE pins are absent.
// ---------------------------------------------------------------------------

namespace ohal::gpio {

namespace detail {
/// Helper base for a GPIO pin that is not bonded out on the STM32U083MCT package.
struct NotBondedMct {
  static void set_mode(PinMode) noexcept = delete;           ///< pin not bonded out (MCT)
  static void set_output_type(OutputType) noexcept = delete; ///< pin not bonded out (MCT)
  static void set_speed(Speed) noexcept = delete;            ///< pin not bonded out (MCT)
  static void set_pull(Pull) noexcept = delete;              ///< pin not bonded out (MCT)
  static void set() noexcept = delete;                       ///< pin not bonded out (MCT)
  static void clear() noexcept = delete;                     ///< pin not bonded out (MCT)
  static Level read_input() noexcept = delete;               ///< pin not bonded out (MCT)
  static Level read_output() noexcept = delete;              ///< pin not bonded out (MCT)
  static void toggle() noexcept = delete;                    ///< pin not bonded out (MCT)
};
} // namespace detail

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

// GPIOE: PE0–PE6, PE10–PE15 are not bonded out on the STM32U083MCT (LQFP80).
// PE3 is also absent on this package (available on UFBGA81 only).
template <>
struct Pin<PortE, 0> : detail::NotBondedMct {}; ///< PE0  not bonded (MCT)
template <>
struct Pin<PortE, 1> : detail::NotBondedMct {}; ///< PE1  not bonded (MCT)
template <>
struct Pin<PortE, 2> : detail::NotBondedMct {}; ///< PE2  not bonded (MCT)
template <>
struct Pin<PortE, 3> : detail::NotBondedMct {}; ///< PE3  not bonded (MCT — absent on LQFP80)
template <>
struct Pin<PortE, 4> : detail::NotBondedMct {}; ///< PE4  not bonded (MCT)
template <>
struct Pin<PortE, 5> : detail::NotBondedMct {}; ///< PE5  not bonded (MCT)
template <>
struct Pin<PortE, 6> : detail::NotBondedMct {}; ///< PE6  not bonded (MCT)
// PE7, PE8, PE9 are bonded out — no override, uses the partial specialisation above.
template <>
struct Pin<PortE, 10> : detail::NotBondedMct {}; ///< PE10 not bonded (MCT)
template <>
struct Pin<PortE, 11> : detail::NotBondedMct {}; ///< PE11 not bonded (MCT)
template <>
struct Pin<PortE, 12> : detail::NotBondedMct {}; ///< PE12 not bonded (MCT)
template <>
struct Pin<PortE, 13> : detail::NotBondedMct {}; ///< PE13 not bonded (MCT)
template <>
struct Pin<PortE, 14> : detail::NotBondedMct {}; ///< PE14 not bonded (MCT)
template <>
struct Pin<PortE, 15> : detail::NotBondedMct {}; ///< PE15 not bonded (MCT)

// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083MCT_GPIO_HPP
