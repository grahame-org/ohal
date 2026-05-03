#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083RC_GPIO_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083RC_GPIO_HPP

// GPIO Pin<>/Port<> specialisations for the STM32U083RC 64-pin packages
// (LQFP64 and UFBGA64).
//
// Both RC packages expose GPIOA, GPIOB, GPIOC, GPIOD and GPIOF; GPIOE is not
// bonded out (DS14463, §4.1).  The underlying register map (base addresses,
// offsets, GpioPortPinImpl, GpioPortImpl) is shared with all STM32U083
// variants via gpio_impl.hpp.

#include "ohal/platforms/stm32u0/models/stm32u083/gpio_impl.hpp"

namespace ohal::gpio {

// Bonded-out ports: GPIOA, GPIOB, GPIOC, GPIOD, GPIOF
template <uint8_t PinNum>
struct Pin<PortA, PinNum> : ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<
                                PinNum, ohal::platforms::stm32u0::stm32u083::GpioA> {};

template <uint8_t PinNum>
struct Pin<PortB, PinNum> : ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<
                                PinNum, ohal::platforms::stm32u0::stm32u083::GpioB> {};

template <uint8_t PinNum>
struct Pin<PortC, PinNum> : ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<
                                PinNum, ohal::platforms::stm32u0::stm32u083::GpioC> {};

template <uint8_t PinNum>
struct Pin<PortD, PinNum> : ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<
                                PinNum, ohal::platforms::stm32u0::stm32u083::GpioD> {};

template <uint8_t PinNum>
struct Pin<PortF, PinNum> : ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<
                                PinNum, ohal::platforms::stm32u0::stm32u083::GpioF> {};

// Not bonded out on the 64-pin RC packages: GPIOE.
// Explicit specialisation produces a descriptive error at the point of use.
template <uint8_t PinNum>
struct Pin<PortE, PinNum> {
  static_assert(detail::always_false<PinNum>::value,
                "ohal: GPIOE is not bonded out on this STM32U083 package.");
};

template <>
struct Port<PortA> : ohal::platforms::stm32u0::stm32u083::GpioPortImpl<
                         ohal::platforms::stm32u0::stm32u083::GpioA> {};

template <>
struct Port<PortB> : ohal::platforms::stm32u0::stm32u083::GpioPortImpl<
                         ohal::platforms::stm32u0::stm32u083::GpioB> {};

template <>
struct Port<PortC> : ohal::platforms::stm32u0::stm32u083::GpioPortImpl<
                         ohal::platforms::stm32u0::stm32u083::GpioC> {};

template <>
struct Port<PortD> : ohal::platforms::stm32u0::stm32u083::GpioPortImpl<
                         ohal::platforms::stm32u0::stm32u083::GpioD> {};

template <>
struct Port<PortF> : ohal::platforms::stm32u0::stm32u083::GpioPortImpl<
                         ohal::platforms::stm32u0::stm32u083::GpioF> {};

// Not bonded out on the 64-pin RC packages: GPIOE.
// Member functions are deleted so that any attempt to call them produces a
// clear compiler diagnostic rather than the primary template's generic error.
template <>
struct Port<PortE> {
  static void set(uint16_t) = delete;             // GPIOE not bonded out on this package
  static void clear(uint16_t) = delete;           // GPIOE not bonded out on this package
  static void write(uint16_t, uint16_t) = delete; // GPIOE not bonded out on this package
};

} // namespace ohal::gpio

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083RC_GPIO_HPP
