#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_TIMER_IMPL_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_TIMER_IMPL_HPP

// Shared STM32U083 timer register map.
//
// This header provides the register address constants, register-struct templates
// (Tim1Regs, Tim23Regs, Tim67Regs, Tim15Regs, Tim16Regs, LptimRegs) and the
// concrete type aliases (Tim1…Tim16, Lptim1…Lptim3) that are common across
// every STM32U083 (and STM32U031) package variant.
//
// It does NOT provide any higher-level Timer<N> API or template specialisations;
// those belong in the public timer.hpp header once a timer abstraction layer is
// defined.  Package-specific timer.hpp headers include timer.hpp (which in turn
// includes this file) and may add their own specialisations at that point.

#include <cstdint>

#include "ohal/core/register.hpp"

namespace ohal::platforms::stm32u0::stm32u083 {

// ---------------------------------------------------------------------------
// Timer peripheral base addresses (RM0503 Rev 4, Table 4)
// ---------------------------------------------------------------------------

inline constexpr uintptr_t kTim1Base = 0x4001'2C00U;
inline constexpr uintptr_t kTim2Base = 0x4000'0000U;
inline constexpr uintptr_t kTim3Base = 0x4000'0400U;
inline constexpr uintptr_t kTim6Base = 0x4000'1000U;
inline constexpr uintptr_t kTim7Base = 0x4000'1400U;
inline constexpr uintptr_t kTim15Base = 0x4001'4000U;
inline constexpr uintptr_t kTim16Base = 0x4001'4400U;
inline constexpr uintptr_t kLptim1Base = 0x4000'7C00U;
inline constexpr uintptr_t kLptim2Base = 0x4000'9400U;
inline constexpr uintptr_t kLptim3Base = 0x4000'9000U;

// ---------------------------------------------------------------------------
// TIMx register offsets (RM0503 Rev 4, §28–§32)
// ---------------------------------------------------------------------------

inline constexpr uintptr_t kTimCr1Offset = 0x00U;
inline constexpr uintptr_t kTimCr2Offset = 0x04U;
inline constexpr uintptr_t kTimSmcrOffset = 0x08U;
inline constexpr uintptr_t kTimDierOffset = 0x0CU;
inline constexpr uintptr_t kTimSrOffset = 0x10U;
inline constexpr uintptr_t kTimEgrOffset = 0x14U;
inline constexpr uintptr_t kTimCcmr1Offset = 0x18U;
inline constexpr uintptr_t kTimCcmr2Offset = 0x1CU;
inline constexpr uintptr_t kTimCcerOffset = 0x20U;
inline constexpr uintptr_t kTimCntOffset = 0x24U;
inline constexpr uintptr_t kTimPscOffset = 0x28U;
inline constexpr uintptr_t kTimArrOffset = 0x2CU;
inline constexpr uintptr_t kTimRcrOffset = 0x30U;
inline constexpr uintptr_t kTimCcr1Offset = 0x34U;
inline constexpr uintptr_t kTimCcr2Offset = 0x38U;
inline constexpr uintptr_t kTimCcr3Offset = 0x3CU;
inline constexpr uintptr_t kTimCcr4Offset = 0x40U;
inline constexpr uintptr_t kTimBdtrOffset = 0x44U;
inline constexpr uintptr_t kTimDcrOffset = 0x48U;
inline constexpr uintptr_t kTimDmarOffset = 0x4CU;
inline constexpr uintptr_t kTimOr1Offset = 0x50U;
inline constexpr uintptr_t kTimCcmr3Offset = 0x54U;
inline constexpr uintptr_t kTimCcr5Offset = 0x58U;
inline constexpr uintptr_t kTimCcr6Offset = 0x5CU;
inline constexpr uintptr_t kTimAf1Offset = 0x60U;
inline constexpr uintptr_t kTimAf2Offset = 0x64U;
inline constexpr uintptr_t kTimTiselOffset = 0x68U;

// ---------------------------------------------------------------------------
// LPTIMx register offsets (RM0503 Rev 4, §39)
// ---------------------------------------------------------------------------

inline constexpr uintptr_t kLptimIsrOffset = 0x00U;
inline constexpr uintptr_t kLptimIcrOffset = 0x04U;
inline constexpr uintptr_t kLptimDierOffset = 0x08U;
inline constexpr uintptr_t kLptimCfgrOffset = 0x0CU;
inline constexpr uintptr_t kLptimCrOffset = 0x10U;
inline constexpr uintptr_t kLptimCcr1Offset = 0x14U;
inline constexpr uintptr_t kLptimArrOffset = 0x18U;
inline constexpr uintptr_t kLptimCntOffset = 0x1CU;
inline constexpr uintptr_t kLptimCfgr2Offset = 0x24U;
inline constexpr uintptr_t kLptimRcrOffset = 0x28U;
inline constexpr uintptr_t kLptimCcmr1Offset = 0x2CU;
inline constexpr uintptr_t kLptimCcmr2Offset = 0x30U;
inline constexpr uintptr_t kLptimCcr2Offset = 0x34U;
inline constexpr uintptr_t kLptimCcr3Offset = 0x38U;
inline constexpr uintptr_t kLptimCcr4Offset = 0x3CU;

// ---------------------------------------------------------------------------
// Register-struct templates
//
// Each template is parametrised on the peripheral base address so that the
// same struct layout can be reused for every instance of the same timer type.
// Instantiate with the base address constant from above, or use the concrete
// type aliases (Tim1, Tim2, …) defined at the end of this header.
// ---------------------------------------------------------------------------

/// Register map for TIM1 (advanced-control timer).
/// @tparam Base  Physical base address of the TIM1 peripheral.
template <uintptr_t Base>
struct Tim1Regs {
  using Cr1 = ohal::core::Register<Base + kTimCr1Offset>;
  using Cr2 = ohal::core::Register<Base + kTimCr2Offset>;
  using Smcr = ohal::core::Register<Base + kTimSmcrOffset>;
  using Dier = ohal::core::Register<Base + kTimDierOffset>;
  using Sr = ohal::core::Register<Base + kTimSrOffset>;
  using Egr = ohal::core::Register<Base + kTimEgrOffset>;
  using Ccmr1 = ohal::core::Register<Base + kTimCcmr1Offset>;
  using Ccmr2 = ohal::core::Register<Base + kTimCcmr2Offset>;
  using Ccer = ohal::core::Register<Base + kTimCcerOffset>;
  using Cnt = ohal::core::Register<Base + kTimCntOffset>;
  using Psc = ohal::core::Register<Base + kTimPscOffset>;
  using Arr = ohal::core::Register<Base + kTimArrOffset>;
  using Rcr = ohal::core::Register<Base + kTimRcrOffset>;
  using Ccr1 = ohal::core::Register<Base + kTimCcr1Offset>;
  using Ccr2 = ohal::core::Register<Base + kTimCcr2Offset>;
  using Ccr3 = ohal::core::Register<Base + kTimCcr3Offset>;
  using Ccr4 = ohal::core::Register<Base + kTimCcr4Offset>;
  using Bdtr = ohal::core::Register<Base + kTimBdtrOffset>;
  using Dcr = ohal::core::Register<Base + kTimDcrOffset>;
  using Dmar = ohal::core::Register<Base + kTimDmarOffset>;
  using Or1 = ohal::core::Register<Base + kTimOr1Offset>;
  using Ccmr3 = ohal::core::Register<Base + kTimCcmr3Offset>;
  using Ccr5 = ohal::core::Register<Base + kTimCcr5Offset>;
  using Ccr6 = ohal::core::Register<Base + kTimCcr6Offset>;
  using Af1 = ohal::core::Register<Base + kTimAf1Offset>;
  using Af2 = ohal::core::Register<Base + kTimAf2Offset>;
  using Tisel = ohal::core::Register<Base + kTimTiselOffset>;
};

/// Register map for TIM2 and TIM3 (general-purpose 32-bit timers).
/// @tparam Base  Physical base address of the TIM2 or TIM3 peripheral.
template <uintptr_t Base>
struct Tim23Regs {
  using Cr1 = ohal::core::Register<Base + kTimCr1Offset>;
  using Cr2 = ohal::core::Register<Base + kTimCr2Offset>;
  using Smcr = ohal::core::Register<Base + kTimSmcrOffset>;
  using Dier = ohal::core::Register<Base + kTimDierOffset>;
  using Sr = ohal::core::Register<Base + kTimSrOffset>;
  using Egr = ohal::core::Register<Base + kTimEgrOffset>;
  using Ccmr1 = ohal::core::Register<Base + kTimCcmr1Offset>;
  using Ccmr2 = ohal::core::Register<Base + kTimCcmr2Offset>;
  using Ccer = ohal::core::Register<Base + kTimCcerOffset>;
  using Cnt = ohal::core::Register<Base + kTimCntOffset>;
  using Psc = ohal::core::Register<Base + kTimPscOffset>;
  using Arr = ohal::core::Register<Base + kTimArrOffset>;
  using Ccr1 = ohal::core::Register<Base + kTimCcr1Offset>;
  using Ccr2 = ohal::core::Register<Base + kTimCcr2Offset>;
  using Ccr3 = ohal::core::Register<Base + kTimCcr3Offset>;
  using Ccr4 = ohal::core::Register<Base + kTimCcr4Offset>;
  using Dcr = ohal::core::Register<Base + kTimDcrOffset>;
  using Dmar = ohal::core::Register<Base + kTimDmarOffset>;
  using Or1 = ohal::core::Register<Base + kTimOr1Offset>;
  using Af1 = ohal::core::Register<Base + kTimAf1Offset>;
  using Tisel = ohal::core::Register<Base + kTimTiselOffset>;
};

/// Register map for TIM6 and TIM7 (basic timers — no output-compare channels).
/// @tparam Base  Physical base address of the TIM6 or TIM7 peripheral.
template <uintptr_t Base>
struct Tim67Regs {
  using Cr1 = ohal::core::Register<Base + kTimCr1Offset>;
  using Cr2 = ohal::core::Register<Base + kTimCr2Offset>;
  using Dier = ohal::core::Register<Base + kTimDierOffset>;
  using Sr = ohal::core::Register<Base + kTimSrOffset>;
  using Egr = ohal::core::Register<Base + kTimEgrOffset>;
  using Cnt = ohal::core::Register<Base + kTimCntOffset>;
  using Psc = ohal::core::Register<Base + kTimPscOffset>;
  using Arr = ohal::core::Register<Base + kTimArrOffset>;
};

/// Register map for TIM15 (general-purpose timer with two capture/compare channels).
/// @tparam Base  Physical base address of the TIM15 peripheral.
template <uintptr_t Base>
struct Tim15Regs {
  using Cr1 = ohal::core::Register<Base + kTimCr1Offset>;
  using Cr2 = ohal::core::Register<Base + kTimCr2Offset>;
  using Smcr = ohal::core::Register<Base + kTimSmcrOffset>;
  using Dier = ohal::core::Register<Base + kTimDierOffset>;
  using Sr = ohal::core::Register<Base + kTimSrOffset>;
  using Egr = ohal::core::Register<Base + kTimEgrOffset>;
  using Ccmr1 = ohal::core::Register<Base + kTimCcmr1Offset>;
  using Ccer = ohal::core::Register<Base + kTimCcerOffset>;
  using Cnt = ohal::core::Register<Base + kTimCntOffset>;
  using Psc = ohal::core::Register<Base + kTimPscOffset>;
  using Arr = ohal::core::Register<Base + kTimArrOffset>;
  using Rcr = ohal::core::Register<Base + kTimRcrOffset>;
  using Ccr1 = ohal::core::Register<Base + kTimCcr1Offset>;
  using Ccr2 = ohal::core::Register<Base + kTimCcr2Offset>;
  using Bdtr = ohal::core::Register<Base + kTimBdtrOffset>;
  using Dcr = ohal::core::Register<Base + kTimDcrOffset>;
  using Dmar = ohal::core::Register<Base + kTimDmarOffset>;
  using Af1 = ohal::core::Register<Base + kTimAf1Offset>;
  using Tisel = ohal::core::Register<Base + kTimTiselOffset>;
};

/// Register map for TIM16 (general-purpose timer with one capture/compare channel).
/// @tparam Base  Physical base address of the TIM16 peripheral.
template <uintptr_t Base>
struct Tim16Regs {
  using Cr1 = ohal::core::Register<Base + kTimCr1Offset>;
  using Cr2 = ohal::core::Register<Base + kTimCr2Offset>;
  using Dier = ohal::core::Register<Base + kTimDierOffset>;
  using Sr = ohal::core::Register<Base + kTimSrOffset>;
  using Egr = ohal::core::Register<Base + kTimEgrOffset>;
  using Ccmr1 = ohal::core::Register<Base + kTimCcmr1Offset>;
  using Ccer = ohal::core::Register<Base + kTimCcerOffset>;
  using Cnt = ohal::core::Register<Base + kTimCntOffset>;
  using Psc = ohal::core::Register<Base + kTimPscOffset>;
  using Arr = ohal::core::Register<Base + kTimArrOffset>;
  using Rcr = ohal::core::Register<Base + kTimRcrOffset>;
  using Ccr1 = ohal::core::Register<Base + kTimCcr1Offset>;
  using Bdtr = ohal::core::Register<Base + kTimBdtrOffset>;
  using Dcr = ohal::core::Register<Base + kTimDcrOffset>;
  using Dmar = ohal::core::Register<Base + kTimDmarOffset>;
  using Af1 = ohal::core::Register<Base + kTimAf1Offset>;
  using Tisel = ohal::core::Register<Base + kTimTiselOffset>;
};

/// Register map for LPTIM1, LPTIM2, and LPTIM3 (low-power timers).
/// @tparam Base  Physical base address of the LPTIMx peripheral.
template <uintptr_t Base>
struct LptimRegs {
  using Isr = ohal::core::Register<Base + kLptimIsrOffset>;
  using Icr = ohal::core::Register<Base + kLptimIcrOffset>;
  using Dier = ohal::core::Register<Base + kLptimDierOffset>;
  using Cfgr = ohal::core::Register<Base + kLptimCfgrOffset>;
  using Cr = ohal::core::Register<Base + kLptimCrOffset>;
  using Ccr1 = ohal::core::Register<Base + kLptimCcr1Offset>;
  using Arr = ohal::core::Register<Base + kLptimArrOffset>;
  using Cnt = ohal::core::Register<Base + kLptimCntOffset>;
  using Cfgr2 = ohal::core::Register<Base + kLptimCfgr2Offset>;
  using Rcr = ohal::core::Register<Base + kLptimRcrOffset>;
  using Ccmr1 = ohal::core::Register<Base + kLptimCcmr1Offset>;
  using Ccmr2 = ohal::core::Register<Base + kLptimCcmr2Offset>;
  using Ccr2 = ohal::core::Register<Base + kLptimCcr2Offset>;
  using Ccr3 = ohal::core::Register<Base + kLptimCcr3Offset>;
  using Ccr4 = ohal::core::Register<Base + kLptimCcr4Offset>;
};

// ---------------------------------------------------------------------------
// Concrete timer instance type aliases
// ---------------------------------------------------------------------------

using Tim1 = Tim1Regs<kTim1Base>;
using Tim2 = Tim23Regs<kTim2Base>;
using Tim3 = Tim23Regs<kTim3Base>;
using Tim6 = Tim67Regs<kTim6Base>;
using Tim7 = Tim67Regs<kTim7Base>;
using Tim15 = Tim15Regs<kTim15Base>;
using Tim16 = Tim16Regs<kTim16Base>;
using Lptim1 = LptimRegs<kLptim1Base>;
using Lptim2 = LptimRegs<kLptim2Base>;
using Lptim3 = LptimRegs<kLptim3Base>;

} // namespace ohal::platforms::stm32u0::stm32u083

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_TIMER_IMPL_HPP
