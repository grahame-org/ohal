#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_GPIO_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_GPIO_HPP

#include <cstdint>

#include "ohal/core/register.hpp"

namespace ohal::platforms::stm32u0::stm32u083 {

// GPIO register offsets within each port block (RM0503 Rev 4, Table 41).
// All offsets are identical across every STM32U0 GPIO port.
inline constexpr uintptr_t kModerOffset = 0x00U;   ///< Pin-mode register
inline constexpr uintptr_t kOtyperOffset = 0x04U;  ///< Output-type register
inline constexpr uintptr_t kOspeedrOffset = 0x08U; ///< Output-speed register
inline constexpr uintptr_t kPupdrOffset = 0x0CU;   ///< Pull-up/pull-down register
inline constexpr uintptr_t kIdrOffset = 0x10U;     ///< Input data register
inline constexpr uintptr_t kOdrOffset = 0x14U;     ///< Output data register
inline constexpr uintptr_t kBsrrOffset = 0x18U;    ///< Bit set/reset register
inline constexpr uintptr_t kLckrOffset = 0x1CU;    ///< Configuration lock register
inline constexpr uintptr_t kAfrlOffset = 0x20U;    ///< Alternate-function low register (pins 0-7)
inline constexpr uintptr_t kAfrhOffset = 0x24U;    ///< Alternate-function high register (pins 8-15)
inline constexpr uintptr_t kBrrOffset = 0x28U;     ///< Bit reset register

// GPIO port base addresses (RM0503 Rev 4, Table 4)
inline constexpr uintptr_t kGpioABase = 0x5000'0000U;
inline constexpr uintptr_t kGpioBBase = 0x5000'0400U;
inline constexpr uintptr_t kGpioCBase = 0x5000'0800U;
inline constexpr uintptr_t kGpioDBase = 0x5000'0C00U;
inline constexpr uintptr_t kGpioEBase = 0x5000'1000U;
inline constexpr uintptr_t kGpioFBase = 0x5000'1400U;

/// Register map for one STM32U083 GPIO port.
///
/// Instantiate with the port base address from the STM32U083 Reference Manual
/// (RM0503 Rev 4, Table 4). All register offsets are identical across every
/// STM32U0 GPIO port.
///
/// @tparam Base  Physical base address of the GPIO port (e.g. kGpioABase for GPIOA).
template <uintptr_t Base>
struct GpioPortRegs {
  using Moder = ohal::core::Register<Base + kModerOffset>;     ///< Pin-mode register (RW)
  using Otyper = ohal::core::Register<Base + kOtyperOffset>;   ///< Output-type register (RW)
  using Ospeedr = ohal::core::Register<Base + kOspeedrOffset>; ///< Output-speed register (RW)
  using Pupdr = ohal::core::Register<Base + kPupdrOffset>;     ///< Pull-up/pull-down register (RW)
  using Idr = ohal::core::Register<Base + kIdrOffset>;         ///< Input data register (RO)
  using Odr = ohal::core::Register<Base + kOdrOffset>;         ///< Output data register (RW)
  using Bsrr = ohal::core::Register<Base + kBsrrOffset>;       ///< Bit set/reset register (WO)
  using Lckr = ohal::core::Register<Base + kLckrOffset>;       ///< Configuration lock register (RW)
  using Afrl = ohal::core::Register<Base + kAfrlOffset>;       ///< AF low register (pins 0-7, RW)
  using Afrh = ohal::core::Register<Base + kAfrhOffset>;       ///< AF high register (pins 8-15, RW)
  using Brr = ohal::core::Register<Base + kBrrOffset>;         ///< Bit reset register (WO)
};

using GpioA = GpioPortRegs<kGpioABase>;
using GpioB = GpioPortRegs<kGpioBBase>;
using GpioC = GpioPortRegs<kGpioCBase>;
using GpioD = GpioPortRegs<kGpioDBase>;
using GpioE = GpioPortRegs<kGpioEBase>;
using GpioF = GpioPortRegs<kGpioFBase>;

} // namespace ohal::platforms::stm32u0::stm32u083

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_GPIO_HPP
