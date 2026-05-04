#ifndef OHAL_PLATFORMS_MSP430FR2XX_MODELS_MSP430FR2355TPT_CAPABILITIES_HPP
#define OHAL_PLATFORMS_MSP430FR2XX_MODELS_MSP430FR2355TPT_CAPABILITIES_HPP

// This header is standalone (does not include msp430fr2355/capabilities.hpp)
// to avoid partial-specialisation redefinition.  It provides all required
// capability trait specialisations for the MSP430FR2355TPT (LQFP48) package.

#include <type_traits>

#include "ohal/core/capabilities.hpp"
#include "ohal/gpio.hpp"
#include "ohal/platforms/msp430fr2xx/models/msp430fr2355/constants.hpp"

namespace ohal::gpio::capabilities {

namespace detail {
// MSP430FR2355TPT (LQFP48) bonded-out pin ranges.
// P1–P4 (PortA–PortD): all 8 bits (0–7) bonded out.
// P5 (PortE):           only bits 0–4 bonded out (5 pins on LQFP48).
// P6 (PortF):           only bits 0–6 bonded out (7 pins on LQFP48).

inline constexpr uint8_t kTptPortEBitCount = 5U; ///< P5: bits 0–4 only on LQFP48
inline constexpr uint8_t kTptPortFBitCount = 7U; ///< P6: bits 0–6 only on LQFP48

template <uint8_t PinNum>
using TptFullPortCapability =
    std::bool_constant<(PinNum < platforms::msp430fr2xx::msp430fr2355::kPinCount)>;

template <uint8_t PinNum>
using TptPortECapability = std::bool_constant<(PinNum < kTptPortEBitCount)>;

template <uint8_t PinNum>
using TptPortFCapability = std::bool_constant<(PinNum < kTptPortFBitCount)>;
} // namespace detail

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

// PortA (P1) — all bits 0–7 bonded out on the LQFP48.
template <uint8_t PinNum>
struct supports_pull<PortA, PinNum> : detail::TptFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortA, PinNum> : detail::TptFullPortCapability<PinNum> {};

// PortB (P2) — all bits 0–7 bonded out on the LQFP48.
template <uint8_t PinNum>
struct supports_pull<PortB, PinNum> : detail::TptFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortB, PinNum> : detail::TptFullPortCapability<PinNum> {};

// PortC (P3) — all bits 0–7 bonded out on the LQFP48.
template <uint8_t PinNum>
struct supports_pull<PortC, PinNum> : detail::TptFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortC, PinNum> : detail::TptFullPortCapability<PinNum> {};

// PortD (P4) — all bits 0–7 bonded out on the LQFP48.
template <uint8_t PinNum>
struct supports_pull<PortD, PinNum> : detail::TptFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortD, PinNum> : detail::TptFullPortCapability<PinNum> {};

// PortE (P5) — only bits 0–4 bonded out on the LQFP48.
template <uint8_t PinNum>
struct supports_pull<PortE, PinNum> : detail::TptPortECapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortE, PinNum> : detail::TptPortECapability<PinNum> {};

// PortF (P6) — only bits 0–6 bonded out on the LQFP48.
template <uint8_t PinNum>
struct supports_pull<PortF, PinNum> : detail::TptPortFCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortF, PinNum> : detail::TptPortFCapability<PinNum> {};

// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio::capabilities

#endif // OHAL_PLATFORMS_MSP430FR2XX_MODELS_MSP430FR2355TPT_CAPABILITIES_HPP
