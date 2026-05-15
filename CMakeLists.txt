cmake_minimum_required(VERSION 3.21)
project(ohal VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

option(OHAL_BUILD_TESTS "Build host-side tests" ON)
set(OHAL_MCU_FAMILY "" CACHE STRING "MCU family (e.g. STM32U0)")
set(OHAL_MCU_MODEL  "" CACHE STRING "MCU model  (e.g. STM32U083)")

add_library(ohal INTERFACE)
target_include_directories(ohal INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(NOT OHAL_MCU_FAMILY STREQUAL "")
  target_compile_definitions(ohal INTERFACE OHAL_FAMILY_${OHAL_MCU_FAMILY})
endif()

if(NOT OHAL_MCU_MODEL STREQUAL "")
  target_compile_definitions(ohal INTERFACE OHAL_MODEL_${OHAL_MCU_MODEL})
endif()

if(OHAL_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests/host)

  # Negative-compile tests: verify that invalid code fails to compile and
  # produces the expected diagnostic message.
  #
  # Usage: ohal_expect_compile_failure(<test-name> <source-file>
  #                                    <expected-error> [define ...])
  #
  # Each extra positional argument after <expected-error> is passed as a
  # preprocessor definition (-D<define>) when compiling the source file.
  # Pipe characters are used internally as list separators to avoid CMake
  # misinterpreting semicolons.
  function(ohal_expect_compile_failure test_name source_file expected_error)
    string(JOIN "|" extra_defines_str ${ARGN})
    add_test(
      NAME ${test_name}
      COMMAND
        ${CMAKE_COMMAND}
        -DCXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DINCLUDE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/include
        "-DEXTRA_DEFINES=${extra_defines_str}"
        -DSOURCE=${source_file}
        "-DEXPECTED_ERROR=${expected_error}"
        "-DOUTPUT_OBJECT=${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${test_name}.o"
        -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/expect_compile_failure.cmake
    )
  endfunction()

  set(NEG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests/negative)

  ohal_expect_compile_failure(
    write_to_readonly_field ${NEG_DIR}/write_to_readonly_field.cpp
    "cannot write to a read-only field"
  )

  ohal_expect_compile_failure(
    read_from_writeonly_field ${NEG_DIR}/read_from_writeonly_field.cpp
    "cannot read from a write-only field"
  )

  ohal_expect_compile_failure(
    overflow_bitfield ${NEG_DIR}/overflow_bitfield.cpp
    "BitField (Offset + Width) exceeds register width"
  )

  ohal_expect_compile_failure(
    no_family_defined ${NEG_DIR}/no_family_defined.cpp "No MCU family defined"
  )

  ohal_expect_compile_failure(
    wrong_model_for_family
    ${NEG_DIR}/wrong_model_for_family.cpp
    "No STM32U0 model defined"
    OHAL_FAMILY_STM32U0
    OHAL_MODEL_MSP430FR2355
  )

  ohal_expect_compile_failure(
    msp430_unsupported_speed
    ${NEG_DIR}/msp430_unsupported_speed.cpp
    "does not support configurable output speed"
    OHAL_FAMILY_MSP430FR2XX
    OHAL_MODEL_MSP430FR2355
  )

  ohal_expect_compile_failure(
    msp430_unsupported_output_type
    ${NEG_DIR}/msp430_unsupported_output_type.cpp
    "does not support configurable output type"
    OHAL_FAMILY_MSP430FR2XX
    OHAL_MODEL_MSP430FR2355
  )
endif()

option(
  OHAL_BUILD_TARGET_TESTS
  "Build bare-metal on-target tests (requires a cross-compiler toolchain)"
  OFF
)
if(OHAL_BUILD_TARGET_TESTS)
  add_subdirectory(tests/target)
endif()
