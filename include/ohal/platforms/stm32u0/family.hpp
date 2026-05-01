#ifndef OHAL_PLATFORMS_STM32U0_FAMILY_HPP
#define OHAL_PLATFORMS_STM32U0_FAMILY_HPP

#if !defined(OHAL_MODEL_STM32U083) && !defined(OHAL_MODEL_STM32U073)
#error "ohal: No STM32U0 model defined. " \
       "Pass -DOHAL_MODEL_STM32U083 (or another U0 model) to the compiler."
#endif

#if defined(OHAL_MODEL_STM32U083)
#include "ohal/platforms/stm32u0/models/stm32u083/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083/uart.hpp"
#endif

#endif // OHAL_PLATFORMS_STM32U0_FAMILY_HPP
