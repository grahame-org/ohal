#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_CAPABILITIES_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_CAPABILITIES_HPP

#include <type_traits>

#include "ohal/core/capabilities.hpp"
#include "ohal/gpio.hpp"

namespace ohal::gpio::capabilities {

// All STM32U083 GPIO pins (on every port) support output-type configuration,
// output-speed configuration, pull-up/pull-down configuration, and alternate-function
// selection.  These partial specialisations override the primary false_type defaults
// for any (Port, PinNum) pair that is a valid STM32U083 port tag.

template <uint8_t PinNum>
struct supports_output_type<PortA, PinNum> : std::true_type {};

template <uint8_t PinNum>
struct supports_output_type<PortB, PinNum> : std::true_type {};

template <uint8_t PinNum>
struct supports_output_type<PortC, PinNum> : std::true_type {};

template <uint8_t PinNum>
struct supports_output_type<PortD, PinNum> : std::true_type {};

template <uint8_t PinNum>
struct supports_output_type<PortE, PinNum> : std::true_type {};

template <uint8_t PinNum>
struct supports_output_type<PortF, PinNum> : std::true_type {};

template <uint8_t PinNum>
struct supports_output_speed<PortA, PinNum> : std::true_type {};

template <uint8_t PinNum>
struct supports_output_speed<PortB, PinNum> : std::true_type {};

template <uint8_t PinNum>
struct supports_output_speed<PortC, PinNum> : std::true_type {};

template <uint8_t PinNum>
struct supports_output_speed<PortD, PinNum> : std::true_type {};

template <uint8_t PinNum>
struct supports_output_speed<PortE, PinNum> : std::true_type {};

template <uint8_t PinNum>
struct supports_output_speed<PortF, PinNum> : std::true_type {};

template <uint8_t PinNum>
struct supports_pull<PortA, PinNum> : std::true_type {};

template <uint8_t PinNum>
struct supports_pull<PortB, PinNum> : std::true_type {};

template <uint8_t PinNum>
struct supports_pull<PortC, PinNum> : std::true_type {};

template <uint8_t PinNum>
struct supports_pull<PortD, PinNum> : std::true_type {};

template <uint8_t PinNum>
struct supports_pull<PortE, PinNum> : std::true_type {};

template <uint8_t PinNum>
struct supports_pull<PortF, PinNum> : std::true_type {};

template <uint8_t PinNum>
struct supports_alternate_function<PortA, PinNum> : std::true_type {};

template <uint8_t PinNum>
struct supports_alternate_function<PortB, PinNum> : std::true_type {};

template <uint8_t PinNum>
struct supports_alternate_function<PortC, PinNum> : std::true_type {};

template <uint8_t PinNum>
struct supports_alternate_function<PortD, PinNum> : std::true_type {};

template <uint8_t PinNum>
struct supports_alternate_function<PortE, PinNum> : std::true_type {};

template <uint8_t PinNum>
struct supports_alternate_function<PortF, PinNum> : std::true_type {};

} // namespace ohal::gpio::capabilities

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_CAPABILITIES_HPP
