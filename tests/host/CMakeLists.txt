include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG v1.15.2
)
# Prevent googletest from overriding our compiler/linker settings on Windows
# cmake-lint: disable=C0103
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ---------------------------------------------------------------------------
# ohal_expect_compile_failure(test_name source_file expected_error)
#
# Registers a CTest test that:
#   1. Compiles SOURCE with the ohal include path and the tests/host directory.
#   2. Expects compilation to FAIL (non-zero exit status).
#   3. Verifies that the compiler output contains EXPECTED_ERROR.
#
# Include directories are passed to the helper cmake script as a
# pipe-separated list so that semicolons in CMake lists are not confused with
# argument separators.
# ---------------------------------------------------------------------------
function(ohal_expect_compile_failure test_name source_file expected_error)
  set(_ohal_include_dir "${CMAKE_SOURCE_DIR}/include")
  set(_test_include_dir "${CMAKE_CURRENT_SOURCE_DIR}")
  set(_all_includes "${_ohal_include_dir}|${_test_include_dir}")

  add_test(
    NAME ${test_name}
    COMMAND
      ${CMAKE_COMMAND}
      "-DSOURCE=${source_file}"
      "-DEXPECTED_ERROR=${expected_error}"
      "-DCXX_COMPILER=${CMAKE_CXX_COMPILER}"
      "-DCXX_STANDARD=${CMAKE_CXX_STANDARD}"
      "-DINCLUDES=${_all_includes}"
      -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/expect_compile_failure.cmake"
  )
endfunction()

# KCU and HCY share the same stm32u083/gpio.hpp partial specialisations and
# introduce no explicit full specialisations for PortC/PortF pins; they are
# ODR-safe in the same binary.
add_executable(ohal_host_tests main.cpp test_register.cpp test_bitfield.cpp test_gpio.cpp
                               test_gpio_stm32u083kcu.cpp test_gpio_stm32u083hcy.cpp)
target_link_libraries(ohal_host_tests PRIVATE ohal GTest::gtest_main GTest::gmock)

# CC-model tests use separate executables: stm32u083cc/gpio.hpp introduces
# explicit full specialisations for non-bonded PortC (bits 0–12) and PortF
# (bits 4–15) pins.  Linking CCU/CCT with KCU/HCY in the same binary would
# produce two different definitions for those Pin<> instantiations — an ODR
# violation.  CCU and CCT share identical specialisations (both delegate to
# stm32u083cc/gpio.hpp) so they can coexist in a single executable.
add_executable(ohal_host_tests_cc main.cpp test_gpio_stm32u083ccu.cpp
                                  test_gpio_stm32u083cct.cpp)
target_link_libraries(ohal_host_tests_cc PRIVATE ohal GTest::gtest_main GTest::gmock)

# RC-model tests use separate executables: the STM32U083RC packages bond out
# GPIOD (giving Pin<PortD> a working definition), while the KCU/HCY models
# provide a static_assert for Pin<PortD>.  Mixing those in one executable would
# be an ODR violation.  RCT and RCI are kept separate so each binary contains
# only one set of explicit template specialisations.
add_executable(ohal_host_tests_rct main.cpp test_gpio_stm32u083rct.cpp
                                   test_uart_timer_stm32u083rct.cpp
                                   test_interrupts_stm32u083rct.cpp)
target_link_libraries(ohal_host_tests_rct PRIVATE ohal GTest::gtest_main GTest::gmock)

add_executable(ohal_host_tests_rci main.cpp test_gpio_stm32u083rci.cpp)
target_link_libraries(ohal_host_tests_rci PRIVATE ohal GTest::gtest_main GTest::gmock)

# MC-model tests use separate executables: the STM32U083MC packages bond out
# GPIOE (giving Pin<PortE> a working definition), while the KCU/HCY/CC models
# provide a static_assert for Pin<PortE>.  Mixing those in one executable would
# be an ODR violation.  MCT and MCI are kept separate because they provide
# different Pin<PortE, 3> specialisations (absent on MCT, bonded on MCI).
add_executable(ohal_host_tests_mct main.cpp test_gpio_stm32u083mct.cpp)
target_link_libraries(ohal_host_tests_mct PRIVATE ohal GTest::gtest_main GTest::gmock)

add_executable(ohal_host_tests_mci main.cpp test_gpio_stm32u083mci.cpp)
target_link_libraries(ohal_host_tests_mci PRIVATE ohal GTest::gtest_main GTest::gmock)

# MSP430FR2355 tests use a separate executable: msp430fr2355/gpio.hpp introduces
# Pin<PortA…PortF, PinNum> partial specialisations backed by MSP430-specific
# GpioPortPinImpl.  Linking this with any STM32U083 test binary would produce
# two different definitions for those Pin<> instantiations — an ODR violation.
add_executable(ohal_host_tests_msp430fr2355 main.cpp test_gpio_msp430fr2355.cpp)
target_link_libraries(ohal_host_tests_msp430fr2355 PRIVATE ohal GTest::gtest_main GTest::gmock)

