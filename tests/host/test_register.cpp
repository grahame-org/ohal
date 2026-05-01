#include "mock/mock_register.hpp"

#include <cstdint>

#include <gtest/gtest.h>

namespace {

// ---------------------------------------------------------------------------
// 32-bit register tests
// ---------------------------------------------------------------------------

static uint32_t reg32_storage{0U};
using Reg32 = ohal::core::MockRegister<uint32_t, &reg32_storage>;

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

TEST_F(Register32Test, SetBitsOrsMask) {
  reg32_storage = 0x0000'0F0Fu;
  Reg32::set_bits(0xF0F0'0000u);
  EXPECT_EQ(reg32_storage, 0xF0F0'0F0Fu);
}

TEST_F(Register32Test, SetBitsDoesNotDisturbOtherBits) {
  reg32_storage = 0xFFFF'FFFFu;
  Reg32::set_bits(0x0000'0001u);
  EXPECT_EQ(reg32_storage, 0xFFFF'FFFFu);
}

TEST_F(Register32Test, ClearBitsClearsMask) {
  reg32_storage = 0xFFFF'FFFFu;
  Reg32::clear_bits(0x0F0F'0F0Fu);
  EXPECT_EQ(reg32_storage, 0xF0F0'F0F0u);
}

TEST_F(Register32Test, ClearBitsDoesNotDisturbOtherBits) {
  reg32_storage = 0x0000'0000u;
  Reg32::clear_bits(0x0000'0001u);
  EXPECT_EQ(reg32_storage, 0x0000'0000u);
}

TEST_F(Register32Test, ModifyAppliesClearThenSet) {
  reg32_storage = 0xFFFF'0000u;
  Reg32::modify(0xFFFF'0000u, 0x0000'FFFFu);
  EXPECT_EQ(reg32_storage, 0x0000'FFFFu);
}

TEST_F(Register32Test, ModifyPreservesUntouchedBits) {
  reg32_storage = 0xABCD'EF01u;
  Reg32::modify(0x0000'FF00u, 0x0000'1200u);
  EXPECT_EQ(reg32_storage, 0xABCD'1201u);
}

// ---------------------------------------------------------------------------
// 8-bit register tests (covers 8-bit MCU platforms such as PIC18)
// ---------------------------------------------------------------------------

static uint8_t reg8_storage{0U};
using Reg8 = ohal::core::MockRegister<uint8_t, &reg8_storage>;

class Register8Test : public ::testing::Test {
protected:
  void SetUp() override { reg8_storage = 0U; }
};

TEST_F(Register8Test, WriteStoresValue) {
  Reg8::write(0xABu);
  EXPECT_EQ(reg8_storage, 0xABu);
}

TEST_F(Register8Test, ReadReturnsStoredValue) {
  reg8_storage = 0xCDu;
  EXPECT_EQ(Reg8::read(), 0xCDu);
}

TEST_F(Register8Test, SetBitsOrsMask) {
  reg8_storage = 0x0Fu;
  Reg8::set_bits(0xF0u);
  EXPECT_EQ(reg8_storage, 0xFFu);
}

TEST_F(Register8Test, SetBitsDoesNotDisturbOtherBits) {
  reg8_storage = 0xFFu;
  Reg8::set_bits(0x01u);
  EXPECT_EQ(reg8_storage, 0xFFu);
}

TEST_F(Register8Test, ClearBitsClearsMask) {
  reg8_storage = 0xFFu;
  Reg8::clear_bits(0x0Fu);
  EXPECT_EQ(reg8_storage, 0xF0u);
}

TEST_F(Register8Test, ClearBitsDoesNotDisturbOtherBits) {
  reg8_storage = 0x00u;
  Reg8::clear_bits(0x01u);
  EXPECT_EQ(reg8_storage, 0x00u);
}

TEST_F(Register8Test, ModifyAppliesClearThenSet) {
  reg8_storage = 0xF0u;
  Reg8::modify(0xF0u, 0x0Fu);
  EXPECT_EQ(reg8_storage, 0x0Fu);
}

TEST_F(Register8Test, ModifyPreservesUntouchedBits) {
  reg8_storage = 0b1010'1010u;
  Reg8::modify(0b1111'0000u, 0b0101'0000u);
  EXPECT_EQ(reg8_storage, 0b0101'1010u);
}

} // namespace
