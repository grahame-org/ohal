#include "mock/mock_register.hpp"

#include <ohal/core/capabilities.hpp>
#include <ohal/core/field.hpp>
#include <ohal/gpio.hpp>
#include <ohal/platforms/stm32u0/models/stm32u083rct/capabilities.hpp>
#include <ohal/platforms/stm32u0/models/stm32u083rct/gpio.hpp>

#include <cstdint>
#include <type_traits>

#include <gtest/gtest.h>

// ---------------------------------------------------------------------------
// Host-side tests for the STM32U083RCT (64-pin LQFP) GPIO model.
//
// The STM32U083RCT exposes GPIOA, GPIOB, GPIOC, GPIOD and GPIOF; GPIOE is
// not bonded out.  These tests verify:
//   1. The capability traits report the correct values for the RCT package.
//   2. The Pin<>/Port<> specialisations resolve to the correct hardware base
//      addresses (i.e. the wiring through the thin-wrapper header is correct).
//   3. Basic behavioural correctness via the mock-register infrastructure.
// ---------------------------------------------------------------------------

namespace {

static uint32_t mock_moder{0U};
static uint32_t mock_otyper{0U};
static uint32_t mock_ospeedr{0U};
static uint32_t mock_pupdr{0U};
static uint32_t mock_idr{0U};
static uint32_t mock_odr{0U};
static uint32_t mock_bsrr{0U};
static uint32_t mock_lckr{0U};
static uint32_t mock_afrl{0U};
static uint32_t mock_afrh{0U};
static uint32_t mock_brr{0U};

/// Mock GPIO port register set with the same nested type aliases as
/// GpioPortRegs<Base>, backed by plain in-memory variables.
struct MockGpioRegs {
  using Moder = ohal::test::MockRegister<uint32_t, &mock_moder>;
  using Otyper = ohal::test::MockRegister<uint32_t, &mock_otyper>;
  using Ospeedr = ohal::test::MockRegister<uint32_t, &mock_ospeedr>;
  using Pupdr = ohal::test::MockRegister<uint32_t, &mock_pupdr>;
  using Idr = ohal::test::MockRegister<uint32_t, &mock_idr>;
  using Odr = ohal::test::MockRegister<uint32_t, &mock_odr>;
  using Bsrr = ohal::test::MockRegister<uint32_t, &mock_bsrr>;
  using Lckr = ohal::test::MockRegister<uint32_t, &mock_lckr>;
  using Afrl = ohal::test::MockRegister<uint32_t, &mock_afrl>;
  using Afrh = ohal::test::MockRegister<uint32_t, &mock_afrh>;
  using Brr = ohal::test::MockRegister<uint32_t, &mock_brr>;
};

using MockPin0 = ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<0U, MockGpioRegs>;
using MockPin5 = ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<5U, MockGpioRegs>;
using MockPin15 = ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<15U, MockGpioRegs>;
using MockPort = ohal::platforms::stm32u0::stm32u083::GpioPortImpl<MockGpioRegs>;

// Second mock register set — used for PortB behavioural tests.  Having
// separate storage ensures the PortB template instantiation is exercised
// independently of PortA and that no register state leaks between the two.
static uint32_t mock_b_moder{0U};
static uint32_t mock_b_otyper{0U};
static uint32_t mock_b_ospeedr{0U};
static uint32_t mock_b_pupdr{0U};
static uint32_t mock_b_idr{0U};
static uint32_t mock_b_odr{0U};
static uint32_t mock_b_bsrr{0U};
static uint32_t mock_b_lckr{0U};
static uint32_t mock_b_afrl{0U};
static uint32_t mock_b_afrh{0U};
static uint32_t mock_b_brr{0U};

struct MockGpioBRegs {
  using Moder = ohal::test::MockRegister<uint32_t, &mock_b_moder>;
  using Otyper = ohal::test::MockRegister<uint32_t, &mock_b_otyper>;
  using Ospeedr = ohal::test::MockRegister<uint32_t, &mock_b_ospeedr>;
  using Pupdr = ohal::test::MockRegister<uint32_t, &mock_b_pupdr>;
  using Idr = ohal::test::MockRegister<uint32_t, &mock_b_idr>;
  using Odr = ohal::test::MockRegister<uint32_t, &mock_b_odr>;
  using Bsrr = ohal::test::MockRegister<uint32_t, &mock_b_bsrr>;
  using Lckr = ohal::test::MockRegister<uint32_t, &mock_b_lckr>;
  using Afrl = ohal::test::MockRegister<uint32_t, &mock_b_afrl>;
  using Afrh = ohal::test::MockRegister<uint32_t, &mock_b_afrh>;
  using Brr = ohal::test::MockRegister<uint32_t, &mock_b_brr>;
};

using MockPortBPin0 = ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<0U, MockGpioBRegs>;
using MockPortBPin15 = ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<15U, MockGpioBRegs>;

// ---------------------------------------------------------------------------
// Test fixture
// ---------------------------------------------------------------------------

class GpioStm32u083RctTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock_moder = 0U;
    mock_otyper = 0U;
    mock_ospeedr = 0U;
    mock_pupdr = 0U;
    mock_idr = 0U;
    mock_odr = 0U;
    mock_bsrr = 0U;
    mock_lckr = 0U;
    mock_afrl = 0U;
    mock_afrh = 0U;
    mock_brr = 0U;
    mock_b_moder = 0U;
    mock_b_otyper = 0U;
    mock_b_ospeedr = 0U;
    mock_b_pupdr = 0U;
    mock_b_idr = 0U;
    mock_b_odr = 0U;
    mock_b_bsrr = 0U;
    mock_b_lckr = 0U;
    mock_b_afrl = 0U;
    mock_b_afrh = 0U;
    mock_b_brr = 0U;
  }
};

