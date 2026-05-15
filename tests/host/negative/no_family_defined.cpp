// Negative-compile test: including platform.hpp with no OHAL_FAMILY_* macro defined
// must fail with a preprocessor error.
// Expected #error: "ohal: No MCU family defined."

#include <ohal/platform.hpp>

int main() { return 0; }
