#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_GPIO_IMPL_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_GPIO_IMPL_HPP

// Shared GPIO register map for all STM32U083 package variants.
//
// This header provides the register address constants, GpioPortRegs<Base>,
// GpioPortPinImpl<PinNum, Regs>, and GpioPortImpl<Regs> types that are
// common across every STM32U083 package.  It does NOT provide the
// ohal::gpio::Pin<> / Port<> partial specialisations, because those differ
// per package (bonded-out port set varies).  Package-specific gpio.hpp
// headers include this file and then provide their own specialisations.

#include <cstdint>
#include <type_traits>

#include "ohal/core/access.hpp"
#include "ohal/core/field.hpp"
#include "ohal/core/register.hpp"
#include "ohal/gpio.hpp"

namespace ohal::platforms::stm32u0::stm32u083
{

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

/// Number of pins per STM32U083 GPIO port (0–15).
inline constexpr uint8_t kStm32u083PinCount = 16U;
/// BSRR bit offset of the reset half: writing a 1 to bit (16 + n) clears pin n.
inline constexpr uint8_t kBsrrResetBitStart = 16U;

/// Register map for one STM32U083 GPIO port.
///
/// Instantiate with the port base address from the STM32U083 Reference Manual
/// (RM0503 Rev 4, Table 4). All register offsets are identical across every
/// STM32U0 GPIO port.
///
/// @tparam Base  Physical base address of the GPIO port (e.g. kGpioABase for GPIOA).
template <uintptr_t Base>
struct GpioPortRegs
{
    using Moder = ohal::core::Register<Base + kModerOffset>;     ///< Pin-mode register (RW)
    using Otyper = ohal::core::Register<Base + kOtyperOffset>;   ///< Output-type register (RW)
    using Ospeedr = ohal::core::Register<Base + kOspeedrOffset>; ///< Output-speed register (RW)
    using Pupdr = ohal::core::Register<Base + kPupdrOffset>; ///< Pull-up/pull-down register (RW)
    using Idr = ohal::core::Register<Base + kIdrOffset>;     ///< Input data register (RO)
    using Odr = ohal::core::Register<Base + kOdrOffset>;     ///< Output data register (RW)
    using Bsrr = ohal::core::Register<Base + kBsrrOffset>;   ///< Bit set/reset register (WO)
    using Lckr = ohal::core::Register<Base + kLckrOffset>;   ///< Configuration lock register (RW)
    using Afrl = ohal::core::Register<Base + kAfrlOffset>;   ///< AF low register (pins 0-7, RW)
    using Afrh = ohal::core::Register<Base + kAfrhOffset>;   ///< AF high register (pins 8-15, RW)
    using Brr = ohal::core::Register<Base + kBrrOffset>;     ///< Bit reset register (WO)
};

using GpioA = GpioPortRegs<kGpioABase>;
using GpioB = GpioPortRegs<kGpioBBase>;
using GpioC = GpioPortRegs<kGpioCBase>;
using GpioD = GpioPortRegs<kGpioDBase>;
using GpioE = GpioPortRegs<kGpioEBase>;
using GpioF = GpioPortRegs<kGpioFBase>;

/// Implements the ohal::gpio::Pin<Port, PinNum> API for one STM32U083 GPIO port.
///
/// Parametrised on @p Regs so that host-side tests can inject a mock register set
/// (using ohal::test::MockRegister types) without modifying the real hardware header.
/// Production code uses GpioA…GpioF (GpioPortRegs<Base>) as the @p Regs argument.
///
/// @tparam PinNum  Zero-based pin number within the port (0–15).
/// @tparam Regs    A type whose nested type aliases (Moder, Otyper, …) satisfy the
///                 ohal::core::Register or MockRegister interface (read() / write()).
template <uint8_t PinNum, typename Regs>
struct GpioPortPinImpl
{
    static_assert(PinNum < kStm32u083PinCount, "ohal: STM32U083 GPIO has pins 0-15 only.");

    // Bit-field descriptors — one per relevant GPIO register.
    // MODER: 2 bits per pin, RW, encodes the pin direction/function.
    using Moder = ohal::core::BitField<typename Regs::Moder, static_cast<uint8_t>(PinNum * 2U), 2U,
                                       ohal::core::Access::ReadWrite, ohal::gpio::PinMode>;
    // OTYPER: 1 bit per pin, RW, encodes push-pull vs open-drain.
    using Otyper = ohal::core::BitField<typename Regs::Otyper, PinNum, 1U,
                                        ohal::core::Access::ReadWrite, ohal::gpio::OutputType>;
    // OSPEEDR: 2 bits per pin, RW, encodes output switching speed.
    using Ospeedr = ohal::core::BitField<typename Regs::Ospeedr, static_cast<uint8_t>(PinNum * 2U),
                                         2U, ohal::core::Access::ReadWrite, ohal::gpio::Speed>;
    // PUPDR: 2 bits per pin, RW, encodes pull-up / pull-down / none.
    using Pupdr = ohal::core::BitField<typename Regs::Pupdr, static_cast<uint8_t>(PinNum * 2U), 2U,
                                       ohal::core::Access::ReadWrite, ohal::gpio::Pull>;
    // IDR: 1 bit per pin, RO, reflects the pin input level.
    using Idr = ohal::core::BitField<typename Regs::Idr, PinNum, 1U, ohal::core::Access::ReadOnly,
                                     ohal::gpio::Level>;
    // ODR: 1 bit per pin, RW, drives the output latch.
    using Odr = ohal::core::BitField<typename Regs::Odr, PinNum, 1U, ohal::core::Access::ReadWrite,
                                     ohal::gpio::Level>;
    // BSRR bits 0-15: write 1 to atomically drive the pin high (write-only).
    using BsrrSet =
        ohal::core::BitField<typename Regs::Bsrr, PinNum, 1U, ohal::core::Access::WriteOnly>;
    // BSRR bits 16-31: write 1 to atomically drive the pin low (write-only).
    using BsrrReset =
        ohal::core::BitField<typename Regs::Bsrr, static_cast<uint8_t>(PinNum + kBsrrResetBitStart),
                             1U, ohal::core::Access::WriteOnly>;

