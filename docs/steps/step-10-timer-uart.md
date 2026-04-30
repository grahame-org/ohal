# Step 10 – Additional Peripherals (Timer, UART)

**Goal:** Replicate the pattern from Steps 7–8 for Timer and UART peripherals on STM32U083.

## Inputs Required for STM32U083 Timer (TIM2)

- MCU family: STM32U0
- MCU model: STM32U083
- TIM2 base address: `0x40000000`
- Relevant registers and offsets (from RM0503):
  - `CR1`   `0x00` — control register 1
  - `CR2`   `0x04` — control register 2
  - `DIER`  `0x0C` — DMA/interrupt enable register
  - `SR`    `0x10` — status register
  - `EGR`   `0x14` — event generation register (WO)
  - `CCMR1` `0x18` — capture/compare mode register 1
  - `CCMR2` `0x1C` — capture/compare mode register 2
  - `CCER`  `0x20` — capture/compare enable register
  - `CNT`   `0x24` — counter register
  - `PSC`   `0x28` — prescaler register
  - `ARR`   `0x2C` — auto-reload register
  - `CCR1`–`CCR4` `0x34`–`0x40` — capture/compare registers

## Inputs Required for STM32U083 UART (USART2)

- MCU family: STM32U0
- MCU model: STM32U083
- USART2 base address: `0x40004400`
- Relevant registers (from RM0503):
  - `CR1` `0x00`
  - `CR2` `0x04`
  - `CR3` `0x08`
  - `BRR` `0x0C` — baud rate register
  - `ISR` `0x1C` — interrupt and status register (RO)
  - `ICR` `0x20` — interrupt flag clear register (WO)
  - `RDR` `0x24` — receive data register (RO)
  - `TDR` `0x28` — transmit data register (WO)

## Approach

Follow the identical pattern as GPIO (Steps 7–8):

1. Define generic enumerations and primary (unimplemented) template in `include/ohal/timer.hpp`
   and `include/ohal/uart.hpp`.
2. Implement partial specialisations in the STM32U083 platform headers
   (`platforms/stm32u0/models/stm32u083/timer.hpp` and `uart.hpp`).
3. Model `RDR` (receive) as `ReadOnly` and `TDR` (transmit) as `WriteOnly` using `BitField<>`
   access control — reading from `TDR` or writing to `RDR` is a compile error.
4. Model `EGR` as `WriteOnly` — reading from it is a compile error.
