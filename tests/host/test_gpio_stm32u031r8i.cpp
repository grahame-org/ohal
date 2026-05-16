
#include <ohal/core/capabilities.hpp>
#include <ohal/gpio.hpp>
#include <ohal/platforms/stm32u0/models/stm32u031r8i/capabilities.hpp>
#include <ohal/platforms/stm32u0/models/stm32u031r8i/gpio.hpp>

#include <gtest/gtest.h>

// ---------------------------------------------------------------------------
// Host-side tests for the STM32U031R8I (UFBGA64) GPIO model.
//
// The STM32U031R8I bonds out
//   GPIOA, GPIOB, GPIOC (0-15), GPIOD (PD2 only), and GPIOF (PF0-PF3);
//   GPIOE is not bonded out.
// Behavioural mock tests are consolidated in test_gpio_stm32u031r8t.cpp.
// This file validates that the R8I per-package wrapper header compiles
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

class GpioStm32u031R8ICapabilityTest : public ::testing::TestWithParam<CapCase>
{
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    BondedPortCapabilities, GpioStm32u031R8ICapabilityTest,
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

TEST_P(GpioStm32u031R8ICapabilityTest, CapabilityIsTrue) { EXPECT_TRUE(GetParam().value); }

struct InvalidCapCase
{
    bool value;
    const char* name;
};

class GpioStm32u031R8IInvalidCapTest : public ::testing::TestWithParam<InvalidCapCase>
{
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    UnbondedAndOutOfRangeCapabilities, GpioStm32u031R8IInvalidCapTest,
    ::testing::Values(
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 16>::value,
            "PortA_Pin16_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortA, 16>::value,
            "PortA_Pin16_OutputSpeed"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 16>::value,
            "PortA_Pin16_Pull"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortA, 16>::value,
            "PortA_Pin16_AlternateFunction"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortD, 16>::value,
            "PortD_Pin16_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortD, 16>::value,
            "PortD_Pin16_OutputSpeed"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_pull<ohal::gpio::PortD, 16>::value,
            "PortD_Pin16_Pull"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortD, 16>::value,
            "PortD_Pin16_AlternateFunction"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortE, 0>::value,
            "PortE_Pin0_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortE, 0>::value,
            "PortE_Pin0_OutputSpeed"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_pull<ohal::gpio::PortE, 0>::value,
            "PortE_Pin0_Pull"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortE, 0>::value,
            "PortE_Pin0_AlternateFunction"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortD, 0>::value,
            "PortD_Pin0_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortD, 0>::value,
            "PortD_Pin0_AlternateFunction"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortF, 4>::value,
            "PortF_Pin4_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortF, 4>::value,
            "PortF_Pin4_AlternateFunction"}),
    [](const ::testing::TestParamInfo<InvalidCapCase>& info) { return info.param.name; });
// clang-format on

TEST_P(GpioStm32u031R8IInvalidCapTest, CapabilityIsFalse) { EXPECT_FALSE(GetParam().value); }

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

class GpioStm32u031R8IWiringTest : public ::testing::TestWithParam<WiringCase>
{
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    PortBsrrAddresses, GpioStm32u031R8IWiringTest,
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

TEST_P(GpioStm32u031R8IWiringTest, BsrrAddressMatchesHardwareBase)
{
    EXPECT_EQ(GetParam().actual, GetParam().expected);
}

} // namespace
