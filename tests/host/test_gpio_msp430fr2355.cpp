#include "mock/mock_register.hpp"

#include <ohal/core/capabilities.hpp>
#include <ohal/gpio.hpp>
#include <ohal/platforms/msp430fr2xx/models/msp430fr2355/capabilities.hpp>
#include <ohal/platforms/msp430fr2xx/models/msp430fr2355/gpio.hpp>

#include <cstdint>

#include <gtest/gtest.h>

// ---------------------------------------------------------------------------
// Mock register set for one MSP430FR2355 8-bit GPIO port.
//
// Each backing variable corresponds to one physical register in GpioPortRegs.
// Static storage duration is required by MockRegister<T, T* Storage>.
// ---------------------------------------------------------------------------

namespace
{

static uint8_t mock_in{0U};
static uint8_t mock_out{0U};
static uint8_t mock_dir{0U};
static uint8_t mock_ren{0U};
static uint8_t mock_sel0{0U};
static uint8_t mock_sel1{0U};

/// A mock GPIO port register set with the same nested type aliases as
/// GpioPortRegs<…>, but backed by plain in-memory variables instead of
/// volatile MMIO addresses.  Passed as the Regs template argument to
/// GpioPortPinImpl in all tests below.
struct MockGpioRegs
{
    using In = ohal::test::MockRegister<uint8_t, &mock_in>;
    using Out = ohal::test::MockRegister<uint8_t, &mock_out>;
    using Dir = ohal::test::MockRegister<uint8_t, &mock_dir>;
    using Ren = ohal::test::MockRegister<uint8_t, &mock_ren>;
    using Sel0 = ohal::test::MockRegister<uint8_t, &mock_sel0>;
    using Sel1 = ohal::test::MockRegister<uint8_t, &mock_sel1>;
};

// Convenience aliases: pins 2, 0, and 7 backed by MockGpioRegs.
using MockPin2 = ohal::platforms::msp430fr2xx::msp430fr2355::GpioPortPinImpl<2U, MockGpioRegs>;
using MockPin0 = ohal::platforms::msp430fr2xx::msp430fr2355::GpioPortPinImpl<0U, MockGpioRegs>;
using MockPin7 = ohal::platforms::msp430fr2xx::msp430fr2355::GpioPortPinImpl<7U, MockGpioRegs>;

// ---------------------------------------------------------------------------
// Test fixture: resets every mock register before each test.
// ---------------------------------------------------------------------------

class GpioMsp430fr2355Test : public ::testing::Test
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
// set() — sets bit PinNum in Out register via read-modify-write
// ---------------------------------------------------------------------------

TEST_F(GpioMsp430fr2355Test, Set_SetsBitInOutRegister)
{
    MockPin2::set();
    EXPECT_EQ((mock_out >> 2U) & 1U, 1U);
}

TEST_F(GpioMsp430fr2355Test, Set_PreservesOtherOutBits)
{
    mock_out = static_cast<uint8_t>(~(1U << 2U)); // all bits set except pin 2
    MockPin2::set();
    EXPECT_EQ(mock_out & static_cast<uint8_t>(~(1U << 2U)), static_cast<uint8_t>(~(1U << 2U)));
}

// ---------------------------------------------------------------------------
// clear() — clears bit PinNum in Out register via read-modify-write
// ---------------------------------------------------------------------------

TEST_F(GpioMsp430fr2355Test, Clear_ClearsBitInOutRegister)
{
    mock_out = static_cast<uint8_t>(1U << 2U);
    MockPin2::clear();
    EXPECT_EQ((mock_out >> 2U) & 1U, 0U);
}

TEST_F(GpioMsp430fr2355Test, Clear_PreservesOtherOutBits)
{
    mock_out = 0xFFU; // all bits set
    MockPin2::clear();
    EXPECT_EQ(mock_out & static_cast<uint8_t>(~(1U << 2U)),
              static_cast<uint8_t>(~(1U << 2U)) & 0xFFU);
}

// ---------------------------------------------------------------------------
// read_input() — reads bit PinNum from In register
// ---------------------------------------------------------------------------

TEST_F(GpioMsp430fr2355Test, ReadInput_ReturnsHigh_WhenInBitSet)
{
    mock_in = static_cast<uint8_t>(1U << 2U);
    EXPECT_EQ(MockPin2::read_input(), ohal::gpio::Level::High);
}

TEST_F(GpioMsp430fr2355Test, ReadInput_ReturnsLow_WhenInBitClear)
{
    mock_in = 0U;
    EXPECT_EQ(MockPin2::read_input(), ohal::gpio::Level::Low);
}

TEST_F(GpioMsp430fr2355Test, ReadInput_IgnoresOtherInBits)
{
    mock_in = static_cast<uint8_t>(~(1U << 2U)); // all bits set except pin 2
    EXPECT_EQ(MockPin2::read_input(), ohal::gpio::Level::Low);
}

// ---------------------------------------------------------------------------
// read_output() — reads bit PinNum from Out register
// ---------------------------------------------------------------------------

TEST_F(GpioMsp430fr2355Test, ReadOutput_ReturnsHigh_WhenOutBitSet)
{
    mock_out = static_cast<uint8_t>(1U << 2U);
    EXPECT_EQ(MockPin2::read_output(), ohal::gpio::Level::High);
}

TEST_F(GpioMsp430fr2355Test, ReadOutput_ReturnsLow_WhenOutBitClear)
{
    mock_out = 0U;
    EXPECT_EQ(MockPin2::read_output(), ohal::gpio::Level::Low);
}

// ---------------------------------------------------------------------------
// toggle() — XOR pin bit in Out register (single read-modify-write)
// ---------------------------------------------------------------------------

TEST_F(GpioMsp430fr2355Test, Toggle_SetsPin_WhenOutputWasLow)
{
    mock_out = 0U; // pin 2 is Low
    MockPin2::toggle();
    EXPECT_EQ((mock_out >> 2U) & 1U, 1U);
}

TEST_F(GpioMsp430fr2355Test, Toggle_ClearsPin_WhenOutputWasHigh)
{
    mock_out = static_cast<uint8_t>(1U << 2U); // pin 2 is High
    MockPin2::toggle();
    EXPECT_EQ((mock_out >> 2U) & 1U, 0U);
}

// ---------------------------------------------------------------------------
// set_mode(Output) — sets Dir bit, clears Sel0 and Sel1 bits
// ---------------------------------------------------------------------------

TEST_F(GpioMsp430fr2355Test, SetMode_Output_SetsDirBit)
{
    MockPin2::set_mode(ohal::gpio::PinMode::Output);
    EXPECT_EQ((mock_dir >> 2U) & 1U, 1U);
}

TEST_F(GpioMsp430fr2355Test, SetMode_Output_ClearsSel0Bit)
{
    mock_sel0 = static_cast<uint8_t>(1U << 2U); // pre-load AF selection
    MockPin2::set_mode(ohal::gpio::PinMode::Output);
    EXPECT_EQ((mock_sel0 >> 2U) & 1U, 0U);
}

TEST_F(GpioMsp430fr2355Test, SetMode_Output_ClearsSel1Bit)
{
    mock_sel1 = static_cast<uint8_t>(1U << 2U);
    MockPin2::set_mode(ohal::gpio::PinMode::Output);
    EXPECT_EQ((mock_sel1 >> 2U) & 1U, 0U);
}

TEST_F(GpioMsp430fr2355Test, SetMode_Output_PreservesOtherDirBits)
{
    mock_dir = static_cast<uint8_t>(~(1U << 2U)); // all bits set except pin 2
    MockPin2::set_mode(ohal::gpio::PinMode::Output);
    EXPECT_EQ(mock_dir & static_cast<uint8_t>(~(1U << 2U)), static_cast<uint8_t>(~(1U << 2U)));
}

// ---------------------------------------------------------------------------
// set_mode(Input) — clears Dir, Sel0, and Sel1 bits
// ---------------------------------------------------------------------------

TEST_F(GpioMsp430fr2355Test, SetMode_Input_ClearsDirBit)
{
    mock_dir = static_cast<uint8_t>(1U << 2U); // pre-load Output mode
    MockPin2::set_mode(ohal::gpio::PinMode::Input);
    EXPECT_EQ((mock_dir >> 2U) & 1U, 0U);
}

TEST_F(GpioMsp430fr2355Test, SetMode_Input_ClearsSel0Bit)
{
    mock_sel0 = static_cast<uint8_t>(1U << 2U);
    MockPin2::set_mode(ohal::gpio::PinMode::Input);
    EXPECT_EQ((mock_sel0 >> 2U) & 1U, 0U);
}

TEST_F(GpioMsp430fr2355Test, SetMode_Input_ClearsSel1Bit)
{
    mock_sel1 = static_cast<uint8_t>(1U << 2U);
    MockPin2::set_mode(ohal::gpio::PinMode::Input);
    EXPECT_EQ((mock_sel1 >> 2U) & 1U, 0U);
}

// ---------------------------------------------------------------------------
// set_mode(AlternateFunction) — sets Sel0, clears Sel1; does NOT write Dir
// ---------------------------------------------------------------------------

TEST_F(GpioMsp430fr2355Test, SetMode_AlternateFunction_SetsSel0Bit)
{
    MockPin2::set_mode(ohal::gpio::PinMode::AlternateFunction);
    EXPECT_EQ((mock_sel0 >> 2U) & 1U, 1U);
}

TEST_F(GpioMsp430fr2355Test, SetMode_AlternateFunction_ClearsSel1Bit)
{
    mock_sel1 = static_cast<uint8_t>(1U << 2U);
    MockPin2::set_mode(ohal::gpio::PinMode::AlternateFunction);
    EXPECT_EQ((mock_sel1 >> 2U) & 1U, 0U);
}

TEST_F(GpioMsp430fr2355Test, SetMode_AlternateFunction_PreservesDirBit)
{
    mock_dir = static_cast<uint8_t>(1U << 2U); // Output mode pre-loaded
    MockPin2::set_mode(ohal::gpio::PinMode::AlternateFunction);
    EXPECT_EQ((mock_dir >> 2U) & 1U, 1U);
}

// ---------------------------------------------------------------------------
// set_mode(Analog) — treated as Input on MSP430: clears Dir, Sel0, Sel1
// ---------------------------------------------------------------------------

TEST_F(GpioMsp430fr2355Test, SetMode_Analog_ClearsDirBit)
{
    mock_dir = static_cast<uint8_t>(1U << 2U);
    MockPin2::set_mode(ohal::gpio::PinMode::Analog);
    EXPECT_EQ((mock_dir >> 2U) & 1U, 0U);
}

TEST_F(GpioMsp430fr2355Test, SetMode_Analog_ClearsSel0Bit)
{
    mock_sel0 = static_cast<uint8_t>(1U << 2U);
    MockPin2::set_mode(ohal::gpio::PinMode::Analog);
    EXPECT_EQ((mock_sel0 >> 2U) & 1U, 0U);
}

TEST_F(GpioMsp430fr2355Test, SetMode_Analog_ClearsSel1Bit)
{
    mock_sel1 = static_cast<uint8_t>(1U << 2U);
    MockPin2::set_mode(ohal::gpio::PinMode::Analog);
    EXPECT_EQ((mock_sel1 >> 2U) & 1U, 0U);
}

// ---------------------------------------------------------------------------
// set_pull(Up) — sets Ren bit and sets Out bit (pull-up = PxOUT=1)
// ---------------------------------------------------------------------------

TEST_F(GpioMsp430fr2355Test, SetPull_Up_SetsRenBit)
{
    MockPin2::set_pull(ohal::gpio::Pull::Up);
    EXPECT_EQ((mock_ren >> 2U) & 1U, 1U);
}

TEST_F(GpioMsp430fr2355Test, SetPull_Up_SetsOutBit)
{
    MockPin2::set_pull(ohal::gpio::Pull::Up);
    EXPECT_EQ((mock_out >> 2U) & 1U, 1U);
}

// ---------------------------------------------------------------------------
// set_pull(Down) — sets Ren bit and clears Out bit (pull-down = PxOUT=0)
// ---------------------------------------------------------------------------

TEST_F(GpioMsp430fr2355Test, SetPull_Down_SetsRenBit)
{
    MockPin2::set_pull(ohal::gpio::Pull::Down);
    EXPECT_EQ((mock_ren >> 2U) & 1U, 1U);
}

TEST_F(GpioMsp430fr2355Test, SetPull_Down_ClearsOutBit)
{
    mock_out = static_cast<uint8_t>(1U << 2U); // pre-load High
    MockPin2::set_pull(ohal::gpio::Pull::Down);
    EXPECT_EQ((mock_out >> 2U) & 1U, 0U);
}

// ---------------------------------------------------------------------------
// set_pull(None) — clears Ren bit; Out register must not be modified
// ---------------------------------------------------------------------------

TEST_F(GpioMsp430fr2355Test, SetPull_None_ClearsRenBit)
{
    mock_ren = static_cast<uint8_t>(1U << 2U); // pre-load enabled
    MockPin2::set_pull(ohal::gpio::Pull::None);
    EXPECT_EQ((mock_ren >> 2U) & 1U, 0U);
}

TEST_F(GpioMsp430fr2355Test, SetPull_None_DoesNotModifyOutRegister)
{
    mock_out = static_cast<uint8_t>(0xA5U); // sentinel: must remain unchanged
    MockPin2::set_pull(ohal::gpio::Pull::None);
    EXPECT_EQ(mock_out, static_cast<uint8_t>(0xA5U));
}

// ---------------------------------------------------------------------------
// Boundary pins: pin 0 and pin 7 use the correct bit positions
// ---------------------------------------------------------------------------

TEST_F(GpioMsp430fr2355Test, SetPin0_SetsBit0InOut)
{
    MockPin0::set();
    EXPECT_EQ(mock_out & 1U, 1U);
}

TEST_F(GpioMsp430fr2355Test, SetPin7_SetsBit7InOut)
{
    MockPin7::set();
    EXPECT_EQ((mock_out >> 7U) & 1U, 1U);
}

TEST_F(GpioMsp430fr2355Test, SetModePin0_Output_SetsDirBit0)
{
    MockPin0::set_mode(ohal::gpio::PinMode::Output);
    EXPECT_EQ(mock_dir & 1U, 1U);
}

TEST_F(GpioMsp430fr2355Test, SetModePin7_Output_SetsDirBit7)
{
    MockPin7::set_mode(ohal::gpio::PinMode::Output);
    EXPECT_EQ((mock_dir >> 7U) & 1U, 1U);
}

// ---------------------------------------------------------------------------
// Capability trait compile-time assertions
// ---------------------------------------------------------------------------

// Supported: pull resistors and alternate-function selection for pins 0–7
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 0>::value,
              "MSP430FR2355 PortA pin 0 must support pull");
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 7>::value,
              "MSP430FR2355 PortA pin 7 must support pull (boundary)");
