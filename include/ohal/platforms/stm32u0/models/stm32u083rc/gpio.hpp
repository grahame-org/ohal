#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083RC_GPIO_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083RC_GPIO_HPP

// Shared GPIO Pin<>/Port<> specialisations for STM32U083RC 64-pin packages:
//   STM32U083RCT (LQFP64) and STM32U083RCI (UFBGA64).
//
// Both packages share the same GPIO bonding:
//   GPIOA (bits 0–15), GPIOB (bits 0–15), GPIOC (bits 0–15),
//   GPIOD (PD2 only), GPIOF (PF0–PF3 only).
// GPIOE is not bonded out on either package (DS14463, §4.1).
//
// This header is included by the thin per-package wrapper headers
// stm32u083rct/gpio.hpp and stm32u083rci/gpio.hpp.

#include "ohal/platforms/stm32u0/models/stm32u083/gpio_impl.hpp"

namespace ohal::gpio
{

// Bonded-out ports with full pin ranges: GPIOA, GPIOB, GPIOC
template <uint8_t PinNum>
struct Pin<PortA, PinNum> : ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<
                                PinNum, ohal::platforms::stm32u0::stm32u083::GpioA>
{
};

template <uint8_t PinNum>
struct Pin<PortB, PinNum> : ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<
                                PinNum, ohal::platforms::stm32u0::stm32u083::GpioB>
{
};

template <uint8_t PinNum>
struct Pin<PortC, PinNum> : ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<
                                PinNum, ohal::platforms::stm32u0::stm32u083::GpioC>
{
};

// GPIOD: only PD2 is bonded out on the 64-pin packages.
// PD2 resolves to the full implementation; all other PD pins are absent.
template <uint8_t PinNum>
struct Pin<PortD, PinNum> : ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<
                                PinNum, ohal::platforms::stm32u0::stm32u083::GpioD>
{
};

// GPIOF: only PF0–PF3 are bonded out on the 64-pin packages.
// PF0–PF3 resolve to the full implementation; all other PF pins are absent.
template <uint8_t PinNum>
struct Pin<PortF, PinNum> : ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<
                                PinNum, ohal::platforms::stm32u0::stm32u083::GpioF>
{
};

// Not bonded out on either 64-pin package: GPIOE.
// Explicit specialisation produces a descriptive error at the point of use.
template <uint8_t PinNum>
struct Pin<PortE, PinNum>
{
    static_assert(detail::always_false<PinNum>::value,
                  "ohal: GPIOE is not bonded out on this STM32U083 package.");
};

template <>
struct Port<PortA>
    : ohal::platforms::stm32u0::stm32u083::GpioPortImpl<ohal::platforms::stm32u0::stm32u083::GpioA>
{
};

template <>
struct Port<PortB>
    : ohal::platforms::stm32u0::stm32u083::GpioPortImpl<ohal::platforms::stm32u0::stm32u083::GpioB>
{
};

template <>
struct Port<PortC>
    : ohal::platforms::stm32u0::stm32u083::GpioPortImpl<ohal::platforms::stm32u0::stm32u083::GpioC>
{
};

template <>
struct Port<PortD>
    : ohal::platforms::stm32u0::stm32u083::GpioPortImpl<ohal::platforms::stm32u0::stm32u083::GpioD>
{
};

template <>
struct Port<PortF>
    : ohal::platforms::stm32u0::stm32u083::GpioPortImpl<ohal::platforms::stm32u0::stm32u083::GpioF>
{
};

// Not bonded out on either 64-pin package: GPIOE.
// Member functions are deleted so that any attempt to call them produces a
// clear compiler diagnostic rather than the primary template's generic error.
template <>
struct Port<PortE>
{
    static void set(uint16_t) = delete;             // GPIOE not bonded out on this package
    static void clear(uint16_t) = delete;           // GPIOE not bonded out on this package
    static void write(uint16_t, uint16_t) = delete; // GPIOE not bonded out on this package
};

} // namespace ohal::gpio

// ---------------------------------------------------------------------------
// Compile-time guards for non-bonded pins within partially-bonded ports.
//
// Pin<PortD, PinNum> covers all PinNum via the partial specialisation above,
// but only PD2 is physically bonded on the 64-pin packages.  The explicit
// full specialisations below override the partial spec for every non-bonded
// PD and PF pin with deleted member functions, producing a clear diagnostic
// for any attempt to use a pad that does not exist on these packages.
// ---------------------------------------------------------------------------

