#include "mock/mock_register.hpp"

#include <ohal/core/capabilities.hpp>
#include <ohal/gpio.hpp>
#include <ohal/platforms/msp430fr2xx/models/msp430fr2355trha/capabilities.hpp>
#include <ohal/platforms/msp430fr2xx/models/msp430fr2355trha/gpio.hpp>

#include <cstdint>

#include <gtest/gtest.h>

// ---------------------------------------------------------------------------
// Host-side tests for the MSP430FR2355TRHA (VQFN40) GPIO model.
//
// The MSP430FR2355TRHA shares the same GPIO register layout as the generic
// MSP430FR2355, but the VQFN40 package bonds out only bits 0-1 on both
// P5 (PortE) and P6 (PortF).  These tests verify:
//   1. Capability traits report the correct values for the TRHA package.
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

class GpioMsp430fr2355TrhaTest : public ::testing::Test
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

TEST_F(GpioMsp430fr2355TrhaTest, Set_SetsBitInOutRegister)
{
    MockPin3::set();
    EXPECT_EQ(mock_out, static_cast<uint8_t>(1U << 3U));
}

TEST_F(GpioMsp430fr2355TrhaTest, Clear_ClearsBitInOutRegister)
{
    mock_out = static_cast<uint8_t>(1U << 3U);
    MockPin3::clear();
    EXPECT_EQ(mock_out, static_cast<uint8_t>(0U));
}

TEST_F(GpioMsp430fr2355TrhaTest, ReadInput_ReturnsHigh_WhenInBitSet)
{
    mock_in = static_cast<uint8_t>(1U << 3U);
    EXPECT_EQ(MockPin3::read_input(), ohal::gpio::Level::High);
}

TEST_F(GpioMsp430fr2355TrhaTest, SetMode_Output_SetsDirBit)
{
    MockPin3::set_mode(ohal::gpio::PinMode::Output);
    EXPECT_EQ(mock_dir, static_cast<uint8_t>(1U << 3U));
}

TEST_F(GpioMsp430fr2355TrhaTest, SetMode_Input_ClearsDirBit)
{
    mock_dir = static_cast<uint8_t>(1U << 3U);
    MockPin3::set_mode(ohal::gpio::PinMode::Input);
    EXPECT_EQ(mock_dir, static_cast<uint8_t>(0U));
}

TEST_F(GpioMsp430fr2355TrhaTest, SetMode_AlternateFunction_SetsSel0Bit)
{
    MockPin3::set_mode(ohal::gpio::PinMode::AlternateFunction);
    EXPECT_EQ(mock_sel0, static_cast<uint8_t>(1U << 3U));
}

TEST_F(GpioMsp430fr2355TrhaTest, SetPull_Up_SetsRenBit)
{
    MockPin3::set_pull(ohal::gpio::Pull::Up);
    EXPECT_EQ(mock_ren, static_cast<uint8_t>(1U << 3U));
}

TEST_F(GpioMsp430fr2355TrhaTest, SetPull_Down_ClearsOutBit)
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
              "Pin<PortA,0> must use P1OUT address on TRHA");

// PortB (P2): base = kPortPair12Base, odd-port sub-offset
static_assert(ohal::gpio::Pin<ohal::gpio::PortB, 0>::OutBit::reg_type::address ==
                  msp430::kPortPair12Base + msp430::kOutPairOffset + msp430::kOddPortSub,
              "Pin<PortB,0> must use P2OUT address on TRHA");

// PortC (P3): base = kPortPair34Base, even-port sub-offset
static_assert(ohal::gpio::Pin<ohal::gpio::PortC, 0>::OutBit::reg_type::address ==
                  msp430::kPortPair34Base + msp430::kOutPairOffset + msp430::kEvenPortSub,
              "Pin<PortC,0> must use P3OUT address on TRHA");

// PortD (P4): base = kPortPair34Base, odd-port sub-offset
static_assert(ohal::gpio::Pin<ohal::gpio::PortD, 0>::OutBit::reg_type::address ==
                  msp430::kPortPair34Base + msp430::kOutPairOffset + msp430::kOddPortSub,
              "Pin<PortD,0> must use P4OUT address on TRHA");

// PortE (P5): base = kPortPair56Base, even-port sub-offset (only bits 0-1 bonded)
static_assert(ohal::gpio::Pin<ohal::gpio::PortE, 1>::OutBit::reg_type::address ==
                  msp430::kPortPair56Base + msp430::kOutPairOffset + msp430::kEvenPortSub,
              "Pin<PortE,1> must use P5OUT address on TRHA");

// PortF (P6): base = kPortPair56Base, odd-port sub-offset (only bits 0-1 bonded)
static_assert(ohal::gpio::Pin<ohal::gpio::PortF, 1>::OutBit::reg_type::address ==
                  msp430::kPortPair56Base + msp430::kOutPairOffset + msp430::kOddPortSub,
              "Pin<PortF,1> must use P6OUT address on TRHA");

struct WiringCase
{
    uintptr_t actual;
    uintptr_t expected;
    const char* name;
};

class GpioMsp430fr2355TrhaWiringTest : public ::testing::TestWithParam<WiringCase>
{
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    PortOutAddresses, GpioMsp430fr2355TrhaWiringTest,
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
            ohal::gpio::Pin<ohal::gpio::PortE, 1>::OutBit::reg_type::address,
            msp430::kPortPair56Base + msp430::kOutPairOffset + msp430::kEvenPortSub,
            "PortE_P5OUT"},
        WiringCase{
            ohal::gpio::Pin<ohal::gpio::PortF, 1>::OutBit::reg_type::address,
            msp430::kPortPair56Base + msp430::kOutPairOffset + msp430::kOddPortSub,
            "PortF_P6OUT"}),
    [](const ::testing::TestParamInfo<WiringCase>& info) { return info.param.name; });
