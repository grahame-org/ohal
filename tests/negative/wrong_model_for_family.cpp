// Negative-compile test: specifying an STM32U0 family together with an MSP430
// model must fail because MSP430FR2355 is not a member of the STM32U0 family.
// Compiled with: -DOHAL_FAMILY_STM32U0 -DOHAL_MODEL_MSP430FR2355
// Expected error: "No STM32U0 model defined"
#include <ohal/ohal.hpp>
