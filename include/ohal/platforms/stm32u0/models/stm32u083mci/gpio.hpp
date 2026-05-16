#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083MCI_GPIO_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083MCI_GPIO_HPP

// GPIO Pin<>/Port<> specialisations for the STM32U083MCI (81-pin UFBGA).
//
// GPIOA/B/C/D/F specialisations are inherited from the shared MC header.
// GPIOE: PE3, PE7, PE8, PE9 are bonded out on the 81-pin UFBGA package.
// PE3 is only bonded on this package (absent on LQFP80).

#include "ohal/platforms/stm32u0/models/stm32u083mc/gpio.hpp"

namespace ohal::gpio
{

// GPIOE: PE3 and PE7–PE9 are bonded out on the STM32U083MCI (UFBGA81).
// The partial specialisation below covers all PE pins via the implementation;
// explicit full specialisations for non-bonded pins override with deleted members.
template <uint8_t PinNum>
struct Pin<PortE, PinNum> : ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<
                                PinNum, ohal::platforms::stm32u0::stm32u083::GpioE>
{
};

template <>
struct Port<PortE>
    : ohal::platforms::stm32u0::stm32u083::GpioPortImpl<ohal::platforms::stm32u0::stm32u083::GpioE>
{
};

} // namespace ohal::gpio

// ---------------------------------------------------------------------------
// Compile-time guards for non-bonded GPIOE pins on the STM32U083MCI (UFBGA81).
// PE3 and PE7–PE9 are bonded out; all other PE pins are absent.
// ---------------------------------------------------------------------------

namespace ohal::gpio
{

namespace detail
{
/// Helper base for a GPIO pin that is not bonded out on the STM32U083MCI package.
struct NotBondedMci
{
    static void set_mode(PinMode) noexcept = delete;           ///< pin not bonded out (MCI)
    static void set_output_type(OutputType) noexcept = delete; ///< pin not bonded out (MCI)
    static void set_speed(Speed) noexcept = delete;            ///< pin not bonded out (MCI)
    static void set_pull(Pull) noexcept = delete;              ///< pin not bonded out (MCI)
    static void set() noexcept = delete;                       ///< pin not bonded out (MCI)
    static void clear() noexcept = delete;                     ///< pin not bonded out (MCI)
    static Level read_input() noexcept = delete;               ///< pin not bonded out (MCI)
    static Level read_output() noexcept = delete;              ///< pin not bonded out (MCI)
    static void toggle() noexcept = delete;                    ///< pin not bonded out (MCI)
};
} // namespace detail

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

// GPIOE: PE0–PE2, PE4–PE6, PE10–PE15 are not bonded out on the STM32U083MCI (UFBGA81).
// PE3 is bonded out on this package (unlike the MCT LQFP80).
template <>
struct Pin<PortE, 0> : detail::NotBondedMci
{
}; ///< PE0  not bonded (MCI)
template <>
struct Pin<PortE, 1> : detail::NotBondedMci
{
}; ///< PE1  not bonded (MCI)
template <>
struct Pin<PortE, 2> : detail::NotBondedMci
{
}; ///< PE2  not bonded (MCI)
// PE3 is bonded out on the MCI (UFBGA81) — no override, uses the partial specialisation.
template <>
struct Pin<PortE, 4> : detail::NotBondedMci
{
}; ///< PE4  not bonded (MCI)
template <>
struct Pin<PortE, 5> : detail::NotBondedMci
{
}; ///< PE5  not bonded (MCI)
template <>
struct Pin<PortE, 6> : detail::NotBondedMci
{
}; ///< PE6  not bonded (MCI)
// PE7, PE8, PE9 are bonded out — no override, uses the partial specialisation above.
template <>
struct Pin<PortE, 10> : detail::NotBondedMci
{
}; ///< PE10 not bonded (MCI)
template <>
struct Pin<PortE, 11> : detail::NotBondedMci
{
}; ///< PE11 not bonded (MCI)
template <>
struct Pin<PortE, 12> : detail::NotBondedMci
{
}; ///< PE12 not bonded (MCI)
template <>
struct Pin<PortE, 13> : detail::NotBondedMci
{
}; ///< PE13 not bonded (MCI)
template <>
struct Pin<PortE, 14> : detail::NotBondedMci
{
}; ///< PE14 not bonded (MCI)
template <>
struct Pin<PortE, 15> : detail::NotBondedMci
{
}; ///< PE15 not bonded (MCI)

// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083MCI_GPIO_HPP
