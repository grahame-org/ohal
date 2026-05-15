# expect_compile_failure.cmake
#
# Invoked by ctest for each ohal_expect_compile_failure test.
# Compiles SOURCE with the given compiler and include paths, then checks that:
#   1. Compilation exits with a non-zero status (i.e. the build fails).
#   2. The compiler's diagnostic output contains EXPECTED_ERROR.
#
# Required variables (supplied via -D on the cmake command line):
#   SOURCE          Path to the C++ source file to compile.
#   EXPECTED_ERROR  Substring expected somewhere in the compiler diagnostic output.
#   CXX_COMPILER    Path to the C++ compiler executable.
#   CXX_STANDARD    Numeric C++ standard to use (e.g. 17).
#   INCLUDES        Pipe-separated list of include directories.

# Decode the pipe-separated include list into -I<dir> flags.
string(REPLACE "|" ";" INCLUDE_LIST "${INCLUDES}")
set(INCLUDE_FLAGS "")
foreach(include_dir IN LISTS INCLUDE_LIST)
  list(APPEND INCLUDE_FLAGS "-I${include_dir}")
endforeach()

execute_process(
  COMMAND
    "${CXX_COMPILER}"
    "-std=c++${CXX_STANDARD}"
    -Wall
    -Wextra
    -fsyntax-only
    ${INCLUDE_FLAGS}
    "${SOURCE}"
  RESULT_VARIABLE _result
  OUTPUT_VARIABLE _stdout
  ERROR_VARIABLE  _stderr
)

# Merge stdout and stderr for the pattern search.
set(_combined "${_stdout}\n${_stderr}")

if(_result EQUAL 0)
  message(
    FATAL_ERROR
    "Expected compilation to fail but it succeeded.\n"
    "Source:   ${SOURCE}\n"
    "Expected error string: ${EXPECTED_ERROR}"
  )
endif()

if(NOT _combined MATCHES "${EXPECTED_ERROR}")
  message(
    FATAL_ERROR
    "Expected error string not found in compiler output.\n"
    "Source:   ${SOURCE}\n"
    "Expected: ${EXPECTED_ERROR}\n"
    "Compiler output:\n${_combined}"
  )
endif()
