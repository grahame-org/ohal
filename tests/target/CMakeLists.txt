# tests/target/CMakeLists.txt
#
# On-target test framework (§11.2).
#
# This sub-tree is only configured when CMake is invoked with a cross-compiler
# toolchain (i.e. when CMAKE_CROSSCOMPILING is set).  It compiles the same
# test source files used by the host-side tests against the real platform
# headers and a minimal bare-metal test runner.
#
# Usage (ARM/STM32U083):
#   cmake -B build-target \
#         -DCMAKE_TOOLCHAIN_FILE=test_project/arm-none-eabi-gcc.cmake \
#         -DOHAL_MCU_FAMILY=STM32U0 \
#         -DOHAL_MCU_MODEL=STM32U083RCT \
#         -DOHAL_BUILD_TARGET_TESTS=ON
#
# The resulting ELF reports pass/fail counts over UART (115200 baud, 8N1).

if(NOT CMAKE_CROSSCOMPILING)
  return()
endif()

# ---------------------------------------------------------------------------
# Minimal bare-metal test runner
# ---------------------------------------------------------------------------

add_executable(ohal_target_tests main.cpp)
target_link_libraries(ohal_target_tests PRIVATE ohal)
target_compile_options(ohal_target_tests PRIVATE
  -Wall -Wextra -Wpedantic -Werror
  -fno-exceptions -fno-rtti
  -Os
)
