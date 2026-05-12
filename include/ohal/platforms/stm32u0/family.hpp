#ifndef OHAL_PLATFORMS_STM32U0_FAMILY_HPP
#define OHAL_PLATFORMS_STM32U0_FAMILY_HPP

// Recognised STM32U0 models. Add a new clause to the conflict check and a
// corresponding #include block below when a new model is implemented (see
// docs/contributor/adding-mcu.md).

#if (defined(OHAL_MODEL_STM32U083KCU) + defined(OHAL_MODEL_STM32U083HCY) +                         \
     defined(OHAL_MODEL_STM32U083CCU) + defined(OHAL_MODEL_STM32U083CCT) +                         \
     defined(OHAL_MODEL_STM32U083RCT) + defined(OHAL_MODEL_STM32U083RCI) +                         \
     defined(OHAL_MODEL_STM32U083MCT) + defined(OHAL_MODEL_STM32U083MCI) +                         \
     defined(OHAL_MODEL_STM32U073) + defined(OHAL_MODEL_STM32U031R6T) +                            \
     defined(OHAL_MODEL_STM32U031R6I) + defined(OHAL_MODEL_STM32U031R8T) +                         \
     defined(OHAL_MODEL_STM32U031R8I) + defined(OHAL_MODEL_STM32U031C6U) +                         \
     defined(OHAL_MODEL_STM32U031C6T) + defined(OHAL_MODEL_STM32U031C8U) +                         \
     defined(OHAL_MODEL_STM32U031C8T) + defined(OHAL_MODEL_STM32U031K4U) +                         \
     defined(OHAL_MODEL_STM32U031K6U) + defined(OHAL_MODEL_STM32U031K8U) +                         \
     defined(OHAL_MODEL_STM32U031G6Y) + defined(OHAL_MODEL_STM32U031G8Y) +                         \
     defined(OHAL_MODEL_STM32U031F4P) + defined(OHAL_MODEL_STM32U031F6P) +                         \
     defined(OHAL_MODEL_STM32U031F8P)) > 1
#error "ohal: Multiple STM32U0 models defined. " \
       "Pass exactly one OHAL_MODEL_* macro to the compiler."
#elif !defined(OHAL_MODEL_STM32U083KCU) && !defined(OHAL_MODEL_STM32U083HCY) &&                    \
    !defined(OHAL_MODEL_STM32U083CCU) && !defined(OHAL_MODEL_STM32U083CCT) &&                      \
    !defined(OHAL_MODEL_STM32U083RCT) && !defined(OHAL_MODEL_STM32U083RCI) &&                      \
    !defined(OHAL_MODEL_STM32U083MCT) && !defined(OHAL_MODEL_STM32U083MCI) &&                      \
    !defined(OHAL_MODEL_STM32U073) && !defined(OHAL_MODEL_STM32U031R6T) &&                         \
    !defined(OHAL_MODEL_STM32U031R6I) && !defined(OHAL_MODEL_STM32U031R8T) &&                      \
    !defined(OHAL_MODEL_STM32U031R8I) && !defined(OHAL_MODEL_STM32U031C6U) &&                      \
    !defined(OHAL_MODEL_STM32U031C6T) && !defined(OHAL_MODEL_STM32U031C8U) &&                      \
    !defined(OHAL_MODEL_STM32U031C8T) && !defined(OHAL_MODEL_STM32U031K4U) &&                      \
    !defined(OHAL_MODEL_STM32U031K6U) && !defined(OHAL_MODEL_STM32U031K8U) &&                      \
    !defined(OHAL_MODEL_STM32U031G6Y) && !defined(OHAL_MODEL_STM32U031G8Y) &&                      \
    !defined(OHAL_MODEL_STM32U031F4P) && !defined(OHAL_MODEL_STM32U031F6P) &&                      \
    !defined(OHAL_MODEL_STM32U031F8P)
