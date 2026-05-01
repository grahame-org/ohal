#include "mock/mock_register.hpp"

#include <ohal/core/register.hpp>

#include <cstdint>

#include <gtest/gtest.h>

namespace {

// ---------------------------------------------------------------------------
// 32-bit register tests
// ---------------------------------------------------------------------------

static uint32_t reg32_storage{0U};
using Reg32 = ohal::test::MockRegister<uint32_t, &reg32_storage>;

class Register32Test : public ::testing::Test {
protected:
  void SetUp() override { reg32_storage = 0U; }
};

TEST_F(Register32Test, WriteStoresValue) {
  Reg32::write(0xDEAD'BEEFu);
  EXPECT_EQ(reg32_storage, 0xDEAD'BEEFu);
}

TEST_F(Register32Test, ReadReturnsStoredValue) {
  reg32_storage = 0xCAFE'BABEu;
  EXPECT_EQ(Reg32::read(), 0xCAFE'BABEu);
}

// ---------------------------------------------------------------------------
// 8-bit register tests (covers 8-bit MCU platforms such as PIC18)
// ---------------------------------------------------------------------------

static uint8_t reg8_storage{0U};
using Reg8 = ohal::test::MockRegister<uint8_t, &reg8_storage>;

class Register8Test : public ::testing::Test {
protected:
  void SetUp() override { reg8_storage = 0U; }
};

TEST_F(Register8Test, WriteStoresValue) {
  Reg8::write(0xABu);
  // Cast to unsigned so GTest prints a decimal integer rather than a character on failure.
  EXPECT_EQ(static_cast<unsigned>(reg8_storage), 0xABu);
}

TEST_F(Register8Test, ReadReturnsStoredValue) {
  reg8_storage = 0xCDu;
  // Cast to unsigned so GTest prints a decimal integer rather than a character on failure.
  EXPECT_EQ(static_cast<unsigned>(Reg8::read()), 0xCDu);
}

} // namespace
