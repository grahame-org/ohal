# Step 17 – Interrupt Handling

**Goal:** Provide a zero-overhead, type-safe interrupt management layer across all supported MCU
families. The layer covers:

- **Global interrupt enable / disable** — architecture-portable on/off of all maskable interrupts.
- **Interrupt controller (NVIC on Cortex-M)** — per-IRQ enable, disable, priority, pending, and active
  queries.
- **External interrupt lines (EXTI on STM32)** — edge-trigger configuration for GPIO pins and other
  event sources routed through the external interrupt controller.
- **Peripheral interrupt sources** — thin additions to the existing `ohal::timer`, `ohal::uart`, and
  subsequent peripheral interfaces to expose each peripheral's own interrupt-enable / flag-clear
  methods.
- **MSP430 port interrupts** — per-port interrupt enable, edge select, and flag management for families
  without an NVIC.

**Prerequisites:** [Step 10 (Timer and UART)](step-10-timer-uart.md) and [Step 11 (Unit
Testing)](step-11-unit-testing.md) must be completed so that the peripheral register maps and
mock-register infrastructure are already in place. [Step 16 (Additional
Peripherals)](step-16-additional-peripherals.md) should be in progress in parallel: each new
peripheral in Step 16 adds interrupt-enable bits that this step documents how to control. [Step 3
(Conventional Commits)](step-03-conventional-commits-merge-queue.md) must be in place so that each
sub-step lands via a conventional commit.

Each sub-step follows the same three-layer pattern used in every other peripheral:

1. **Generic interface** (`include/ohal/<header>.hpp`) — enumerations, tag types, primary (unimplemented)
   template, capability trait primary templates.
2. **Platform specialisation** (`platforms/<family>/models/<model>/<header>.hpp`) — partial specialisations
   using `Register<>` and `BitField<>`.
3. **Host unit tests** — mock register addresses, positive and negative-compile tests.

## 17.1 Implementation Order

Implement sub-steps in this order:

| Order | Sub-step                            | Reason                                                                                                                                  |
| ----- | ----------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------- |
| 1     | **Global enable / disable**         | Portable foundation. Required by all subsequent sub-steps to guard critical sections during configuration.                              |
| 2     | **NVIC (Cortex-M interrupt ctrl)**  | Core ARM mechanism for enabling individual IRQs. Required before any peripheral ISR can fire.                                           |
| 3     | **Peripheral interrupt sources**    | Adds `enable_interrupt()` / `disable_interrupt()` / `clear_flag()` to existing peripheral types. Exercises NVIC configuration in tests. |
| 4     | **EXTI (STM32 external interrupt)** | Builds on NVIC: EXTI lines are routed to NVIC IRQs. Requires SYSCFG EXTICR register map.                                                |
| 5     | **MSP430 port interrupts**          | Non-ARM path: no NVIC, per-port IE/IES/IFG registers. Capability-gated behind `has_nvic`.                                               |

---

## 17.2 Global Interrupt Enable / Disable (`ohal::irq`)

### Namespace: `ohal::irq`

This sub-step provides the global interrupt on/off operations and an RAII guard. These map to a
single instruction on all supported architectures (`CPSIE i` / `CPSID i` on Cortex-M, `EINT()` /
`DINT()` on MSP430) and must compile to exactly that instruction with no overhead.

### Inputs Required

| Item                   | STM32U083 (Cortex-M0+)                        | MSP430FR2355                                  |
| ---------------------- | --------------------------------------------- | --------------------------------------------- |
| Enable all interrupts  | `CPSIE i` intrinsic (ARM CMSIS or `__asm`)    | GIE bit set in SR (`EINT()` / `__eint()`)     |
| Disable all interrupts | `CPSID i` intrinsic                           | GIE bit clear in SR (`DINT()` / `__dint()`)   |
| Save/restore state     | `__get_PRIMASK()` / `__set_PRIMASK()` (CMSIS) | `__get_SR_register()` / `__bis_SR_register()` |
| Architecture define    | `__ARM_ARCH` or `OHAL_FAMILY_STM32U0`         | `OHAL_FAMILY_MSP430FR2XX`                     |

