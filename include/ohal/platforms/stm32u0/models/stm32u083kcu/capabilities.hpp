#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083KCU_CAPABILITIES_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083KCU_CAPABILITIES_HPP

// The STM32U083KCU (32-pin UFQFPN) bonds out GPIOA, GPIOB, GPIOC and GPIOF;
// GPIOD and GPIOE are present in silicon but not bonded out on this package
// (DS14463, §4.1).  All capability trait specialisations are defined in the
// shared stm32u083 header.

#include "ohal/platforms/stm32u0/models/stm32u083/capabilities.hpp"

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083KCU_CAPABILITIES_HPP
