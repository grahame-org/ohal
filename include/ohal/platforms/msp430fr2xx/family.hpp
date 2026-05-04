#ifndef OHAL_PLATFORMS_MSP430FR2XX_FAMILY_HPP
#define OHAL_PLATFORMS_MSP430FR2XX_FAMILY_HPP

// This header is included only when OHAL_FAMILY_MSP430FR2XX is defined
// (see ohal/platform.hpp).  The outer #ifdef prevents clang-tidy, which
// processes every header with -DOHAL_FAMILY_STM32U0, from triggering the
// model-not-defined #error.
#ifdef OHAL_FAMILY_MSP430FR2XX

// Recognised MSP430FR2xx models. Add a new clause to both guards and a corresponding
// #include block below when a new model is implemented (see docs/plan.md §7.2).
//
// When a second MSP430FR2xx model is added, replace the single-model guard below with
// the two-clause STM32U0 pattern:
//
//   #if defined(OHAL_MODEL_MSP430FR2355) && defined(OHAL_MODEL_<NEW>)
//   #error "ohal: Multiple MSP430FR2xx models defined. " \
//          "Pass exactly one OHAL_MODEL_* macro to the compiler."
//   #elif !defined(OHAL_MODEL_MSP430FR2355) && !defined(OHAL_MODEL_<NEW>)
//   #error "ohal: No MSP430FR2xx model defined. ..."
//   #endif

#if !defined(OHAL_MODEL_MSP430FR2355)
#error "ohal: No MSP430FR2xx model defined. " \
       "Pass -DOHAL_MODEL_MSP430FR2355 (or another MSP430FR2xx model) to the compiler."
#endif

#if defined(OHAL_MODEL_MSP430FR2355)
#include "ohal/platforms/msp430fr2xx/models/msp430fr2355/capabilities.hpp"
#include "ohal/platforms/msp430fr2xx/models/msp430fr2355/gpio.hpp"
#endif

#endif // OHAL_FAMILY_MSP430FR2XX

#endif // OHAL_PLATFORMS_MSP430FR2XX_FAMILY_HPP
