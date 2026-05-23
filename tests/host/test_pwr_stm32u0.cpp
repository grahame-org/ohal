// Host-side register-address wiring tests for the STM32U0 PWR peripheral.
//
// Exercises the PWR register map defined in
// ohal/platforms/stm32u0/models/stm32u083/pwr.hpp.  The PWR peripheral
// is identical across all STM32U031, STM32U073, and STM32U083 variants
// (single instance at base 0x40007000), so a single test file covers all
// sub-families.
//
// Each test checks that the compile-time ::address constant exposed by every
// ohal::core::Register instantiation resolves to the correct MMIO address
// derived from the base address and the corresponding offset constant.
// Register offsets are verified against Table 31 (RM0503 Rev 4, §4.4).
#include <ohal/platforms/stm32u0/models/stm32u083/pwr.hpp>

#include <cstdint>

#include <gtest/gtest.h>

namespace
{

namespace pwr = ohal::platforms::stm32u0::stm32u083;

// ---------------------------------------------------------------------------
// Static (compile-time) address assertions for a representative set of
// registers.  These catch any future mis-wiring at build time.
// ---------------------------------------------------------------------------

// Base address: 0x4000'7000 (RM0503 Rev 4, Table 4)
static_assert(pwr::Pwr::Cr1::address == pwr::kPwrBase + pwr::kPwrCr1Offset,
              "Pwr::Cr1 must map to PWR_CR1 address");
static_assert(pwr::Pwr::Scr::address == pwr::kPwrBase + pwr::kPwrScrOffset,
              "Pwr::Scr must map to PWR_SCR address");
static_assert(pwr::Pwr::Pucra::address == pwr::kPwrBase + pwr::kPwrPucraOffset,
              "Pwr::Pucra must map to PWR_PUCRA address");
static_assert(pwr::Pwr::Pdcrf::address == pwr::kPwrBase + pwr::kPwrPdcrfOffset,
              "Pwr::Pdcrf must map to PWR_PDCRF address");

// ---------------------------------------------------------------------------
// Runtime parameterised wiring tests: each case checks that one register's
// compile-time address constant matches the expected base + offset formula.
// ---------------------------------------------------------------------------

struct WiringCase
{
    uintptr_t actual;
    uintptr_t expected;
    const char* name;
};

class PwrStm32u0WiringTest : public ::testing::TestWithParam<WiringCase>
{
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    RegisterAddresses, PwrStm32u0WiringTest,
    ::testing::Values(
        // Table 31 — PWR register map (RM0503 Rev 4, §4.4)
        WiringCase{pwr::Pwr::Cr1::address,   pwr::kPwrBase + pwr::kPwrCr1Offset,   "Pwr_Cr1"},
        WiringCase{pwr::Pwr::Cr2::address,   pwr::kPwrBase + pwr::kPwrCr2Offset,   "Pwr_Cr2"},
        WiringCase{pwr::Pwr::Cr3::address,   pwr::kPwrBase + pwr::kPwrCr3Offset,   "Pwr_Cr3"},
        WiringCase{pwr::Pwr::Cr4::address,   pwr::kPwrBase + pwr::kPwrCr4Offset,   "Pwr_Cr4"},
        WiringCase{pwr::Pwr::Sr1::address,   pwr::kPwrBase + pwr::kPwrSr1Offset,   "Pwr_Sr1"},
        WiringCase{pwr::Pwr::Sr2::address,   pwr::kPwrBase + pwr::kPwrSr2Offset,   "Pwr_Sr2"},
        WiringCase{pwr::Pwr::Scr::address,   pwr::kPwrBase + pwr::kPwrScrOffset,   "Pwr_Scr"},
        WiringCase{pwr::Pwr::Pucra::address, pwr::kPwrBase + pwr::kPwrPucraOffset, "Pwr_Pucra"},
        WiringCase{pwr::Pwr::Pdcra::address, pwr::kPwrBase + pwr::kPwrPdcraOffset, "Pwr_Pdcra"},
        WiringCase{pwr::Pwr::Pucrb::address, pwr::kPwrBase + pwr::kPwrPucrbOffset, "Pwr_Pucrb"},
        WiringCase{pwr::Pwr::Pdcrb::address, pwr::kPwrBase + pwr::kPwrPdcrbOffset, "Pwr_Pdcrb"},
        WiringCase{pwr::Pwr::Pucrc::address, pwr::kPwrBase + pwr::kPwrPucrcOffset, "Pwr_Pucrc"},
        WiringCase{pwr::Pwr::Pdcrc::address, pwr::kPwrBase + pwr::kPwrPdcrcOffset, "Pwr_Pdcrc"},
        WiringCase{pwr::Pwr::Pucrd::address, pwr::kPwrBase + pwr::kPwrPucrdOffset, "Pwr_Pucrd"},
        WiringCase{pwr::Pwr::Pdcrd::address, pwr::kPwrBase + pwr::kPwrPdcrdOffset, "Pwr_Pdcrd"},
        WiringCase{pwr::Pwr::Pucre::address, pwr::kPwrBase + pwr::kPwrPucreOffset, "Pwr_Pucre"},
        WiringCase{pwr::Pwr::Pdcre::address, pwr::kPwrBase + pwr::kPwrPdcreOffset, "Pwr_Pdcre"},
        WiringCase{pwr::Pwr::Pucrf::address, pwr::kPwrBase + pwr::kPwrPucrfOffset, "Pwr_Pucrf"},
        WiringCase{pwr::Pwr::Pdcrf::address, pwr::kPwrBase + pwr::kPwrPdcrfOffset, "Pwr_Pdcrf"}),
    [](const ::testing::TestParamInfo<WiringCase>& info) { return info.param.name; });
// clang-format on

TEST_P(PwrStm32u0WiringTest, AddressMatchesHardwareBase)
{
    EXPECT_EQ(GetParam().actual, GetParam().expected);
}

// ---------------------------------------------------------------------------
// Spot-check absolute addresses against Table 31 hard-coded values.
// These guard against accidental base-address or offset corruption.
// ---------------------------------------------------------------------------

TEST(PwrStm32u0AbsoluteAddressTest, BaseAddressIsCorrect)
{
    EXPECT_EQ(pwr::kPwrBase, 0x4000'7000U);
}

TEST(PwrStm32u0AbsoluteAddressTest, Cr1AddressMatchesTable31)
{
    // Table 31: PWR_CR1 at offset 0x000 → absolute 0x40007000
    EXPECT_EQ(pwr::Pwr::Cr1::address, 0x4000'7000U);
}

TEST(PwrStm32u0AbsoluteAddressTest, ScrAddressMatchesTable31)
{
    // Table 31: PWR_SCR at offset 0x018 → absolute 0x40007018
    EXPECT_EQ(pwr::Pwr::Scr::address, 0x4000'7018U);
}

TEST(PwrStm32u0AbsoluteAddressTest, PucraAddressMatchesTable31)
{
    // Table 31: PWR_PUCRA at offset 0x020 → absolute 0x40007020
    EXPECT_EQ(pwr::Pwr::Pucra::address, 0x4000'7020U);
}

TEST(PwrStm32u0AbsoluteAddressTest, PdcrfAddressMatchesTable31)
{
    // Table 31: PWR_PDCRF at offset 0x04C → absolute 0x4000704C
    EXPECT_EQ(pwr::Pwr::Pdcrf::address, 0x4000'704CU);
}

} // namespace