// ---------------------------------------------------------------------------
// Basic behavioural tests
// ---------------------------------------------------------------------------

TEST_F(GpioStm32u083RctTest, Set_WritesBitToBsrr) {
  MockPin5::set();
  EXPECT_EQ(mock_bsrr, 1U << 5U);
}

TEST_F(GpioStm32u083RctTest, Clear_WritesBsrrResetBit) {
  MockPin5::clear();
  EXPECT_EQ(mock_bsrr, 1U << 21U);
}

TEST_F(GpioStm32u083RctTest, SetMode_Output_WritesModerBits) {
  MockPin5::set_mode(ohal::gpio::PinMode::Output);
  EXPECT_EQ(mock_moder & (0b11U << 10U), 0b01U << 10U);
}

TEST_F(GpioStm32u083RctTest, SetMode_Input_WritesModerBits) {
  mock_moder = 0b11U << 10U;
  MockPin5::set_mode(ohal::gpio::PinMode::Input);
  EXPECT_EQ(mock_moder & (0b11U << 10U), 0U);
}

TEST_F(GpioStm32u083RctTest, ReadInput_ReturnsHigh_WhenIdrBitSet) {
  mock_idr = 1U << 5U;
  EXPECT_EQ(MockPin5::read_input(), ohal::gpio::Level::High);
}

TEST_F(GpioStm32u083RctTest, PortSet_WritesMaskToBsrrLow16Bits) {
  MockPort::set(0x00FFU);
  EXPECT_EQ(mock_bsrr, 0x00FFU);
}

TEST_F(GpioStm32u083RctTest, PortClear_WritesMaskToBsrrHigh16Bits) {
  MockPort::clear(0x00FFU);
  EXPECT_EQ(mock_bsrr, 0x00FF0000U);
}

TEST_F(GpioStm32u083RctTest, PortWrite_CombinesSetAndClearMasksInSingleBsrrWrite) {
  MockPort::write(0x000FU, 0x00F0U);
  EXPECT_EQ(mock_bsrr, 0x00F0000FU);
}

