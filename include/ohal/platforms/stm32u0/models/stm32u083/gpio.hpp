#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_GPIO_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_GPIO_HPP

#include <ohal/core/register.hpp>

#include <cstdint>

namespace ohal::platforms::stm32u0::stm32u083 {

/// Base addresses for each GPIO port on the STM32U083 (RM0503 §2.2.2).
inline constexpr uintptr_t kGpioABase = 0x42020000U;
inline constexpr uintptr_t kGpioBBase = 0x42020400U;
inline constexpr uintptr_t kGpioCBase = 0x42020800U;
inline constexpr uintptr_t kGpioDBase = 0x42020C00U;
inline constexpr uintptr_t kGpioEBase = 0x42021000U;
inline constexpr uintptr_t kGpioFBase = 0x42021400U;

/// Register offsets within each GPIO peripheral block (identical for all STM32U0 GPIO ports).
inline constexpr uintptr_t kModerOffset = 0x00U;
inline constexpr uintptr_t kOtyperOffset = 0x04U;
inline constexpr uintptr_t kOspeedrOffset = 0x08U;
inline constexpr uintptr_t kPupdrOffset = 0x0CU;
inline constexpr uintptr_t kIdrOffset = 0x10U;
inline constexpr uintptr_t kOdrOffset = 0x14U;
inline constexpr uintptr_t kBsrrOffset = 0x18U;
inline constexpr uintptr_t kLckrOffset = 0x1CU;
inline constexpr uintptr_t kAfrlOffset = 0x20U;
inline constexpr uintptr_t kAfrhOffset = 0x24U;

/// All registers for a single GPIO port, parameterised by the port's base address.
///
/// Each member type is a zero-RAM ohal::core::Register<> specialisation that models one
/// 32-bit memory-mapped register.  The offsets follow RM0503 Table 66.
template <uintptr_t Base>
struct GpioPort {
  using MODER = ohal::core::Register<Base + kModerOffset>;
  using OTYPER = ohal::core::Register<Base + kOtyperOffset>;
  using OSPEEDR = ohal::core::Register<Base + kOspeedrOffset>;
  using PUPDR = ohal::core::Register<Base + kPupdrOffset>;
  using IDR = ohal::core::Register<Base + kIdrOffset>;
  using ODR = ohal::core::Register<Base + kOdrOffset>;
  using BSRR = ohal::core::Register<Base + kBsrrOffset>;
  using LCKR = ohal::core::Register<Base + kLckrOffset>;
  using AFRL = ohal::core::Register<Base + kAfrlOffset>;
  using AFRH = ohal::core::Register<Base + kAfrhOffset>;
};

/// Concrete GPIO port types for all six ports present on the STM32U083.
using GpioA = GpioPort<kGpioABase>;
using GpioB = GpioPort<kGpioBBase>;
using GpioC = GpioPort<kGpioCBase>;
using GpioD = GpioPort<kGpioDBase>;
using GpioE = GpioPort<kGpioEBase>;
using GpioF = GpioPort<kGpioFBase>;

} // namespace ohal::platforms::stm32u0::stm32u083

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_GPIO_HPP
