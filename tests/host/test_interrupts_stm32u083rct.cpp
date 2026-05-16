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

} // namespace
