#include "mock/mock_register.hpp"

#include <ohal/core/capabilities.hpp>
#include <ohal/gpio.hpp>
#include <ohal/platforms/msp430fr2xx/models/msp430fr2355trsm/capabilities.hpp>
#include <ohal/platforms/msp430fr2xx/models/msp430fr2355trsm/gpio.hpp>

#include <cstdint>

#include <gtest/gtest.h>

// ---------------------------------------------------------------------------
// Host-side tests for the MSP430FR2355TRSM (VQFN32) GPIO model.
//
// The MSP430FR2355TRSM shares the same GPIO register layout as the generic
// MSP430FR2355, but the VQFN32 package bonds out only selected pins:
//   PortB (P2): bits 0,1,4,5,6,7 (bits 2,3 not bonded)
//   PortD (P4): bits 0,1,2,3,6,7 (bits 4,5 not bonded)
//   PortE/PortF: not bonded at all
// These tests verify:
//   1. Capability traits report the correct values for the TRSM package.
//   2. The Pin<> specialisations use the correct MMIO register addresses.
//   3. Basic behavioural correctness (via mock-register infrastructure).
// ---------------------------------------------------------------------------

namespace
{

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

struct MockGpioRegs
{
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

class GpioMsp430fr2355TrsmTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        mock_in = 0U;
        mock_out = 0U;
        mock_dir = 0U;
        mock_ren = 0U;
        mock_sel0 = 0U;
        mock_sel1 = 0U;
    }
};

// ---------------------------------------------------------------------------
// Basic behavioural tests
// ---------------------------------------------------------------------------

TEST_F(GpioMsp430fr2355TrsmTest, Set_SetsBitInOutRegister)
{
    MockPin3::set();
    EXPECT_EQ(mock_out, static_cast<uint8_t>(1U << 3U));
}

TEST_F(GpioMsp430fr2355TrsmTest, Clear_ClearsBitInOutRegister)
{
    mock_out = static_cast<uint8_t>(1U << 3U);
    MockPin3::clear();
    EXPECT_EQ(mock_out, static_cast<uint8_t>(0U));
}

TEST_F(GpioMsp430fr2355TrsmTest, ReadInput_ReturnsHigh_WhenInBitSet)
{
    mock_in = static_cast<uint8_t>(1U << 3U);
    EXPECT_EQ(MockPin3::read_input(), ohal::gpio::Level::High);
}

TEST_F(GpioMsp430fr2355TrsmTest, SetMode_Output_SetsDirBit)
{
    MockPin3::set_mode(ohal::gpio::PinMode::Output);
    EXPECT_EQ(mock_dir, static_cast<uint8_t>(1U << 3U));
}

TEST_F(GpioMsp430fr2355TrsmTest, SetMode_Input_ClearsDirBit)
{
    mock_dir = static_cast<uint8_t>(1U << 3U);
    MockPin3::set_mode(ohal::gpio::PinMode::Input);
    EXPECT_EQ(mock_dir, static_cast<uint8_t>(0U));
}

TEST_F(GpioMsp430fr2355TrsmTest, SetMode_AlternateFunction_SetsSel0Bit)
{
    MockPin3::set_mode(ohal::gpio::PinMode::AlternateFunction);
    EXPECT_EQ(mock_sel0, static_cast<uint8_t>(1U << 3U));
}

TEST_F(GpioMsp430fr2355TrsmTest, SetPull_Up_SetsRenBit)
{
    MockPin3::set_pull(ohal::gpio::Pull::Up);
    EXPECT_EQ(mock_ren, static_cast<uint8_t>(1U << 3U));
}

TEST_F(GpioMsp430fr2355TrsmTest, SetPull_Down_ClearsOutBit)
{
    mock_out = static_cast<uint8_t>(1U << 3U);
    MockPin3::set_pull(ohal::gpio::Pull::Down);
    EXPECT_EQ(mock_out, static_cast<uint8_t>(0U));
}

// ---------------------------------------------------------------------------
// Register-address wiring tests
// ---------------------------------------------------------------------------

