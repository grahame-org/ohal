#ifndef OHAL_CORE_FIELD_HPP
#define OHAL_CORE_FIELD_HPP

#include <climits>
#include <cstdint>
#include <type_traits>

#include "ohal/core/access.hpp"

namespace ohal::core {

/// Compile-time descriptor for a contiguous bit field within a hardware register.
///
/// @tparam Reg        The register type that backs this field (e.g. Register<Addr, T> or
///                    MockRegister<T, Storage>). Must expose `value_type`, `read()`, and
///                    `write(value_type)`.
/// @tparam Offset     Bit offset of the field's LSB within the register (0-based).
/// @tparam Width      Number of bits in the field. Must be at least 1.
/// @tparam Acc        Access policy (ReadOnly, WriteOnly, or ReadWrite).
/// @tparam ValueType  Type returned by read() and accepted by write(). Defaults to the
///                    register's raw integer type. Pass an enum type to get typed read/write.
template <typename Reg, uint8_t Offset, uint8_t Width, Access Acc,
          typename ValueType = typename Reg::value_type>
struct BitField {
  using reg_type = Reg;
  using value_type = ValueType;
  using raw_type = typename Reg::value_type;

  static constexpr uint8_t offset = Offset;
  static constexpr uint8_t width = Width;
  static constexpr Access access = Acc;

  // Compute the bit mask using uintmax_t arithmetic to avoid shift-count overflow
  // for any supported register width (8, 16, 32-bit). The static_assert below
  // guarantees Width <= sizeof(raw_type)*8 <= 32 in practice.
  static constexpr raw_type mask =
      static_cast<raw_type>(((uintmax_t{1} << Width) - uintmax_t{1}) << Offset);

  static_assert(Width > 0, "ohal: BitField width must be at least 1");
  static_assert(static_cast<unsigned>(Offset) + static_cast<unsigned>(Width) <=
                    sizeof(raw_type) * static_cast<unsigned>(CHAR_BIT),
                "ohal: BitField (Offset + Width) exceeds register width");

  /// Read the field from the register and return it as @p ValueType.
  ///
  /// @note Triggers a compile error if @p Acc is WriteOnly.
  [[nodiscard]] static ValueType read() noexcept {
    static_assert(Acc != Access::WriteOnly, "ohal: cannot read from a write-only field");
    return static_cast<ValueType>((Reg::read() & mask) >> Offset);
  }

  /// Write @p value into the field, preserving all other bits in the register.
  ///
  /// @note Triggers a compile error if @p Acc is ReadOnly.
  /// @note This is a non-atomic read-modify-write sequence. Callers that require
  ///       atomicity must arrange interrupt masking or use hardware-provided atomic
  ///       mechanisms (e.g. ARM BSRR) at the call site.
  static void write(ValueType value) noexcept {
    static_assert(Acc != Access::ReadOnly, "ohal: cannot write to a read-only field");
    raw_type const current = Reg::read();
    Reg::write(static_cast<raw_type>((current & ~mask) |
                                     ((static_cast<raw_type>(value) << Offset) & mask)));
  }
};

} // namespace ohal::core

#endif // OHAL_CORE_FIELD_HPP
