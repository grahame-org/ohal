#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U031F_CAPABILITIES_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U031F_CAPABILITIES_HPP

#include <type_traits>

#include "ohal/core/capabilities.hpp"
#include "ohal/gpio.hpp"

namespace ohal::gpio::capabilities {

namespace detail {
// STM32U031F (TSSOP20) bonded-out pin ranges.
// GPIOA: bits 0-14 bonded out (PA15 absent).
// GPIOB: bits 0-1 and 4-9 bonded out (bits 2-3 and 10-15 absent).
// GPIOC: only bits 14-15 bonded out (bits 0-13 absent).
// GPIOF: only bits 2-3 bonded out (bits 0-1 and 4-15 absent).
// GPIOD and GPIOE: not bonded out.

inline constexpr uint8_t kFFullPortPinCount = 16U;
inline constexpr uint8_t kFPortABitCount = 15U; ///< PA0-PA14 bonded (PA15 absent)
inline constexpr uint8_t kFPortBLowCount = 2U;  ///< PB0-PB1 bonded
inline constexpr uint8_t kFPortBMidStart = 4U;  ///< PB4-PB9 bonded start
inline constexpr uint8_t kFPortBMidEnd = 10U;   ///< PB4-PB9 bonded end (exclusive)
inline constexpr uint8_t kFPortCFirstBit = 14U; ///< PC14-PC15 bonded
inline constexpr uint8_t kFPortFFirstPin = 2U;  ///< PF2-PF3 bonded
inline constexpr uint8_t kFPortFLastPin = 3U;   ///< PF2-PF3 bonded

template <uint8_t PinNum>
using FPortACapability = std::bool_constant<(PinNum < kFPortABitCount)>;

template <uint8_t PinNum>
using FPortBCapability = std::bool_constant<(PinNum < kFPortBLowCount) ||
                                            (PinNum >= kFPortBMidStart && PinNum < kFPortBMidEnd)>;

template <uint8_t PinNum>
using FPortCCapability =
    std::bool_constant<(PinNum >= kFPortCFirstBit && PinNum < kFFullPortPinCount)>;

template <uint8_t PinNum>
using FPortFCapability =
    std::bool_constant<(PinNum == kFPortFFirstPin || PinNum == kFPortFLastPin)>;
} // namespace detail

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

// PortA — bits 0-14 bonded out (PA15 absent) on all TSSOP20 F packages.
template <uint8_t PinNum>
struct supports_output_type<PortA, PinNum> : detail::FPortACapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortA, PinNum> : detail::FPortACapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortA, PinNum> : detail::FPortACapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortA, PinNum> : detail::FPortACapability<PinNum> {};

// PortB — bits 0-1 and 4-9 bonded out on all TSSOP20 F packages.
template <uint8_t PinNum>
struct supports_output_type<PortB, PinNum> : detail::FPortBCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortB, PinNum> : detail::FPortBCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortB, PinNum> : detail::FPortBCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortB, PinNum> : detail::FPortBCapability<PinNum> {};

// PortC — only bits 14-15 bonded out on all TSSOP20 F packages.
template <uint8_t PinNum>
struct supports_output_type<PortC, PinNum> : detail::FPortCCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortC, PinNum> : detail::FPortCCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortC, PinNum> : detail::FPortCCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortC, PinNum> : detail::FPortCCapability<PinNum> {};

// PortF — only bits 2-3 bonded out on all TSSOP20 F packages.
template <uint8_t PinNum>
struct supports_output_type<PortF, PinNum> : detail::FPortFCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortF, PinNum> : detail::FPortFCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortF, PinNum> : detail::FPortFCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortF, PinNum> : detail::FPortFCapability<PinNum> {};

// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio::capabilities

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U031F_CAPABILITIES_HPP
