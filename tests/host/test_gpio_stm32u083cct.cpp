#include "mock/mock_register.hpp"

#include <ohal/core/capabilities.hpp>
#include <ohal/core/field.hpp>
#include <ohal/gpio.hpp>
#include <ohal/platforms/stm32u0/models/stm32u083cct/capabilities.hpp>
#include <ohal/platforms/stm32u0/models/stm32u083cct/gpio.hpp>

#include <cstdint>

#include <gtest/gtest.h>

// ---------------------------------------------------------------------------
// Host-side tests for the STM32U083CCT (48-pin LQFP) GPIO model.
//
// The STM32U083CCT shares the same GPIO register map and bonded-out port set
// (GPIOA, GPIOB, GPIOC, GPIOF) as the STM32U083KCU.  These tests verify:
//   1. The capability traits report the correct values for the CCT package.
//   2. The Pin<>/Port<> specialisations resolve to the correct hardware
//      base addresses (i.e. the wiring through the thin-wrapper header is
//      correct).
//   3. Basic behavioural correctness via the mock-register infrastructure.
// ---------------------------------------------------------------------------

namespace
{

static uint32_t mock_moder{0U};
static uint32_t mock_otyper{0U};
static uint32_t mock_ospeedr{0U};
static uint32_t mock_pupdr{0U};
static uint32_t mock_idr{0U};
static uint32_t mock_odr{0U};
static uint32_t mock_bsrr{0U};
static uint32_t mock_lckr{0U};
static uint32_t mock_afrl{0U};
static uint32_t mock_afrh{0U};
static uint32_t mock_brr{0U};

/// Mock GPIO port register set with the same nested type aliases as
/// GpioPortRegs<Base>, backed by plain in-memory variables.
struct MockGpioRegs
{
    using Moder = ohal::test::MockRegister<uint32_t, &mock_moder>;
    using Otyper = ohal::test::MockRegister<uint32_t, &mock_otyper>;
    using Ospeedr = ohal::test::MockRegister<uint32_t, &mock_ospeedr>;
    using Pupdr = ohal::test::MockRegister<uint32_t, &mock_pupdr>;
    using Idr = ohal::test::MockRegister<uint32_t, &mock_idr>;
    using Odr = ohal::test::MockRegister<uint32_t, &mock_odr>;
    using Bsrr = ohal::test::MockRegister<uint32_t, &mock_bsrr>;
    using Lckr = ohal::test::MockRegister<uint32_t, &mock_lckr>;
    using Afrl = ohal::test::MockRegister<uint32_t, &mock_afrl>;
    using Afrh = ohal::test::MockRegister<uint32_t, &mock_afrh>;
    using Brr = ohal::test::MockRegister<uint32_t, &mock_brr>;
};

using MockPin5 = ohal::platforms::stm32u0::stm32u083::GpioPortPinImpl<5U, MockGpioRegs>;
using MockPort = ohal::platforms::stm32u0::stm32u083::GpioPortImpl<MockGpioRegs>;

// ---------------------------------------------------------------------------
// Test fixture
// ---------------------------------------------------------------------------

class GpioStm32u083CctTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        mock_moder = 0U;
        mock_otyper = 0U;
        mock_ospeedr = 0U;
        mock_pupdr = 0U;
        mock_idr = 0U;
        mock_odr = 0U;
        mock_bsrr = 0U;
        mock_lckr = 0U;
        mock_afrl = 0U;
        mock_afrh = 0U;
        mock_brr = 0U;
    }
};

// ---------------------------------------------------------------------------
// Basic behavioural tests
// ---------------------------------------------------------------------------

TEST_F(GpioStm32u083CctTest, Set_WritesBitToBsrr)
{
    MockPin5::set();
    EXPECT_EQ(mock_bsrr, 1U << 5U);
}

TEST_F(GpioStm32u083CctTest, Clear_WritesBsrrResetBit)
{
    MockPin5::clear();
    EXPECT_EQ(mock_bsrr, 1U << 21U);
}

TEST_F(GpioStm32u083CctTest, SetMode_Output_WritesModerBits)
{
    MockPin5::set_mode(ohal::gpio::PinMode::Output);
    EXPECT_EQ(mock_moder & (0b11U << 10U), 0b01U << 10U);
}

TEST_F(GpioStm32u083CctTest, SetMode_Input_WritesModerBits)
{
    mock_moder = 0b11U << 10U;
    MockPin5::set_mode(ohal::gpio::PinMode::Input);
    EXPECT_EQ(mock_moder & (0b11U << 10U), 0U);
}

