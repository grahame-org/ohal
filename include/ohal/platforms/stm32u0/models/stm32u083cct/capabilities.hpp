#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083CCT_CAPABILITIES_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083CCT_CAPABILITIES_HPP

#include <type_traits>

#include "ohal/core/capabilities.hpp"
#include "ohal/gpio.hpp"

namespace ohal::gpio::capabilities {

namespace detail {
// STM32U083CCT (48-pin LQFP) bonded-out pin ranges.
// GPIOA and GPIOB: all 16 bits (0–15) bonded out.
// GPIOC: only bits 13–15 bonded out (bits 0–12 are absent on this package).
// GPIOF: only bits 0–3 bonded out (bits 4–15 are absent on this package).
// GPIOD and GPIOE: not bonded out.

inline constexpr uint8_t kCctFullPortPinCount = 16U;
inline constexpr uint8_t kCctPortCFirstBit = 13U;
inline constexpr uint8_t kCctPortFBitCount = 4U;

template <uint8_t PinNum>
using CctFullPortCapability = std::bool_constant<(PinNum < kCctFullPortPinCount)>;

template <uint8_t PinNum>
using CctPortCCapability =
    std::bool_constant<(PinNum >= kCctPortCFirstBit && PinNum < kCctFullPortPinCount)>;

template <uint8_t PinNum>
using CctPortFCapability = std::bool_constant<(PinNum < kCctPortFBitCount)>;
} // namespace detail

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

// PortA — all bits 0–15 bonded out on the LQFP48 package.
template <uint8_t PinNum>
struct supports_output_type<PortA, PinNum> : detail::CctFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortA, PinNum> : detail::CctFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortA, PinNum> : detail::CctFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortA, PinNum> : detail::CctFullPortCapability<PinNum> {};

// PortB — all bits 0–15 bonded out on the LQFP48 package.
template <uint8_t PinNum>
struct supports_output_type<PortB, PinNum> : detail::CctFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortB, PinNum> : detail::CctFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortB, PinNum> : detail::CctFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortB, PinNum> : detail::CctFullPortCapability<PinNum> {};

// PortC — only bits 13–15 bonded out on the LQFP48 package.
template <uint8_t PinNum>
struct supports_output_type<PortC, PinNum> : detail::CctPortCCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortC, PinNum> : detail::CctPortCCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortC, PinNum> : detail::CctPortCCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortC, PinNum> : detail::CctPortCCapability<PinNum> {};

// PortF — only bits 0–3 bonded out on the LQFP48 package.
template <uint8_t PinNum>
struct supports_output_type<PortF, PinNum> : detail::CctPortFCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortF, PinNum> : detail::CctPortFCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortF, PinNum> : detail::CctPortFCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortF, PinNum> : detail::CctPortFCapability<PinNum> {};

// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio::capabilities

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083CCT_CAPABILITIES_HPP
