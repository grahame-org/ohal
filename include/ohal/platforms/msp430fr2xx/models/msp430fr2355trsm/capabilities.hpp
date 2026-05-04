#ifndef OHAL_PLATFORMS_MSP430FR2XX_MODELS_MSP430FR2355TRSM_CAPABILITIES_HPP
#define OHAL_PLATFORMS_MSP430FR2XX_MODELS_MSP430FR2355TRSM_CAPABILITIES_HPP

// This header is standalone to avoid partial-specialisation redefinition.
// MSP430FR2355TRSM (VQFN32):
//   PortA (P1): all 8 bits bonded.
//   PortB (P2): bits 0,1,4,5,6,7 bonded (bits 2,3 not bonded) — mask 0xF3.
//   PortC (P3): all 8 bits bonded.
//   PortD (P4): bits 0,1,2,3,6,7 bonded (bits 4,5 not bonded) — mask 0xCF.
//   PortE (P5): not bonded.
//   PortF (P6): not bonded.

#include <type_traits>

#include "ohal/core/capabilities.hpp"
#include "ohal/gpio.hpp"
#include "ohal/platforms/msp430fr2xx/models/msp430fr2355/constants.hpp"

namespace ohal::gpio::capabilities {

namespace detail {
// VQFN32 bonded-pin bitmasks.
inline constexpr uint8_t kTrsmPortBMask = 0xF3U; ///< P2: bits 0,1,4,5,6,7
inline constexpr uint8_t kTrsmPortDMask = 0xCFU; ///< P4: bits 0,1,2,3,6,7

template <uint8_t PinNum>
using TrsmFullPortCapability =
    std::bool_constant<(PinNum < platforms::msp430fr2xx::msp430fr2355::kPinCount)>;

template <uint8_t PinNum, uint8_t Mask>
using TrsmMaskedPortCapability =
    std::bool_constant<(PinNum < platforms::msp430fr2xx::msp430fr2355::kPinCount) &&
                       (((Mask >> PinNum) & 1U) != 0U)>;
} // namespace detail

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

// PortA (P1) — all bits 0-7 bonded out on the VQFN32.
template <uint8_t PinNum>
struct supports_pull<PortA, PinNum> : detail::TrsmFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortA, PinNum> : detail::TrsmFullPortCapability<PinNum> {};

// PortB (P2) — bits 0,1,4,5,6,7 bonded (mask 0xF3).
template <uint8_t PinNum>
struct supports_pull<PortB, PinNum>
    : detail::TrsmMaskedPortCapability<PinNum, detail::kTrsmPortBMask> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortB, PinNum>
    : detail::TrsmMaskedPortCapability<PinNum, detail::kTrsmPortBMask> {};

// PortC (P3) — all bits 0-7 bonded out on the VQFN32.
template <uint8_t PinNum>
struct supports_pull<PortC, PinNum> : detail::TrsmFullPortCapability<PinNum> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortC, PinNum> : detail::TrsmFullPortCapability<PinNum> {};

// PortD (P4) — bits 0,1,2,3,6,7 bonded (mask 0xCF).
template <uint8_t PinNum>
struct supports_pull<PortD, PinNum>
    : detail::TrsmMaskedPortCapability<PinNum, detail::kTrsmPortDMask> {};

template <uint8_t PinNum>
struct supports_alternate_function<PortD, PinNum>
    : detail::TrsmMaskedPortCapability<PinNum, detail::kTrsmPortDMask> {};

// PortE (P5) — not bonded out on the VQFN32.
template <uint8_t PinNum>
struct supports_pull<PortE, PinNum> : std::false_type {};

template <uint8_t PinNum>
struct supports_alternate_function<PortE, PinNum> : std::false_type {};

// PortF (P6) — not bonded out on the VQFN32.
template <uint8_t PinNum>
struct supports_pull<PortF, PinNum> : std::false_type {};

template <uint8_t PinNum>
struct supports_alternate_function<PortF, PinNum> : std::false_type {};

// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio::capabilities

#endif // OHAL_PLATFORMS_MSP430FR2XX_MODELS_MSP430FR2355TRSM_CAPABILITIES_HPP
