# Step 1 – Repository and Build Infrastructure

**Goal:** A working CMake project that can build and run host-side tests.

**Inputs required:** None — no hardware knowledge needed at this step.

## Tasks

1. Create root `CMakeLists.txt` requiring CMake ≥ 3.21 and C++17.
2. Add a `tests/host/CMakeLists.txt` using CMake's built-in `FetchContent` to pull a testing
   framework (e.g., [Catch2](https://github.com/catchorg/Catch2) or
   [doctest](https://github.com/doctest/doctest)) at configure time.
3. Add a CMake option `OHAL_BUILD_TESTS` (default `ON`) to enable/disable tests.
4. Add CMake options `OHAL_MCU_FAMILY` and `OHAL_MCU_MODEL` so the family/model can also be passed
   through CMake (these become compile definitions forwarded to the compiler).
5. Configure compiler flags: `-Wall -Wextra -Wpedantic -Werror -std=c++17`.
6. Verify the build succeeds with an empty `main.cpp`.

## Acceptance Criteria

- `cmake -B build && cmake --build build` succeeds.
- `ctest --test-dir build` runs (even with no tests yet).
