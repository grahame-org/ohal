# Safely driving an H-bridge

An H-bridge is a common circuit for driving a DC motor in both directions. It places four
switches — or their transistor equivalents — around a load, forming an "H" shape. Driving it
safely from firmware requires care: certain intermediate pin states cause a _shoot-through_ fault
that shorts the supply rail directly to ground.

This guide explains the hazard, shows why naive sequential `Pin<>` calls are insufficient for
some transitions, and demonstrates the BSRR-based atomic pattern that eliminates the problem on
STM32 targets.

## The shoot-through hazard

A half-bridge has two switches on the same rail leg:

```text
VCC
 |
[S_HI]  ← high-side switch
 |
 +——→ Motor terminal
 |
[S_LO]  ← low-side switch
 |
GND
```

If S_HI and S_LO are both closed at the same time, VCC is shorted to GND through the switch
resistance. Even a microsecond-wide pulse can destroy transistors or trip protection circuits.

A full H-bridge has two such legs (leg A and leg B). To drive the motor forward, leg A has its
high-side on and leg B has its low-side on:

| State   | A_HI | A_LO | B_HI | B_LO |
| ------- | ---- | ---- | ---- | ---- |
| Forward | 1    | 0    | 0    | 1    |
| Reverse | 0    | 1    | 1    | 0    |
| Coast   | 0    | 0    | 0    | 0    |
| Brake   | 0    | 1    | 0    | 1    |

The forbidden states are `A_HI=1, A_LO=1` (shoot-through on leg A) and `B_HI=1, B_LO=1`
(shoot-through on leg B). Transitioning between Forward and Reverse must never pass through
either of those states, even for a single instruction.

## Why sequential Pin calls are not enough

`Pin<>::set()` and `Pin<>::clear()` each compile to a single volatile bus write — one `str`
instruction on ARM Cortex-M. They are atomic in the sense that neither performs a
read-modify-write. However, two sequential calls are _two separate bus transactions_ with a
visible hardware state between them.

Consider a naive direction reversal on four pins mapped to PA4–PA7:

```cpp
using AHi = Pin<PortA, 4>;
using ALo = Pin<PortA, 5>;
using BHi = Pin<PortA, 6>;
using BLo = Pin<PortA, 7>;

// Currently Forward: AHi=1, ALo=0, BHi=0, BLo=1
// Attempting to switch to Reverse (UNSAFE ordering):
AHi::clear();  // PA4=0 — OK
ALo::set();    // PA5=1 — OK, A leg safe
BHi::set();    // PA6=1 — OK
BLo::clear();  // PA7=0 — OK, done
```

This particular call order is actually safe for this bridge because each leg is updated before
the next is touched. But the ordering is subtle and fragile: swap any two lines and a
shoot-through window appears. With more complex state machines, control flow branches, or
interrupt handlers that read pin state, reasoning about every ordering becomes unreliable.

## Safe pattern 1: deactivate before activating (careful sequencing)

When the H-bridge topology guarantees a safe intermediate state, you can sequence calls safely
by always turning off the potentially hazardous gate before activating its pair:

```cpp
// Forward → Reverse via the Coast intermediate state
// Step 1: drive both legs to coast (all outputs off)
AHi::clear();
BLo::clear();
// Intermediate: AHi=0, ALo=0, BHi=0, BLo=0 — Coast, no shoot-through possible

// Step 2: activate the new direction
ALo::set();
BHi::set();
// Final: AHi=0, ALo=1, BHi=1, BLo=0 — Reverse
```

This is always safe because the intermediate state (Coast) is a defined, harmless operating
point. The two steps _are_ visible to the hardware as distinct states, but neither is a
shoot-through condition.

Use this pattern when:

