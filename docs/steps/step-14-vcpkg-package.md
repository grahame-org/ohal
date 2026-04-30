# Step 14 – vcpkg Package

**Goal:** Publish OHAL as a `vcpkg` package so that consumers can add it as a dependency with a
single manifest entry, and wire the same vcpkg package into the OHAL test project itself to
validate the integration end-to-end.

**Prerequisite:** [Step 13 (Release Automation)](step-13-release-automation.md) must be in
place first — `release-please` is responsible for automatically bumping the `version` field in
`vcpkg.json` and `ports/ohal/vcpkg.json` on each release.

## 14.1 Why vcpkg

`vcpkg` is the standard C++ package manager supported natively by CMake and Visual Studio.
Making OHAL a vcpkg package means consumers avoid manual `FetchContent` wiring and get automatic
version pinning, patch management, and IDE integration.

## 14.2 Files to Create

### `vcpkg.json` (manifest at repo root)

```json
{
  "name": "ohal",
  "version": "0.1.0",
  "description": "Zero-overhead, header-only Hardware Abstraction Layer for embedded MCUs",
  "homepage": "https://github.com/grahame-org/ohal",
  "license": "MIT",
  "supports": "!(windows | osx)",
  "dependencies": []
}
```

Notes:
- `dependencies` is empty because OHAL itself has no runtime dependencies.
- `supports` can be relaxed once host-only builds on Windows/macOS are validated.
- The `version` field is kept in sync with `CMakeLists.txt` by `release-please`
  (see [Step 13](step-13-release-automation.md)).

### `cmake/ohal-config.cmake.in`

The CMake package config file enables `find_package(ohal CONFIG REQUIRED)` after installation
or consumption through vcpkg:

```cmake
# cmake/ohal-config.cmake.in
@PACKAGE_INIT@

include("${CMAKE_CURRENT_LIST_DIR}/ohal-targets.cmake")

check_required_components(ohal)
```

### CMakeLists.txt install target additions

```cmake
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(ohal INTERFACE)
target_include_directories(ohal INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(TARGETS ohal EXPORT ohal-targets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT ohal-targets
    FILE ohal-targets.cmake
    NAMESPACE ohal::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ohal
)

configure_package_config_file(
    cmake/ohal-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/ohal-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ohal
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/ohal-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/ohal-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/ohal-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ohal
)
```

### `ports/ohal/portfile.cmake`

If OHAL is published to a private vcpkg registry or as an overlay port:

```cmake
# ports/ohal/portfile.cmake
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO grahame-org/ohal
    REF "v${VERSION}"
    SHA512 <sha512-of-release-tarball>
    HEAD_REF main
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DOHAL_BUILD_TESTS=OFF
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(PACKAGE_NAME ohal CONFIG_PATH lib/cmake/ohal)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
```

### `ports/ohal/vcpkg.json`

```json
{
  "name": "ohal",
  "version": "0.1.0",
  "description": "Zero-overhead, header-only Hardware Abstraction Layer for embedded MCUs",
  "dependencies": [
    { "name": "vcpkg-cmake", "host": true },
    { "name": "vcpkg-cmake-config", "host": true }
  ]
}
```

## 14.3 Using the Package from the Test Project

The OHAL repository uses vcpkg to consume its own installed package in an integration test
build. This validates the install target and the `find_package` path end-to-end.

### `tests/integration/CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.21)
project(ohal_integration_test LANGUAGES CXX)

find_package(ohal CONFIG REQUIRED)

add_executable(blink_stm32 blink_stm32.cpp)
target_link_libraries(blink_stm32 PRIVATE ohal::ohal)
target_compile_definitions(blink_stm32 PRIVATE
    OHAL_FAMILY_STM32U0 OHAL_MODEL_STM32U083)
target_compile_features(blink_stm32 PRIVATE cxx_std_17)

add_executable(blink_pic18 blink_pic18.cpp)
target_link_libraries(blink_pic18 PRIVATE ohal::ohal)
target_compile_definitions(blink_pic18 PRIVATE
    OHAL_FAMILY_PIC OHAL_MODEL_PIC18F4550)
target_compile_features(blink_pic18 PRIVATE cxx_std_17)
```

### `tests/integration/vcpkg.json`

```json
{
  "dependencies": [
    { "name": "ohal", "version>=": "0.1.0" }
  ]
}
```

### Integration Test CI Job

Add a job to the CI workflow (see [Step 12](step-12-ci.md)):

1. Install the current HEAD as a vcpkg overlay port (`--overlay-ports=ports/`).
2. Configure and build `tests/integration/` using `CMAKE_TOOLCHAIN_FILE` pointing to vcpkg's
   toolchain.
3. Verify both `blink_stm32` and `blink_pic18` compile without errors.

This job runs on the host with `g++` (cross-compile flags are not needed for the
`find_package` validation). It validates the full `find_package(ohal CONFIG REQUIRED)` path.

## 14.4 Version Synchronisation

`release-please` (Step 13) automatically bumps the `version` field in `vcpkg.json` and
`ports/ohal/vcpkg.json` as part of each release PR. Both files are listed in
`release-please-config.json` under `extra-files` (see [Step 13](step-13-release-automation.md)).
The `SHA512` in `portfile.cmake` must be updated manually when the release tarball changes;
this can be scripted as part of the release workflow.
