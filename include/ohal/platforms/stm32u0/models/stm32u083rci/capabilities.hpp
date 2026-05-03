#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083RCI_CAPABILITIES_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083RCI_CAPABILITIES_HPP

#include <type_traits>

#include "ohal/core/capabilities.hpp"
#include "ohal/gpio.hpp"

namespace ohal::gpio::capabilities {

namespace detail {
// STM32U083RCI (64-pin UFBGA) bonded-out pin ranges.
// GPIOA, GPIOB, GPIOC, GPIOD and GPIOF: all 16 bits (0–15) bonded out.
// GPIOE: not bonded out.

inline constexpr uint8_t kRciPortPinCount = 16U;

template <uint8_t PinNum>
using RciPortCapability = std::bool_constant<(PinNum < kRciPortPinCount)>;
} // namespace detail

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

// PortA — all bits 0–15 bonded out on the UFBGA64 package.
template <uint8_t PinNum>
struct supports_output_type<PortA, PinNum> : detail::RciPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortA, PinNum> : detail::RciPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortA, PinNum> : detail::RciPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortA, PinNum> : detail::RciPortCapability<PinNum> {};

// PortB — all bits 0–15 bonded out on the UFBGA64 package.
template <uint8_t PinNum>
struct supports_output_type<PortB, PinNum> : detail::RciPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortB, PinNum> : detail::RciPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortB, PinNum> : detail::RciPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortB, PinNum> : detail::RciPortCapability<PinNum> {};

// PortC — all bits 0–15 bonded out on the UFBGA64 package.
template <uint8_t PinNum>
struct supports_output_type<PortC, PinNum> : detail::RciPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortC, PinNum> : detail::RciPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortC, PinNum> : detail::RciPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortC, PinNum> : detail::RciPortCapability<PinNum> {};

// PortD — all bits 0–15 bonded out on the UFBGA64 package.
template <uint8_t PinNum>
struct supports_output_type<PortD, PinNum> : detail::RciPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortD, PinNum> : detail::RciPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortD, PinNum> : detail::RciPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortD, PinNum> : detail::RciPortCapability<PinNum> {};

// PortF — all bits 0–15 bonded out on the UFBGA64 package.
template <uint8_t PinNum>
struct supports_output_type<PortF, PinNum> : detail::RciPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortF, PinNum> : detail::RciPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortF, PinNum> : detail::RciPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortF, PinNum> : detail::RciPortCapability<PinNum> {};

// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio::capabilities

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083RCI_CAPABILITIES_HPP
