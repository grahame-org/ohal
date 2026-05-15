
#include <ohal/core/capabilities.hpp>
#include <ohal/gpio.hpp>
#include <ohal/platforms/stm32u0/models/stm32u073mbi/capabilities.hpp>
#include <ohal/platforms/stm32u0/models/stm32u073mbi/gpio.hpp>

#include <gtest/gtest.h>

// ---------------------------------------------------------------------------
// Host-side tests for the STM32U073MBI (UFBGA81) GPIO model.
//
// Bonds out GPIOA, GPIOB, GPIOC, GPIOD (bits 0-6 and 8-13),
// GPIOE (PE3, PE7-PE9; PE3 only on UFBGA81), GPIOF (PF0-PF3 only).
// Behavioural mock tests are consolidated in test_gpio_stm32u073mci.cpp.
// This file validates that the per-package wrapper header compiles
// and wires correctly.
// ---------------------------------------------------------------------------

namespace {

struct CapCase {
  bool value;
  const char* name;
};

class GpioStm32u073MBICapabilityTest : public ::testing::TestWithParam<CapCase> {};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    BondedPortCapabilities, GpioStm32u073MBICapabilityTest,
    ::testing::Values(
        CapCase{ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 0>::value,
                "PortA_Pin0_OutputType"},
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortD, 2>::value,
                "PortD_Pin2_Pull"},
        CapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortE, 3>::value,
                "PortE_Pin3_AlternateFunction"},
        CapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortE, 7>::value,
                "PortE_Pin7_AlternateFunction"},
        CapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortF, 3>::value,
                "PortF_Pin3_AlternateFunction"}),
    [](const ::testing::TestParamInfo<CapCase>& info) { return info.param.name; });
// clang-format on

TEST_P(GpioStm32u073MBICapabilityTest, CapabilityIsTrue) { EXPECT_TRUE(GetParam().value); }

struct InvalidCapCase {
  bool value;
  const char* name;
};

class GpioStm32u073MBIInvalidCapTest : public ::testing::TestWithParam<InvalidCapCase> {};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    UnbondedAndOutOfRangeCapabilities, GpioStm32u073MBIInvalidCapTest,
    ::testing::Values(
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortD, 7>::value,
            "PortD_Pin7_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortE, 0>::value,
            "PortE_Pin0_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortF, 4>::value,
            "PortF_Pin4_AlternateFunction"}),
    [](const ::testing::TestParamInfo<InvalidCapCase>& info) { return info.param.name; });
// clang-format on

TEST_P(GpioStm32u073MBIInvalidCapTest, CapabilityIsFalse) { EXPECT_FALSE(GetParam().value); }

// ---------------------------------------------------------------------------
// Port-wiring tests
// ---------------------------------------------------------------------------

namespace wiring = ohal::platforms::stm32u0::stm32u083;

struct WiringCase {
  uintptr_t actual;
  uintptr_t expected;
  const char* name;
};

class GpioStm32u073MBIWiringTest : public ::testing::TestWithParam<WiringCase> {};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    PortBsrrAddresses, GpioStm32u073MBIWiringTest,
    ::testing::Values(
        WiringCase{ohal::gpio::Pin<ohal::gpio::PortA, 0>::BsrrSet::reg_type::address,
                   wiring::kGpioABase + wiring::kBsrrOffset, "PortA"},
        WiringCase{ohal::gpio::Pin<ohal::gpio::PortD, 2>::BsrrSet::reg_type::address,
                   wiring::kGpioDBase + wiring::kBsrrOffset, "PortD"},
        WiringCase{ohal::gpio::Pin<ohal::gpio::PortE, 3>::BsrrSet::reg_type::address,
                   wiring::kGpioEBase + wiring::kBsrrOffset, "PortE"},
        WiringCase{ohal::gpio::Pin<ohal::gpio::PortF, 0>::BsrrSet::reg_type::address,
                   wiring::kGpioFBase + wiring::kBsrrOffset, "PortF"}),
    [](const ::testing::TestParamInfo<WiringCase>& info) { return info.param.name; });
// clang-format on

TEST_P(GpioStm32u073MBIWiringTest, BsrrAddressMatchesHardwareBase) {
  EXPECT_EQ(GetParam().actual, GetParam().expected);
}

} // namespace
