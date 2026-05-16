// Negative-compile test: writing to a ReadOnly BitField must be rejected at compile time.
// Expected static_assert: "ohal: cannot write to a read-only field"

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
using RoField = ohal::core::BitField<Reg, 0, 1, ohal::core::Access::ReadOnly>;

void test()
{
    RoField::write(1U); // NOLINT — intentional: triggers "cannot write to a read-only field"
}

} // namespace
