# Adding a new MCU family or model

This guide explains how to extend ohal with support for a new MCU family, or a new model within
an existing family. Read [Architecture overview](architecture.md) first if you have not already.

## Before you write any code

Gather the following information from the official datasheet or technical reference manual for the
target MCU. Implementation that relies on guessed or approximated register data will silently
produce incorrect hardware behaviour.

**For every new family:**

- Full list of supported models (for `family.hpp` validation).
- Base addresses for all GPIO ports.
- Offset and width of every GPIO configuration register (MODER, OTYPER, OSPEEDR, PUPDR, or
  equivalent).
- Access type for each register: ReadOnly, WriteOnly, or ReadWrite.
- Whether the MCU has a hardware atomic set/clear register (e.g. BSRR on STM32, OUTSET/OUTCLR on
  nRF52).
- Which GPIO features are absent on this family (affects `capabilities.hpp` specialisations).
- Toolchain required for cross-compilation.

**For a new model within an existing family:**

- Differences in GPIO port count or base addresses compared to the existing model in the family.
- Any registers that differ in layout.

## Adding a new MCU family

### Step 1 — Create the family header

Create `include/ohal/platforms/<family>/family.hpp`. This header must:

1. Error if no model define for this family is present.
2. Error if more than one model define is present.
3. Include the model headers for each supported model.

```cpp
// include/ohal/platforms/nrf5/family.hpp
#ifndef OHAL_PLATFORMS_NRF5_FAMILY_HPP
#define OHAL_PLATFORMS_NRF5_FAMILY_HPP

#if !defined(OHAL_MODEL_NRF52840)
#error "ohal: No nRF5 model defined. Pass -DOHAL_MODEL_NRF52840 to the compiler."
#endif

#if defined(OHAL_MODEL_NRF52840)
#include "ohal/platforms/nrf5/models/nrf52840/gpio.hpp"
#include "ohal/platforms/nrf5/models/nrf52840/capabilities.hpp"
#endif

#endif // OHAL_PLATFORMS_NRF5_FAMILY_HPP
```

### Step 2 — Create the register map header

Create `include/ohal/platforms/<family>/models/<model>/gpio.hpp`. This header:

- Defines register address constants (base addresses + offsets).
- Optionally defines a `GpioPortRegs<Base>` struct that collects all register types for one port.
- Provides a partial specialisation of `ohal::gpio::Pin<Port, PinNum>` for each supported port.

Each method in the specialisation must use `ohal::core::BitField<>` or `ohal::core::Register<>`
directly — no raw pointer casts, no inline assembly.

```cpp
// include/ohal/platforms/nrf5/models/nrf52840/gpio.hpp (skeleton)
#ifndef OHAL_PLATFORMS_NRF5_MODELS_NRF52840_GPIO_HPP
#define OHAL_PLATFORMS_NRF5_MODELS_NRF52840_GPIO_HPP

#include <cstdint>
#include "ohal/core/field.hpp"
#include "ohal/core/register.hpp"
#include "ohal/gpio.hpp"
#include "ohal/platforms/nrf5/models/nrf52840/capabilities.hpp"

namespace ohal::gpio {

inline constexpr uintptr_t kNrf52840P0Base = 0x5000'0000U;
// … define offsets for OUT, OUTSET, OUTCLR, IN, DIR, DIRSET, DIRCLR …

template <uint8_t PinNum>
struct Pin<PortA, PinNum> {
    static_assert(PinNum < 32u, "ohal: nRF52840 P0 has pins 0-31 only.");

    // OUTSET: write-only register, bit N sets the output high atomically.
    using OutSet = ohal::core::BitField<
        ohal::core::Register<kNrf52840P0Base + kOutsetOffset>,
        PinNum, 1u, ohal::core::Access::WriteOnly>;

    // OUTCLR: write-only register, bit N drives the output low atomically.
    using OutClr = ohal::core::BitField<
        ohal::core::Register<kNrf52840P0Base + kOutclrOffset>,
        PinNum, 1u, ohal::core::Access::WriteOnly>;

    static void set()   noexcept { OutSet::write(1u); }
    static void clear() noexcept { OutClr::write(1u); }

    // … implement set_mode, read_input, toggle, etc. …
};

} // namespace ohal::gpio

#endif // OHAL_PLATFORMS_NRF5_MODELS_NRF52840_GPIO_HPP
```

#### Handling unsupported features

If the MCU does not support a feature (e.g. no configurable output speed), declare the method and
add a `static_assert` that references the capability trait. This produces a clear compile error
message when application code calls the method:

```cpp
static void set_speed(Speed) noexcept {
    static_assert(capabilities::supports_output_speed<PortA, PinNum>::value,
        "ohal: nRF52840 GPIO does not support configurable output speed.");
}
```