// clang-format on

TEST_P(GpioMsp430fr2355TrhaWiringTest, OutRegisterAddressMatchesHardwareBase)
{
    EXPECT_EQ(GetParam().actual, GetParam().expected);
}

// ---------------------------------------------------------------------------
// Capability trait compile-time assertions
// ---------------------------------------------------------------------------

// PortA-D: all bits 0-7 supported
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 0>::value,
              "TRHA PortA pin 0 must support pull");
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 7>::value,
              "TRHA PortA pin 7 must support pull");
static_assert(!ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 8>::value,
              "TRHA PortA pin 8 must not support pull (out of range)");

static_assert(ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortD, 7>::value,
              "TRHA PortD pin 7 must support alternate function");
static_assert(!ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortD, 8>::value,
              "TRHA PortD pin 8 must not support alternate function (out of range)");

// PortE (P5): bits 0-1 supported; bits 2-7 not bonded
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortE, 0>::value,
              "TRHA PortE pin 0 must support pull");
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortE, 1>::value,
              "TRHA PortE pin 1 must support pull (boundary)");
static_assert(!ohal::gpio::capabilities::supports_pull<ohal::gpio::PortE, 2>::value,
              "TRHA PortE pin 2 must not support pull (not bonded on VQFN40)");
static_assert(!ohal::gpio::capabilities::supports_pull<ohal::gpio::PortE, 7>::value,
              "TRHA PortE pin 7 must not support pull (not bonded on VQFN40)");

// PortF (P6): bits 0-1 supported; bits 2-7 not bonded
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortF, 0>::value,
              "TRHA PortF pin 0 must support pull");
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortF, 1>::value,
              "TRHA PortF pin 1 must support pull (boundary)");
static_assert(!ohal::gpio::capabilities::supports_pull<ohal::gpio::PortF, 2>::value,
              "TRHA PortF pin 2 must not support pull (not bonded on VQFN40)");
static_assert(!ohal::gpio::capabilities::supports_pull<ohal::gpio::PortF, 7>::value,
              "TRHA PortF pin 7 must not support pull (not bonded on VQFN40)");

// output_type and output_speed not supported
static_assert(!ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 0>::value,
              "TRHA must not report supports_output_type");
static_assert(!ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortA, 0>::value,
              "TRHA must not report supports_output_speed");

// ---------------------------------------------------------------------------
// Capability trait runtime tests — bonded pins (must be true)
// ---------------------------------------------------------------------------

struct CapCase
{
    bool value;
    const char* name;
};

class GpioMsp430fr2355TrhaCapabilityTest : public ::testing::TestWithParam<CapCase>
{
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    TrhaBondedCapabilities, GpioMsp430fr2355TrhaCapabilityTest,
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
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortE, 1>::value,
                "PortE_Pin1_Pull_Boundary"},
        CapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortE, 1>::value,
                "PortE_Pin1_AlternateFunction_Boundary"},
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortF, 0>::value,
                "PortF_Pin0_Pull"},
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortF, 1>::value,
                "PortF_Pin1_Pull_Boundary"},
        CapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortF, 1>::value,
                "PortF_Pin1_AlternateFunction_Boundary"}),
    [](const ::testing::TestParamInfo<CapCase>& info) { return info.param.name; });
// clang-format on

TEST_P(GpioMsp430fr2355TrhaCapabilityTest, BondedPinCapability_ReturnsTrue)
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

class GpioMsp430fr2355TrhaNegCapabilityTest : public ::testing::TestWithParam<NegCapCase>
{
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    TrhaNonBondedCapabilities, GpioMsp430fr2355TrhaNegCapabilityTest,
    ::testing::Values(
        // PortA out-of-range
        NegCapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 8>::value,
                   "PortA_Pin8_Pull_OutOfRange"},
        NegCapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortA, 8>::value,
                   "PortA_Pin8_AF_OutOfRange"},
        // PortE — bits 2-7 not bonded on VQFN40
        NegCapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortE, 2>::value,
                   "PortE_Pin2_Pull_NotBonded"},
        NegCapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortE, 7>::value,
                   "PortE_Pin7_Pull_NotBonded"},
        NegCapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortE, 2>::value,
                   "PortE_Pin2_AF_NotBonded"},
        // PortF — bits 2-7 not bonded on VQFN40
        NegCapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortF, 2>::value,
                   "PortF_Pin2_Pull_NotBonded"},
        NegCapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortF, 7>::value,
                   "PortF_Pin7_Pull_NotBonded"},
        NegCapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortF, 2>::value,
                   "PortF_Pin2_AF_NotBonded"},
        // output_type and output_speed not supported
        NegCapCase{ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 0>::value,
                   "PortA_Pin0_OutputType_NotSupported"},
        NegCapCase{ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortA, 0>::value,
                   "PortA_Pin0_OutputSpeed_NotSupported"}),
    [](const ::testing::TestParamInfo<NegCapCase>& info) { return info.param.name; });
// clang-format on

TEST_P(GpioMsp430fr2355TrhaNegCapabilityTest, NonBondedPinCapability_ReturnsFalse)
{
    EXPECT_FALSE(GetParam().value);
}

} // namespace
