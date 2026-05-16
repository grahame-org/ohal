#ifndef OHAL_UART_HPP
#define OHAL_UART_HPP

#include <cstdint>
#include <type_traits>

namespace ohal::uart
{

inline constexpr uint32_t kDefaultBaudRate = 115'200U;

enum class WordLength : uint8_t
{
    Bits7 = 0,
    Bits8 = 1,
    Bits9 = 2,
};

enum class StopBits : uint8_t
{
    One = 0,
    Half = 1,
    Two = 2,
    OneAndHalf = 3,
};

enum class Parity : uint8_t
{
    None = 0,
    Even = 1,
    Odd = 2,
};

enum class HwFlowCtl : uint8_t
{
    None = 0,
    Rts = 1,
    Cts = 2,
    RtsCts = 3,
};

struct Config
{
    uint32_t baud_rate = kDefaultBaudRate;
    WordLength word_length = WordLength::Bits8;
    StopBits stop_bits = StopBits::One;
    Parity parity = Parity::None;
    HwFlowCtl hw_flow_ctl = HwFlowCtl::None;
};

template <typename Instance>
struct Port
{
    static_assert(sizeof(Instance) == 0,
                  "ohal: uart::Port is not implemented for the selected MCU. "
                  "Ensure -DOHAL_FAMILY_* and -DOHAL_MODEL_* are set correctly.");
};

namespace capabilities
{

template <typename Instance>
struct supports_hardware_flow_control : std::false_type
{
};

template <typename Instance>
struct supports_dma : std::false_type
{
};

template <typename Instance>
struct supports_multiprocessor_communication : std::false_type
{
};

template <typename Instance>
struct supports_synchronous_mode : std::false_type
{
};

template <typename Instance>
struct supports_smartcard_mode : std::false_type
{
};

template <typename Instance>
struct supports_single_wire_half_duplex : std::false_type
{
};

template <typename Instance>
struct supports_irda_mode : std::false_type
{
};

template <typename Instance>
struct supports_lin_mode : std::false_type
{
};

template <typename Instance>
struct supports_receiver_timeout : std::false_type
{
};

template <typename Instance>
struct supports_modbus_mode : std::false_type
{
};

template <typename Instance>
struct supports_auto_baud_rate_detection : std::false_type
{
};

template <typename Instance>
struct supports_driver_enable : std::false_type
{
};

template <typename Instance>
struct supports_data_length_7_8_9_bits : std::false_type
{
};

template <typename Instance>
struct supports_fifo : std::false_type
{
};

template <typename Instance>
struct tx_rx_fifo_size_bytes : std::integral_constant<uint8_t, 0U>
{
};

template <typename Instance>
struct supports_prescaler : std::false_type
{
};

template <typename Instance>
struct supports_wakeup_from_stop_0_1 : std::false_type
{
};

template <typename Instance>
struct supports_dual_clock_domain : std::false_type
{
};

} // namespace capabilities

} // namespace ohal::uart

#endif // OHAL_UART_HPP