- The H-bridge driver tolerates a brief Coast state (nearly all designs do).
- The bridge pins span more than one GPIO port (see [Multi-port bridges](#multi-port-bridges)).
- You want code that is easy to audit without knowing anything about BSRR.

## Safe pattern 2: atomic BSRR write (same port, STM32)

The STM32 GPIO BSRR register is a write-only, 32-bit register that sets and resets pins in a
single bus transaction:

- **Bits 15:0** — set bits. Writing `1` to bit _n_ drives pin _n_ high.
- **Bits 31:16** — reset bits. Writing `1` to bit _n+16_ drives pin _n_ low.

Both actions happen simultaneously: one `str` instruction, no intermediate state at the
hardware level.

`ohal` exposes BSRR through the platform register map in the
`ohal::platforms::stm32u0::stm32u083` namespace. It is included automatically when
`<ohal/ohal.hpp>` is included with the STM32U083 defines.

```cpp
// Compile with: -DOHAL_FAMILY_STM32U0 -DOHAL_MODEL_STM32U083 -std=c++17
#include <ohal/ohal.hpp>

using namespace ohal::gpio;
namespace regs = ohal::platforms::stm32u0::stm32u083;

// H-bridge pins — all on Port A for single-write BSRR access
using AHi = Pin<PortA, 4>;
using ALo = Pin<PortA, 5>;
using BHi = Pin<PortA, 6>;
using BLo = Pin<PortA, 7>;

// BSRR bit helpers
static constexpr uint32_t set_bit(uint8_t pin)   { return 1U << pin; }
static constexpr uint32_t reset_bit(uint8_t pin) { return 1U << (pin + 16U); }

// Encode each H-bridge state as a single BSRR value computed at compile time.
// Each write atomically sets and clears the required pins — no intermediate state exists.

static constexpr uint32_t kForward =
    set_bit(4) | reset_bit(5) | reset_bit(6) | set_bit(7);  // AHi=1, ALo=0, BHi=0, BLo=1

static constexpr uint32_t kReverse =
    reset_bit(4) | set_bit(5) | set_bit(6) | reset_bit(7);  // AHi=0, ALo=1, BHi=1, BLo=0

static constexpr uint32_t kCoast =
    reset_bit(4) | reset_bit(5) | reset_bit(6) | reset_bit(7);  // all off

static constexpr uint32_t kBrake =
    reset_bit(4) | set_bit(5) | reset_bit(6) | set_bit(7);  // ALo=1, BLo=1 (low-side brake)

void drive_forward() { regs::GpioA::Bsrr::write(kForward); }
void drive_reverse() { regs::GpioA::Bsrr::write(kReverse); }
void coast()         { regs::GpioA::Bsrr::write(kCoast); }
void brake()         { regs::GpioA::Bsrr::write(kBrake); }
```

All four state constants are computed at compile time. Each function call compiles to a single
`str` instruction that loads the constant and writes it to the BSRR address — exactly the same
cost as a hand-written register access.

### Initialising the pins

Configure the pins once before the first BSRR write. Use the standard `Pin<>` configuration
API; pin mode and output-type configuration do not need to be atomic with respect to each other:

```cpp
void hbridge_init() {
    AHi::set_mode(PinMode::Output);
    ALo::set_mode(PinMode::Output);
    BHi::set_mode(PinMode::Output);
    BLo::set_mode(PinMode::Output);

    AHi::set_output_type(OutputType::PushPull);
    ALo::set_output_type(OutputType::PushPull);
    BHi::set_output_type(OutputType::PushPull);
    BLo::set_output_type(OutputType::PushPull);

    // Start in a safe (Coast) state before enabling the gate driver power supply
    coast();
}
```

## Multi-port bridges

BSRR can only update pins that belong to the same GPIO port. If your H-bridge control pins span
two ports — for example AHi/ALo on Port A and BHi/BLo on Port B — a single BSRR write cannot
cover all four pins.

In that situation, use Pattern 1 (deactivate before activating) and keep the Coast state as your
safe intermediate step:

```cpp
// Forward → Reverse, pins on two ports
void drive_reverse_from_forward() {
    // Step 1: deactivate both active outputs atomically within each port
    regs::GpioA::Bsrr::write(reset_bit(4));  // AHi=0
    regs::GpioB::Bsrr::write(reset_bit(7));  // BLo=0
    // Brief Coast state — both ports are now all-off

    // Step 2: activate the new direction
    regs::GpioA::Bsrr::write(set_bit(5));    // ALo=1
    regs::GpioB::Bsrr::write(set_bit(6));    // BHi=1
}
```

The intermediate state here is a partial Coast (AHi and BLo off, ALo and BHi still off), which
is safe. The two deactivation writes are still separate transactions, but they deactivate — so
no shoot-through window is possible between them.

**Design recommendation:** where the PCB layout permits, assign all four H-bridge control pins
to a single GPIO port. This allows the single-BSRR-write pattern and makes the code
self-documenting: every state transition is a single named constant.

## Summary

| Pattern                           | Intermediate state | Works across ports | Notes                                      |
| --------------------------------- | ------------------ | ------------------ | ------------------------------------------ |
| Sequential `Pin::set/clear`       | Yes — visible      | Yes                | Order must be carefully audited per bridge |
| Deactivate-first sequencing       | Yes — safe (Coast) | Yes                | Always correct; trivial to audit           |
| Single BSRR write (`Bsrr::write`) | No                 | Same port only     | Most robust; compile-time state constants  |

Use `Register<>::write()` — and the BSRR register it wraps — whenever you need to update
multiple pins as one indivisible hardware action. The same principle applies to any peripheral
where a single write-only register controls several outputs simultaneously.
