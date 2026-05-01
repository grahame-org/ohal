#include <ohal/core/capabilities.hpp>
#include <ohal/gpio.hpp>

#include <gtest/gtest.h>

// ---------------------------------------------------------------------------
// Enum value tests
// ---------------------------------------------------------------------------

TEST(GpioPinModeTest, EnumValues) {
  EXPECT_EQ(static_cast<unsigned>(ohal::gpio::PinMode::Input), 0U);
  EXPECT_EQ(static_cast<unsigned>(ohal::gpio::PinMode::Output), 1U);
  EXPECT_EQ(static_cast<unsigned>(ohal::gpio::PinMode::AlternateFunction), 2U);
  EXPECT_EQ(static_cast<unsigned>(ohal::gpio::PinMode::Analog), 3U);
}

TEST(GpioOutputTypeTest, EnumValues) {
  EXPECT_EQ(static_cast<unsigned>(ohal::gpio::OutputType::PushPull), 0U);
  EXPECT_EQ(static_cast<unsigned>(ohal::gpio::OutputType::OpenDrain), 1U);
}

TEST(GpioSpeedTest, EnumValues) {
  EXPECT_EQ(static_cast<unsigned>(ohal::gpio::Speed::Low), 0U);
  EXPECT_EQ(static_cast<unsigned>(ohal::gpio::Speed::Medium), 1U);
  EXPECT_EQ(static_cast<unsigned>(ohal::gpio::Speed::High), 2U);
  EXPECT_EQ(static_cast<unsigned>(ohal::gpio::Speed::VeryHigh), 3U);
}

TEST(GpioPullTest, EnumValues) {
  EXPECT_EQ(static_cast<unsigned>(ohal::gpio::Pull::None), 0U);
  EXPECT_EQ(static_cast<unsigned>(ohal::gpio::Pull::Up), 1U);
  EXPECT_EQ(static_cast<unsigned>(ohal::gpio::Pull::Down), 2U);
}

TEST(GpioLevelTest, EnumValues) {
  EXPECT_EQ(static_cast<unsigned>(ohal::gpio::Level::Low), 0U);
  EXPECT_EQ(static_cast<unsigned>(ohal::gpio::Level::High), 1U);
}

// ---------------------------------------------------------------------------
// Capability trait default tests
// ---------------------------------------------------------------------------
// The primary templates default to false_type for any (Port, PinNum) pair
// that has not been specialised by a platform header.

namespace {

// Use a dummy port type (not one of the real port tags) to ensure no platform
// specialisation could accidentally match.
struct DummyPort {};

static_assert(!ohal::gpio::capabilities::supports_output_type<DummyPort, 0>::value,
              "supports_output_type must default to false");
static_assert(!ohal::gpio::capabilities::supports_output_speed<DummyPort, 0>::value,
              "supports_output_speed must default to false");
static_assert(!ohal::gpio::capabilities::supports_pull<DummyPort, 0>::value,
              "supports_pull must default to false");
static_assert(!ohal::gpio::capabilities::supports_alternate_function<DummyPort, 0>::value,
              "supports_alternate_function must default to false");

} // namespace

TEST(GpioCapabilitiesTest, DefaultToFalse) {
  EXPECT_FALSE((ohal::gpio::capabilities::supports_output_type<DummyPort, 0>::value));
  EXPECT_FALSE((ohal::gpio::capabilities::supports_output_speed<DummyPort, 0>::value));
  EXPECT_FALSE((ohal::gpio::capabilities::supports_pull<DummyPort, 0>::value));
  EXPECT_FALSE((ohal::gpio::capabilities::supports_alternate_function<DummyPort, 0>::value));
}

// ---------------------------------------------------------------------------
// Negative-compile tests (manual verification only)
// ---------------------------------------------------------------------------
//
// The following illegal usage must NOT compile.  Verify by temporarily
// uncommenting the instantiation and confirming the static_assert fires:
//
//   ohal::gpio::Pin<ohal::gpio::PortA, 0> pin;
//   // expected: "ohal: gpio::Pin is not implemented for the selected MCU. ..."
//
// (The primary template fires because no platform specialisation is present
// in a build that does not define OHAL_FAMILY_* and OHAL_MODEL_*.)
