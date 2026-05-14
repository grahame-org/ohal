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

TEST(Stm32u083RctUartPortMappingTest, Usart2PortMapsToUsart2Base) {
  EXPECT_EQ(ohal::uart::Port<Usart2Tag>::Cr1::address, kUsart2Base + kUsartCr1Offset);
}

TEST(Stm32u083RctUartPortMappingTest, Lpuart3PortAvailableForStm32u083) {
  EXPECT_EQ(ohal::uart::Port<Lpuart3Tag>::Cr1::address, kLpuart3Base + kUsartCr1Offset);
}

TEST(Stm32u083RctUartFifoCapabilityTest, Usart2SupportsFifo) {
  EXPECT_TRUE((ohal::uart::capabilities::supports_fifo<Usart2Tag>::value));
}

TEST(Stm32u083RctUartFifoCapabilityTest, Usart3DoesNotSupportFifo) {
  EXPECT_FALSE((ohal::uart::capabilities::supports_fifo<Usart3Tag>::value));
}

TEST(Stm32u083RctUartFifoCapabilityTest, Usart2FifoSizeMatchesSpec) {
  EXPECT_EQ(ohal::uart::capabilities::tx_rx_fifo_size_bytes<Usart2Tag>::value, kUsartFifoSizeBytes);
}

TEST(Stm32u083RctUartDualClockCapabilityTest, Lpuart1SupportsDualClockDomain) {
  EXPECT_TRUE((ohal::uart::capabilities::supports_dual_clock_domain<Lpuart1Tag>::value));
}

TEST(Stm32u083RctUartDualClockCapabilityTest, Usart2SupportsDualClockDomain) {
  EXPECT_TRUE((ohal::uart::capabilities::supports_dual_clock_domain<Usart2Tag>::value));
}

TEST(Stm32u083RctGpioCoverageTest, TestProjectLedPinOutputCapabilityMatchesExpected) {
  EXPECT_TRUE((ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 5>::value));
}

TEST(Stm32u083RctTimerAccessTest, Tim2EgrAccessModeMatchesExpected) {
  EXPECT_EQ((ohal::timer::Channel<Tim2, 0>::Egr::access), ohal::core::Access::WriteOnly);
}

struct Tim2ChannelMapCase {
  bool value;
  const char* name;
};

class Stm32u083RctTimerChannelMappingTest : public ::testing::TestWithParam<Tim2ChannelMapCase> {};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Tim2ChannelMappingChecks, Stm32u083RctTimerChannelMappingTest,
    ::testing::Values(
        Tim2ChannelMapCase{std::is_same_v<ohal::timer::Channel<Tim2, 0>::Ccr, Tim2::Ccr1>, "Channel0MapsToCcr1"},
        Tim2ChannelMapCase{std::is_same_v<ohal::timer::Channel<Tim2, 1>::Ccr, Tim2::Ccr2>, "Channel1MapsToCcr2"},
        Tim2ChannelMapCase{std::is_same_v<ohal::timer::Channel<Tim2, 2>::Ccr, Tim2::Ccr3>, "Channel2MapsToCcr3"},
        Tim2ChannelMapCase{std::is_same_v<ohal::timer::Channel<Tim2, 3>::Ccr, Tim2::Ccr4>, "Channel3MapsToCcr4"}),
    [](const ::testing::TestParamInfo<Tim2ChannelMapCase>& info) { return info.param.name; });
// clang-format on

TEST_P(Stm32u083RctTimerChannelMappingTest, ChannelToCcrMappingMatchesExpected) {
  EXPECT_TRUE(GetParam().value);
}

} // namespace
