#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083CCI_GPIO_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083CCI_GPIO_HPP

// The STM32U083CCI (48-pin UFQFPN) shares the same GPIO register map as the
// STM32U083KCU (32-pin UFQFPN): identical base addresses, offsets, and bonded-
// out ports (GPIOA, GPIOB, GPIOC, GPIOF; GPIOD and GPIOE are not bonded out).
// All GpioPortRegs, GpioPortPinImpl, GpioPortImpl types and ohal::gpio::Pin<>
// / Port<> specialisations from the shared stm32u083 header apply unchanged.

#include "ohal/platforms/stm32u0/models/stm32u083/gpio.hpp"

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083CCI_GPIO_HPP
