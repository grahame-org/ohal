// Negative-compile test: calling set_speed() on an MSP430FR2355 GPIO pin must be
// rejected at compile time because the platform does not support configurable speed.
// Expected static_assert: "ohal: MSP430FR2355 GPIO does not support configurable output speed."

#include <ohal/gpio.hpp>
#include <ohal/platforms/msp430fr2xx/models/msp430fr2355/gpio.hpp>

#include <cstdint>

namespace {

static uint8_t in_reg{};
static uint8_t out_reg{};
static uint8_t dir_reg{};
static uint8_t ren_reg{};
static uint8_t sel0_reg{};
static uint8_t sel1_reg{};

template <typename T, T* S>
struct MockReg8 {
  using value_type = T;
  [[nodiscard]] static T read() noexcept { return *S; }
  static void write(T v) noexcept { *S = v; }
};

struct MockRegs {
  using In = MockReg8<uint8_t, &in_reg>;
  using Out = MockReg8<uint8_t, &out_reg>;
  using Dir = MockReg8<uint8_t, &dir_reg>;
  using Ren = MockReg8<uint8_t, &ren_reg>;
  using Sel0 = MockReg8<uint8_t, &sel0_reg>;
  using Sel1 = MockReg8<uint8_t, &sel1_reg>;
};

using Pin2 = ohal::platforms::msp430fr2xx::msp430fr2355::GpioPortPinImpl<2U, MockRegs>;

void test() {
  // NOLINTNEXTLINE — intentional: triggers "does not support configurable output speed"
  Pin2::set_speed(ohal::gpio::Speed::High);
}

} // namespace
