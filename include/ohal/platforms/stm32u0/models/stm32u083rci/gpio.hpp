#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083RCI_GPIO_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083RCI_GPIO_HPP

// The STM32U083RCI (64-pin UFBGA) shares the same GPIO register map and
// bonded-out port set (GPIOA, GPIOB, GPIOC, GPIOD, GPIOF) as the
// STM32U083RCT (64-pin LQFP).  All GpioPortRegs, GpioPortPinImpl,
// GpioPortImpl types and ohal::gpio::Pin<>/Port<> specialisations from the
// shared stm32u083rc header apply unchanged.

#include "ohal/platforms/stm32u0/models/stm32u083rc/gpio.hpp"

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083RCI_GPIO_HPP
