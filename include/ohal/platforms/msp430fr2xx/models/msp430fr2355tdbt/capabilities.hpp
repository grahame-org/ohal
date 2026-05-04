#ifndef OHAL_PLATFORMS_MSP430FR2XX_MODELS_MSP430FR2355TDBT_CAPABILITIES_HPP
#define OHAL_PLATFORMS_MSP430FR2XX_MODELS_MSP430FR2355TDBT_CAPABILITIES_HPP

// This header is standalone to avoid partial-specialisation redefinition.
// MSP430FR2355TDBT (TSSOP38): P1-P4 fully bonded, P5 bits 0-1, P6 not bonded.

#include <type_traits>

#include "ohal/core/capabilities.hpp"
#include "ohal/gpio.hpp"
#include "ohal/platforms/msp430fr2xx/models/msp430fr2355/constants.hpp"

namespace ohal::gpio::capabilities {

namespace detail {
/// MSP430FR2355TDBT (TSSOP38) bonded-out pin ranges.
/// P1-P4 (PortA-PortD): all 8 bits bonded out.
/// P5 (PortE):           only bits 0-1 bonded out.
/// P6 (PortF):           not bonded out.

inline constexpr uint8_t kTdbtPortEBitCount = 2U; ///< P5: bits 0-1 only on TSSOP38

template <uint8_t PinNum>
using TdbtFullPortCapability =
    std::bool_constant<(PinNum < platforms::msp430fr2xx::msp430fr2355::kPinCount)>;

template <uint8_t PinNum>
using TdbtPortECapability = std::bool_constant<(PinNum < kTdbtPortEBitCount)>;
} // namespace detail

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

// PortA (P1) — all bits 0-7 bonded out on the TSSOP38.
template <uint8_t PinNum>
struct supports_pull<PortA, PinNum> : detail::TdbtFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortA, PinNum> : detail::TdbtFullPortCapability<PinNum> {};

// PortB (P2) — all bits 0-7 bonded out on the TSSOP38.
template <uint8_t PinNum>
struct supports_pull<PortB, PinNum> : detail::TdbtFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortB, PinNum> : detail::TdbtFullPortCapability<PinNum> {};

// PortC (P3) — all bits 0-7 bonded out on the TSSOP38.
template <uint8_t PinNum>
struct supports_pull<PortC, PinNum> : detail::TdbtFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortC, PinNum> : detail::TdbtFullPortCapability<PinNum> {};

// PortD (P4) — all bits 0-7 bonded out on the TSSOP38.
template <uint8_t PinNum>
struct supports_pull<PortD, PinNum> : detail::TdbtFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortD, PinNum> : detail::TdbtFullPortCapability<PinNum> {};

// PortE (P5) — only bits 0-1 bonded out on the TSSOP38.
template <uint8_t PinNum>
struct supports_pull<PortE, PinNum> : detail::TdbtPortECapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortE, PinNum> : detail::TdbtPortECapability<PinNum> {};

// PortF (P6) — not bonded out on the TSSOP38.
template <uint8_t PinNum>
struct supports_pull<PortF, PinNum> : std::false_type {};

template <uint8_t PinNum>
struct supports_alternate_function<PortF, PinNum> : std::false_type {};

// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio::capabilities

#endif // OHAL_PLATFORMS_MSP430FR2XX_MODELS_MSP430FR2355TDBT_CAPABILITIES_HPP
