#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083RCT_CAPABILITIES_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083RCT_CAPABILITIES_HPP

#include <type_traits>

#include "ohal/core/capabilities.hpp"
#include "ohal/gpio.hpp"

namespace ohal::gpio::capabilities {

namespace detail {
// STM32U083RCT (64-pin LQFP) bonded-out pin ranges.
// GPIOA, GPIOB, GPIOC: all 16 bits (0–15) bonded out.
// GPIOD: only PD2 bonded out (DS14463, §4.1).
// GPIOF: only PF0–PF3 bonded out (DS14463, §4.1).
// GPIOE: not bonded out.

inline constexpr uint8_t kRctFullPortPinCount = 16U;
inline constexpr uint8_t kRctPortDPin = 2U;
inline constexpr uint8_t kRctPortFBitCount = 4U;

template <uint8_t PinNum>
using RctFullPortCapability = std::bool_constant<(PinNum < kRctFullPortPinCount)>;

template <uint8_t PinNum>
using RctPortDCapability = std::bool_constant<(PinNum == kRctPortDPin)>;

template <uint8_t PinNum>
using RctPortFCapability = std::bool_constant<(PinNum < kRctPortFBitCount)>;
} // namespace detail

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

// PortA — all bits 0–15 bonded out on the LQFP64 package.
template <uint8_t PinNum>
struct supports_output_type<PortA, PinNum> : detail::RctFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortA, PinNum> : detail::RctFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortA, PinNum> : detail::RctFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortA, PinNum> : detail::RctFullPortCapability<PinNum> {};

// PortB — all bits 0–15 bonded out on the LQFP64 package.
template <uint8_t PinNum>
struct supports_output_type<PortB, PinNum> : detail::RctFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortB, PinNum> : detail::RctFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortB, PinNum> : detail::RctFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortB, PinNum> : detail::RctFullPortCapability<PinNum> {};

// PortC — all bits 0–15 bonded out on the LQFP64 package.
template <uint8_t PinNum>
struct supports_output_type<PortC, PinNum> : detail::RctFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortC, PinNum> : detail::RctFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortC, PinNum> : detail::RctFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortC, PinNum> : detail::RctFullPortCapability<PinNum> {};

// PortD — only PD2 bonded out on the LQFP64 package.
template <uint8_t PinNum>
struct supports_output_type<PortD, PinNum> : detail::RctPortDCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortD, PinNum> : detail::RctPortDCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortD, PinNum> : detail::RctPortDCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortD, PinNum> : detail::RctPortDCapability<PinNum> {};

// PortF — only PF0–PF3 bonded out on the LQFP64 package.
template <uint8_t PinNum>
struct supports_output_type<PortF, PinNum> : detail::RctPortFCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortF, PinNum> : detail::RctPortFCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortF, PinNum> : detail::RctPortFCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortF, PinNum> : detail::RctPortFCapability<PinNum> {};

// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio::capabilities

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083RCT_CAPABILITIES_HPP
