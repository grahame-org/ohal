# Step 20 – UART Implementation (`ohal::uart`)

**Goal:** Complete the stub in
`include/ohal/platforms/stm32u0/models/stm32u083/uart.hpp` (left empty after Step 10)
with a full `ohal::uart::Port<Usart2>` specialisation for USART2. This replaces
`BSP_COM_Init(COM1, &BspCOMInit)` in `test_project` with zero-overhead ohal calls.

**Prerequisites:**

- [Step 10 (Timer and UART)](step-10-timer-uart.md): `include/ohal/uart.hpp` exists with the
  generic interface stub; the platform `uart.hpp` is an empty placeholder.
- [Step 18 (GPIO AF)](step-18-gpio-alternate-function.md): `set_alternate_function()` is
  available to configure USART2 TX/RX pins (PA2 AF1 / PA3 AF1 — verify from RM0503).
- [Step 19 (Clock Enable)](step-19-clock-enable.md): `clock::Enable<Usart2>::enable()` is
  available to enable the USART2 peripheral clock before register access.

---

## Inputs Required

### USART2 register map

Register offsets are taken from the CMSIS device header `stm32u083xx.h` (struct
`USART_TypeDef`). USART2 base address: **`0x40004400`** (from memory-map entry in
`stm32u083xx.h`).

| Register | Offset | Access | Description                                                  |
| -------- | ------ | ------ | ------------------------------------------------------------ |
| `CR1`    | `0x00` | RW     | Control register 1 — UART enable, word length, parity, TE/RE |
| `CR2`    | `0x04` | RW     | Control register 2 — stop bits, clock configuration          |
| `CR3`    | `0x08` | RW     | Control register 3 — hardware flow control, DMA              |
| `BRR`    | `0x0C` | RW     | Baud rate register                                           |
| `RQR`    | `0x18` | WO     | Request register (software-initiated actions)                |
| `ISR`    | `0x1C` | RO     | Interrupt and status register                                |
| `ICR`    | `0x20` | WO     | Interrupt flag clear register (write 1 to clear)             |
| `RDR`    | `0x24` | RO     | Receive data register                                        |
| `TDR`    | `0x28` | WO     | Transmit data register                                       |

### RM0503 look-up required before implementation

> The following bit positions and encodings are **not** extractable from the CMSIS header
> comments alone and must be sourced from **RM0503** before any implementation or test can
> be written:
>
> **CR1 bit fields (RM0503 section "USART control register 1"):**
>
> - `UE` (USART Enable) — bit position within CR1
> - `TE` (Transmitter Enable) — bit position
> - `RE` (Receiver Enable) — bit position
> - `M0` (Word length bit 0) — bit position; together with `M1` selects 7/8/9-bit frames
> - `M1` (Word length bit 1) — bit position
> - `PCE` (Parity Control Enable) — bit position
> - `PS` (Parity Selection: 0=even, 1=odd) — bit position
> - `OVER8` (Oversampling mode: 0=16x, 1=8x) — bit position; needed for BRR calculation
>
> **CR2 bit fields:**
>
> - `STOP[1:0]` (Stop bits: 00=1, 01=0.5, 10=2, 11=1.5) — bit position of the LSB of
>   the 2-bit field
>
> **CR3 bit fields:**
>
> - `RTSE` (RTS hardware flow control) — bit position
> - `CTSE` (CTS hardware flow control) — bit position
>
> **BRR calculation formula:**
>
> - With OVER8=0 (16x oversampling): `BRR = f_CK / baud_rate`
> - With OVER8=1 (8x oversampling): different formula — verify from RM0503
> - The peripheral clock frequency `f_CK` depends on the clock tree configured in Step 22.
>   For the `test_project` baseline (HSI 16 MHz, no PLL), `f_CK = 16 000 000`.
>
> **ISR status bits:**
>
> - `TXE` or `TXFNF` (Transmit data register empty / TX FIFO not full) — bit position;
>   needed to poll before writing TDR in blocking-transmit mode
> - `RXNE` or `RXFNE` (Receive data register not empty) — bit position; needed to poll
>   before reading RDR in blocking-receive mode
>
> **AF number for USART2 pins on STM32U083RCT:**
>
> - PA2 as USART2_TX — AF number (expected AF1, must confirm from RM0503 Annex A)
> - PA3 as USART2_RX — AF number (expected AF1, must confirm from RM0503 Annex A)

