#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_PWR_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_PWR_HPP

// Shared STM32U0 PWR register map.
//
// This header provides the PWR peripheral base address constant,
// register-offset constants, the PwrRegs<Base> template struct, and the
// Pwr concrete type alias.  The register layout is identical across every
// STM32U031, STM32U073, and STM32U083 device variant (RM0503 Rev 4, §4).
//
// Register-map source: Table 31 (RM0503 Rev 4, §4.4).

#include <cstdint>

#include "ohal/core/field.hpp"
#include "ohal/core/register.hpp"
#include "ohal/pwr.hpp"

namespace ohal::platforms::stm32u0::stm32u083
{

// ---------------------------------------------------------------------------
// PWR peripheral base address (RM0503 Rev 4, Table 4)
// ---------------------------------------------------------------------------

inline constexpr uintptr_t kPwrBase = 0x4000'7000U;
inline constexpr uint8_t kPwrRegisterWidthBits = 32U;

// ---------------------------------------------------------------------------
// PWR register offsets (RM0503 Rev 4, Table 31)
// ---------------------------------------------------------------------------

inline constexpr uintptr_t kPwrCr1Offset = 0x000U;   ///< Power control register 1
inline constexpr uintptr_t kPwrCr2Offset = 0x004U;   ///< Power control register 2
inline constexpr uintptr_t kPwrCr3Offset = 0x008U;   ///< Power control register 3
inline constexpr uintptr_t kPwrCr4Offset = 0x00CU;   ///< Power control register 4
inline constexpr uintptr_t kPwrSr1Offset = 0x010U;   ///< Power status register 1
inline constexpr uintptr_t kPwrSr2Offset = 0x014U;   ///< Power status register 2
inline constexpr uintptr_t kPwrScrOffset = 0x018U;   ///< Power status clear register
inline constexpr uintptr_t kPwrPucraOffset = 0x020U; ///< Power Port A pull-up control register
inline constexpr uintptr_t kPwrPdcraOffset = 0x024U; ///< Power Port A pull-down control register
inline constexpr uintptr_t kPwrPucrbOffset = 0x028U; ///< Power Port B pull-up control register
inline constexpr uintptr_t kPwrPdcrbOffset = 0x02CU; ///< Power Port B pull-down control register
inline constexpr uintptr_t kPwrPucrcOffset = 0x030U; ///< Power Port C pull-up control register
inline constexpr uintptr_t kPwrPdcrcOffset = 0x034U; ///< Power Port C pull-down control register
inline constexpr uintptr_t kPwrPucrdOffset = 0x038U; ///< Power Port D pull-up control register
inline constexpr uintptr_t kPwrPdcrdOffset = 0x03CU; ///< Power Port D pull-down control register
inline constexpr uintptr_t kPwrPucreOffset = 0x040U; ///< Power Port E pull-up control register
inline constexpr uintptr_t kPwrPdcreOffset = 0x044U; ///< Power Port E pull-down control register
inline constexpr uintptr_t kPwrPucrfOffset = 0x048U; ///< Power Port F pull-up control register
inline constexpr uintptr_t kPwrPdcrfOffset = 0x04CU; ///< Power Port F pull-down control register

// ---------------------------------------------------------------------------
// PWR register-struct template
//
// Parametrised on the peripheral base address so the layout can be reused
// for any future alias or mock base.  Use the concrete Pwr alias below for
// production code.
// ---------------------------------------------------------------------------

/// Register map for the PWR peripheral (RM0503 Rev 4, §4.4).
/// @tparam Base  Physical base address of the PWR peripheral.
template <uintptr_t Base>
struct PwrRegs
{
    using Cr1 = ohal::core::Register<Base + kPwrCr1Offset>;     ///< §4.4.1
    using Cr2 = ohal::core::Register<Base + kPwrCr2Offset>;     ///< §4.4.2
    using Cr3 = ohal::core::Register<Base + kPwrCr3Offset>;     ///< §4.4.3
    using Cr4 = ohal::core::Register<Base + kPwrCr4Offset>;     ///< §4.4.4
    using Sr1 = ohal::core::Register<Base + kPwrSr1Offset>;     ///< §4.4.5
    using Sr2 = ohal::core::Register<Base + kPwrSr2Offset>;     ///< §4.4.6
    using Scr = ohal::core::Register<Base + kPwrScrOffset>;     ///< §4.4.7
    using Pucra = ohal::core::Register<Base + kPwrPucraOffset>; ///< §4.4.8
    using Pdcra = ohal::core::Register<Base + kPwrPdcraOffset>; ///< §4.4.9
    using Pucrb = ohal::core::Register<Base + kPwrPucrbOffset>; ///< §4.4.10
    using Pdcrb = ohal::core::Register<Base + kPwrPdcrbOffset>; ///< §4.4.11
    using Pucrc = ohal::core::Register<Base + kPwrPucrcOffset>; ///< §4.4.12
    using Pdcrc = ohal::core::Register<Base + kPwrPdcrcOffset>; ///< §4.4.13
    using Pucrd = ohal::core::Register<Base + kPwrPucrdOffset>; ///< §4.4.14
    using Pdcrd = ohal::core::Register<Base + kPwrPdcrdOffset>; ///< §4.4.15
    using Pucre = ohal::core::Register<Base + kPwrPucreOffset>; ///< §4.4.16
    using Pdcre = ohal::core::Register<Base + kPwrPdcreOffset>; ///< §4.4.17
    using Pucrf = ohal::core::Register<Base + kPwrPucrfOffset>; ///< §4.4.18
    using Pdcrf = ohal::core::Register<Base + kPwrPdcrfOffset>; ///< §4.4.19
};

// ---------------------------------------------------------------------------
// Concrete PWR instance type alias
// ---------------------------------------------------------------------------

using Pwr = PwrRegs<kPwrBase>;

// ---------------------------------------------------------------------------
// Instance tag type
// ---------------------------------------------------------------------------

struct PwrTag
{
};

} // namespace ohal::platforms::stm32u0::stm32u083

// ---------------------------------------------------------------------------
// ohal::pwr::Controller specialisation for STM32U0 PWR
// ---------------------------------------------------------------------------

namespace ohal::pwr
{

template <>
struct Controller<ohal::platforms::stm32u0::stm32u083::PwrTag>
{
    using Regs = ohal::platforms::stm32u0::stm32u083::Pwr;

    using Cr1 = typename Regs::Cr1;
    using Cr2 = typename Regs::Cr2;
    using Cr3 = typename Regs::Cr3;
    using Cr4 = typename Regs::Cr4;
    using Sr1 = typename Regs::Sr1;
    using Sr2 = typename Regs::Sr2;
    using Scr = ohal::core::BitField<typename Regs::Scr, 0U,
                                     ohal::platforms::stm32u0::stm32u083::kPwrRegisterWidthBits,
                                     ohal::core::Access::WriteOnly>;
    using Pucra = typename Regs::Pucra;
    using Pdcra = typename Regs::Pdcra;
    using Pucrb = typename Regs::Pucrb;
    using Pdcrb = typename Regs::Pdcrb;
    using Pucrc = typename Regs::Pucrc;
    using Pdcrc = typename Regs::Pdcrc;
    using Pucrd = typename Regs::Pucrd;
    using Pdcrd = typename Regs::Pdcrd;
    using Pucre = typename Regs::Pucre;
    using Pdcre = typename Regs::Pdcre;
    using Pucrf = typename Regs::Pucrf;
    using Pdcrf = typename Regs::Pdcrf;
};

} // namespace ohal::pwr

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_PWR_HPP
