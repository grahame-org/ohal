#include <ohal/core/access.hpp>
#include <ohal/core/capabilities.hpp>
#include <ohal/gpio.hpp>
#include <ohal/platforms/stm32u0/models/stm32u083rct/capabilities.hpp>
#include <ohal/platforms/stm32u0/models/stm32u083rct/gpio.hpp>
#include <ohal/platforms/stm32u0/models/stm32u083rct/timer.hpp>
#include <ohal/platforms/stm32u0/models/stm32u083rct/uart.hpp>
#include <ohal/platforms/stm32u0/models/stm32u083rct/uart_capabilities.hpp>

#include <type_traits>

#include <gtest/gtest.h>

namespace {

using namespace ohal::platforms::stm32u0::stm32u083;

// ─── Port base address tests ──────────────────────────────────────────────────

TEST(Stm32u083RctUartPortMappingTest, Usart2PortMapsToUsart2Base) {
  EXPECT_EQ(ohal::uart::Port<Usart2Tag>::Cr1::address, kUsart2Base + kUsartCr1Offset);
}

TEST(Stm32u083RctUartPortMappingTest, Lpuart3PortAvailableForStm32u083) {
  EXPECT_EQ(ohal::uart::Port<Lpuart3Tag>::Cr1::address, kLpuart3Base + kUsartCr1Offset);
}

struct UartPortBaseCase {
  uintptr_t actual_address;
  uintptr_t expected_address;
  const char* name;
};

class Stm32u083RctUartPortBaseAddressTest : public ::testing::TestWithParam<UartPortBaseCase> {};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    PortBaseAddresses, Stm32u083RctUartPortBaseAddressTest,
    ::testing::Values(
        UartPortBaseCase{ohal::uart::Port<Usart1Tag>::Cr1::address,
                         kUsart1Base + kUsartCr1Offset, "Usart1Cr1"},
        UartPortBaseCase{ohal::uart::Port<Usart3Tag>::Cr1::address,
                         kUsart3Base + kUsartCr1Offset, "Usart3Cr1"},
        UartPortBaseCase{ohal::uart::Port<Usart4Tag>::Cr1::address,
                         kUsart4Base + kUsartCr1Offset, "Usart4Cr1"},
        UartPortBaseCase{ohal::uart::Port<Lpuart1Tag>::Cr1::address,
                         kLpuart1Base + kUsartCr1Offset, "Lpuart1Cr1"},
        UartPortBaseCase{ohal::uart::Port<Lpuart2Tag>::Cr1::address,
                         kLpuart2Base + kUsartCr1Offset, "Lpuart2Cr1"}),
    [](const ::testing::TestParamInfo<UartPortBaseCase>& info) {
      return info.param.name;
    });
// clang-format on

TEST_P(Stm32u083RctUartPortBaseAddressTest, Cr1AddressMatchesExpected) {
  EXPECT_EQ(GetParam().actual_address, GetParam().expected_address);
}

// ─── Capability tests ─────────────────────────────────────────────────────────
//
// Each INSTANTIATE_TEST_SUITE_P block covers one capability from Table 200
// (RM0503 §34.4).  Values encode both positive and negative assertions as a
// bool so that a single EXPECT_TRUE drives every case uniformly.

struct UartCapCase {
  bool value;
  const char* name;
};

class Stm32u083RctUartCapabilityTest : public ::testing::TestWithParam<UartCapCase> {};

TEST_P(Stm32u083RctUartCapabilityTest, CapabilityMatchesExpected) { EXPECT_TRUE(GetParam().value); }

// clang-format off

// Hardware flow control — supported by all tiers (full / basic / low-power).
INSTANTIATE_TEST_SUITE_P(
    HardwareFlowControl, Stm32u083RctUartCapabilityTest,
    ::testing::Values(
        UartCapCase{ohal::uart::capabilities::supports_hardware_flow_control<Usart1Tag>::value,  "Usart1Supports"},
        UartCapCase{ohal::uart::capabilities::supports_hardware_flow_control<Usart2Tag>::value,  "Usart2Supports"},
        UartCapCase{ohal::uart::capabilities::supports_hardware_flow_control<Usart3Tag>::value,  "Usart3Supports"},
        UartCapCase{ohal::uart::capabilities::supports_hardware_flow_control<Usart4Tag>::value,  "Usart4Supports"},
        UartCapCase{ohal::uart::capabilities::supports_hardware_flow_control<Lpuart1Tag>::value, "Lpuart1Supports"},
        UartCapCase{ohal::uart::capabilities::supports_hardware_flow_control<Lpuart2Tag>::value, "Lpuart2Supports"},
        UartCapCase{ohal::uart::capabilities::supports_hardware_flow_control<Lpuart3Tag>::value, "Lpuart3Supports"}),
    [](const ::testing::TestParamInfo<UartCapCase>& info) { return info.param.name; });

