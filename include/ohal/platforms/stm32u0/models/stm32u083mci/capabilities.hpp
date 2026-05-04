#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083MCI_CAPABILITIES_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083MCI_CAPABILITIES_HPP

// STM32U083MCI (UFBGA81) capability traits.
//
// The MCI package shares PortA/B/C/D/F bonding with the MCT (LQFP80) package;
// those capability trait specialisations are in the shared header.
// PortE differs: the MCI bonds out PE3, PE7, PE8, and PE9 (PE3 is absent on the LQFP80).

#include <type_traits>

#include "ohal/platforms/stm32u0/models/stm32u083mc/capabilities.hpp"

namespace ohal::gpio::capabilities {

namespace detail {
// STM32U083MCI (UFBGA81) PortE bonding: PE3, PE7, PE8, PE9 are bonded out.
// PE3 is only bonded on the 81-pin UFBGA package (absent on LQFP80).
inline constexpr uint8_t kMciPortEPin3 = 3U;
inline constexpr uint8_t kMciPortEFirstPin = 7U;
inline constexpr uint8_t kMciPortELastPin = 9U;

template <uint8_t PinNum>
using MciPortECapability =
    std::bool_constant<(PinNum == kMciPortEPin3) ||
                       (PinNum >= kMciPortEFirstPin && PinNum <= kMciPortELastPin)>;
} // namespace detail

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

// PortE — PE3 and PE7–PE9 bonded out on the STM32U083MCI (UFBGA81).
template <uint8_t PinNum>
struct supports_output_type<PortE, PinNum> : detail::MciPortECapability<PinNum> {};

template <uint8_t PinNum>
struct supports_output_speed<PortE, PinNum> : detail::MciPortECapability<PinNum> {};

template <uint8_t PinNum>
struct supports_pull<PortE, PinNum> : detail::MciPortECapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortE, PinNum> : detail::MciPortECapability<PinNum> {};

// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio::capabilities

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083MCI_CAPABILITIES_HPP
