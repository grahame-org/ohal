#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083CC_GPIO_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083CC_GPIO_HPP

// Shared STM32U083CC GPIO header for the 48-pin packages (CCI UFQFPN and CCT LQFP).
// Bonded-out ports: GPIOA (bits 0–15), GPIOB (bits 0–15),
//                   GPIOC (bits 13–15 only), GPIOF (bits 0–3 only).
// GPIOD and GPIOE are not bonded out on these packages.

#include "ohal/platforms/stm32u0/models/stm32u083/gpio.hpp"

// ---------------------------------------------------------------------------
// Compile-time guards for the STM32U083CC 48-pin packages.
//
// The shared stm32u083/gpio.hpp provides Pin<PortC, PinNum> and
// Pin<PortF, PinNum> partial specialisations covering all bits 0–15.
// The CC packages only bond out PC13–PC15 and PF0–PF3; all other bits are
// absent.  The explicit full specialisations below override the shared partial
// specs for the non-bonded bits with deleted member functions, so that code
// targeting a non-existent pad produces a clear compile-time diagnostic.
// ---------------------------------------------------------------------------

namespace ohal::gpio
{

namespace detail
{
/// Helper base for a GPIO pin that is not bonded out on the STM32U083CC 48-pin packages.
/// All member functions are deleted so that any attempt to call them produces
/// a "call to deleted function" error pointing at the non-bonded pad.
struct NotBondedCc
{
    static void set_mode(PinMode) noexcept = delete;           ///< pin not bonded out on CC
    static void set_output_type(OutputType) noexcept = delete; ///< pin not bonded out on CC
    static void set_speed(Speed) noexcept = delete;            ///< pin not bonded out on CC
    static void set_pull(Pull) noexcept = delete;              ///< pin not bonded out on CC
    static void set() noexcept = delete;                       ///< pin not bonded out on CC
    static void clear() noexcept = delete;                     ///< pin not bonded out on CC
    static Level read_input() noexcept = delete;               ///< pin not bonded out on CC
    static Level read_output() noexcept = delete;              ///< pin not bonded out on CC
    static void toggle() noexcept = delete;                    ///< pin not bonded out on CC
};
} // namespace detail

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

// GPIOC: only bits 13, 14, 15 are bonded out on the STM32U083CC 48-pin packages.
// Bits 0–12 are absent on these packages; using them is a compile-time error.
template <>
struct Pin<PortC, 0> : detail::NotBondedCc
{
}; ///< PC0  not bonded (CC)
template <>
struct Pin<PortC, 1> : detail::NotBondedCc
{
}; ///< PC1  not bonded (CC)
template <>
struct Pin<PortC, 2> : detail::NotBondedCc
{
}; ///< PC2  not bonded (CC)
template <>
struct Pin<PortC, 3> : detail::NotBondedCc
{
}; ///< PC3  not bonded (CC)
template <>
struct Pin<PortC, 4> : detail::NotBondedCc
{
}; ///< PC4  not bonded (CC)
template <>
struct Pin<PortC, 5> : detail::NotBondedCc
{
}; ///< PC5  not bonded (CC)
template <>
struct Pin<PortC, 6> : detail::NotBondedCc
{
}; ///< PC6  not bonded (CC)
template <>
struct Pin<PortC, 7> : detail::NotBondedCc
{
}; ///< PC7  not bonded (CC)
template <>
struct Pin<PortC, 8> : detail::NotBondedCc
{
}; ///< PC8  not bonded (CC)
template <>
struct Pin<PortC, 9> : detail::NotBondedCc
{
}; ///< PC9  not bonded (CC)
template <>
struct Pin<PortC, 10> : detail::NotBondedCc
{
}; ///< PC10 not bonded (CC)
template <>
struct Pin<PortC, 11> : detail::NotBondedCc
{
}; ///< PC11 not bonded (CC)
template <>
struct Pin<PortC, 12> : detail::NotBondedCc
{
}; ///< PC12 not bonded (CC)

// GPIOF: only bits 0, 1, 2, 3 are bonded out on the STM32U083CC 48-pin packages.
// Bits 4–15 are absent on these packages; using them is a compile-time error.
template <>
struct Pin<PortF, 4> : detail::NotBondedCc
{
}; ///< PF4  not bonded (CC)
template <>
struct Pin<PortF, 5> : detail::NotBondedCc
{
}; ///< PF5  not bonded (CC)
template <>
struct Pin<PortF, 6> : detail::NotBondedCc
{
}; ///< PF6  not bonded (CC)
template <>
struct Pin<PortF, 7> : detail::NotBondedCc
{
}; ///< PF7  not bonded (CC)
template <>
struct Pin<PortF, 8> : detail::NotBondedCc
{
}; ///< PF8  not bonded (CC)
template <>
struct Pin<PortF, 9> : detail::NotBondedCc
{
}; ///< PF9  not bonded (CC)
template <>
struct Pin<PortF, 10> : detail::NotBondedCc
{
}; ///< PF10 not bonded (CC)
template <>
struct Pin<PortF, 11> : detail::NotBondedCc
{
}; ///< PF11 not bonded (CC)
template <>
struct Pin<PortF, 12> : detail::NotBondedCc
{
}; ///< PF12 not bonded (CC)
template <>
struct Pin<PortF, 13> : detail::NotBondedCc
{
}; ///< PF13 not bonded (CC)
template <>
struct Pin<PortF, 14> : detail::NotBondedCc
{
}; ///< PF14 not bonded (CC)
template <>
struct Pin<PortF, 15> : detail::NotBondedCc
{
}; ///< PF15 not bonded (CC)

// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083CC_GPIO_HPP
