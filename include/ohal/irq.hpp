#ifndef OHAL_IRQ_HPP
#define OHAL_IRQ_HPP

#include <type_traits>

namespace ohal::irq {

template <typename Family>
struct has_global_irq_control : std::false_type {};

template <typename Family>
struct GlobalController {
  static_assert(sizeof(Family) == 0,
                "ohal: irq::GlobalController is not implemented for the selected MCU family.");
};

template <typename Family>
class Guard {
public:
  using Controller = GlobalController<Family>;
  using state_type = typename Controller::state_type;

  Guard() noexcept : saved_state_(Controller::save_state()) { Controller::disable(); }
  ~Guard() noexcept { Controller::restore_state(saved_state_); }

  Guard(const Guard&) = delete;
  Guard& operator=(const Guard&) = delete;
  Guard(Guard&&) = delete;
  Guard& operator=(Guard&&) = delete;

private:
  state_type saved_state_;
};

} // namespace ohal::irq

#endif // OHAL_IRQ_HPP
