#ifndef OHAL_PLATFORMS_MSP430FR2XX_FAMILY_HPP
#define OHAL_PLATFORMS_MSP430FR2XX_FAMILY_HPP

// This header is included only when OHAL_FAMILY_MSP430FR2XX is defined
// (see ohal/platform.hpp).  The outer #ifdef prevents clang-tidy, which
// processes every header with -DOHAL_FAMILY_STM32U0, from triggering the
// model-not-defined #error.
#ifdef OHAL_FAMILY_MSP430FR2XX

// Recognised MSP430FR2xx models/packages.  Add a new clause to both guards
// and a corresponding #include block below when a new model or package is
// implemented (see docs/plan.md §7.2).

#if (defined(OHAL_MODEL_MSP430FR2355) + defined(OHAL_MODEL_MSP430FR2355TPT)) > 1
#error "ohal: Multiple MSP430FR2xx models defined. " \
       "Pass exactly one OHAL_MODEL_* macro to the compiler."
#elif !defined(OHAL_MODEL_MSP430FR2355) && !defined(OHAL_MODEL_MSP430FR2355TPT)
#error "ohal: No MSP430FR2xx model defined. " \
       "Pass -DOHAL_MODEL_MSP430FR2355 or -DOHAL_MODEL_MSP430FR2355TPT " \
       "(or another MSP430FR2xx model) to the compiler."
#endif

#if defined(OHAL_MODEL_MSP430FR2355)
#include "ohal/platforms/msp430fr2xx/models/msp430fr2355/capabilities.hpp"
#include "ohal/platforms/msp430fr2xx/models/msp430fr2355/gpio.hpp"
#elif defined(OHAL_MODEL_MSP430FR2355TPT)
#include "ohal/platforms/msp430fr2xx/models/msp430fr2355tpt/capabilities.hpp"
#include "ohal/platforms/msp430fr2xx/models/msp430fr2355tpt/gpio.hpp"
#endif

#endif // OHAL_FAMILY_MSP430FR2XX

#endif // OHAL_PLATFORMS_MSP430FR2XX_FAMILY_HPP
