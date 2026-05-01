#include "mock/mock_register.hpp"

#include <ohal/core/access.hpp>
#include <ohal/core/field.hpp>

#include <cstdint>

#include <gtest/gtest.h>

// ---------------------------------------------------------------------------
// Test fixtures
// ---------------------------------------------------------------------------

namespace {

static uint32_t field32_storage{0U};
using Reg32 = ohal::test::MockRegister<uint32_t, &field32_storage>;

static uint8_t field8_storage{0U};
using Reg8 = ohal::test::MockRegister<uint8_t, &field8_storage>;

class BitField32Test : public ::testing::Test {
protected:
  void SetUp() override { field32_storage = 0U; }
};

class BitField8Test : public ::testing::Test {
protected:
  void SetUp() override { field8_storage = 0U; }
};

// ---------------------------------------------------------------------------
// 32-bit ReadWrite field tests
// ---------------------------------------------------------------------------

// A 3-bit ReadWrite field at bits [6:4] in a 32-bit register.
using RwField = ohal::core::BitField<Reg32, 4, 3, ohal::core::Access::ReadWrite>;

TEST_F(BitField32Test, ReadWriteField_ReadReturnsCorrectBits) {
  // Bits [6:4] = 0b101 = 5
  field32_storage = 0b0101'0000U;
  EXPECT_EQ(static_cast<uint32_t>(RwField::read()), 5U);
}

TEST_F(BitField32Test, ReadWriteField_ReadIgnoresOtherBits) {
  // Set other bits outside [6:4] to 1 to ensure they are masked out.
  field32_storage = ~0b0111'0000U;
  EXPECT_EQ(static_cast<uint32_t>(RwField::read()), 0U);
}

TEST_F(BitField32Test, ReadWriteField_WriteUpdatesCorrectBits) {
  RwField::write(static_cast<uint32_t>(0b110U));
  EXPECT_EQ(field32_storage & 0b0111'0000U, 0b0110'0000U);
}

TEST_F(BitField32Test, ReadWriteField_WritePreservesOtherBits) {
  // Pre-load the register with all-ones outside the field.
  field32_storage = ~0b0111'0000U;
  RwField::write(static_cast<uint32_t>(0b101U));
  // Other bits must still be 1.
  EXPECT_EQ(field32_storage & ~0b0111'0000U, ~0b0111'0000U);
  // Field bits must reflect the new value.
  EXPECT_EQ(field32_storage & 0b0111'0000U, 0b0101'0000U);
}

TEST_F(BitField32Test, ReadWriteField_WriteTruncatesExtraBits) {
  // Writing a value with bits set outside the field width should be silently masked.
  RwField::write(static_cast<uint32_t>(0xFFU)); // only 3 bits wide → 0b111
  EXPECT_EQ(field32_storage & 0b0111'0000U, 0b0111'0000U);
  // No bits outside the field should be set.
  EXPECT_EQ(field32_storage & ~0b0111'0000U, 0U);
}

// ---------------------------------------------------------------------------
// ReadOnly field test
// ---------------------------------------------------------------------------

using RoField = ohal::core::BitField<Reg32, 8, 4, ohal::core::Access::ReadOnly>;

TEST_F(BitField32Test, ReadOnlyField_ReadReturnsCorrectBits) {
  field32_storage = 0x0B00U; // bits [11:8] = 0xB
  EXPECT_EQ(static_cast<uint32_t>(RoField::read()), 0xBU);
}

// ---------------------------------------------------------------------------
// WriteOnly field test
// ---------------------------------------------------------------------------

using WoField = ohal::core::BitField<Reg32, 16, 8, ohal::core::Access::WriteOnly>;

TEST_F(BitField32Test, WriteOnlyField_WriteUpdatesCorrectBits) {
  WoField::write(static_cast<uint32_t>(0xABU));
  EXPECT_EQ(field32_storage & 0x00FF'0000U, 0x00AB'0000U);
}

TEST_F(BitField32Test, WriteOnlyField_WriteDoesNotReadRegister) {
  // Pre-load other bits. A WriteOnly write must NOT read-modify-write;
  // only the field bits should appear in the result.
  field32_storage = 0xFFFF'FFFFU;
  WoField::write(static_cast<uint32_t>(0x12U));
  // Only bits [23:16] should reflect the written value; all other bits are
  // cleared because a WriteOnly write skips the read step.
  EXPECT_EQ(field32_storage, 0x0012'0000U);
}

// ---------------------------------------------------------------------------
// 8-bit register field tests
// ---------------------------------------------------------------------------

// A 4-bit ReadWrite field at bits [7:4] of an 8-bit register.
using NibbleHigh = ohal::core::BitField<Reg8, 4, 4, ohal::core::Access::ReadWrite>;

TEST_F(BitField8Test, ReadWriteField8_ReadReturnsCorrectNibble) {
  field8_storage = 0xC0U;
  EXPECT_EQ(static_cast<unsigned>(NibbleHigh::read()), 0xCU);
}

TEST_F(BitField8Test, ReadWriteField8_WriteUpdatesCorrectNibble) {
  field8_storage = 0x0FU;
  NibbleHigh::write(static_cast<uint8_t>(0x7U));
  EXPECT_EQ(field8_storage, 0x7FU);
}

// ---------------------------------------------------------------------------
// Custom enum ValueType test
// ---------------------------------------------------------------------------

enum class Speed : uint32_t { Low = 0, Medium = 1, High = 2, VeryHigh = 3 };

using SpeedField = ohal::core::BitField<Reg32, 2, 2, ohal::core::Access::ReadWrite, Speed>;

TEST_F(BitField32Test, EnumValueType_ReadReturnsEnum) {
  field32_storage = static_cast<uint32_t>(Speed::High) << 2U;
  EXPECT_EQ(SpeedField::read(), Speed::High);
}

TEST_F(BitField32Test, EnumValueType_WriteAcceptsEnum) {
  SpeedField::write(Speed::VeryHigh);
  EXPECT_EQ((field32_storage >> 2U) & 0b11U, static_cast<uint32_t>(Speed::VeryHigh));
}

// ---------------------------------------------------------------------------
// Compile-time mask computation check
// ---------------------------------------------------------------------------

TEST(BitFieldMaskTest, MaskIsCorrect) {
  EXPECT_EQ(RwField::mask, 0b0111'0000U);
  EXPECT_EQ(RoField::mask, 0x0F00U);
  EXPECT_EQ(WoField::mask, 0x00FF'0000U);
  EXPECT_EQ(NibbleHigh::mask, 0xF0U);
}

// ---------------------------------------------------------------------------
// Negative-compile tests (manual verification only)
// ---------------------------------------------------------------------------
//
// The following illegal usages must NOT compile. They can be verified by
// temporarily uncommenting the relevant line and confirming a static_assert
// failure with the expected message:
//
//   RoField::write(0U);
//   // expected: "ohal: cannot write to a read-only field"
//
//   WoField::read();
//   // expected: "ohal: cannot read from a write-only field"
//
// Overflow bounds check (Offset + Width > register width):
//   using Overflow = ohal::core::BitField<Reg32, 30, 4, ohal::core::Access::ReadWrite>;
//   // expected: "ohal: BitField (Offset + Width) exceeds register width"
//
//   using ZeroWidth = ohal::core::BitField<Reg32, 0, 0, ohal::core::Access::ReadWrite>;
//   // expected: "ohal: BitField width must be at least 1"

} // namespace
