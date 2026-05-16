// Negative-compile test: a BitField whose Offset+Width exceeds the register width
// must be rejected at compile time.
// Expected static_assert: "ohal: BitField (Offset + Width) exceeds register width"

#include <ohal/core/access.hpp>
#include <ohal/core/field.hpp>

#include <cstdint>

namespace
{

static uint32_t storage{};

template <typename T, T* S>
struct MockReg
{
    using value_type = T;
    [[nodiscard]] static T read() noexcept { return *S; }
    static void write(T v) noexcept { *S = v; }
};

using Reg = MockReg<uint32_t, &storage>;

// Instantiate the overflowing BitField by using it in a function.
// Offset=30, Width=4 → 30+4=34 > 32: overflow fires the static_assert.
// NOLINTNEXTLINE — intentional: triggers "BitField (Offset + Width) exceeds register width"
using OvField = ohal::core::BitField<Reg, 30, 4, ohal::core::Access::ReadWrite>;

void test()
{
    OvField::write(0U); // forces template instantiation → static_assert fires
}

} // namespace
