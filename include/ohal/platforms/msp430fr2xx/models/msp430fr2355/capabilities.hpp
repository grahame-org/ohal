#ifndef OHAL_PLATFORMS_MSP430FR2XX_MODELS_MSP430FR2355_CAPABILITIES_HPP
#define OHAL_PLATFORMS_MSP430FR2XX_MODELS_MSP430FR2355_CAPABILITIES_HPP

#include <type_traits>

#include "ohal/core/capabilities.hpp"
#include "ohal/gpio.hpp"
#include "ohal/platforms/msp430fr2xx/models/msp430fr2355/constants.hpp"

namespace ohal::gpio::capabilities
{

namespace detail
{
/// Evaluates to true_type for valid MSP430FR2355 pin numbers (0–7), false_type otherwise.
/// Reuses kPinCount from the platform namespace to avoid duplicating the constant.
template <uint8_t PinNum>
using Msp430fr2355PortCapability =
    std::bool_constant<(PinNum < platforms::msp430fr2xx::msp430fr2355::kPinCount)>;
} // namespace detail

// MSP430FR2355 supports pull resistors and alternate-function selection on all
// ports and pins 0–7.  Output type and output speed are not supported; the
// default false_type primary templates handle those.

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
template <uint8_t PinNum>
struct supports_pull<PortA, PinNum> : detail::Msp430fr2355PortCapability<PinNum>
{
};
template <uint8_t PinNum>
struct supports_pull<PortB, PinNum> : detail::Msp430fr2355PortCapability<PinNum>
{
};
template <uint8_t PinNum>
struct supports_pull<PortC, PinNum> : detail::Msp430fr2355PortCapability<PinNum>
{
};
template <uint8_t PinNum>
struct supports_pull<PortD, PinNum> : detail::Msp430fr2355PortCapability<PinNum>
{
};
template <uint8_t PinNum>
struct supports_pull<PortE, PinNum> : detail::Msp430fr2355PortCapability<PinNum>
{
};
template <uint8_t PinNum>
struct supports_pull<PortF, PinNum> : detail::Msp430fr2355PortCapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_alternate_function<PortA, PinNum> : detail::Msp430fr2355PortCapability<PinNum>
{
};
template <uint8_t PinNum>
struct supports_alternate_function<PortB, PinNum> : detail::Msp430fr2355PortCapability<PinNum>
{
};
template <uint8_t PinNum>
struct supports_alternate_function<PortC, PinNum> : detail::Msp430fr2355PortCapability<PinNum>
{
};
template <uint8_t PinNum>
struct supports_alternate_function<PortD, PinNum> : detail::Msp430fr2355PortCapability<PinNum>
{
};
template <uint8_t PinNum>
struct supports_alternate_function<PortE, PinNum> : detail::Msp430fr2355PortCapability<PinNum>
{
};
template <uint8_t PinNum>
struct supports_alternate_function<PortF, PinNum> : detail::Msp430fr2355PortCapability<PinNum>
{
};
// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio::capabilities

#endif // OHAL_PLATFORMS_MSP430FR2XX_MODELS_MSP430FR2355_CAPABILITIES_HPP
