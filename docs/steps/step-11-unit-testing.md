# Step 11 – Host and Target Unit Testing

**Goal:** A complete, runnable test suite on the development host, and a framework for on-target
testing.

## 11.1 Host Testing with Mock Registers

The mock infrastructure provides two complementary mechanisms for host tests:

1. **`MockRegister<T, &storage>` tests** — `MockRegister<T, &storage>` is a type alias that models
   the same `read()`/`write()` API as `Register<Addr, T>` but reads from and writes to a plain
   variable. All host tests (including `Register<>` and `BitField<>` coverage) use this approach
   because `mock_addr()` returns a runtime `uintptr_t` computed from `reinterpret_cast`, which
   is not a constant expression in C++17 and cannot be used as a `Register<>` non-type template
   parameter.

2. **Shared mock memory** — `mock_memory`, `mock_addr(N)`, and `mock_addr8(N)` are provided in
   `mock_register.hpp` for completeness and potential legacy use, but GPIO and register tests
   do **not** instantiate `Register<mock_addr(N)>` directly. All host-test register accesses go
   through `MockRegister`.

The `mock_register.hpp` header provides both:

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

The example below tests the `Register<>` core template via mechanism (2) (MockRegister):

```cpp
// tests/host/test_register.cpp  (GoogleTest)
#include <gtest/gtest.h>
#include "mock/mock_register.hpp"
#include "ohal/core/register.hpp"

static uint32_t mock_storage{0U};

TEST(RegisterTest, WriteStoresValueAtCorrectAddress) {
    mock_storage = 0U;
    using Reg = ohal::test::MockRegister<uint32_t, &mock_storage>;
    Reg::write(0xDEADBEEFu);
    EXPECT_EQ(mock_storage, 0xDEADBEEFu);
}
```

**How registers are mocked:** The STM32U083 platform header defines
`GpioPortPinImpl<PinNum, Regs>` parameterised on a `Regs` type. In normal builds the
`Pin<PortA, N>` specialisation injects `GpioA` (which wraps the real hardware addresses).
Host test builds instead instantiate `GpioPortPinImpl<PinNum, MockGpioRegs>` directly, where
`MockGpioRegs` carries `ohal::test::MockRegister<uint32_t, &storage>` type aliases — no address
override macros are needed. The MSP430FR2355 implementation follows the same template-injection
pattern using `GpioPortPinImpl<PinNum, Regs>` with `ohal::test::MockRegister<uint8_t, &storage8>`
backing variables for its 8-bit registers.

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
| `wrong_model_for_family`         | `OHAL_FAMILY_STM32U0` + `OHAL_MODEL_MSP430FR2355`         | `No STM32U0 model defined`                         |
| `msp430_unsupported_speed`       | `Pin<PortA,2>::set_speed(Speed::High)` with MSP430 target | `does not support configurable output speed`       |
| `msp430_unsupported_output_type` | `Pin<PortA,2>::set_output_type(OutputType::OpenDrain)`    | `does not support configurable output type`        |

## 11.4 Test Coverage Targets

| Component               | Test type                       | Coverage target                                                                                                                                                                                                              |
| ----------------------- | ------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `Register<uint32_t>`    | Host unit tests                 | 100% of all methods                                                                                                                                                                                                          |
| `Register<uint8_t>`     | Host unit tests                 | 100% of all methods (for 8-bit platform coverage)                                                                                                                                                                            |
| `BitField<>`            | Host unit tests                 | 100% of all methods + negative-compile tests for access violations                                                                                                                                                           |
| `platform.hpp`          | Negative-compile tests          | All invalid define combinations                                                                                                                                                                                              |
| `stm32u083/gpio.hpp`    | Host unit tests with mock       | All GPIO methods for at least pins 0 and 15 of at least PortA and PortB                                                                                                                                                      |
| `msp430fr2355/gpio.hpp` | Host unit tests with 8-bit mock | All GPIO methods for at least pins 0 and 7 of PortA; capability traits (`supports_pull`, `supports_alternate_function`) true for valid pins and false for out-of-range pins; negative-compile tests for unsupported features |
| Consumer API            | Host integration test           | Typical GPIO init + toggle sequence                                                                                                                                                                                          |
