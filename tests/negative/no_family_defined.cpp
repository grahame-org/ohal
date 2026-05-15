// Negative-compile test: including ohal/platform.hpp without any OHAL_FAMILY_*
// define must produce a compile error.
// Expected error: "No MCU family defined"
#include <ohal/platform.hpp>