// ---------------------------------------------------------------------------
// Behavioural tests: remaining GPIO methods on pin 5
// ---------------------------------------------------------------------------

TEST_F(GpioStm32u083RctTest, SetMode_AlternateFunction_WritesModerBits) {
  MockPin5::set_mode(ohal::gpio::PinMode::AlternateFunction);
  EXPECT_EQ(mock_moder & (0b11U << 10U), 0b10U << 10U);
}

TEST_F(GpioStm32u083RctTest, SetMode_Analog_WritesModerBits) {
  MockPin5::set_mode(ohal::gpio::PinMode::Analog);
  EXPECT_EQ(mock_moder & (0b11U << 10U), 0b11U << 10U);
}

TEST_F(GpioStm32u083RctTest, SetOutputType_PushPull_ClearsOtyperBit) {
  mock_otyper = 1U << 5U; // pre-load OpenDrain
  MockPin5::set_output_type(ohal::gpio::OutputType::PushPull);
  EXPECT_EQ((mock_otyper >> 5U) & 1U, 0U);
}

TEST_F(GpioStm32u083RctTest, SetOutputType_OpenDrain_SetsOtyperBit) {
  MockPin5::set_output_type(ohal::gpio::OutputType::OpenDrain);
  EXPECT_EQ((mock_otyper >> 5U) & 1U, 1U);
}

TEST_F(GpioStm32u083RctTest, SetOutputType_PreservesOtherOtyperBits) {
  mock_otyper = ~(1U << 5U); // all bits set except pin 5
  MockPin5::set_output_type(ohal::gpio::OutputType::OpenDrain);
  EXPECT_EQ(mock_otyper & ~(1U << 5U), ~(1U << 5U));
}

TEST_F(GpioStm32u083RctTest, SetSpeed_Low_WritesOspeedrBits) {
  mock_ospeedr = 0b11U << 10U; // pre-load VeryHigh
  MockPin5::set_speed(ohal::gpio::Speed::Low);
  EXPECT_EQ(mock_ospeedr & (0b11U << 10U), 0U);
}

TEST_F(GpioStm32u083RctTest, SetSpeed_VeryHigh_WritesOspeedrBits) {
  MockPin5::set_speed(ohal::gpio::Speed::VeryHigh);
  EXPECT_EQ(mock_ospeedr & (0b11U << 10U), 0b11U << 10U);
}

TEST_F(GpioStm32u083RctTest, SetSpeed_PreservesOtherOspeedrBits) {
  mock_ospeedr = ~(0b11U << 10U); // all bits set except pin 5's field
  MockPin5::set_speed(ohal::gpio::Speed::VeryHigh);
  EXPECT_EQ(mock_ospeedr & ~(0b11U << 10U), ~(0b11U << 10U));
}

TEST_F(GpioStm32u083RctTest, SetPull_None_WritesPupdrBits) {
  mock_pupdr = 0b11U << 10U; // pre-load PullDown
  MockPin5::set_pull(ohal::gpio::Pull::None);
  EXPECT_EQ(mock_pupdr & (0b11U << 10U), 0U);
}

TEST_F(GpioStm32u083RctTest, SetPull_Up_WritesPupdrBits) {
  MockPin5::set_pull(ohal::gpio::Pull::Up);
  EXPECT_EQ(mock_pupdr & (0b11U << 10U), 0b01U << 10U);
}

TEST_F(GpioStm32u083RctTest, SetPull_Down_WritesPupdrBits) {
  MockPin5::set_pull(ohal::gpio::Pull::Down);
  EXPECT_EQ(mock_pupdr & (0b11U << 10U), 0b10U << 10U);
}

TEST_F(GpioStm32u083RctTest, SetPull_PreservesOtherPupdrBits) {
  mock_pupdr = ~(0b11U << 10U); // all bits set except pin 5's field
  MockPin5::set_pull(ohal::gpio::Pull::Up);
  EXPECT_EQ(mock_pupdr & ~(0b11U << 10U), ~(0b11U << 10U));
}

