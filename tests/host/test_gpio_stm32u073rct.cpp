#include "mock/mock_register.hpp"

#include <ohal/core/capabilities.hpp>
#include <ohal/gpio.hpp>
#include <ohal/platforms/stm32u0/models/stm32u073rct/capabilities.hpp>
#include <ohal/platforms/stm32u0/models/stm32u073rct/gpio.hpp>

#include <cstdint>

#include <gtest/gtest.h>

// ---------------------------------------------------------------------------
// Host-side tests for the STM32U073RCT (64-pin LQFP) GPIO model.
//
// The STM32U073RCT bonds out GPIOA, GPIOB, GPIOC, GPIOD (PD2 only) and GPIOF
// (PF0-PF3); GPIOE is not bonded out (DS14548, §4.1).
// These tests verify:
//   1. The capability traits report the correct values for the RCT package.
//   2. The Pin<>/Port<> specialisations resolve to the correct hardware base
//      addresses (i.e. the wiring through the thin-wrapper header is correct).
//   3. Basic behavioural correctness via the mock-register infrastructure.
// ---------------------------------------------------------------------------

namespace
{

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
struct MockGpioRegs
{
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

using MockPin5 = ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<5U, MockGpioRegs>;
using MockPort = ohal::platforms::stm32u0::stm32u083::GpioPortImpl<MockGpioRegs>;

// ---------------------------------------------------------------------------
// Test fixture
// ---------------------------------------------------------------------------

class GpioStm32u073RctTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
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
// Basic behavioural tests
// ---------------------------------------------------------------------------

TEST_F(GpioStm32u073RctTest, Set_WritesBitToBsrr)
{
    MockPin5::set();
    EXPECT_EQ(mock_bsrr, 1U << 5U);
}

TEST_F(GpioStm32u073RctTest, Clear_WritesBsrrResetBit)
{
    MockPin5::clear();
    EXPECT_EQ(mock_bsrr, 1U << 21U);
}

TEST_F(GpioStm32u073RctTest, SetMode_Output_WritesModerBits)
{
    MockPin5::set_mode(ohal::gpio::PinMode::Output);
    EXPECT_EQ(mock_moder & (0b11U << 10U), 0b01U << 10U);
}

TEST_F(GpioStm32u073RctTest, SetMode_Input_WritesModerBits)
{
    mock_moder = 0b11U << 10U;
    MockPin5::set_mode(ohal::gpio::PinMode::Input);
    EXPECT_EQ(mock_moder & (0b11U << 10U), 0U);
}

TEST_F(GpioStm32u073RctTest, ReadInput_ReturnsHigh_WhenIdrBitSet)
{
    mock_idr = 1U << 5U;
    EXPECT_EQ(MockPin5::read_input(), ohal::gpio::Level::High);
}

TEST_F(GpioStm32u073RctTest, PortSet_WritesMaskToBsrrLow16Bits)
{
    MockPort::set(0x00FFU);
    EXPECT_EQ(mock_bsrr, 0x00FFU);
}

TEST_F(GpioStm32u073RctTest, PortClear_WritesMaskToBsrrHigh16Bits)
{
    MockPort::clear(0x00FFU);
    EXPECT_EQ(mock_bsrr, 0x00FF0000U);
}

TEST_F(GpioStm32u073RctTest, PortWrite_CombinesSetAndClearMasksInSingleBsrrWrite)
{
    MockPort::write(0x000FU, 0x00F0U);
    EXPECT_EQ(mock_bsrr, 0x00F0000FU);
}

// ---------------------------------------------------------------------------
// Capability trait tests
// ---------------------------------------------------------------------------

struct CapCase
{
    bool value;
    const char* name;
};

class GpioStm32u073RctCapabilityTest : public ::testing::TestWithParam<CapCase>
{
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    BondedPortCapabilities, GpioStm32u073RctCapabilityTest,
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

TEST_P(GpioStm32u073RctCapabilityTest, CapabilityIsTrue) { EXPECT_TRUE(GetParam().value); }

struct InvalidCapCase
{
    bool value;
    const char* name;
};

class GpioStm32u073RctInvalidCapTest : public ::testing::TestWithParam<InvalidCapCase>
{
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    UnbondedAndOutOfRangeCapabilities, GpioStm32u073RctInvalidCapTest,
    ::testing::Values(
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 16>::value,
            "PortA_Pin16_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortD, 16>::value,
            "PortD_Pin16_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortD, 0>::value,
            "PortD_Pin0_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortE, 0>::value,
            "PortE_Pin0_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortF, 4>::value,
            "PortF_Pin4_OutputType"}),
    [](const ::testing::TestParamInfo<InvalidCapCase>& info) { return info.param.name; });
// clang-format on

TEST_P(GpioStm32u073RctInvalidCapTest, CapabilityIsFalse) { EXPECT_FALSE(GetParam().value); }

// ---------------------------------------------------------------------------
// Port-wiring tests
// ---------------------------------------------------------------------------

namespace wiring = ohal::platforms::stm32u0::stm32u083;

struct WiringCase
{
    uintptr_t actual;
    uintptr_t expected;
    const char* name;
};

class GpioStm32u073RctWiringTest : public ::testing::TestWithParam<WiringCase>
{
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    PortBsrrAddresses, GpioStm32u073RctWiringTest,
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

TEST_P(GpioStm32u073RctWiringTest, BsrrAddressMatchesHardwareBase)
{
    EXPECT_EQ(GetParam().actual, GetParam().expected);
}

} // namespace
