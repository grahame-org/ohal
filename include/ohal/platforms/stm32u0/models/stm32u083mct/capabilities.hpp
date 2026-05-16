#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083MCT_CAPABILITIES_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083MCT_CAPABILITIES_HPP

// STM32U083MCT (LQFP80) capability traits.
//
// The MCT package shares PortA/B/C/D/F bonding with the MCI (UFBGA81) package;
// those capability trait specialisations are in the shared header.
// PortE differs: the MCT bonds out only PE7–PE9 (PE3 is absent on the 80-pin LQFP).

#include <type_traits>

#include "ohal/platforms/stm32u0/models/stm32u083mc/capabilities.hpp"

namespace ohal::gpio::capabilities
{

namespace detail
{
// STM32U083MCT (LQFP80) PortE bonding: only PE7, PE8, PE9 are bonded out.
// PE3 is not bonded on the 80-pin LQFP package (available on UFBGA81 only).
inline constexpr uint8_t kMctPortEFirstPin = 7U;
inline constexpr uint8_t kMctPortELastPin = 9U;

template <uint8_t PinNum>
using MctPortECapability =
    std::bool_constant<(PinNum >= kMctPortEFirstPin && PinNum <= kMctPortELastPin)>;
} // namespace detail

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

// PortE — only PE7–PE9 bonded out on the STM32U083MCT (LQFP80).
template <uint8_t PinNum>
struct supports_output_type<PortE, PinNum> : detail::MctPortECapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_output_speed<PortE, PinNum> : detail::MctPortECapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_pull<PortE, PinNum> : detail::MctPortECapability<PinNum>
{
};

template <uint8_t PinNum>
struct supports_alternate_function<PortE, PinNum> : detail::MctPortECapability<PinNum>
{
};

// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio::capabilities

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083MCT_CAPABILITIES_HPP
