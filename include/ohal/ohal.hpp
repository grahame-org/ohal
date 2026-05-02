#ifndef OHAL_OHAL_HPP
#define OHAL_OHAL_HPP

#include <ohal/core/access.hpp>
#include <ohal/core/capabilities.hpp>
#include <ohal/core/field.hpp>
#include <ohal/core/register.hpp>
#include <ohal/gpio.hpp>

// Include platform selection when a target MCU family is defined.
// This pulls in the MCU-specific Pin<> specialisations and triggers the
// compile-time checks for invalid/missing family or model defines.
// Omitted in host-side (non-target) builds where no OHAL_FAMILY_* macro is
// defined, so that host tests can include this header without errors.
#if defined(OHAL_FAMILY_STM32U0) || defined(OHAL_FAMILY_TI_MSPM0) || defined(OHAL_FAMILY_PIC)
#include <ohal/platform.hpp>
#endif

#endif // OHAL_OHAL_HPP
