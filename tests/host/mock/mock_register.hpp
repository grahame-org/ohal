#ifndef OHAL_TESTS_MOCK_MOCK_REGISTER_HPP
#define OHAL_TESTS_MOCK_MOCK_REGISTER_HPP

#include <cstdint>

namespace ohal::core {

/// Test double for Register<Addr, T>.
///
/// Uses a compile-time pointer to a static variable instead of a raw hardware address,
/// making it safe to instantiate in host-side unit tests without touching arbitrary memory.
/// The interface and logic are identical to Register<Addr, T>.
///
/// @tparam T        Register width type.
/// @tparam Storage  Pointer to a static variable that acts as the register's backing store.
template <typename T, T* Storage>
struct MockRegister {
  using value_type = T;

  [[nodiscard]] static T read() noexcept { return *Storage; }

  static void write(T value) noexcept { *Storage = value; }

  static void set_bits(T mask) noexcept { write(read() | mask); }

  static void clear_bits(T mask) noexcept { write(read() & static_cast<T>(~mask)); }

  static void modify(T clear_mask, T set_mask) noexcept {
    write((read() & static_cast<T>(~clear_mask)) | set_mask);
  }
};

} // namespace ohal::core

#endif // OHAL_TESTS_MOCK_MOCK_REGISTER_HPP
