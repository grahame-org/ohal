#include "mock/mock_register.hpp"

#include <ohal/core/capabilities.hpp>
#include <ohal/core/field.hpp>
#include <ohal/gpio.hpp>
#include <ohal/platforms/stm32u0/models/stm32u031g8y/capabilities.hpp>
#include <ohal/platforms/stm32u0/models/stm32u031g8y/gpio.hpp>

#include <cstdint>

#include <gtest/gtest.h>

// ---------------------------------------------------------------------------
// Host-side tests for the STM32U031G8Y (WLCSP27) GPIO model.
//
// The STM32U031G8Y bonds out
//   GPIOA (0-14; PA15 absent), GPIOB (0-1, 5-7), GPIOC (14-15), GPIOF (2-3);
//   GPIOD and GPIOE are not bonded out.
// These tests verify:
//   1. Capability traits report the correct values for the G8Y package.
//   2. Pin<>/Port<> specialisations resolve to the correct hardware addresses.
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

class GpioStm32u031G8YTest : public ::testing::Test
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

TEST_F(GpioStm32u031G8YTest, Set_WritesBitToBsrr)
{
    MockPin5::set();
    EXPECT_EQ(mock_bsrr, 1U << 5U);
}

TEST_F(GpioStm32u031G8YTest, Clear_WritesBsrrResetBit)
{
    MockPin5::clear();
    EXPECT_EQ(mock_bsrr, 1U << 21U);
}

TEST_F(GpioStm32u031G8YTest, SetMode_Output_WritesModerBits)
{
    MockPin5::set_mode(ohal::gpio::PinMode::Output);
    EXPECT_EQ(mock_moder & (0b11U << 10U), 0b01U << 10U);
}

TEST_F(GpioStm32u031G8YTest, SetMode_Input_WritesModerBits)
{
    mock_moder = 0b11U << 10U;
    MockPin5::set_mode(ohal::gpio::PinMode::Input);
    EXPECT_EQ(mock_moder & (0b11U << 10U), 0U);
}

TEST_F(GpioStm32u031G8YTest, ReadInput_ReturnsHigh_WhenIdrBitSet)
{
    mock_idr = 1U << 5U;
    EXPECT_EQ(MockPin5::read_input(), ohal::gpio::Level::High);
}

TEST_F(GpioStm32u031G8YTest, PortSet_WritesMaskToBsrrLow16Bits)
{
    MockPort::set(0x00FFU);
    EXPECT_EQ(mock_bsrr, 0x00FFU);
}

TEST_F(GpioStm32u031G8YTest, PortClear_WritesMaskToBsrrHigh16Bits)
{
    MockPort::clear(0x00FFU);
    EXPECT_EQ(mock_bsrr, 0x00FF0000U);
}

TEST_F(GpioStm32u031G8YTest, PortWrite_CombinesSetAndClearMasksInSingleBsrrWrite)
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

class GpioStm32u031G8YCapabilityTest : public ::testing::TestWithParam<CapCase>
{
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    BondedPortCapabilities, GpioStm32u031G8YCapabilityTest,
    ::testing::Values(
        CapCase{ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 0>::value,
                "PortA_Pin0_OutputType"},
        CapCase{ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortA, 14>::value,
                "PortA_Pin14_OutputSpeed"},
        CapCase{ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortB, 0>::value,
                "PortB_Pin0_OutputType"},
        CapCase{ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortB, 1>::value,
                "PortB_Pin1_OutputType"},
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortB, 5>::value,
                "PortB_Pin5_Pull"},
        CapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortB, 7>::value,
                "PortB_Pin7_AlternateFunction"},
        CapCase{ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortC, 14>::value,
                "PortC_Pin14_OutputType"},
        CapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortC, 15>::value,
                "PortC_Pin15_AlternateFunction"},
        CapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortF, 2>::value,
                "PortF_Pin2_AlternateFunction"},
        CapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortF, 3>::value,
                "PortF_Pin3_AlternateFunction"}),
    [](const ::testing::TestParamInfo<CapCase>& info) { return info.param.name; });
// clang-format on

TEST_P(GpioStm32u031G8YCapabilityTest, CapabilityIsTrue) { EXPECT_TRUE(GetParam().value); }

struct InvalidCapCase
{
    bool value;
    const char* name;
};

class GpioStm32u031G8YInvalidCapTest : public ::testing::TestWithParam<InvalidCapCase>
{
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    UnbondedAndOutOfRangeCapabilities, GpioStm32u031G8YInvalidCapTest,
    ::testing::Values(
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 15>::value,
            "PortA_Pin15_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 16>::value,
            "PortA_Pin16_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortD, 0>::value,
            "PortD_Pin0_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_pull<ohal::gpio::PortD, 0>::value,
            "PortD_Pin0_Pull"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortE, 0>::value,
            "PortE_Pin0_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_pull<ohal::gpio::PortE, 0>::value,
            "PortE_Pin0_Pull"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortB, 2>::value,
            "PortB_Pin2_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortB, 4>::value,
            "PortB_Pin4_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortB, 8>::value,
            "PortB_Pin8_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortC, 0>::value,
            "PortC_Pin0_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortC, 13>::value,
            "PortC_Pin13_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortF, 0>::value,
            "PortF_Pin0_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortF, 4>::value,
            "PortF_Pin4_OutputType"}),
    [](const ::testing::TestParamInfo<InvalidCapCase>& info) { return info.param.name; });
// clang-format on

TEST_P(GpioStm32u031G8YInvalidCapTest, CapabilityIsFalse) { EXPECT_FALSE(GetParam().value); }

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

class GpioStm32u031G8YWiringTest : public ::testing::TestWithParam<WiringCase>
{
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    PortBsrrAddresses, GpioStm32u031G8YWiringTest,
    ::testing::Values(
        WiringCase{ohal::gpio::Pin<ohal::gpio::PortA, 0>::BsrrSet::reg_type::address,
                   wiring::kGpioABase + wiring::kBsrrOffset, "PortA"},
        WiringCase{ohal::gpio::Pin<ohal::gpio::PortB, 0>::BsrrSet::reg_type::address,
                   wiring::kGpioBBase + wiring::kBsrrOffset, "PortB"},
        WiringCase{ohal::gpio::Pin<ohal::gpio::PortC, 14>::BsrrSet::reg_type::address,
                   wiring::kGpioCBase + wiring::kBsrrOffset, "PortC"},
        WiringCase{ohal::gpio::Pin<ohal::gpio::PortF, 2>::BsrrSet::reg_type::address,
                   wiring::kGpioFBase + wiring::kBsrrOffset, "PortF"}),
    [](const ::testing::TestParamInfo<WiringCase>& info) { return info.param.name; });
// clang-format on

TEST_P(GpioStm32u031G8YWiringTest, BsrrAddressMatchesHardwareBase)
{
    EXPECT_EQ(GetParam().actual, GetParam().expected);
}

} // namespace
