#ifndef OHAL_PLATFORMS_STM32U0_FAMILY_HPP
#define OHAL_PLATFORMS_STM32U0_FAMILY_HPP

// Recognised STM32U0 models. Add a new clause to the conflict check and a
// corresponding #include block below when a new model is implemented (see
// docs/contributor/adding-mcu.md).

#if (defined(OHAL_MODEL_STM32U083KCU) + defined(OHAL_MODEL_STM32U083HCY) +                         \
     defined(OHAL_MODEL_STM32U083CCU) + defined(OHAL_MODEL_STM32U083CCT) +                         \
     defined(OHAL_MODEL_STM32U083RCT) + defined(OHAL_MODEL_STM32U083RCI) +                         \
     defined(OHAL_MODEL_STM32U083MCT) + defined(OHAL_MODEL_STM32U083MCI) +                         \
     defined(OHAL_MODEL_STM32U073)) > 1
#error "ohal: Multiple STM32U0 models defined. " \
       "Pass exactly one OHAL_MODEL_* macro to the compiler."
#elif !defined(OHAL_MODEL_STM32U083KCU) && !defined(OHAL_MODEL_STM32U083HCY) &&                    \
    !defined(OHAL_MODEL_STM32U083CCU) && !defined(OHAL_MODEL_STM32U083CCT) &&                      \
    !defined(OHAL_MODEL_STM32U083RCT) && !defined(OHAL_MODEL_STM32U083RCI) &&                      \
    !defined(OHAL_MODEL_STM32U083MCT) && !defined(OHAL_MODEL_STM32U083MCI) &&                      \
    !defined(OHAL_MODEL_STM32U073)
#error "ohal: No STM32U0 model defined. "                                                           \
       "Pass -DOHAL_MODEL_STM32U083KCU, -DOHAL_MODEL_STM32U083HCY, "                               \
       "-DOHAL_MODEL_STM32U083CCU, -DOHAL_MODEL_STM32U083CCT, "                                    \
       "-DOHAL_MODEL_STM32U083RCT, -DOHAL_MODEL_STM32U083RCI, "                                    \
       "-DOHAL_MODEL_STM32U083MCT, -DOHAL_MODEL_STM32U083MCI, "                                    \
       "(or another U0 model) to the compiler."
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
#elif defined(OHAL_MODEL_STM32U083CCU)
#include "ohal/platforms/stm32u0/models/stm32u083ccu/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083ccu/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083ccu/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083ccu/uart.hpp"
#elif defined(OHAL_MODEL_STM32U083CCT)
#include "ohal/platforms/stm32u0/models/stm32u083cct/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083cct/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083cct/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083cct/uart.hpp"
#elif defined(OHAL_MODEL_STM32U083RCT)
#include "ohal/platforms/stm32u0/models/stm32u083rct/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083rct/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083rct/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083rct/uart.hpp"
#elif defined(OHAL_MODEL_STM32U083RCI)
#include "ohal/platforms/stm32u0/models/stm32u083rci/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083rci/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083rci/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083rci/uart.hpp"
#elif defined(OHAL_MODEL_STM32U083MCT)
#include "ohal/platforms/stm32u0/models/stm32u083mct/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083mct/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083mct/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083mct/uart.hpp"
#elif defined(OHAL_MODEL_STM32U083MCI)
#include "ohal/platforms/stm32u0/models/stm32u083mci/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083mci/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083mci/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083mci/uart.hpp"
#elif defined(OHAL_MODEL_STM32U073)
// STM32U073 register maps and capabilities are not yet implemented.
// Remove this #error and add a models/stm32u073/ directory when implementing
// that model (Step 15).
#error "ohal: OHAL_MODEL_STM32U073 is not yet implemented. " \
       "Support for this model will be added in a future step."
#endif

#endif // OHAL_PLATFORMS_STM32U0_FAMILY_HPP
