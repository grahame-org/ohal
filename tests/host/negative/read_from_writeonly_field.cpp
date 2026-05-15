// Negative-compile test: reading from a WriteOnly BitField must be rejected at compile time.
// Expected static_assert: "ohal: cannot read from a write-only field"

#include <ohal/core/access.hpp>
#include <ohal/core/field.hpp>

#include <cstdint>

namespace {

static uint32_t storage{};

template <typename T, T* S>
struct MockReg {
  using value_type = T;
  [[nodiscard]] static T read() noexcept { return *S; }
  static void write(T v) noexcept { *S = v; }
};

using Reg = MockReg<uint32_t, &storage>;
using WoField = ohal::core::BitField<Reg, 0, 1, ohal::core::Access::WriteOnly>;

void test() {
  (void)WoField::read(); // NOLINT — intentional: triggers "cannot read from a write-only field"
}

} // namespace
