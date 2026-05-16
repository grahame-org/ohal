// Exercises the include chain: stm32u031r8t/timer.hpp → stm32u083/timer.hpp
// → timer_impl.hpp.  The STM32U031 package header does NOT define
// OHAL_STM32U0_ENABLE_LPTIM3, so kLptim3Base and Lptim3 are absent.
#include <ohal/platforms/stm32u0/models/stm32u031r8t/timer.hpp>
// STM32U0 IRQ numbers are currently provided via this stm32u083 path with
// conditional compilation for sub-family differences.
#include <ohal/platforms/stm32u0/models/stm32u083/irq_numbers.hpp>

#include <cstdint>

#include <gtest/gtest.h>

#if defined(OHAL_STM32U0_ENABLE_LPTIM3)
#error "STM32U031 tests must compile without OHAL_STM32U0_ENABLE_LPTIM3."
#endif

// ---------------------------------------------------------------------------
// Host-side register-address wiring tests for the STM32U031 timer peripherals.
//
// Exercises the include chain stm32u031r8t/timer.hpp → stm32u083/timer.hpp
// → timer_impl.hpp.  STM32U031 packages expose TIM1/2/3/6/7/15/16 plus
// LPTIM1 and LPTIM2.  LPTIM3 is absent: OHAL_STM32U0_ENABLE_LPTIM3 is not
// defined for STM32U031, so kLptim3Base and Lptim3 do not exist in this
// translation unit.  See test_timer_stm32u083.cpp for the STM32U083 path
// where LPTIM3 is present.
//
// Each test checks that the compile-time ::address constant exposed by every
// ohal::core::Register instantiation resolves to the correct MMIO address.
// ---------------------------------------------------------------------------

