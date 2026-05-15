cmake_minimum_required(VERSION 3.21)
project(ohal-stm32u083-check LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ohal is header-only; reference the headers directly from the source tree.
add_library(ohal INTERFACE)
target_include_directories(ohal INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../../../include)
target_compile_definitions(ohal INTERFACE OHAL_FAMILY_STM32U0 OHAL_MODEL_STM32U083KCU)

add_library(blink STATIC blink.cpp)
target_link_libraries(blink PRIVATE ohal)
target_compile_options(
  blink
  PRIVATE -mcpu=cortex-m0plus
          -mthumb
          -mfloat-abi=soft
          -Os
          -ffunction-sections
          -fdata-sections
          -fno-exceptions
          -fno-rtti
          -Wall
          -Wextra
)
