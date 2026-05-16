#ifndef OHAL_NVIC_HPP
#define OHAL_NVIC_HPP

#include <cstdint>
#include <type_traits>

namespace ohal::nvic {

template <typename Family>
struct has_nvic : std::false_type {};

template <typename Family>
struct has_system_exception_priority_control : std::false_type {};

template <typename Family>
struct nvic_priority_bits : std::integral_constant<uint8_t, 0U> {};

template <typename Family, auto Irq>
struct Controller {
  static_assert(sizeof(Family) == 0,
                "ohal: nvic::Controller is not implemented for the selected MCU family.");
};

template <typename Family, auto SysExc>
struct SystemController {
  static_assert(sizeof(Family) == 0,
                "ohal: nvic::SystemController is not implemented for the selected MCU family.");
};

} // namespace ohal::nvic

#endif // OHAL_NVIC_HPP