static_assert(!ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 8>::value,
              "MSP430FR2355 PortA pin 8 must not support pull (out of range)");
static_assert(ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortA, 0>::value,
              "MSP430FR2355 PortA pin 0 must support alternate function");
static_assert(!ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortA, 8>::value,
              "MSP430FR2355 PortA pin 8 must not support alternate function (out of range)");

// Not supported: output type and output speed remain false_type
static_assert(!ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 0>::value,
              "MSP430FR2355 must not report supports_output_type");
static_assert(!ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortA, 0>::value,
              "MSP430FR2355 must not report supports_output_speed");

// ---------------------------------------------------------------------------
// Capability trait runtime tests
// ---------------------------------------------------------------------------

struct CapCase
{
    bool value;
    const char* name;
};

class GpioMsp430fr2355CapabilityTest : public ::testing::TestWithParam<CapCase>
{
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Msp430fr2355Capabilities, GpioMsp430fr2355CapabilityTest,
    ::testing::Values(
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 0>::value,
                "PortA_Pin0_Pull"},
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 7>::value,
                "PortA_Pin7_Pull_Boundary"},
        CapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortA, 0>::value,
                "PortA_Pin0_AlternateFunction"},
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortB, 3>::value,
                "PortB_Pin3_Pull"},
        CapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortC, 5>::value,
                "PortC_Pin5_AlternateFunction"},
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortD, 7>::value,
                "PortD_Pin7_Pull"},
        CapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortE, 0>::value,
                "PortE_Pin0_AlternateFunction"},
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortF, 7>::value,
                "PortF_Pin7_Pull"}),
    [](const ::testing::TestParamInfo<CapCase>& info) { return info.param.name; });