// DMA — supported by all tiers.
INSTANTIATE_TEST_SUITE_P(
    Dma, Stm32u083RctUartCapabilityTest,
    ::testing::Values(
        UartCapCase{ohal::uart::capabilities::supports_dma<Usart1Tag>::value,  "Usart1Supports"},
        UartCapCase{ohal::uart::capabilities::supports_dma<Usart2Tag>::value,  "Usart2Supports"},
        UartCapCase{ohal::uart::capabilities::supports_dma<Usart3Tag>::value,  "Usart3Supports"},
        UartCapCase{ohal::uart::capabilities::supports_dma<Usart4Tag>::value,  "Usart4Supports"},
        UartCapCase{ohal::uart::capabilities::supports_dma<Lpuart1Tag>::value, "Lpuart1Supports"},
        UartCapCase{ohal::uart::capabilities::supports_dma<Lpuart2Tag>::value, "Lpuart2Supports"},
        UartCapCase{ohal::uart::capabilities::supports_dma<Lpuart3Tag>::value, "Lpuart3Supports"}),
    [](const ::testing::TestParamInfo<UartCapCase>& info) { return info.param.name; });

// Multiprocessor communication — supported by all tiers.
INSTANTIATE_TEST_SUITE_P(
    MultiprocessorCommunication, Stm32u083RctUartCapabilityTest,
    ::testing::Values(
        UartCapCase{ohal::uart::capabilities::supports_multiprocessor_communication<Usart1Tag>::value,  "Usart1Supports"},
        UartCapCase{ohal::uart::capabilities::supports_multiprocessor_communication<Usart2Tag>::value,  "Usart2Supports"},
        UartCapCase{ohal::uart::capabilities::supports_multiprocessor_communication<Usart3Tag>::value,  "Usart3Supports"},
        UartCapCase{ohal::uart::capabilities::supports_multiprocessor_communication<Usart4Tag>::value,  "Usart4Supports"},
        UartCapCase{ohal::uart::capabilities::supports_multiprocessor_communication<Lpuart1Tag>::value, "Lpuart1Supports"},
        UartCapCase{ohal::uart::capabilities::supports_multiprocessor_communication<Lpuart2Tag>::value, "Lpuart2Supports"},
        UartCapCase{ohal::uart::capabilities::supports_multiprocessor_communication<Lpuart3Tag>::value, "Lpuart3Supports"}),
    [](const ::testing::TestParamInfo<UartCapCase>& info) { return info.param.name; });

// Synchronous mode — full and basic tiers only; low-power instances do not support it.
INSTANTIATE_TEST_SUITE_P(
    SynchronousMode, Stm32u083RctUartCapabilityTest,
    ::testing::Values(
        UartCapCase{ohal::uart::capabilities::supports_synchronous_mode<Usart1Tag>::value,   "Usart1Supports"},
        UartCapCase{ohal::uart::capabilities::supports_synchronous_mode<Usart2Tag>::value,   "Usart2Supports"},
        UartCapCase{ohal::uart::capabilities::supports_synchronous_mode<Usart3Tag>::value,   "Usart3Supports"},
        UartCapCase{ohal::uart::capabilities::supports_synchronous_mode<Usart4Tag>::value,   "Usart4Supports"},
        UartCapCase{!ohal::uart::capabilities::supports_synchronous_mode<Lpuart1Tag>::value, "Lpuart1DoesNotSupport"},
        UartCapCase{!ohal::uart::capabilities::supports_synchronous_mode<Lpuart2Tag>::value, "Lpuart2DoesNotSupport"},
        UartCapCase{!ohal::uart::capabilities::supports_synchronous_mode<Lpuart3Tag>::value, "Lpuart3DoesNotSupport"}),
    [](const ::testing::TestParamInfo<UartCapCase>& info) { return info.param.name; });

