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

#include <type_traits>

#include "ohal/core/field.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083/timer_impl.hpp"
#include "ohal/timer.hpp"

namespace ohal::timer {

template <uint8_t ChannelNum>
struct Channel<ohal::platforms::stm32u0::stm32u083::Tim2, ChannelNum> {
  static_assert(ChannelNum < 4U, "ohal: STM32U083 TIM2 has channels 0-3 only.");

  using Regs = ohal::platforms::stm32u0::stm32u083::Tim2;

  // Timer-wide registers are exposed here for convenience, while Ccr is the
  // channel-specific register selected by ChannelNum.
  using Cr1 = typename Regs::Cr1;
  using Cr2 = typename Regs::Cr2;
  using Dier = typename Regs::Dier;
  using Sr = typename Regs::Sr;
  using Ccmr1 = typename Regs::Ccmr1;
  using Ccmr2 = typename Regs::Ccmr2;
  using Ccer = typename Regs::Ccer;
  using Cnt = typename Regs::Cnt;
  using Psc = typename Regs::Psc;
  using Arr = typename Regs::Arr;

  using Egr = ohal::core::BitField<typename Regs::Egr, 0U,
                                   ohal::platforms::stm32u0::stm32u083::kTimRegisterWidthBits,
                                   ohal::core::Access::WriteOnly>;

  // Capture/compare register for this channel (ChannelNum 0→CCR1, 1→CCR2, 2→CCR3, 3→CCR4).
  using Ccr = std::conditional_t<
      ChannelNum == 0U, typename Regs::Ccr1,
      std::conditional_t<
          ChannelNum == 1U, typename Regs::Ccr2,
          std::conditional_t<ChannelNum == 2U, typename Regs::Ccr3, typename Regs::Ccr4>>>;
};

} // namespace ohal::timer

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_TIMER_HPP
