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

// Convenience alias: port backed by MockGpioRegs.
// Tests exercise GpioPortImpl directly; the real Port<PortA> delegates to the
// same code path.
using MockPort = ohal::platforms::stm32u0::stm32u083::GpioPortImpl<MockGpioRegs>;

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

TEST_F(GpioStm32u083Test, Set_DoesNotModifyModer) {
  MockPin5::set();
  EXPECT_EQ(mock_moder, 0U);
}

// ---------------------------------------------------------------------------
// clear() — writes 1u << (PinNum + 16) to BSRR; parametrised over boundary pins
// ---------------------------------------------------------------------------

template <uint8_t PinNum>
struct PinClearConfig {
  using PinType = ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<PinNum, MockGpioRegs>;
  static constexpr uint32_t expected_bsrr = 1U << (PinNum + 16U);
};

template <typename Config>
class GpioStm32u083ClearBoundaryTest : public ::testing::Test {
protected:
  void SetUp() override { mock_bsrr = 0U; }
};

using ClearBoundaryConfigs =
    ::testing::Types<PinClearConfig<0U>, PinClearConfig<5U>, PinClearConfig<15U>>;
TYPED_TEST_SUITE(GpioStm32u083ClearBoundaryTest, ClearBoundaryConfigs);

TYPED_TEST(GpioStm32u083ClearBoundaryTest, WritesBsrrResetBit) {
  TypeParam::PinType::clear();
  EXPECT_EQ(mock_bsrr, TypeParam::expected_bsrr);
}

// ---------------------------------------------------------------------------
// Port::set() — writes uint16_t mask to BSRR bits 0-15
// ---------------------------------------------------------------------------

TEST_F(GpioStm32u083Test, PortSet_WritesMaskToBsrrLow16Bits) {
  MockPort::set(0x00FFU);
  EXPECT_EQ(mock_bsrr, 0x00FFU);
}

TEST_F(GpioStm32u083Test, PortSet_AllPins_WritesMaskToBsrr) {
  MockPort::set(0xFFFFU);
  EXPECT_EQ(mock_bsrr, 0xFFFFU);
}

TEST_F(GpioStm32u083Test, PortSet_DoesNotModifyModer) {
  MockPort::set(0xFFFFU);
  EXPECT_EQ(mock_moder, 0U);
}

// ---------------------------------------------------------------------------
// Port::clear() — writes uint16_t mask to BSRR bits 16-31
// ---------------------------------------------------------------------------

TEST_F(GpioStm32u083Test, PortClear_WritesMaskToBsrrHigh16Bits) {
  MockPort::clear(0x00FFU);
  EXPECT_EQ(mock_bsrr, 0x00FF0000U);
}

TEST_F(GpioStm32u083Test, PortClear_AllPins_WritesMaskToBsrr) {
  MockPort::clear(0xFFFFU);
  EXPECT_EQ(mock_bsrr, 0xFFFF0000U);
}

TEST_F(GpioStm32u083Test, PortClear_DoesNotModifyModer) {
  MockPort::clear(0xFFFFU);
  EXPECT_EQ(mock_moder, 0U);
}

// ---------------------------------------------------------------------------
// Port::write() — writes combined set/clear mask to BSRR in a single store
// ---------------------------------------------------------------------------

TEST_F(GpioStm32u083Test, PortWrite_CombinesSetAndClearMasksInSingleBsrrWrite) {
  MockPort::write(0x000FU, 0x00F0U);
  EXPECT_EQ(mock_bsrr, 0x00F0000FU);
}

TEST_F(GpioStm32u083Test, PortWrite_OnlySetMask_WritesMaskToBsrrLow16Bits) {
  MockPort::write(0xABCDU, 0x0000U);
  EXPECT_EQ(mock_bsrr, 0x0000ABCDU);
}

TEST_F(GpioStm32u083Test, PortWrite_OnlyClearMask_WritesMaskToBsrrHigh16Bits) {
  MockPort::write(0x0000U, 0xABCDU);
  EXPECT_EQ(mock_bsrr, 0xABCD0000U);
}

TEST_F(GpioStm32u083Test, PortWrite_DoesNotModifyModer) {
  MockPort::write(0xFFFFU, 0xFFFFU);
  EXPECT_EQ(mock_moder, 0U);
}

// ---------------------------------------------------------------------------
// set_mode() — writes 2-bit PinMode value to MODER at offset PinNum*2
// ---------------------------------------------------------------------------

struct SetModeCase {
  ohal::gpio::PinMode mode;
  uint32_t moder_preload;  ///< initial MODER bits [11:10] before set_mode()
  uint32_t expected_field; ///< expected MODER bits [11:10] after set_mode()
  const char* name;
};