// Smartcard mode — full tier only (USART1/2); absent on basic and low-power.
INSTANTIATE_TEST_SUITE_P(
    SmartcardMode, Stm32u083RctUartCapabilityTest,
    ::testing::Values(
        UartCapCase{ohal::uart::capabilities::supports_smartcard_mode<Usart1Tag>::value,   "Usart1Supports"},
        UartCapCase{ohal::uart::capabilities::supports_smartcard_mode<Usart2Tag>::value,   "Usart2Supports"},
        UartCapCase{!ohal::uart::capabilities::supports_smartcard_mode<Usart3Tag>::value,  "Usart3DoesNotSupport"},
        UartCapCase{!ohal::uart::capabilities::supports_smartcard_mode<Usart4Tag>::value,  "Usart4DoesNotSupport"},
        UartCapCase{!ohal::uart::capabilities::supports_smartcard_mode<Lpuart1Tag>::value, "Lpuart1DoesNotSupport"},
        UartCapCase{!ohal::uart::capabilities::supports_smartcard_mode<Lpuart2Tag>::value, "Lpuart2DoesNotSupport"},
        UartCapCase{!ohal::uart::capabilities::supports_smartcard_mode<Lpuart3Tag>::value, "Lpuart3DoesNotSupport"}),
    [](const ::testing::TestParamInfo<UartCapCase>& info) { return info.param.name; });

// Single-wire half-duplex — supported by all tiers.
INSTANTIATE_TEST_SUITE_P(
    SingleWireHalfDuplex, Stm32u083RctUartCapabilityTest,
    ::testing::Values(
        UartCapCase{ohal::uart::capabilities::supports_single_wire_half_duplex<Usart1Tag>::value,  "Usart1Supports"},
        UartCapCase{ohal::uart::capabilities::supports_single_wire_half_duplex<Usart2Tag>::value,  "Usart2Supports"},
        UartCapCase{ohal::uart::capabilities::supports_single_wire_half_duplex<Usart3Tag>::value,  "Usart3Supports"},
        UartCapCase{ohal::uart::capabilities::supports_single_wire_half_duplex<Usart4Tag>::value,  "Usart4Supports"},
        UartCapCase{ohal::uart::capabilities::supports_single_wire_half_duplex<Lpuart1Tag>::value, "Lpuart1Supports"},
        UartCapCase{ohal::uart::capabilities::supports_single_wire_half_duplex<Lpuart2Tag>::value, "Lpuart2Supports"},
        UartCapCase{ohal::uart::capabilities::supports_single_wire_half_duplex<Lpuart3Tag>::value, "Lpuart3Supports"}),
    [](const ::testing::TestParamInfo<UartCapCase>& info) { return info.param.name; });

// IrDA SIR ENDEC — full tier only (USART1/2); absent on basic and low-power.
INSTANTIATE_TEST_SUITE_P(
    IrdaMode, Stm32u083RctUartCapabilityTest,
    ::testing::Values(
        UartCapCase{ohal::uart::capabilities::supports_irda_mode<Usart1Tag>::value,   "Usart1Supports"},
        UartCapCase{ohal::uart::capabilities::supports_irda_mode<Usart2Tag>::value,   "Usart2Supports"},
        UartCapCase{!ohal::uart::capabilities::supports_irda_mode<Usart3Tag>::value,  "Usart3DoesNotSupport"},
        UartCapCase{!ohal::uart::capabilities::supports_irda_mode<Usart4Tag>::value,  "Usart4DoesNotSupport"},
        UartCapCase{!ohal::uart::capabilities::supports_irda_mode<Lpuart1Tag>::value, "Lpuart1DoesNotSupport"},
        UartCapCase{!ohal::uart::capabilities::supports_irda_mode<Lpuart2Tag>::value, "Lpuart2DoesNotSupport"},
        UartCapCase{!ohal::uart::capabilities::supports_irda_mode<Lpuart3Tag>::value, "Lpuart3DoesNotSupport"}),
    [](const ::testing::TestParamInfo<UartCapCase>& info) { return info.param.name; });

