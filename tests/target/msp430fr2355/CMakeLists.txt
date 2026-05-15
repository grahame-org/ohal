cmake_minimum_required(VERSION 3.21)
project(ohal-msp430fr2355-check LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ohal is header-only; reference the headers directly from the source tree.
add_library(ohal INTERFACE)
target_include_directories(ohal INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../../../include)
target_compile_definitions(ohal INTERFACE OHAL_FAMILY_MSP430FR2XX OHAL_MODEL_MSP430FR2355)

add_library(blink STATIC blink.cpp)
target_link_libraries(blink PRIVATE ohal)
target_compile_options(
  blink
  PRIVATE -mmcu=msp430fr2355
          -Os
          -ffunction-sections
          -fdata-sections
          -fno-exceptions
          -fno-rtti
          -Wall
          -Wextra
)
