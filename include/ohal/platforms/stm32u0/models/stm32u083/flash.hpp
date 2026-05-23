#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_FLASH_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_FLASH_HPP

// Shared STM32U0 FLASH register map.
//
// This header provides the FLASH peripheral base address constant,
// register-offset constants, the FlashRegs<Base> template struct, and the
// Flash concrete type alias.  The register layout is identical across every
// STM32U031, STM32U073, and STM32U083 device variant (RM0503 Rev 4, §3).
//
// Register-map source: Table 19 (RM0503 Rev 4, §3.7).
// Interrupt events: Table 18 (RM0503 Rev 4, §3.6).

#include <cstdint>

#include "ohal/core/register.hpp"

namespace ohal::platforms::stm32u0::stm32u083
{

// ---------------------------------------------------------------------------
// FLASH peripheral base address (RM0503 Rev 4, Table 4)
// ---------------------------------------------------------------------------

inline constexpr uintptr_t kFlashBase = 0x4002'2000U;

// ---------------------------------------------------------------------------
// FLASH register offsets (RM0503 Rev 4, Table 19)
// ---------------------------------------------------------------------------

inline constexpr uintptr_t kFlashAcrOffset = 0x000U;       ///< Access control register
inline constexpr uintptr_t kFlashKeyrOffset = 0x008U;      ///< Flash key register
inline constexpr uintptr_t kFlashOptkeyrOffset = 0x00CU;   ///< Option key register
inline constexpr uintptr_t kFlashSrOffset = 0x010U;        ///< Status register
inline constexpr uintptr_t kFlashCrOffset = 0x014U;        ///< Control register
inline constexpr uintptr_t kFlashEccrOffset = 0x018U;      ///< ECC register
inline constexpr uintptr_t kFlashOptrOffset = 0x020U;      ///< Option register
inline constexpr uintptr_t kFlashWrp1arOffset = 0x02CU;    ///< WRP area A address register
inline constexpr uintptr_t kFlashWrp1brOffset = 0x030U;    ///< WRP area B address register
inline constexpr uintptr_t kFlashSecrOffset = 0x080U;      ///< Security register
inline constexpr uintptr_t kFlashOem1keyr1Offset = 0x088U; ///< OEM1 key register 1 (bits 31:0)
inline constexpr uintptr_t kFlashOem1keyr2Offset = 0x08CU; ///< OEM1 key register 2 (bits 63:32)
inline constexpr uintptr_t kFlashOem1keyr3Offset = 0x090U; ///< OEM1 key register 3 (bits 95:64)
inline constexpr uintptr_t kFlashOem1keyr4Offset = 0x094U; ///< OEM1 key register 4 (bits 127:96)
inline constexpr uintptr_t kFlashOem2keyr1Offset = 0x098U; ///< OEM2 key register 1 (bits 31:0)
inline constexpr uintptr_t kFlashOem2keyr2Offset = 0x09CU; ///< OEM2 key register 2 (bits 63:32)
inline constexpr uintptr_t kFlashOem2keyr3Offset = 0x0A0U; ///< OEM2 key register 3 (bits 95:64)
inline constexpr uintptr_t kFlashOem2keyr4Offset = 0x0A4U; ///< OEM2 key register 4 (bits 127:96)
inline constexpr uintptr_t kFlashOemkeysrOffset = 0x0A8U;  ///< OEM key status register
inline constexpr uintptr_t kFlashHdpcrOffset = 0x0ACU;     ///< HDP control register
inline constexpr uintptr_t kFlashHdpextrOffset = 0x0B0U;   ///< HDP extension register

// ---------------------------------------------------------------------------
// FLASH register-struct template
//
// Parametrised on the peripheral base address so the layout can be reused
// for any future alias or mock base.  Use the concrete Flash alias below for
// production code.
// ---------------------------------------------------------------------------

/// Register map for the FLASH peripheral (RM0503 Rev 4, §3.7).
/// @tparam Base  Physical base address of the FLASH peripheral.
template <uintptr_t Base>
struct FlashRegs
{
    using Acr = ohal::core::Register<Base + kFlashAcrOffset>;             ///< §3.7.1
    using Keyr = ohal::core::Register<Base + kFlashKeyrOffset>;           ///< §3.7.2
    using Optkeyr = ohal::core::Register<Base + kFlashOptkeyrOffset>;     ///< §3.7.3
    using Sr = ohal::core::Register<Base + kFlashSrOffset>;               ///< §3.7.4
    using Cr = ohal::core::Register<Base + kFlashCrOffset>;               ///< §3.7.5
    using Eccr = ohal::core::Register<Base + kFlashEccrOffset>;           ///< §3.7.6
    using Optr = ohal::core::Register<Base + kFlashOptrOffset>;           ///< §3.7.7
    using Wrp1ar = ohal::core::Register<Base + kFlashWrp1arOffset>;       ///< §3.7.8
    using Wrp1br = ohal::core::Register<Base + kFlashWrp1brOffset>;       ///< §3.7.9
    using Secr = ohal::core::Register<Base + kFlashSecrOffset>;           ///< §3.7.10
    using Oem1keyr1 = ohal::core::Register<Base + kFlashOem1keyr1Offset>; ///< §3.7.11
    using Oem1keyr2 = ohal::core::Register<Base + kFlashOem1keyr2Offset>; ///< §3.7.12
    using Oem1keyr3 = ohal::core::Register<Base + kFlashOem1keyr3Offset>; ///< §3.7.13
    using Oem1keyr4 = ohal::core::Register<Base + kFlashOem1keyr4Offset>; ///< §3.7.14
    using Oem2keyr1 = ohal::core::Register<Base + kFlashOem2keyr1Offset>; ///< §3.7.15
    using Oem2keyr2 = ohal::core::Register<Base + kFlashOem2keyr2Offset>; ///< §3.7.16
    using Oem2keyr3 = ohal::core::Register<Base + kFlashOem2keyr3Offset>; ///< §3.7.17
    using Oem2keyr4 = ohal::core::Register<Base + kFlashOem2keyr4Offset>; ///< §3.7.18
    using Oemkeysr =
        ohal::core::Register<Base + kFlashOemkeysrOffset>; ///< §3.7.19 (FLASH_KEYSR in Table 19)
    using Hdpcr = ohal::core::Register<Base + kFlashHdpcrOffset>;     ///< §3.7.20
    using Hdpextr = ohal::core::Register<Base + kFlashHdpextrOffset>; ///< §3.7.21
};

// ---------------------------------------------------------------------------
// Concrete FLASH instance type alias
// ---------------------------------------------------------------------------

using Flash = FlashRegs<kFlashBase>;

} // namespace ohal::platforms::stm32u0::stm32u083

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_FLASH_HPP