// LIN mode — full tier only (USART1/2); absent on basic and low-power.
INSTANTIATE_TEST_SUITE_P(
    LinMode, Stm32u083RctUartCapabilityTest,
    ::testing::Values(
        UartCapCase{ohal::uart::capabilities::supports_lin_mode<Usart1Tag>::value,   "Usart1Supports"},
        UartCapCase{ohal::uart::capabilities::supports_lin_mode<Usart2Tag>::value,   "Usart2Supports"},
        UartCapCase{!ohal::uart::capabilities::supports_lin_mode<Usart3Tag>::value,  "Usart3DoesNotSupport"},
        UartCapCase{!ohal::uart::capabilities::supports_lin_mode<Usart4Tag>::value,  "Usart4DoesNotSupport"},
        UartCapCase{!ohal::uart::capabilities::supports_lin_mode<Lpuart1Tag>::value, "Lpuart1DoesNotSupport"},
        UartCapCase{!ohal::uart::capabilities::supports_lin_mode<Lpuart2Tag>::value, "Lpuart2DoesNotSupport"},
        UartCapCase{!ohal::uart::capabilities::supports_lin_mode<Lpuart3Tag>::value, "Lpuart3DoesNotSupport"}),
    [](const ::testing::TestParamInfo<UartCapCase>& info) { return info.param.name; });

// Receiver timeout interrupt — full tier only (USART1/2); absent on basic and low-power.
INSTANTIATE_TEST_SUITE_P(
    ReceiverTimeout, Stm32u083RctUartCapabilityTest,
    ::testing::Values(
        UartCapCase{ohal::uart::capabilities::supports_receiver_timeout<Usart1Tag>::value,   "Usart1Supports"},
        UartCapCase{ohal::uart::capabilities::supports_receiver_timeout<Usart2Tag>::value,   "Usart2Supports"},
        UartCapCase{!ohal::uart::capabilities::supports_receiver_timeout<Usart3Tag>::value,  "Usart3DoesNotSupport"},
        UartCapCase{!ohal::uart::capabilities::supports_receiver_timeout<Usart4Tag>::value,  "Usart4DoesNotSupport"},
        UartCapCase{!ohal::uart::capabilities::supports_receiver_timeout<Lpuart1Tag>::value, "Lpuart1DoesNotSupport"},
        UartCapCase{!ohal::uart::capabilities::supports_receiver_timeout<Lpuart2Tag>::value, "Lpuart2DoesNotSupport"},
        UartCapCase{!ohal::uart::capabilities::supports_receiver_timeout<Lpuart3Tag>::value, "Lpuart3DoesNotSupport"}),
    [](const ::testing::TestParamInfo<UartCapCase>& info) { return info.param.name; });

// Modbus communication — full tier only (USART1/2); absent on basic and low-power.
INSTANTIATE_TEST_SUITE_P(
    ModbusMode, Stm32u083RctUartCapabilityTest,
    ::testing::Values(
        UartCapCase{ohal::uart::capabilities::supports_modbus_mode<Usart1Tag>::value,   "Usart1Supports"},
        UartCapCase{ohal::uart::capabilities::supports_modbus_mode<Usart2Tag>::value,   "Usart2Supports"},
        UartCapCase{!ohal::uart::capabilities::supports_modbus_mode<Usart3Tag>::value,  "Usart3DoesNotSupport"},
        UartCapCase{!ohal::uart::capabilities::supports_modbus_mode<Usart4Tag>::value,  "Usart4DoesNotSupport"},
        UartCapCase{!ohal::uart::capabilities::supports_modbus_mode<Lpuart1Tag>::value, "Lpuart1DoesNotSupport"},
        UartCapCase{!ohal::uart::capabilities::supports_modbus_mode<Lpuart2Tag>::value, "Lpuart2DoesNotSupport"},
        UartCapCase{!ohal::uart::capabilities::supports_modbus_mode<Lpuart3Tag>::value, "Lpuart3DoesNotSupport"}),
    [](const ::testing::TestParamInfo<UartCapCase>& info) { return info.param.name; });

// Auto baud rate detection — full tier only (USART1/2); absent on basic and low-power.
INSTANTIATE_TEST_SUITE_P(
    AutoBaudRateDetection, Stm32u083RctUartCapabilityTest,
    ::testing::Values(
        UartCapCase{ohal::uart::capabilities::supports_auto_baud_rate_detection<Usart1Tag>::value,   "Usart1Supports"},
        UartCapCase{ohal::uart::capabilities::supports_auto_baud_rate_detection<Usart2Tag>::value,   "Usart2Supports"},
        UartCapCase{!ohal::uart::capabilities::supports_auto_baud_rate_detection<Usart3Tag>::value,  "Usart3DoesNotSupport"},
        UartCapCase{!ohal::uart::capabilities::supports_auto_baud_rate_detection<Usart4Tag>::value,  "Usart4DoesNotSupport"},
        UartCapCase{!ohal::uart::capabilities::supports_auto_baud_rate_detection<Lpuart1Tag>::value, "Lpuart1DoesNotSupport"},
        UartCapCase{!ohal::uart::capabilities::supports_auto_baud_rate_detection<Lpuart2Tag>::value, "Lpuart2DoesNotSupport"},
        UartCapCase{!ohal::uart::capabilities::supports_auto_baud_rate_detection<Lpuart3Tag>::value, "Lpuart3DoesNotSupport"}),
    [](const ::testing::TestParamInfo<UartCapCase>& info) { return info.param.name; });

