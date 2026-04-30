# Step 5 – Bit Field and Access-Control Abstractions

**Goal:** `BitField<Reg, Offset, Width, Access>` — a compile-time descriptor for a field within a
register, with enforced read/write access control.

**Inputs required:** None.

## Key Design Decisions

- Access control is checked by `static_assert` at the call site — attempting to `read()` a
  write-only field or `write()` a read-only field causes a compile error with a descriptive
  message.
- `Offset + Width` is bounds-checked against `sizeof(T) * 8` at instantiation time.
- Fields returning enum values use a second template parameter `ValueType` so that callers get
  back the enum type rather than a raw integer.

## Interface

```cpp
// include/ohal/core/field.hpp
#ifndef OHAL_CORE_FIELD_HPP
#define OHAL_CORE_FIELD_HPP

#include <cstdint>
#include "ohal/core/access.hpp"

namespace ohal::core {

template <
    typename Reg,
    uint8_t  Offset,
    uint8_t  Width,
    Access   Acc,
    typename ValueType = typename Reg::value_type
>
struct BitField {
    using reg_type   = Reg;
    using value_type = ValueType;
    using raw_type   = typename Reg::value_type;

    static constexpr uint8_t  offset = Offset;
    static constexpr uint8_t  width  = Width;
    static constexpr Access   access = Acc;
    static constexpr raw_type mask   =
        static_cast<raw_type>(((raw_type{1} << Width) - raw_type{1}) << Offset);

    static_assert(Width > 0,
        "ohal: BitField width must be at least 1");
    static_assert(static_cast<unsigned>(Offset) + static_cast<unsigned>(Width)
                      <= sizeof(raw_type) * 8u,
        "ohal: BitField (Offset + Width) exceeds register width");

    static ValueType read() noexcept {
        static_assert(Acc != Access::WriteOnly,
            "ohal: cannot read from a write-only field");
        return static_cast<ValueType>((Reg::read() & mask) >> Offset);
    }

    static void write(ValueType value) noexcept {
        static_assert(Acc != Access::ReadOnly,
            "ohal: cannot write to a read-only field");
        Reg::modify(mask, (static_cast<raw_type>(value) << Offset) & mask);
    }
};

} // namespace ohal::core

#endif // OHAL_CORE_FIELD_HPP
```

```cpp
// include/ohal/core/access.hpp
#ifndef OHAL_CORE_ACCESS_HPP
#define OHAL_CORE_ACCESS_HPP

#include <cstdint>

namespace ohal::core {

enum class Access : uint8_t {
    ReadOnly  = 0,
    WriteOnly = 1,
    ReadWrite = 2,
};

} // namespace ohal::core

#endif // OHAL_CORE_ACCESS_HPP
```

## Tests to Write (host, Step 11)

- Writing to a `ReadWrite` field updates only the correct bits.
- Reading from a `ReadWrite` field returns the correct extracted value.
- Static assertion fires when `Offset + Width` overflows (verified by negative-compile test).
- `WriteOnly` field's `read()` produces the compiler error `cannot read from a write-only field`
  (negative-compile test).
- `ReadOnly` field's `write()` produces the compiler error `cannot write to a read-only field`
  (negative-compile test).
