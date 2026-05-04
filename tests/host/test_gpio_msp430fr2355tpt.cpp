#include "mock/mock_register.hpp"

#include <ohal/core/capabilities.hpp>
#include <ohal/gpio.hpp>
#include <ohal/platforms/msp430fr2xx/models/msp430fr2355tpt/capabilities.hpp>
#include <ohal/platforms/msp430fr2xx/models/msp430fr2355tpt/gpio.hpp>

#include <cstdint>

#include <gtest/gtest.h>

// ---------------------------------------------------------------------------
// Host-side tests for the MSP430FR2355TPT (LQFP48) GPIO model.
//
// The MSP430FR2355TPT shares the same GPIO register layout as the generic
// MSP430FR2355, but the LQFP48 package bonds out only bits 0–4 on P5 (PortE)
// and bits 0–6 on P6 (PortF).  These tests verify:
//   1. Capability traits report the correct values for the TPT package.
//   2. The Pin<> specialisations use the correct MMIO register addresses.
//   3. Basic behavioural correctness (via mock-register infrastructure).
// ---------------------------------------------------------------------------

namespace {

namespace msp430 = ohal::platforms::msp430fr2xx::msp430fr2355;

// ---------------------------------------------------------------------------
// Mock register set for one MSP430FR2355 8-bit GPIO port.
// ---------------------------------------------------------------------------

static uint8_t mock_in{0U};
static uint8_t mock_out{0U};
static uint8_t mock_dir{0U};
static uint8_t mock_ren{0U};
static uint8_t mock_sel0{0U};
static uint8_t mock_sel1{0U};

struct MockGpioRegs {
  using In = ohal::test::MockRegister<uint8_t, &mock_in>;
  using Out = ohal::test::MockRegister<uint8_t, &mock_out>;
  using Dir = ohal::test::MockRegister<uint8_t, &mock_dir>;
  using Ren = ohal::test::MockRegister<uint8_t, &mock_ren>;
  using Sel0 = ohal::test::MockRegister<uint8_t, &mock_sel0>;
  using Sel1 = ohal::test::MockRegister<uint8_t, &mock_sel1>;
};

using MockPin3 = msp430::GpioPortPinImpl<3U, MockGpioRegs>;

// ---------------------------------------------------------------------------
// Test fixture
// ---------------------------------------------------------------------------

class GpioMsp430fr2355TptTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock_in = 0U;
    mock_out = 0U;
    mock_dir = 0U;
    mock_ren = 0U;
    mock_sel0 = 0U;
    mock_sel1 = 0U;
  }
};

// ---------------------------------------------------------------------------
// Basic behavioural tests (delegates to the shared GpioPortPinImpl)
// ---------------------------------------------------------------------------

TEST_F(GpioMsp430fr2355TptTest, Set_SetsBitInOutRegister) {
  MockPin3::set();
  EXPECT_EQ((mock_out >> 3U) & 1U, 1U);
}

TEST_F(GpioMsp430fr2355TptTest, Clear_ClearsBitInOutRegister) {
  mock_out = static_cast<uint8_t>(1U << 3U);
  MockPin3::clear();
  EXPECT_EQ((mock_out >> 3U) & 1U, 0U);
}

TEST_F(GpioMsp430fr2355TptTest, ReadInput_ReturnsHigh_WhenInBitSet) {
  mock_in = static_cast<uint8_t>(1U << 3U);
  EXPECT_EQ(MockPin3::read_input(), ohal::gpio::Level::High);
}

TEST_F(GpioMsp430fr2355TptTest, SetMode_Output_SetsDirBit) {
  MockPin3::set_mode(ohal::gpio::PinMode::Output);
  EXPECT_EQ((mock_dir >> 3U) & 1U, 1U);
}

