#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U031G_CAPABILITIES_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U031G_CAPABILITIES_HPP

#include <type_traits>

#include "ohal/core/capabilities.hpp"
#include "ohal/gpio.hpp"

namespace ohal::gpio::capabilities
{

namespace detail
{
// STM32U031G (WLCSP27) bonded-out pin ranges.
// GPIOA: bits 0-14 bonded out (PA15 absent).
// GPIOB: bits 0-1 and 5-7 bonded out (bits 2-4 and 8-15 absent).
// GPIOC: only bits 14-15 bonded out (bits 0-13 absent).
// GPIOF: only bits 2-3 bonded out (bits 0-1 and 4-15 absent).
// GPIOD and GPIOE: not bonded out.

inline constexpr uint8_t kGFullPortPinCount = 16U;
inline constexpr uint8_t kGPortABitCount = 15U; ///< PA0-PA14 bonded (PA15 absent)
inline constexpr uint8_t kGPortBLowCount = 2U;  ///< PB0-PB1 bonded
inline constexpr uint8_t kGPortBMidStart = 5U;  ///< PB5-PB7 bonded start
inline constexpr uint8_t kGPortBMidEnd = 8U;    ///< PB5-PB7 bonded end (exclusive)
inline constexpr uint8_t kGPortCFirstBit = 14U; ///< PC14-PC15 bonded
inline constexpr uint8_t kGPortFFirstPin = 2U;  ///< PF2-PF3 bonded
inline constexpr uint8_t kGPortFLastPin = 3U;   ///< PF2-PF3 bonded

template <uint8_t PinNum>
using GPortACapability = std::bool_constant<(PinNum < kGPortABitCount)>;

template <uint8_t PinNum>
using GPortBCapability = std::bool_constant<(PinNum < kGPortBLowCount) ||
                                            (PinNum >= kGPortBMidStart && PinNum < kGPortBMidEnd)>;

template <uint8_t PinNum>
using GPortCCapability =
    std::bool_constant<(PinNum >= kGPortCFirstBit && PinNum < kGFullPortPinCount)>;

template <uint8_t PinNum>
using GPortFCapability =
    std::bool_constant<(PinNum == kGPortFFirstPin || PinNum == kGPortFLastPin)>;
} // namespace detail

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

// PortA — bits 0-14 bonded out (PA15 absent) on all WLCSP27 G packages.
template <uint8_t PinNum>
struct supports_output_type<PortA, PinNum> : detail::GPortACapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_output_speed<PortA, PinNum> : detail::GPortACapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_pull<PortA, PinNum> : detail::GPortACapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_alternate_function<PortA, PinNum> : detail::GPortACapability<PinNum>
{
};

// PortB — bits 0-1 and 5-7 bonded out on all WLCSP27 G packages.
template <uint8_t PinNum>
struct supports_output_type<PortB, PinNum> : detail::GPortBCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_output_speed<PortB, PinNum> : detail::GPortBCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_pull<PortB, PinNum> : detail::GPortBCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_alternate_function<PortB, PinNum> : detail::GPortBCapability<PinNum>
{
};

// PortC — only bits 14-15 bonded out on all WLCSP27 G packages.
template <uint8_t PinNum>
struct supports_output_type<PortC, PinNum> : detail::GPortCCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_output_speed<PortC, PinNum> : detail::GPortCCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_pull<PortC, PinNum> : detail::GPortCCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_alternate_function<PortC, PinNum> : detail::GPortCCapability<PinNum>
{
};

// PortF — only bits 2-3 bonded out on all WLCSP27 G packages.
template <uint8_t PinNum>
struct supports_output_type<PortF, PinNum> : detail::GPortFCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_output_speed<PortF, PinNum> : detail::GPortFCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_pull<PortF, PinNum> : detail::GPortFCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_alternate_function<PortF, PinNum> : detail::GPortFCapability<PinNum>
{
};

// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio::capabilities

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U031G_CAPABILITIES_HPP