// Driver enable — supported by all tiers.
INSTANTIATE_TEST_SUITE_P(
    DriverEnable, Stm32u083RctUartCapabilityTest,
    ::testing::Values(
        UartCapCase{ohal::uart::capabilities::supports_driver_enable<Usart1Tag>::value,  "Usart1Supports"},
        UartCapCase{ohal::uart::capabilities::supports_driver_enable<Usart2Tag>::value,  "Usart2Supports"},
        UartCapCase{ohal::uart::capabilities::supports_driver_enable<Usart3Tag>::value,  "Usart3Supports"},
        UartCapCase{ohal::uart::capabilities::supports_driver_enable<Usart4Tag>::value,  "Usart4Supports"},
        UartCapCase{ohal::uart::capabilities::supports_driver_enable<Lpuart1Tag>::value, "Lpuart1Supports"},
        UartCapCase{ohal::uart::capabilities::supports_driver_enable<Lpuart2Tag>::value, "Lpuart2Supports"},
        UartCapCase{ohal::uart::capabilities::supports_driver_enable<Lpuart3Tag>::value, "Lpuart3Supports"}),
    [](const ::testing::TestParamInfo<UartCapCase>& info) { return info.param.name; });

// Data length 7/8/9 bits — supported by all tiers.
INSTANTIATE_TEST_SUITE_P(
    DataLength789Bits, Stm32u083RctUartCapabilityTest,
    ::testing::Values(
        UartCapCase{ohal::uart::capabilities::supports_data_length_7_8_9_bits<Usart1Tag>::value,  "Usart1Supports"},
        UartCapCase{ohal::uart::capabilities::supports_data_length_7_8_9_bits<Usart2Tag>::value,  "Usart2Supports"},
        UartCapCase{ohal::uart::capabilities::supports_data_length_7_8_9_bits<Usart3Tag>::value,  "Usart3Supports"},
        UartCapCase{ohal::uart::capabilities::supports_data_length_7_8_9_bits<Usart4Tag>::value,  "Usart4Supports"},
        UartCapCase{ohal::uart::capabilities::supports_data_length_7_8_9_bits<Lpuart1Tag>::value, "Lpuart1Supports"},
        UartCapCase{ohal::uart::capabilities::supports_data_length_7_8_9_bits<Lpuart2Tag>::value, "Lpuart2Supports"},
        UartCapCase{ohal::uart::capabilities::supports_data_length_7_8_9_bits<Lpuart3Tag>::value, "Lpuart3Supports"}),
    [](const ::testing::TestParamInfo<UartCapCase>& info) { return info.param.name; });

// Tx/Rx FIFO — full and low-power tiers; absent on basic (USART3/4).
INSTANTIATE_TEST_SUITE_P(
    Fifo, Stm32u083RctUartCapabilityTest,
    ::testing::Values(
        UartCapCase{ohal::uart::capabilities::supports_fifo<Usart1Tag>::value,   "Usart1Supports"},
        UartCapCase{ohal::uart::capabilities::supports_fifo<Usart2Tag>::value,   "Usart2Supports"},
        UartCapCase{!ohal::uart::capabilities::supports_fifo<Usart3Tag>::value,  "Usart3DoesNotSupport"},
        UartCapCase{!ohal::uart::capabilities::supports_fifo<Usart4Tag>::value,  "Usart4DoesNotSupport"},
        UartCapCase{ohal::uart::capabilities::supports_fifo<Lpuart1Tag>::value,  "Lpuart1Supports"},
        UartCapCase{ohal::uart::capabilities::supports_fifo<Lpuart2Tag>::value,  "Lpuart2Supports"},
        UartCapCase{ohal::uart::capabilities::supports_fifo<Lpuart3Tag>::value,  "Lpuart3Supports"}),
    [](const ::testing::TestParamInfo<UartCapCase>& info) { return info.param.name; });

