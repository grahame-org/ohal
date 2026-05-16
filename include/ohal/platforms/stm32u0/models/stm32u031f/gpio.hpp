#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U031F_GPIO_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U031F_GPIO_HPP

// Shared GPIO Pin<>/Port<> specialisations for STM32U031F TSSOP20 packages:
//   STM32U031F4P, STM32U031F6P, STM32U031F8P.
//
// Bonded-out ports:
//   GPIOA (bits 0-14; PA15 absent), GPIOB (bits 0-1, 4-9),
//   GPIOC (bits 14-15 only), GPIOF (bits 2-3 only).
// GPIOD and GPIOE are not bonded out on this package.
//
// This header includes the shared STM32U083 GPIO register map and adds
// explicit full specialisations to mark non-bonded pins as deleted.

#include "ohal/platforms/stm32u0/models/stm32u083/gpio.hpp"

// ---------------------------------------------------------------------------
// Compile-time guards for non-bonded pins on STM32U031F TSSOP20 packages.
//
// stm32u083/gpio.hpp provides Pin<PortA/B/C/F, PinNum> partial specialisations
// covering all bits 0-15.  The F packages only bond out:
//   PA0-PA14 (PA15 absent),
//   PB0-PB1, PB4-PB9 (PB2-PB3 and PB10-PB15 absent),
//   PC14-PC15 (PC0-PC13 absent),
//   PF2-PF3 (PF0-PF1 and PF4-PF15 absent).
// Explicit full specialisations below override the partial specs for every
// non-bonded pin with deleted member functions.
// ---------------------------------------------------------------------------

namespace ohal::gpio
{

namespace detail
{
/// Helper base for GPIO pins not bonded out on STM32U031F TSSOP20 packages.
struct NotBondedF
{
    static void set_mode(PinMode) noexcept = delete;           ///< pin not bonded out (F)
    static void set_output_type(OutputType) noexcept = delete; ///< pin not bonded out (F)
    static void set_speed(Speed) noexcept = delete;            ///< pin not bonded out (F)
    static void set_pull(Pull) noexcept = delete;              ///< pin not bonded out (F)
    static void set() noexcept = delete;                       ///< pin not bonded out (F)
    static void clear() noexcept = delete;                     ///< pin not bonded out (F)
    static Level read_input() noexcept = delete;               ///< pin not bonded out (F)
    static Level read_output() noexcept = delete;              ///< pin not bonded out (F)
    static void toggle() noexcept = delete;                    ///< pin not bonded out (F)
};
} // namespace detail

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

// GPIOA: bit 15 absent on STM32U031F TSSOP20 packages.
template <>
struct Pin<PortA, 15> : detail::NotBondedF
{
}; ///< PA15 not bonded (F)

// GPIOB: bits 2-3, 10-15 absent on STM32U031F TSSOP20 packages.
template <>
struct Pin<PortB, 2> : detail::NotBondedF
{
}; ///< PB2  not bonded (F)
template <>
struct Pin<PortB, 3> : detail::NotBondedF
{
}; ///< PB3  not bonded (F)
template <>
struct Pin<PortB, 10> : detail::NotBondedF
{
}; ///< PB10 not bonded (F)
template <>
struct Pin<PortB, 11> : detail::NotBondedF
{
}; ///< PB11 not bonded (F)
template <>
struct Pin<PortB, 12> : detail::NotBondedF
{
}; ///< PB12 not bonded (F)
template <>
struct Pin<PortB, 13> : detail::NotBondedF
{
}; ///< PB13 not bonded (F)
template <>
struct Pin<PortB, 14> : detail::NotBondedF
{
}; ///< PB14 not bonded (F)
template <>
struct Pin<PortB, 15> : detail::NotBondedF
{
}; ///< PB15 not bonded (F)

// GPIOC: bits 0-13 absent on STM32U031F TSSOP20 packages.
template <>
struct Pin<PortC, 0> : detail::NotBondedF
{
}; ///< PC0  not bonded (F)
template <>
struct Pin<PortC, 1> : detail::NotBondedF
{
}; ///< PC1  not bonded (F)
template <>
struct Pin<PortC, 2> : detail::NotBondedF
{
}; ///< PC2  not bonded (F)
template <>
struct Pin<PortC, 3> : detail::NotBondedF
{
}; ///< PC3  not bonded (F)
template <>
struct Pin<PortC, 4> : detail::NotBondedF
{
}; ///< PC4  not bonded (F)
template <>
struct Pin<PortC, 5> : detail::NotBondedF
{
}; ///< PC5  not bonded (F)
template <>
struct Pin<PortC, 6> : detail::NotBondedF
{
}; ///< PC6  not bonded (F)
template <>
struct Pin<PortC, 7> : detail::NotBondedF
{
}; ///< PC7  not bonded (F)
template <>
struct Pin<PortC, 8> : detail::NotBondedF
{
}; ///< PC8  not bonded (F)
template <>
struct Pin<PortC, 9> : detail::NotBondedF
{
}; ///< PC9  not bonded (F)
template <>
struct Pin<PortC, 10> : detail::NotBondedF
{
}; ///< PC10 not bonded (F)
template <>
struct Pin<PortC, 11> : detail::NotBondedF
{
}; ///< PC11 not bonded (F)
template <>
struct Pin<PortC, 12> : detail::NotBondedF
{
}; ///< PC12 not bonded (F)
template <>
struct Pin<PortC, 13> : detail::NotBondedF
{
}; ///< PC13 not bonded (F)

// GPIOF: bits 0-1, 4-15 absent on STM32U031F TSSOP20 packages.
template <>
struct Pin<PortF, 0> : detail::NotBondedF
{
}; ///< PF0  not bonded (F)
template <>
struct Pin<PortF, 1> : detail::NotBondedF
{
}; ///< PF1  not bonded (F)
template <>
struct Pin<PortF, 4> : detail::NotBondedF
{
}; ///< PF4  not bonded (F)
template <>
struct Pin<PortF, 5> : detail::NotBondedF
{
}; ///< PF5  not bonded (F)
template <>
struct Pin<PortF, 6> : detail::NotBondedF
{
}; ///< PF6  not bonded (F)
template <>
struct Pin<PortF, 7> : detail::NotBondedF
{
}; ///< PF7  not bonded (F)
template <>
struct Pin<PortF, 8> : detail::NotBondedF
{
}; ///< PF8  not bonded (F)
template <>
struct Pin<PortF, 9> : detail::NotBondedF
{
}; ///< PF9  not bonded (F)
template <>
struct Pin<PortF, 10> : detail::NotBondedF
{
}; ///< PF10 not bonded (F)
template <>
struct Pin<PortF, 11> : detail::NotBondedF
{
}; ///< PF11 not bonded (F)
template <>
struct Pin<PortF, 12> : detail::NotBondedF
{
}; ///< PF12 not bonded (F)
template <>
struct Pin<PortF, 13> : detail::NotBondedF
{
}; ///< PF13 not bonded (F)
template <>
struct Pin<PortF, 14> : detail::NotBondedF
{
}; ///< PF14 not bonded (F)
template <>
struct Pin<PortF, 15> : detail::NotBondedF
{
}; ///< PF15 not bonded (F)

// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U031F_GPIO_HPP