// PortA (P1): base = kPortPair12Base, even-port sub-offset
static_assert(ohal::gpio::Pin<ohal::gpio::PortA, 0>::OutBit::reg_type::address ==
                  msp430::kPortPair12Base + msp430::kOutPairOffset + msp430::kEvenPortSub,
              "Pin<PortA,0> must use P1OUT address on TRSM");

// PortB (P2): base = kPortPair12Base, odd-port sub-offset
static_assert(ohal::gpio::Pin<ohal::gpio::PortB, 0>::OutBit::reg_type::address ==
                  msp430::kPortPair12Base + msp430::kOutPairOffset + msp430::kOddPortSub,
              "Pin<PortB,0> must use P2OUT address on TRSM");

// PortC (P3): base = kPortPair34Base, even-port sub-offset
static_assert(ohal::gpio::Pin<ohal::gpio::PortC, 0>::OutBit::reg_type::address ==
                  msp430::kPortPair34Base + msp430::kOutPairOffset + msp430::kEvenPortSub,
              "Pin<PortC,0> must use P3OUT address on TRSM");

// PortD (P4): base = kPortPair34Base, odd-port sub-offset
static_assert(ohal::gpio::Pin<ohal::gpio::PortD, 0>::OutBit::reg_type::address ==
                  msp430::kPortPair34Base + msp430::kOutPairOffset + msp430::kOddPortSub,
              "Pin<PortD,0> must use P4OUT address on TRSM");

struct WiringCase
{
    uintptr_t actual;
    uintptr_t expected;
    const char* name;
};

class GpioMsp430fr2355TrsmWiringTest : public ::testing::TestWithParam<WiringCase>
{
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    PortOutAddresses, GpioMsp430fr2355TrsmWiringTest,
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
            "PortD_P4OUT"}),
    [](const ::testing::TestParamInfo<WiringCase>& info) { return info.param.name; });
// clang-format on

TEST_P(GpioMsp430fr2355TrsmWiringTest, OutRegisterAddressMatchesHardwareBase)
{
    EXPECT_EQ(GetParam().actual, GetParam().expected);
}

// ---------------------------------------------------------------------------
// Capability trait compile-time assertions
// ---------------------------------------------------------------------------

// PortA: all bits 0-7 supported
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 0>::value,
              "TRSM PortA pin 0 must support pull");
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 7>::value,
              "TRSM PortA pin 7 must support pull");
static_assert(!ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 8>::value,
              "TRSM PortA pin 8 must not support pull");

// PortB: bits 0,1,4,5,6,7 bonded; bits 2,3 not bonded
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortB, 0>::value,
              "TRSM PortB pin 0 must support pull");
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortB, 1>::value,
              "TRSM PortB pin 1 must support pull");
static_assert(!ohal::gpio::capabilities::supports_pull<ohal::gpio::PortB, 2>::value,
              "TRSM PortB pin 2 must not support pull (not bonded)");
static_assert(!ohal::gpio::capabilities::supports_pull<ohal::gpio::PortB, 3>::value,
              "TRSM PortB pin 3 must not support pull (not bonded)");
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortB, 4>::value,
              "TRSM PortB pin 4 must support pull");
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortB, 7>::value,
              "TRSM PortB pin 7 must support pull");

// PortC: all bits 0-7 supported
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortC, 0>::value,
              "TRSM PortC pin 0 must support pull");
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortC, 7>::value,
              "TRSM PortC pin 7 must support pull");

// PortD: bits 0-3,6,7 bonded; bits 4,5 not bonded
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortD, 0>::value,
              "TRSM PortD pin 0 must support pull");
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortD, 3>::value,
              "TRSM PortD pin 3 must support pull");
static_assert(!ohal::gpio::capabilities::supports_pull<ohal::gpio::PortD, 4>::value,
              "TRSM PortD pin 4 must not support pull (not bonded)");
static_assert(!ohal::gpio::capabilities::supports_pull<ohal::gpio::PortD, 5>::value,
              "TRSM PortD pin 5 must not support pull (not bonded)");
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortD, 6>::value,
              "TRSM PortD pin 6 must support pull");
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortD, 7>::value,
              "TRSM PortD pin 7 must support pull");