// Prescaler — full and low-power tiers; absent on basic (USART3/4).
INSTANTIATE_TEST_SUITE_P(
    Prescaler, Stm32u083RctUartCapabilityTest,
    ::testing::Values(
        UartCapCase{ohal::uart::capabilities::supports_prescaler<Usart1Tag>::value,   "Usart1Supports"},
        UartCapCase{ohal::uart::capabilities::supports_prescaler<Usart2Tag>::value,   "Usart2Supports"},
        UartCapCase{!ohal::uart::capabilities::supports_prescaler<Usart3Tag>::value,  "Usart3DoesNotSupport"},
        UartCapCase{!ohal::uart::capabilities::supports_prescaler<Usart4Tag>::value,  "Usart4DoesNotSupport"},
        UartCapCase{ohal::uart::capabilities::supports_prescaler<Lpuart1Tag>::value,  "Lpuart1Supports"},
        UartCapCase{ohal::uart::capabilities::supports_prescaler<Lpuart2Tag>::value,  "Lpuart2Supports"},
        UartCapCase{ohal::uart::capabilities::supports_prescaler<Lpuart3Tag>::value,  "Lpuart3Supports"}),
    [](const ::testing::TestParamInfo<UartCapCase>& info) { return info.param.name; });

// Wake-up from low-power mode — full and low-power tiers; absent on basic (USART3/4).
INSTANTIATE_TEST_SUITE_P(
    WakeupFromStop, Stm32u083RctUartCapabilityTest,
    ::testing::Values(
        UartCapCase{ohal::uart::capabilities::supports_wakeup_from_stop_0_1<Usart1Tag>::value,   "Usart1Supports"},
        UartCapCase{ohal::uart::capabilities::supports_wakeup_from_stop_0_1<Usart2Tag>::value,   "Usart2Supports"},
        UartCapCase{!ohal::uart::capabilities::supports_wakeup_from_stop_0_1<Usart3Tag>::value,  "Usart3DoesNotSupport"},
        UartCapCase{!ohal::uart::capabilities::supports_wakeup_from_stop_0_1<Usart4Tag>::value,  "Usart4DoesNotSupport"},
        UartCapCase{ohal::uart::capabilities::supports_wakeup_from_stop_0_1<Lpuart1Tag>::value,  "Lpuart1Supports"},
        UartCapCase{ohal::uart::capabilities::supports_wakeup_from_stop_0_1<Lpuart2Tag>::value,  "Lpuart2Supports"},
        UartCapCase{ohal::uart::capabilities::supports_wakeup_from_stop_0_1<Lpuart3Tag>::value,  "Lpuart3Supports"}),
    [](const ::testing::TestParamInfo<UartCapCase>& info) { return info.param.name; });

// Dual clock domain / wake-up — full and low-power tiers; absent on basic (USART3/4).
INSTANTIATE_TEST_SUITE_P(
    DualClockDomain, Stm32u083RctUartCapabilityTest,
    ::testing::Values(
        UartCapCase{ohal::uart::capabilities::supports_dual_clock_domain<Usart1Tag>::value,   "Usart1Supports"},
        UartCapCase{ohal::uart::capabilities::supports_dual_clock_domain<Usart2Tag>::value,   "Usart2Supports"},
        UartCapCase{!ohal::uart::capabilities::supports_dual_clock_domain<Usart3Tag>::value,  "Usart3DoesNotSupport"},
        UartCapCase{!ohal::uart::capabilities::supports_dual_clock_domain<Usart4Tag>::value,  "Usart4DoesNotSupport"},
        UartCapCase{ohal::uart::capabilities::supports_dual_clock_domain<Lpuart1Tag>::value,  "Lpuart1Supports"},
        UartCapCase{ohal::uart::capabilities::supports_dual_clock_domain<Lpuart2Tag>::value,  "Lpuart2Supports"},
        UartCapCase{ohal::uart::capabilities::supports_dual_clock_domain<Lpuart3Tag>::value,  "Lpuart3Supports"}),
    [](const ::testing::TestParamInfo<UartCapCase>& info) { return info.param.name; });

// clang-format on

// ─── FIFO size tests ──────────────────────────────────────────────────────────

struct UartFifoSizeCase {
  uint8_t actual_size;
  uint8_t expected_size;
  const char* name;
};

