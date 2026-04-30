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
endif()
