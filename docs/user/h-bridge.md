# Safely driving an H-bridge

An H-bridge is a common circuit for driving a DC motor in both directions. It places four
switches — or their transistor equivalents — around a load, forming an "H" shape. Driving it
safely from firmware requires care: certain intermediate pin states cause a _shoot-through_ fault
that shorts the supply rail directly to ground.

This guide explains the hazard, shows why naive sequential `Pin<>` calls are insufficient for
some transitions, and demonstrates the `Port<>::write()`-based atomic pattern that eliminates
the problem.

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

## Safe pattern 2: atomic port write (same port)

`ohal::gpio::Port<PortTag>::write(set_mask, clear_mask)` sets and resets multiple pins in a
single bus transaction:

- **`set_mask`** — every bit _n_ set here drives pin _n_ high.
- **`clear_mask`** — every bit _n_ set here drives pin _n_ low.

Both actions happen simultaneously on hardware that provides an atomic set/reset register
(e.g. the STM32 BSRR): one `str` instruction, no intermediate state at the hardware level. On
platforms without such a register the implementation performs two writes.

```cpp
// Compile with: -DOHAL_FAMILY_STM32U0 -DOHAL_MODEL_STM32U083 -std=c++17
#include <ohal/ohal.hpp>

using namespace ohal::gpio;

// H-bridge pins — all on Port A for single-write atomic access
using AHi = Pin<PortA, 4>;
using ALo = Pin<PortA, 5>;
using BHi = Pin<PortA, 6>;
using BLo = Pin<PortA, 7>;

// Encode each H-bridge state as a pair of (set, clear) masks computed at
// compile time.  Port<PortA>::write() compiles to a single BSRR store on
// STM32 — no intermediate hardware state is ever visible.

static constexpr uint16_t kForwardSet   = (1U << 4) | (1U << 7); // AHi=1, BLo=1
static constexpr uint16_t kForwardClear = (1U << 5) | (1U << 6); // ALo=0, BHi=0

static constexpr uint16_t kReverseSet   = (1U << 5) | (1U << 6); // ALo=1, BHi=1
static constexpr uint16_t kReverseClear = (1U << 4) | (1U << 7); // AHi=0, BLo=0

static constexpr uint16_t kCoastClear   = (1U << 4) | (1U << 5) | (1U << 6) | (1U << 7);
static constexpr uint16_t kBrakeSet     = (1U << 5) | (1U << 7); // ALo=1, BLo=1
static constexpr uint16_t kBrakeClear   = (1U << 4) | (1U << 6); // AHi=0, BHi=0

void drive_forward() { Port<PortA>::write(kForwardSet,  kForwardClear); }
void drive_reverse() { Port<PortA>::write(kReverseSet,  kReverseClear); }
void coast()         { Port<PortA>::write(0,            kCoastClear);   }
void brake()         { Port<PortA>::write(kBrakeSet,    kBrakeClear);   }
```

All four state constants are computed at compile time. Each function call compiles to a single
`str` instruction that loads the constant and writes it to the hardware register address —
exactly the same cost as a hand-written register access.

### Initialising the pins

Configure the pins once before the first `Port<>` write. Use the standard `Pin<>` configuration
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

`Port<>::write()` can only update pins that belong to the same GPIO port. If your H-bridge
control pins span two ports — for example AHi/ALo on Port A and BHi/BLo on Port B — a single
`Port<>::write()` call cannot cover all four pins.

In that situation, use Pattern 1 (deactivate before activating) and keep the Coast state as your
safe intermediate step:

```cpp
// Forward → Reverse, pins on two ports
void drive_reverse_from_forward() {
    // Step 1: deactivate both active outputs atomically within each port
    Port<PortA>::clear(1U << 4);  // AHi=0
    Port<PortB>::clear(1U << 7);  // BLo=0
    // Brief Coast state — both ports are now all-off

    // Step 2: activate the new direction
    Port<PortA>::set(1U << 5);    // ALo=1
    Port<PortB>::set(1U << 6);    // BHi=1
}
```

The intermediate state here is a partial Coast (AHi and BLo off, ALo and BHi still off), which
is safe. The two deactivation writes are still separate transactions, but they deactivate — so
no shoot-through window is possible between them.

**Design recommendation:** where the PCB layout permits, assign all four H-bridge control pins
to a single GPIO port. This allows the single-BSRR-write pattern and makes the code
self-documenting: every state transition is a single named constant.

## Summary

| Pattern                     | Intermediate state  | Works across ports | Notes                                              |
| --------------------------- | ------------------- | ------------------ | -------------------------------------------------- |
| Sequential `Pin::set/clear` | Yes — visible       | Yes                | Order must be carefully audited per bridge         |
| Deactivate-first sequencing | Yes — safe (Coast)  | Yes                | Always correct; trivial to audit                   |
| `Port::write(set, clear)`   | No (atomic hw only) | Same port only     | Most robust on STM32; compile-time state constants |

Use `Port<PortTag>::write()` whenever you need to update multiple pins as one indivisible
hardware action. On platforms with a dedicated atomic set/reset register (e.g. STM32 BSRR) it
compiles to a single `str` instruction. The same principle applies to any peripheral where a
single write-only register controls several outputs simultaneously.
