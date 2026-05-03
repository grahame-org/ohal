#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_GPIO_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_GPIO_HPP

// The STM32U083KCU (32-pin UFQFPN) and STM32U083HCY (42-ball WLCSP) expose
// only GPIOA, GPIOB, GPIOC and GPIOF.  GPIOD and GPIOE exist in silicon but
// are not bonded out on these packages.
//
// This header pulls in the shared STM32U083 register map and then provides
// Pin<>/Port<> specialisations for the bonded-out ports (A/B/C/F) plus
// diagnostic-only specialisations for the unbonded ports (D/E) that produce
// a clear "not bonded out on this package" error rather than a generic one.

#include "ohal/platforms/stm32u0/models/stm32u083/gpio_impl.hpp"

// ---------------------------------------------------------------------------
// ohal::gpio::Pin<> and Port<> specialisations for STM32U083 packages where
// only GPIOA, GPIOB, GPIOC and GPIOF are bonded out (e.g. STM32U083KCU
// 32-pin UFQFPN, STM32U083HCY 42-ball WLCSP, STM32U083CCI 48-pin UFQFPN,
// and STM32U083CCT 48-pin LQFP).
//
// GPIOD and GPIOE exist in silicon but are not bonded out on these packages.
// Explicit Pin<PortD/E> and Port<PortD/E> specialisations are provided below
// so that using these ports produces a clear "not bonded out on this package"
// diagnostic rather than the primary template's generic "not implemented" message.
// ---------------------------------------------------------------------------

namespace ohal::gpio {

// Bonded-out ports: GPIOA, GPIOB, GPIOC, GPIOF
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
struct Pin<PortF, PinNum> : ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<
                                PinNum, ohal::platforms::stm32u0::stm32u083::GpioF> {};

// Not bonded out on packages with only A/B/C/F: GPIOD, GPIOE
// Explicit specialisations produce a descriptive error at the point of use.
template <uint8_t PinNum>
struct Pin<PortD, PinNum> {
  static_assert(detail::always_false<PinNum>::value,
                "ohal: GPIOD is not bonded out on this STM32U083 package.");
};

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
struct Port<PortF> : ohal::platforms::stm32u0::stm32u083::GpioPortImpl<
                         ohal::platforms::stm32u0::stm32u083::GpioF> {};

// Not bonded out on packages with only A/B/C/F: GPIOD, GPIOE.
// Member functions are deleted so that any attempt to call them produces a
// clear compiler diagnostic rather than the primary template's generic error.
template <>
struct Port<PortD> {
  static void set(uint16_t) = delete;             // GPIOD not bonded out on this package
  static void clear(uint16_t) = delete;           // GPIOD not bonded out on this package
  static void write(uint16_t, uint16_t) = delete; // GPIOD not bonded out on this package
};

template <>
struct Port<PortE> {
  static void set(uint16_t) = delete;             // GPIOE not bonded out on this package
  static void clear(uint16_t) = delete;           // GPIOE not bonded out on this package
  static void write(uint16_t, uint16_t) = delete; // GPIOE not bonded out on this package
};

} // namespace ohal::gpio

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_GPIO_HPP
