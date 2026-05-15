// Minimal blink compile-check for STM32U083KCU.
// Built as a static library (no startup code or linker script needed).
// Compile with: -DOHAL_FAMILY_STM32U0 -DOHAL_MODEL_STM32U083KCU -std=c++17

#include <ohal/ohal.hpp>

using namespace ohal::gpio;

using Led = Pin<PortA, 5>;
using Button = Pin<PortC, 13>;

void blink_init() noexcept {
  Led::set_mode(PinMode::Output);
  Led::set_output_type(OutputType::PushPull);
  Led::set_speed(Speed::Low);
  Led::set_pull(Pull::None);
  Led::set();
}

void blink_toggle() noexcept { Led::toggle(); }

void button_init() noexcept {
  Button::set_mode(PinMode::Input);
  Button::set_pull(Pull::Up);
}

bool button_pressed() noexcept { return Button::read_input() == Level::Low; }
