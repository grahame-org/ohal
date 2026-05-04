#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083RC_CAPABILITIES_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083RC_CAPABILITIES_HPP

#include <type_traits>

#include "ohal/core/capabilities.hpp"
#include "ohal/gpio.hpp"

namespace ohal::gpio::capabilities {

namespace detail {
// STM32U083RC (64-pin LQFP64 and UFBGA64) bonded-out pin ranges.
// GPIOA, GPIOB, GPIOC: all 16 bits (0–15) bonded out.
// GPIOD: only PD2 bonded out (DS14463, §4.1).
// GPIOF: only PF0–PF3 bonded out (DS14463, §4.1).
// GPIOE: not bonded out.

inline constexpr uint8_t kRcFullPortPinCount = 16U;
inline constexpr uint8_t kRcPortDPin = 2U;
inline constexpr uint8_t kRcPortFBitCount = 4U;

template <uint8_t PinNum>
using RcFullPortCapability = std::bool_constant<(PinNum < kRcFullPortPinCount)>;

template <uint8_t PinNum>
using RcPortDCapability = std::bool_constant<(PinNum == kRcPortDPin)>;

template <uint8_t PinNum>
using RcPortFCapability = std::bool_constant<(PinNum < kRcPortFBitCount)>;
} // namespace detail

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

// PortA — all bits 0–15 bonded out on both 64-pin RC packages.
template <uint8_t PinNum>
struct supports_output_type<PortA, PinNum> : detail::RcFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortA, PinNum> : detail::RcFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortA, PinNum> : detail::RcFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortA, PinNum> : detail::RcFullPortCapability<PinNum> {};

// PortB — all bits 0–15 bonded out on both 64-pin RC packages.
template <uint8_t PinNum>
struct supports_output_type<PortB, PinNum> : detail::RcFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortB, PinNum> : detail::RcFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortB, PinNum> : detail::RcFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortB, PinNum> : detail::RcFullPortCapability<PinNum> {};

// PortC — all bits 0–15 bonded out on both 64-pin RC packages.
template <uint8_t PinNum>
struct supports_output_type<PortC, PinNum> : detail::RcFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortC, PinNum> : detail::RcFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortC, PinNum> : detail::RcFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortC, PinNum> : detail::RcFullPortCapability<PinNum> {};

// PortD — only PD2 bonded out on both 64-pin RC packages.
template <uint8_t PinNum>
struct supports_output_type<PortD, PinNum> : detail::RcPortDCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortD, PinNum> : detail::RcPortDCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortD, PinNum> : detail::RcPortDCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortD, PinNum> : detail::RcPortDCapability<PinNum> {};

// PortF — only PF0–PF3 bonded out on both 64-pin RC packages.
template <uint8_t PinNum>
struct supports_output_type<PortF, PinNum> : detail::RcPortFCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortF, PinNum> : detail::RcPortFCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortF, PinNum> : detail::RcPortFCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortF, PinNum> : detail::RcPortFCapability<PinNum> {};

// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio::capabilities

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083RC_CAPABILITIES_HPP