TEST_F(GpioStm32u083RctTest, ReadOutput_ReturnsHigh_WhenOdrBitSet) {
  mock_odr = 1U << 5U;
  EXPECT_EQ(MockPin5::read_output(), ohal::gpio::Level::High);
}

TEST_F(GpioStm32u083RctTest, ReadOutput_ReturnsLow_WhenOdrBitClear) {
  mock_odr = 0U;
  EXPECT_EQ(MockPin5::read_output(), ohal::gpio::Level::Low);
}

TEST_F(GpioStm32u083RctTest, Toggle_SetsBsrrSetBit_WhenOutputWasLow) {
  mock_odr = 0U; // pin 5 output is Low
  MockPin5::toggle();
  EXPECT_EQ(mock_bsrr, 1U << 5U); // BSRR set bit written
}

TEST_F(GpioStm32u083RctTest, Toggle_SetsBsrrResetBit_WhenOutputWasHigh) {
  mock_odr = 1U << 5U; // pin 5 output is High
  MockPin5::toggle();
  EXPECT_EQ(mock_bsrr, 1U << 21U); // BSRR reset bit written
}

// ---------------------------------------------------------------------------
// Boundary pin 0: verify bit-0 position in each register field
// ---------------------------------------------------------------------------

TEST_F(GpioStm32u083RctTest, Pin0_Set_WritesBit0ToBsrr) {
  MockPin0::set();
  EXPECT_EQ(mock_bsrr, 1U);
}

TEST_F(GpioStm32u083RctTest, Pin0_Clear_WritesBit16ToBsrr) {
  MockPin0::clear();
  EXPECT_EQ(mock_bsrr, 1U << 16U);
}

TEST_F(GpioStm32u083RctTest, Pin0_ReadInput_ReturnsHigh_WhenIdrBit0Set) {
  mock_idr = 1U;
  EXPECT_EQ(MockPin0::read_input(), ohal::gpio::Level::High);
}

TEST_F(GpioStm32u083RctTest, Pin0_ReadOutput_ReturnsHigh_WhenOdrBit0Set) {
  mock_odr = 1U;
  EXPECT_EQ(MockPin0::read_output(), ohal::gpio::Level::High);
}

TEST_F(GpioStm32u083RctTest, Pin0_SetMode_Output_WritesBits1To0InModer) {
  MockPin0::set_mode(ohal::gpio::PinMode::Output);
  EXPECT_EQ(mock_moder & 0b11U, 0b01U);
}

TEST_F(GpioStm32u083RctTest, Pin0_Toggle_SetsBsrrSetBit_WhenOutputWasLow) {
  mock_odr = 0U;
  MockPin0::toggle();
  EXPECT_EQ(mock_bsrr, 1U);
}

// ---------------------------------------------------------------------------
// Boundary pin 15: verify bit-15 position in each register field
// ---------------------------------------------------------------------------

TEST_F(GpioStm32u083RctTest, Pin15_Set_WritesBit15ToBsrr) {
  MockPin15::set();
  EXPECT_EQ(mock_bsrr, 1U << 15U);
}

TEST_F(GpioStm32u083RctTest, Pin15_Clear_WritesBit31ToBsrr) {
  MockPin15::clear();
  EXPECT_EQ(mock_bsrr, 1U << 31U);
}

TEST_F(GpioStm32u083RctTest, Pin15_ReadInput_ReturnsHigh_WhenIdrBit15Set) {
  mock_idr = 1U << 15U;
  EXPECT_EQ(MockPin15::read_input(), ohal::gpio::Level::High);
}

TEST_F(GpioStm32u083RctTest, Pin15_ReadOutput_ReturnsHigh_WhenOdrBit15Set) {
  mock_odr = 1U << 15U;
  EXPECT_EQ(MockPin15::read_output(), ohal::gpio::Level::High);
}

