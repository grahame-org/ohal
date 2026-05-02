#include "mock/mock_register.hpp"

#include <ohal/core/capabilities.hpp>
#include <ohal/core/field.hpp>
#include <ohal/gpio.hpp>
#include <ohal/platforms/stm32u0/models/stm32u083/capabilities.hpp>
#include <ohal/platforms/stm32u0/models/stm32u083/gpio.hpp>

#include <cstdint>

#include <gtest/gtest.h>

// ---------------------------------------------------------------------------
// Mock register set for a single STM32U083 GPIO port.
//
// Each backing variable corresponds to one physical register in GpioPortRegs.
// Static storage duration is required by MockRegister<T, T* Storage>.
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

/// A mock GPIO port register set with the same nested type aliases as
/// GpioPortRegs<Base>, but backed by plain in-memory variables instead of
/// volatile MMIO addresses.  Passed as the Regs template argument to
/// GpioPortPinImpl in all tests below.
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

// Convenience alias: pin 5 backed by MockGpioRegs.
// Tests exercise GpioPortPinImpl directly so that host builds need no
// hardware addresses; the real Pin<PortA, 5> delegates to the same code path.
using MockPin5 = ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<5U, MockGpioRegs>;
using MockPin0 = ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<0U, MockGpioRegs>;
using MockPin15 = ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<15U, MockGpioRegs>;

// ---------------------------------------------------------------------------
// Test fixture: resets every mock register before each test.
// ---------------------------------------------------------------------------

class GpioStm32u083Test : public ::testing::Test {
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
  }
};

// ---------------------------------------------------------------------------
// set() — writes 1u << PinNum to BSRR
// ---------------------------------------------------------------------------

TEST_F(GpioStm32u083Test, Set_WritesBitToBsrr) {
  MockPin5::set();
  EXPECT_EQ(mock_bsrr, 1U << 5U);
}

TEST_F(GpioStm32u083Test, Set_DoesNotModifyOtherRegisters) {
  MockPin5::set();
  EXPECT_EQ(mock_moder, 0U);
}

// ---------------------------------------------------------------------------
// clear() — writes 1u << (PinNum + 16) to BSRR
// ---------------------------------------------------------------------------

TEST_F(GpioStm32u083Test, Clear_WritesBsrrResetBit) {
  MockPin5::clear();
  EXPECT_EQ(mock_bsrr, 1U << 21U);
}

TEST_F(GpioStm32u083Test, ClearPin0_WritesBsrrResetBit) {
  MockPin0::clear();
  EXPECT_EQ(mock_bsrr, 1U << 16U);
}

TEST_F(GpioStm32u083Test, ClearPin15_WritesBsrrResetBit) {
  MockPin15::clear();
  EXPECT_EQ(mock_bsrr, 1U << 31U);
}

// ---------------------------------------------------------------------------
// set_mode() — writes 2-bit PinMode value to MODER at offset PinNum*2
// ---------------------------------------------------------------------------

// Pin 5: MODER bits [11:10].  Output = 0b01.
TEST_F(GpioStm32u083Test, SetMode_Output_WritesModer) {
  MockPin5::set_mode(ohal::gpio::PinMode::Output);
  EXPECT_EQ(mock_moder & (0b11U << 10U), 0b01U << 10U);
}

TEST_F(GpioStm32u083Test, SetMode_Output_PreservesOtherModerBits) {
  mock_moder = ~(0b11U << 10U); // all ones except bits [11:10]
  MockPin5::set_mode(ohal::gpio::PinMode::Output);
  EXPECT_EQ(mock_moder & ~(0b11U << 10U), ~(0b11U << 10U));
}

TEST_F(GpioStm32u083Test, SetMode_Input_WritesModer) {
  mock_moder = 0b11U << 10U; // pre-load Output
  MockPin5::set_mode(ohal::gpio::PinMode::Input);
  EXPECT_EQ(mock_moder & (0b11U << 10U), 0U);
}

