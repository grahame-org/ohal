#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_TIMER_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_TIMER_HPP

// Shared STM32U083 timer public header.
//
// This header pulls in the shared STM32U083 timer register map (timer_impl.hpp)
// which provides the base address constants, register-offset constants,
// register-struct templates, and concrete timer type aliases (Tim1…Tim16,
// Lptim1…Lptim3).
//
// Package-specific timer.hpp headers include this file.  When a higher-level
// Timer<N> abstraction API is added in the future, any specialisations that
// are common across packages will live here (analogous to how gpio.hpp
// includes gpio_impl.hpp and then provides the shared Pin<>/Port<>
// specialisations).

#include "ohal/platforms/stm32u0/models/stm32u083/timer_impl.hpp"

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_TIMER_HPP
