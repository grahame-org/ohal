#ifndef OHAL_UART_HPP
#define OHAL_UART_HPP

#include <cstdint>

namespace ohal::uart {

enum class WordLength : uint8_t {
  Bits7 = 0,
  Bits8 = 1,
  Bits9 = 2,
};

enum class StopBits : uint8_t {
  One = 0,
  Half = 1,
  Two = 2,
  OneAndHalf = 3,
};

enum class Parity : uint8_t {
  None = 0,
  Even = 1,
  Odd = 2,
};

enum class HwFlowCtl : uint8_t {
  None = 0,
  Rts = 1,
  Cts = 2,
  RtsCts = 3,
};

struct Config {
  uint32_t baud_rate = 115'200U;
  WordLength word_length = WordLength::Bits8;
  StopBits stop_bits = StopBits::One;
  Parity parity = Parity::None;
  HwFlowCtl hw_flow_ctl = HwFlowCtl::None;
};

template <typename Instance>
struct Port {
  static_assert(sizeof(Instance) == 0, "ohal: uart::Port is not implemented for the selected MCU. "
                                        "Ensure -DOHAL_FAMILY_* and -DOHAL_MODEL_* are set correctly.");
};

} // namespace ohal::uart

#endif // OHAL_UART_HPP
