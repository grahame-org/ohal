# Step 21 – External Interrupts and NVIC (`ohal::exti`, `ohal::nvic`, `ohal::irq`)

**Goal:** Implement the three headers that together enable the USER button (PC13, falling-edge
EXTI, EXTI4_15 NVIC IRQ) to be configured and handled entirely through ohal, replacing
`BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI)` and `BSP_PB_IRQHandler(BUTTON_USER)`.

This step is the focused implementation of the interrupt layer designed in
[Step 17](step-17-interrupt-handling.md), scoped to the Cortex-M/STM32U0 path and to the
specific peripherals needed by `test_project`. MSP430 port interrupts (section 17.5) are
out of scope here.

**Prerequisites:**

- [Step 17 (Interrupt Handling)](step-17-interrupt-handling.md): generic interface design.
- [Step 19 (Clock Enable)](step-19-clock-enable.md): `clock::Enable<Syscfg>::enable()` must
  be called before the EXTI configuration register (`EXTICR`) is written, since SYSCFG is the
  clock domain that gates write access to EXTICR on STM32U0.

This step is split into three sequential sub-steps, each landing as a separate PR:

| Sub-step | Header       | Unlocks                                             |
| -------- | ------------ | --------------------------------------------------- |
| 21a      | `ohal::irq`  | Global interrupt enable/disable and RAII guard      |
| 21b      | `ohal::nvic` | Per-IRQ enable, disable, priority on Cortex-M       |
| 21c      | `ohal::exti` | EXTI line trigger config, pending flags, ISR helper |

---

## Sub-step 21a — Global Interrupt Control (`ohal::irq`)

### Inputs Required

The following instructions and intrinsics are architecture-defined (ARM Cortex-M0+) and do
not require an RM0503 look-up:

| Operation                       | Cortex-M0+ mechanism                       |
| ------------------------------- | ------------------------------------------ |
| Enable all maskable interrupts  | `CPSIE i` (`__enable_irq()` in CMSIS)      |
| Disable all maskable interrupts | `CPSID i` (`__disable_irq()`)              |
| Save interrupt state            | Read PRIMASK register (`__get_PRIMASK()`)  |
| Restore interrupt state         | Write PRIMASK register (`__set_PRIMASK()`) |

> **Note:** The RAII guard must save PRIMASK before disabling and restore it on destruction
> to support safe nesting. Do not use `__enable_irq()` in the destructor — always restore
> the saved value.

### Generic interface sketch (`include/ohal/irq.hpp`)

```cpp
namespace ohal::irq {

template <typename Family>
struct GlobalController {
    static_assert(sizeof(Family) == 0,
        "ohal: irq::GlobalController is not implemented for this MCU family.");
};

template <typename Family>
class Guard {
public:
    Guard() noexcept;   // saves PRIMASK, then disables
    ~Guard() noexcept;  // restores saved PRIMASK
    Guard(const Guard&)            = delete;
    Guard& operator=(const Guard&) = delete;
private:
    uint32_t saved_primask_;
};

} // namespace ohal::irq
```

### STM32U0 specialisation

The Cortex-M0+ implementation lives in
`include/ohal/platforms/stm32u0/models/stm32u083/irq.hpp` and uses CMSIS intrinsics
(`__get_PRIMASK`, `__set_PRIMASK`, `__enable_irq`, `__disable_irq`) or inline assembly.

### Acceptance criteria (21a)

1. `irq::Guard<STM32U0Family> g;` saves the current PRIMASK value and then disables interrupts.
2. When `g` goes out of scope, the original PRIMASK value is restored (not unconditionally
   enabled).
3. Two nested `Guard` objects: inner destructs first, PRIMASK is still disabled; outer
   destructs second, PRIMASK is restored to the original enabled state.
4. `GlobalController<STM32U0Family>::enable()` and `disable()` compile to a single instruction
   (`CPSIE i` / `CPSID i`) with no overhead when optimised.
