#ifndef OHAL_PLATFORMS_MSP430FR2XX_FAMILY_HPP
#define OHAL_PLATFORMS_MSP430FR2XX_FAMILY_HPP

// This header is included only when OHAL_FAMILY_MSP430FR2XX is defined
// (see ohal/platform.hpp).  The outer #ifdef prevents clang-tidy, which
// processes every header with -DOHAL_FAMILY_STM32U0, from triggering the
// model-not-defined #error.
#ifdef OHAL_FAMILY_MSP430FR2XX

// Recognised MSP430FR2xx models/packages.  Add a new clause to both guards
// and a corresponding #include block below when a new model or package is
// implemented.

#if (defined(OHAL_MODEL_MSP430FR2355) + defined(OHAL_MODEL_MSP430FR2355TPT) +   \
     defined(OHAL_MODEL_MSP430FR2355TDBT) + defined(OHAL_MODEL_MSP430FR2355TRSM) + \
     defined(OHAL_MODEL_MSP430FR2355TRHA) + defined(OHAL_MODEL_MSP430FR2353TPT) + \
     defined(OHAL_MODEL_MSP430FR2353TDBT) + defined(OHAL_MODEL_MSP430FR2353TRSM) + \
     defined(OHAL_MODEL_MSP430FR2353TRHA) + defined(OHAL_MODEL_MSP430FR2155TPT) + \
     defined(OHAL_MODEL_MSP430FR2155TDBT) + defined(OHAL_MODEL_MSP430FR2155TRSM) + \
     defined(OHAL_MODEL_MSP430FR2155TRHA) + defined(OHAL_MODEL_MSP430FR2153TPT) + \
     defined(OHAL_MODEL_MSP430FR2153TDBT) + defined(OHAL_MODEL_MSP430FR2153TRSM) + \
     defined(OHAL_MODEL_MSP430FR2153TRHA)) > 1
#error "ohal: Multiple MSP430FR2xx models defined. " \
       "Pass exactly one OHAL_MODEL_* macro to the compiler."
#elif !defined(OHAL_MODEL_MSP430FR2355) && !defined(OHAL_MODEL_MSP430FR2355TPT) &&   \
    !defined(OHAL_MODEL_MSP430FR2355TDBT) && !defined(OHAL_MODEL_MSP430FR2355TRSM) && \
    !defined(OHAL_MODEL_MSP430FR2355TRHA) && !defined(OHAL_MODEL_MSP430FR2353TPT) && \
    !defined(OHAL_MODEL_MSP430FR2353TDBT) && !defined(OHAL_MODEL_MSP430FR2353TRSM) && \
    !defined(OHAL_MODEL_MSP430FR2353TRHA) && !defined(OHAL_MODEL_MSP430FR2155TPT) && \
    !defined(OHAL_MODEL_MSP430FR2155TDBT) && !defined(OHAL_MODEL_MSP430FR2155TRSM) && \
    !defined(OHAL_MODEL_MSP430FR2155TRHA) && !defined(OHAL_MODEL_MSP430FR2153TPT) && \
    !defined(OHAL_MODEL_MSP430FR2153TDBT) && !defined(OHAL_MODEL_MSP430FR2153TRSM) && \
    !defined(OHAL_MODEL_MSP430FR2153TRHA)
#error "ohal: No MSP430FR2xx model defined. " \
       "Pass -DOHAL_MODEL_MSP430FR2355 or another MSP430FR2xx model macro " \
       "to the compiler."
#endif

