# Adding a new peripheral type

This guide explains how to extend ohal with a new peripheral type (for example Timer or UART).
Read [Architecture overview](architecture.md) first if you have not already.

## The pattern

Every peripheral in ohal follows three layers:

1. **Generic interface** (`include/ohal/<peripheral>.hpp`) — enumerations, capability traits, and a
   primary template that fires a `static_assert` when unspecialised.
2. **Platform register map** (`include/ohal/platforms/<family>/models/<model>/<peripheral>.hpp`) —
   address constants and partial specialisations of the primary template.
3. **Host tests** (`tests/host/test_<peripheral>_<model>.cpp`) — mock-register tests that run on
   a development host.

GPIO is the reference implementation. Study `include/ohal/gpio.hpp`,
`include/ohal/platforms/stm32u0/models/stm32u083/gpio.hpp`, and `tests/host/test_gpio.cpp`
before starting.

## Step 1 — Define the generic interface

Create `include/ohal/<peripheral>.hpp`. This file must:

- Define enumerations that represent every configuration value (mirroring the bit patterns written
  to hardware registers where possible).
- Declare a primary template whose body consists only of a `static_assert` that fires when no
  platform specialisation is available.
- Include `<cstdint>` for `uint8_t` used as the enum underlying type.

```cpp
// include/ohal/timer.hpp  (example skeleton)
#ifndef OHAL_TIMER_HPP
#define OHAL_TIMER_HPP

#include <cstdint>

namespace ohal::timer {

enum class ClockDivision : uint8_t { Div1 = 0, Div2 = 1, Div4 = 2 };

enum class CounterMode : uint8_t { Up = 0, Down = 1, CenterAligned1 = 2 };

/// Generic timer channel type. Platform headers provide partial specialisations.
/// @tparam Instance  Timer instance tag type (e.g. Tim2).
/// @tparam Channel   Zero-based channel number.
template <typename Instance, uint8_t Channel>
struct TimerChannel {
    static_assert(sizeof(Instance) == 0,
        "ohal: timer::TimerChannel is not implemented for the selected MCU. "
        "Ensure -DOHAL_FAMILY_* and -DOHAL_MODEL_* are set correctly.");
};

} // namespace ohal::timer

#endif // OHAL_TIMER_HPP
```

**Naming rules:**

- Enum underlying type: `uint8_t` (matches register field widths and keeps the API strongly typed).
- Primary template `static_assert` message: prefix with `ohal:`, name the peripheral and template,
  and suggest checking the MCU defines.

## Step 2 — Add capability traits (if needed)

If the new peripheral has optional features that vary by MCU (e.g. only some timers support DMA),
add primary traits to `include/ohal/core/capabilities.hpp`:

```cpp
// In include/ohal/core/capabilities.hpp (append to the existing namespace block)
namespace ohal::timer::capabilities {

template <typename Instance, uint8_t Channel>
struct supports_dma_burst : std::false_type {};

} // namespace ohal::timer::capabilities
```

Platform headers then specialise these to `std::true_type` where the feature is supported.

## Step 3 — Include the new header in `ohal.hpp`

Add an `#include` line to `include/ohal/ohal.hpp`:

```cpp
// include/ohal/ohal.hpp
#include <ohal/timer.hpp>
```

## Step 4 — Implement the platform register map

For each MCU model that should support the peripheral, create
`include/ohal/platforms/<family>/models/<model>/<peripheral>.hpp`.

- Define compile-time base address and offset constants.
- Use `ohal::core::Register<>` for whole-register access (atomic reads and writes).
- Use `ohal::core::BitField<>` for sub-register fields, with the appropriate `Access` policy.
- Declare fields that correspond to write-only hardware registers (e.g. event generation, status
  clear) as `ohal::core::Access::WriteOnly` so that reading them is a compile error.
- Declare fields for read-only registers (e.g. status, input data) as `ohal::core::Access::ReadOnly`
  so that writing them is a compile error.

```cpp
// include/ohal/platforms/stm32u0/models/stm32u083/timer.hpp  (excerpt)
#include "ohal/core/field.hpp"
#include "ohal/core/register.hpp"
#include "ohal/timer.hpp"

namespace ohal::timer {

struct Tim2 {};  // instance tag type

template <uint8_t Channel>
struct TimerChannel<Tim2, Channel> {
    static_assert(Channel < 4u, "ohal: STM32U083 TIM2 has channels 0-3 only.");

    // TIM2 base address (RM0503 Rev 4)
    static constexpr uintptr_t kTim2Base = 0x4000'0000U;

    // Prescaler (PSC) register — RW
    using Psc = ohal::core::Register<kTim2Base + 0x28U>;

    // Auto-reload (ARR) register — RW
    using Arr = ohal::core::Register<kTim2Base + 0x2CU>;

    // Event generation register (EGR) — WO
    using Egr = ohal::core::Register<kTim2Base + 0x14U>;

    static void set_prescaler(uint32_t psc) noexcept { Psc::write(psc); }
    static void set_autoreload(uint32_t arr) noexcept { Arr::write(arr); }
    static void generate_update_event() noexcept { Egr::write(0x01U); }
};

} // namespace ohal::timer
```

## Step 5 — Include the new file from `family.hpp`

Add the new header to the model's include list in `family.hpp`:

```cpp
// platforms/stm32u0/family.hpp  (excerpt)
#if defined(OHAL_MODEL_STM32U083)
#include "ohal/platforms/stm32u0/models/stm32u083/gpio.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083/timer.hpp"   // ← add this line
#include "ohal/platforms/stm32u0/models/stm32u083/capabilities.hpp"
#endif
```

## Step 6 — Write host-side tests

Create `tests/host/test_<peripheral>_<model>.cpp`. Use `ohal::test::MockRegister<T, &storage>` so
that tests run on a development host without hardware.

A test file must cover at minimum:

- Every public method writes or reads the correct mock register slot.
- Write-only registers are never read — use `ohal::test::ReadCountingMockRegister` and assert that
  `read_count == 0` after a write-only method call.
- Every negative-compile scenario is documented as a comment block (see `test_gpio.cpp` and
  `test_bitfield.cpp` for the established comment pattern).

Add the new file to `tests/host/CMakeLists.txt`:

```cmake
add_executable(ohal_host_tests main.cpp test_register.cpp test_bitfield.cpp
    test_gpio.cpp test_<peripheral>_<model>.cpp)
```

## Step 7 — Verify lint and build

```sh
./lint.sh
cmake -B build -DOHAL_BUILD_TESTS=ON && cmake --build build && ctest --test-dir build
```

Conventional commit title example:

```text
feat(stm32u0): add TIM2 timer peripheral for STM32U083
```

## Checklist summary

- [ ] `include/ohal/<peripheral>.hpp` created with enumerations and primary template.
- [ ] Capability traits added to `include/ohal/core/capabilities.hpp` if needed.
- [ ] `include/ohal/ohal.hpp` updated to include the new peripheral header.
- [ ] Platform register map header created for each supporting model.
- [ ] Platform model `family.hpp` updated to include the new header.
- [ ] `tests/host/test_<peripheral>_<model>.cpp` created and added to `CMakeLists.txt`.
- [ ] `./lint.sh` passes.
- [ ] Host tests pass (`ctest --test-dir build`).
- [ ] Pull request title follows conventional commit convention.