class GpioStm32u083SetModeTest : public ::testing::TestWithParam<SetModeCase> {
protected:
  // SetUp is called after the test parameter is bound, so GetParam() is valid here.
  void SetUp() override { mock_moder = GetParam().moder_preload; }
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    PinModes, GpioStm32u083SetModeTest,
    ::testing::Values(
        SetModeCase{ohal::gpio::PinMode::Output,            0U,           0b01U << 10U, "Output"},
        SetModeCase{ohal::gpio::PinMode::Input,             0b11U << 10U, 0U,           "Input"},
        SetModeCase{ohal::gpio::PinMode::AlternateFunction, 0U,           0b10U << 10U, "AlternateFunction"},
        SetModeCase{ohal::gpio::PinMode::Analog,            0U,           0b11U << 10U, "Analog"}),
    [](const ::testing::TestParamInfo<SetModeCase>& info) { return info.param.name; });
// clang-format on

TEST_P(GpioStm32u083SetModeTest, WritesCorrectModerBits) {
  MockPin5::set_mode(GetParam().mode);
  EXPECT_EQ(mock_moder & (0b11U << 10U), GetParam().expected_field);
}

// Pin 5: MODER bits [11:10] — verify that writing any mode preserves the other bits.
TEST_F(GpioStm32u083Test, SetMode_Output_OtherModerBitsPreserved) {
  mock_moder = ~(0b11U << 10U); // all ones except bits [11:10]
  MockPin5::set_mode(ohal::gpio::PinMode::Output);
  EXPECT_EQ(mock_moder & ~(0b11U << 10U), ~(0b11U << 10U));
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
  mock_pupdr = 0b11U << 10U; // pre-load with non-zero bits (0b11 is reserved; Down = 0b10)
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

// ---------------------------------------------------------------------------
// set() and clear() must never call read() on BSRR (write-only register)
//
// Uses ReadCountingMockRegister for the BSRR slot so that any accidental
// read-modify-write regression is caught on the host without needing real hardware.
// ---------------------------------------------------------------------------

static uint32_t bsrr_counting_storage{0U};
using BsrrCountingReg = ohal::test::ReadCountingMockRegister<uint32_t, &bsrr_counting_storage>;

struct MockGpioRegsCountingBsrr {
  using Moder = ohal::test::MockRegister<uint32_t, &mock_moder>;
  using Otyper = ohal::test::MockRegister<uint32_t, &mock_otyper>;
  using Ospeedr = ohal::test::MockRegister<uint32_t, &mock_ospeedr>;
  using Pupdr = ohal::test::MockRegister<uint32_t, &mock_pupdr>;
  using Idr = ohal::test::MockRegister<uint32_t, &mock_idr>;
  using Odr = ohal::test::MockRegister<uint32_t, &mock_odr>;
  using Bsrr = BsrrCountingReg;
  using Lckr = ohal::test::MockRegister<uint32_t, &mock_lckr>;
  using Afrl = ohal::test::MockRegister<uint32_t, &mock_afrl>;
  using Afrh = ohal::test::MockRegister<uint32_t, &mock_afrh>;
  using Brr = ohal::test::MockRegister<uint32_t, &mock_brr>;
};

using CountingPin5 =
    ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<5U, MockGpioRegsCountingBsrr>;

using CountingPort = ohal::platforms::stm32u0::stm32u083::GpioPortImpl<MockGpioRegsCountingBsrr>;

class GpioStm32u083BsrrReadCountTest : public ::testing::Test {
protected:
  void SetUp() override { BsrrCountingReg::reset(); }
};

TEST_F(GpioStm32u083BsrrReadCountTest, Set_NeverCallsReadOnBsrr) {
  CountingPin5::set();
  EXPECT_EQ(BsrrCountingReg::read_count, 0U);
}

TEST_F(GpioStm32u083BsrrReadCountTest, Clear_NeverCallsReadOnBsrr) {
  CountingPin5::clear();
  EXPECT_EQ(BsrrCountingReg::read_count, 0U);
}

TEST_F(GpioStm32u083BsrrReadCountTest, PortSet_NeverCallsReadOnBsrr) {
  CountingPort::set(0xFFFFU);
  EXPECT_EQ(BsrrCountingReg::read_count, 0U);
}

TEST_F(GpioStm32u083BsrrReadCountTest, PortClear_NeverCallsReadOnBsrr) {
  CountingPort::clear(0xFFFFU);
  EXPECT_EQ(BsrrCountingReg::read_count, 0U);
}

TEST_F(GpioStm32u083BsrrReadCountTest, PortWrite_NeverCallsReadOnBsrr) {
  CountingPort::write(0x000FU, 0x00F0U);
  EXPECT_EQ(BsrrCountingReg::read_count, 0U);
}

// ---------------------------------------------------------------------------
// Capability trait: invalid pin numbers must report false
// ---------------------------------------------------------------------------

static_assert(!ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 16>::value,
              "PortA pin 16 (out of range) must not report supports_output_type");
static_assert(!ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortA, 16>::value,
              "PortA pin 16 (out of range) must not report supports_output_speed");
static_assert(!ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 16>::value,
              "PortA pin 16 (out of range) must not report supports_pull");
static_assert(!ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortA, 16>::value,
              "PortA pin 16 (out of range) must not report supports_alternate_function");

struct InvalidPinCapCase {
  bool value;
  const char* name;
};

class GpioStm32u083InvalidPinCapTest : public ::testing::TestWithParam<InvalidPinCapCase> {};

INSTANTIATE_TEST_SUITE_P(
    InvalidPinCapabilities, GpioStm32u083InvalidPinCapTest,
    ::testing::Values(
        InvalidPinCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 16>::value,
            "PortA_Pin16_OutputType"},
        InvalidPinCapCase{
            ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortA, 16>::value,
            "PortA_Pin16_OutputSpeed"},
        InvalidPinCapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 16>::value,
                          "PortA_Pin16_Pull"},
        InvalidPinCapCase{
            ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortA, 16>::value,
            "PortA_Pin16_AlternateFunction"}),
    [](const ::testing::TestParamInfo<InvalidPinCapCase>& info) { return info.param.name; });