TEST_F(GpioStm32u083RctTest, Pin15_SetMode_Output_WritesBits31To30InModer) {
  MockPin15::set_mode(ohal::gpio::PinMode::Output);
  EXPECT_EQ(mock_moder & (0b11U << 30U), 0b01U << 30U);
}

TEST_F(GpioStm32u083RctTest, Pin15_Toggle_SetsBsrrSetBit_WhenOutputWasLow) {
  mock_odr = 0U;
  MockPin15::toggle();
  EXPECT_EQ(mock_bsrr, 1U << 15U);
}

// ---------------------------------------------------------------------------
// PortB behavioural tests: exercise the same template with MockGpioBRegs
// (separate backing storage confirms there is no cross-port state sharing).
// ---------------------------------------------------------------------------

TEST_F(GpioStm32u083RctTest, PortBPin0_Set_WritesBit0ToBBsrr) {
  MockPortBPin0::set();
  EXPECT_EQ(mock_b_bsrr, 1U);
}

TEST_F(GpioStm32u083RctTest, PortBPin0_Clear_WritesBit16ToBBsrr) {
  MockPortBPin0::clear();
  EXPECT_EQ(mock_b_bsrr, 1U << 16U);
}

TEST_F(GpioStm32u083RctTest, PortBPin0_SetMode_Output_WritesBitsToB) {
  MockPortBPin0::set_mode(ohal::gpio::PinMode::Output);
  EXPECT_EQ(mock_b_moder & 0b11U, 0b01U);
}

TEST_F(GpioStm32u083RctTest, PortBPin0_SetMode_DoesNotAffectPortA) {
  MockPortBPin0::set_mode(ohal::gpio::PinMode::Output);
  EXPECT_EQ(mock_moder, 0U);
}

TEST_F(GpioStm32u083RctTest, PortBPin15_Set_WritesBit15ToBBsrr) {
  MockPortBPin15::set();
  EXPECT_EQ(mock_b_bsrr, 1U << 15U);
}

TEST_F(GpioStm32u083RctTest, PortBPin15_ReadOutput_ReturnsHigh_WhenOdrBit15Set) {
  mock_b_odr = 1U << 15U;
  EXPECT_EQ(MockPortBPin15::read_output(), ohal::gpio::Level::High);
}

// ---------------------------------------------------------------------------
// Capability trait tests: bonded-out ports (A, B, C, D, F) must report true.
// ---------------------------------------------------------------------------

static_assert(ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 0>::value,
              "PortA must support output type on RCT");
static_assert(ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortA, 0>::value,
              "PortA must support output speed on RCT");
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 0>::value,
              "PortA must support pull on RCT");
static_assert(ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortA, 0>::value,
              "PortA must support alternate function on RCT");

static_assert(ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortD, 2>::value,
              "PortD must support output type on RCT");
static_assert(ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortD, 2>::value,
              "PortD must support output speed on RCT");
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortD, 2>::value,
              "PortD must support pull on RCT");
static_assert(ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortD, 2>::value,
              "PortD must support alternate function on RCT");

static_assert(ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortF, 3>::value,
              "PortF must support output type on RCT");
static_assert(ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortF, 3>::value,
              "PortF must support output speed on RCT");
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortF, 3>::value,
              "PortF must support pull on RCT");
static_assert(ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortF, 3>::value,
              "PortF must support alternate function on RCT");

// Out-of-range pin numbers must report false even on bonded-out ports.
static_assert(!ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 16>::value,
              "PortA pin 16 (out of range) must not report supports_output_type on RCT");
static_assert(!ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortA, 16>::value,
              "PortA pin 16 (out of range) must not report supports_output_speed on RCT");
static_assert(!ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 16>::value,
              "PortA pin 16 (out of range) must not report supports_pull on RCT");