TEST_F(GpioStm32u083Test, SetMode_AlternateFunction_WritesModer) {
  MockPin5::set_mode(ohal::gpio::PinMode::AlternateFunction);
  EXPECT_EQ(mock_moder & (0b11U << 10U), 0b10U << 10U);
}

TEST_F(GpioStm32u083Test, SetMode_Analog_WritesModer) {
  MockPin5::set_mode(ohal::gpio::PinMode::Analog);
  EXPECT_EQ(mock_moder & (0b11U << 10U), 0b11U << 10U);
}

// ---------------------------------------------------------------------------
// set_output_type() — writes 1-bit OutputType value to OTYPER at offset PinNum
// ---------------------------------------------------------------------------

TEST_F(GpioStm32u083Test, SetOutputType_OpenDrain_WritesOtyper) {
  MockPin5::set_output_type(ohal::gpio::OutputType::OpenDrain);
  EXPECT_EQ(mock_otyper & (1U << 5U), 1U << 5U);
}

TEST_F(GpioStm32u083Test, SetOutputType_PushPull_WritesOtyper) {
  mock_otyper = 1U << 5U; // pre-load OpenDrain
  MockPin5::set_output_type(ohal::gpio::OutputType::PushPull);
  EXPECT_EQ(mock_otyper & (1U << 5U), 0U);
}

// ---------------------------------------------------------------------------
// set_speed() — writes 2-bit Speed value to OSPEEDR at offset PinNum*2
// ---------------------------------------------------------------------------

TEST_F(GpioStm32u083Test, SetSpeed_VeryHigh_WritesOspeedr) {
  MockPin5::set_speed(ohal::gpio::Speed::VeryHigh);
  EXPECT_EQ(mock_ospeedr & (0b11U << 10U), 0b11U << 10U);
}

TEST_F(GpioStm32u083Test, SetSpeed_Low_WritesOspeedr) {
  mock_ospeedr = 0b11U << 10U; // pre-load VeryHigh
  MockPin5::set_speed(ohal::gpio::Speed::Low);
  EXPECT_EQ(mock_ospeedr & (0b11U << 10U), 0U);
}

// ---------------------------------------------------------------------------
// set_pull() — writes 2-bit Pull value to PUPDR at offset PinNum*2
// ---------------------------------------------------------------------------

TEST_F(GpioStm32u083Test, SetPull_Up_WritesPupdr) {
  MockPin5::set_pull(ohal::gpio::Pull::Up);
  EXPECT_EQ(mock_pupdr & (0b11U << 10U), 0b01U << 10U);
}

TEST_F(GpioStm32u083Test, SetPull_Down_WritesPupdr) {
  MockPin5::set_pull(ohal::gpio::Pull::Down);
  EXPECT_EQ(mock_pupdr & (0b11U << 10U), 0b10U << 10U);
}

TEST_F(GpioStm32u083Test, SetPull_None_WritesPupdr) {
  mock_pupdr = 0b11U << 10U; // pre-load Down
  MockPin5::set_pull(ohal::gpio::Pull::None);
  EXPECT_EQ(mock_pupdr & (0b11U << 10U), 0U);
}

// ---------------------------------------------------------------------------
// read_input() — reads bit PinNum from IDR
// ---------------------------------------------------------------------------

TEST_F(GpioStm32u083Test, ReadInput_ReturnsHigh_WhenIdrBitSet) {
  mock_idr = 1U << 5U;
  EXPECT_EQ(MockPin5::read_input(), ohal::gpio::Level::High);
}

TEST_F(GpioStm32u083Test, ReadInput_ReturnsLow_WhenIdrBitClear) {
  mock_idr = 0U;
  EXPECT_EQ(MockPin5::read_input(), ohal::gpio::Level::Low);
}

TEST_F(GpioStm32u083Test, ReadInput_IgnoresOtherIdrBits) {
  mock_idr = ~(1U << 5U); // all bits set except pin 5
  EXPECT_EQ(MockPin5::read_input(), ohal::gpio::Level::Low);
}

