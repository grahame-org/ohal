// tests/target/main.cpp
//
// Minimal bare-metal test runner for on-target unit tests (§11.2).
//
// Entry point for the ohal_target_tests firmware image.  Runs each registered
// test function, counts pass/fail, and reports the result over UART.
//
// This file is a framework placeholder.  Concrete test functions are linked in
// from the same source files used for host-side tests (test_register.cpp,
// test_bitfield.cpp, etc.) when this target is cross-compiled.  The runner is
// intentionally minimal so it can operate before the C++ runtime is fully
// initialised (no heap, no exceptions, no RTTI).

#include <cstdint>

// ---------------------------------------------------------------------------
// Bare-metal test API — implemented here, declared in test_runner.hpp.
// ---------------------------------------------------------------------------

namespace ohal::target_test {

static unsigned pass_count = 0U;
static unsigned fail_count = 0U;

void report_pass() noexcept { ++pass_count; }
void report_fail() noexcept { ++fail_count; }

} // namespace ohal::target_test

// ---------------------------------------------------------------------------
// Entry point
// ---------------------------------------------------------------------------

// NOLINTNEXTLINE(bugprone-exception-escape)
int main() {
  // TODO: call registered test functions here when they are wired up.
  // For each test, call ohal::target_test::report_pass() or report_fail().

  // Signal result via a simple spin-loop.  Replace with UART output once the
  // BSP provides a UART initialisation routine.
  while (true) {
    // A debugger can inspect pass_count / fail_count here.
    (void)ohal::target_test::pass_count;
    (void)ohal::target_test::fail_count;
  }
}
