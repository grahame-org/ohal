#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_UART_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_UART_HPP

#include <cstdint>

#include "ohal/core/field.hpp"
#include "ohal/core/register.hpp"
#include "ohal/uart.hpp"

namespace ohal::platforms::stm32u0::stm32u083 {

inline constexpr uintptr_t kUsart1Base = 0x4001'3800U;
inline constexpr uintptr_t kUsart2Base = 0x4000'4400U;
inline constexpr uintptr_t kUsart3Base = 0x4000'4800U;
inline constexpr uintptr_t kUsart4Base = 0x4000'4C00U;
inline constexpr uintptr_t kLpuart1Base = 0x4000'8000U;
inline constexpr uintptr_t kLpuart2Base = 0x4000'8400U;
inline constexpr uintptr_t kLpuart3Base = 0x4000'8C00U;

inline constexpr uintptr_t kUsartCr1Offset = 0x00U;
inline constexpr uintptr_t kUsartCr2Offset = 0x04U;
inline constexpr uintptr_t kUsartCr3Offset = 0x08U;
inline constexpr uintptr_t kUsartBrrOffset = 0x0CU;
inline constexpr uintptr_t kUsartIsrOffset = 0x1CU;
inline constexpr uintptr_t kUsartIcrOffset = 0x20U;
inline constexpr uintptr_t kUsartRdrOffset = 0x24U;
inline constexpr uintptr_t kUsartTdrOffset = 0x28U;
inline constexpr uint8_t kUsartRegisterWidthBits = 32U;
inline constexpr uint8_t kUsartDataBits = 9U;

struct Usart1Tag {};
struct Usart2Tag {};
struct Usart3Tag {};
struct Usart4Tag {};
struct Lpuart1Tag {};
struct Lpuart2Tag {};
struct Lpuart3Tag {};

template <uintptr_t Base>
struct UsartRegs {
  using Cr1 = ohal::core::Register<Base + kUsartCr1Offset>;
  using Cr2 = ohal::core::Register<Base + kUsartCr2Offset>;
  using Cr3 = ohal::core::Register<Base + kUsartCr3Offset>;
  using Brr = ohal::core::Register<Base + kUsartBrrOffset>;
  using Isr = ohal::core::Register<Base + kUsartIsrOffset>;
  using Icr = ohal::core::Register<Base + kUsartIcrOffset>;
  using Rdr = ohal::core::Register<Base + kUsartRdrOffset>;
  using Tdr = ohal::core::Register<Base + kUsartTdrOffset>;
};

using Usart1 = UsartRegs<kUsart1Base>;
using Usart2 = UsartRegs<kUsart2Base>;
using Usart3 = UsartRegs<kUsart3Base>;
using Usart4 = UsartRegs<kUsart4Base>;
using Lpuart1 = UsartRegs<kLpuart1Base>;
using Lpuart2 = UsartRegs<kLpuart2Base>;
using Lpuart3 = UsartRegs<kLpuart3Base>;

} // namespace ohal::platforms::stm32u0::stm32u083

namespace ohal::uart {

namespace detail {

template <typename RegsType>
struct UsartPortBase {
  using Regs = RegsType;

