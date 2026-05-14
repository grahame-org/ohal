# Copilot instructions for the `ohal` repository

## Completing work on a PR

**Always run `bash lint.sh` from the repository root before pushing any
changes to a PR or marking a PR ready for review.**

The lint script checks:

- Prettier formatting on all `*.md`, `*.yml`, `*.yaml`, and `*.json` files
- `clang-format` on all C++ source and header files
- `clang-tidy` on all C++ sources (compiled against each test target)
- `cmake-lint` on CMake files
- `yamllint` on YAML files
- JSON schema validation of all model specs
- `shellcheck` on shell scripts
- `markdownlint` on Markdown files

Any failure will cause CI to fail. Fix all lint errors before pushing.

> **Note:** `lint.sh` scans all CMake files in the tree, including those
> generated inside the `build/` directory. Remove `build/` before running
> `lint.sh`:
>
> ```sh
> rm -rf build/
> bash lint.sh
> ```

## Commit messages

All commit messages and PR titles **must** follow the
[Conventional Commits](https://www.conventionalcommits.org/) format:

```
<type>(<scope>): <short summary>
```

- **type**: `feat` | `fix` | `docs` | `test` | `ci` | `build` | `refactor` | `chore`
- **scope**: optional — e.g. `core` | `gpio` | `stm32u0` | `ci` | `vcpkg`
- **summary**: present tense, lowercase, no trailing period

Examples:

```
feat(gpio): add STM32U073 partial specialisation
fix(core): correct mask calculation for Width=1 BitField at Offset=31
docs(plan): add step-16 additional peripherals
ci: add nRF5 cross-compile job
```

The PR title becomes the squash-merge commit message and is used by
release-please to determine version bumps and CHANGELOG entries. CI enforces
this via the **Conventional Commits** workflow — a non-conforming title will
block the PR.

## Pull request template

The repository provides a pull request template at
`.github/PULL_REQUEST_TEMPLATE.md`. **Always** read the template before
opening or updating a PR and fill in every section. Do not delete or skip any
section headings; leave placeholder text only when a section genuinely does not
apply to the change.

## Build and test

```sh
cmake -B build -DOHAL_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build
```

## Build test_project (cross-compiled firmware)

The `test_project` directory contains an STM32U083 firmware project. Build it
with the Ubuntu-packaged ARM bare-metal cross-compiler (`gcc-arm-none-eabi`).

The cross-compiler is pre-installed in the Copilot dev environment. To build:

```sh
cd test_project
cmake -B build -DCMAKE_TOOLCHAIN_FILE=arm-none-eabi-gcc.cmake -G "Unix Makefiles"
cmake --build build -j "$(nproc)"
```

The resulting ELF, HEX, and BIN files are written into `test_project/build/`.

## Adding a new STM32U0 model

1. Add per-package `capabilities.hpp`, `gpio.hpp`, `timer.hpp`, and
   `uart.hpp` under `include/ohal/platforms/stm32u0/models/<model>/`.
2. Register the new `OHAL_MODEL_*` macro in
   `include/ohal/platforms/stm32u0/family.hpp` (conflict-count check **and**
   the `#include` chain).
3. Add a model spec YAML under `docs/specs/stm32/models/<model>.yml` and
   validate it passes `bash lint.sh`.
4. Add a host test file `tests/host/test_gpio_<model>.cpp` and wire it up in
   `tests/host/CMakeLists.txt`.
5. **ODR rule:** each `add_executable` in `tests/host/CMakeLists.txt` must
   link only test files whose headers provide an identical (or non-conflicting)
   set of `ohal::gpio::Pin<>` / capability-trait explicit specialisations.
   Models that introduce different full specialisations for the same
   `Pin<Port, N>` template instantiation **must** be placed in separate
   executables.