5. `has_global_irq_control<STM32U0Family>` specialises to `true_type`.

---

## Sub-step 21b — NVIC Controller (`ohal::nvic`)

### Inputs Required

The NVIC registers are at fixed ARM architecture addresses (common to all Cortex-M cores):

| Register  | Address      | Access | Description                                          |
| --------- | ------------ | ------ | ---------------------------------------------------- |
| `ISER[0]` | `0xE000E100` | RW     | Interrupt Set Enable Register                        |
| `ICER[0]` | `0xE000E180` | RW     | Interrupt Clear Enable Register                      |
| `ISPR[0]` | `0xE000E200` | RW     | Interrupt Set Pending Register                       |
| `ICPR[0]` | `0xE000E280` | RW     | Interrupt Clear Pending Register                     |
| `IPR[0]`  | `0xE000E400` | RW     | Interrupt Priority Register (4 IRQs per 32-bit word) |

**Cortex-M0+ priority:** only bits `[7:6]` of each 8-bit priority field are implemented,
giving four priority levels (0 = highest).

### RM0503 look-up required before implementation

> Before writing `irq_numbers.hpp`, gather the full vector table from **RM0503 Table 67**
> (or equivalent table titled "STM32U083 interrupt and exception vectors"). The following
> IRQ numbers are the minimum required for the `test_project` conversion; all values must
> be confirmed from the table before use in code:
>
> | Peripheral | IRQ name        | Expected IRQ# | Confirm from RM0503 |
> | ---------- | --------------- | ------------- | ------------------- |
> | EXTI 4–15  | `EXTI4_15_IRQn` | 7             | ✗ must confirm      |
>
> The full table should also include IRQ numbers for TIM2, TIM3, USART2, LPUART1, I2C1, SPI1,
> ADC1, and DMA1 channels 1–7, to make `irq_numbers.hpp` useful beyond the test_project scope.

### File to create

`include/ohal/platforms/stm32u0/models/stm32u083/irq_numbers.hpp`:

```cpp
namespace ohal::platforms::stm32u0::stm32u083 {

enum class IrqNumber : uint8_t {
    // Values must be verified from RM0503 Table 67 before this file is committed.
    Exti0_1    = /* RM0503 value */,
    Exti2_3    = /* RM0503 value */,
    Exti4_15   = /* RM0503 value */,  // Required for test_project USER button
    Tim2       = /* RM0503 value */,
    Tim3       = /* RM0503 value */,
    Usart2     = /* RM0503 value */,
    I2c1       = /* RM0503 value */,
    // … add all remaining IRQs from the RM0503 vector table …
};

} // namespace ohal::platforms::stm32u0::stm32u083
```

### Generic interface sketch (`include/ohal/nvic.hpp`)

```cpp
namespace ohal::nvic {

template <typename Family>
struct has_nvic : std::false_type {};

template <typename Family, auto Irq>
struct Controller {
    static_assert(sizeof(Family) == 0,
        "ohal: nvic::Controller is not implemented for this MCU family.");
};

} // namespace ohal::nvic
```

The STM32U0 specialisation exposes `enable_irq()`, `disable_irq()`,
`set_priority(uint8_t)`, `get_priority()`, `set_pending()`, `clear_pending()`,
`is_pending()`.

### Acceptance criteria (21b)

1. `nvic::Controller<STM32U0Family, IrqNumber::Exti4_15>::enable_irq()` sets bit `Exti4_15`
   in `NVIC_ISER[0]` at `0xE000E100`.
2. `disable_irq()` sets the same bit in `NVIC_ICER[0]` at `0xE000E180`.
3. `set_priority(1)` writes `1 << 6` (Cortex-M0+ only implements bits `[7:6]`) to the
   correct 8-bit slot in `NVIC_IPR`.
4. `has_nvic<STM32U0Family>` is `true_type`; `has_nvic<MSP430FR2XX>` is `false_type`.
5. Calling any `nvic::` method for a non-NVIC family fails with `static_assert`.