TEST_F(GpioMsp430fr2355TptTest, SetMode_Input_ClearsDirBit) {
  mock_dir = static_cast<uint8_t>(1U << 3U);
  MockPin3::set_mode(ohal::gpio::PinMode::Input);
  EXPECT_EQ((mock_dir >> 3U) & 1U, 0U);
}

TEST_F(GpioMsp430fr2355TptTest, SetMode_AlternateFunction_SetsSel0Bit) {
  MockPin3::set_mode(ohal::gpio::PinMode::AlternateFunction);
  EXPECT_EQ((mock_sel0 >> 3U) & 1U, 1U);
}

TEST_F(GpioMsp430fr2355TptTest, SetPull_Up_SetsRenBit) {
  MockPin3::set_pull(ohal::gpio::Pull::Up);
  EXPECT_EQ((mock_ren >> 3U) & 1U, 1U);
}

TEST_F(GpioMsp430fr2355TptTest, SetPull_Down_ClearsOutBit) {
  mock_out = static_cast<uint8_t>(1U << 3U);
  MockPin3::set_pull(ohal::gpio::Pull::Down);
  EXPECT_EQ((mock_out >> 3U) & 1U, 0U);
}

// ---------------------------------------------------------------------------
// Register-address wiring tests: verify Pin<PortX, N> resolves to the
// correct MMIO addresses for the MSP430FR2355 shared GPIO layout.
// ---------------------------------------------------------------------------

// PortA (P1): base = kPortPair12Base, even-port sub-offset
static_assert(ohal::gpio::Pin<ohal::gpio::PortA, 0>::OutBit::reg_type::address ==
                  msp430::kPortPair12Base + msp430::kOutPairOffset + msp430::kEvenPortSub,
              "Pin<PortA,0> must use P1OUT address on TPT");

// PortB (P2): base = kPortPair12Base, odd-port sub-offset
static_assert(ohal::gpio::Pin<ohal::gpio::PortB, 0>::OutBit::reg_type::address ==
                  msp430::kPortPair12Base + msp430::kOutPairOffset + msp430::kOddPortSub,
              "Pin<PortB,0> must use P2OUT address on TPT");

// PortC (P3): base = kPortPair34Base, even-port sub-offset
static_assert(ohal::gpio::Pin<ohal::gpio::PortC, 0>::OutBit::reg_type::address ==
                  msp430::kPortPair34Base + msp430::kOutPairOffset + msp430::kEvenPortSub,
              "Pin<PortC,0> must use P3OUT address on TPT");

// PortD (P4): base = kPortPair34Base, odd-port sub-offset
static_assert(ohal::gpio::Pin<ohal::gpio::PortD, 0>::OutBit::reg_type::address ==
                  msp430::kPortPair34Base + msp430::kOutPairOffset + msp430::kOddPortSub,
              "Pin<PortD,0> must use P4OUT address on TPT");

// PortE (P5): base = kPortPair56Base, even-port sub-offset (only bits 0–4 bonded)
static_assert(ohal::gpio::Pin<ohal::gpio::PortE, 4>::OutBit::reg_type::address ==
                  msp430::kPortPair56Base + msp430::kOutPairOffset + msp430::kEvenPortSub,
              "Pin<PortE,4> must use P5OUT address on TPT");

// PortF (P6): base = kPortPair56Base, odd-port sub-offset (only bits 0–6 bonded)
static_assert(ohal::gpio::Pin<ohal::gpio::PortF, 6>::OutBit::reg_type::address ==
                  msp430::kPortPair56Base + msp430::kOutPairOffset + msp430::kOddPortSub,
              "Pin<PortF,6> must use P6OUT address on TPT");

struct WiringCase {
  uintptr_t actual;
  uintptr_t expected;
  const char* name;
};

