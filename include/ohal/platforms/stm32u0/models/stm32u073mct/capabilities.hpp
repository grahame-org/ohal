#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U073MCT_CAPABILITIES_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U073MCT_CAPABILITIES_HPP

// The STM32U073MCT bonds out GPIOA, GPIOB, GPIOC, GPIOD (bits 0-6 and 8-13),
// GPIOE (PE7-PE9 only) and GPIOF (PF0-PF3); PE3 is not bonded on this package
// (DS14548, §4.1). These specialisations are identical to the STM32U083MCT.

#include "ohal/platforms/stm32u0/models/stm32u083mct/capabilities.hpp"

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U073MCT_CAPABILITIES_HPP
