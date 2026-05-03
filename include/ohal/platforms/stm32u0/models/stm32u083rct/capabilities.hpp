#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083RCT_CAPABILITIES_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083RCT_CAPABILITIES_HPP

#include <type_traits>

#include "ohal/core/capabilities.hpp"
#include "ohal/gpio.hpp"

namespace ohal::gpio::capabilities {

namespace detail {
// STM32U083RCT (64-pin LQFP) bonded-out pin ranges.
// GPIOA, GPIOB, GPIOC, GPIOD and GPIOF: all 16 bits (0–15) bonded out.
// GPIOE: not bonded out.

inline constexpr uint8_t kRctPortPinCount = 16U;

template <uint8_t PinNum>
using RctPortCapability = std::bool_constant<(PinNum < kRctPortPinCount)>;
} // namespace detail

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

// PortA — all bits 0–15 bonded out on the LQFP64 package.
template <uint8_t PinNum>
struct supports_output_type<PortA, PinNum> : detail::RctPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortA, PinNum> : detail::RctPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortA, PinNum> : detail::RctPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortA, PinNum> : detail::RctPortCapability<PinNum> {};

// PortB — all bits 0–15 bonded out on the LQFP64 package.
template <uint8_t PinNum>
struct supports_output_type<PortB, PinNum> : detail::RctPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortB, PinNum> : detail::RctPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortB, PinNum> : detail::RctPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortB, PinNum> : detail::RctPortCapability<PinNum> {};

// PortC — all bits 0–15 bonded out on the LQFP64 package.
template <uint8_t PinNum>
struct supports_output_type<PortC, PinNum> : detail::RctPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortC, PinNum> : detail::RctPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortC, PinNum> : detail::RctPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortC, PinNum> : detail::RctPortCapability<PinNum> {};

// PortD — all bits 0–15 bonded out on the LQFP64 package.
template <uint8_t PinNum>
struct supports_output_type<PortD, PinNum> : detail::RctPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortD, PinNum> : detail::RctPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortD, PinNum> : detail::RctPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortD, PinNum> : detail::RctPortCapability<PinNum> {};

// PortF — all bits 0–15 bonded out on the LQFP64 package.
template <uint8_t PinNum>
struct supports_output_type<PortF, PinNum> : detail::RctPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortF, PinNum> : detail::RctPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortF, PinNum> : detail::RctPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortF, PinNum> : detail::RctPortCapability<PinNum> {};

// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio::capabilities

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083RCT_CAPABILITIES_HPP
