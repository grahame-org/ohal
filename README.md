# ohal — Zero Overhead Open HAL

`ohal` is a header-only, C++17 hardware abstraction library for microcontrollers. Every peripheral
operation compiles to the same instruction sequence as a hand-written `volatile` register access.
There is no runtime overhead, no heap allocation, and no virtual dispatch.

## Key features

- **Zero RAM** — all configuration is encoded in types and template parameters.
- **Zero overhead** — every HAL call compiles to a single `volatile` bus transaction or better.
- **Compile-time safety** — reading a write-only register, writing a read-only register, or using a
  feature that the selected MCU does not support are all **compile errors**, not runtime errors.
- **Cross-platform** — the same `ohal::gpio` API works on STM32, PIC18, and future families with no
  changes to application code.
- **C++17 strict** — no compiler extensions, no C++20 features.

## Quick start

```cmake
# CMakeLists.txt
include(FetchContent)
FetchContent_Declare(ohal GIT_REPOSITORY https://github.com/grahame-org/ohal GIT_TAG main)
FetchContent_MakeAvailable(ohal)

target_link_libraries(my_app PRIVATE ohal)
target_compile_definitions(my_app PRIVATE OHAL_FAMILY_STM32U0 OHAL_MODEL_STM32U083)
```

```cpp
// main.cpp — illustrative example (Pin<> methods land in Step 8)
#include <ohal/ohal.hpp>

using namespace ohal::gpio;
using Led = Pin<PortA, 5>;

int main() {
    Led::set_mode(PinMode::Output);
    Led::set();
    while (true) { Led::toggle(); }
}
```

## Documentation

| Audience                                   | Start here                                                   |
| ------------------------------------------ | ------------------------------------------------------------ |
| **End users** — using ohal in a project    | [Getting started](docs/user/getting-started.md)              |
| **End users** — GPIO API reference         | [GPIO API](docs/user/gpio-api.md)                            |
| **End users** — supported MCUs             | [Supported MCUs](docs/user/supported-mcus.md)                |
| **End users** — safe multi-pin switching   | [H-bridge guide](docs/user/h-bridge.md)                      |
| **Contributors** — architecture overview   | [Architecture](docs/contributor/architecture.md)             |
| **Contributors** — register specifications | [Specifications](docs/contributor/specs.md)                  |
| **Contributors** — adding a new MCU        | [Adding an MCU](docs/contributor/adding-mcu.md)              |
| **Contributors** — adding a new peripheral | [Adding a peripheral](docs/contributor/adding-peripheral.md) |

## Project status

The core abstraction layers and GPIO interface are implemented. Platform specialisations for
individual MCU models are in active development. See [Supported MCUs](docs/user/supported-mcus.md)
for the current status of each family and model.

## License

[MIT](LICENSE)