---

## Sub-step 21c — External Interrupt Lines (`ohal::exti`)

### Inputs Required

The following register layout is taken from the CMSIS device header `stm32u083xx.h`
(struct `EXTI_TypeDef`). **EXTI base address: `0x40021800`**.

> **Important correction vs Step 17:** On STM32U0, the EXTICR registers are part of the
> **EXTI block** (at `EXTI_BASE + 0x60`), not in the SYSCFG block. The SYSCFG base address
> `0x40010000` is used only for other SYSCFG configuration (CFGR1, CFGR2, etc.). Step 17
> described SYSCFG EXTICR which applies to other STM32 families; for STM32U0 use the offsets
> below.

| Register    | Offset | Access   | Description                                                        |
| ----------- | ------ | -------- | ------------------------------------------------------------------ |
| `RTSR1`     | `0x00` | RW       | Rising trigger selection (bit N = enable rising trigger on line N) |
| `FTSR1`     | `0x04` | RW       | Falling trigger selection                                          |
| `SWIER1`    | `0x08` | RW       | Software interrupt/event trigger (write 1 to trigger)              |
| `RPR1`      | `0x0C` | RW (W1C) | Rising edge pending register (write 1 to clear)                    |
| `FPR1`      | `0x10` | RW (W1C) | Falling edge pending register (write 1 to clear)                   |
| `EXTICR[0]` | `0x60` | RW       | EXTI port selection for lines 0–3 (8 bits per line)                |
| `EXTICR[1]` | `0x64` | RW       | EXTI port selection for lines 4–7                                  |
| `EXTICR[2]` | `0x68` | RW       | EXTI port selection for lines 8–11                                 |
| `EXTICR[3]` | `0x6C` | RW       | EXTI port selection for lines 12–15                                |
| `IMR1`      | `0x80` | RW       | Interrupt mask register (bit N = 1 unmasks line N)                 |
| `EMR1`      | `0x84` | RW       | Event mask register                                                |

**EXTICR field layout:** Each `EXTICR[n]` register holds four 8-bit fields (one per EXTI
line). For line N: the field is in `EXTICR[N / 4]` at bit offset `(N % 4) * 8`.

### RM0503 look-up required before implementation

> Before writing any EXTICR specialisation:
>
> 1. **EXTICR port encoding** — confirm from **RM0503** the 8-bit value that selects each
>    GPIO port for an EXTI line. The expected encoding (derived from the ST HAL
>    `GPIO_GET_INDEX` function) is: PA = 0x00, PB = 0x01, PC = 0x02, PD = 0x03, PE = 0x04,
>    PF = 0x05. This must be verified from the "EXTICR" register description in RM0503 before
>    it is used in `irq_numbers.hpp` or any test.
> 2. **IMR/EMR reset values** — confirm whether IMR1 and EMR1 have non-zero reset values
>    (some STM32 families mask certain lines at reset). Any non-zero reset value changes the
>    required read-modify-write sequence for `enable_interrupt()`.
>
> For PC13 specifically (BUTTON_USER):
>
> - Line 13 is in `EXTICR[3]` (offset `0x6C`) at bits `[15:8]`.
> - The port-selection value to write for GPIOC must be confirmed from RM0503.

### Generic interface sketch (`include/ohal/exti.hpp`)

```cpp
namespace ohal::exti {

template <typename Family>
struct has_exti : std::false_type {};

enum class Trigger : uint8_t { Rising = 0, Falling = 1, RisingFalling = 2 };

template <typename Port, uint8_t PinNum>
struct Line {
    static_assert(sizeof(Port) == 0,
        "ohal: exti::Line is not implemented for this MCU.");
};

} // namespace ohal::exti
```

The STM32U0 specialisation of `Line<Port, PinNum>` exposes:

