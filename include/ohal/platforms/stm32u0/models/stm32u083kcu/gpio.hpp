#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083KCU_GPIO_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083KCU_GPIO_HPP

// The STM32U083KCU (32-pin UFQFPN) uses the GPIO register map defined in the
// shared stm32u083 header: identical base addresses, offsets, and bonded-out
// ports (GPIOA, GPIOB, GPIOC, GPIOF; GPIOD and GPIOE are not bonded out).
// All GpioPortRegs, GpioPortPinImpl, GpioPortImpl types and ohal::gpio::Pin<>
// / Port<> specialisations from the shared stm32u083 header apply unchanged.

#include "ohal/platforms/stm32u0/models/stm32u083/gpio.hpp"

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083KCU_GPIO_HPP
