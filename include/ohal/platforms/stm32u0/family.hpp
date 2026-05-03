#ifndef OHAL_PLATFORMS_STM32U0_FAMILY_HPP
#define OHAL_PLATFORMS_STM32U0_FAMILY_HPP

// Recognised STM32U0 models. Add a new clause to the conflict check and a
// corresponding #include block below when a new model is implemented (see
// docs/contributor/adding-mcu.md).

#if (defined(OHAL_MODEL_STM32U083KCU) + defined(OHAL_MODEL_STM32U083HCY) +                         \
     defined(OHAL_MODEL_STM32U083CCI) + defined(OHAL_MODEL_STM32U083CCT) +                         \
     defined(OHAL_MODEL_STM32U073)) > 1
#error "ohal: Multiple STM32U0 models defined. " \
       "Pass exactly one OHAL_MODEL_* macro to the compiler."
#elif !defined(OHAL_MODEL_STM32U083KCU) && !defined(OHAL_MODEL_STM32U083HCY) &&                    \
    !defined(OHAL_MODEL_STM32U083CCI) && !defined(OHAL_MODEL_STM32U083CCT) &&                      \
    !defined(OHAL_MODEL_STM32U073)
#error "ohal: No STM32U0 model defined. " \
       "Pass -DOHAL_MODEL_STM32U083KCU, -DOHAL_MODEL_STM32U083HCY, -DOHAL_MODEL_STM32U083CCI, " \
       "-DOHAL_MODEL_STM32U083CCT (or another U0 model) to the compiler."
#endif

#if defined(OHAL_MODEL_STM32U083KCU)
#include "ohal/platforms/stm32u0/models/stm32u083kcu/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083kcu/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083kcu/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083kcu/uart.hpp"
#elif defined(OHAL_MODEL_STM32U083HCY)
#include "ohal/platforms/stm32u0/models/stm32u083hcy/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083hcy/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083hcy/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083hcy/uart.hpp"
#elif defined(OHAL_MODEL_STM32U083CCI)
#include "ohal/platforms/stm32u0/models/stm32u083cci/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083cci/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083cci/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083cci/uart.hpp"
#elif defined(OHAL_MODEL_STM32U083CCT)
#include "ohal/platforms/stm32u0/models/stm32u083cct/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083cct/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083cct/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083cct/uart.hpp"
#elif defined(OHAL_MODEL_STM32U073)
// STM32U073 register maps and capabilities are not yet implemented.
// Remove this #error and add a models/stm32u073/ directory when implementing
// that model (Step 15).
#error "ohal: OHAL_MODEL_STM32U073 is not yet implemented. " \
       "Support for this model will be added in a future step."
#endif

#endif // OHAL_PLATFORMS_STM32U0_FAMILY_HPP
