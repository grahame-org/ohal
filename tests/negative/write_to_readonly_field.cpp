// Negative-compile test: writing to a read-only BitField must fail.
// Expected error: "cannot write to a read-only field"
#include <cstdint>

#include <ohal/core/field.hpp>

static uint32_t reg_storage{0U};

struct TestReg
{
    using value_type = uint32_t;
    static uint32_t read() noexcept { return reg_storage; }
    static void write(uint32_t v) noexcept { reg_storage = v; }
};

using RoField = ohal::core::BitField<TestReg, 0, 4, ohal::core::Access::ReadOnly>;

int main() { RoField::write(0U); }