The RAII guard must save the previous interrupt state (PRIMASK on Cortex-M, GIE bit on MSP430)
and restore it unconditionally on destruction, enabling safe nesting.

### Generic Interface Sketch

```cpp
// include/ohal/irq.hpp  (excerpt)
namespace ohal::irq {

// Enable all maskable interrupts.
// On Cortex-M: CPSIE i.  On MSP430: __eint().
template <typename Family>
struct GlobalController {
    static_assert(sizeof(Family) == 0,
        "ohal: irq::GlobalController is not implemented for the selected MCU family.");
};

// RAII guard: disables interrupts on construction, restores previous state on destruction.
template <typename Family>
class Guard {
public:
    Guard();   // saves state, then disables
    ~Guard();  // restores saved state
private:
    typename GlobalController<Family>::state_type saved_state_;
};

} // namespace ohal::irq
```

### Capability Traits

- `has_global_irq_control<Family>` — `true` for all supported families (every MCU can mask/unmask
  all interrupts).

---

## 17.3 NVIC / Interrupt Controller (`ohal::nvic`)

### Namespace: `ohal::nvic`

The Nested Vectored Interrupt Controller is an ARM Cortex-M core peripheral at fixed addresses defined
by the ARM architecture. Its registers are not in the normal peripheral bus address space. MSP430FR2355
and other non-ARM families do not have an NVIC; use `has_nvic<Family>` to guard usage at compile time.

### Register Map (ARM architecture — common to all Cortex-M cores)

| Register   | Address      | Description                                                               |
| ---------- | ------------ | ------------------------------------------------------------------------- |
| `ISER[0]`  | `0xE000E100` | Interrupt Set Enable Register (RW)                                        |
| `ICER[0]`  | `0xE000E180` | Interrupt Clear Enable Register (WO)                                      |
| `ISPR[0]`  | `0xE000E200` | Interrupt Set Pending Register (RW)                                       |
| `ICPR[0]`  | `0xE000E280` | Interrupt Clear Pending Register (WO)                                     |
| `IPR[0–7]` | `0xE000E400` | Interrupt Priority Registers (RW; 4 IRQs per 32-bit word, 8 bits per IRQ) |

**Cortex-M0+ priority resolution:** Only bits `[7:6]` of each 8-bit priority field are implemented.
This gives 4 priority levels (0 = highest, 3 = lowest). Cortex-M3/M4 devices implement more bits.

### Inputs Required (STM32U083)

Before implementing, gather from RM0503 Table 67 (vector table):

- IRQ numbers for all peripherals. The following are required for the peripherals already modelled:

| Peripheral | IRQ name                  | IRQ number |
| ---------- | ------------------------- | ---------- |
| EXTI 0–1   | `EXTI0_1_IRQn`            | 5          |
| EXTI 2–3   | `EXTI2_3_IRQn`            | 6          |
| EXTI 4–15  | `EXTI4_15_IRQn`           | 7          |
| TIM2       | `TIM2_IRQn`               | 15         |
| TIM3       | `TIM3_IRQn`               | 16         |
| USART2     | `USART2_IRQn`             | 28         |
| LPUART1    | `LPUART1_IRQn`            | 29         |
| SPI1       | `SPI1_IRQn`               | 25         |
| I2C1       | `I2C1_IRQn`               | 23         |
| ADC1       | `ADC1_IRQn`               | 12         |
| DMA1 Ch1–7 | `DMA1_Channel1_IRQn` etc. | 9–15       |

The complete IRQ table from RM0503 must be recorded in a platform-layer header
(`platforms/stm32u0/models/stm32u083/irq_numbers.hpp`) as a `constexpr` enum before any
`ohal::nvic` specialisation is written.

### Generic Interface Sketch

