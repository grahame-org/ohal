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

// All STM32U083 GPIO pins (on every port) support output-type configuration,
// output-speed configuration, pull-up/pull-down configuration, and alternate-function
// selection.  These partial specialisations override the primary false_type defaults
// for any (Port, PinNum) pair that is a valid STM32U083 port tag.
//
// Capability is reported as true only for valid pin numbers (0-15), which matches
// the range accepted by GpioPortPinImpl.  Out-of-range pin numbers correctly report
// false so that generic code relying on these traits is not misled.

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
template <uint8_t PinNum>
struct supports_output_type<PortA, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_type<PortB, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_type<PortC, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_type<PortD, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_type<PortE, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_type<PortF, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortA, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortB, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortC, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortD, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortE, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortF, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortA, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortB, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortC, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortD, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortE, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortF, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortA, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortB, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortC, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortD, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortE, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortF, PinNum> : detail::Stm32u083PortCapability<PinNum> {};
// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio::capabilities

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_CAPABILITIES_HPP
