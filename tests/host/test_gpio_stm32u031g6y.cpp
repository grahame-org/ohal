
#include <ohal/core/capabilities.hpp>
#include <ohal/gpio.hpp>
#include <ohal/platforms/stm32u0/models/stm32u031g6y/capabilities.hpp>
#include <ohal/platforms/stm32u0/models/stm32u031g6y/gpio.hpp>

#include <gtest/gtest.h>

// ---------------------------------------------------------------------------
// Host-side tests for the STM32U031G6Y (WLCSP27) GPIO model.
//
// The STM32U031G6Y bonds out
//   GPIOA (0-14; PA15 absent), GPIOB (0-1, 5-7), GPIOC (14-15), GPIOF (2-3);
//   GPIOD and GPIOE are not bonded out.
// Behavioural mock tests are consolidated in test_gpio_stm32u031g8y.cpp.
// This file validates that the G6Y per-package wrapper header compiles
// and wires correctly.
// ---------------------------------------------------------------------------

namespace
{

// ---------------------------------------------------------------------------
// Capability trait tests
// ---------------------------------------------------------------------------

struct CapCase
{
    bool value;
    const char* name;
};

class GpioStm32u031G6YCapabilityTest : public ::testing::TestWithParam<CapCase>
{
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    BondedPortCapabilities, GpioStm32u031G6YCapabilityTest,
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

TEST_P(GpioStm32u031G6YCapabilityTest, CapabilityIsTrue) { EXPECT_TRUE(GetParam().value); }

struct InvalidCapCase
{
    bool value;
    const char* name;
};

class GpioStm32u031G6YInvalidCapTest : public ::testing::TestWithParam<InvalidCapCase>
{
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    UnbondedAndOutOfRangeCapabilities, GpioStm32u031G6YInvalidCapTest,
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

TEST_P(GpioStm32u031G6YInvalidCapTest, CapabilityIsFalse) { EXPECT_FALSE(GetParam().value); }

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

class GpioStm32u031G6YWiringTest : public ::testing::TestWithParam<WiringCase>
{
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    PortBsrrAddresses, GpioStm32u031G6YWiringTest,
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

TEST_P(GpioStm32u031G6YWiringTest, BsrrAddressMatchesHardwareBase)
{
    EXPECT_EQ(GetParam().actual, GetParam().expected);
}

} // namespace