TEST_F(GpioStm32u083CctTest, ReadInput_ReturnsHigh_WhenIdrBitSet)
{
    mock_idr = 1U << 5U;
    EXPECT_EQ(MockPin5::read_input(), ohal::gpio::Level::High);
}

TEST_F(GpioStm32u083CctTest, PortSet_WritesMaskToBsrrLow16Bits)
{
    MockPort::set(0x00FFU);
    EXPECT_EQ(mock_bsrr, 0x00FFU);
}

TEST_F(GpioStm32u083CctTest, PortClear_WritesMaskToBsrrHigh16Bits)
{
    MockPort::clear(0x00FFU);
    EXPECT_EQ(mock_bsrr, 0x00FF0000U);
}

TEST_F(GpioStm32u083CctTest, PortWrite_CombinesSetAndClearMasksInSingleBsrrWrite)
{
    MockPort::write(0x000FU, 0x00F0U);
    EXPECT_EQ(mock_bsrr, 0x00F0000FU);
}

// ---------------------------------------------------------------------------
// Capability trait tests: bonded-out ports (A, B, C, F) must report true.
// ---------------------------------------------------------------------------

static_assert(ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 0>::value,
              "PortA must support output type on CCT");
static_assert(ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortA, 0>::value,
              "PortA must support output speed on CCT");
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 0>::value,
              "PortA must support pull on CCT");
static_assert(ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortA, 0>::value,
              "PortA must support alternate function on CCT");

static_assert(ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortF, 3>::value,
              "PortF must support output type on CCT");
static_assert(ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortF, 3>::value,
              "PortF must support output speed on CCT");
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortF, 3>::value,
              "PortF must support pull on CCT");
static_assert(ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortF, 3>::value,
              "PortF must support alternate function on CCT");

// Out-of-range pin numbers must report false even on bonded-out ports.
static_assert(!ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 16>::value,
              "PortA pin 16 (out of range) must not report supports_output_type on CCT");
static_assert(!ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortA, 16>::value,
              "PortA pin 16 (out of range) must not report supports_output_speed on CCT");
static_assert(!ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 16>::value,
              "PortA pin 16 (out of range) must not report supports_pull on CCT");
static_assert(!ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortA, 16>::value,
              "PortA pin 16 (out of range) must not report supports_alternate_function on CCT");

// PortD and PortE are not bonded out on the 48-pin LQFP package.
static_assert(!ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortD, 0>::value,
              "PortD must not report supports_output_type (not bonded out on CCT)");
static_assert(!ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortD, 0>::value,
              "PortD must not report supports_output_speed (not bonded out on CCT)");
static_assert(!ohal::gpio::capabilities::supports_pull<ohal::gpio::PortD, 0>::value,
              "PortD must not report supports_pull (not bonded out on CCT)");
static_assert(!ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortD, 0>::value,
              "PortD must not report supports_alternate_function (not bonded out on CCT)");

static_assert(!ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortE, 0>::value,
              "PortE must not report supports_output_type (not bonded out on CCT)");
static_assert(!ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortE, 0>::value,
              "PortE must not report supports_output_speed (not bonded out on CCT)");
static_assert(!ohal::gpio::capabilities::supports_pull<ohal::gpio::PortE, 0>::value,
              "PortE must not report supports_pull (not bonded out on CCT)");
static_assert(!ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortE, 0>::value,
              "PortE must not report supports_alternate_function (not bonded out on CCT)");

// PortC bits 0–12 are not bonded out on the 48-pin LQFP package.
static_assert(!ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortC, 0>::value,
              "PortC pin 0 must not report supports_output_type (not bonded out on CCT)");
static_assert(!ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortC, 0>::value,
              "PortC pin 0 must not report supports_output_speed (not bonded out on CCT)");
static_assert(!ohal::gpio::capabilities::supports_pull<ohal::gpio::PortC, 0>::value,
              "PortC pin 0 must not report supports_pull (not bonded out on CCT)");
static_assert(!ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortC, 0>::value,
              "PortC pin 0 must not report supports_alternate_function (not bonded out on CCT)");
static_assert(!ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortC, 12>::value,
              "PortC pin 12 must not report supports_output_type (not bonded out on CCT)");
static_assert(!ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortC, 12>::value,
              "PortC pin 12 must not report supports_output_speed (not bonded out on CCT)");