    // AFR: 4 bits per pin in AFRL (pins 0–7) or AFRH (pins 8–15).
    // kAfrHighPinStart: first pin number stored in AFRH (pins 8–15).
    // kAfrShift is the bit-offset of the pin's AF field within its AFR register.
    static constexpr uint8_t kAfrHighPinStart = 8U;
    static constexpr uint8_t kAfrShift = static_cast<uint8_t>((PinNum % kAfrHighPinStart) * 4U);
    static constexpr uint32_t kAfrMask = 0xFUL << kAfrShift;
    static constexpr uint8_t kAfrFieldMask = 0xFU; // 4-bit alternate-function field mask

    static void set_mode(ohal::gpio::PinMode mode) noexcept { Moder::write(mode); }
    static void set_output_type(ohal::gpio::OutputType output_type) noexcept
    {
        Otyper::write(output_type);
    }
    static void set_speed(ohal::gpio::Speed speed) noexcept { Ospeedr::write(speed); }
    static void set_pull(ohal::gpio::Pull pull) noexcept { Pupdr::write(pull); }

    /// Selects the alternate function for the pin by writing the 4-bit @p alt_func value
    /// into AFRL (pins 0–7) or AFRH (pins 8–15). Valid values are 0–15.
    static void set_alternate_function(uint8_t alt_func) noexcept
    {
        const uint32_t encoded = (static_cast<uint32_t>(alt_func) & kAfrFieldMask) << kAfrShift;
        if constexpr (PinNum < kAfrHighPinStart)
        {
            Regs::Afrl::write((Regs::Afrl::read() & ~kAfrMask) | encoded);
        }
        else
        {
            Regs::Afrh::write((Regs::Afrh::read() & ~kAfrMask) | encoded);
        }
    }

    /// Drives the pin high via BSRR — a single, atomic 32-bit write with no read.
    static void set() noexcept { BsrrSet::write(1U); }

    /// Drives the pin low via BSRR — a single, atomic 32-bit write with no read.
    static void clear() noexcept { BsrrReset::write(1U); }

    /// Returns the sampled input level from IDR.
    [[nodiscard]] static ohal::gpio::Level read_input() noexcept { return Idr::read(); }

    /// Returns the current output latch value from ODR.
    [[nodiscard]] static ohal::gpio::Level read_output() noexcept { return Odr::read(); }

    /// Toggles the output: reads ODR then drives the opposite level via BSRR.
    static void toggle() noexcept
    {
        if (read_output() == ohal::gpio::Level::Low)
        {
            set();
        }
        else
        {
            clear();
        }
    }
};

/// Implements the ohal::gpio::Port<PortTag> API for one STM32U083 GPIO port.
///
/// All three operations write directly to the BSRR register — no read before
/// write.  BSRR bits 0–15 atomically set the corresponding output pins; bits
/// 16–31 atomically reset them.  Writing both halves in a single 32-bit store
/// (as @c write() does) is therefore a single atomic bus transaction with no
/// intermediate hardware state visible to interrupts or DMA.
///
/// Parametrised on @p Regs so that host-side tests can inject a mock register
/// set (using ohal::test::MockRegister types) without modifying the real
/// hardware header.  Production code uses GpioA…GpioF (GpioPortRegs<Base>)
/// as the @p Regs argument.
///
/// @tparam Regs  A type whose Bsrr nested type alias satisfies the Register
///               or MockRegister interface (write()).
template <typename Regs>
struct GpioPortImpl
{
    /// Exposed so host-test wiring checks can inspect the BSRR register address.
    using BsrrReg = typename Regs::Bsrr;

    /// Drives every pin whose bit is set in @p mask high — a single BSRR write,
    /// no read.
    static void set(uint16_t mask) noexcept { Regs::Bsrr::write(static_cast<uint32_t>(mask)); }

    /// Drives every pin whose bit is set in @p mask low — a single BSRR write,
    /// no read.
    static void clear(uint16_t mask) noexcept
    {
        Regs::Bsrr::write(static_cast<uint32_t>(mask) << kBsrrResetBitStart);
    }

    /// Drives the pins in @p set_mask high and the pins in @p clear_mask low in
    /// a single BSRR write — no intermediate hardware state is visible.
    static void write(uint16_t set_mask, uint16_t clear_mask) noexcept
    {
        Regs::Bsrr::write(static_cast<uint32_t>(set_mask) |
                          (static_cast<uint32_t>(clear_mask) << kBsrrResetBitStart));
    }
};

} // namespace ohal::platforms::stm32u0::stm32u083

namespace ohal::gpio::detail
{
/// Helper that is always false but depends on a template parameter, so that
/// static_assert can be deferred to instantiation time (rather than firing at
/// class-template definition time, which is a risk for non-dependent expressions).
template <uint8_t>
struct always_false : std::false_type
{
};
} // namespace ohal::gpio::detail

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_GPIO_IMPL_HPP
