#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_UART_CAPABILITIES_BASE_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_UART_CAPABILITIES_BASE_HPP

// Shared UART capability trait specialisations for the six USART/LPUART instances
// present on ALL STM32U0 sub-families:
//   - USART1, USART2  — full feature set  (RM0503 Table 200, column 1)
//   - USART3, USART4  — basic feature set (RM0503 Table 200, column 2)
//   - LPUART1, LPUART2 — low-power feature set (RM0503 Table 200, column 3)
//
// LPUART3 (low-power, STM32U073/STM32U083 only) is NOT included here.
// Including code must add LPUART3 specialisations separately, or rely on
// stm32u083/uart_capabilities.hpp which does so.

#include "ohal/platforms/stm32u0/models/stm32u083/uart.hpp"

namespace ohal::uart::capabilities
{

template <>
struct supports_hardware_flow_control<ohal::platforms::stm32u0::stm32u083::Usart1Tag>
    : std::true_type
{
};
template <>
struct supports_hardware_flow_control<ohal::platforms::stm32u0::stm32u083::Usart2Tag>
    : std::true_type
{
};
template <>
struct supports_hardware_flow_control<ohal::platforms::stm32u0::stm32u083::Usart3Tag>
    : std::true_type
{
};
template <>
struct supports_hardware_flow_control<ohal::platforms::stm32u0::stm32u083::Usart4Tag>
    : std::true_type
{
};
template <>
struct supports_hardware_flow_control<ohal::platforms::stm32u0::stm32u083::Lpuart1Tag>
    : std::true_type
{
};
template <>
struct supports_hardware_flow_control<ohal::platforms::stm32u0::stm32u083::Lpuart2Tag>
    : std::true_type
{
};

template <>
struct supports_dma<ohal::platforms::stm32u0::stm32u083::Usart1Tag> : std::true_type
{
};
template <>
struct supports_dma<ohal::platforms::stm32u0::stm32u083::Usart2Tag> : std::true_type
{
};
template <>
struct supports_dma<ohal::platforms::stm32u0::stm32u083::Usart3Tag> : std::true_type
{
};
template <>
struct supports_dma<ohal::platforms::stm32u0::stm32u083::Usart4Tag> : std::true_type
{
};
template <>
struct supports_dma<ohal::platforms::stm32u0::stm32u083::Lpuart1Tag> : std::true_type
{
};
template <>
struct supports_dma<ohal::platforms::stm32u0::stm32u083::Lpuart2Tag> : std::true_type
{
};

template <>
struct supports_multiprocessor_communication<ohal::platforms::stm32u0::stm32u083::Usart1Tag>
    : std::true_type
{
};
template <>
struct supports_multiprocessor_communication<ohal::platforms::stm32u0::stm32u083::Usart2Tag>
    : std::true_type
{
};
template <>
struct supports_multiprocessor_communication<ohal::platforms::stm32u0::stm32u083::Usart3Tag>
    : std::true_type
{
};
template <>
struct supports_multiprocessor_communication<ohal::platforms::stm32u0::stm32u083::Usart4Tag>
    : std::true_type
{
};
template <>
struct supports_multiprocessor_communication<ohal::platforms::stm32u0::stm32u083::Lpuart1Tag>
    : std::true_type
{
};
template <>
struct supports_multiprocessor_communication<ohal::platforms::stm32u0::stm32u083::Lpuart2Tag>
    : std::true_type
{
};

template <>
struct supports_synchronous_mode<ohal::platforms::stm32u0::stm32u083::Usart1Tag> : std::true_type
{
};
template <>
struct supports_synchronous_mode<ohal::platforms::stm32u0::stm32u083::Usart2Tag> : std::true_type
{
};
template <>
struct supports_synchronous_mode<ohal::platforms::stm32u0::stm32u083::Usart3Tag> : std::true_type
{
};
template <>
struct supports_synchronous_mode<ohal::platforms::stm32u0::stm32u083::Usart4Tag> : std::true_type
{
};

template <>
struct supports_smartcard_mode<ohal::platforms::stm32u0::stm32u083::Usart1Tag> : std::true_type
{
};
template <>
struct supports_smartcard_mode<ohal::platforms::stm32u0::stm32u083::Usart2Tag> : std::true_type
{
};

template <>
struct supports_single_wire_half_duplex<ohal::platforms::stm32u0::stm32u083::Usart1Tag>
    : std::true_type
{
};
template <>
struct supports_single_wire_half_duplex<ohal::platforms::stm32u0::stm32u083::Usart2Tag>
    : std::true_type
{
};
template <>
struct supports_single_wire_half_duplex<ohal::platforms::stm32u0::stm32u083::Usart3Tag>
    : std::true_type
{
};
template <>
struct supports_single_wire_half_duplex<ohal::platforms::stm32u0::stm32u083::Usart4Tag>
    : std::true_type
{
};
template <>
struct supports_single_wire_half_duplex<ohal::platforms::stm32u0::stm32u083::Lpuart1Tag>
    : std::true_type
{
};
template <>
struct supports_single_wire_half_duplex<ohal::platforms::stm32u0::stm32u083::Lpuart2Tag>
    : std::true_type
{
};

template <>
struct supports_irda_mode<ohal::platforms::stm32u0::stm32u083::Usart1Tag> : std::true_type
{
};
template <>
struct supports_irda_mode<ohal::platforms::stm32u0::stm32u083::Usart2Tag> : std::true_type
{
};

template <>
struct supports_lin_mode<ohal::platforms::stm32u0::stm32u083::Usart1Tag> : std::true_type
{
};
template <>
struct supports_lin_mode<ohal::platforms::stm32u0::stm32u083::Usart2Tag> : std::true_type
{
};

template <>
struct supports_receiver_timeout<ohal::platforms::stm32u0::stm32u083::Usart1Tag> : std::true_type
{
};
template <>
struct supports_receiver_timeout<ohal::platforms::stm32u0::stm32u083::Usart2Tag> : std::true_type
{
};

template <>
struct supports_modbus_mode<ohal::platforms::stm32u0::stm32u083::Usart1Tag> : std::true_type
{
};
template <>
struct supports_modbus_mode<ohal::platforms::stm32u0::stm32u083::Usart2Tag> : std::true_type
{
};

template <>
struct supports_auto_baud_rate_detection<ohal::platforms::stm32u0::stm32u083::Usart1Tag>
    : std::true_type
{
};
template <>
struct supports_auto_baud_rate_detection<ohal::platforms::stm32u0::stm32u083::Usart2Tag>
    : std::true_type
{
};

template <>
struct supports_driver_enable<ohal::platforms::stm32u0::stm32u083::Usart1Tag> : std::true_type
{
};
template <>
struct supports_driver_enable<ohal::platforms::stm32u0::stm32u083::Usart2Tag> : std::true_type
{
};
template <>
struct supports_driver_enable<ohal::platforms::stm32u0::stm32u083::Usart3Tag> : std::true_type
{
};
template <>
struct supports_driver_enable<ohal::platforms::stm32u0::stm32u083::Usart4Tag> : std::true_type
{
};
template <>
struct supports_driver_enable<ohal::platforms::stm32u0::stm32u083::Lpuart1Tag> : std::true_type
{
};
template <>
struct supports_driver_enable<ohal::platforms::stm32u0::stm32u083::Lpuart2Tag> : std::true_type
{
};

template <>
struct supports_data_length_7_8_9_bits<ohal::platforms::stm32u0::stm32u083::Usart1Tag>
    : std::true_type
{
};
template <>
struct supports_data_length_7_8_9_bits<ohal::platforms::stm32u0::stm32u083::Usart2Tag>
    : std::true_type
{
};
template <>
struct supports_data_length_7_8_9_bits<ohal::platforms::stm32u0::stm32u083::Usart3Tag>
    : std::true_type
{
};
template <>
struct supports_data_length_7_8_9_bits<ohal::platforms::stm32u0::stm32u083::Usart4Tag>
    : std::true_type
{
};
template <>
struct supports_data_length_7_8_9_bits<ohal::platforms::stm32u0::stm32u083::Lpuart1Tag>
    : std::true_type
{
};
template <>
struct supports_data_length_7_8_9_bits<ohal::platforms::stm32u0::stm32u083::Lpuart2Tag>
    : std::true_type
{
};

template <>
struct supports_fifo<ohal::platforms::stm32u0::stm32u083::Usart1Tag> : std::true_type
{
};
template <>
struct supports_fifo<ohal::platforms::stm32u0::stm32u083::Usart2Tag> : std::true_type
{
};
template <>
struct supports_fifo<ohal::platforms::stm32u0::stm32u083::Lpuart1Tag> : std::true_type
{
};
template <>
struct supports_fifo<ohal::platforms::stm32u0::stm32u083::Lpuart2Tag> : std::true_type
{
};

template <>
struct tx_rx_fifo_size_bytes<ohal::platforms::stm32u0::stm32u083::Usart1Tag>
    : std::integral_constant<uint8_t, ohal::platforms::stm32u0::stm32u083::kUsartFifoSizeBytes>
{
};
template <>
struct tx_rx_fifo_size_bytes<ohal::platforms::stm32u0::stm32u083::Usart2Tag>
    : std::integral_constant<uint8_t, ohal::platforms::stm32u0::stm32u083::kUsartFifoSizeBytes>
{
};
template <>
struct tx_rx_fifo_size_bytes<ohal::platforms::stm32u0::stm32u083::Lpuart1Tag>
    : std::integral_constant<uint8_t, ohal::platforms::stm32u0::stm32u083::kUsartFifoSizeBytes>
{
};
template <>
struct tx_rx_fifo_size_bytes<ohal::platforms::stm32u0::stm32u083::Lpuart2Tag>
    : std::integral_constant<uint8_t, ohal::platforms::stm32u0::stm32u083::kUsartFifoSizeBytes>
{
};

template <>
struct supports_prescaler<ohal::platforms::stm32u0::stm32u083::Usart1Tag> : std::true_type
{
};
template <>
struct supports_prescaler<ohal::platforms::stm32u0::stm32u083::Usart2Tag> : std::true_type
{
};
template <>
struct supports_prescaler<ohal::platforms::stm32u0::stm32u083::Lpuart1Tag> : std::true_type
{
};
template <>
struct supports_prescaler<ohal::platforms::stm32u0::stm32u083::Lpuart2Tag> : std::true_type
{
};

template <>
struct supports_wakeup_from_stop_0_1<ohal::platforms::stm32u0::stm32u083::Usart1Tag>
    : std::true_type
{
};
template <>
struct supports_wakeup_from_stop_0_1<ohal::platforms::stm32u0::stm32u083::Usart2Tag>
    : std::true_type
{
};
template <>
struct supports_wakeup_from_stop_0_1<ohal::platforms::stm32u0::stm32u083::Lpuart1Tag>
    : std::true_type
{
};
template <>
struct supports_wakeup_from_stop_0_1<ohal::platforms::stm32u0::stm32u083::Lpuart2Tag>
    : std::true_type
{
};

template <>
struct supports_dual_clock_domain<ohal::platforms::stm32u0::stm32u083::Usart1Tag> : std::true_type
{
};
template <>
struct supports_dual_clock_domain<ohal::platforms::stm32u0::stm32u083::Usart2Tag> : std::true_type
{
};
template <>
struct supports_dual_clock_domain<ohal::platforms::stm32u0::stm32u083::Lpuart1Tag> : std::true_type
{
};
template <>
struct supports_dual_clock_domain<ohal::platforms::stm32u0::stm32u083::Lpuart2Tag> : std::true_type
{
};

} // namespace ohal::uart::capabilities

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_UART_CAPABILITIES_BASE_HPP
