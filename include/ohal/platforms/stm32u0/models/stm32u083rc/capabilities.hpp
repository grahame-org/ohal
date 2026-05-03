#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083RC_CAPABILITIES_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083RC_CAPABILITIES_HPP

// Capability traits for the STM32U083RC 64-pin packages (LQFP64 and UFBGA64).
//
// Both RC packages expose GPIOA, GPIOB, GPIOC, GPIOD and GPIOF; GPIOE is not
// bonded out (DS14463, §4.1).  The A/B/C/F traits are inherited from the
// shared stm32u083 header.  This header adds the PortD specialisations.

#include "ohal/platforms/stm32u0/models/stm32u083/capabilities.hpp"

namespace ohal::gpio::capabilities {

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
template <uint8_t PinNum>
struct supports_output_type<PortD, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortD, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortD, PinNum> : detail::Stm32u083PortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortD, PinNum> : detail::Stm32u083PortCapability<PinNum> {};
// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio::capabilities

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083RC_CAPABILITIES_HPP
