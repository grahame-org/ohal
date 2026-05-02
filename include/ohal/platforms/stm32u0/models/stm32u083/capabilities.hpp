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
struct supports_output_type<PortA, PinNum>
    : std::bool_constant<(PinNum < detail::kStm32u083PinCount)> {};

template <uint8_t PinNum>
struct supports_output_type<PortB, PinNum>
    : std::bool_constant<(PinNum < detail::kStm32u083PinCount)> {};

template <uint8_t PinNum>
struct supports_output_type<PortC, PinNum>
    : std::bool_constant<(PinNum < detail::kStm32u083PinCount)> {};

template <uint8_t PinNum>
struct supports_output_type<PortD, PinNum>
    : std::bool_constant<(PinNum < detail::kStm32u083PinCount)> {};

template <uint8_t PinNum>
struct supports_output_type<PortE, PinNum>
    : std::bool_constant<(PinNum < detail::kStm32u083PinCount)> {};

template <uint8_t PinNum>
struct supports_output_type<PortF, PinNum>
    : std::bool_constant<(PinNum < detail::kStm32u083PinCount)> {};

template <uint8_t PinNum>
struct supports_output_speed<PortA, PinNum>
    : std::bool_constant<(PinNum < detail::kStm32u083PinCount)> {};

template <uint8_t PinNum>
struct supports_output_speed<PortB, PinNum>
    : std::bool_constant<(PinNum < detail::kStm32u083PinCount)> {};

template <uint8_t PinNum>
struct supports_output_speed<PortC, PinNum>
    : std::bool_constant<(PinNum < detail::kStm32u083PinCount)> {};

template <uint8_t PinNum>
struct supports_output_speed<PortD, PinNum>
    : std::bool_constant<(PinNum < detail::kStm32u083PinCount)> {};

template <uint8_t PinNum>
struct supports_output_speed<PortE, PinNum>
    : std::bool_constant<(PinNum < detail::kStm32u083PinCount)> {};

template <uint8_t PinNum>
struct supports_output_speed<PortF, PinNum>
    : std::bool_constant<(PinNum < detail::kStm32u083PinCount)> {};

template <uint8_t PinNum>
struct supports_pull<PortA, PinNum> : std::bool_constant<(PinNum < detail::kStm32u083PinCount)> {};

template <uint8_t PinNum>
struct supports_pull<PortB, PinNum> : std::bool_constant<(PinNum < detail::kStm32u083PinCount)> {};

template <uint8_t PinNum>
struct supports_pull<PortC, PinNum> : std::bool_constant<(PinNum < detail::kStm32u083PinCount)> {};

template <uint8_t PinNum>
struct supports_pull<PortD, PinNum> : std::bool_constant<(PinNum < detail::kStm32u083PinCount)> {};

template <uint8_t PinNum>
struct supports_pull<PortE, PinNum> : std::bool_constant<(PinNum < detail::kStm32u083PinCount)> {};

template <uint8_t PinNum>
struct supports_pull<PortF, PinNum> : std::bool_constant<(PinNum < detail::kStm32u083PinCount)> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortA, PinNum>
    : std::bool_constant<(PinNum < detail::kStm32u083PinCount)> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortB, PinNum>
    : std::bool_constant<(PinNum < detail::kStm32u083PinCount)> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortC, PinNum>
    : std::bool_constant<(PinNum < detail::kStm32u083PinCount)> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortD, PinNum>
    : std::bool_constant<(PinNum < detail::kStm32u083PinCount)> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortE, PinNum>
    : std::bool_constant<(PinNum < detail::kStm32u083PinCount)> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortF, PinNum>
    : std::bool_constant<(PinNum < detail::kStm32u083PinCount)> {};
// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio::capabilities

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_CAPABILITIES_HPP
