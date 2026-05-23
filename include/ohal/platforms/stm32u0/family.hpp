#ifndef OHAL_PLATFORMS_STM32U0_FAMILY_HPP
#define OHAL_PLATFORMS_STM32U0_FAMILY_HPP

// Recognised STM32U0 models. Add a new clause to the conflict check and a
// corresponding #include block below when a new model is implemented (see
// docs/contributor/adding-mcu.md).

#if (defined(OHAL_MODEL_STM32U083KCU) + defined(OHAL_MODEL_STM32U083HCY) +                         \
     defined(OHAL_MODEL_STM32U083CCU) + defined(OHAL_MODEL_STM32U083CCT) +                         \
     defined(OHAL_MODEL_STM32U083RCT) + defined(OHAL_MODEL_STM32U083RCI) +                         \
     defined(OHAL_MODEL_STM32U083MCT) + defined(OHAL_MODEL_STM32U083MCI) +                         \
     defined(OHAL_MODEL_STM32U073K8U) + defined(OHAL_MODEL_STM32U073KBU) +                         \
     defined(OHAL_MODEL_STM32U073KCU) + defined(OHAL_MODEL_STM32U073C8T) +                         \
     defined(OHAL_MODEL_STM32U073C8U) + defined(OHAL_MODEL_STM32U073CBT) +                         \
     defined(OHAL_MODEL_STM32U073CBU) + defined(OHAL_MODEL_STM32U073CCT) +                         \
     defined(OHAL_MODEL_STM32U073CCU) + defined(OHAL_MODEL_STM32U073H8Y) +                         \
     defined(OHAL_MODEL_STM32U073HBY) + defined(OHAL_MODEL_STM32U073HCY) +                         \
     defined(OHAL_MODEL_STM32U073R8T) + defined(OHAL_MODEL_STM32U073R8I) +                         \
     defined(OHAL_MODEL_STM32U073RBT) + defined(OHAL_MODEL_STM32U073RBI) +                         \
     defined(OHAL_MODEL_STM32U073RCT) + defined(OHAL_MODEL_STM32U073RCI) +                         \
     defined(OHAL_MODEL_STM32U073M8T) + defined(OHAL_MODEL_STM32U073M8I) +                         \
     defined(OHAL_MODEL_STM32U073MBT) + defined(OHAL_MODEL_STM32U073MBI) +                         \
     defined(OHAL_MODEL_STM32U073MCT) + defined(OHAL_MODEL_STM32U073MCI) +                         \
     defined(OHAL_MODEL_STM32U031R6T) + defined(OHAL_MODEL_STM32U031R6I) +                         \
     defined(OHAL_MODEL_STM32U031R8T) + defined(OHAL_MODEL_STM32U031R8I) +                         \
     defined(OHAL_MODEL_STM32U031C6U) + defined(OHAL_MODEL_STM32U031C6T) +                         \
     defined(OHAL_MODEL_STM32U031C8U) + defined(OHAL_MODEL_STM32U031C8T) +                         \
     defined(OHAL_MODEL_STM32U031K4U) + defined(OHAL_MODEL_STM32U031K6U) +                         \
     defined(OHAL_MODEL_STM32U031K8U) + defined(OHAL_MODEL_STM32U031G6Y) +                         \
     defined(OHAL_MODEL_STM32U031G8Y) + defined(OHAL_MODEL_STM32U031F4P) +                         \
     defined(OHAL_MODEL_STM32U031F6P) + defined(OHAL_MODEL_STM32U031F8P)) > 1
#error "ohal: Multiple STM32U0 models defined. " \
       "Pass exactly one OHAL_MODEL_* macro to the compiler."