class Stm32u083RctUartFifoSizeTest : public ::testing::TestWithParam<UartFifoSizeCase> {};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    FifoSizes, Stm32u083RctUartFifoSizeTest,
    ::testing::Values(
        UartFifoSizeCase{ohal::uart::capabilities::tx_rx_fifo_size_bytes<Usart1Tag>::value,  kUsartFifoSizeBytes, "Usart1"},
        UartFifoSizeCase{ohal::uart::capabilities::tx_rx_fifo_size_bytes<Usart2Tag>::value,  kUsartFifoSizeBytes, "Usart2"},
        UartFifoSizeCase{ohal::uart::capabilities::tx_rx_fifo_size_bytes<Lpuart1Tag>::value, kUsartFifoSizeBytes, "Lpuart1"},
        UartFifoSizeCase{ohal::uart::capabilities::tx_rx_fifo_size_bytes<Lpuart2Tag>::value, kUsartFifoSizeBytes, "Lpuart2"},
        UartFifoSizeCase{ohal::uart::capabilities::tx_rx_fifo_size_bytes<Lpuart3Tag>::value, kUsartFifoSizeBytes, "Lpuart3"}),
    [](const ::testing::TestParamInfo<UartFifoSizeCase>& info) {
      return info.param.name;
    });
// clang-format on

TEST_P(Stm32u083RctUartFifoSizeTest, FifoSizeMatchesSpec) {
  EXPECT_EQ(GetParam().actual_size, GetParam().expected_size);
}

// ─── GPIO and timer tests ─────────────────────────────────────────────────────
//
// These tests guard the exact GPIO pins used by the test_project firmware
// (main.cpp on the NUCLEO-U083RC board) to catch regressions as the firmware
// is progressively refactored to use ohal directly.
//
// Pins in scope:
//   PA5  — LED (Output/PushPull/VeryHigh speed/NoPull, Toggle)
//   PC13 — User button (EXTI input)
//   PA2  — COM1/USART2 TX (GPIO_AF7_USART2)
//   PA3  — COM1/USART2 RX (GPIO_AF7_USART2)
//   PB8  — I2C1 SCL (GPIO_AF4_I2C1)
//   PB9  — I2C1 SDA (GPIO_AF4_I2C1)

TEST(Stm32u083RctGpioCoverageTest, TestProjectLedPinOutputCapabilityMatchesExpected) {
  EXPECT_TRUE((ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 5>::value));
}

struct GpioPinCapCase {
  bool value;
  const char* name;
};

class Stm32u083RctTestProjectGpioCapabilityTest : public ::testing::TestWithParam<GpioPinCapCase> {
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    TestProjectPinCapabilities, Stm32u083RctTestProjectGpioCapabilityTest,
    ::testing::Values(
        // LED pin PA5: output speed (used by set_speed(Speed::VeryHigh))
        GpioPinCapCase{ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortA, 5>::value,
                       "PA5_LedPin_SupportsOutputSpeed"},
        // LED pin PA5: pull resistor (used by set_pull(Pull::None))
        GpioPinCapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 5>::value,
                       "PA5_LedPin_SupportsPull"},
        // User button PC13: confirm it is a bonded GPIO pin on this package
        GpioPinCapCase{ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortC, 13>::value,
                       "PC13_ButtonPin_IsBonded"},
        // COM1/USART2 TX: PA2 must support alternate function (GPIO_AF7_USART2)
        GpioPinCapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortA, 2>::value,
                       "PA2_Usart2TxPin_SupportsAlternateFunction"},
        // COM1/USART2 RX: PA3 must support alternate function (GPIO_AF7_USART2)
        GpioPinCapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortA, 3>::value,
                       "PA3_Usart2RxPin_SupportsAlternateFunction"},
        // I2C1 SCL: PB8 must support alternate function (GPIO_AF4_I2C1)
        GpioPinCapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortB, 8>::value,
                       "PB8_I2c1SclPin_SupportsAlternateFunction"},
        // I2C1 SDA: PB9 must support alternate function (GPIO_AF4_I2C1)
        GpioPinCapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortB, 9>::value,
                       "PB9_I2c1SdaPin_SupportsAlternateFunction"}),
    [](const ::testing::TestParamInfo<GpioPinCapCase>& info) { return info.param.name; });
// clang-format on

TEST_P(Stm32u083RctTestProjectGpioCapabilityTest, CapabilityMatchesExpected) {
  EXPECT_TRUE(GetParam().value);
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
