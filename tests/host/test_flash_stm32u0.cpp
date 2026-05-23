// Host-side register-address wiring tests for the STM32U0 FLASH peripheral.
//
// Exercises the FLASH register map defined in
// ohal/platforms/stm32u0/models/stm32u083/flash.hpp.  The FLASH peripheral
// is identical across all STM32U031, STM32U073, and STM32U083 variants
// (single instance at base 0x40022000), so a single test file covers all
// sub-families.
//
// Each test checks that the compile-time ::address constant exposed by every
// ohal::core::Register instantiation resolves to the correct MMIO address
// derived from the base address and the corresponding offset constant.
// Register offsets are verified against Table 19 (RM0503 Rev 4, §3.7).
#include <ohal/platforms/stm32u0/models/stm32u083/flash.hpp>

#include <cstdint>

#include <gtest/gtest.h>

namespace
{

namespace flash = ohal::platforms::stm32u0::stm32u083;

// ---------------------------------------------------------------------------
// Static (compile-time) address assertions for a representative set of
// registers.  These catch any future mis-wiring at build time.
// ---------------------------------------------------------------------------

// Base address: 0x4002'2000 (RM0503 Rev 4, Table 4)
static_assert(flash::Flash::Acr::address == flash::kFlashBase + flash::kFlashAcrOffset,
              "Flash::Acr must map to FLASH_ACR address");
static_assert(flash::Flash::Sr::address == flash::kFlashBase + flash::kFlashSrOffset,
              "Flash::Sr must map to FLASH_SR address");
static_assert(flash::Flash::Cr::address == flash::kFlashBase + flash::kFlashCrOffset,
              "Flash::Cr must map to FLASH_CR address");
static_assert(flash::Flash::Oemkeysr::address == flash::kFlashBase + flash::kFlashOemkeysrOffset,
              "Flash::Oemkeysr must map to OEMKEYSR address (listed as FLASH_KEYSR in Table 19)");
static_assert(flash::Flash::Hdpextr::address == flash::kFlashBase + flash::kFlashHdpextrOffset,
              "Flash::Hdpextr must map to FLASH_HDPEXTR address");

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

class FlashStm32u0WiringTest : public ::testing::TestWithParam<WiringCase>
{
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    RegisterAddresses, FlashStm32u0WiringTest,
    ::testing::Values(
        // Table 19 — FLASH register map (RM0503 Rev 4, §3.7)
        WiringCase{flash::Flash::Acr::address,      flash::kFlashBase + flash::kFlashAcrOffset,      "Flash_Acr"},
        WiringCase{flash::Flash::Keyr::address,     flash::kFlashBase + flash::kFlashKeyrOffset,     "Flash_Keyr"},
        WiringCase{flash::Flash::Optkeyr::address,  flash::kFlashBase + flash::kFlashOptkeyrOffset,  "Flash_Optkeyr"},
        WiringCase{flash::Flash::Sr::address,       flash::kFlashBase + flash::kFlashSrOffset,       "Flash_Sr"},
        WiringCase{flash::Flash::Cr::address,       flash::kFlashBase + flash::kFlashCrOffset,       "Flash_Cr"},
        WiringCase{flash::Flash::Eccr::address,     flash::kFlashBase + flash::kFlashEccrOffset,     "Flash_Eccr"},
        WiringCase{flash::Flash::Optr::address,     flash::kFlashBase + flash::kFlashOptrOffset,     "Flash_Optr"},
        WiringCase{flash::Flash::Wrp1ar::address,   flash::kFlashBase + flash::kFlashWrp1arOffset,   "Flash_Wrp1ar"},
        WiringCase{flash::Flash::Wrp1br::address,   flash::kFlashBase + flash::kFlashWrp1brOffset,   "Flash_Wrp1br"},
        WiringCase{flash::Flash::Secr::address,     flash::kFlashBase + flash::kFlashSecrOffset,     "Flash_Secr"},
        WiringCase{flash::Flash::Oem1keyr1::address, flash::kFlashBase + flash::kFlashOem1keyr1Offset, "Flash_Oem1keyr1"},
        WiringCase{flash::Flash::Oem1keyr2::address, flash::kFlashBase + flash::kFlashOem1keyr2Offset, "Flash_Oem1keyr2"},
        WiringCase{flash::Flash::Oem1keyr3::address, flash::kFlashBase + flash::kFlashOem1keyr3Offset, "Flash_Oem1keyr3"},
        WiringCase{flash::Flash::Oem1keyr4::address, flash::kFlashBase + flash::kFlashOem1keyr4Offset, "Flash_Oem1keyr4"},
        WiringCase{flash::Flash::Oem2keyr1::address, flash::kFlashBase + flash::kFlashOem2keyr1Offset, "Flash_Oem2keyr1"},
        WiringCase{flash::Flash::Oem2keyr2::address, flash::kFlashBase + flash::kFlashOem2keyr2Offset, "Flash_Oem2keyr2"},
        WiringCase{flash::Flash::Oem2keyr3::address, flash::kFlashBase + flash::kFlashOem2keyr3Offset, "Flash_Oem2keyr3"},
        WiringCase{flash::Flash::Oem2keyr4::address, flash::kFlashBase + flash::kFlashOem2keyr4Offset, "Flash_Oem2keyr4"},
        WiringCase{flash::Flash::Oemkeysr::address, flash::kFlashBase + flash::kFlashOemkeysrOffset, "Flash_Oemkeysr"},
        WiringCase{flash::Flash::Hdpcr::address,    flash::kFlashBase + flash::kFlashHdpcrOffset,    "Flash_Hdpcr"},
        WiringCase{flash::Flash::Hdpextr::address,  flash::kFlashBase + flash::kFlashHdpextrOffset,  "Flash_Hdpextr"}),
    [](const ::testing::TestParamInfo<WiringCase>& info) { return info.param.name; });
// clang-format on

TEST_P(FlashStm32u0WiringTest, AddressMatchesHardwareBase)
{
    EXPECT_EQ(GetParam().actual, GetParam().expected);
}

// ---------------------------------------------------------------------------
// Spot-check absolute addresses against Table 19 hard-coded values.
// These guard against accidental base-address or offset corruption.
// ---------------------------------------------------------------------------

TEST(FlashStm32u0AbsoluteAddressTest, BaseAddressIsCorrect)
{
    EXPECT_EQ(flash::kFlashBase, 0x4002'2000U);
}

TEST(FlashStm32u0AbsoluteAddressTest, AcrAddressMatchesTable19)
{
    // Table 19: FLASH_ACR at offset 0x000 → absolute 0x40022000
    EXPECT_EQ(flash::Flash::Acr::address, 0x4002'2000U);
}

TEST(FlashStm32u0AbsoluteAddressTest, CrAddressMatchesTable19)
{
    // Table 19: FLASH_CR at offset 0x014 → absolute 0x40022014
    EXPECT_EQ(flash::Flash::Cr::address, 0x4002'2014U);
}

TEST(FlashStm32u0AbsoluteAddressTest, SecrAddressMatchesTable19)
{
    // Table 19: FLASH_SECR at offset 0x080 → absolute 0x40022080
    EXPECT_EQ(flash::Flash::Secr::address, 0x4002'2080U);
}

TEST(FlashStm32u0AbsoluteAddressTest, HdpextrAddressMatchesTable19)
{
    // Table 19: FLASH_HDPEXTR at offset 0x0B0 → absolute 0x400220B0
    EXPECT_EQ(flash::Flash::Hdpextr::address, 0x4002'20B0U);
}

} // namespace
