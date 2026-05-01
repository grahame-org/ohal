#ifndef OHAL_PLATFORMS_STM32U0_FAMILY_HPP
#define OHAL_PLATFORMS_STM32U0_FAMILY_HPP

// Recognised STM32U0 models. Add a new !defined(...) clause and a corresponding
// #include block below when a new model is implemented (see docs/plan.md §7.2).
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

// OHAL_MODEL_STM32U073: register maps and capabilities not yet implemented.
// Add a models/stm32u073/ directory and a corresponding #include block here
// when implementing that model (Step 15).

#endif // OHAL_PLATFORMS_STM32U0_FAMILY_HPP
