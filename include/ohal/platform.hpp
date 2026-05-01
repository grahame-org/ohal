#ifndef OHAL_PLATFORM_HPP
#define OHAL_PLATFORM_HPP

#if !defined(OHAL_FAMILY_STM32U0) && !defined(OHAL_FAMILY_TI_MSPM0) && !defined(OHAL_FAMILY_PIC)
#error "ohal: No MCU family defined. " \
       "Pass one of -DOHAL_FAMILY_STM32U0, -DOHAL_FAMILY_TI_MSPM0, " \
       "-DOHAL_FAMILY_PIC to the compiler."
#endif

#if defined(OHAL_FAMILY_STM32U0)
#include "ohal/platforms/stm32u0/family.hpp"
#elif defined(OHAL_FAMILY_TI_MSPM0)
#include "ohal/platforms/ti_mspm0/family.hpp"
#elif defined(OHAL_FAMILY_PIC)
#include "ohal/platforms/pic/family.hpp"
#endif

#endif // OHAL_PLATFORM_HPP
