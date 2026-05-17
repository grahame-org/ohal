#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_EXTI_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_EXTI_HPP

#include <cstdint>
#include <type_traits>

#include "ohal/core/register.hpp"
#include "ohal/exti.hpp"
#include "ohal/gpio.hpp"
#include "ohal/platforms/stm32u0/family_tag.hpp"

namespace ohal::platforms::stm32u0::stm32u083
{

inline constexpr uintptr_t kExtiBase = 0x4002'1800U;
inline constexpr uintptr_t kExtiImr1Offset = 0x00U;
inline constexpr uintptr_t kExtiEmr1Offset = 0x04U;
inline constexpr uintptr_t kExtiRtsr1Offset = 0x08U;
inline constexpr uintptr_t kExtiFtsr1Offset = 0x0CU;
inline constexpr uintptr_t kExtiSwier1Offset = 0x10U;
inline constexpr uintptr_t kExtiRpr1Offset = 0x14U;
inline constexpr uintptr_t kExtiFpr1Offset = 0x18U;

inline constexpr uintptr_t kSyscfgBase = 0x4001'0000U;
inline constexpr uintptr_t kSyscfgExticr1Offset = 0x08U;

} // namespace ohal::platforms::stm32u0::stm32u083

namespace ohal::exti
{

template <>
struct has_exti<ohal::platforms::stm32u0::Family> : std::true_type
{
};

namespace detail
{

inline constexpr uint8_t kGpioExtiLineCount = 16U;
inline constexpr uint8_t kPortCodeA = 0U;
inline constexpr uint8_t kPortCodeB = 1U;
inline constexpr uint8_t kPortCodeC = 2U;
inline constexpr uint8_t kPortCodeD = 3U;
inline constexpr uint8_t kPortCodeE = 4U;
inline constexpr uint8_t kPortCodeF = 5U;

template <uint8_t PortCode, uint8_t PinNum>
struct Stm32u0ExtiGpioLine
{
    static_assert(PinNum < kGpioExtiLineCount,
                  "ohal: STM32U0 EXTI GPIO line is valid only for pins 0-15.");

    static constexpr uint32_t kLineMask = 1UL << PinNum;
    static constexpr uint32_t kExticrIndex = PinNum / 4U;
    static constexpr uint32_t kExticrShift = (PinNum % 4U) * 4U;
    static constexpr uint32_t kExticrMask = 0xFUL << kExticrShift;
    static constexpr uint32_t kPortCode = PortCode;

    using Imr1 = ohal::core::Register<ohal::platforms::stm32u0::stm32u083::kExtiBase +
                                      ohal::platforms::stm32u0::stm32u083::kExtiImr1Offset>;
    using Emr1 = ohal::core::Register<ohal::platforms::stm32u0::stm32u083::kExtiBase +
                                      ohal::platforms::stm32u0::stm32u083::kExtiEmr1Offset>;
    using Rtsr1 = ohal::core::Register<ohal::platforms::stm32u0::stm32u083::kExtiBase +
                                       ohal::platforms::stm32u0::stm32u083::kExtiRtsr1Offset>;
    using Ftsr1 = ohal::core::Register<ohal::platforms::stm32u0::stm32u083::kExtiBase +
                                       ohal::platforms::stm32u0::stm32u083::kExtiFtsr1Offset>;
    using Swier1 = ohal::core::Register<ohal::platforms::stm32u0::stm32u083::kExtiBase +
                                        ohal::platforms::stm32u0::stm32u083::kExtiSwier1Offset>;
    using Rpr1 = ohal::core::Register<ohal::platforms::stm32u0::stm32u083::kExtiBase +
                                      ohal::platforms::stm32u0::stm32u083::kExtiRpr1Offset>;
    using Fpr1 = ohal::core::Register<ohal::platforms::stm32u0::stm32u083::kExtiBase +
                                      ohal::platforms::stm32u0::stm32u083::kExtiFpr1Offset>;
    using Exticr = ohal::core::Register<ohal::platforms::stm32u0::stm32u083::kSyscfgBase +
                                        ohal::platforms::stm32u0::stm32u083::kSyscfgExticr1Offset +
                                        (kExticrIndex * sizeof(uint32_t))>;

    static void configure(Trigger trigger) noexcept
    {
        configure_port_route();

        uint32_t rising = Rtsr1::read();
        uint32_t falling = Ftsr1::read();

        switch (trigger)
        {
        case Trigger::Rising:
            rising |= kLineMask;
            falling &= ~kLineMask;
            break;
        case Trigger::Falling:
            rising &= ~kLineMask;
            falling |= kLineMask;
            break;
        case Trigger::RisingFalling:
            rising |= kLineMask;
            falling |= kLineMask;
            break;
        default:
            break;
        }

        Rtsr1::write(rising);
        Ftsr1::write(falling);
    }

    static void enable_interrupt() noexcept { Imr1::write(Imr1::read() | kLineMask); }

    static void disable_interrupt() noexcept { Imr1::write(Imr1::read() & ~kLineMask); }

    static void enable_event() noexcept { Emr1::write(Emr1::read() | kLineMask); }

    [[nodiscard]] static bool is_pending_rising() noexcept
    {
        return (Rpr1::read() & kLineMask) != 0U;
    }

    [[nodiscard]] static bool is_pending_falling() noexcept
    {
        return (Fpr1::read() & kLineMask) != 0U;
    }

    static void clear_pending_rising() noexcept { Rpr1::write(kLineMask); }

    static void clear_pending_falling() noexcept { Fpr1::write(kLineMask); }

    static void trigger_software() noexcept { Swier1::write(kLineMask); }

  private:
    static void configure_port_route() noexcept
    {
        const uint32_t value = Exticr::read();
        const uint32_t cleaned = value & ~kExticrMask;
        const uint32_t encoded = (kPortCode << kExticrShift) & kExticrMask;
        Exticr::write(cleaned | encoded);
    }
};

} // namespace detail

template <uint8_t PinNum>
struct Line<ohal::gpio::PortA, PinNum> : detail::Stm32u0ExtiGpioLine<detail::kPortCodeA, PinNum>
{
};

template <uint8_t PinNum>
struct Line<ohal::gpio::PortB, PinNum> : detail::Stm32u0ExtiGpioLine<detail::kPortCodeB, PinNum>
{
};

template <uint8_t PinNum>
struct Line<ohal::gpio::PortC, PinNum> : detail::Stm32u0ExtiGpioLine<detail::kPortCodeC, PinNum>
{
};

template <uint8_t PinNum>
struct Line<ohal::gpio::PortD, PinNum> : detail::Stm32u0ExtiGpioLine<detail::kPortCodeD, PinNum>
{
};

template <uint8_t PinNum>
struct Line<ohal::gpio::PortE, PinNum> : detail::Stm32u0ExtiGpioLine<detail::kPortCodeE, PinNum>
{
};

template <uint8_t PinNum>
struct Line<ohal::gpio::PortF, PinNum> : detail::Stm32u0ExtiGpioLine<detail::kPortCodeF, PinNum>
{
};

} // namespace ohal::exti

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_EXTI_HPP