// ---------------------------------------------------------------------------
// read_output() — reads bit PinNum from ODR
// ---------------------------------------------------------------------------

TEST_F(GpioStm32u083Test, ReadOutput_ReturnsHigh_WhenOdrBitSet) {
  mock_odr = 1U << 5U;
  EXPECT_EQ(MockPin5::read_output(), ohal::gpio::Level::High);
}

TEST_F(GpioStm32u083Test, ReadOutput_ReturnsLow_WhenOdrBitClear) {
  mock_odr = 0U;
  EXPECT_EQ(MockPin5::read_output(), ohal::gpio::Level::Low);
}

// ---------------------------------------------------------------------------
// toggle() — reads ODR then drives opposite level via BSRR
// ---------------------------------------------------------------------------

TEST_F(GpioStm32u083Test, Toggle_SetsPin_WhenOutputWasLow) {
  mock_odr = 0U; // pin 5 is Low
  MockPin5::toggle();
  EXPECT_EQ(mock_bsrr, 1U << 5U);
}

TEST_F(GpioStm32u083Test, Toggle_ClearsPin_WhenOutputWasHigh) {
  mock_odr = 1U << 5U; // pin 5 is High
  MockPin5::toggle();
  EXPECT_EQ(mock_bsrr, 1U << 21U);
}

// ---------------------------------------------------------------------------
// Boundary pins: verify pin 0 and pin 15 use the correct bit positions
// ---------------------------------------------------------------------------

TEST_F(GpioStm32u083Test, SetPin0_WritesBit0ToBsrr) {
  MockPin0::set();
  EXPECT_EQ(mock_bsrr, 1U << 0U);
}

TEST_F(GpioStm32u083Test, SetPin15_WritesBit15ToBsrr) {
  MockPin15::set();
  EXPECT_EQ(mock_bsrr, 1U << 15U);
}

TEST_F(GpioStm32u083Test, SetModePin0_WritesModer) {
  MockPin0::set_mode(ohal::gpio::PinMode::Output);
  EXPECT_EQ(mock_moder & 0b11U, 0b01U);
}

TEST_F(GpioStm32u083Test, SetModePin15_WritesModer) {
  MockPin15::set_mode(ohal::gpio::PinMode::Output);
  EXPECT_EQ(mock_moder & (0b11U << 30U), 0b01U << 30U);
}

// ---------------------------------------------------------------------------
// Capability trait tests for STM32U083 GPIO pins
// ---------------------------------------------------------------------------

static_assert(ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 0>::value,
              "PortA must support output type");
static_assert(ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortA, 0>::value,
              "PortA must support output speed");
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 0>::value,
              "PortA must support pull");
static_assert(ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortA, 0>::value,
              "PortA must support alternate function");

static_assert(ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortF, 15>::value,
              "PortF must support output type");
static_assert(ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortF, 15>::value,
              "PortF must support output speed");
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortF, 15>::value,
              "PortF must support pull");
static_assert(ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortF, 15>::value,
              "PortF must support alternate function");

struct CapCase {
  bool value;
  const char* name;
};

class GpioStm32u083CapabilityTest : public ::testing::TestWithParam<CapCase> {};

INSTANTIATE_TEST_SUITE_P(
    AllCapabilities, GpioStm32u083CapabilityTest,
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
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortD, 11>::value,
                "PortD_Pin11_Pull"},
        CapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortF, 15>::value,
                "PortF_Pin15_AlternateFunction"}),
    [](const ::testing::TestParamInfo<CapCase>& info) { return info.param.name; });

TEST_P(GpioStm32u083CapabilityTest, CapabilityIsTrue) { EXPECT_TRUE(GetParam().value); }

// ---------------------------------------------------------------------------
// Negative-compile tests (manual verification only)
// ---------------------------------------------------------------------------
//
// Writing to a read-only field must not compile.  Verify by temporarily
// uncommenting the lines below and confirming a static_assert fires:
//
//   MockPin5::Idr::write(ohal::gpio::Level::High);
//   // expected: "ohal: cannot write to a read-only field"

} // namespace
