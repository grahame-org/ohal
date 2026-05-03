# Step 11 – Host and Target Unit Testing

**Goal:** A complete, runnable test suite on the development host, and a framework for on-target
testing.

## 11.1 Host Testing with Mock Registers

The mock infrastructure replaces the `volatile` memory-mapped I/O with plain in-memory arrays.
This is achieved by **not** using the real platform headers during host tests. Instead, the test
provides its own register addresses pointing into a local array:

```cpp
// tests/host/mock/mock_register.hpp
#ifndef OHAL_TESTS_HOST_MOCK_MOCK_REGISTER_HPP
#define OHAL_TESTS_HOST_MOCK_MOCK_REGISTER_HPP

#include <array>
#include <cstdint>

namespace ohal::test {

// 256 bytes of simulated register space (64 x 32-bit slots, or 256 x 8-bit slots).
inline std::array<uint32_t, 64> mock_memory{};

// Helper: reset mock memory between tests.
inline void reset_mock() { mock_memory.fill(0); }

// Returns the uintptr_t address of 32-bit slot N in mock_memory.
// Note: converting a data pointer to uintptr_t is implementation-defined.
// This is acceptable for host-side test code where the goal is to redirect
// register accesses into a plain memory array on a hosted implementation;
// newer standards do not make this pointer-to-integer mapping portable.
inline uintptr_t mock_addr(std::size_t slot) {
    return reinterpret_cast<uintptr_t>(mock_memory.data()) + slot * sizeof(uint32_t);
}

// Returns the uintptr_t address of 8-bit slot N in mock_memory.
// Used for 8-bit port registers (e.g. MSP430FR2355 GPIO ports).
inline uintptr_t mock_addr8(std::size_t slot) {
    return reinterpret_cast<uintptr_t>(
        reinterpret_cast<uint8_t*>(mock_memory.data()) + slot);
}

} // namespace ohal::test

#endif // OHAL_TESTS_HOST_MOCK_MOCK_REGISTER_HPP
```

Host tests instantiate `Register<mock_addr(N)>` and `BitField<Register<mock_addr(N)>, ...>` and
verify that the correct memory locations are modified:

```cpp
// tests/host/test_register.cpp  (example using Catch2)
#include <catch2/catch_test_macros.hpp>
#include "mock/mock_register.hpp"
#include "ohal/core/register.hpp"

TEST_CASE("Register::write stores value at correct address") {
    ohal::test::reset_mock();
    using Reg = ohal::core::Register<ohal::test::mock_addr(0)>;
    Reg::write(0xDEADBEEFu);
    REQUIRE(ohal::test::mock_memory[0] == 0xDEADBEEFu);
}

TEST_CASE("Register::set_bits ORs without disturbing other bits") {
    ohal::test::reset_mock();
    ohal::test::mock_memory[0] = 0xF0F0F0F0u;
    using Reg = ohal::core::Register<ohal::test::mock_addr(0)>;
    Reg::set_bits(0x0F0F0F0Fu);
    REQUIRE(ohal::test::mock_memory[0] == 0xFFFFFFFFu);
}
```

**How addresses are mocked:** The STM32U083 platform header uses preprocessor macros for base
addresses (`OHAL_STM32U083_GPIOA_BASE`). In normal builds these default to the real hardware
addresses. In host test builds the CMake target passes
`-DOHAL_STM32U083_GPIOA_BASE=ohal::test::mock_addr(0)` (and similar for other ports/registers),
redirecting all register accesses into the mock array. The same mechanism is used for MSP430FR2355
using `mock_addr8()` for 8-bit slots.

## 11.2 Target Testing

On-target tests use the same test source files but compiled for the real MCU. A minimal
on-target test runner:

1. Resets all GPIOs to a known state.
2. Calls each test function.
3. Reports pass/fail over UART or an LED toggle pattern.

The test CMake target `ohal_target_tests` links the test sources plus the real platform headers
and the BSP startup files. It is built only when cross-compiling for a supported target:

- ARM targets: `arm-none-eabi-g++` (STM32U083)
- MSP430 targets: `msp430-elf-g++` (MSP430FR2355)

## 11.3 Negative-Compile Tests

For tests that must confirm a compile error occurs, add a CMake helper
`ohal_expect_compile_failure`:

```cmake
# Compiles the source with expected_error string in compiler stderr.
# Fails the CMake test if the compilation succeeds or if the error string is absent.
function(ohal_expect_compile_failure test_name source_file expected_error)
    add_test(NAME ${test_name}
        COMMAND ${CMAKE_COMMAND}
            -DSOURCE=${source_file}
            -DEXPECTED_ERROR=${expected_error}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/expect_compile_failure.cmake
    )
endfunction()
```

Example negative-compile tests:

| Test name                        | Source fragment                                           | Expected error string                              |
| -------------------------------- | --------------------------------------------------------- | -------------------------------------------------- |
| `write_to_readonly_field`        | `IDR_field::write(Level::High)`                           | `cannot write to a read-only field`                |
| `read_from_writeonly_field`      | `BSRR_SET_field::read()`                                  | `cannot read from a write-only field`              |
| `overflow_bitfield`              | `BitField<Reg, 30, 4, RW>`                                | `BitField (Offset + Width) exceeds register width` |
| `no_family_defined`              | compile with no defines                                   | `No MCU family defined`                            |
| `wrong_model_for_family`         | `OHAL_FAMILY_STM32U0` + `OHAL_MODEL_MSP430FR2355`         | `not in family STM32U0`                            |
| `msp430_unsupported_speed`       | `Pin<PortA,2>::set_speed(Speed::High)` with MSP430 target | `does not support configurable output speed`       |
| `msp430_unsupported_output_type` | `Pin<PortA,2>::set_output_type(OutputType::OpenDrain)`    | `does not support configurable output type`        |

## 11.4 Test Coverage Targets

| Component               | Test type                       | Coverage target                                                                                      |
| ----------------------- | ------------------------------- | ---------------------------------------------------------------------------------------------------- |
| `Register<uint32_t>`    | Host unit tests                 | 100% of all methods                                                                                  |
| `Register<uint8_t>`     | Host unit tests                 | 100% of all methods (for 8-bit platform coverage)                                                    |
| `BitField<>`            | Host unit tests                 | 100% of all methods + negative-compile tests for access violations                                   |
| `platform.hpp`          | Negative-compile tests          | All invalid define combinations                                                                      |
| `stm32u083/gpio.hpp`    | Host unit tests with mock       | All GPIO methods for at least pins 0 and 15 of at least PortA and PortB                              |
| `msp430fr2355/gpio.hpp` | Host unit tests with 8-bit mock | All GPIO methods for at least pins 0 and 7 of PortA; unsupported features via negative-compile tests |
| Consumer API            | Host integration test           | Typical GPIO init + toggle sequence                                                                  |
