#ifndef OHAL_CORE_CAPABILITIES_HPP
#define OHAL_CORE_CAPABILITIES_HPP

#include <cstdint>
#include <type_traits>

namespace ohal::gpio::capabilities {

/// Primary template: GPIO output-type configuration (push-pull / open-drain).
/// Platform headers specialise this to std::true_type for every Port/PinNum pair
/// that supports the feature. The default is false so that unimplemented platforms
/// fail noisily at compile time.
template <typename Port, uint8_t PinNum>
struct supports_output_type : std::false_type {};

/// Primary template: GPIO output-speed configuration.
template <typename Port, uint8_t PinNum>
struct supports_output_speed : std::false_type {};

/// Primary template: GPIO pull-up / pull-down configuration.
template <typename Port, uint8_t PinNum>
struct supports_pull : std::false_type {};

/// Primary template: GPIO alternate-function selection.
template <typename Port, uint8_t PinNum>
struct supports_alternate_function : std::false_type {};

} // namespace ohal::gpio::capabilities

#endif // OHAL_CORE_CAPABILITIES_HPP
