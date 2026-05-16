#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_NVIC_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_NVIC_HPP

#include <cstdint>
#include <type_traits>

#include "ohal/core/register.hpp"
#include "ohal/nvic.hpp"
#include "ohal/platforms/stm32u0/family_tag.hpp"
#include "ohal/platforms/stm32u0/models/stm32u083/irq_numbers.hpp"

namespace ohal::platforms::stm32u0::stm32u083 {

inline constexpr uintptr_t kNvicIserBase = 0xE000'E100U;
inline constexpr uintptr_t kNvicIcerBase = 0xE000'E180U;
inline constexpr uintptr_t kNvicIsprBase = 0xE000'E200U;
inline constexpr uintptr_t kNvicIcprBase = 0xE000'E280U;
inline constexpr uintptr_t kNvicIabrBase = 0xE000'E300U;
inline constexpr uintptr_t kNvicIprBase = 0xE000'E400U;
inline constexpr uint8_t kNvicIrqPerWord = 32U;
inline constexpr uint8_t kNvicPriorityFieldBits = 8U;

} // namespace ohal::platforms::stm32u0::stm32u083

namespace ohal::nvic {

template <>
struct has_nvic<ohal::platforms::stm32u0::Family> : std::true_type {};

template <>
struct nvic_priority_bits<ohal::platforms::stm32u0::Family> : std::integral_constant<uint8_t, 2U> {
};

template <auto Irq>
struct Controller<ohal::platforms::stm32u0::Family, Irq> {
  using IrqType = std::decay_t<decltype(Irq)>;
  static_assert(std::is_enum_v<IrqType>,
                "ohal: nvic::Controller IRQ parameter must be an enum constant.");
  static_assert(std::is_same_v<IrqType, ohal::platforms::stm32u0::stm32u083::IrqNumber>,
                "ohal: STM32U0 nvic::Controller expects stm32u083::IrqNumber.");

  static constexpr uint32_t kIrqNumber = static_cast<uint32_t>(Irq);
  static constexpr uint32_t kRegisterIndex =
      kIrqNumber / ohal::platforms::stm32u0::stm32u083::kNvicIrqPerWord;
  static constexpr uint32_t kBitIndex =
      kIrqNumber % ohal::platforms::stm32u0::stm32u083::kNvicIrqPerWord;
  static constexpr uint32_t kBitMask = 1UL << kBitIndex;
  static constexpr uint32_t kPriorityRegisterIndex = kIrqNumber / 4U;
  static constexpr uint32_t kPriorityFieldIndex = kIrqNumber % 4U;
  static constexpr uint32_t kPriorityShift =
      kPriorityFieldIndex * ohal::platforms::stm32u0::stm32u083::kNvicPriorityFieldBits;
  static constexpr uint32_t kPriorityEncodedShift =
      kPriorityShift + (8U - nvic_priority_bits<ohal::platforms::stm32u0::Family>::value);
  static constexpr uint32_t kPriorityValueMask =
      (1UL << nvic_priority_bits<ohal::platforms::stm32u0::Family>::value) - 1UL;
  static constexpr uint32_t kPriorityFieldMask = 0xFFUL << kPriorityShift;

  using Iser = ohal::core::Register<ohal::platforms::stm32u0::stm32u083::kNvicIserBase +
                                    (kRegisterIndex * sizeof(uint32_t))>;
  using Icer = ohal::core::Register<ohal::platforms::stm32u0::stm32u083::kNvicIcerBase +
                                    (kRegisterIndex * sizeof(uint32_t))>;
  using Ispr = ohal::core::Register<ohal::platforms::stm32u0::stm32u083::kNvicIsprBase +
                                    (kRegisterIndex * sizeof(uint32_t))>;
  using Icpr = ohal::core::Register<ohal::platforms::stm32u0::stm32u083::kNvicIcprBase +
                                    (kRegisterIndex * sizeof(uint32_t))>;
  using Iabr = ohal::core::Register<ohal::platforms::stm32u0::stm32u083::kNvicIabrBase +
                                    (kRegisterIndex * sizeof(uint32_t))>;
  using Ipr = ohal::core::Register<ohal::platforms::stm32u0::stm32u083::kNvicIprBase +
                                   (kPriorityRegisterIndex * sizeof(uint32_t))>;

  static void enable_irq() noexcept { Iser::write(kBitMask); }

  static void disable_irq() noexcept { Icer::write(kBitMask); }

  static void set_pending() noexcept { Ispr::write(kBitMask); }

  static void clear_pending() noexcept { Icpr::write(kBitMask); }

  [[nodiscard]] static bool is_pending() noexcept { return (Ispr::read() & kBitMask) != 0U; }

  [[nodiscard]] static bool is_active() noexcept { return (Iabr::read() & kBitMask) != 0U; }

  static void set_priority(uint8_t priority) noexcept {
    const uint32_t value = Ipr::read();
    const uint32_t cleaned = value & ~kPriorityFieldMask;
    const uint32_t encoded = (static_cast<uint32_t>(priority) & kPriorityValueMask)
                             << kPriorityEncodedShift;
    Ipr::write(cleaned | encoded);
  }

  [[nodiscard]] static uint8_t get_priority() noexcept {
    const uint32_t value = Ipr::read();
    return static_cast<uint8_t>((value >> kPriorityEncodedShift) & kPriorityValueMask);
  }
};

} // namespace ohal::nvic

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_NVIC_HPP
