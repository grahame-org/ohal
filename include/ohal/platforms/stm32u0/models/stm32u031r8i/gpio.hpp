#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U031R8I_GPIO_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U031R8I_GPIO_HPP

// GPIO Pin<>/Port<> specialisations for the STM32U031R8I (UFBGA64).
// The STM32U031 R8I package shares its GPIO bonding with the other 64-pin
// STM32U031 R-series packages (LQFP64/UFBGA64):
//   GPIOA (bits 0-15), GPIOB (bits 0-15), GPIOC (bits 0-15),
//   GPIOD (PD2 only), GPIOF (PF0-PF3 only).
// GPIOE is not bonded out.
// These specialisations are identical to the STM32U083RC packages (DS14463/DS14581, §4.1).

#include "ohal/platforms/stm32u0/models/stm32u083rc/gpio.hpp"

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U031R8I_GPIO_HPP