```cpp
// include/ohal/nvic.hpp  (excerpt)
namespace ohal::nvic {

// Capability trait: false for non-Cortex-M families (e.g. MSP430FR2XX)
template <typename Family>
struct has_nvic : std::false_type {};

// IrqNumber is a platform-provided enum (e.g. ohal::stm32u0::IrqNumber::Tim2).
// All nvic:: functions static_assert has_nvic<Family> to prevent use on non-NVIC platforms.

template <typename Family, typename IrqNumber, IrqNumber Irq>
struct Controller {
    static_assert(sizeof(Family) == 0,
        "ohal: nvic::Controller is not implemented for the selected MCU family.");
};

} // namespace ohal::nvic
```

The platform specialisation exposes:

| Method                           | Description                                                                     |
| -------------------------------- | ------------------------------------------------------------------------------- |
| `enable_irq()`                   | Set the corresponding bit in `NVIC_ISER`                                        |
| `disable_irq()`                  | Set the corresponding bit in `NVIC_ICER`                                        |
| `set_priority(uint8_t priority)` | Write the priority field in `NVIC_IPR`                                          |
| `get_priority() -> uint8_t`      | Read the priority field from `NVIC_IPR`                                         |
| `set_pending()`                  | Set the corresponding bit in `NVIC_ISPR`                                        |
| `clear_pending()`                | Set the corresponding bit in `NVIC_ICPR`                                        |
| `is_pending() -> bool`           | Read the corresponding bit from `NVIC_ISPR`                                     |
| `is_active() -> bool`            | Read the corresponding bit from `NVIC_IABR` (Cortex-M3+ only; capability-gated) |

### Capability Traits

- `has_nvic<Family>` — `true_type` for `STM32U0` and other Cortex-M families; `false_type` for
  `MSP430FR2XX`.
- `nvic_priority_bits<Family>` — `uint8_t` constant: 2 for Cortex-M0+, 3 for Cortex-M3, 4 for
  Cortex-M4. Needed to validate priority arguments.

---

## 17.4 External Interrupt Lines (`ohal::exti`)

### Namespace: `ohal::exti`

STM32 devices route GPIO pin transitions and other event sources through an External Interrupt /
Event Controller (EXTI). Each EXTI line can trigger a CPU interrupt (via NVIC), a hardware event
(waking from WFE), or both. Configuring a GPIO edge interrupt requires both the EXTI registers and the
SYSCFG EXTICR registers (to select which GPIO port drives each EXTI line).

