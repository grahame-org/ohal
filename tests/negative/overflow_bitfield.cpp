// Negative-compile test: a BitField whose (Offset + Width) exceeds the
// register width must fail.
// Expected error: "BitField (Offset + Width) exceeds register width"
#include <cstdint>

#include <ohal/core/field.hpp>

static uint32_t reg_storage{0U};

struct TestReg {
  using value_type = uint32_t;
  static uint32_t read() noexcept { return reg_storage; }
  static void write(uint32_t v) noexcept { reg_storage = v; }
};

// Offset=30, Width=4 → 30+4=34 > 32: overflow.
using OverflowField = ohal::core::BitField<TestReg, 30, 4, ohal::core::Access::ReadWrite>;

int main() { (void)OverflowField::mask; }