static_assert(!ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortA, 16>::value,
              "PortA pin 16 (out of range) must not report supports_alternate_function on RCT");
static_assert(!ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortD, 16>::value,
              "PortD pin 16 (out of range) must not report supports_output_type on RCT");
static_assert(!ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortD, 16>::value,
              "PortD pin 16 (out of range) must not report supports_output_speed on RCT");
static_assert(!ohal::gpio::capabilities::supports_pull<ohal::gpio::PortD, 16>::value,
              "PortD pin 16 (out of range) must not report supports_pull on RCT");
static_assert(!ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortD, 16>::value,
              "PortD pin 16 (out of range) must not report supports_alternate_function on RCT");

// Non-bonded pins within partially-bonded ports.
// GPIOD: only PD2 bonded out — PD0 is not present on this package.
static_assert(!ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortD, 0>::value,
              "PortD pin 0 (not bonded) must not report supports_output_type on RCT");
static_assert(!ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortD, 0>::value,
              "PortD pin 0 (not bonded) must not report supports_output_speed on RCT");
static_assert(!ohal::gpio::capabilities::supports_pull<ohal::gpio::PortD, 0>::value,
              "PortD pin 0 (not bonded) must not report supports_pull on RCT");
static_assert(!ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortD, 0>::value,
              "PortD pin 0 (not bonded) must not report supports_alternate_function on RCT");
// GPIOF: only PF0–PF3 bonded out — PF4 is not present on this package.
static_assert(!ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortF, 4>::value,
              "PortF pin 4 (not bonded) must not report supports_output_type on RCT");
static_assert(!ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortF, 4>::value,
              "PortF pin 4 (not bonded) must not report supports_output_speed on RCT");
static_assert(!ohal::gpio::capabilities::supports_pull<ohal::gpio::PortF, 4>::value,
              "PortF pin 4 (not bonded) must not report supports_pull on RCT");
static_assert(!ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortF, 4>::value,
              "PortF pin 4 (not bonded) must not report supports_alternate_function on RCT");

// PortE is not bonded out on the 64-pin LQFP package.
static_assert(!ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortE, 0>::value,
              "PortE must not report supports_output_type (not bonded out on RCT)");
static_assert(!ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortE, 0>::value,
              "PortE must not report supports_output_speed (not bonded out on RCT)");
static_assert(!ohal::gpio::capabilities::supports_pull<ohal::gpio::PortE, 0>::value,
              "PortE must not report supports_pull (not bonded out on RCT)");
static_assert(!ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortE, 0>::value,
              "PortE must not report supports_alternate_function (not bonded out on RCT)");

struct CapCase {
  bool value;
  const char* name;
};

class GpioStm32u083RctCapabilityTest : public ::testing::TestWithParam<CapCase> {};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    BondedPortCapabilities, GpioStm32u083RctCapabilityTest,
    ::testing::Values(
        CapCase{ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 0>::value,
                "PortA_Pin0_OutputType"},
        CapCase{ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortA, 0>::value,
                "PortA_Pin0_OutputSpeed"},
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 0>::value,
                "PortA_Pin0_Pull"},
        CapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortA, 0>::value,
                "PortA_Pin0_AlternateFunction"},
        CapCase{ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortB, 7>::value,
                "PortB_Pin7_OutputType"},
        CapCase{ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortC, 3>::value,
                "PortC_Pin3_OutputSpeed"},
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortD, 2>::value,
                "PortD_Pin2_Pull"},
        CapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortD, 2>::value,
                "PortD_Pin2_AlternateFunction"},
        CapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortF, 3>::value,
                "PortF_Pin3_AlternateFunction"}),
    [](const ::testing::TestParamInfo<CapCase>& info) { return info.param.name; });
// clang-format on

TEST_P(GpioStm32u083RctCapabilityTest, CapabilityIsTrue) { EXPECT_TRUE(GetParam().value); }

