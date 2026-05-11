#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U031C8U_GPIO_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U031C8U_GPIO_HPP

// GPIO Pin<>/Port<> specialisations for the STM32U031C8U (UFQFPN48).
// The STM32U031 C8U package shares its GPIO bonding with the other 48-pin
// STM32U031 C-series packages (UFQFPN48/LQFP48):
//   GPIOA (bits 0-15), GPIOB (bits 0-15),
//   GPIOC (bits 13-15 only), GPIOF (bits 0-3 only).
// GPIOD and GPIOE are not bonded out.
// These specialisations are identical to the STM32U083CC packages (DS14463/DS14581, §4.1).

#include "ohal/platforms/stm32u0/models/stm32u083cc/gpio.hpp"

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U031C8U_GPIO_HPP