---

## Approach

This step splits into two distinct PRs to keep each change reviewable independently:

### PR A — Generic interface (`include/ohal/uart.hpp`)

Extend the existing stub (created in Step 10) with enumerations and the primary unimplemented
template. This PR adds no hardware-specific code and requires no RM0503 input.

```cpp
// include/ohal/uart.hpp  (complete content for PR A)
#ifndef OHAL_UART_HPP
#define OHAL_UART_HPP

#include <cstdint>

namespace ohal::uart {

enum class WordLength : uint8_t { Bits7 = 0, Bits8 = 1, Bits9 = 2 };
enum class StopBits   : uint8_t { One = 0, Half = 1, Two = 2, OneAndHalf = 3 };
enum class Parity     : uint8_t { None = 0, Even = 1, Odd  = 2 };
enum class HwFlowCtl  : uint8_t { None = 0, Rts  = 1, Cts  = 2, RtsCts = 3 };

/// Configuration bundle passed to Port<>::configure().
struct Config {
    uint32_t   baud_rate    = 115200U;
    WordLength word_length  = WordLength::Bits8;
    StopBits   stop_bits    = StopBits::One;
    Parity     parity       = Parity::None;
    HwFlowCtl  hw_flow_ctl  = HwFlowCtl::None;
};

/// Generic UART/USART port.  Platform headers specialise this for each
/// peripheral instance (e.g. Usart2).
template <typename Instance>
struct Port {
    static_assert(sizeof(Instance) == 0,
        "ohal: uart::Port is not implemented for the selected MCU. "
        "Ensure -DOHAL_FAMILY_* and -DOHAL_MODEL_* are set correctly.");
};

} // namespace ohal::uart

#endif // OHAL_UART_HPP
```

Wire `#include <ohal/uart.hpp>` into `include/ohal/ohal.hpp`.

### PR B — STM32U083 specialisation

After RM0503 look-up is complete, implement
`include/ohal/platforms/stm32u0/models/stm32u083/uart.hpp`:

```cpp
// include/ohal/platforms/stm32u0/models/stm32u083/uart.hpp  (excerpt)
namespace ohal::platforms::stm32u0::stm32u083 {

inline constexpr uintptr_t kUsart2Base = 0x4000'4400U;

// Peripheral instance tag type for USART2
struct Usart2Tag {};

// Register offsets (stm32u083xx.h struct USART_TypeDef)
template <uintptr_t Base>
struct UsartRegs {
    using Cr1 = ohal::core::Register<Base + 0x00U>;
    using Cr2 = ohal::core::Register<Base + 0x04U>;
    using Cr3 = ohal::core::Register<Base + 0x08U>;
    using Brr = ohal::core::Register<Base + 0x0CU>;
    using Isr = ohal::core::Register<Base + 0x1CU>; ///< Status register (RO)
    using Icr = ohal::core::Register<Base + 0x20U>; ///< Clear register (WO)
    using Rdr = ohal::core::Register<Base + 0x24U>; ///< Receive data register (RO)
    using Tdr = ohal::core::Register<Base + 0x28U>; ///< Transmit data register (WO)
};

using Usart2Regs = UsartRegs<kUsart2Base>;

} // namespace ohal::platforms::stm32u0::stm32u083

namespace ohal::uart {

template <>
struct Port<ohal::platforms::stm32u0::stm32u083::Usart2Tag> {
    using Regs = ohal::platforms::stm32u0::stm32u083::Usart2Regs;

    /// Configures baud rate, word length, stop bits, parity, and flow control.
    /// Must be called while UE = 0 (USART disabled).
    /// Peripheral clock frequency used for BRR calculation must match the
    /// actual APB1 clock configured in Step 22.
    static void configure(const ohal::uart::Config& cfg,
                          uint32_t peripheral_clock_hz) noexcept;

    /// Enables the USART (sets UE, TE, RE in CR1).
    static void enable() noexcept;

    /// Disables the USART (clears UE in CR1).
    static void disable() noexcept;

    /// Blocks until TXE/TXFNF is set, then writes one byte to TDR.
    /// ISR and TDR are modelled as separate BitFields so that the access
    /// control of each is enforced at compile time (ISR = RO, TDR = WO).
    static void transmit_byte(uint8_t data) noexcept;

    /// Blocks until RXNE/RXFNE is set, then reads one byte from RDR.
    [[nodiscard]] static uint8_t receive_byte() noexcept;
};

} // namespace ohal::uart
```