struct InvalidCapCase {
  bool value;
  const char* name;
};

class GpioStm32u083RctInvalidCapTest : public ::testing::TestWithParam<InvalidCapCase> {};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    UnbondedAndOutOfRangeCapabilities, GpioStm32u083RctInvalidCapTest,
    ::testing::Values(
        // Out-of-range pin on a bonded port
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 16>::value,
            "PortA_Pin16_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortA, 16>::value,
            "PortA_Pin16_OutputSpeed"},
        InvalidCapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 16>::value,
                       "PortA_Pin16_Pull"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortA, 16>::value,
            "PortA_Pin16_AlternateFunction"},
        // Out-of-range pin on PortD (bonded out, but pin 16 is beyond the 0–15 range)
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortD, 16>::value,
            "PortD_Pin16_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortD, 16>::value,
            "PortD_Pin16_OutputSpeed"},
        InvalidCapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortD, 16>::value,
                       "PortD_Pin16_Pull"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortD, 16>::value,
            "PortD_Pin16_AlternateFunction"},
        // PortE — not bonded out on the RCT 64-pin LQFP package
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortE, 0>::value,
            "PortE_Pin0_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortE, 0>::value,
            "PortE_Pin0_OutputSpeed"},
        InvalidCapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortE, 0>::value,
                       "PortE_Pin0_Pull"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortE, 0>::value,
            "PortE_Pin0_AlternateFunction"},
        // Non-bonded pins within partially-bonded PortD (only PD2 present)
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortD, 0>::value,
            "PortD_Pin0_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortD, 0>::value,
            "PortD_Pin0_AlternateFunction"},
        // Non-bonded pins within partially-bonded PortF (only PF0–PF3 present)
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortF, 4>::value,
            "PortF_Pin4_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortF, 4>::value,
            "PortF_Pin4_AlternateFunction"}),
    [](const ::testing::TestParamInfo<InvalidCapCase>& info) { return info.param.name; });
// clang-format on

TEST_P(GpioStm32u083RctInvalidCapTest, CapabilityIsFalse) { EXPECT_FALSE(GetParam().value); }

// ---------------------------------------------------------------------------
// Port-wiring tests: verify that Pin<PortX, N> resolves to the correct MMIO
// base address for the STM32U083RCT package.
// ---------------------------------------------------------------------------

namespace wiring = ohal::platforms::stm32u0::stm32u083;

static_assert(ohal::gpio::Pin<ohal::gpio::PortA, 0>::BsrrSet::reg_type::address ==
                  wiring::kGpioABase + wiring::kBsrrOffset,
              "Pin<PortA,0> must use GPIOA BSRR address on RCT");

static_assert(ohal::gpio::Pin<ohal::gpio::PortB, 0>::BsrrSet::reg_type::address ==
                  wiring::kGpioBBase + wiring::kBsrrOffset,
              "Pin<PortB,0> must use GPIOB BSRR address on RCT");

static_assert(ohal::gpio::Pin<ohal::gpio::PortC, 0>::BsrrSet::reg_type::address ==
                  wiring::kGpioCBase + wiring::kBsrrOffset,
              "Pin<PortC,0> must use GPIOC BSRR address on RCT");

static_assert(ohal::gpio::Pin<ohal::gpio::PortD, 2>::BsrrSet::reg_type::address ==
                  wiring::kGpioDBase + wiring::kBsrrOffset,
              "Pin<PortD,2> must use GPIOD BSRR address on RCT");

static_assert(ohal::gpio::Pin<ohal::gpio::PortF, 0>::BsrrSet::reg_type::address ==
                  wiring::kGpioFBase + wiring::kBsrrOffset,
              "Pin<PortF,0> must use GPIOF BSRR address on RCT");

static_assert(ohal::gpio::Port<ohal::gpio::PortA>::BsrrReg::address ==
                  wiring::kGpioABase + wiring::kBsrrOffset,
              "Port<PortA> must use GPIOA BSRR address on RCT");