#elif !defined(OHAL_MODEL_STM32U083KCU) && !defined(OHAL_MODEL_STM32U083HCY) &&                    \
    !defined(OHAL_MODEL_STM32U083CCU) && !defined(OHAL_MODEL_STM32U083CCT) &&                      \
    !defined(OHAL_MODEL_STM32U083RCT) && !defined(OHAL_MODEL_STM32U083RCI) &&                      \
    !defined(OHAL_MODEL_STM32U083MCT) && !defined(OHAL_MODEL_STM32U083MCI) &&                      \
    !defined(OHAL_MODEL_STM32U073K8U) && !defined(OHAL_MODEL_STM32U073KBU) &&                      \
    !defined(OHAL_MODEL_STM32U073KCU) && !defined(OHAL_MODEL_STM32U073C8T) &&                      \
    !defined(OHAL_MODEL_STM32U073C8U) && !defined(OHAL_MODEL_STM32U073CBT) &&                      \
    !defined(OHAL_MODEL_STM32U073CBU) && !defined(OHAL_MODEL_STM32U073CCT) &&                      \
    !defined(OHAL_MODEL_STM32U073CCU) && !defined(OHAL_MODEL_STM32U073H8Y) &&                      \
    !defined(OHAL_MODEL_STM32U073HBY) && !defined(OHAL_MODEL_STM32U073HCY) &&                      \
    !defined(OHAL_MODEL_STM32U073R8T) && !defined(OHAL_MODEL_STM32U073R8I) &&                      \
    !defined(OHAL_MODEL_STM32U073RBT) && !defined(OHAL_MODEL_STM32U073RBI) &&                      \
    !defined(OHAL_MODEL_STM32U073RCT) && !defined(OHAL_MODEL_STM32U073RCI) &&                      \
    !defined(OHAL_MODEL_STM32U073M8T) && !defined(OHAL_MODEL_STM32U073M8I) &&                      \
    !defined(OHAL_MODEL_STM32U073MBT) && !defined(OHAL_MODEL_STM32U073MBI) &&                      \
    !defined(OHAL_MODEL_STM32U073MCT) && !defined(OHAL_MODEL_STM32U073MCI) &&                      \
    !defined(OHAL_MODEL_STM32U031R6T) && !defined(OHAL_MODEL_STM32U031R6I) &&                      \
    !defined(OHAL_MODEL_STM32U031R8T) && !defined(OHAL_MODEL_STM32U031R8I) &&                      \
    !defined(OHAL_MODEL_STM32U031C6U) && !defined(OHAL_MODEL_STM32U031C6T) &&                      \
    !defined(OHAL_MODEL_STM32U031C8U) && !defined(OHAL_MODEL_STM32U031C8T) &&                      \
    !defined(OHAL_MODEL_STM32U031K4U) && !defined(OHAL_MODEL_STM32U031K6U) &&                      \
    !defined(OHAL_MODEL_STM32U031K8U) && !defined(OHAL_MODEL_STM32U031G6Y) &&                      \
    !defined(OHAL_MODEL_STM32U031G8Y) && !defined(OHAL_MODEL_STM32U031F4P) &&                      \
    !defined(OHAL_MODEL_STM32U031F6P) && !defined(OHAL_MODEL_STM32U031F8P)