class GpioMsp430fr2355TptWiringTest : public ::testing::TestWithParam<WiringCase> {};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    PortOutAddresses, GpioMsp430fr2355TptWiringTest,
    ::testing::Values(
        WiringCase{
            ohal::gpio::Pin<ohal::gpio::PortA, 0>::OutBit::reg_type::address,
            msp430::kPortPair12Base + msp430::kOutPairOffset + msp430::kEvenPortSub,
            "PortA_P1OUT"},
        WiringCase{
            ohal::gpio::Pin<ohal::gpio::PortB, 0>::OutBit::reg_type::address,
            msp430::kPortPair12Base + msp430::kOutPairOffset + msp430::kOddPortSub,
            "PortB_P2OUT"},
        WiringCase{
            ohal::gpio::Pin<ohal::gpio::PortC, 0>::OutBit::reg_type::address,
            msp430::kPortPair34Base + msp430::kOutPairOffset + msp430::kEvenPortSub,
            "PortC_P3OUT"},
        WiringCase{
            ohal::gpio::Pin<ohal::gpio::PortD, 0>::OutBit::reg_type::address,
            msp430::kPortPair34Base + msp430::kOutPairOffset + msp430::kOddPortSub,
            "PortD_P4OUT"},
        WiringCase{
            ohal::gpio::Pin<ohal::gpio::PortE, 4>::OutBit::reg_type::address,
            msp430::kPortPair56Base + msp430::kOutPairOffset + msp430::kEvenPortSub,
            "PortE_P5OUT"},
        WiringCase{
            ohal::gpio::Pin<ohal::gpio::PortF, 6>::OutBit::reg_type::address,
            msp430::kPortPair56Base + msp430::kOutPairOffset + msp430::kOddPortSub,
            "PortF_P6OUT"}),
    [](const ::testing::TestParamInfo<WiringCase>& info) { return info.param.name; });
// clang-format on

TEST_P(GpioMsp430fr2355TptWiringTest, OutRegisterAddressMatchesHardwareBase) {
  EXPECT_EQ(GetParam().actual, GetParam().expected);
}

// ---------------------------------------------------------------------------
// Capability trait compile-time assertions
// ---------------------------------------------------------------------------

// PortA–PortD: all bits 0–7 supported (P1–P4 fully bonded)
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 0>::value,
              "TPT PortA pin 0 must support pull");
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 7>::value,
              "TPT PortA pin 7 must support pull");
static_assert(!ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 8>::value,
              "TPT PortA pin 8 must not support pull (out of range)");

static_assert(ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortD, 7>::value,
              "TPT PortD pin 7 must support alternate function");
static_assert(!ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortD, 8>::value,
              "TPT PortD pin 8 must not support alternate function (out of range)");

// PortE (P5): bits 0–4 supported; bits 5–7 not bonded on LQFP48
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortE, 0>::value,
              "TPT PortE pin 0 must support pull");
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortE, 4>::value,
              "TPT PortE pin 4 must support pull (boundary)");
static_assert(!ohal::gpio::capabilities::supports_pull<ohal::gpio::PortE, 5>::value,
              "TPT PortE pin 5 must not support pull (not bonded on LQFP48)");
static_assert(!ohal::gpio::capabilities::supports_pull<ohal::gpio::PortE, 7>::value,
              "TPT PortE pin 7 must not support pull (not bonded on LQFP48)");
static_assert(ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortE, 4>::value,
              "TPT PortE pin 4 must support alternate function (boundary)");
static_assert(!ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortE, 5>::value,
              "TPT PortE pin 5 must not support alternate function (not bonded on LQFP48)");

// PortF (P6): bits 0–6 supported; bit 7 not bonded on LQFP48
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortF, 0>::value,
              "TPT PortF pin 0 must support pull");
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortF, 6>::value,
              "TPT PortF pin 6 must support pull (boundary)");
static_assert(!ohal::gpio::capabilities::supports_pull<ohal::gpio::PortF, 7>::value,
              "TPT PortF pin 7 must not support pull (not bonded on LQFP48)");
static_assert(ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortF, 6>::value,
              "TPT PortF pin 6 must support alternate function (boundary)");