> **RDR and TDR access control:** model `RDR` as `ohal::core::Access::ReadOnly` and `TDR` as
> `ohal::core::Access::WriteOnly` using `BitField<>`. A `static_assert` in `BitField::write`
> and `BitField::read` then makes writing to `RDR` or reading from `TDR` a compile error.

---

## Tests to Write (host)

Add `tests/host/test_uart_stm32u083rct.cpp`.

### Configuration tests

Using mock registers, call `Port<Usart2Tag>::configure(cfg, 16'000'000)` with the configs
below and verify the resulting register writes:

| Config                    | Expected BRR value            | Expected CR1 fields set                  |
| ------------------------- | ----------------------------- | ---------------------------------------- |
| 115200 baud, 8N1, no flow | `16'000'000 / 115'200` (≈139) | UE=0 during configure; M0=M1=PCE=OVER8=0 |
| 9600 baud, 8N1, no flow   | `16'000'000 / 9'600` (=1667)  | same                                     |
| 115200 baud, 8E1, no flow | same BRR                      | PCE=1, PS=0                              |

> BRR values above assume OVER8=0 (16× oversampling) and must be confirmed against the
> formula in RM0503 once CR1 bit positions are known.

### Enable / disable tests

- `enable()` sets UE, TE, RE bits in CR1 (confirm exact positions from RM0503 first).
- `disable()` clears UE and leaves TE/RE unchanged (or clears all three — decide policy).

### Transmit / receive tests

- `transmit_byte(0xAB)` with ISR polled mock: after ISR.TXE is pre-set by the mock, the
  function writes `0xAB` to TDR.
- `receive_byte()` with RDR mock pre-loaded: after ISR.RXNE is pre-set, the function reads
  and returns the pre-loaded byte.

### Access-control negative-compile tests

- Writing to `RDR` (modelled as RO) fails with `static_assert`.
- Reading from `TDR` (modelled as WO) fails with `static_assert`.

---

## Acceptance Criteria

1. `Port<Usart2Tag>::configure({.baud_rate=115200, ...}, 16'000'000)` writes the correct BRR
   value (per RM0503 formula) and leaves `UE = 0` in CR1.
2. `Port<Usart2Tag>::enable()` sets `UE`, `TE`, and `RE` in CR1.
3. `Port<Usart2Tag>::transmit_byte(x)` writes `x` to TDR only (no other register changes).
4. `Port<Usart2Tag>::receive_byte()` reads from RDR only.
5. Attempting `Regs::Rdr::write(...)` in application code produces a `static_assert` at
   compile time (access = ReadOnly).
6. Attempting to read from `TDR` in application code produces a `static_assert` (WriteOnly).
7. All existing host tests pass; `bash lint.sh` reports no errors.

---

## `test_project` Conversion

After this step, replace the BSP COM initialisation in `main.cpp`:

```cpp
// Before — ST HAL/BSP
COM_InitTypeDef BspCOMInit;
BspCOMInit.BaudRate    = 115200;
BspCOMInit.WordLength  = COM_WORDLENGTH_8B;
BspCOMInit.StopBits    = COM_STOPBITS_1;
BspCOMInit.Parity      = COM_PARITY_NONE;
BspCOMInit.HwFlowCtl   = COM_HWCONTROL_NONE;
if (BSP_COM_Init(COM1, &BspCOMInit) != BSP_ERROR_NONE) {
    Error_Handler();
}

// After — ohal (peripheral clock and pin AF already enabled by Steps 18/19)
ohal::uart::Port<ohal::platforms::stm32u0::stm32u083::Usart2Tag>::configure(
    {.baud_rate   = 115'200U,
     .word_length = ohal::uart::WordLength::Bits8,
     .stop_bits   = ohal::uart::StopBits::One,
     .parity      = ohal::uart::Parity::None,
     .hw_flow_ctl = ohal::uart::HwFlowCtl::None},
    16'000'000U);   // peripheral clock = HSI 16 MHz (configured in Step 22)
ohal::uart::Port<ohal::platforms::stm32u0::stm32u083::Usart2Tag>::enable();
```

Remove `COM_InitTypeDef BspCOMInit`, the BSP COM include, and the `Error_Handler()` guard
(if no other BSP calls remain that can fault).
