# cmake/expect_compile_failure.cmake
#
# Compile a source file and verify that:
#   1. Compilation fails (non-zero exit code).
#   2. The expected error message appears in the compiler output.
#
# Required variables (pass via -D on the cmake command line):
#   CXX_COMPILER   Path to the C++ compiler binary.
#   INCLUDE_DIR    Top-level OHAL include directory (-I path).
#   SOURCE         Absolute path to the source file to compile.
#   EXPECTED_ERROR Substring expected to appear in the compiler output.
#   OUTPUT_OBJECT  Path for the compiler output object file (use a build-dir
#                  temp path; on POSIX this is discarded, but naming it
#                  explicitly keeps the script portable to non-POSIX hosts).
#
# Optional variables:
#   EXTRA_DEFINES  Pipe-separated list of preprocessor definitions to add
#                  (e.g. "OHAL_FAMILY_STM32U0|OHAL_MODEL_MSP430FR2355").
#                  Pipe is used as the separator so that semicolons are not
#                  mis-interpreted as CMake list separators.

cmake_minimum_required(VERSION 3.21)

foreach(var CXX_COMPILER INCLUDE_DIR SOURCE EXPECTED_ERROR OUTPUT_OBJECT)
  if(NOT DEFINED ${var})
    message(FATAL_ERROR "expect_compile_failure.cmake: ${var} is not defined")
  endif()
endforeach()

set(COMPILE_ARGS "-std=c++17" "-I${INCLUDE_DIR}" "-c" "${SOURCE}" "-o" "${OUTPUT_OBJECT}")

if(DEFINED EXTRA_DEFINES AND NOT EXTRA_DEFINES STREQUAL "")
  string(REPLACE "|" ";" extra_define_list "${EXTRA_DEFINES}")
  foreach(def IN LISTS extra_define_list)
    list(APPEND COMPILE_ARGS "-D${def}")
  endforeach()
endif()

execute_process(
  COMMAND "${CXX_COMPILER}" ${COMPILE_ARGS}
  RESULT_VARIABLE compile_result
  ERROR_VARIABLE compile_error
  OUTPUT_QUIET
)

if(compile_result EQUAL 0)
  message(
    FATAL_ERROR
    "Expected compilation to fail but it succeeded.\n"
    "Source: ${SOURCE}\n"
    "Expected error: '${EXPECTED_ERROR}'"
  )
endif()

string(FIND "${compile_error}" "${EXPECTED_ERROR}" error_pos)
if(error_pos EQUAL -1)
  message(
    FATAL_ERROR
    "Compilation failed as expected but the required message was not found.\n"
    "Expected: '${EXPECTED_ERROR}'\n"
    "Compiler output:\n${compile_error}"
  )
endif()
