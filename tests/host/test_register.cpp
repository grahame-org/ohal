#include "mock/mock_register.hpp"

#include <ohal/core/register.hpp>

#include <cstdint>

#include <gtest/gtest.h>

namespace {

// ---------------------------------------------------------------------------
// Per-width test configurations
// ---------------------------------------------------------------------------

static uint32_t reg32_storage{0U};
static uint8_t reg8_storage{0U};

struct Config32 {
  using StorageType = uint32_t;
  using Reg = ohal::test::MockRegister<uint32_t, &reg32_storage>;
  static constexpr uint32_t write_val = 0xDEAD'BEEFu;
  static constexpr uint32_t read_val = 0xCAFE'BABEu;
  static void reset() noexcept { reg32_storage = 0U; }
  static StorageType& storage() noexcept { return reg32_storage; }
};

// Config for 8-bit MCU platforms such as PIC18.
struct Config8 {
  using StorageType = uint8_t;
  using Reg = ohal::test::MockRegister<uint8_t, &reg8_storage>;
  static constexpr uint8_t write_val = 0xABu;
  static constexpr uint8_t read_val = 0xCDu;
  static void reset() noexcept { reg8_storage = 0U; }
  static StorageType& storage() noexcept { return reg8_storage; }
};

// ---------------------------------------------------------------------------
// Typed test suite (one test body exercised for every register-width config)
// ---------------------------------------------------------------------------

template <typename Config>
class RegisterTest : public ::testing::Test {
protected:
  void SetUp() override { Config::reset(); }
};

using RegisterConfigs = ::testing::Types<Config32, Config8>;
TYPED_TEST_SUITE(RegisterTest, RegisterConfigs);

TYPED_TEST(RegisterTest, WriteStoresValue) {
  using Reg = typename TypeParam::Reg;
  Reg::write(TypeParam::write_val);
  // Cast to uint32_t so GTest prints a decimal integer rather than a character on failure.
  EXPECT_EQ(static_cast<uint32_t>(TypeParam::storage()),
            static_cast<uint32_t>(TypeParam::write_val));
}

TYPED_TEST(RegisterTest, ReadReturnsStoredValue) {
  TypeParam::storage() = TypeParam::read_val;
  using Reg = typename TypeParam::Reg;
  // Cast to uint32_t so GTest prints a decimal integer rather than a character on failure.
  EXPECT_EQ(static_cast<uint32_t>(Reg::read()), static_cast<uint32_t>(TypeParam::read_val));
}

} // namespace
