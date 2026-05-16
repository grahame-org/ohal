#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083MC_CAPABILITIES_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083MC_CAPABILITIES_HPP

#include <type_traits>

#include "ohal/core/capabilities.hpp"
#include "ohal/gpio.hpp"

namespace ohal::gpio::capabilities
{

namespace detail
{
// STM32U083MC (80/81-pin MCT LQFP80 and MCI UFBGA81) bonded-out pin ranges.
// GPIOA, GPIOB, GPIOC: all 16 bits (0–15) bonded out.
// GPIOD: bits 0–6 and 8–13 bonded out; PD7, PD14, PD15 are absent.
// GPIOF: only PF0–PF3 bonded out; bits 4–15 are absent.
// GPIOE: differs between packages — defined per package (see stm32u083mct and stm32u083mci).

inline constexpr uint8_t kMcFullPortPinCount = 16U;
inline constexpr uint8_t kMcPortDAbsentPin = 7U;
inline constexpr uint8_t kMcPortDLastPin = 13U;
inline constexpr uint8_t kMcPortFBitCount = 4U;

template <uint8_t PinNum>
using McFullPortCapability = std::bool_constant<(PinNum < kMcFullPortPinCount)>;

template <uint8_t PinNum>
using McPortDCapability =
    std::bool_constant<(PinNum < kMcPortDAbsentPin) ||
                       (PinNum > kMcPortDAbsentPin && PinNum <= kMcPortDLastPin)>;

template <uint8_t PinNum>
using McPortFCapability = std::bool_constant<(PinNum < kMcPortFBitCount)>;
} // namespace detail

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

// PortA — all bits 0–15 bonded out on both 80/81-pin MC packages.
template <uint8_t PinNum>
struct supports_output_type<PortA, PinNum> : detail::McFullPortCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_output_speed<PortA, PinNum> : detail::McFullPortCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_pull<PortA, PinNum> : detail::McFullPortCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_alternate_function<PortA, PinNum> : detail::McFullPortCapability<PinNum>
{
};

// PortB — all bits 0–15 bonded out on both 80/81-pin MC packages.
template <uint8_t PinNum>
struct supports_output_type<PortB, PinNum> : detail::McFullPortCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_output_speed<PortB, PinNum> : detail::McFullPortCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_pull<PortB, PinNum> : detail::McFullPortCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_alternate_function<PortB, PinNum> : detail::McFullPortCapability<PinNum>
{
};

// PortC — all bits 0–15 bonded out on both 80/81-pin MC packages.
template <uint8_t PinNum>
struct supports_output_type<PortC, PinNum> : detail::McFullPortCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_output_speed<PortC, PinNum> : detail::McFullPortCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_pull<PortC, PinNum> : detail::McFullPortCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_alternate_function<PortC, PinNum> : detail::McFullPortCapability<PinNum>
{
};

// PortD — bits 0–6 and 8–13 bonded out; PD7, PD14, PD15 are absent.
template <uint8_t PinNum>
struct supports_output_type<PortD, PinNum> : detail::McPortDCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_output_speed<PortD, PinNum> : detail::McPortDCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_pull<PortD, PinNum> : detail::McPortDCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_alternate_function<PortD, PinNum> : detail::McPortDCapability<PinNum>
{
};

// PortF — only PF0–PF3 bonded out on both 80/81-pin MC packages.
template <uint8_t PinNum>
struct supports_output_type<PortF, PinNum> : detail::McPortFCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_output_speed<PortF, PinNum> : detail::McPortFCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_pull<PortF, PinNum> : detail::McPortFCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_alternate_function<PortF, PinNum> : detail::McPortFCapability<PinNum>
{
};

// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

// PortE capability specialisations differ between MCT and MCI packages and
// are therefore defined in the per-package headers (stm32u083mct/capabilities.hpp
// and stm32u083mci/capabilities.hpp).

} // namespace ohal::gpio::capabilities

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083MC_CAPABILITIES_HPP
