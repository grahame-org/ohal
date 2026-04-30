# Step 3 – Core Register Abstraction Layer

**Goal:** A header-only `Register<Addr, T>` type that models a single hardware register.

**Inputs required:** None — addresses are supplied by the platform layer (Step 5 onward).

## Key Design Decisions

- The type is a struct template with **no data members**. All methods are `static`.
- The volatile pointer cast is the only place in the codebase that touches `reinterpret_cast`;
  it is wrapped in this single header.
- `T` defaults to `uint32_t`; 8-bit MCUs (e.g. PIC18) use `uint8_t`, 16-bit MCUs use `uint16_t`.

## Interface

```cpp
// include/ohal/core/register.hpp
#ifndef OHAL_CORE_REGISTER_HPP
#define OHAL_CORE_REGISTER_HPP

#include <cstdint>

namespace ohal::core {

template <uintptr_t Address, typename T = uint32_t>
struct Register {
    using value_type = T;
    static constexpr uintptr_t address = Address;

    static T read() noexcept {
        return *reinterpret_cast<volatile T const*>(address);
    }

    static void write(T value) noexcept {
        *reinterpret_cast<volatile T*>(address) = value;
    }

    static void set_bits(T mask) noexcept {
        write(read() | mask);
    }

    static void clear_bits(T mask) noexcept {
        write(read() & static_cast<T>(~mask));
    }

    static void modify(T clear_mask, T set_mask) noexcept {
        write((read() & static_cast<T>(~clear_mask)) | set_mask);
    }
};

} // namespace ohal::core

#endif // OHAL_CORE_REGISTER_HPP
```

## Tests to Write (host, Step 10)

- `Register::write` stores the value at the correct address (use mock memory array as the
  "register").
- `Register::read` returns the stored value.
- `Register::set_bits` ORs correctly without disturbing other bits.
- `Register::clear_bits` ANDs correctly.
- `Register::modify` applies clear then set in a single logical step.
- All of the above repeated with `T = uint8_t` to cover 8-bit platforms.
