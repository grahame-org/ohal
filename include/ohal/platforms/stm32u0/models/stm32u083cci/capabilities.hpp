#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083CCI_CAPABILITIES_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083CCI_CAPABILITIES_HPP

#include <type_traits>

#include "ohal/core/capabilities.hpp"
#include "ohal/gpio.hpp"

namespace ohal::gpio::capabilities {

namespace detail {
// STM32U083CCI (48-pin UFQFPN) bonded-out pin ranges.
// GPIOA and GPIOB: all 16 bits (0–15) bonded out.
// GPIOC: only bits 13–15 bonded out (bits 0–12 are absent on this package).
// GPIOF: only bits 0–3 bonded out (bits 4–15 are absent on this package).
// GPIOD and GPIOE: not bonded out.

inline constexpr uint8_t kCciFullPortPinCount = 16U;
inline constexpr uint8_t kCciPortCFirstBit = 13U;
inline constexpr uint8_t kCciPortFBitCount = 4U;

template <uint8_t PinNum>
using CciFullPortCapability = std::bool_constant<(PinNum < kCciFullPortPinCount)>;

template <uint8_t PinNum>
using CciPortCCapability =
    std::bool_constant<(PinNum >= kCciPortCFirstBit && PinNum < kCciFullPortPinCount)>;

template <uint8_t PinNum>
using CciPortFCapability = std::bool_constant<(PinNum < kCciPortFBitCount)>;
} // namespace detail

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

// PortA — all bits 0–15 bonded out on the UFQFPN48 package.
template <uint8_t PinNum>
struct supports_output_type<PortA, PinNum> : detail::CciFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortA, PinNum> : detail::CciFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortA, PinNum> : detail::CciFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortA, PinNum> : detail::CciFullPortCapability<PinNum> {};

// PortB — all bits 0–15 bonded out on the UFQFPN48 package.
template <uint8_t PinNum>
struct supports_output_type<PortB, PinNum> : detail::CciFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortB, PinNum> : detail::CciFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortB, PinNum> : detail::CciFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortB, PinNum> : detail::CciFullPortCapability<PinNum> {};

// PortC — only bits 13–15 bonded out on the UFQFPN48 package.
template <uint8_t PinNum>
struct supports_output_type<PortC, PinNum> : detail::CciPortCCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortC, PinNum> : detail::CciPortCCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortC, PinNum> : detail::CciPortCCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortC, PinNum> : detail::CciPortCCapability<PinNum> {};

// PortF — only bits 0–3 bonded out on the UFQFPN48 package.
template <uint8_t PinNum>
struct supports_output_type<PortF, PinNum> : detail::CciPortFCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortF, PinNum> : detail::CciPortFCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortF, PinNum> : detail::CciPortFCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortF, PinNum> : detail::CciPortFCapability<PinNum> {};

// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio::capabilities

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083CCI_CAPABILITIES_HPP
