#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U031_UART_CAPABILITIES_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U031_UART_CAPABILITIES_HPP

// UART capability trait specialisations for all STM32U031 USART/LPUART instances.
//
// All STM32U031 package variants expose the same six serial peripherals:
//   - USART1, USART2  — full feature set  (RM0503 Table 200, column 1)
//   - USART3, USART4  — basic feature set (RM0503 Table 200, column 2)
//   - LPUART1, LPUART2 — low-power feature set (RM0503 Table 200, column 3)
//
// LPUART3 is not present on STM32U031. For STM32U083 packages (which also
// include LPUART3), include
// ohal/platforms/stm32u0/models/stm32u083/uart_capabilities.hpp instead.
//
// The specialisations for the six common peripherals live in
// uart_capabilities_base.hpp, which is shared with the STM32U083 layer to
// ensure a single definition of each explicit specialisation.

#include "ohal/platforms/stm32u0/models/stm32u083/uart_capabilities_base.hpp"

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U031_UART_CAPABILITIES_HPP