namespace {

namespace tim = ohal::platforms::stm32u0::stm32u083;
// STM32U0 IRQ numbering currently shares this stm32u083 namespace path.
namespace irq_wiring = ohal::platforms::stm32u0::stm32u083;

// ---------------------------------------------------------------------------
// Static (compile-time) address assertions for a representative register in
// each timer instance.  These catch any future mis-wiring at build time.
// ---------------------------------------------------------------------------

// TIM1 (advanced-control) — base 0x4001'2C00
static_assert(tim::Tim1::Cr1::address == tim::kTim1Base + tim::kTimCr1Offset,
              "Tim1::Cr1 must map to TIM1_CR1 address");
static_assert(tim::Tim1::Arr::address == tim::kTim1Base + tim::kTimArrOffset,
              "Tim1::Arr must map to TIM1_ARR address");
static_assert(tim::Tim1::Tisel::address == tim::kTim1Base + tim::kTimTiselOffset,
              "Tim1::Tisel must map to TIM1_TISEL address");

// TIM2 (general-purpose 32-bit) — base 0x4000'0000
static_assert(tim::Tim2::Cr1::address == tim::kTim2Base + tim::kTimCr1Offset,
              "Tim2::Cr1 must map to TIM2_CR1 address");
static_assert(tim::Tim2::Arr::address == tim::kTim2Base + tim::kTimArrOffset,
              "Tim2::Arr must map to TIM2_ARR address");
static_assert(tim::Tim2::Tisel::address == tim::kTim2Base + tim::kTimTiselOffset,
              "Tim2::Tisel must map to TIM2_TISEL address");

// TIM3 (general-purpose 32-bit) — base 0x4000'0400
static_assert(tim::Tim3::Cr1::address == tim::kTim3Base + tim::kTimCr1Offset,
              "Tim3::Cr1 must map to TIM3_CR1 address");
static_assert(tim::Tim3::Arr::address == tim::kTim3Base + tim::kTimArrOffset,
              "Tim3::Arr must map to TIM3_ARR address");
static_assert(tim::Tim3::Tisel::address == tim::kTim3Base + tim::kTimTiselOffset,
              "Tim3::Tisel must map to TIM3_TISEL address");

// TIM6 (basic) — base 0x4000'1000
static_assert(tim::Tim6::Cr1::address == tim::kTim6Base + tim::kTimCr1Offset,
              "Tim6::Cr1 must map to TIM6_CR1 address");
static_assert(tim::Tim6::Arr::address == tim::kTim6Base + tim::kTimArrOffset,
              "Tim6::Arr must map to TIM6_ARR address");

// TIM7 (basic) — base 0x4000'1400
static_assert(tim::Tim7::Cr1::address == tim::kTim7Base + tim::kTimCr1Offset,
              "Tim7::Cr1 must map to TIM7_CR1 address");
static_assert(tim::Tim7::Arr::address == tim::kTim7Base + tim::kTimArrOffset,
              "Tim7::Arr must map to TIM7_ARR address");

// TIM15 — base 0x4001'4000
static_assert(tim::Tim15::Cr1::address == tim::kTim15Base + tim::kTimCr1Offset,
              "Tim15::Cr1 must map to TIM15_CR1 address");
static_assert(tim::Tim15::Arr::address == tim::kTim15Base + tim::kTimArrOffset,
              "Tim15::Arr must map to TIM15_ARR address");
static_assert(tim::Tim15::Tisel::address == tim::kTim15Base + tim::kTimTiselOffset,
              "Tim15::Tisel must map to TIM15_TISEL address");

// TIM16 — base 0x4001'4400
static_assert(tim::Tim16::Cr1::address == tim::kTim16Base + tim::kTimCr1Offset,
              "Tim16::Cr1 must map to TIM16_CR1 address");
static_assert(tim::Tim16::Arr::address == tim::kTim16Base + tim::kTimArrOffset,
              "Tim16::Arr must map to TIM16_ARR address");
static_assert(tim::Tim16::Tisel::address == tim::kTim16Base + tim::kTimTiselOffset,
              "Tim16::Tisel must map to TIM16_TISEL address");

// LPTIM1 — base 0x4000'7C00
static_assert(tim::Lptim1::Isr::address == tim::kLptim1Base + tim::kLptimIsrOffset,
              "Lptim1::Isr must map to LPTIM1_ISR address");
static_assert(tim::Lptim1::Arr::address == tim::kLptim1Base + tim::kLptimArrOffset,
              "Lptim1::Arr must map to LPTIM1_ARR address");
static_assert(tim::Lptim1::Ccr4::address == tim::kLptim1Base + tim::kLptimCcr4Offset,
              "Lptim1::Ccr4 must map to LPTIM1_CCR4 address");

// LPTIM2 — base 0x4000'9400
static_assert(tim::Lptim2::Isr::address == tim::kLptim2Base + tim::kLptimIsrOffset,
              "Lptim2::Isr must map to LPTIM2_ISR address");
static_assert(tim::Lptim2::Arr::address == tim::kLptim2Base + tim::kLptimArrOffset,
              "Lptim2::Arr must map to LPTIM2_ARR address");
static_assert(tim::Lptim2::Ccr4::address == tim::kLptim2Base + tim::kLptimCcr4Offset,
              "Lptim2::Ccr4 must map to LPTIM2_CCR4 address");

// LPTIM3 is absent on STM32U031 — kLptim3Base and Lptim3 are not defined when
// OHAL_STM32U0_ENABLE_LPTIM3 is not set, so no static_asserts for LPTIM3 here.

// ---------------------------------------------------------------------------
// Runtime parameterised wiring tests: each case checks that one register's
// compile-time address constant matches the expected base + offset formula.
// ---------------------------------------------------------------------------

struct WiringCase {
  uintptr_t actual;
  uintptr_t expected;
  const char* name;
};

class TimerStm32u031WiringTest : public ::testing::TestWithParam<WiringCase> {};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    RegisterAddresses, TimerStm32u031WiringTest,
    ::testing::Values(
        // TIM1 — advanced-control timer
        WiringCase{tim::Tim1::Cr1::address,   tim::kTim1Base + tim::kTimCr1Offset,    "Tim1_Cr1"},
        WiringCase{tim::Tim1::Arr::address,   tim::kTim1Base + tim::kTimArrOffset,    "Tim1_Arr"},
        WiringCase{tim::Tim1::Bdtr::address,  tim::kTim1Base + tim::kTimBdtrOffset,   "Tim1_Bdtr"},
        WiringCase{tim::Tim1::Tisel::address, tim::kTim1Base + tim::kTimTiselOffset,  "Tim1_Tisel"},
        // TIM2 — general-purpose 32-bit timer
        WiringCase{tim::Tim2::Cr1::address,   tim::kTim2Base + tim::kTimCr1Offset,    "Tim2_Cr1"},
        WiringCase{tim::Tim2::Arr::address,   tim::kTim2Base + tim::kTimArrOffset,    "Tim2_Arr"},
        WiringCase{tim::Tim2::Tisel::address, tim::kTim2Base + tim::kTimTiselOffset,  "Tim2_Tisel"},
        // TIM3 — general-purpose 32-bit timer
        WiringCase{tim::Tim3::Cr1::address,   tim::kTim3Base + tim::kTimCr1Offset,    "Tim3_Cr1"},
        WiringCase{tim::Tim3::Arr::address,   tim::kTim3Base + tim::kTimArrOffset,    "Tim3_Arr"},
        WiringCase{tim::Tim3::Tisel::address, tim::kTim3Base + tim::kTimTiselOffset,  "Tim3_Tisel"},
        // TIM6 — basic timer
        WiringCase{tim::Tim6::Cr1::address,   tim::kTim6Base + tim::kTimCr1Offset,    "Tim6_Cr1"},
        WiringCase{tim::Tim6::Arr::address,   tim::kTim6Base + tim::kTimArrOffset,    "Tim6_Arr"},
        // TIM7 — basic timer
        WiringCase{tim::Tim7::Cr1::address,   tim::kTim7Base + tim::kTimCr1Offset,    "Tim7_Cr1"},
        WiringCase{tim::Tim7::Arr::address,   tim::kTim7Base + tim::kTimArrOffset,    "Tim7_Arr"},
        // TIM15 — general-purpose timer with two capture/compare channels
        WiringCase{tim::Tim15::Cr1::address,   tim::kTim15Base + tim::kTimCr1Offset,   "Tim15_Cr1"},
        WiringCase{tim::Tim15::Arr::address,   tim::kTim15Base + tim::kTimArrOffset,   "Tim15_Arr"},
        WiringCase{tim::Tim15::Bdtr::address,  tim::kTim15Base + tim::kTimBdtrOffset,  "Tim15_Bdtr"},
        WiringCase{tim::Tim15::Tisel::address, tim::kTim15Base + tim::kTimTiselOffset, "Tim15_Tisel"},
        // TIM16 — general-purpose timer with one capture/compare channel
        WiringCase{tim::Tim16::Cr1::address,   tim::kTim16Base + tim::kTimCr1Offset,   "Tim16_Cr1"},
        WiringCase{tim::Tim16::Arr::address,   tim::kTim16Base + tim::kTimArrOffset,   "Tim16_Arr"},
        WiringCase{tim::Tim16::Bdtr::address,  tim::kTim16Base + tim::kTimBdtrOffset,  "Tim16_Bdtr"},
        WiringCase{tim::Tim16::Tisel::address, tim::kTim16Base + tim::kTimTiselOffset, "Tim16_Tisel"},
        // LPTIM1 — low-power timer
        WiringCase{tim::Lptim1::Isr::address,  tim::kLptim1Base + tim::kLptimIsrOffset,  "Lptim1_Isr"},
        WiringCase{tim::Lptim1::Arr::address,  tim::kLptim1Base + tim::kLptimArrOffset,  "Lptim1_Arr"},
        WiringCase{tim::Lptim1::Ccr4::address, tim::kLptim1Base + tim::kLptimCcr4Offset, "Lptim1_Ccr4"},
        // LPTIM2 — low-power timer
        WiringCase{tim::Lptim2::Isr::address,  tim::kLptim2Base + tim::kLptimIsrOffset,  "Lptim2_Isr"},
        WiringCase{tim::Lptim2::Arr::address,  tim::kLptim2Base + tim::kLptimArrOffset,  "Lptim2_Arr"},
        WiringCase{tim::Lptim2::Ccr4::address, tim::kLptim2Base + tim::kLptimCcr4Offset, "Lptim2_Ccr4"}),
        // LPTIM3 is absent on STM32U031 — no test cases for it here.
    [](const ::testing::TestParamInfo<WiringCase>& info) { return info.param.name; });
// clang-format on

TEST_P(TimerStm32u031WiringTest, AddressMatchesHardwareBase) {
  EXPECT_EQ(GetParam().actual, GetParam().expected);
}

TEST(Stm32u031IrqVectorNumberTest, IrqNumberTim15_MapsToPosition19) {
  EXPECT_EQ(static_cast<uint8_t>(irq_wiring::IrqNumber::Tim15), 19U);
}

} // namespace
