#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U031K_CAPABILITIES_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U031K_CAPABILITIES_HPP

#include <type_traits>

#include "ohal/core/capabilities.hpp"
#include "ohal/gpio.hpp"

namespace ohal::gpio::capabilities
{

namespace detail
{
// STM32U031K (32-pin UFQFPN) bonded-out pin ranges.
// GPIOA: all 16 bits (0-15) bonded out.
// GPIOB: bits 0-1 and 3-7 bonded out (bit 2 and bits 8-15 absent).
// GPIOC: only bits 14-15 bonded out (bits 0-13 absent).
// GPIOF: only bits 2-3 bonded out (bits 0-1 and 4-15 absent).
// GPIOD and GPIOE: not bonded out.

inline constexpr uint8_t kKFullPortPinCount = 16U;
inline constexpr uint8_t kKPortBLowCount = 2U;  ///< PB0-PB1 bonded
inline constexpr uint8_t kKPortBMidStart = 3U;  ///< PB3-PB7 bonded start
inline constexpr uint8_t kKPortBMidEnd = 8U;    ///< PB3-PB7 bonded end (exclusive)
inline constexpr uint8_t kKPortCFirstBit = 14U; ///< PC14-PC15 bonded
inline constexpr uint8_t kKPortFFirstPin = 2U;  ///< PF2-PF3 bonded
inline constexpr uint8_t kKPortFLastPin = 3U;   ///< PF2-PF3 bonded

template <uint8_t PinNum>
using KFullPortCapability = std::bool_constant<(PinNum < kKFullPortPinCount)>;

template <uint8_t PinNum>
using KPortBCapability = std::bool_constant<(PinNum < kKPortBLowCount) ||
                                            (PinNum >= kKPortBMidStart && PinNum < kKPortBMidEnd)>;

template <uint8_t PinNum>
using KPortCCapability =
    std::bool_constant<(PinNum >= kKPortCFirstBit && PinNum < kKFullPortPinCount)>;

template <uint8_t PinNum>
using KPortFCapability =
    std::bool_constant<(PinNum == kKPortFFirstPin || PinNum == kKPortFLastPin)>;
} // namespace detail

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

// PortA — all bits 0-15 bonded out on all 32-pin K packages.
template <uint8_t PinNum>
struct supports_output_type<PortA, PinNum> : detail::KFullPortCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_output_speed<PortA, PinNum> : detail::KFullPortCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_pull<PortA, PinNum> : detail::KFullPortCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_alternate_function<PortA, PinNum> : detail::KFullPortCapability<PinNum>
{
};

// PortB — bits 0-1 and 3-7 bonded out on all 32-pin K packages.
template <uint8_t PinNum>
struct supports_output_type<PortB, PinNum> : detail::KPortBCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_output_speed<PortB, PinNum> : detail::KPortBCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_pull<PortB, PinNum> : detail::KPortBCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_alternate_function<PortB, PinNum> : detail::KPortBCapability<PinNum>
{
};

// PortC — only bits 14-15 bonded out on all 32-pin K packages.
template <uint8_t PinNum>
struct supports_output_type<PortC, PinNum> : detail::KPortCCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_output_speed<PortC, PinNum> : detail::KPortCCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_pull<PortC, PinNum> : detail::KPortCCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_alternate_function<PortC, PinNum> : detail::KPortCCapability<PinNum>
{
};

// PortF — only bits 2-3 bonded out on all 32-pin K packages.
template <uint8_t PinNum>
struct supports_output_type<PortF, PinNum> : detail::KPortFCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_output_speed<PortF, PinNum> : detail::KPortFCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_pull<PortF, PinNum> : detail::KPortFCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_alternate_function<PortF, PinNum> : detail::KPortFCapability<PinNum>
{
};

// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio::capabilities

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U031K_CAPABILITIES_HPP
