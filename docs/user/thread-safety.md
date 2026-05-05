# Thread safety and interrupt safety

`ohal` is a zero-overhead hardware abstraction library. It does not include a scheduler, a mutex, or any other
synchronisation primitive — those belong to the application layer. This page explains which `ohal` operations are
inherently safe to call from any context, which are not, and what the application must do to make the unsafe ones safe.

## Terminology used in this document

| Term               | Meaning                                                                                   |
| ------------------ | ----------------------------------------------------------------------------------------- |
| _atomic_           | Completes in a single bus transaction; no other code can observe an intermediate state    |
| _non-atomic_       | Requires multiple bus transactions; an interrupt between them may observe a torn state    |
| _critical section_ | A region of code that must execute without interruption                                   |
| _ISR_              | Interrupt Service Routine — code that runs asynchronously in response to a hardware event |

## Which operations are atomic (safe without protection)

The following operations write to a dedicated hardware register that performs set and/or reset in a single bus
transaction. They do **not** read before writing, so no intermediate state is ever exposed to an interrupt handler
or to a second CPU core.

| Operation                           | Mechanism (STM32U083)     | Safe from ISR | Safe from thread |
| ----------------------------------- | ------------------------- | ------------- | ---------------- |
| `Pin<>::set()`                      | BSRR write (32-bit store) | ✅ Yes        | ✅ Yes           |
| `Pin<>::clear()`                    | BSRR write (32-bit store) | ✅ Yes        | ✅ Yes           |
| `Port<>::set(mask)`                 | BSRR write (32-bit store) | ✅ Yes        | ✅ Yes           |
| `Port<>::clear(mask)`               | BSRR write (32-bit store) | ✅ Yes        | ✅ Yes           |
| `Port<>::write(set_mask, clr_mask)` | BSRR write (32-bit store) | ✅ Yes        | ✅ Yes           |
| `Pin<>::read_input()`               | IDR read (32-bit load)    | ✅ Yes        | ✅ Yes           |
| `Pin<>::read_output()`              | ODR read (32-bit load)    | ✅ Yes        | ✅ Yes           |

> **Platform note:** The BSRR atomicity guarantee applies to STM32U083. On platforms that do not provide a
> hardware set/reset register, `Port<>::set()` and `Port<>::clear()` fall back to a read-modify-write on ODR and are
> **not** atomic. Consult the [GPIO API reference](gpio-api.md) platform-specific notes for your target.

## Which operations are non-atomic (require protection)

### `Pin<>::toggle()`

`toggle()` reads the current output latch (ODR) to decide whether to set or clear, then writes to BSRR:

```text
read ODR   → observe current level
               ↑ interrupt can fire here and change pin state ↑
write BSRR → drive opposite level
```

If an ISR (or another RTOS task) changes the same pin between the read and the write, `toggle()` will
re-apply a stale decision. On STM32U083, this window is just two instructions, but that is enough for a
high-priority interrupt to corrupt the result.

**This is the key safety concern for applications that share GPIO pins between main-line code and ISRs.**

### Configuration methods (`set_mode`, `set_output_type`, `set_speed`, `set_pull`)

All four configuration methods read the current register value, modify the relevant bit-field, and write the
result back. If two callers (e.g. main-line code and an ISR) configure different pins on the same port
simultaneously, the second write can silently undo the first:

```text
Caller A: read MODER → mask in new bits for PA5 → write MODER
                                              ↑ Caller B reads the same stale value,
                                                masks in bits for PA6, writes back,
                                                erasing Caller A's change ↑
```

Configuration methods are typically called once during initialisation, before any ISR is enabled. Calling them
after interrupts are enabled — especially from different contexts — requires a critical section.

## Safe patterns

### ✅ Pattern 1 — atomic set/clear (no protection needed)

Use `Pin<>::set()` and `Pin<>::clear()` for single-pin output changes that must be safe from ISR context. Each
call is a single write to BSRR with no read, so there is nothing for an interrupt to corrupt.

```cpp
using Led = ohal::gpio::Pin<ohal::gpio::PortA, 5>;

// Safe to call from main-line code and from an ISR simultaneously.
// Each is a single 32-bit BSRR store — no intermediate state.
Led::set();
Led::clear();
```

### ✅ Pattern 2 — atomic multi-pin write (no protection needed)

Use `Port<>::write()` when multiple pins must change simultaneously with no intermediate hardware state (e.g.
H-bridge direction reversal, parallel bus updates). See [Safely driving an H-bridge](h-bridge.md) for a detailed
worked example.

```cpp
using namespace ohal::gpio;

static constexpr uint16_t kForwardSet   = (1U << 4) | (1U << 7);
static constexpr uint16_t kForwardClear = (1U << 5) | (1U << 6);

// Single BSRR store — no intermediate hardware state visible to ISRs.
Port<PortA>::write(kForwardSet, kForwardClear);
```

### ✅ Pattern 3 — toggle with bare-metal interrupt masking

On bare-metal Cortex-M systems (no RTOS), save and restore the interrupt-mask register around any
read-modify-write sequence. Use the CMSIS intrinsics for this:

```cpp
using Led = ohal::gpio::Pin<ohal::gpio::PortA, 5>;

// Save current interrupt mask and disable all maskable interrupts.
uint32_t primask = __get_PRIMASK();
__disable_irq();  // CPSID I — masks all interrupts except NMI / HardFault

Led::toggle();    // read ODR, write BSRR — protected by PRIMASK

__set_PRIMASK(primask);  // restore previous state (re-enables if it was enabled before)
```

