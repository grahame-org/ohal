set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# Use STATIC_LIBRARY mode so CMake does not need a linker script or startup
# file when testing the toolchain at configure time.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
# cmake-lint: disable=C0103
set(CMAKE_AR arm-none-eabi-gcc-ar CACHE FILEPATH "ar tool")
# cmake-lint: disable=C0103
set(CMAKE_RANLIB arm-none-eabi-gcc-ranlib CACHE FILEPATH "ranlib tool")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
