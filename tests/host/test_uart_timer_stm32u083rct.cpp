#include <ohal/core/access.hpp>
#include <ohal/core/capabilities.hpp>
#include <ohal/gpio.hpp>
#include <ohal/platforms/stm32u0/models/stm32u083rct/capabilities.hpp>
#include <ohal/platforms/stm32u0/models/stm32u083rct/gpio.hpp>
#include <ohal/platforms/stm32u0/models/stm32u083rct/timer.hpp>
#include <ohal/platforms/stm32u0/models/stm32u083rct/uart.hpp>

#include <type_traits>

#include <gtest/gtest.h>

namespace {

using namespace ohal::platforms::stm32u0::stm32u083;

struct BoolCase {
  bool value;
  const char* name;
};

class Stm32u083RctUartPortMappingTest : public ::testing::TestWithParam<BoolCase> {};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    TestProjectUartPortMapping, Stm32u083RctUartPortMappingTest,
    ::testing::Values(
        BoolCase{
            ohal::uart::Port<Usart2Tag>::Cr1::address == (kUsart2Base + kUsartCr1Offset),
            "Usart2PortMapsToUsart2Base"},
        BoolCase{
            ohal::uart::Port<Lpuart3Tag>::Cr1::address == (kLpuart3Base + kUsartCr1Offset),
            "Lpuart3PortAvailableForStm32u083"}),
    [](const ::testing::TestParamInfo<BoolCase>& info) { return info.param.name; });
// clang-format on

TEST_P(Stm32u083RctUartPortMappingTest, Mapping_MatchesExpected) { EXPECT_TRUE(GetParam().value); }

class Stm32u083RctUartFifoCapabilityTest : public ::testing::TestWithParam<BoolCase> {};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    TestProjectUartFifoCapabilities, Stm32u083RctUartFifoCapabilityTest,
    ::testing::Values(
        BoolCase{ohal::uart::capabilities::supports_fifo<Usart2Tag>::value, "Usart2SupportsFifo"},
        BoolCase{!ohal::uart::capabilities::supports_fifo<Usart3Tag>::value, "Usart3DoesNotSupportFifo"},
        BoolCase{
            ohal::uart::capabilities::tx_rx_fifo_size_bytes<Usart2Tag>::value == kUsartFifoSizeBytes,
            "Usart2FifoSizeMatchesSpec"}),
    [](const ::testing::TestParamInfo<BoolCase>& info) { return info.param.name; });
// clang-format on

TEST_P(Stm32u083RctUartFifoCapabilityTest, Capability_MatchesExpected) {
  EXPECT_TRUE(GetParam().value);
}

class Stm32u083RctUartDualClockCapabilityTest : public ::testing::TestWithParam<BoolCase> {};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    TestProjectUartDualClockCapabilities, Stm32u083RctUartDualClockCapabilityTest,
    ::testing::Values(
        BoolCase{ohal::uart::capabilities::supports_dual_clock_domain<Lpuart1Tag>::value,
                 "Lpuart1SupportsDualClockDomain"},
        BoolCase{ohal::uart::capabilities::supports_dual_clock_domain<Usart2Tag>::value,
                 "Usart2SupportsDualClockDomain"}),
    [](const ::testing::TestParamInfo<BoolCase>& info) { return info.param.name; });
// clang-format on

TEST_P(Stm32u083RctUartDualClockCapabilityTest, Capability_MatchesExpected) {
  EXPECT_TRUE(GetParam().value);
}

TEST(Stm32u083RctGpioCoverageTest, TestProjectLedPinOutputCapability_MatchesExpected) {
  EXPECT_TRUE((ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 5>::value));
}

class Stm32u083RctTimerChannelMappingTest : public ::testing::TestWithParam<BoolCase> {};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Tim2ChannelCoverage, Stm32u083RctTimerChannelMappingTest,
    ::testing::Values(
        BoolCase{
            std::is_same_v<ohal::timer::Channel<Tim2, 0>::Ccr, Tim2::Ccr1>,
            "Channel0MapsToCcr1"},
        BoolCase{
            std::is_same_v<ohal::timer::Channel<Tim2, 1>::Ccr, Tim2::Ccr2>,
            "Channel1MapsToCcr2"},
        BoolCase{
            std::is_same_v<ohal::timer::Channel<Tim2, 2>::Ccr, Tim2::Ccr3>,
            "Channel2MapsToCcr3"},
        BoolCase{
            std::is_same_v<ohal::timer::Channel<Tim2, 3>::Ccr, Tim2::Ccr4>,
            "Channel3MapsToCcr4"}),
    [](const ::testing::TestParamInfo<BoolCase>& info) { return info.param.name; });
// clang-format on

TEST_P(Stm32u083RctTimerChannelMappingTest, Mapping_MatchesExpected) {
  EXPECT_TRUE(GetParam().value);
}

TEST(Stm32u083RctTimerAccessTest, Tim2EgrAccessMode_MatchesExpected) {
  EXPECT_TRUE((ohal::timer::Channel<Tim2, 0>::Egr::access == ohal::core::Access::WriteOnly));
}

} // namespace
