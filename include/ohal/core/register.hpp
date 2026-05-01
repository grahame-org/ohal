#ifndef OHAL_CORE_REGISTER_HPP
#define OHAL_CORE_REGISTER_HPP

#include <cstdint>

namespace ohal::core {

/// Models a single memory-mapped hardware register at compile-time address @p Address.
///
/// All operations are static; this type has no data members and occupies zero RAM.
/// The volatile pointer cast is the only use of reinterpret_cast in the codebase;
/// it is intentionally isolated here.
///
/// @tparam Address  Physical address of the register (supplied by the platform layer).
/// @tparam T        Register width type (uint32_t for most MCUs; uint8_t for 8-bit platforms).
template <uintptr_t Address, typename T = uint32_t>
struct Register {
  using value_type = T;
  static constexpr uintptr_t address = Address;

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  [[nodiscard]] static T read() noexcept { return *reinterpret_cast<volatile T const*>(address); }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  static void write(T value) noexcept { *reinterpret_cast<volatile T*>(address) = value; }

  static void set_bits(T mask) noexcept { write(read() | mask); }

  static void clear_bits(T mask) noexcept { write(read() & static_cast<T>(~mask)); }

  static void modify(T clear_mask, T set_mask) noexcept {
    write((read() & static_cast<T>(~clear_mask)) | set_mask);
  }
};

} // namespace ohal::core

#endif // OHAL_CORE_REGISTER_HPP
