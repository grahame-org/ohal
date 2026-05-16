#include <ohal/exti.hpp>
#include <ohal/irq.hpp>
#include <ohal/nvic.hpp>
#include <ohal/platforms/stm32u0/models/stm32u083/exti.hpp>
#include <ohal/platforms/stm32u0/models/stm32u083/irq.hpp>
#include <ohal/platforms/stm32u0/models/stm32u083/irq_numbers.hpp>
#include <ohal/platforms/stm32u0/models/stm32u083/nvic.hpp>
#include <ohal/platforms/stm32u0/models/stm32u083rct/gpio.hpp>

#include <type_traits>

#include <gtest/gtest.h>

namespace {

using Family = ohal::platforms::stm32u0::Family;
namespace nvic_wiring = ohal::platforms::stm32u0::stm32u083;

TEST(Stm32u083InterruptTraitsTest, FamilyHasGlobalInterruptControl) {
  EXPECT_TRUE(ohal::irq::has_global_irq_control<Family>::value);
}

TEST(Stm32u083InterruptTraitsTest, FamilyHasNvic) {
  EXPECT_TRUE(ohal::nvic::has_nvic<Family>::value);
}

TEST(Stm32u083InterruptTraitsTest, FamilyHasExti) {
  EXPECT_TRUE(ohal::exti::has_exti<Family>::value);
}

TEST(Stm32u083InterruptTraitsTest, FamilyHasSystemExceptionPriorityControl) {
  EXPECT_TRUE(ohal::nvic::has_system_exception_priority_control<Family>::value);
}

TEST(Stm32u083InterruptTraitsTest, NvicPriorityBitsMatchSpecification) {
  EXPECT_EQ(ohal::nvic::nvic_priority_bits<Family>::value, 2U);
}

struct IrqVectorCase {
  uint8_t actual;
  uint8_t expected;
  const char* name;
};

class Stm32u083IrqVectorNumberTest : public ::testing::TestWithParam<IrqVectorCase> {};

INSTANTIATE_TEST_SUITE_P(
    Numbering, Stm32u083IrqVectorNumberTest,
    ::testing::Values(
        IrqVectorCase{static_cast<uint8_t>(nvic_wiring::IrqNumber::Exti0_1), 5U, "Exti0_1"},
        IrqVectorCase{static_cast<uint8_t>(nvic_wiring::IrqNumber::Tim2), 15U, "Tim2"},
        IrqVectorCase{static_cast<uint8_t>(nvic_wiring::IrqNumber::I2c1), 23U, "I2c1"},
        IrqVectorCase{static_cast<uint8_t>(nvic_wiring::IrqNumber::Usart2Lpuart2), 28U,
                      "Usart2Lpuart2"},
        IrqVectorCase{static_cast<uint8_t>(nvic_wiring::IrqNumber::AesRng), 31U, "AesRng"}),
    [](const ::testing::TestParamInfo<IrqVectorCase>& info) { return info.param.name; });

TEST_P(Stm32u083IrqVectorNumberTest, IrqNumberMatchesExpectedPosition) {
  EXPECT_EQ(GetParam().actual, GetParam().expected);
}

using Tim2Nvic = ohal::nvic::Controller<Family, nvic_wiring::IrqNumber::Tim2>;
using Exti0_1Nvic = ohal::nvic::Controller<Family, nvic_wiring::IrqNumber::Exti0_1>;
using Pa5Line = ohal::exti::Line<ohal::gpio::PortA, 5U>;
using Pf13Line = ohal::exti::Line<ohal::gpio::PortF, 13U>;

TEST(Stm32u083InterruptWiringTest, Tim2UsesNvicIserWordZero) {
  EXPECT_EQ(Tim2Nvic::Iser::address, nvic_wiring::kNvicIserBase);
}

TEST(Stm32u083InterruptWiringTest, Exti0_1UsesBitFive) {
  EXPECT_EQ(Exti0_1Nvic::kBitMask, 1UL << 5U);
}

TEST(Stm32u083InterruptWiringTest, ExtiLineUsesExtiBaseForInterruptMaskRegister) {
  EXPECT_EQ(Pa5Line::Imr1::address, nvic_wiring::kExtiBase + nvic_wiring::kExtiImr1Offset);
}

TEST(Stm32u083InterruptWiringTest, ExtiPortAUsesSyscfgCodeZero) {
  EXPECT_EQ(Pa5Line::kPortCode, 0U);
}

TEST(Stm32u083InterruptWiringTest, ExtiPortFUsesSyscfgCodeFive) {
  EXPECT_EQ(Pf13Line::kPortCode, 5U);
}

TEST(Stm32u083InterruptWiringTest, ExtiLineFiveUsesExticr2Register) {
  EXPECT_EQ(Pa5Line::Exticr::address,
            nvic_wiring::kSyscfgBase + nvic_wiring::kSyscfgExticr1Offset + sizeof(uint32_t));
}

TEST(Stm32u083InterruptWiringTest, ExtiLineThirteenUsesExticr4Register) {
  EXPECT_EQ(Pf13Line::Exticr::address,
            nvic_wiring::kSyscfgBase + nvic_wiring::kSyscfgExticr1Offset + (3U * sizeof(uint32_t)));
}

// ---------------------------------------------------------------------------
// EXC_RETURN constants — ARMv6-M exception return magic values (PM0223 §2.3.7).
// ---------------------------------------------------------------------------

TEST(Stm32u083ExcReturnTest, HandlerMspValueMatchesArchSpec) {
  EXPECT_EQ(ohal::irq::exc_return::kHandlerMsp, 0xFFFFFFF1U);
}

TEST(Stm32u083ExcReturnTest, ThreadMspValueMatchesArchSpec) {
  EXPECT_EQ(ohal::irq::exc_return::kThreadMsp, 0xFFFFFFF9U);
}

TEST(Stm32u083ExcReturnTest, ThreadPspValueMatchesArchSpec) {
  EXPECT_EQ(ohal::irq::exc_return::kThreadPsp, 0xFFFFFFFDU);
}

// ---------------------------------------------------------------------------
// System exception (SHPR) wiring tests — verify register addresses and shifts.
// ---------------------------------------------------------------------------

using SVCallCtrl = ohal::nvic::SystemController<Family, nvic_wiring::SystemException::SVCall>;
using PendSVCtrl = ohal::nvic::SystemController<Family, nvic_wiring::SystemException::PendSV>;
using SysTickCtrl =
    ohal::nvic::SystemController<Family, nvic_wiring::SystemException::SysTickTimer>;

TEST(Stm32u083SystemExceptionWiringTest, SVCallUsesShpr2Register) {
  EXPECT_EQ(SVCallCtrl::Shpr::address, nvic_wiring::kScbBase + nvic_wiring::kScbShpr2Offset);
}

TEST(Stm32u083SystemExceptionWiringTest, PendSVUsesShpr3Register) {
  EXPECT_EQ(PendSVCtrl::Shpr::address, nvic_wiring::kScbBase + nvic_wiring::kScbShpr3Offset);
}

TEST(Stm32u083SystemExceptionWiringTest, SysTickUsesShpr3Register) {
  EXPECT_EQ(SysTickCtrl::Shpr::address, nvic_wiring::kScbBase + nvic_wiring::kScbShpr3Offset);
}

TEST(Stm32u083SystemExceptionWiringTest, SVCallPriorityEncodedInBits31To30OfShpr2) {
  EXPECT_EQ(SVCallCtrl::kPriorityEncodedShift, 30U);
}

TEST(Stm32u083SystemExceptionWiringTest, PendSVPriorityEncodedInBits23To22OfShpr3) {
  EXPECT_EQ(PendSVCtrl::kPriorityEncodedShift, 22U);
}

TEST(Stm32u083SystemExceptionWiringTest, SysTickPriorityEncodedInBits31To30OfShpr3) {
  EXPECT_EQ(SysTickCtrl::kPriorityEncodedShift, 30U);
}

TEST(Stm32u083SystemExceptionWiringTest, SystemExceptionPriorityValueMaskIsTwoBits) {
  EXPECT_EQ(SVCallCtrl::kPriorityValueMask, 0x3U);
}

// ---------------------------------------------------------------------------
// SystemException IRQ-number values match the CMSIS convention.
// ---------------------------------------------------------------------------

TEST(Stm32u083SystemExceptionNumberTest, SVCallIrqNumberMatchesCmsisCodes) {
  EXPECT_EQ(static_cast<int8_t>(nvic_wiring::SystemException::SVCall), -5);
}

TEST(Stm32u083SystemExceptionNumberTest, PendSVIrqNumberMatchesCmsisCodes) {
  EXPECT_EQ(static_cast<int8_t>(nvic_wiring::SystemException::PendSV), -2);
}

TEST(Stm32u083SystemExceptionNumberTest, SysTickIrqNumberMatchesCmsisCodes) {
  EXPECT_EQ(static_cast<int8_t>(nvic_wiring::SystemException::SysTickTimer), -1);
}

} // namespace