#error "ohal: No STM32U0 model defined. "                                                           \
       "Pass -DOHAL_MODEL_STM32U083KCU, -DOHAL_MODEL_STM32U083HCY, "                               \
       "-DOHAL_MODEL_STM32U083CCU, -DOHAL_MODEL_STM32U083CCT, "                                    \
       "-DOHAL_MODEL_STM32U083RCT, -DOHAL_MODEL_STM32U083RCI, "                                    \
       "-DOHAL_MODEL_STM32U083MCT, -DOHAL_MODEL_STM32U083MCI, "                                    \
       "-DOHAL_MODEL_STM32U031R8T, -DOHAL_MODEL_STM32U031R8I, "                                    \
       "-DOHAL_MODEL_STM32U031R6T, -DOHAL_MODEL_STM32U031R6I, "                                    \
       "-DOHAL_MODEL_STM32U031C8U, -DOHAL_MODEL_STM32U031C8T, "                                    \
       "-DOHAL_MODEL_STM32U031C6U, -DOHAL_MODEL_STM32U031C6T, "                                    \
       "-DOHAL_MODEL_STM32U031K8U, -DOHAL_MODEL_STM32U031K6U, -DOHAL_MODEL_STM32U031K4U, "         \
       "-DOHAL_MODEL_STM32U031G8Y, -DOHAL_MODEL_STM32U031G6Y, "                                    \
       "-DOHAL_MODEL_STM32U031F8P, -DOHAL_MODEL_STM32U031F6P, -DOHAL_MODEL_STM32U031F4P, "         \
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
#elif defined(OHAL_MODEL_STM32U031R8T)
#include "ohal/platforms/stm32u0/models/stm32u031r8t/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031r8t/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031r8t/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031r8t/uart.hpp"
#elif defined(OHAL_MODEL_STM32U031R8I)
#include "ohal/platforms/stm32u0/models/stm32u031r8i/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031r8i/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031r8i/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031r8i/uart.hpp"
#elif defined(OHAL_MODEL_STM32U031R6T)
#include "ohal/platforms/stm32u0/models/stm32u031r6t/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031r6t/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031r6t/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031r6t/uart.hpp"
#elif defined(OHAL_MODEL_STM32U031R6I)
#include "ohal/platforms/stm32u0/models/stm32u031r6i/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031r6i/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031r6i/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031r6i/uart.hpp"
#elif defined(OHAL_MODEL_STM32U031C8U)
#include "ohal/platforms/stm32u0/models/stm32u031c8u/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031c8u/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031c8u/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031c8u/uart.hpp"
#elif defined(OHAL_MODEL_STM32U031C8T)
#include "ohal/platforms/stm32u0/models/stm32u031c8t/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031c8t/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031c8t/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031c8t/uart.hpp"
#elif defined(OHAL_MODEL_STM32U031C6U)
#include "ohal/platforms/stm32u0/models/stm32u031c6u/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031c6u/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031c6u/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031c6u/uart.hpp"
#elif defined(OHAL_MODEL_STM32U031C6T)
#include "ohal/platforms/stm32u0/models/stm32u031c6t/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031c6t/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031c6t/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031c6t/uart.hpp"
#elif defined(OHAL_MODEL_STM32U031K8U)
#include "ohal/platforms/stm32u0/models/stm32u031k8u/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031k8u/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031k8u/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031k8u/uart.hpp"
#elif defined(OHAL_MODEL_STM32U031K6U)
#include "ohal/platforms/stm32u0/models/stm32u031k6u/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031k6u/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031k6u/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031k6u/uart.hpp"
#elif defined(OHAL_MODEL_STM32U031K4U)
#include "ohal/platforms/stm32u0/models/stm32u031k4u/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031k4u/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031k4u/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031k4u/uart.hpp"
#elif defined(OHAL_MODEL_STM32U031G8Y)
#include "ohal/platforms/stm32u0/models/stm32u031g8y/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031g8y/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031g8y/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031g8y/uart.hpp"
#elif defined(OHAL_MODEL_STM32U031G6Y)
#include "ohal/platforms/stm32u0/models/stm32u031g6y/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031g6y/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031g6y/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031g6y/uart.hpp"
#elif defined(OHAL_MODEL_STM32U031F8P)
#include "ohal/platforms/stm32u0/models/stm32u031f8p/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031f8p/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031f8p/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031f8p/uart.hpp"
#elif defined(OHAL_MODEL_STM32U031F6P)
#include "ohal/platforms/stm32u0/models/stm32u031f6p/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031f6p/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031f6p/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031f6p/uart.hpp"
#elif defined(OHAL_MODEL_STM32U031F4P)
#include "ohal/platforms/stm32u0/models/stm32u031f4p/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031f4p/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031f4p/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031f4p/uart.hpp"
#endif

#endif // OHAL_PLATFORMS_STM32U0_FAMILY_HPP