# MSP430FR2355TPT tests use a separate executable: msp430fr2355tpt/capabilities.hpp
# provides different partial specialisations for PortE and PortF than
# msp430fr2355/capabilities.hpp.  Combining them in one binary would cause
# two conflicting definitions of those specialisations — an ODR violation.
add_executable(ohal_host_tests_msp430fr2355tpt main.cpp test_gpio_msp430fr2355tpt.cpp)
target_link_libraries(ohal_host_tests_msp430fr2355tpt PRIVATE ohal GTest::gtest_main GTest::gmock)

# MSP430FR2355TDBT tests use a separate executable: msp430fr2355tdbt/capabilities.hpp
# provides different partial specialisations for PortE and PortF than other packages.
add_executable(ohal_host_tests_msp430fr2355tdbt main.cpp test_gpio_msp430fr2355tdbt.cpp)
target_link_libraries(ohal_host_tests_msp430fr2355tdbt PRIVATE ohal GTest::gtest_main GTest::gmock)

# MSP430FR2355TRSM tests use a separate executable: msp430fr2355trsm/capabilities.hpp
# provides different partial specialisations for all ports than other packages.
add_executable(ohal_host_tests_msp430fr2355trsm main.cpp test_gpio_msp430fr2355trsm.cpp)
target_link_libraries(ohal_host_tests_msp430fr2355trsm PRIVATE ohal GTest::gtest_main GTest::gmock)

# MSP430FR2355TRHA tests use a separate executable: msp430fr2355trha/capabilities.hpp
# provides different partial specialisations for PortE and PortF than other packages.
add_executable(ohal_host_tests_msp430fr2355trha main.cpp test_gpio_msp430fr2355trha.cpp)
target_link_libraries(ohal_host_tests_msp430fr2355trha PRIVATE ohal GTest::gtest_main GTest::gmock)

# STM32U031R-series tests use a separate executable: stm32u031r*/gpio.hpp delegates
# to stm32u083rc/gpio.hpp which introduces explicit full specialisations for non-bonded
# PortD and PortF pins.  Mixing with KCU/HCY/CC models would be an ODR violation.
# All four R-series packages (R6T/R6I/R8T/R8I) share identical Pin<> specialisations
# (all delegate to stm32u083rc/gpio.hpp) so they are ODR-safe in one executable.
add_executable(ohal_host_tests_stm32u031r main.cpp test_gpio_stm32u031r6t.cpp
                                           test_gpio_stm32u031r6i.cpp test_gpio_stm32u031r8t.cpp
                                           test_gpio_stm32u031r8i.cpp)
target_link_libraries(ohal_host_tests_stm32u031r PRIVATE ohal GTest::gtest_main GTest::gmock)

# STM32U031C-series tests use a separate executable: stm32u031c*/gpio.hpp delegates
# to stm32u083cc/gpio.hpp which introduces explicit full specialisations for non-bonded
# PortC (bits 0-12) and PortF (bits 4-15) pins.  Cannot be combined with R-series.
# All four C-series packages (C6U/C6T/C8U/C8T) share identical Pin<> specialisations
# so they are ODR-safe in one executable.
add_executable(ohal_host_tests_stm32u031c main.cpp test_gpio_stm32u031c6u.cpp
                                           test_gpio_stm32u031c6t.cpp test_gpio_stm32u031c8u.cpp
                                           test_gpio_stm32u031c8t.cpp)
target_link_libraries(ohal_host_tests_stm32u031c PRIVATE ohal GTest::gtest_main GTest::gmock)

# STM32U031K-series tests use a separate executable: stm32u031k/gpio.hpp introduces
# explicit full specialisations for non-bonded PortB, PortC and PortF pins with a
# different set from the C-series and R-series.  Cannot be combined with other models.
# All three K-series packages (K4U/K6U/K8U) share identical Pin<> specialisations.
add_executable(ohal_host_tests_stm32u031k main.cpp test_gpio_stm32u031k4u.cpp
                                           test_gpio_stm32u031k6u.cpp test_gpio_stm32u031k8u.cpp)
target_link_libraries(ohal_host_tests_stm32u031k PRIVATE ohal GTest::gtest_main GTest::gmock)

# STM32U031G-series tests use a separate executable: stm32u031g/gpio.hpp introduces
# explicit full specialisations for PortA bit 15, PortB bits 2-4 and 8-15, PortC
# bits 0-13, and PortF bits 0-1 and 4-15.  PA15 is absent (differs from K-series).
# Both G-series packages (G6Y/G8Y) share identical Pin<> specialisations.
add_executable(ohal_host_tests_stm32u031g main.cpp test_gpio_stm32u031g6y.cpp
                                           test_gpio_stm32u031g8y.cpp)
