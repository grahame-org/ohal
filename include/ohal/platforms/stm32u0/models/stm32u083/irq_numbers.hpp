#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_IRQ_NUMBERS_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_IRQ_NUMBERS_HPP

#include <cstdint>

namespace ohal::platforms::stm32u0::stm32u083 {

enum class IrqNumber : uint8_t {
  WwdgIwdg = 0U,
  PvdPvm = 1U,
  RtcTamp = 2U,
  Flash = 3U,
  RccCrs = 4U,
  Exti0_1 = 5U,
  Exti2_3 = 6U,
  Exti4_15 = 7U,
  Usb = 8U,
  Dma1Channel1 = 9U,
  Dma1Channel2_3 = 10U,
  Dma1Channel4_5_6_7DmamuxDma2Channel1_2_3_4_5 = 11U,
  AdcComp = 12U,
  Tim1BrkUpTrgCom = 13U,
  Tim1Cc = 14U,
  Tim2 = 15U,
  Tim3 = 16U,
  Tim6DacLptim1 = 17U,
  Tim7Lptim2 = 18U,
  Tim15Lptim3 = 19U,
  Tim16 = 20U,
  Tsc = 21U,
  Lcd = 22U,
  I2c1 = 23U,
  I2c2I2c3I2c4 = 24U,
  Spi1 = 25U,
  Spi2Spi3 = 26U,
  Usart1 = 27U,
  Usart2Lpuart2 = 28U,
  Usart3Lpuart1 = 29U,
  Usart4Lpuart3 = 30U,
  AesRng = 31U,
};

} // namespace ohal::platforms::stm32u0::stm32u083

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_IRQ_NUMBERS_HPP