// PortE/PortF: not bonded
static_assert(!ohal::gpio::capabilities::supports_pull<ohal::gpio::PortE, 0>::value,
              "TRSM PortE pin 0 must not support pull");
static_assert(!ohal::gpio::capabilities::supports_pull<ohal::gpio::PortF, 0>::value,
              "TRSM PortF pin 0 must not support pull");

// output_type and output_speed not supported
static_assert(!ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 0>::value,
              "TRSM must not report supports_output_type");
static_assert(!ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortA, 0>::value,
              "TRSM must not report supports_output_speed");

// ---------------------------------------------------------------------------
// Capability trait runtime tests — bonded pins (must be true)
// ---------------------------------------------------------------------------

struct CapCase
{
    bool value;
    const char* name;
};

class GpioMsp430fr2355TrsmCapabilityTest : public ::testing::TestWithParam<CapCase>
{
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    TrsmBondedCapabilities, GpioMsp430fr2355TrsmCapabilityTest,
    ::testing::Values(
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 0>::value,
                "PortA_Pin0_Pull"},
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 7>::value,
                "PortA_Pin7_Pull_Boundary"},
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortB, 0>::value,
                "PortB_Pin0_Pull"},
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortB, 1>::value,
                "PortB_Pin1_Pull"},
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortB, 4>::value,
                "PortB_Pin4_Pull"},
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortB, 7>::value,
                "PortB_Pin7_Pull"},
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortC, 0>::value,
                "PortC_Pin0_Pull"},
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortC, 7>::value,
                "PortC_Pin7_Pull"},
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortD, 0>::value,
                "PortD_Pin0_Pull"},
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortD, 3>::value,
                "PortD_Pin3_Pull"},
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortD, 6>::value,
                "PortD_Pin6_Pull"},
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortD, 7>::value,
                "PortD_Pin7_Pull"}),
    [](const ::testing::TestParamInfo<CapCase>& info) { return info.param.name; });
// clang-format on

TEST_P(GpioMsp430fr2355TrsmCapabilityTest, BondedPinCapability_ReturnsTrue)
{
    EXPECT_TRUE(GetParam().value);
}

// ---------------------------------------------------------------------------
// Capability trait runtime tests — non-bonded and out-of-range (must be false)
// ---------------------------------------------------------------------------

struct NegCapCase
{
    bool value;
    const char* name;
};

class GpioMsp430fr2355TrsmNegCapabilityTest : public ::testing::TestWithParam<NegCapCase>
{
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    TrsmNonBondedCapabilities, GpioMsp430fr2355TrsmNegCapabilityTest,
    ::testing::Values(
        // PortA out-of-range
        NegCapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 8>::value,
                   "PortA_Pin8_Pull_OutOfRange"},
        // PortB not bonded
        NegCapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortB, 2>::value,
                   "PortB_Pin2_Pull_NotBonded"},
        NegCapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortB, 3>::value,
                   "PortB_Pin3_Pull_NotBonded"},
        NegCapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortB, 2>::value,
                   "PortB_Pin2_AF_NotBonded"},
        // PortD not bonded
        NegCapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortD, 4>::value,
                   "PortD_Pin4_Pull_NotBonded"},
        NegCapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortD, 5>::value,
                   "PortD_Pin5_Pull_NotBonded"},
        NegCapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortD, 4>::value,
                   "PortD_Pin4_AF_NotBonded"},
        // PortE/PortF not bonded
        NegCapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortE, 0>::value,
                   "PortE_Pin0_Pull_NotBonded"},
        NegCapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortF, 0>::value,
                   "PortF_Pin0_Pull_NotBonded"},
        // output_type and output_speed not supported
        NegCapCase{ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 0>::value,
                   "PortA_Pin0_OutputType_NotSupported"},
        NegCapCase{ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortA, 0>::value,
                   "PortA_Pin0_OutputSpeed_NotSupported"}),
    [](const ::testing::TestParamInfo<NegCapCase>& info) { return info.param.name; });
// clang-format on

TEST_P(GpioMsp430fr2355TrsmNegCapabilityTest, NonBondedPinCapability_ReturnsFalse)
{
    EXPECT_FALSE(GetParam().value);
}

} // namespace