MSP430FR2355 does not have a separate EXTI block — port interrupt configuration is covered in
[section 17.5](#175-msp430-port-interrupts-msp430fr2xx-only). Use `has_exti<Family>` to gate
compilation.

### Inputs Required (STM32U083)

**EXTI block** (base address `0x40021800`, from RM0503):

| Register | Offset | Description                                                  | Access   |
| -------- | ------ | ------------------------------------------------------------ | -------- |
| `IMR1`   | `0x00` | Interrupt mask register: bit N=1 unmasks EXTI line N         | RW       |
| `EMR1`   | `0x04` | Event mask register: bit N=1 enables event on EXTI line N    | RW       |
| `RTSR1`  | `0x08` | Rising trigger selection: bit N=1 triggers on rising edge    | RW       |
| `FTSR1`  | `0x0C` | Falling trigger selection: bit N=1 triggers on falling edge  | RW       |
| `SWIER1` | `0x10` | Software interrupt/event register: writing 1 triggers line N | RW       |
| `RPR1`   | `0x14` | Rising edge pending register (W1C — write 1 to clear)        | RW (W1C) |
| `FPR1`   | `0x18` | Falling edge pending register (W1C)                          | RW (W1C) |

**SYSCFG block** (base address `0x40010000`, from RM0503) — routes GPIO ports to EXTI lines:

| Register  | Offset | Description                                           |
| --------- | ------ | ----------------------------------------------------- |
| `EXTICR1` | `0x08` | EXTI source selection for lines 0–3 (4 bits per line) |
| `EXTICR2` | `0x0C` | EXTI source selection for lines 4–7                   |
| `EXTICR3` | `0x10` | EXTI source selection for lines 8–11                  |
| `EXTICR4` | `0x14` | EXTI source selection for lines 12–15                 |

EXTICR field encoding (from RM0503): `0x0` = PA, `0x1` = PB, `0x2` = PC, `0x3` = PD, `0x4` = PE,
`0x5` = PF. This encoding must be recorded before the SYSCFG specialisation is written.

EXTI line N corresponds to GPIO pin N on the selected port. The EXTI line number equals the pin
number; only one port per pin number can be the active EXTI source at a time.

### Generic Interface Sketch

```cpp
// include/ohal/exti.hpp  (excerpt)
namespace ohal::exti {

// Capability trait
template <typename Family>
struct has_exti : std::false_type {};

enum class Trigger : uint8_t { Rising = 0, Falling = 1, RisingFalling = 2 };

// Line<Port, PinNum> ties one EXTI line to a specific GPIO pin at compile time.
template <typename Port, uint8_t PinNum>
struct Line {
    static_assert(sizeof(Port) == 0,
        "ohal: exti::Line is not implemented for the selected MCU.");
};

} // namespace ohal::exti
```

The platform specialisation exposes:

| Method                         | Description                                           |
| ------------------------------ | ----------------------------------------------------- |
| `configure(Trigger t)`         | Set RTSR1/FTSR1 bits and route port via SYSCFG EXTICR |
| `enable_interrupt()`           | Set the corresponding bit in EXTI IMR1                |
| `disable_interrupt()`          | Clear the corresponding bit in EXTI IMR1              |
| `enable_event()`               | Set the corresponding bit in EXTI EMR1                |
| `is_pending_rising() -> bool`  | Read the corresponding bit in EXTI RPR1               |
| `is_pending_falling() -> bool` | Read the corresponding bit in EXTI FPR1               |
| `clear_pending_rising()`       | Write 1 to the corresponding bit in EXTI RPR1 (W1C)   |
| `clear_pending_falling()`      | Write 1 to the corresponding bit in EXTI FPR1 (W1C)   |
| `trigger_software()`           | Write 1 to the corresponding bit in EXTI SWIER1       |

### Design Note: Shared EXTI lines

On STM32, EXTI lines 0–1 share one NVIC IRQ (`EXTI0_1_IRQn`), lines 2–3 share another
(`EXTI2_3_IRQn`), and lines 4–15 share a third (`EXTI4_15_IRQn`). The ISR body must read `RPR1`
or `FPR1` to determine which pin(s) fired. OHAL's `exti::Line<Port, PinNum>::is_pending_rising()`
provides the individual pending check inside the ISR.

---

## 17.5 MSP430 Port Interrupts (MSP430FR2XX only)

### Namespace: `ohal::irq` (via `ohal::gpio::Pin<>` extension)

MSP430FR2355 has no NVIC. Individual port pins can trigger interrupts through per-port IE, IES, IFG,
and IV registers. These are accessible through an extension to the GPIO platform specialisation rather
than a separate header.

### Inputs Required (MSP430FR2355)

Gather from the MSP430FR2355 datasheet and SLAU445I family user's guide:

| Register | Port | Address  | Description                                                  | Access |
| -------- | ---- | -------- | ------------------------------------------------------------ | ------ |
| `P1IES`  | P1   | `0x0218` | Interrupt edge select (0=rising, 1=falling per bit)          | RW     |
| `P1IFG`  | P1   | `0x021A` | Interrupt flag register (W1C to clear)                       | RW     |
| `P1IE`   | P1   | `0x021C` | Interrupt enable register                                    | RW     |
| `P1IV`   | P1   | `0x020E` | Interrupt vector word (RO; read clears highest-priority IFG) | RO     |
| `P2IES`  | P2   | `0x0219` | Interrupt edge select                                        | RW     |
| `P2IFG`  | P2   | `0x021B` | Interrupt flag register                                      | RW     |
| `P2IE`   | P2   | `0x021D` | Interrupt enable register                                    | RW     |
| `P2IV`   | P2   | `0x021E` | Interrupt vector word (RO)                                   | RO     |

Only ports P1 and P2 (PortA) support interrupts on MSP430FR2355. P3–P6 do not have interrupt
capability; attempting to configure an interrupt on those ports must produce a `static_assert`.

The exact addresses above must be verified against the MSP430FR2355 datasheet before implementation.

### Generic Interface Extension

Port-interrupt methods are added to the platform specialisation of `ohal::gpio::Pin<>` rather than a
standalone header. The capability trait `supports_interrupt<Port, PinNum>` gates availability:

```cpp
// include/ohal/core/capabilities.hpp  (addition)
namespace ohal::gpio::capabilities {

template <typename Port, uint8_t PinNum>
struct supports_interrupt : std::false_type {};

} // namespace ohal::gpio::capabilities
```

Platform specialisations for MSP430FR2355 set `supports_interrupt` to `true_type` for PortA (P1/P2)
pins only. The methods added to `Pin<>` for interrupt-capable pins are:

| Method                  | Description                      |
| ----------------------- | -------------------------------- |
| `enable_interrupt()`    | Set the pin bit in `PxIE`        |
| `disable_interrupt()`   | Clear the pin bit in `PxIE`      |
| `set_edge(Edge e)`      | Set/clear the pin bit in `PxIES` |
| `is_flag_set() -> bool` | Read the pin bit in `PxIFG`      |
| `clear_flag()`          | Clear the pin bit in `PxIFG`     |

### Design Note: Interrupt Vector Register (`PxIV`)

`PxIV` is a read-only register whose value encodes the highest-priority pending interrupt source.
Reading it atomically clears that flag. This is not per-pin; it is per-port. A separate
`ohal::gpio::Port<PortA>::read_interrupt_vector() -> uint8_t` method exposes this register for use
inside an MSP430 ISR body, where the application checks the returned value against known constants
to determine which pin fired.

---

## 17.6 ISR Function Registration

OHAL does not manage ISR function bodies or vector table entries. On Cortex-M, ISR functions must be
defined by the application as `extern "C"` functions with names matching the vector table entries in
the startup file (e.g., `TIM2_IRQHandler`). On MSP430, ISR functions use the
`__attribute__((interrupt(VECTOR_NUMBER)))` attribute.

OHAL provides, per platform, a header of `constexpr` IRQ-number constants and vector-name string
macros to document the mapping between NVIC IRQ numbers and their corresponding ISR function names.
This is a documentation aid, not a registration mechanism.

### Inputs Required

- The full vector table from the applicable reference manual (RM0503 for STM32U083, datasheet for
  MSP430FR2355), listing every IRQ name, IRQ number, and default handler function name.
- Confirmation of the startup file naming convention used by the project's toolchain
  (e.g., `arm-none-eabi-g++` with a CMSIS-style `startup_stm32u083xx.s`).

---

## 17.7 Host Test Strategy

Each sub-step in this section requires host-side unit tests following the mock-register pattern from
[Step 11](step-11-unit-testing.md):

| Test file                              | Covers                                                                                                    |
| -------------------------------------- | --------------------------------------------------------------------------------------------------------- |
| `test_irq_global_stm32u083.cpp`        | `irq::Guard` saves/restores PRIMASK; `enable()`/`disable()` write the expected PRIMASK value              |
| `test_nvic_stm32u083.cpp`              | `nvic::Controller<>::enable_irq()` sets the correct ISER bit; priority write hits the correct IPR field   |
| `test_exti_stm32u083.cpp`              | `exti::Line<PortA,5>::configure(Trigger::Rising)` sets RTSR1 bit 5 and EXTICR1 field; pending checks      |
| `test_irq_msp430fr2355.cpp`            | `Pin<PortA,2>::enable_interrupt()` sets P1IE bit 2; `clear_flag()` clears P1IFG bit 2; `has_nvic` = false |
| Negative-compile tests (all platforms) | `exti::Line` on a non-EXTI family → `static_assert`; `nvic::Controller` on MSP430 → `static_assert`       |
