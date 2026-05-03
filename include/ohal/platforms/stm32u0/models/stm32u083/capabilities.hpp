#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_CAPABILITIES_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_CAPABILITIES_HPP

#include <type_traits>

#include "ohal/core/capabilities.hpp"
#include "ohal/gpio.hpp"

namespace ohal::gpio::capabilities {

namespace detail {
/// STM32U083 GPIO ports have 16 pins (0–15).  Used as the upper bound in the
/// capability specialisations below to ensure out-of-range pin numbers return false.
inline constexpr uint8_t kStm32u083PinCount = 16U;

/// Base for every STM32U083 GPIO capability trait specialisation.
/// Evaluates to `std::true_type` for valid pin numbers (0–15) and
/// `std::false_type` for any pin number outside that range.
template <uint8_t PinNum>
using Stm32u083PortCapability = std::bool_constant<(PinNum < kStm32u083PinCount)>;
} // namespace detail

// The STM32U083KCU is a 32-pin UFQFPN package.  Of the six GPIO ports present in
// silicon (A–F), only GPIOA, GPIOB, GPIOC and GPIOF have bonded-out pins on this
// package; GPIOD and GPIOE are not bonded out (DS14463, §4.1).
//
// These partial specialisations therefore override the primary false_type defaults
// only for PortA, PortB, PortC and PortF.  Capability is reported as true only for
// valid pin numbers (0–15), which matches the range accepted by GpioPortPinImpl.
// Out-of-range pin numbers correctly report false so that generic code relying on
// these traits is not misled.

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
template <uint8_t PinNum>
struct supports_output_type<PortA, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_type<PortB, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_type<PortC, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_type<PortF, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortA, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortB, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortC, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortF, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortA, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortB, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortC, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortF, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortA, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortB, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortC, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortF, PinNum> : detail::Stm32u083PortCapability<PinNum> {};
// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio::capabilities

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_CAPABILITIES_HPP