namespace ohal::gpio
{

namespace detail
{
/// Helper base for a GPIO pin that is not bonded out on the STM32U083RC 64-pin packages.
/// All member functions are deleted so that any attempt to call them produces
/// a "call to deleted function" error pointing at the non-bonded pad.
struct NotBondedRc
{
    static void set_mode(PinMode) noexcept = delete;           ///< pin not bonded out (RC)
    static void set_output_type(OutputType) noexcept = delete; ///< pin not bonded out (RC)
    static void set_speed(Speed) noexcept = delete;            ///< pin not bonded out (RC)
    static void set_pull(Pull) noexcept = delete;              ///< pin not bonded out (RC)
    static void set() noexcept = delete;                       ///< pin not bonded out (RC)
    static void clear() noexcept = delete;                     ///< pin not bonded out (RC)
    static Level read_input() noexcept = delete;               ///< pin not bonded out (RC)
    static Level read_output() noexcept = delete;              ///< pin not bonded out (RC)
    static void toggle() noexcept = delete;                    ///< pin not bonded out (RC)
};
} // namespace detail

// NOLINTBEGIN(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

// GPIOD: only PD2 is bonded out on the STM32U083RC 64-pin packages.
// All other PD pins are absent on these packages.
template <>
struct Pin<PortD, 0> : detail::NotBondedRc
{
}; ///< PD0  not bonded (RC)
template <>
struct Pin<PortD, 1> : detail::NotBondedRc
{
}; ///< PD1  not bonded (RC)
// PD2 is bonded out — no override, uses the partial specialisation above.
template <>
struct Pin<PortD, 3> : detail::NotBondedRc
{
}; ///< PD3  not bonded (RC)
template <>
struct Pin<PortD, 4> : detail::NotBondedRc
{
}; ///< PD4  not bonded (RC)
template <>
struct Pin<PortD, 5> : detail::NotBondedRc
{
}; ///< PD5  not bonded (RC)
template <>
struct Pin<PortD, 6> : detail::NotBondedRc
{
}; ///< PD6  not bonded (RC)
template <>
struct Pin<PortD, 7> : detail::NotBondedRc
{
}; ///< PD7  not bonded (RC)
template <>
struct Pin<PortD, 8> : detail::NotBondedRc
{
}; ///< PD8  not bonded (RC)
template <>
struct Pin<PortD, 9> : detail::NotBondedRc
{
}; ///< PD9  not bonded (RC)
template <>
struct Pin<PortD, 10> : detail::NotBondedRc
{
}; ///< PD10 not bonded (RC)
template <>
struct Pin<PortD, 11> : detail::NotBondedRc
{
}; ///< PD11 not bonded (RC)
template <>
struct Pin<PortD, 12> : detail::NotBondedRc
{
}; ///< PD12 not bonded (RC)
template <>
struct Pin<PortD, 13> : detail::NotBondedRc
{
}; ///< PD13 not bonded (RC)
template <>
struct Pin<PortD, 14> : detail::NotBondedRc
{
}; ///< PD14 not bonded (RC)
template <>
struct Pin<PortD, 15> : detail::NotBondedRc
{
}; ///< PD15 not bonded (RC)

// GPIOF: only PF0–PF3 are bonded out on the STM32U083RC 64-pin packages.
// Bits 4–15 are absent on these packages.
template <>
struct Pin<PortF, 4> : detail::NotBondedRc
{
}; ///< PF4  not bonded (RC)
template <>
struct Pin<PortF, 5> : detail::NotBondedRc
{
}; ///< PF5  not bonded (RC)
template <>
struct Pin<PortF, 6> : detail::NotBondedRc
{
}; ///< PF6  not bonded (RC)
template <>
struct Pin<PortF, 7> : detail::NotBondedRc
{
}; ///< PF7  not bonded (RC)
template <>
struct Pin<PortF, 8> : detail::NotBondedRc
{
}; ///< PF8  not bonded (RC)
template <>
struct Pin<PortF, 9> : detail::NotBondedRc
{
}; ///< PF9  not bonded (RC)
template <>
struct Pin<PortF, 10> : detail::NotBondedRc
{
}; ///< PF10 not bonded (RC)
template <>
struct Pin<PortF, 11> : detail::NotBondedRc
{
}; ///< PF11 not bonded (RC)
template <>
struct Pin<PortF, 12> : detail::NotBondedRc
{
}; ///< PF12 not bonded (RC)
template <>
struct Pin<PortF, 13> : detail::NotBondedRc
{
}; ///< PF13 not bonded (RC)
template <>
struct Pin<PortF, 14> : detail::NotBondedRc
{
}; ///< PF14 not bonded (RC)
template <>
struct Pin<PortF, 15> : detail::NotBondedRc
{
}; ///< PF15 not bonded (RC)

// NOLINTEND(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

} // namespace ohal::gpio

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083RC_GPIO_HPP