target_link_libraries(ohal_host_tests_stm32u031g PRIVATE ohal GTest::gtest_main GTest::gmock)

# STM32U031F-series tests use a separate executable: stm32u031f/gpio.hpp introduces
# explicit full specialisations for PortA bit 15, PortB bits 2-3 and 10-15, PortC
# bits 0-13, and PortF bits 0-1 and 4-15.  PB bonding differs from G-series.
# All three F-series packages (F4P/F6P/F8P) share identical Pin<> specialisations.
add_executable(ohal_host_tests_stm32u031f main.cpp test_gpio_stm32u031f4p.cpp
                                           test_gpio_stm32u031f6p.cpp test_gpio_stm32u031f8p.cpp)
target_link_libraries(ohal_host_tests_stm32u031f PRIVATE ohal GTest::gtest_main GTest::gmock)

# STM32U083 timer register-map wiring tests.  The package header
# (stm32u083rct/timer.hpp) defines OHAL_STM32U0_ENABLE_LPTIM3 before
# including the shared implementation, exercising the full include chain
# including the LPTIM3 peripheral present on all STM32U083 packages.
add_executable(ohal_host_tests_timer main.cpp test_timer_stm32u083.cpp)
target_link_libraries(ohal_host_tests_timer PRIVATE ohal GTest::gtest_main GTest::gmock)

# STM32U031 timer register-map wiring tests.  The package header
# (stm32u031r8t/timer.hpp) does NOT define OHAL_STM32U0_ENABLE_LPTIM3,
# validating the LPTIM3-absent path (kLptim3Base and Lptim3 are not defined).
add_executable(ohal_host_tests_timer_stm32u031 main.cpp test_timer_stm32u031.cpp)
target_link_libraries(ohal_host_tests_timer_stm32u031 PRIVATE ohal GTest::gtest_main GTest::gmock)

# STM32U073 K/C/H-series tests.  All twelve variants (k8u, kbu, kcu, c8t, c8u,
# cbt, cbu, cct, ccu, h8y, hby, hcy) delegate to stm32u083/gpio.hpp, producing
# identical Pin<> specialisations.  Kept separate from the STM32U083 KCU/HCY
# executable because those two targets include different sets of model headers,
# and combining them would link two TUs that each define the same partial
# specialisations at different include paths (an ODR hazard).
add_executable(ohal_host_tests_stm32u073khc
    main.cpp
    test_gpio_stm32u073kcu.cpp
    test_gpio_stm32u073kbu.cpp
    test_gpio_stm32u073k8u.cpp
    test_gpio_stm32u073cct.cpp
    test_gpio_stm32u073ccu.cpp
    test_gpio_stm32u073cbt.cpp
    test_gpio_stm32u073cbu.cpp
    test_gpio_stm32u073c8t.cpp
    test_gpio_stm32u073c8u.cpp
    test_gpio_stm32u073hcy.cpp
    test_gpio_stm32u073hby.cpp
    test_gpio_stm32u073h8y.cpp)
target_link_libraries(ohal_host_tests_stm32u073khc PRIVATE ohal GTest::gtest_main GTest::gmock)

# STM32U073 R-series tests.  All six variants (r8t, r8i, rbt, rbi, rct, rci)
# delegate to stm32u083rc/gpio.hpp, producing identical Pin<> specialisations.
# They are ODR-safe in one executable.
add_executable(ohal_host_tests_stm32u073r
    main.cpp
    test_gpio_stm32u073rct.cpp
    test_gpio_stm32u073rci.cpp
    test_gpio_stm32u073rbt.cpp
    test_gpio_stm32u073rbi.cpp
    test_gpio_stm32u073r8t.cpp
    test_gpio_stm32u073r8i.cpp)
target_link_libraries(ohal_host_tests_stm32u073r PRIVATE ohal GTest::gtest_main GTest::gmock)

# STM32U073 M(T)-series tests.  All three LQFP80 variants (m8t, mbt, mct)
# delegate to stm32u083mct/gpio.hpp, producing identical Pin<> specialisations.
# Kept separate from the MCI executable: MCT has Pin<PortE, 3> deleted; MCI
# defines it as a bonded GPIO — different full specialisations cause ODR issues.
add_executable(ohal_host_tests_stm32u073mt
    main.cpp
    test_gpio_stm32u073mct.cpp
    test_gpio_stm32u073mbt.cpp
    test_gpio_stm32u073m8t.cpp)
target_link_libraries(ohal_host_tests_stm32u073mt PRIVATE ohal GTest::gtest_main GTest::gmock)