| Method                         | Description                                       |
| ------------------------------ | ------------------------------------------------- |
| `configure(Trigger t)`         | Sets RTSR1/FTSR1 and writes port code into EXTICR |
| `enable_interrupt()`           | Sets bit N in IMR1                                |
| `disable_interrupt()`          | Clears bit N in IMR1                              |
| `enable_event()`               | Sets bit N in EMR1                                |
| `is_pending_rising() -> bool`  | Reads bit N in RPR1                               |
| `is_pending_falling() -> bool` | Reads bit N in FPR1                               |
| `clear_pending_rising()`       | Writes 1 to bit N in RPR1 (W1C)                   |
| `clear_pending_falling()`      | Writes 1 to bit N in FPR1 (W1C)                   |
| `trigger_software()`           | Writes 1 to bit N in SWIER1                       |

### Acceptance criteria (21c)

1. `exti::Line<PortC, 13>::configure(Trigger::Falling)`:
   - Sets bit 13 in FTSR1.
   - Clears bit 13 in RTSR1.
   - Writes the GPIOC port code (value confirmed from RM0503) to bits `[15:8]` of
     `EXTICR[3]` (at `0x40021800 + 0x6C`).
2. `exti::Line<PortC, 13>::enable_interrupt()` sets bit 13 in IMR1 (at
   `0x40021800 + 0x80`).
3. `exti::Line<PortC, 13>::is_pending_falling()` reads bit 13 from FPR1 (at
   `0x40021800 + 0x10`).
4. `exti::Line<PortC, 13>::clear_pending_falling()` writes 1 to bit 13 in FPR1
   without reading FPR1 first (W1C — no read-modify-write needed).
5. `has_exti<STM32U0Family>` is `true_type`; `has_exti<MSP430FR2XX>` is `false_type`.
6. `exti::Line<>` on a non-EXTI family fires `static_assert`.
7. All existing host tests pass; `bash lint.sh` reports no errors.

---

## Host Test Files

| File                                 | Covers                                                |
| ------------------------------------ | ----------------------------------------------------- |
| `tests/host/test_irq_stm32u083.cpp`  | `Guard` save/restore; `enable()`/`disable()`          |
| `tests/host/test_nvic_stm32u083.cpp` | ISER/ICER/IPR register writes for Exti4_15 IRQ        |
| `tests/host/test_exti_stm32u083.cpp` | RTSR1/FTSR1/EXTICR/IMR1/FPR1 register writes for PC13 |

---

## `test_project` Conversion

After all three sub-steps are complete, replace the button initialisation:

```cpp
// Before — ST HAL/BSP (main.cpp)
BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

// After — ohal
// PC13: input, pull-up (pin already configured; only interrupt setup remains)
using ButtonLine = ohal::exti::Line<ohal::gpio::PortC, 13U>;
using Exti4_15   = ohal::nvic::Controller<
    ohal::platforms::stm32u0::STM32U0Family,
    ohal::platforms::stm32u0::stm32u083::IrqNumber::Exti4_15>;

ButtonLine::configure(ohal::exti::Trigger::Falling);
ButtonLine::clear_pending_falling();
ButtonLine::enable_interrupt();
Exti4_15::enable_irq();
Exti4_15::set_priority(1U); // priority 1 of 3 (Cortex-M0+: 0=highest, 3=lowest; only bits [7:6] implemented)
ohal::irq::GlobalController<ohal::platforms::stm32u0::STM32U0Family>::enable();
```

Replace the ISR body in `stm32u0xx_it.cpp`:

```cpp
// Before
void EXTI4_15_IRQHandler(void) {
    BSP_PB_IRQHandler(BUTTON_USER);
}

// After
void EXTI4_15_IRQHandler(void) {
    if (ohal::exti::Line<ohal::gpio::PortC, 13U>::is_pending_falling()) {
        ohal::exti::Line<ohal::gpio::PortC, 13U>::clear_pending_falling();
        // application callback here
    }
}
```

Remove all BSP push-button includes and the `EXTI_HandleTypeDef hpb_exti[]` extern once
BSP_PB is no longer used.
