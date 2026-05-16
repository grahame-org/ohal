#ifndef OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_IRQ_HPP
#define OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_IRQ_HPP

#include <cstdint>

#include "ohal/irq.hpp"
#include "ohal/platforms/stm32u0/family_tag.hpp"

namespace ohal::irq
{

template <>
struct has_global_irq_control<ohal::platforms::stm32u0::Family> : std::true_type
{
};

template <>
struct GlobalController<ohal::platforms::stm32u0::Family>
{
    using state_type = uint32_t;

    [[nodiscard]] static state_type save_state() noexcept
    {
#if defined(__arm__) || defined(__thumb__) || defined(__ARM_ARCH)
        state_type primask{0U};
        __asm__ volatile("MRS %0, PRIMASK" : "=r"(primask));
        return primask;
#else
        return host_primask_state_;
#endif
    }

    static void restore_state(state_type state) noexcept
    {
#if defined(__arm__) || defined(__thumb__) || defined(__ARM_ARCH)
        __asm__ volatile("MSR PRIMASK, %0" : : "r"(state) : "memory");
#else
        host_primask_state_ = state;
#endif
    }

    static void enable() noexcept
    {
#if defined(__arm__) || defined(__thumb__) || defined(__ARM_ARCH)
        __asm__ volatile("CPSIE i" : : : "memory");
#else
        host_primask_state_ = 0U;
#endif
    }

    static void disable() noexcept
    {
#if defined(__arm__) || defined(__thumb__) || defined(__ARM_ARCH)
        __asm__ volatile("CPSID i" : : : "memory");
#else
        host_primask_state_ = 1U;
#endif
    }

  private:
#if !defined(__arm__) && !defined(__thumb__) && !defined(__ARM_ARCH)
    static inline state_type host_primask_state_{0U};
#endif
};

} // namespace ohal::irq

#endif // OHAL_PLATFORMS_STM32U0_MODELS_STM32U083_IRQ_HPP
