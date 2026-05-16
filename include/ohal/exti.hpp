#ifndef OHAL_EXTI_HPP
#define OHAL_EXTI_HPP

#include <cstdint>
#include <type_traits>

namespace ohal::exti {

template <typename Family>
struct has_exti : std::false_type {};

enum class Trigger : uint8_t {
  Rising = 0,
  Falling = 1,
  RisingFalling = 2,
};

template <typename Port, uint8_t PinNum>
struct Line {
  static_assert(sizeof(Port) == 0, "ohal: exti::Line is not implemented for the selected MCU.");
};

} // namespace ohal::exti

#endif // OHAL_EXTI_HPP