#if defined(OHAL_MODEL_MSP430FR2355)
#include "ohal/platforms/msp430fr2xx/models/msp430fr2355/capabilities.hpp"
#include "ohal/platforms/msp430fr2xx/models/msp430fr2355/gpio.hpp"
#elif defined(OHAL_MODEL_MSP430FR2355TPT)
#include "ohal/platforms/msp430fr2xx/models/msp430fr2355tpt/capabilities.hpp"
#include "ohal/platforms/msp430fr2xx/models/msp430fr2355tpt/gpio.hpp"
#elif defined(OHAL_MODEL_MSP430FR2355TDBT)
#include "ohal/platforms/msp430fr2xx/models/msp430fr2355tdbt/capabilities.hpp"
#include "ohal/platforms/msp430fr2xx/models/msp430fr2355tdbt/gpio.hpp"
#elif defined(OHAL_MODEL_MSP430FR2355TRSM)
#include "ohal/platforms/msp430fr2xx/models/msp430fr2355trsm/capabilities.hpp"
#include "ohal/platforms/msp430fr2xx/models/msp430fr2355trsm/gpio.hpp"
#elif defined(OHAL_MODEL_MSP430FR2355TRHA)
#include "ohal/platforms/msp430fr2xx/models/msp430fr2355trha/capabilities.hpp"
#include "ohal/platforms/msp430fr2xx/models/msp430fr2355trha/gpio.hpp"
#elif defined(OHAL_MODEL_MSP430FR2353TPT)
#include "ohal/platforms/msp430fr2xx/models/msp430fr2353tpt/capabilities.hpp"
#include "ohal/platforms/msp430fr2xx/models/msp430fr2353tpt/gpio.hpp"
#elif defined(OHAL_MODEL_MSP430FR2353TDBT)
#include "ohal/platforms/msp430fr2xx/models/msp430fr2353tdbt/capabilities.hpp"
#include "ohal/platforms/msp430fr2xx/models/msp430fr2353tdbt/gpio.hpp"
#elif defined(OHAL_MODEL_MSP430FR2353TRSM)
#include "ohal/platforms/msp430fr2xx/models/msp430fr2353trsm/capabilities.hpp"
#include "ohal/platforms/msp430fr2xx/models/msp430fr2353trsm/gpio.hpp"
#elif defined(OHAL_MODEL_MSP430FR2353TRHA)
#include "ohal/platforms/msp430fr2xx/models/msp430fr2353trha/capabilities.hpp"
#include "ohal/platforms/msp430fr2xx/models/msp430fr2353trha/gpio.hpp"
#elif defined(OHAL_MODEL_MSP430FR2155TPT)
#include "ohal/platforms/msp430fr2xx/models/msp430fr2155tpt/capabilities.hpp"
#include "ohal/platforms/msp430fr2xx/models/msp430fr2155tpt/gpio.hpp"
#elif defined(OHAL_MODEL_MSP430FR2155TDBT)
#include "ohal/platforms/msp430fr2xx/models/msp430fr2155tdbt/capabilities.hpp"
#include "ohal/platforms/msp430fr2xx/models/msp430fr2155tdbt/gpio.hpp"
#elif defined(OHAL_MODEL_MSP430FR2155TRSM)
#include "ohal/platforms/msp430fr2xx/models/msp430fr2155trsm/capabilities.hpp"
#include "ohal/platforms/msp430fr2xx/models/msp430fr2155trsm/gpio.hpp"
#elif defined(OHAL_MODEL_MSP430FR2155TRHA)
#include "ohal/platforms/msp430fr2xx/models/msp430fr2155trha/capabilities.hpp"
#include "ohal/platforms/msp430fr2xx/models/msp430fr2155trha/gpio.hpp"
#elif defined(OHAL_MODEL_MSP430FR2153TPT)
#include "ohal/platforms/msp430fr2xx/models/msp430fr2153tpt/capabilities.hpp"
#include "ohal/platforms/msp430fr2xx/models/msp430fr2153tpt/gpio.hpp"
#elif defined(OHAL_MODEL_MSP430FR2153TDBT)
#include "ohal/platforms/msp430fr2xx/models/msp430fr2153tdbt/capabilities.hpp"
#include "ohal/platforms/msp430fr2xx/models/msp430fr2153tdbt/gpio.hpp"
#elif defined(OHAL_MODEL_MSP430FR2153TRSM)
#include "ohal/platforms/msp430fr2xx/models/msp430fr2153trsm/capabilities.hpp"
#include "ohal/platforms/msp430fr2xx/models/msp430fr2153trsm/gpio.hpp"
#elif defined(OHAL_MODEL_MSP430FR2153TRHA)
#include "ohal/platforms/msp430fr2xx/models/msp430fr2153trha/capabilities.hpp"
#include "ohal/platforms/msp430fr2xx/models/msp430fr2153trha/gpio.hpp"
#endif

#endif // OHAL_FAMILY_MSP430FR2XX

#endif // OHAL_PLATFORMS_MSP430FR2XX_FAMILY_HPP
