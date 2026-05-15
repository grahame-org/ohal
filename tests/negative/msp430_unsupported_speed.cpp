// Negative-compile test: MSP430FR2355 does not support configurable output speed.
// Compiled with: -DOHAL_FAMILY_MSP430FR2XX -DOHAL_MODEL_MSP430FR2355
// Expected error: "does not support configurable output speed"
#include <ohal/ohal.hpp>

using namespace ohal::gpio;

void test() noexcept { Pin<PortA, 2>::set_speed(Speed::High); }