The save/restore form (`__get_PRIMASK()` / `__set_PRIMASK()`) is preferred over a simple
`__disable_irq()` / `__enable_irq()` pair because it is safe to call from nested critical sections: if
interrupts were already disabled when this code runs, `__enable_irq()` would re-enable them too early.

> **Why not always use `__disable_irq()`?** In a bare-metal application with no RTOS this works, but the
> unconditional `__enable_irq()` on exit is a latent bug if the code is ever called from an already-masked
> context (e.g. inside another critical section or from a high-priority ISR). The save/restore pattern is
> a habit worth forming from the start.

### ✅ Pattern 4 — toggle under an RTOS critical section

RTOS schedulers typically implement critical sections by raising the interrupt priority threshold
(`BASEPRI` on Cortex-M) rather than masking all interrupts. This allows high-priority ISRs (e.g. hard
real-time motor control at priority 0) to continue firing while lower-priority ones are blocked.

Always use the RTOS-provided primitives rather than calling `__disable_irq()` directly in RTOS-based code,
to preserve the scheduler's priority model:

**FreeRTOS:**

```cpp
taskENTER_CRITICAL();  // raises BASEPRI to configMAX_SYSCALL_INTERRUPT_PRIORITY
Led::toggle();
taskEXIT_CRITICAL();
```

**Zephyr:**

```cpp
int key = irq_lock();
Led::toggle();
irq_unlock(key);
```

**Generic principle:** Wrap any `toggle()` call (or any `set_mode` / `set_pull` call made after
initialisation) in a critical section whose scope is as narrow as possible — just the single `ohal`
call — to minimise interrupt latency.

### ✅ Pattern 5 — configuration during initialisation (no protection needed)

The safest approach for configuration is to complete all `set_mode()`, `set_output_type()`, `set_speed()`, and
`set_pull()` calls before enabling any interrupts or starting the RTOS scheduler. In this window no ISR
can fire, so there is nothing to race with.

```cpp
int main() {
    // All configuration happens before interrupts are enabled.
    Led::set_mode(PinMode::Output);
    Led::set_output_type(OutputType::PushPull);
    Led::set_speed(Speed::Low);
    Led::set_pull(Pull::None);
    Led::clear();  // safe initial state

    // Now enable interrupts / start scheduler.
    HAL_Init();           // vendor HAL, enables SysTick
    vTaskStartScheduler(); // FreeRTOS scheduler
}
```

## Unsafe patterns

### ❌ Unsafe — `toggle()` without a critical section (ISR + main-line conflict)

```cpp
// In main-line code:
Led::toggle();  // reads ODR → ISR fires here → writes BSRR with stale value
```

If an ISR modifies the same pin (or reads pin state to make a decision) between the ODR read and the BSRR
write inside `toggle()`, the result is a corrupted output level. The window is tiny (~2 instructions) but
not zero.

### ❌ Unsafe — unconditional `__enable_irq()` in a nested context

```cpp
// BAD: re-enables interrupts even if the caller already had them disabled.
__disable_irq();
Led::toggle();
__enable_irq();  // ← may break the outer critical section
```

Use the save/restore form (Pattern 3) instead.

### ❌ Unsafe — configuration after scheduler start without a critical section

```cpp
// In a FreeRTOS task, after vTaskStartScheduler() has been called:
Led::set_mode(PinMode::Output);   // read-modify-write on MODER, no protection
```

If another task or ISR accesses any pin on the same port's configuration registers concurrently, one of the
writes will be lost. Use `taskENTER_CRITICAL()` / `taskEXIT_CRITICAL()` or move configuration to before
`vTaskStartScheduler()`.

### ❌ Unsafe — calling `__disable_irq()` from RTOS code

```cpp
// BAD in a FreeRTOS application: bypasses the RTOS priority model.
__disable_irq();
Led::toggle();
__enable_irq();
```

`__disable_irq()` sets PRIMASK and masks _all_ interrupts, including those at priorities above
`configMAX_SYSCALL_INTERRUPT_PRIORITY`. This starves high-priority ISRs that the RTOS expects to remain
active. Use the RTOS critical section API (Pattern 4) instead.

## Decision guide

```text
Do I need to call toggle() or a configuration method after interrupts are enabled?
│
├─ No  → call freely, no protection needed
│
└─ Yes → Am I using an RTOS?
         │
         ├─ No (bare metal) → save PRIMASK, __disable_irq(), call, restore PRIMASK
         │
         └─ Yes → use RTOS critical section (taskENTER_CRITICAL, irq_lock, …)
                  Do NOT call __disable_irq() directly.
```

## Summary table

| Call site                              | `set()` / `clear()` | `toggle()`                | `set_mode()` / configuration |
| -------------------------------------- | ------------------- | ------------------------- | ---------------------------- |
| Main-line only, no ISR touches the pin | ✅ Safe             | ✅ Safe                   | ✅ Safe                      |
| Main-line + ISR touch the same pin     | ✅ Safe (BSRR)      | ⚠️ Needs critical section | ⚠️ Needs critical section    |
| Bare-metal critical section            | ✅ Safe             | ✅ Safe (PRIMASK)         | ✅ Safe (PRIMASK)            |
| RTOS critical section                  | ✅ Safe             | ✅ Safe (BASEPRI)         | ✅ Safe (BASEPRI)            |
| Two RTOS tasks, no critical section    | ✅ Safe (BSRR)      | ❌ Race condition         | ❌ Race condition            |

## Further reading

- [GPIO API reference](gpio-api.md) — per-operation atomicity notes and platform-specific details.
- [Safely driving an H-bridge](h-bridge.md) — worked example of `Port<>::write()` for atomic multi-pin changes.
- [Getting started](getting-started.md) — project setup and MCU selection.
