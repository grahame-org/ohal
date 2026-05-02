#include <ohal/core/capabilities.hpp>
#include <ohal/gpio.hpp>

#include <gtest/gtest.h>

// ---------------------------------------------------------------------------
// Enum value tests (value-parameterised — one assertion per test instance)
// ---------------------------------------------------------------------------

namespace {

struct EnumCase {
  unsigned actual;
  unsigned expected;
  const char* name;
};

// --- PinMode ---

class GpioPinModeTest : public ::testing::TestWithParam<EnumCase> {};

INSTANTIATE_TEST_SUITE_P(
    PinModeValues, GpioPinModeTest,
    ::testing::Values(EnumCase{static_cast<unsigned>(ohal::gpio::PinMode::Input), 0U, "Input"},
                      EnumCase{static_cast<unsigned>(ohal::gpio::PinMode::Output), 1U, "Output"},
                      EnumCase{static_cast<unsigned>(ohal::gpio::PinMode::AlternateFunction), 2U,
                               "AlternateFunction"},
                      EnumCase{static_cast<unsigned>(ohal::gpio::PinMode::Analog), 3U, "Analog"}),
    [](const ::testing::TestParamInfo<EnumCase>& info) { return info.param.name; });

TEST_P(GpioPinModeTest, ValueIsCorrect) {
  const auto& p = GetParam();
  EXPECT_EQ(p.actual, p.expected);
}

// --- OutputType ---

class GpioOutputTypeTest : public ::testing::TestWithParam<EnumCase> {};

INSTANTIATE_TEST_SUITE_P(
    OutputTypeValues, GpioOutputTypeTest,
    ::testing::Values(
        EnumCase{static_cast<unsigned>(ohal::gpio::OutputType::PushPull), 0U, "PushPull"},
        EnumCase{static_cast<unsigned>(ohal::gpio::OutputType::OpenDrain), 1U, "OpenDrain"}),
    [](const ::testing::TestParamInfo<EnumCase>& info) { return info.param.name; });

TEST_P(GpioOutputTypeTest, ValueIsCorrect) {
  const auto& p = GetParam();
  EXPECT_EQ(p.actual, p.expected);
}

// --- Speed ---

class GpioSpeedTest : public ::testing::TestWithParam<EnumCase> {};

INSTANTIATE_TEST_SUITE_P(
    SpeedValues, GpioSpeedTest,
    ::testing::Values(EnumCase{static_cast<unsigned>(ohal::gpio::Speed::Low), 0U, "Low"},
                      EnumCase{static_cast<unsigned>(ohal::gpio::Speed::Medium), 1U, "Medium"},
                      EnumCase{static_cast<unsigned>(ohal::gpio::Speed::High), 2U, "High"},
                      EnumCase{static_cast<unsigned>(ohal::gpio::Speed::VeryHigh), 3U, "VeryHigh"}),
    [](const ::testing::TestParamInfo<EnumCase>& info) { return info.param.name; });

TEST_P(GpioSpeedTest, ValueIsCorrect) {
  const auto& p = GetParam();
  EXPECT_EQ(p.actual, p.expected);
}

// --- Pull ---

class GpioPullTest : public ::testing::TestWithParam<EnumCase> {};

INSTANTIATE_TEST_SUITE_P(
    PullValues, GpioPullTest,
    ::testing::Values(EnumCase{static_cast<unsigned>(ohal::gpio::Pull::None), 0U, "None"},
                      EnumCase{static_cast<unsigned>(ohal::gpio::Pull::Up), 1U, "Up"},
                      EnumCase{static_cast<unsigned>(ohal::gpio::Pull::Down), 2U, "Down"}),
    [](const ::testing::TestParamInfo<EnumCase>& info) { return info.param.name; });

TEST_P(GpioPullTest, ValueIsCorrect) {
  const auto& p = GetParam();
  EXPECT_EQ(p.actual, p.expected);
}

// --- Level ---

class GpioLevelTest : public ::testing::TestWithParam<EnumCase> {};

INSTANTIATE_TEST_SUITE_P(
    LevelValues, GpioLevelTest,
    ::testing::Values(EnumCase{static_cast<unsigned>(ohal::gpio::Level::Low), 0U, "Low"},
                      EnumCase{static_cast<unsigned>(ohal::gpio::Level::High), 1U, "High"}),
    [](const ::testing::TestParamInfo<EnumCase>& info) { return info.param.name; });

TEST_P(GpioLevelTest, ValueIsCorrect) {
  const auto& p = GetParam();
  EXPECT_EQ(p.actual, p.expected);
}

// ---------------------------------------------------------------------------
// Capability trait default tests (one test per trait)
// ---------------------------------------------------------------------------
// The primary templates default to false_type for any (Port, PinNum) pair
// that has not been specialised by a platform header.

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

TEST(GpioCapabilitiesTest, SupportsOutputTypeDefaultsFalse) {
  EXPECT_FALSE((ohal::gpio::capabilities::supports_output_type<DummyPort, 0>::value));
}

TEST(GpioCapabilitiesTest, SupportsOutputSpeedDefaultsFalse) {
  EXPECT_FALSE((ohal::gpio::capabilities::supports_output_speed<DummyPort, 0>::value));
}

TEST(GpioCapabilitiesTest, SupportsPullDefaultsFalse) {
  EXPECT_FALSE((ohal::gpio::capabilities::supports_pull<DummyPort, 0>::value));
}

TEST(GpioCapabilitiesTest, SupportsAlternateFunctionDefaultsFalse) {
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

} // namespace
