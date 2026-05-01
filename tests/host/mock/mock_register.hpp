#ifndef OHAL_TESTS_HOST_MOCK_MOCK_REGISTER_HPP
#define OHAL_TESTS_HOST_MOCK_MOCK_REGISTER_HPP

#include <array>
#include <cstdint>

namespace ohal::test {

/// 256 bytes of simulated register space (64 × 32-bit slots, or 256 × 8-bit slots).
/// Used by host-side tests that redirect Register<> accesses into mock memory
/// via build-system address defines (see step-11-unit-testing.md §11.1).
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
inline std::array<uint32_t, 64> mock_memory{};

/// Resets the entire mock memory region to zero. Call in test SetUp().
inline void reset_mock() noexcept { mock_memory.fill(0); }

/// Returns the uintptr_t address of the 32-bit slot at index @p slot in mock_memory.
///
/// @note Pointer-to-integer conversion is implementation-defined (C++ [expr.reinterpret.cast]).
///       This is acceptable for host-side test code on a hosted C++ implementation where the
///       goal is to redirect MMIO register accesses into a plain memory array.
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
inline uintptr_t mock_addr(std::size_t slot) noexcept {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<uintptr_t>(mock_memory.data()) + slot * sizeof(uint32_t);
}

/// Returns the uintptr_t address of the 8-bit slot at index @p slot in mock_memory.
/// Used for 8-bit platform tests (e.g. PIC18F4550).
inline uintptr_t mock_addr8(std::size_t slot) noexcept {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<uintptr_t>(reinterpret_cast<uint8_t*>(mock_memory.data()) + slot);
}

/// Test double for ohal::core::Register<Addr, T>.
///
/// Uses a static pointer (T* Storage) as the backing store instead of a raw uintptr_t
/// address. This is the correct C++17 host-testing approach because reinterpret_cast
/// from pointer to uintptr_t is not a constant expression (C++17 [expr.const]), so
/// mock_addr() cannot supply a Register<> template argument at compile time.
/// MockRegister exposes the same interface as Register<>: only the atomic read() and
/// write() operations.
///
/// @tparam T        Register width type.
/// @tparam Storage  Pointer to a variable with static storage duration that acts as the
///                  register's backing store.
template <typename T, T* Storage>
struct MockRegister {
  using value_type = T;

  [[nodiscard]] static T read() noexcept { return *Storage; }

  static void write(T value) noexcept { *Storage = value; }
};

} // namespace ohal::test

#endif // OHAL_TESTS_HOST_MOCK_MOCK_REGISTER_HPP
