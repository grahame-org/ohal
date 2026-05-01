#ifndef OHAL_CORE_REGISTER_HPP
#define OHAL_CORE_REGISTER_HPP

#include <cstdint>
#include <type_traits>

namespace ohal::core {

/// Models a single memory-mapped hardware register at compile-time address @p Address.
///
/// **Atomicity guarantee:** only `read()` and `write()` are exposed. Each compiles to a
/// single `volatile` bus transaction, which is the hardware-guaranteed atomic unit for
/// MMIO registers. Read-modify-write sequences (set_bits, clear_bits, modify) are
/// intentionally absent from this API because they require multiple bus transactions and
/// are therefore non-atomic with respect to interrupt handlers. Callers that need
/// read-modify-write operations must use hardware-provided atomic mechanisms (e.g.,
/// an ARM GPIO BSRR register) or arrange interrupt masking at the call site.
///
/// All operations are static; this type has no data members and occupies zero RAM.
/// The volatile pointer cast is the only use of reinterpret_cast in the codebase;
/// it is intentionally isolated here.
///
/// @tparam Address  Physical address of the register (supplied by the platform layer).
/// @tparam T        Register width type (uint32_t for most MCUs; uint8_t for 8-bit platforms).
template <uintptr_t Address, typename T = uint32_t>
struct Register {
  static_assert(std::is_unsigned_v<T> && std::is_integral_v<T>,
                "ohal: Register type parameter T must be an unsigned integer type");

  using value_type = T;
  static constexpr uintptr_t address = Address;

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  [[nodiscard]] static T read() noexcept { return *reinterpret_cast<volatile T const*>(address); }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  static void write(T value) noexcept { *reinterpret_cast<volatile T*>(address) = value; }
};

} // namespace ohal::core

#endif // OHAL_CORE_REGISTER_HPP