TEST_P(GpioStm32u083InvalidPinCapTest, CapabilityIsFalse) { EXPECT_FALSE(GetParam().value); }

// ---------------------------------------------------------------------------
// Port-wiring tests: verify that each Pin<PortX, N> specialisation resolves to
// the correct hardware base address.
//
// Each production specialisation inherits from GpioPortPinImpl<N, GpioX> where
// GpioX = GpioPortRegs<kGpioXBase>.  Checking the address of BsrrSet::reg_type
// (the register type backing the BSRR set-bit field) exercises the full
// Port → Regs → Register<Address> chain without any hardware access.
// ---------------------------------------------------------------------------

namespace wiring = ohal::platforms::stm32u0::stm32u083;

static_assert(ohal::gpio::Pin<ohal::gpio::PortA, 0>::BsrrSet::reg_type::address ==
                  wiring::kGpioABase + wiring::kBsrrOffset,
              "Pin<PortA,0> must use GPIOA BSRR address");

static_assert(ohal::gpio::Pin<ohal::gpio::PortB, 0>::BsrrSet::reg_type::address ==
                  wiring::kGpioBBase + wiring::kBsrrOffset,
              "Pin<PortB,0> must use GPIOB BSRR address");

static_assert(ohal::gpio::Pin<ohal::gpio::PortC, 0>::BsrrSet::reg_type::address ==
                  wiring::kGpioCBase + wiring::kBsrrOffset,
              "Pin<PortC,0> must use GPIOC BSRR address");

static_assert(ohal::gpio::Pin<ohal::gpio::PortD, 0>::BsrrSet::reg_type::address ==
                  wiring::kGpioDBase + wiring::kBsrrOffset,
              "Pin<PortD,0> must use GPIOD BSRR address");

static_assert(ohal::gpio::Pin<ohal::gpio::PortE, 0>::BsrrSet::reg_type::address ==
                  wiring::kGpioEBase + wiring::kBsrrOffset,
              "Pin<PortE,0> must use GPIOE BSRR address");

static_assert(ohal::gpio::Pin<ohal::gpio::PortF, 0>::BsrrSet::reg_type::address ==
                  wiring::kGpioFBase + wiring::kBsrrOffset,
              "Pin<PortF,0> must use GPIOF BSRR address");

// Verify the MODER address for PortA and PortB to catch any offset errors.
static_assert(ohal::gpio::Pin<ohal::gpio::PortA, 0>::Moder::reg_type::address ==
                  wiring::kGpioABase + wiring::kModerOffset,
              "Pin<PortA,0> must use GPIOA MODER address");

static_assert(ohal::gpio::Pin<ohal::gpio::PortB, 0>::Moder::reg_type::address ==
                  wiring::kGpioBBase + wiring::kModerOffset,
              "Pin<PortB,0> must use GPIOB MODER address");

struct WiringCase {
  uintptr_t actual;
  uintptr_t expected;
  const char* name;
};

class GpioStm32u083WiringTest : public ::testing::TestWithParam<WiringCase> {};