# STM32U073 M(I)-series tests.  All three UFBGA81 variants (m8i, mbi, mci)
# delegate to stm32u083mci/gpio.hpp, producing identical Pin<> specialisations.
add_executable(ohal_host_tests_stm32u073mi
    main.cpp
    test_gpio_stm32u073mci.cpp
    test_gpio_stm32u073mbi.cpp
    test_gpio_stm32u073m8i.cpp)
target_link_libraries(ohal_host_tests_stm32u073mi PRIVATE ohal GTest::gtest_main GTest::gmock)

# STM32U0 PWR register-map wiring tests.  The PWR peripheral is identical
# across all STM32U031/U073/U083 variants (single instance at 0x40007000), so
# a single test executable covers all sub-families.
add_executable(ohal_host_tests_pwr main.cpp test_pwr_stm32u0.cpp)
target_link_libraries(ohal_host_tests_pwr PRIVATE ohal GTest::gtest_main GTest::gmock)

# STM32U0 FLASH register-map wiring tests.  The FLASH peripheral is identical
# across all STM32U031/U073/U083 variants (single instance at 0x40022000), so
# a single test executable covers all sub-families.
add_executable(ohal_host_tests_flash main.cpp test_flash_stm32u0.cpp)
target_link_libraries(ohal_host_tests_flash PRIVATE ohal GTest::gtest_main GTest::gmock)

include(GoogleTest)
gtest_discover_tests(ohal_host_tests)
gtest_discover_tests(ohal_host_tests_cc)
gtest_discover_tests(ohal_host_tests_rct)
gtest_discover_tests(ohal_host_tests_rci)
gtest_discover_tests(ohal_host_tests_mct)
gtest_discover_tests(ohal_host_tests_mci)
gtest_discover_tests(ohal_host_tests_msp430fr2355)
gtest_discover_tests(ohal_host_tests_msp430fr2355tpt)
gtest_discover_tests(ohal_host_tests_msp430fr2355tdbt)
gtest_discover_tests(ohal_host_tests_msp430fr2355trsm)
gtest_discover_tests(ohal_host_tests_msp430fr2355trha)
gtest_discover_tests(ohal_host_tests_stm32u031r)
gtest_discover_tests(ohal_host_tests_stm32u031c)
gtest_discover_tests(ohal_host_tests_stm32u031k)
gtest_discover_tests(ohal_host_tests_stm32u031g)
gtest_discover_tests(ohal_host_tests_timer)
gtest_discover_tests(ohal_host_tests_timer_stm32u031)
gtest_discover_tests(ohal_host_tests_stm32u031f)
gtest_discover_tests(ohal_host_tests_stm32u073khc)
gtest_discover_tests(ohal_host_tests_stm32u073r)
gtest_discover_tests(ohal_host_tests_stm32u073mt)
gtest_discover_tests(ohal_host_tests_stm32u073mi)
gtest_discover_tests(ohal_host_tests_flash)
gtest_discover_tests(ohal_host_tests_pwr)

# ---------------------------------------------------------------------------
# Negative-compile tests (§11.3)
#
# Each test tries to compile a source file that contains an intentional error
# and verifies the compiler rejects it with the expected diagnostic message.
# ---------------------------------------------------------------------------

set(_neg_src "${CMAKE_CURRENT_SOURCE_DIR}/negative")

ohal_expect_compile_failure(
  NegativeCompile_WriteToReadOnlyField
  "${_neg_src}/write_to_readonly_field.cpp"
  "ohal: cannot write to a read-only field"
)

ohal_expect_compile_failure(
  NegativeCompile_ReadFromWriteOnlyField
  "${_neg_src}/read_from_writeonly_field.cpp"
  "ohal: cannot read from a write-only field"
)

ohal_expect_compile_failure(
  NegativeCompile_OverflowBitField
  "${_neg_src}/overflow_bitfield.cpp"
  "ohal: BitField \\(Offset \\+ Width\\) exceeds register width"
)

ohal_expect_compile_failure(
  NegativeCompile_NoFamilyDefined
  "${_neg_src}/no_family_defined.cpp"
  "ohal: No MCU family defined"
)

ohal_expect_compile_failure(
  NegativeCompile_WrongModelForFamily
  "${_neg_src}/wrong_model_for_family.cpp"
  "ohal: No STM32U0 model defined"
)

ohal_expect_compile_failure(
  NegativeCompile_Msp430UnsupportedSpeed
  "${_neg_src}/msp430_unsupported_speed.cpp"
  "ohal: MSP430FR2355 GPIO does not support configurable output speed"
)

ohal_expect_compile_failure(
  NegativeCompile_Msp430UnsupportedOutputType
  "${_neg_src}/msp430_unsupported_output_type.cpp"
  "ohal: MSP430FR2355 GPIO does not support configurable output type"
)
