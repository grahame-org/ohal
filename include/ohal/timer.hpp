#ifndef OHAL_TIMER_HPP
#define OHAL_TIMER_HPP

#include <cstdint>

namespace ohal::timer {

enum class ClockDivision : uint8_t {
  Div1 = 0,
  Div2 = 1,
  Div4 = 2,
};

enum class CounterMode : uint8_t {
  Up = 0,
  Down = 1,
  CenterAligned1 = 2,
  CenterAligned2 = 3,
  CenterAligned3 = 4,
};

template <typename Instance, uint8_t ChannelNum>
struct Channel {
  static_assert(sizeof(Instance) == 0,
                "ohal: timer::Channel is not implemented for the selected MCU. "
                "Ensure -DOHAL_FAMILY_* and -DOHAL_MODEL_* are set correctly.");
};

} // namespace ohal::timer

#endif // OHAL_TIMER_HPP
