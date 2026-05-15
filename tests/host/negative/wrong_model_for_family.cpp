// Negative-compile test: selecting the STM32U0 family but providing a model macro
// that does not belong to that family must fail with a preprocessor error.
// Expected #error: "ohal: No STM32U0 model defined."

// Simulate a mis-matched model: OHAL_MODEL_MSP430FR2355 is not a STM32U0 model.
#define OHAL_FAMILY_STM32U0
#define OHAL_MODEL_MSP430FR2355

#include <ohal/platform.hpp>

int main() { return 0; }