static_assert(!ohal::gpio::capabilities::supports_pull<ohal::gpio::PortC, 12>::value,
              "PortC pin 12 must not report supports_pull (not bonded out on CCT)");
static_assert(!ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortC, 12>::value,
              "PortC pin 12 must not report supports_alternate_function (not bonded out on CCT)");

// PortF bits 4–15 are not bonded out on the 48-pin LQFP package.
static_assert(!ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortF, 4>::value,
              "PortF pin 4 must not report supports_output_type (not bonded out on CCT)");
static_assert(!ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortF, 4>::value,
              "PortF pin 4 must not report supports_output_speed (not bonded out on CCT)");
static_assert(!ohal::gpio::capabilities::supports_pull<ohal::gpio::PortF, 4>::value,
              "PortF pin 4 must not report supports_pull (not bonded out on CCT)");
static_assert(!ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortF, 4>::value,
              "PortF pin 4 must not report supports_alternate_function (not bonded out on CCT)");

struct CapCase
{
    bool value;
    const char* name;
};

class GpioStm32u083CctCapabilityTest : public ::testing::TestWithParam<CapCase>
{
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    BondedPortCapabilities, GpioStm32u083CctCapabilityTest,
    ::testing::Values(
        CapCase{ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 0>::value,
                "PortA_Pin0_OutputType"},
        CapCase{ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortA, 0>::value,
                "PortA_Pin0_OutputSpeed"},
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 0>::value,
                "PortA_Pin0_Pull"},
        CapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortA, 0>::value,
                "PortA_Pin0_AlternateFunction"},
        CapCase{ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortB, 7>::value,
                "PortB_Pin7_OutputType"},
        CapCase{ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortC, 13>::value,
                "PortC_Pin13_OutputSpeed"},
        CapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortC, 14>::value,
                "PortC_Pin14_Pull"},
        CapCase{ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortF, 3>::value,
                "PortF_Pin3_AlternateFunction"}),
    [](const ::testing::TestParamInfo<CapCase>& info) { return info.param.name; });
// clang-format on

TEST_P(GpioStm32u083CctCapabilityTest, CapabilityIsTrue) { EXPECT_TRUE(GetParam().value); }

struct InvalidCapCase
{
    bool value;
    const char* name;
};

class GpioStm32u083CctInvalidCapTest : public ::testing::TestWithParam<InvalidCapCase>
{
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    UnbondedAndOutOfRangeCapabilities, GpioStm32u083CctInvalidCapTest,
    ::testing::Values(
        // Out-of-range pin on a bonded port
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 16>::value,
            "PortA_Pin16_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortA, 16>::value,
            "PortA_Pin16_OutputSpeed"},
        InvalidCapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortA, 16>::value,
                       "PortA_Pin16_Pull"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortA, 16>::value,
            "PortA_Pin16_AlternateFunction"},
        // PortD — not bonded out on the CCT 48-pin LQFP package
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortD, 0>::value,
            "PortD_Pin0_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortD, 0>::value,
            "PortD_Pin0_OutputSpeed"},
        InvalidCapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortD, 0>::value,
                       "PortD_Pin0_Pull"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortD, 0>::value,
            "PortD_Pin0_AlternateFunction"},
        // PortE — not bonded out on the CCT 48-pin LQFP package
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortE, 0>::value,
            "PortE_Pin0_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortE, 0>::value,
            "PortE_Pin0_OutputSpeed"},
        InvalidCapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortE, 0>::value,
                       "PortE_Pin0_Pull"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortE, 0>::value,
            "PortE_Pin0_AlternateFunction"},
        // PortC bits 0–12 — not bonded out on the CCT 48-pin LQFP package
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortC, 0>::value,
            "PortC_Pin0_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortC, 0>::value,
            "PortC_Pin0_OutputSpeed"},
        InvalidCapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortC, 0>::value,
                       "PortC_Pin0_Pull"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortC, 0>::value,
            "PortC_Pin0_AlternateFunction"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortC, 12>::value,
            "PortC_Pin12_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortC, 12>::value,
            "PortC_Pin12_OutputSpeed"},
        InvalidCapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortC, 12>::value,
                       "PortC_Pin12_Pull"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortC, 12>::value,
            "PortC_Pin12_AlternateFunction"},
        // PortF bits 4–15 — not bonded out on the CCT 48-pin LQFP package
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortF, 4>::value,
            "PortF_Pin4_OutputType"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortF, 4>::value,
            "PortF_Pin4_OutputSpeed"},
        InvalidCapCase{ohal::gpio::capabilities::supports_pull<ohal::gpio::PortF, 4>::value,
                       "PortF_Pin4_Pull"},
        InvalidCapCase{
            ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortF, 4>::value,
            "PortF_Pin4_AlternateFunction"}),
    [](const ::testing::TestParamInfo<InvalidCapCase>& info) { return info.param.name; });