#error "ohal: No STM32U0 model defined. "                                                           \
       "Pass -DOHAL_MODEL_STM32U083KCU, -DOHAL_MODEL_STM32U083HCY, "                               \
       "-DOHAL_MODEL_STM32U083CCU, -DOHAL_MODEL_STM32U083CCT, "                                    \
       "-DOHAL_MODEL_STM32U083RCT, -DOHAL_MODEL_STM32U083RCI, "                                    \
       "-DOHAL_MODEL_STM32U083MCT, -DOHAL_MODEL_STM32U083MCI, "                                    \
       "-DOHAL_MODEL_STM32U073KCU, -DOHAL_MODEL_STM32U073KBU, -DOHAL_MODEL_STM32U073K8U, "         \
       "-DOHAL_MODEL_STM32U073CCT, -DOHAL_MODEL_STM32U073CCU, "                                    \
       "-DOHAL_MODEL_STM32U073CBT, -DOHAL_MODEL_STM32U073CBU, "                                    \
       "-DOHAL_MODEL_STM32U073C8T, -DOHAL_MODEL_STM32U073C8U, "                                    \
       "-DOHAL_MODEL_STM32U073HCY, -DOHAL_MODEL_STM32U073HBY, -DOHAL_MODEL_STM32U073H8Y, "         \
       "-DOHAL_MODEL_STM32U073RCT, -DOHAL_MODEL_STM32U073RCI, "                                    \
       "-DOHAL_MODEL_STM32U073RBT, -DOHAL_MODEL_STM32U073RBI, "                                    \
       "-DOHAL_MODEL_STM32U073R8T, -DOHAL_MODEL_STM32U073R8I, "                                    \
       "-DOHAL_MODEL_STM32U073MCT, -DOHAL_MODEL_STM32U073MCI, "                                    \
       "-DOHAL_MODEL_STM32U073MBT, -DOHAL_MODEL_STM32U073MBI, "                                    \
       "-DOHAL_MODEL_STM32U073M8T, -DOHAL_MODEL_STM32U073M8I, "                                    \
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
// STM32U083 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u083kcu/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083kcu/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083kcu/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083kcu/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083kcu/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U083HCY)
// STM32U083 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u083hcy/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083hcy/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083hcy/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083hcy/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083hcy/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U083CCU)
// STM32U083 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u083ccu/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083ccu/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083ccu/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083ccu/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083ccu/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U083CCT)
// STM32U083 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u083cct/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083cct/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083cct/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083cct/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083cct/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U083RCT)
// STM32U083 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u083rct/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083rct/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083rct/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083rct/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083rct/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U083RCI)
// STM32U083 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u083rci/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083rci/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083rci/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083rci/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083rci/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U083MCT)
// STM32U083 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u083mct/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083mct/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083mct/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083mct/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083mct/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U083MCI)
// STM32U083 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u083mci/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083mci/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083mci/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083mci/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083mci/uart_capabilities.hpp"
// ---------------------------------------------------------------------------
// STM32U073 — 24 package variants.  All have LPUART3 and LPTIM3.
// ---------------------------------------------------------------------------
#elif defined(OHAL_MODEL_STM32U073KCU)
// STM32U073 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u073kcu/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073kcu/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073kcu/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073kcu/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073kcu/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U073KBU)
// STM32U073 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u073kbu/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073kbu/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073kbu/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073kbu/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073kbu/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U073K8U)
// STM32U073 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u073k8u/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073k8u/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073k8u/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073k8u/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073k8u/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U073CCT)
// STM32U073 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u073cct/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073cct/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073cct/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073cct/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073cct/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U073CCU)
// STM32U073 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u073ccu/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073ccu/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073ccu/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073ccu/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073ccu/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U073CBT)
// STM32U073 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u073cbt/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073cbt/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073cbt/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073cbt/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073cbt/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U073CBU)
// STM32U073 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u073cbu/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073cbu/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073cbu/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073cbu/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073cbu/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U073C8T)
// STM32U073 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u073c8t/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073c8t/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073c8t/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073c8t/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073c8t/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U073C8U)
// STM32U073 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u073c8u/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073c8u/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073c8u/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073c8u/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073c8u/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U073HCY)
// STM32U073 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u073hcy/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073hcy/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073hcy/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073hcy/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073hcy/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U073HBY)
// STM32U073 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u073hby/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073hby/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073hby/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073hby/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073hby/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U073H8Y)
// STM32U073 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u073h8y/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073h8y/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073h8y/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073h8y/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073h8y/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U073RCT)
// STM32U073 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u073rct/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073rct/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073rct/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073rct/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073rct/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U073RCI)
// STM32U073 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u073rci/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073rci/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073rci/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073rci/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073rci/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U073RBT)
// STM32U073 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u073rbt/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073rbt/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073rbt/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073rbt/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073rbt/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U073RBI)
// STM32U073 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u073rbi/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073rbi/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073rbi/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073rbi/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073rbi/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U073R8T)
// STM32U073 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u073r8t/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073r8t/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073r8t/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073r8t/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073r8t/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U073R8I)
// STM32U073 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u073r8i/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073r8i/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073r8i/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073r8i/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073r8i/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U073MCT)
// STM32U073 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u073mct/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073mct/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073mct/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073mct/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073mct/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U073MCI)
// STM32U073 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u073mci/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073mci/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073mci/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073mci/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073mci/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U073MBT)
// STM32U073 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u073mbt/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073mbt/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073mbt/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073mbt/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073mbt/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U073MBI)
// STM32U073 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u073mbi/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073mbi/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073mbi/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073mbi/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073mbi/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U073M8T)
// STM32U073 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u073m8t/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073m8t/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073m8t/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073m8t/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073m8t/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U073M8I)
// STM32U073 includes LPUART3 and LPTIM3.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif
#ifndef OHAL_STM32U0_ENABLE_LPTIM3
#define OHAL_STM32U0_ENABLE_LPTIM3
#endif
#include "ohal/platforms/stm32u0/models/stm32u073m8i/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073m8i/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073m8i/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073m8i/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u073m8i/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U031R8T)
#include "ohal/platforms/stm32u0/models/stm32u031r8t/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031r8t/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031r8t/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031r8t/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031r8t/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U031R8I)
#include "ohal/platforms/stm32u0/models/stm32u031r8i/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031r8i/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031r8i/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031r8i/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031r8i/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U031R6T)
#include "ohal/platforms/stm32u0/models/stm32u031r6t/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031r6t/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031r6t/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031r6t/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031r6t/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U031R6I)
#include "ohal/platforms/stm32u0/models/stm32u031r6i/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031r6i/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031r6i/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031r6i/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031r6i/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U031C8U)
#include "ohal/platforms/stm32u0/models/stm32u031c8u/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031c8u/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031c8u/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031c8u/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031c8u/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U031C8T)
#include "ohal/platforms/stm32u0/models/stm32u031c8t/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031c8t/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031c8t/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031c8t/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031c8t/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U031C6U)
#include "ohal/platforms/stm32u0/models/stm32u031c6u/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031c6u/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031c6u/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031c6u/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031c6u/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U031C6T)
#include "ohal/platforms/stm32u0/models/stm32u031c6t/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031c6t/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031c6t/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031c6t/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031c6t/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U031K8U)
#include "ohal/platforms/stm32u0/models/stm32u031k8u/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031k8u/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031k8u/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031k8u/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031k8u/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U031K6U)
#include "ohal/platforms/stm32u0/models/stm32u031k6u/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031k6u/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031k6u/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031k6u/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031k6u/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U031K4U)
#include "ohal/platforms/stm32u0/models/stm32u031k4u/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031k4u/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031k4u/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031k4u/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031k4u/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U031G8Y)
#include "ohal/platforms/stm32u0/models/stm32u031g8y/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031g8y/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031g8y/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031g8y/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031g8y/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U031G6Y)
#include "ohal/platforms/stm32u0/models/stm32u031g6y/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031g6y/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031g6y/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031g6y/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031g6y/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U031F8P)
#include "ohal/platforms/stm32u0/models/stm32u031f8p/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031f8p/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031f8p/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031f8p/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031f8p/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U031F6P)
#include "ohal/platforms/stm32u0/models/stm32u031f6p/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031f6p/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031f6p/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031f6p/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031f6p/uart_capabilities.hpp"
#elif defined(OHAL_MODEL_STM32U031F4P)
#include "ohal/platforms/stm32u0/models/stm32u031f4p/capabilities.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031f4p/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031f4p/timer.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031f4p/uart.hpp"
#include "ohal/platforms/stm32u0/models/stm32u031f4p/uart_capabilities.hpp"
#endif

#include "ohal/platforms/stm32u0/models/stm32u083/exti.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083/flash.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083/irq.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083/irq_numbers.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083/nvic.hpp"

#endif // OHAL_PLATFORMS_STM32U0_FAMILY_HPP
