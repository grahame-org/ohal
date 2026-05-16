#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U031G_GPIO_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U031G_GPIO_HPP

// Shared GPIO Pin<>/Port<> specialisations for STM32U031G WLCSP27 packages:
//   STM32U031G6Y, STM32U031G8Y.
//
// Bonded-out ports:
//   GPIOA (bits 0-14; PA15 absent), GPIOB (bits 0-1, 5-7),
//   GPIOC (bits 14-15 only), GPIOF (bits 2-3 only).
// GPIOD and GPIOE are not bonded out on this package.
//
// This header includes the shared STM32U083 GPIO register map and adds
// explicit full specialisations to mark non-bonded pins as deleted.

#include "ohal/platforms/stm32u0/models/stm32u083/gpio.hpp"

// ---------------------------------------------------------------------------
// Compile-time guards for non-bonded pins on STM32U031G WLCSP27 packages.
//
// stm32u083/gpio.hpp provides Pin<PortA/B/C/F, PinNum> partial specialisations
// covering all bits 0-15.  The G packages only bond out:
//   PA0-PA14 (PA15 absent),
//   PB0-PB1, PB5-PB7 (PB2-PB4 and PB8-PB15 absent),
//   PC14-PC15 (PC0-PC13 absent),
//   PF2-PF3 (PF0-PF1 and PF4-PF15 absent).
// Explicit full specialisations below override the partial specs for every
// non-bonded pin with deleted member functions.
// ---------------------------------------------------------------------------

namespace ohal::gpio
{

namespace detail
{
/// Helper base for GPIO pins not bonded out on STM32U031G WLCSP27 packages.
struct NotBondedG
{
    static void set_mode(PinMode) noexcept = delete;           ///< pin not bonded out (G)
    static void set_output_type(OutputType) noexcept = delete; ///< pin not bonded out (G)
    static void set_speed(Speed) noexcept = delete;            ///< pin not bonded out (G)
    static void set_pull(Pull) noexcept = delete;              ///< pin not bonded out (G)
    static void set() noexcept = delete;                       ///< pin not bonded out (G)
    static void clear() noexcept = delete;                     ///< pin not bonded out (G)
    static Level read_input() noexcept = delete;               ///< pin not bonded out (G)
    static Level read_output() noexcept = delete;              ///< pin not bonded out (G)
    static void toggle() noexcept = delete;                    ///< pin not bonded out (G)
};
} // namespace detail

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

// GPIOA: bit 15 absent on STM32U031G WLCSP27 packages.
template <>
struct Pin<PortA, 15> : detail::NotBondedG
{
}; ///< PA15 not bonded (G)

// GPIOB: bits 2-4, 8-15 absent on STM32U031G WLCSP27 packages.
template <>
struct Pin<PortB, 2> : detail::NotBondedG
{
}; ///< PB2  not bonded (G)
template <>
struct Pin<PortB, 3> : detail::NotBondedG
{
}; ///< PB3  not bonded (G)
template <>
struct Pin<PortB, 4> : detail::NotBondedG
{
}; ///< PB4  not bonded (G)
template <>
struct Pin<PortB, 8> : detail::NotBondedG
{
}; ///< PB8  not bonded (G)
template <>
struct Pin<PortB, 9> : detail::NotBondedG
{
}; ///< PB9  not bonded (G)
template <>
struct Pin<PortB, 10> : detail::NotBondedG
{
}; ///< PB10 not bonded (G)
template <>
struct Pin<PortB, 11> : detail::NotBondedG
{
}; ///< PB11 not bonded (G)
template <>
struct Pin<PortB, 12> : detail::NotBondedG
{
}; ///< PB12 not bonded (G)
template <>
struct Pin<PortB, 13> : detail::NotBondedG
{
}; ///< PB13 not bonded (G)
template <>
struct Pin<PortB, 14> : detail::NotBondedG
{
}; ///< PB14 not bonded (G)
template <>
struct Pin<PortB, 15> : detail::NotBondedG
{
}; ///< PB15 not bonded (G)

// GPIOC: bits 0-13 absent on STM32U031G WLCSP27 packages.
template <>
struct Pin<PortC, 0> : detail::NotBondedG
{
}; ///< PC0  not bonded (G)
template <>
struct Pin<PortC, 1> : detail::NotBondedG
{
}; ///< PC1  not bonded (G)
template <>
struct Pin<PortC, 2> : detail::NotBondedG
{
}; ///< PC2  not bonded (G)
template <>
struct Pin<PortC, 3> : detail::NotBondedG
{
}; ///< PC3  not bonded (G)
template <>
struct Pin<PortC, 4> : detail::NotBondedG
{
}; ///< PC4  not bonded (G)
template <>
struct Pin<PortC, 5> : detail::NotBondedG
{
}; ///< PC5  not bonded (G)
template <>
struct Pin<PortC, 6> : detail::NotBondedG
{
}; ///< PC6  not bonded (G)
template <>
struct Pin<PortC, 7> : detail::NotBondedG
{
}; ///< PC7  not bonded (G)
template <>
struct Pin<PortC, 8> : detail::NotBondedG
{
}; ///< PC8  not bonded (G)
template <>
struct Pin<PortC, 9> : detail::NotBondedG
{
}; ///< PC9  not bonded (G)
template <>
struct Pin<PortC, 10> : detail::NotBondedG
{
}; ///< PC10 not bonded (G)
template <>
struct Pin<PortC, 11> : detail::NotBondedG
{
}; ///< PC11 not bonded (G)
template <>
struct Pin<PortC, 12> : detail::NotBondedG
{
}; ///< PC12 not bonded (G)
template <>
struct Pin<PortC, 13> : detail::NotBondedG
{
}; ///< PC13 not bonded (G)

// GPIOF: bits 0-1, 4-15 absent on STM32U031G WLCSP27 packages.
template <>
struct Pin<PortF, 0> : detail::NotBondedG
{
}; ///< PF0  not bonded (G)
template <>
struct Pin<PortF, 1> : detail::NotBondedG
{
}; ///< PF1  not bonded (G)
template <>
struct Pin<PortF, 4> : detail::NotBondedG
{
}; ///< PF4  not bonded (G)
template <>
struct Pin<PortF, 5> : detail::NotBondedG
{
}; ///< PF5  not bonded (G)
template <>
struct Pin<PortF, 6> : detail::NotBondedG
{
}; ///< PF6  not bonded (G)
template <>
struct Pin<PortF, 7> : detail::NotBondedG
{
}; ///< PF7  not bonded (G)
template <>
struct Pin<PortF, 8> : detail::NotBondedG
{
}; ///< PF8  not bonded (G)
template <>
struct Pin<PortF, 9> : detail::NotBondedG
{
}; ///< PF9  not bonded (G)
template <>
struct Pin<PortF, 10> : detail::NotBondedG
{
}; ///< PF10 not bonded (G)
template <>
struct Pin<PortF, 11> : detail::NotBondedG
{
}; ///< PF11 not bonded (G)
template <>
struct Pin<PortF, 12> : detail::NotBondedG
{
}; ///< PF12 not bonded (G)
template <>
struct Pin<PortF, 13> : detail::NotBondedG
{
}; ///< PF13 not bonded (G)
template <>
struct Pin<PortF, 14> : detail::NotBondedG
{
}; ///< PF14 not bonded (G)
template <>
struct Pin<PortF, 15> : detail::NotBondedG
{
}; ///< PF15 not bonded (G)

// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U031G_GPIO_HPP
