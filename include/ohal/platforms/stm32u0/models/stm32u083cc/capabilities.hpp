#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083CC_CAPABILITIES_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083CC_CAPABILITIES_HPP

#include <type_traits>

#include "ohal/core/capabilities.hpp"
#include "ohal/gpio.hpp"

namespace ohal::gpio::capabilities
{

namespace detail
{
// STM32U083CC (48-pin UFQFPN and LQFP) bonded-out pin ranges.
// GPIOA and GPIOB: all 16 bits (0–15) bonded out.
// GPIOC: only bits 13–15 bonded out (bits 0–12 are absent on this package).
// GPIOF: only bits 0–3 bonded out (bits 4–15 are absent on this package).
// GPIOD and GPIOE: not bonded out.

inline constexpr uint8_t kCcFullPortPinCount = 16U;
inline constexpr uint8_t kCcPortCFirstBit = 13U;
inline constexpr uint8_t kCcPortFBitCount = 4U;

template <uint8_t PinNum>
using CcFullPortCapability = std::bool_constant<(PinNum < kCcFullPortPinCount)>;

template <uint8_t PinNum>
using CcPortCCapability =
    std::bool_constant<(PinNum >= kCcPortCFirstBit && PinNum < kCcFullPortPinCount)>;

template <uint8_t PinNum>
using CcPortFCapability = std::bool_constant<(PinNum < kCcPortFBitCount)>;
} // namespace detail

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

// PortA — all bits 0–15 bonded out on both 48-pin CC packages.
template <uint8_t PinNum>
struct supports_output_type<PortA, PinNum> : detail::CcFullPortCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_output_speed<PortA, PinNum> : detail::CcFullPortCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_pull<PortA, PinNum> : detail::CcFullPortCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_alternate_function<PortA, PinNum> : detail::CcFullPortCapability<PinNum>
{
};

// PortB — all bits 0–15 bonded out on both 48-pin CC packages.
template <uint8_t PinNum>
struct supports_output_type<PortB, PinNum> : detail::CcFullPortCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_output_speed<PortB, PinNum> : detail::CcFullPortCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_pull<PortB, PinNum> : detail::CcFullPortCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_alternate_function<PortB, PinNum> : detail::CcFullPortCapability<PinNum>
{
};

// PortC — only bits 13–15 bonded out on both 48-pin CC packages.
template <uint8_t PinNum>
struct supports_output_type<PortC, PinNum> : detail::CcPortCCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_output_speed<PortC, PinNum> : detail::CcPortCCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_pull<PortC, PinNum> : detail::CcPortCCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_alternate_function<PortC, PinNum> : detail::CcPortCCapability<PinNum>
{
};

// PortF — only bits 0–3 bonded out on both 48-pin CC packages.
template <uint8_t PinNum>
struct supports_output_type<PortF, PinNum> : detail::CcPortFCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_output_speed<PortF, PinNum> : detail::CcPortFCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_pull<PortF, PinNum> : detail::CcPortFCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_alternate_function<PortF, PinNum> : detail::CcPortFCapability<PinNum>
{
};

// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio::capabilities

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083CC_CAPABILITIES_HPP
