# Getting started with ohal

`ohal` is a header-only C++17 hardware abstraction library. This guide walks you from a blank
project to a working GPIO blink example.

## Prerequisites

| Tool         | Minimum version | Notes                                                                        |
| ------------ | --------------- | ---------------------------------------------------------------------------- |
| C++ compiler | C++17           | `arm-none-eabi-g++` for ARM targets; any hosted compiler for host-side tests |
| CMake        | 3.21            | Used to consume the library and run tests                                    |
| Git          | any             | Required by CMake `FetchContent`                                             |

## Adding ohal to your project

### Option 1: CMake FetchContent (recommended)

Add the following to your `CMakeLists.txt`:

```cmake
include(FetchContent)
FetchContent_Declare(
    ohal
    GIT_REPOSITORY https://github.com/grahame-org/ohal
    GIT_TAG        main
)
FetchContent_MakeAvailable(ohal)

target_link_libraries(my_app PRIVATE ohal)
```

### Option 2: Git submodule

```bash
git submodule add https://github.com/grahame-org/ohal extern/ohal
```

```cmake
add_subdirectory(extern/ohal EXCLUDE_FROM_ALL)
target_link_libraries(my_app PRIVATE ohal)
```

## Selecting your MCU

ohal requires two compile-time defines that identify the MCU family and model. Pass them via
`target_compile_definitions` in CMake:

```cmake
target_compile_definitions(my_app PRIVATE
    OHAL_FAMILY_STM32U0
    OHAL_MODEL_STM32U083
)
```

Or as compiler flags:

```sh
arm-none-eabi-g++ -DOHAL_FAMILY_STM32U0 -DOHAL_MODEL_STM32U083 ...
```

Omitting either define, or combining defines from different families, is a **compile error** with a
descriptive message. For example:

```text
error: "ohal: No MCU family defined. Pass exactly one of -DOHAL_FAMILY_STM32U0,
        -DOHAL_FAMILY_TI_MSPM0, -DOHAL_FAMILY_PIC to the compiler."
```

See [Supported MCUs](supported-mcus.md) for the full list of valid define combinations.

## Including the library

Use a single top-level include in every translation unit that needs the HAL:

```cpp
#include <ohal/ohal.hpp>
```

This header pulls in all core abstractions and the GPIO interface. No other includes are needed.

## First program: blink an LED

> **Note — illustrative example:** The GPIO methods shown below (`set_mode`, `set_output_type`,
> `set_speed`, `set_pull`, `set`, `toggle`) are part of the planned `Pin<>` interface and will be
> available once the STM32U083 platform specialisation lands in Step 8. The compile setup, type
> aliases, and `#include` are correct today; only the method calls are forward-looking.

The following example configures PA5 as a push-pull output and toggles it indefinitely. It targets
the STM32U083.

```cpp
// Compile with: -DOHAL_FAMILY_STM32U0 -DOHAL_MODEL_STM32U083 -std=c++17

#include <ohal/ohal.hpp>

using namespace ohal::gpio;

// Alias the pin to a descriptive name. This is a type alias, not a variable —
// it occupies zero RAM.
using Led = Pin<PortA, 5>;

int main() {
    Led::set_mode(PinMode::Output);
    Led::set_output_type(OutputType::PushPull);
    Led::set_speed(Speed::Low);
    Led::set_pull(Pull::None);
    Led::set();  // initial state: high

    while (true) {
        Led::toggle();
        // insert your delay here
    }
}
```

## Understanding compile errors

ohal uses `static_assert` and `#error` directives to catch misuse at compile time. All error
messages are prefixed with `ohal:` so they are easy to grep for in build logs.

| Situation                  | Example error message                                               |
| -------------------------- | ------------------------------------------------------------------- |
| No MCU family define       | `ohal: No MCU family defined. Pass exactly one of …`                |
| No model define            | `ohal: No STM32U0 model defined. Pass -DOHAL_MODEL_STM32U083 …`     |
| Wrong model for family     | `ohal: Model PIC18F4550 is not part of family STM32U0.`             |
| Unsupported peripheral     | `ohal: gpio::Pin is not implemented for the selected MCU.`          |
| Unsupported feature        | `ohal: PIC18F4550 GPIO does not support configurable output speed.` |
| Write to read-only field   | `ohal: cannot write to a read-only field`                           |
| Read from write-only field | `ohal: cannot read from a write-only field`                         |

## Next steps

- [GPIO API reference](gpio-api.md) — full description of every GPIO method and enumeration.
- [Supported MCUs](supported-mcus.md) — which families and models are available today.