  using Cr1 = typename Regs::Cr1;
  using Cr2 = typename Regs::Cr2;
  using Cr3 = typename Regs::Cr3;
  using Brr = typename Regs::Brr;
  using Isr = ohal::core::BitField<typename Regs::Isr, 0U,
                                   ohal::platforms::stm32u0::stm32u083::kUsartRegisterWidthBits,
                                   ohal::core::Access::ReadOnly>;
  using Icr = ohal::core::BitField<typename Regs::Icr, 0U,
                                   ohal::platforms::stm32u0::stm32u083::kUsartRegisterWidthBits,
                                   ohal::core::Access::WriteOnly>;
  using Rdr = ohal::core::BitField<typename Regs::Rdr, 0U,
                                   ohal::platforms::stm32u0::stm32u083::kUsartDataBits,
                                   ohal::core::Access::ReadOnly, uint16_t>;
  using Tdr = ohal::core::BitField<typename Regs::Tdr, 0U,
                                   ohal::platforms::stm32u0::stm32u083::kUsartDataBits,
                                   ohal::core::Access::WriteOnly, uint16_t>;
};

} // namespace detail

template <>
struct Port<ohal::platforms::stm32u0::stm32u083::Usart1Tag>
    : detail::UsartPortBase<ohal::platforms::stm32u0::stm32u083::Usart1> {};

template <>
struct Port<ohal::platforms::stm32u0::stm32u083::Usart2Tag>
    : detail::UsartPortBase<ohal::platforms::stm32u0::stm32u083::Usart2> {};

template <>
struct Port<ohal::platforms::stm32u0::stm32u083::Usart3Tag>
    : detail::UsartPortBase<ohal::platforms::stm32u0::stm32u083::Usart3> {};

template <>
struct Port<ohal::platforms::stm32u0::stm32u083::Usart4Tag>
    : detail::UsartPortBase<ohal::platforms::stm32u0::stm32u083::Usart4> {};

template <>
struct Port<ohal::platforms::stm32u0::stm32u083::Lpuart1Tag>
    : detail::UsartPortBase<ohal::platforms::stm32u0::stm32u083::Lpuart1> {};

template <>
struct Port<ohal::platforms::stm32u0::stm32u083::Lpuart2Tag>
    : detail::UsartPortBase<ohal::platforms::stm32u0::stm32u083::Lpuart2> {};

#if !defined(OHAL_MODEL_STM32U031R6T) && !defined(OHAL_MODEL_STM32U031R6I) &&                      \
    !defined(OHAL_MODEL_STM32U031R8T) && !defined(OHAL_MODEL_STM32U031R8I) &&                      \
    !defined(OHAL_MODEL_STM32U031C6U) && !defined(OHAL_MODEL_STM32U031C6T) &&                      \
    !defined(OHAL_MODEL_STM32U031C8U) && !defined(OHAL_MODEL_STM32U031C8T) &&                      \
    !defined(OHAL_MODEL_STM32U031K4U) && !defined(OHAL_MODEL_STM32U031K6U) &&                      \
    !defined(OHAL_MODEL_STM32U031K8U) && !defined(OHAL_MODEL_STM32U031G6Y) &&                      \
    !defined(OHAL_MODEL_STM32U031G8Y) && !defined(OHAL_MODEL_STM32U031F4P) &&                      \
    !defined(OHAL_MODEL_STM32U031F6P) && !defined(OHAL_MODEL_STM32U031F8P)
template <>
struct Port<ohal::platforms::stm32u0::stm32u083::Lpuart3Tag>
    : detail::UsartPortBase<ohal::platforms::stm32u0::stm32u083::Lpuart3> {};
#endif

namespace capabilities {

template <>
struct supports_synchronous_mode<ohal::platforms::stm32u0::stm32u083::Usart1Tag> : std::true_type {
};
template <>
struct supports_synchronous_mode<ohal::platforms::stm32u0::stm32u083::Usart2Tag> : std::true_type {
};
template <>
struct supports_synchronous_mode<ohal::platforms::stm32u0::stm32u083::Usart3Tag> : std::true_type {
};
template <>
struct supports_synchronous_mode<ohal::platforms::stm32u0::stm32u083::Usart4Tag> : std::true_type {
};

template <>
struct supports_smartcard_mode<ohal::platforms::stm32u0::stm32u083::Usart1Tag> : std::true_type {};
template <>
struct supports_smartcard_mode<ohal::platforms::stm32u0::stm32u083::Usart2Tag> : std::true_type {};

template <>
struct supports_irda_mode<ohal::platforms::stm32u0::stm32u083::Usart1Tag> : std::true_type {};
template <>
struct supports_irda_mode<ohal::platforms::stm32u0::stm32u083::Usart2Tag> : std::true_type {};

template <>
struct supports_lin_mode<ohal::platforms::stm32u0::stm32u083::Usart1Tag> : std::true_type {};
template <>
struct supports_lin_mode<ohal::platforms::stm32u0::stm32u083::Usart2Tag> : std::true_type {};

template <>
struct supports_receiver_timeout<ohal::platforms::stm32u0::stm32u083::Usart1Tag> : std::true_type {
};
template <>
struct supports_receiver_timeout<ohal::platforms::stm32u0::stm32u083::Usart2Tag> : std::true_type {
};

template <>
struct supports_modbus_mode<ohal::platforms::stm32u0::stm32u083::Usart1Tag> : std::true_type {};
template <>
struct supports_modbus_mode<ohal::platforms::stm32u0::stm32u083::Usart2Tag> : std::true_type {};

template <>
struct supports_auto_baud_rate_detection<ohal::platforms::stm32u0::stm32u083::Usart1Tag>
    : std::true_type {};
template <>
struct supports_auto_baud_rate_detection<ohal::platforms::stm32u0::stm32u083::Usart2Tag>
    : std::true_type {};

template <>
struct supports_fifo<ohal::platforms::stm32u0::stm32u083::Usart1Tag> : std::true_type {};
template <>
struct supports_fifo<ohal::platforms::stm32u0::stm32u083::Usart2Tag> : std::true_type {};
template <>
struct supports_fifo<ohal::platforms::stm32u0::stm32u083::Lpuart1Tag> : std::true_type {};
template <>
struct supports_fifo<ohal::platforms::stm32u0::stm32u083::Lpuart2Tag> : std::true_type {};

template <>
struct supports_prescaler<ohal::platforms::stm32u0::stm32u083::Usart1Tag> : std::true_type {};
template <>
struct supports_prescaler<ohal::platforms::stm32u0::stm32u083::Usart2Tag> : std::true_type {};
template <>
struct supports_prescaler<ohal::platforms::stm32u0::stm32u083::Lpuart1Tag> : std::true_type {};
template <>
struct supports_prescaler<ohal::platforms::stm32u0::stm32u083::Lpuart2Tag> : std::true_type {};

template <>
struct supports_wakeup_from_stop_0_1<ohal::platforms::stm32u0::stm32u083::Usart1Tag>
    : std::true_type {};
template <>
struct supports_wakeup_from_stop_0_1<ohal::platforms::stm32u0::stm32u083::Usart2Tag>
    : std::true_type {};
template <>
struct supports_wakeup_from_stop_0_1<ohal::platforms::stm32u0::stm32u083::Lpuart1Tag>
    : std::true_type {};
template <>
struct supports_wakeup_from_stop_0_1<ohal::platforms::stm32u0::stm32u083::Lpuart2Tag>
    : std::true_type {};

template <>
struct supports_dual_clock_domain<ohal::platforms::stm32u0::stm32u083::Lpuart1Tag>
    : std::true_type {};
template <>
struct supports_dual_clock_domain<ohal::platforms::stm32u0::stm32u083::Lpuart2Tag>
    : std::true_type {};
#if !defined(OHAL_MODEL_STM32U031R6T) && !defined(OHAL_MODEL_STM32U031R6I) &&                      \
    !defined(OHAL_MODEL_STM32U031R8T) && !defined(OHAL_MODEL_STM32U031R8I) &&                      \
    !defined(OHAL_MODEL_STM32U031C6U) && !defined(OHAL_MODEL_STM32U031C6T) &&                      \
    !defined(OHAL_MODEL_STM32U031C8U) && !defined(OHAL_MODEL_STM32U031C8T) &&                      \
    !defined(OHAL_MODEL_STM32U031K4U) && !defined(OHAL_MODEL_STM32U031K6U) &&                      \
    !defined(OHAL_MODEL_STM32U031K8U) && !defined(OHAL_MODEL_STM32U031G6Y) &&                      \
    !defined(OHAL_MODEL_STM32U031G8Y) && !defined(OHAL_MODEL_STM32U031F4P) &&                      \
    !defined(OHAL_MODEL_STM32U031F6P) && !defined(OHAL_MODEL_STM32U031F8P)
template <>
struct supports_fifo<ohal::platforms::stm32u0::stm32u083::Lpuart3Tag> : std::true_type {};
template <>
struct supports_prescaler<ohal::platforms::stm32u0::stm32u083::Lpuart3Tag> : std::true_type {};
template <>
struct supports_wakeup_from_stop_0_1<ohal::platforms::stm32u0::stm32u083::Lpuart3Tag>
    : std::true_type {};
template <>
struct supports_dual_clock_domain<ohal::platforms::stm32u0::stm32u083::Lpuart3Tag>
    : std::true_type {};
#endif

} // namespace capabilities

} // namespace ohal::uart

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_UART_HPP