static_assert(!ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortF, 7>::value,
              "TPT PortF pin 7 must not support alternate function (not bonded on LQFP48)");

// output_type and output_speed are not supported on any MSP430FR2355 port/pin.
static_assert(!ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 0>::value,
              "TPT must not report supports_output_type");
static_assert(!ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortA, 0>::value,
              "TPT must not report supports_output_speed");

// ---------------------------------------------------------------------------
// Capability trait runtime tests — bonded pins (must be true)
// ---------------------------------------------------------------------------

struct CapCase {
  bool value;
  const char* name;
};

class GpioMsp430fr2355TptCapabilityTest : public ::testing::TestWithParam<CapCase> {};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    TptBondedCapabilities, GpioMsp430fr2355TptCapabilityTest,
    ::testing::Values(
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 0>::value,
                "PortA_Pin0_Pull"},
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 7>::value,
                "PortA_Pin7_Pull_Boundary"},
        CapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortB, 0>::value,
                "PortB_Pin0_AlternateFunction"},
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortC, 7>::value,
                "PortC_Pin7_Pull"},
        CapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortD, 7>::value,
                "PortD_Pin7_AlternateFunction"},
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortE, 0>::value,
                "PortE_Pin0_Pull"},
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortE, 4>::value,
                "PortE_Pin4_Pull_Boundary"},
        CapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortE, 4>::value,
                "PortE_Pin4_AlternateFunction_Boundary"},
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortF, 0>::value,
                "PortF_Pin0_Pull"},
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortF, 6>::value,
                "PortF_Pin6_Pull_Boundary"},
        CapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortF, 6>::value,
                "PortF_Pin6_AlternateFunction_Boundary"}),
    [](const ::testing::TestParamInfo<CapCase>& info) { return info.param.name; });
// clang-format on

TEST_P(GpioMsp430fr2355TptCapabilityTest, CapabilityIsTrue) { EXPECT_TRUE(GetParam().value); }

// ---------------------------------------------------------------------------
// Capability trait runtime tests — non-bonded and out-of-range (must be false)
// ---------------------------------------------------------------------------

struct NegCapCase {
  bool value;
  const char* name;
};

class GpioMsp430fr2355TptNegCapabilityTest : public ::testing::TestWithParam<NegCapCase> {};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    TptNonBondedCapabilities, GpioMsp430fr2355TptNegCapabilityTest,
    ::testing::Values(
        // PortA out-of-range
        NegCapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 8>::value,
                   "PortA_Pin8_Pull_OutOfRange"},
        NegCapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortA, 8>::value,
                   "PortA_Pin8_AF_OutOfRange"},
        // PortE — bits 5–7 not bonded on LQFP48
        NegCapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortE, 5>::value,
                   "PortE_Pin5_Pull_NotBonded"},
        NegCapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortE, 6>::value,
                   "PortE_Pin6_Pull_NotBonded"},
        NegCapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortE, 7>::value,
                   "PortE_Pin7_Pull_NotBonded"},
        NegCapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortE, 5>::value,
                   "PortE_Pin5_AF_NotBonded"},
        // PortF — bit 7 not bonded on LQFP48
        NegCapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortF, 7>::value,
                   "PortF_Pin7_Pull_NotBonded"},
        NegCapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortF, 7>::value,
                   "PortF_Pin7_AF_NotBonded"},
        // output_type and output_speed not supported on any pin
        NegCapCase{ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 0>::value,
                   "PortA_Pin0_OutputType_NotSupported"},
        NegCapCase{ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortA, 0>::value,
                   "PortA_Pin0_OutputSpeed_NotSupported"}),
    [](const ::testing::TestParamInfo<NegCapCase>& info) { return info.param.name; });
// clang-format on

TEST_P(GpioMsp430fr2355TptNegCapabilityTest, CapabilityIsFalse) { EXPECT_FALSE(GetParam().value); }

} // namespace