### Step 3 — Create the capability traits header

Create `include/ohal/platforms/<family>/models/<model>/capabilities.hpp`. Specialise the traits
from `ohal/core/capabilities.hpp` to `std::true_type` for features that this model supports. Do
not add specialisations for unsupported features — the default `false_type` handles those.

```cpp
// include/ohal/platforms/nrf5/models/nrf52840/capabilities.hpp
#ifndef OHAL_PLATFORMS_NRF5_MODELS_NRF52840_CAPABILITIES_HPP
#define OHAL_PLATFORMS_NRF5_MODELS_NRF52840_CAPABILITIES_HPP

#include <type_traits>
#include "ohal/core/capabilities.hpp"

namespace ohal::gpio::capabilities {

// nRF52840 GPIO supports pull configuration on every port/pin.
template <typename Port, uint8_t PinNum>
struct supports_pull<Port, PinNum> : std::true_type {};

// nRF52840 GPIO does not support configurable output type or speed;
// the default false_type primary templates already handle those.

} // namespace ohal::gpio::capabilities

#endif // OHAL_PLATFORMS_NRF5_MODELS_NRF52840_CAPABILITIES_HPP
```

### Step 4 — Register the family in `platform.hpp`

Add the new family to the `#if … #elif` chain in `include/ohal/platform.hpp`:

```cpp
// include/ohal/platform.hpp  (excerpt — add the new elif block)
#elif defined(OHAL_FAMILY_NRF5)
#include "ohal/platforms/nrf5/family.hpp"
```

Also add `OHAL_FAMILY_NRF5` to the top-level error message so users see it when no family define
is passed:

```cpp
#error "ohal: No MCU family defined. " \
       "Pass exactly one of -DOHAL_FAMILY_STM32U0, -DOHAL_FAMILY_TI_MSPM0, " \
       "-DOHAL_FAMILY_PIC, -DOHAL_FAMILY_NRF5 to the compiler."
```

### Step 5 — Write host-side tests

Create `tests/host/test_gpio_<model>.cpp`. Use `ohal::test::MockRegister<T, &storage>` as the
backing store so that tests run on a development host without hardware.

A complete test file must cover at minimum:

- `Pin<PortA, 0>::set()` writes the correct bit pattern to the correct mock slot.
- `Pin<PortA, 0>::clear()` writes the correct bit pattern.
- `Pin<PortA, 0>::set_mode(PinMode::Output)` writes the correct value to the direction register.
- `Pin<PortA, 0>::set_mode(PinMode::Input)` writes the correct value.
- `Pin<PortA, 0>::read_input()` returns `Level::High` when the mock input register bit is set.
- For every unsupported feature: a manual negative-compile note confirming the `static_assert`
  message, documented as a comment (see `tests/host/test_gpio.cpp` for the comment pattern).

Add the new test file to `tests/host/CMakeLists.txt`:

```cmake
add_executable(ohal_host_tests main.cpp test_register.cpp test_bitfield.cpp
    test_gpio.cpp test_gpio_<model>.cpp)
```

### Step 6 — Verify lint and build

Run the repository lint script and the host test suite before opening a pull request:

```sh
./lint.sh
cmake -B build -DOHAL_BUILD_TESTS=ON && cmake --build build && ctest --test-dir build
```

The pull request title must follow the conventional commit convention, e.g.:

```text
feat(nrf5): add nRF52840 GPIO partial specialisations
```

## Adding a new model to an existing family

1. Gather the register map differences between the new model and the existing model from the
   datasheet.
2. If the register layout is identical to an existing model (common within STM32 sub-families),
   create a thin `models/<new_model>/gpio.hpp` that includes the shared register definitions from
   the existing model and adjusts only the differing addresses.
3. Add the new model to `family.hpp`'s model validation list.
4. Update the `#error` message in `family.hpp` if the supported model list has changed.
5. Add or extend host tests to cover the new model's specific addresses or pin counts.
6. Conventional commit title example: `feat(stm32u0): add STM32U073 model support`.

## Checklist summary

- [ ] Register map sourced from official datasheet; all addresses verified.
- [ ] `include/ohal/platforms/<family>/family.hpp` created with model validation.
- [ ] `include/ohal/platforms/<family>/models/<model>/gpio.hpp` created with `Pin<>` specialisations.
- [ ] `include/ohal/platforms/<family>/models/<model>/capabilities.hpp` created.
- [ ] `include/ohal/platform.hpp` updated with new `#elif` branch and error message.
- [ ] `tests/host/test_gpio_<model>.cpp` created and added to `CMakeLists.txt`.
- [ ] `./lint.sh` passes.
- [ ] Host tests pass (`ctest --test-dir build`).
- [ ] Pull request title follows conventional commit convention.