INSTANTIATE_TEST_SUITE_P(
    PortBsrrAddresses, GpioStm32u083WiringTest,
    ::testing::Values(WiringCase{ohal::gpio::Pin<ohal::gpio::PortA, 0>::BsrrSet::reg_type::address,
                                 wiring::kGpioABase + wiring::kBsrrOffset, "PortA"},
                      WiringCase{ohal::gpio::Pin<ohal::gpio::PortB, 0>::BsrrSet::reg_type::address,
                                 wiring::kGpioBBase + wiring::kBsrrOffset, "PortB"},
                      WiringCase{ohal::gpio::Pin<ohal::gpio::PortC, 0>::BsrrSet::reg_type::address,
                                 wiring::kGpioCBase + wiring::kBsrrOffset, "PortC"},
                      WiringCase{ohal::gpio::Pin<ohal::gpio::PortD, 0>::BsrrSet::reg_type::address,
                                 wiring::kGpioDBase + wiring::kBsrrOffset, "PortD"},
                      WiringCase{ohal::gpio::Pin<ohal::gpio::PortE, 0>::BsrrSet::reg_type::address,
                                 wiring::kGpioEBase + wiring::kBsrrOffset, "PortE"},
                      WiringCase{ohal::gpio::Pin<ohal::gpio::PortF, 0>::BsrrSet::reg_type::address,
                                 wiring::kGpioFBase + wiring::kBsrrOffset, "PortF"}),
    [](const ::testing::TestParamInfo<WiringCase>& info) { return info.param.name; });

TEST_P(GpioStm32u083WiringTest, BsrrAddressMatchesHardwareBase) {
  EXPECT_EQ(GetParam().actual, GetParam().expected);
}

// ---------------------------------------------------------------------------
// Port<PortX> wiring tests: verify each Port<> specialisation resolves to the
// correct hardware BSRR address.
//
// Port<PortX> inherits from GpioPortImpl<GpioX> which exposes BsrrReg = Bsrr.
// Checking Port<PortX>::BsrrReg::address exercises the full PortTag →
// GpioPortImpl<Regs> → Register<Address> chain.
// ---------------------------------------------------------------------------

static_assert(ohal::gpio::Port<ohal::gpio::PortA>::BsrrReg::address ==
                  wiring::kGpioABase + wiring::kBsrrOffset,
              "Port<PortA> must use GPIOA BSRR address");

static_assert(ohal::gpio::Port<ohal::gpio::PortB>::BsrrReg::address ==
                  wiring::kGpioBBase + wiring::kBsrrOffset,
              "Port<PortB> must use GPIOB BSRR address");

static_assert(ohal::gpio::Port<ohal::gpio::PortC>::BsrrReg::address ==
                  wiring::kGpioCBase + wiring::kBsrrOffset,
              "Port<PortC> must use GPIOC BSRR address");

static_assert(ohal::gpio::Port<ohal::gpio::PortD>::BsrrReg::address ==
                  wiring::kGpioDBase + wiring::kBsrrOffset,
              "Port<PortD> must use GPIOD BSRR address");

static_assert(ohal::gpio::Port<ohal::gpio::PortE>::BsrrReg::address ==
                  wiring::kGpioEBase + wiring::kBsrrOffset,
              "Port<PortE> must use GPIOE BSRR address");

static_assert(ohal::gpio::Port<ohal::gpio::PortF>::BsrrReg::address ==
                  wiring::kGpioFBase + wiring::kBsrrOffset,
              "Port<PortF> must use GPIOF BSRR address");

class GpioStm32u083PortWiringTest : public ::testing::TestWithParam<WiringCase> {};

INSTANTIATE_TEST_SUITE_P(
    PortBsrrAddresses, GpioStm32u083PortWiringTest,
    ::testing::Values(WiringCase{ohal::gpio::Port<ohal::gpio::PortA>::BsrrReg::address,
                                 wiring::kGpioABase + wiring::kBsrrOffset, "PortA"},
                      WiringCase{ohal::gpio::Port<ohal::gpio::PortB>::BsrrReg::address,
                                 wiring::kGpioBBase + wiring::kBsrrOffset, "PortB"},
                      WiringCase{ohal::gpio::Port<ohal::gpio::PortC>::BsrrReg::address,
                                 wiring::kGpioCBase + wiring::kBsrrOffset, "PortC"},
                      WiringCase{ohal::gpio::Port<ohal::gpio::PortD>::BsrrReg::address,
                                 wiring::kGpioDBase + wiring::kBsrrOffset, "PortD"},
                      WiringCase{ohal::gpio::Port<ohal::gpio::PortE>::BsrrReg::address,
                                 wiring::kGpioEBase + wiring::kBsrrOffset, "PortE"},
                      WiringCase{ohal::gpio::Port<ohal::gpio::PortF>::BsrrReg::address,
                                 wiring::kGpioFBase + wiring::kBsrrOffset, "PortF"}),
    [](const ::testing::TestParamInfo<WiringCase>& info) { return info.param.name; });

TEST_P(GpioStm32u083PortWiringTest, BsrrAddressMatchesHardwareBase) {
  EXPECT_EQ(GetParam().actual, GetParam().expected);
}

} // namespace
