#include "mock/mock_register.hpp"

#include <ohal/core/capabilities.hpp>
#include <ohal/core/field.hpp>
#include <ohal/gpio.hpp>
#include <ohal/platforms/stm32u0/models/stm32u031g6y/capabilities.hpp>
#include <ohal/platforms/stm32u0/models/stm32u031g6y/gpio.hpp>

#include <cstdint>

#include <gtest/gtest.h>

// ---------------------------------------------------------------------------
// Host-side tests for the STM32U031G6Y (WLCSP27) GPIO model.
//
// The STM32U031G6Y shares the same GPIO bonding as the other STM32U031
// G-series packages.  These tests verify that the per-package wrapper
// header correctly pulls in the shared GPIO specialisations and capability
// traits.
// ---------------------------------------------------------------------------

namespace {

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
struct MockGpioRegs {
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

class GpioStm32u031G6yTest : public ::testing::Test {
protected:
  void SetUp() override {
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

TEST_F(GpioStm32u031G6yTest, Set_WritesBitToBsrr) {
  MockPin5::set();
  EXPECT_EQ(mock_bsrr, 1U << 5U);
}

TEST_F(GpioStm32u031G6yTest, Clear_WritesBsrrResetBit) {
  MockPin5::clear();
  EXPECT_EQ(mock_bsrr, 1U << 21U);
}

TEST_F(GpioStm32u031G6yTest, SetMode_Output_WritesModerBits) {
  MockPin5::set_mode(ohal::gpio::PinMode::Output);
  EXPECT_EQ(mock_moder & (0b11U << 10U), 0b01U << 10U);
}

TEST_F(GpioStm32u031G6yTest, SetMode_Input_WritesModerBits) {
  mock_moder = 0b11U << 10U;
  MockPin5::set_mode(ohal::gpio::PinMode::Input);
  EXPECT_EQ(mock_moder & (0b11U << 10U), 0U);
}

TEST_F(GpioStm32u031G6yTest, ReadInput_ReturnsHigh_WhenIdrBitSet) {
  mock_idr = 1U << 5U;
  EXPECT_EQ(MockPin5::read_input(), ohal::gpio::Level::High);
}

TEST_F(GpioStm32u031G6yTest, PortSet_WritesMaskToBsrrLow16Bits) {
  MockPort::set(0x00FFU);
  EXPECT_EQ(mock_bsrr, 0x00FFU);
}

TEST_F(GpioStm32u031G6yTest, PortClear_WritesMaskToBsrrHigh16Bits) {
  MockPort::clear(0x00FFU);
  EXPECT_EQ(mock_bsrr, 0x00FF0000U);
}

TEST_F(GpioStm32u031G6yTest, PortWrite_CombinesSetAndClearMasksInSingleBsrrWrite) {
  MockPort::write(0x000FU, 0x00F0U);
  EXPECT_EQ(mock_bsrr, 0x00F0000FU);
}

// ---------------------------------------------------------------------------
// Capability trait tests
// ---------------------------------------------------------------------------

static_assert(ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 0>::value,
              "PortA pin 0 must support output type on G6Y");
static_assert(ohal::gpio::capabilities::supports_output_speed<ohal::gpio::PortA, 14>::value,
              "PortA pin 14 must support output speed on G6Y");
static_assert(ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortB, 0>::value,
              "PortB pin 0 must support output type on G6Y");
static_assert(ohal::gpio::capabilities::supports_pull<ohal::gpio::PortB, 5>::value,
              "PortB pin 5 must support pull on G6Y");
static_assert(ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortC, 14>::value,
              "PortC pin 14 must support output type on G6Y");
static_assert(ohal::gpio::capabilities::supports_alternate_function<ohal::gpio::PortF, 2>::value,
              "PortF pin 2 must support alternate function on G6Y");

static_assert(!ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 15>::value,
              "PortA pin 15 must not report supports_output_type on G6Y");
static_assert(!ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortA, 16>::value,
              "PortA pin 16 must not report supports_output_type on G6Y");
static_assert(!ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortD, 0>::value,
              "PortD must not report supports_output_type on G6Y");
static_assert(!ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortE, 0>::value,
              "PortE must not report supports_output_type on G6Y");
static_assert(!ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortB, 2>::value,
              "PortB pin 2 must not report supports_output_type on G6Y");
static_assert(!ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortB, 4>::value,
              "PortB pin 4 must not report supports_output_type on G6Y");
static_assert(!ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortC, 0>::value,
              "PortC pin 0 must not report supports_output_type on G6Y");
static_assert(!ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortF, 0>::value,
              "PortF pin 0 must not report supports_output_type on G6Y");
static_assert(!ohal::gpio::capabilities::supports_output_type<ohal::gpio::PortF, 4>::value,
              "PortF pin 4 must not report supports_output_type on G6Y");

// ---------------------------------------------------------------------------
// Port-wiring tests
// ---------------------------------------------------------------------------

namespace wiring = ohal::platforms::stm32u0::stm32u083;

static_assert(ohal::gpio::Pin<ohal::gpio::PortA, 0>::BsrrSet::reg_type::address ==
                  wiring::kGpioABase + wiring::kBsrrOffset,
              "Pin<PortA,0> must use GPIOA BSRR address on G6Y");
static_assert(ohal::gpio::Pin<ohal::gpio::PortB, 0>::BsrrSet::reg_type::address ==
                  wiring::kGpioBBase + wiring::kBsrrOffset,
              "Pin<PortB,0> must use GPIOB BSRR address on G6Y");
static_assert(ohal::gpio::Pin<ohal::gpio::PortC, 14>::BsrrSet::reg_type::address ==
                  wiring::kGpioCBase + wiring::kBsrrOffset,
              "Pin<PortC,14> must use GPIOC BSRR address on G6Y");
static_assert(ohal::gpio::Pin<ohal::gpio::PortF, 2>::BsrrSet::reg_type::address ==
                  wiring::kGpioFBase + wiring::kBsrrOffset,
              "Pin<PortF,2> must use GPIOF BSRR address on G6Y");

static_assert(ohal::gpio::Port<ohal::gpio::PortA>::BsrrReg::address ==
                  wiring::kGpioABase + wiring::kBsrrOffset,
              "Port<PortA> must use GPIOA BSRR address on G6Y");

} // namespace
