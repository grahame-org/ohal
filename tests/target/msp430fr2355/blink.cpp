// Minimal blink compile-check for MSP430FR2355.
// Built as a static library (no startup code or linker script needed).
// Compile with: -DOHAL_FAMILY_MSP430FR2XX -DOHAL_MODEL_MSP430FR2355 -std=c++17

#include <ohal/ohal.hpp>

using namespace ohal::gpio;

using Led = Pin<PortA, 2>;
using Button = Pin<PortA, 3>;

void blink_init() noexcept
{
    Led::set_mode(PinMode::Output);
    Led::set();
}

void blink_toggle() noexcept { Led::toggle(); }

void button_init() noexcept
{
    Button::set_mode(PinMode::Input);
    Button::set_pull(Pull::Up);
}

bool button_pressed() noexcept { return Button::read_input() == Level::Low; }