// clang-format on

TEST_P(GpioMsp430fr2355CapabilityTest, CapabilityIsTrue) { EXPECT_TRUE(GetParam().value); }

// ---------------------------------------------------------------------------
// Negative capability trait runtime tests (out-of-range and unsupported)
// ---------------------------------------------------------------------------

struct NegCapCase
{
    bool value;
    const char* name;
};

class GpioMsp430fr2355NegCapabilityTest : public ::testing::TestWithParam<NegCapCase>
{
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Msp430fr2355NegCapabilities, GpioMsp430fr2355NegCapabilityTest,
    ::testing::Values(
        NegCapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 8>::value,
                   "PortA_Pin8_Pull_OutOfRange"},
        NegCapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortA, 8>::value,
                   "PortA_Pin8_AF_OutOfRange"},
        NegCapCase{ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 0>::value,
                   "PortA_Pin0_OutputType_NotSupported"},
        NegCapCase{ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortA, 0>::value,
                   "PortA_Pin0_OutputSpeed_NotSupported"}),
    [](const ::testing::TestParamInfo<NegCapCase>& info) { return info.param.name; });
// clang-format on

TEST_P(GpioMsp430fr2355NegCapabilityTest, CapabilityIsFalse) { EXPECT_FALSE(GetParam().value); }

// ---------------------------------------------------------------------------
// Negative-compile tests (manual verification only)
// ---------------------------------------------------------------------------
//
// The following calls must not compile.  Verify by temporarily uncommenting
// the lines below and confirming a static_assert fires:
//
//   MockPin2::set_speed(ohal::gpio::Speed::High);
//   // expected: "ohal: MSP430FR2355 GPIO does not support configurable output speed."
//
//   MockPin2::set_output_type(ohal::gpio::OutputType::OpenDrain);
//   // expected: "ohal: MSP430FR2355 GPIO does not support configurable output type."

} // namespace