static_assert(ohal::gpio::Port<ohal::gpio::PortD>::BsrrReg::address ==
                  wiring::kGpioDBase + wiring::kBsrrOffset,
              "Port<PortD> must use GPIOD BSRR address on RCT");

static_assert(ohal::gpio::Port<ohal::gpio::PortF>::BsrrReg::address ==
                  wiring::kGpioFBase + wiring::kBsrrOffset,
              "Port<PortF> must use GPIOF BSRR address on RCT");

struct WiringCase {
  uintptr_t actual;
  uintptr_t expected;
  const char* name;
};

class GpioStm32u083RctWiringTest : public ::testing::TestWithParam<WiringCase> {};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    PortBsrrAddresses, GpioStm32u083RctWiringTest,
    ::testing::Values(
        WiringCase{ohal::gpio::Pin<ohal::gpio::PortA, 0>::BsrrSet::reg_type::address,
                   wiring::kGpioABase + wiring::kBsrrOffset, "PortA"},
        WiringCase{ohal::gpio::Pin<ohal::gpio::PortB, 0>::BsrrSet::reg_type::address,
                   wiring::kGpioBBase + wiring::kBsrrOffset, "PortB"},
        WiringCase{ohal::gpio::Pin<ohal::gpio::PortC, 0>::BsrrSet::reg_type::address,
                   wiring::kGpioCBase + wiring::kBsrrOffset, "PortC"},
        WiringCase{ohal::gpio::Pin<ohal::gpio::PortD, 2>::BsrrSet::reg_type::address,
                   wiring::kGpioDBase + wiring::kBsrrOffset, "PortD"},
        WiringCase{ohal::gpio::Pin<ohal::gpio::PortF, 0>::BsrrSet::reg_type::address,
                   wiring::kGpioFBase + wiring::kBsrrOffset, "PortF"}),
    [](const ::testing::TestParamInfo<WiringCase>& info) { return info.param.name; });
// clang-format on

TEST_P(GpioStm32u083RctWiringTest, BsrrAddressMatchesHardwareBase) {
  EXPECT_EQ(GetParam().actual, GetParam().expected);
}

// ---------------------------------------------------------------------------
// Consumer API integration: typical GPIO init + toggle sequence.
//
// consumer_api_init_toggle_never_called() demonstrates that the canonical
// usage pattern — configure a pin then toggle it — compiles correctly through
// the public ohal::gpio::Pin<> interface when the STM32U083RCT platform
// headers are active.  The function is never invoked at run time (it would
// write to real hardware MMIO addresses), but the compiler fully instantiates
// it, so any type-level or static_assert errors would be caught at build time.
// ---------------------------------------------------------------------------

[[maybe_unused]] static void consumer_api_init_toggle_never_called() {
  using namespace ohal::gpio;
  Pin<PortA, 5>::set_mode(PinMode::Output);
  Pin<PortA, 5>::set_output_type(OutputType::PushPull);
  Pin<PortA, 5>::set_speed(Speed::VeryHigh);
  Pin<PortA, 5>::set_pull(Pull::None);
  Pin<PortA, 5>::set();
  Pin<PortA, 5>::toggle();
  Pin<PortA, 5>::clear();
  (void)Pin<PortA, 5>::read_input();
  (void)Pin<PortA, 5>::read_output();
}

// Runtime test: verify that Pin<PortA, 5> derives from the expected concrete type so
// the consumer API above uses the real STM32U083 implementation.
TEST(Stm32u083RctConsumerApiTest, LedPin_DerivesFromStm32u083GpioPortPinImpl) {
  EXPECT_TRUE((std::is_base_of_v<ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<
                                     5U, ohal::platforms::stm32u0::stm32u083::GpioA>,
                                 ohal::gpio::Pin<ohal::gpio::PortA, 5>>));
}

} // namespace