// clang-format on

TEST_P(GpioStm32u083CctInvalidCapTest, CapabilityIsFalse) { EXPECT_FALSE(GetParam().value); }

// ---------------------------------------------------------------------------
// Port-wiring tests: verify that Pin<PortX, N> resolves to the correct MMIO
// base address for the STM32U083CCT package.
// ---------------------------------------------------------------------------

namespace wiring = ohal::platforms::stm32u0::stm32u083;

static_assert(ohal::gpio::Pin<ohal::gpio::PortA, 0>::BsrrSet::reg_type::address ==
                  wiring::kGpioABase + wiring::kBsrrOffset,
              "Pin<PortA,0> must use GPIOA BSRR address on CCT");

static_assert(ohal::gpio::Pin<ohal::gpio::PortB, 0>::BsrrSet::reg_type::address ==
                  wiring::kGpioBBase + wiring::kBsrrOffset,
              "Pin<PortB,0> must use GPIOB BSRR address on CCT");

static_assert(ohal::gpio::Pin<ohal::gpio::PortC, 13>::BsrrSet::reg_type::address ==
                  wiring::kGpioCBase + wiring::kBsrrOffset,
              "Pin<PortC,13> must use GPIOC BSRR address on CCT");

static_assert(ohal::gpio::Pin<ohal::gpio::PortF, 0>::BsrrSet::reg_type::address ==
                  wiring::kGpioFBase + wiring::kBsrrOffset,
              "Pin<PortF,0> must use GPIOF BSRR address on CCT");

static_assert(ohal::gpio::Port<ohal::gpio::PortA>::BsrrReg::address ==
                  wiring::kGpioABase + wiring::kBsrrOffset,
              "Port<PortA> must use GPIOA BSRR address on CCT");

static_assert(ohal::gpio::Port<ohal::gpio::PortB>::BsrrReg::address ==
                  wiring::kGpioBBase + wiring::kBsrrOffset,
              "Port<PortB> must use GPIOB BSRR address on CCT");

static_assert(ohal::gpio::Port<ohal::gpio::PortC>::BsrrReg::address ==
                  wiring::kGpioCBase + wiring::kBsrrOffset,
              "Port<PortC> must use GPIOC BSRR address on CCT");

static_assert(ohal::gpio::Port<ohal::gpio::PortF>::BsrrReg::address ==
                  wiring::kGpioFBase + wiring::kBsrrOffset,
              "Port<PortF> must use GPIOF BSRR address on CCT");

struct WiringCase
{
    uintptr_t actual;
    uintptr_t expected;
    const char* name;
};

class GpioStm32u083CctWiringTest : public ::testing::TestWithParam<WiringCase>
{
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    PortBsrrAddresses, GpioStm32u083CctWiringTest,
    ::testing::Values(
        WiringCase{ohal::gpio::Pin<ohal::gpio::PortA, 0>::BsrrSet::reg_type::address,
                   wiring::kGpioABase + wiring::kBsrrOffset, "PortA"},
        WiringCase{ohal::gpio::Pin<ohal::gpio::PortB, 0>::BsrrSet::reg_type::address,
                   wiring::kGpioBBase + wiring::kBsrrOffset, "PortB"},
        WiringCase{ohal::gpio::Pin<ohal::gpio::PortC, 13>::BsrrSet::reg_type::address,
                   wiring::kGpioCBase + wiring::kBsrrOffset, "PortC"},
        WiringCase{ohal::gpio::Pin<ohal::gpio::PortF, 0>::BsrrSet::reg_type::address,
                   wiring::kGpioFBase + wiring::kBsrrOffset, "PortF"}),
    [](const ::testing::TestParamInfo<WiringCase>& info) { return info.param.name; });
// clang-format on

TEST_P(GpioStm32u083CctWiringTest, BsrrAddressMatchesHardwareBase)
{
    EXPECT_EQ(GetParam().actual, GetParam().expected);
}

} // namespace
