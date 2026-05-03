#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083RCI_CAPABILITIES_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083RCI_CAPABILITIES_HPP

#include <type_traits>

#include "ohal/core/capabilities.hpp"
#include "ohal/gpio.hpp"

namespace ohal::gpio::capabilities {

namespace detail {
// STM32U083RCI (64-pin UFBGA) bonded-out pin ranges.
// GPIOA, GPIOB, GPIOC: all 16 bits (0–15) bonded out.
// GPIOD: only PD2 bonded out (DS14463, §4.1).
// GPIOF: only PF0–PF3 bonded out (DS14463, §4.1).
// GPIOE: not bonded out.

inline constexpr uint8_t kRciFullPortPinCount = 16U;
inline constexpr uint8_t kRciPortDPin = 2U;
inline constexpr uint8_t kRciPortFBitCount = 4U;

template <uint8_t PinNum>
using RciFullPortCapability = std::bool_constant<(PinNum < kRciFullPortPinCount)>;

template <uint8_t PinNum>
using RciPortDCapability = std::bool_constant<(PinNum == kRciPortDPin)>;

template <uint8_t PinNum>
using RciPortFCapability = std::bool_constant<(PinNum < kRciPortFBitCount)>;
} // namespace detail

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

// PortA — all bits 0–15 bonded out on the UFBGA64 package.
template <uint8_t PinNum>
struct supports_output_type<PortA, PinNum> : detail::RciFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortA, PinNum> : detail::RciFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortA, PinNum> : detail::RciFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortA, PinNum> : detail::RciFullPortCapability<PinNum> {};

// PortB — all bits 0–15 bonded out on the UFBGA64 package.
template <uint8_t PinNum>
struct supports_output_type<PortB, PinNum> : detail::RciFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortB, PinNum> : detail::RciFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortB, PinNum> : detail::RciFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortB, PinNum> : detail::RciFullPortCapability<PinNum> {};

// PortC — all bits 0–15 bonded out on the UFBGA64 package.
template <uint8_t PinNum>
struct supports_output_type<PortC, PinNum> : detail::RciFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortC, PinNum> : detail::RciFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortC, PinNum> : detail::RciFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortC, PinNum> : detail::RciFullPortCapability<PinNum> {};

// PortD — only PD2 bonded out on the UFBGA64 package.
template <uint8_t PinNum>
struct supports_output_type<PortD, PinNum> : detail::RciPortDCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortD, PinNum> : detail::RciPortDCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortD, PinNum> : detail::RciPortDCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortD, PinNum> : detail::RciPortDCapability<PinNum> {};

// PortF — only PF0–PF3 bonded out on the UFBGA64 package.
template <uint8_t PinNum>
struct supports_output_type<PortF, PinNum> : detail::RciPortFCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortF, PinNum> : detail::RciPortFCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortF, PinNum> : detail::RciPortFCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortF, PinNum> : detail::RciPortFCapability<PinNum> {};

// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio::capabilities

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083RCI_CAPABILITIES_HPP
