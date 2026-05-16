#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_UART_CAPABILITIES_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_UART_CAPABILITIES_HPP

// UART capability trait specialisations for all STM32U083 USART/LPUART instances.
//
// All STM32U083 package variants expose the same seven serial peripherals:
//   - USART1, USART2  — full feature set  (RM0503 Table 200, column 1)
//   - USART3, USART4  — basic feature set (RM0503 Table 200, column 2)
//   - LPUART1, LPUART2, LPUART3 — low-power feature set (RM0503 Table 200, column 3)
//
// For STM32U031 packages (which lack LPUART3), include
// ohal/platforms/stm32u0/models/stm32u031/uart_capabilities.hpp instead.

// Signal that LPUART3 is available in this build (STM32U083).  Defined here so
// that the macro is set before uart.hpp (via uart_capabilities_base.hpp) is
// first included, regardless of the order in which callers include the
// per-package uart.hpp and uart_capabilities.hpp wrappers.
#ifndef OHAL_STM32U0_ENABLE_LPUART3
#define OHAL_STM32U0_ENABLE_LPUART3
#endif

// Common specialisations for USART1–4 and LPUART1–2.
#include "ohal/platforms/stm32u0/models/stm32u083/uart_capabilities_base.hpp"

namespace ohal::uart::capabilities
{

// LPUART3 additional specialisations (STM32U083 only).

template <>
struct supports_hardware_flow_control<ohal::platforms::stm32u0::stm32u083::Lpuart3Tag>
    : std::true_type
{
};

template <>
struct supports_dma<ohal::platforms::stm32u0::stm32u083::Lpuart3Tag> : std::true_type
{
};

template <>
struct supports_multiprocessor_communication<ohal::platforms::stm32u0::stm32u083::Lpuart3Tag>
    : std::true_type
{
};

template <>
struct supports_single_wire_half_duplex<ohal::platforms::stm32u0::stm32u083::Lpuart3Tag>
    : std::true_type
{
};

template <>
struct supports_driver_enable<ohal::platforms::stm32u0::stm32u083::Lpuart3Tag> : std::true_type
{
};

template <>
struct supports_data_length_7_8_9_bits<ohal::platforms::stm32u0::stm32u083::Lpuart3Tag>
    : std::true_type
{
};

template <>
struct supports_fifo<ohal::platforms::stm32u0::stm32u083::Lpuart3Tag> : std::true_type
{
};

template <>
struct tx_rx_fifo_size_bytes<ohal::platforms::stm32u0::stm32u083::Lpuart3Tag>
    : std::integral_constant<uint8_t, ohal::platforms::stm32u0::stm32u083::kUsartFifoSizeBytes>
{
};

template <>
struct supports_prescaler<ohal::platforms::stm32u0::stm32u083::Lpuart3Tag> : std::true_type
{
};

template <>
struct supports_wakeup_from_stop_0_1<ohal::platforms::stm32u0::stm32u083::Lpuart3Tag>
    : std::true_type
{
};

template <>
struct supports_dual_clock_domain<ohal::platforms::stm32u0::stm32u083::Lpuart3Tag> : std::true_type
{
};

} // namespace ohal::uart::capabilities

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_UART_CAPABILITIES_HPP
