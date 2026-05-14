# Step 22 – RCC Clock Tree and SysTick (`ohal::clock` extensions)

**Goal:** Replace `SystemClock_Config()`, `HAL_Init()`, and `HAL_IncTick()` in `test_project`
with ohal calls, eliminating the last remaining ST HAL dependencies.

This is the most RM0503-intensive step and should not be started until all required
register details have been gathered. The step is split into four sequential sub-steps to
keep each PR small:

| Sub-step | Covers                               | Converts in test_project                                         |
| -------- | ------------------------------------ | ---------------------------------------------------------------- |
| 22a      | PWR voltage scaling                  | `HAL_PWREx_ControlVoltageScaling`                                |
| 22b      | Flash latency                        | `FLASH_LATENCY_1` in `HAL_RCC_ClockConfig`                       |
| 22c      | RCC oscillator + clock source select | `HAL_RCC_OscConfig`, `HAL_RCC_ClockConfig`, `SystemClock_Config` |
| 22d      | SysTick abstraction                  | `HAL_Init`, `HAL_IncTick`                                        |

**Prerequisites:**

- [Step 19 (Clock Enable)](step-19-clock-enable.md): `ohal::clock` namespace and
  `ClockEnableImpl` helper exist.
- [Step 21 (EXTI/NVIC)](step-21-exti-nvic.md): `ohal::irq` and `ohal::nvic` are
  implemented, since SysTick on Cortex-M involves the SysTick exception which is related to
  interrupt infrastructure.
- All RM0503 sections listed below must be read and all required values recorded before any
  sub-step is implemented.

---

## RM0503 Sections Required Before Any Implementation

> **All values in this step must be sourced from RM0503 before writing code.**
> Do not implement any sub-step until the relevant RM0503 section has been read and the
> required field values recorded here.

| Sub-step | RM0503 section(s) to read                                                     | Values needed                                                                                                                                                                                                                   |
| -------- | ----------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 22a      | PWR chapter — "Voltage regulator" / "PWR_CR1" register description            | VOS bit position and field width in `PWR_CR1`; encoding for Range 1 / Range 2; any sequence requirement (e.g. must set VOS before/after enabling oscillator)                                                                    |
| 22b      | FLASH chapter — "FLASH_ACR" register description                              | LATENCY bit positions; the number of wait states required at 16 MHz on Range 2 voltage (confirm it is 1 WS or 0 WS)                                                                                                             |
| 22c      | RCC chapter — "RCC_CR" register description; "RCC_CFGR1" register description | HSION bit, HSIRDY bit positions in `RCC_CR`; SW (clock source select) and SWS (clock source status) bit positions in `RCC_CFGR1`; HPRE (AHB prescaler) and PPRE (APB prescaler) bit positions; any mandatory wait loop sequence |
| 22d      | RCC chapter — SysTick section or Cortex-M0+ integration manual                | SysTick base address (`0xE000E010`); CTRL, LOAD, VAL register offsets; CLKSOURCE bit (internal clock vs external); TICKINT bit; ENABLE bit; COUNTFLAG bit                                                                       |

---

## Sub-step 22a — PWR Voltage Scaling (`ohal::power`)

### Inputs Required

**PWR base address:** `0x40007000` (from `stm32u083xx.h` memory map).

| Register | Offset | Access | Description                                                |
| -------- | ------ | ------ | ---------------------------------------------------------- |
| `CR1`    | `0x00` | RW     | Power control register 1 — voltage scaling, low-power mode |

> **Source from RM0503 before implementing:**
>
> - `VOS` bit position and width in `PWR_CR1`.
> - Encoding: which value selects Range 1 (higher performance, higher power) and which
>   selects Range 2 (lower power, lower max frequency)?
> - Is there a VOSF ready flag that must be polled after changing VOS? If so, its register
>   and bit position.
> - Any sequencing requirement with respect to flash latency or oscillator changes.

### Generic interface sketch

```cpp
// include/ohal/power.hpp  (addition for this sub-step)
namespace ohal::power {

enum class VoltageRange : uint8_t {
    Range1 = /* encoding from RM0503 */,
    Range2 = /* encoding from RM0503 */,
};

template <typename Family>
struct VoltageScaling {
    static_assert(sizeof(Family) == 0,
        "ohal: power::VoltageScaling is not implemented for this MCU family.");
};

} // namespace ohal::power
```

### Acceptance criteria (22a)

1. `VoltageScaling<STM32U0Family>::set(VoltageRange::Range2)` writes the RM0503-confirmed
   encoding for Range 2 into the VOS field of `PWR_CR1` at `0x40007000 + 0x00`.
2. All other bits of `PWR_CR1` are preserved (read-modify-write).
3. If a VOSF ready flag exists, `set()` polls it until cleared before returning.
4. `bash lint.sh` reports no errors.

---

## Sub-step 22b — Flash Latency (`ohal::flash`)

### Inputs Required

**FLASH base address:** `0x40022000` (from `stm32u083xx.h` memory map).

| Register | Offset | Access | Description                                                          |
| -------- | ------ | ------ | -------------------------------------------------------------------- |
| `ACR`    | `0x00` | RW     | Flash access control register — latency, prefetch, instruction cache |

`LATENCY` field occupies bits `[2:0]` of `FLASH_ACR` (confirmed from `stm32u083xx.h` bit
definitions `FLASH_ACR_LATENCY_Pos = 0`, `FLASH_ACR_LATENCY_Msk = 0x7`).

> **Source from RM0503 before implementing:**
>
> - The required LATENCY value at 16 MHz system clock on Range 2 voltage. At Range 2 on
>   STM32U0, the expected answer is 1 wait state, but this must be confirmed from the
>   RM0503 table that relates frequency/voltage to required flash latency.
> - Whether `PRFTEN` (prefetch enable) or `ICEN` (instruction cache enable) should be set
>   alongside latency, and whether there is any ordering requirement (set latency before or
>   after switching clock source).

### Generic interface sketch

```cpp
// include/ohal/flash.hpp  (new header for this sub-step)
namespace ohal::flash {

/// Number of flash wait states (0 = zero wait states; 1 = one wait state; etc.)
enum class Latency : uint8_t { WS0 = 0, WS1 = 1, WS2 = 2, WS3 = 3 };

template <typename Family>
struct AccessControl {
    static_assert(sizeof(Family) == 0,
        "ohal: flash::AccessControl is not implemented for this MCU family.");
};

} // namespace ohal::flash
```

### Acceptance criteria (22b)

1. `flash::AccessControl<STM32U0Family>::set_latency(Latency::WS1)` writes `1` to bits
   `[2:0]` of `FLASH_ACR` at `0x40022000 + 0x00`.
2. All other bits of `FLASH_ACR` are preserved.
3. `bash lint.sh` reports no errors.

---

## Sub-step 22c — RCC Oscillator and Clock Source (`ohal::clock` extension)

### Inputs Required

**RCC base address:** `0x40021000`.

| Register | Offset        | Access | Description                                                   |
| -------- | ------------- | ------ | ------------------------------------------------------------- |
| `CR`     | (from RM0503) | RW     | Clock control register — oscillator enable/ready flags        |
| `CFGR1`  | (from RM0503) | RW     | Clock configuration — system clock source, AHB/APB prescalers |

> **Source from RM0503 before implementing (all values below are examples only):**
>
> - `RCC_CR`: bit position of `HSION` (HSI oscillator enable); bit position of `HSIRDY`
>   (HSI ready flag, RO).
> - `RCC_CFGR1`: bit position and width of `SW` (system clock switch — selects HSI, HSE,
>   PLL, LSI, LSE); bit position and width of `SWS` (system clock switch status, RO);
>   bit position and width of `HPRE` (AHB prescaler); bit position and width of `PPRE`
>   (APB prescaler).
> - Polling requirement: after setting `HSION`, must software poll `HSIRDY` before switching
>   `SW` to HSI? If so, what is the recommended approach or timeout?
> - After changing `SW`, must software poll `SWS` to confirm the switch has completed?

### Scope for test_project

`test_project`'s `SystemClock_Config` performs this sequence:

1. Set `PWR_CR1` VOS = Range 2 → covered by sub-step 22a.
2. Set `FLASH_ACR` LATENCY = 1 → covered by sub-step 22b.
3. Enable HSI, poll HSIRDY.
4. Set SYSCLK source = HSI (HPRE = /1, PPRE = /1).

There is no PLL in this configuration. The full PLL configuration path is out of scope for
`test_project` and may be addressed in a later step.

### Generic interface sketch

```cpp
// Addition to include/ohal/clock.hpp
namespace ohal::clock {

enum class OscillatorSource : uint8_t { Hsi = 0, Hse = 1, Pll = 2 };
enum class AhbPrescaler    : uint8_t { Div1 = 0, Div2 = 1, /* … */ };
enum class ApbPrescaler    : uint8_t { Div1 = 0, Div2 = 4, /* … */ };

struct ClockConfig {
    OscillatorSource source  = OscillatorSource::Hsi;
    AhbPrescaler     ahb_pre = AhbPrescaler::Div1;
    ApbPrescaler     apb_pre = ApbPrescaler::Div1;
};

template <typename Family>
struct OscillatorController {
    static_assert(sizeof(Family) == 0,
        "ohal: clock::OscillatorController not implemented for this MCU.");
};

} // namespace ohal::clock
```

> **Enum encoding:** the numeric values in `AhbPrescaler` and `ApbPrescaler` must match the
> hardware register encoding from RM0503, not the divisor value.

### Acceptance criteria (22c)

1. `OscillatorController<STM32U0Family>::enable_hsi()` sets the HSION bit in `RCC_CR` and
   polls HSIRDY until it reads 1.
2. `OscillatorController<STM32U0Family>::set_sysclk(ClockConfig{})` writes the SW field in
   `RCC_CFGR1` to the HSI source value and polls SWS until the switch is confirmed.
3. Both operations compile to correct register-access sequences with no heap allocation.
4. `bash lint.sh` reports no errors.

---

## Sub-step 22d — SysTick Abstraction

### Inputs Required

SysTick registers are at fixed ARM architecture addresses (not STM32-specific; defined in the
Cortex-M0+ Technical Reference Manual and the ARM Architecture Reference Manual):

| Register | Address      | Access | Description                                                |
| -------- | ------------ | ------ | ---------------------------------------------------------- |
| `CTRL`   | `0xE000E010` | RW     | Control and status (ENABLE, TICKINT, CLKSOURCE, COUNTFLAG) |
| `LOAD`   | `0xE000E014` | RW     | Reload value (24-bit)                                      |
| `VAL`    | `0xE000E018` | RW     | Current counter value (write any value to clear)           |
| `CALIB`  | `0xE000E01C` | RO     | Calibration value (optional; not required for basic use)   |

> **Source from RM0503 and Cortex-M0+ TRM before implementing:**
>
> - `CTRL` bit positions: COUNTFLAG (read-only, clears on read), CLKSOURCE (1 = processor
>   clock, 0 = external reference), TICKINT (1 = generate SysTick exception), ENABLE.
> - The `LOAD` register is 24 bits wide: confirm that the upper 8 bits are reserved/ignored
>   and that writing a value larger than 0x00FFFFFF has defined behaviour.
> - Confirm that the SysTick interrupt fires the `SysTick_Handler` (vector number 15 in the
>   Cortex-M exception table, not an NVIC-managed IRQ).

### Scope

`test_project` uses SysTick only as a 1 ms tick source for `HAL_Delay`. The ohal SysTick
abstraction needs:

1. `SysTick::configure(uint32_t reload_value)` — sets `LOAD`, clears `VAL`, enables with
   TICKINT=1 and CLKSOURCE=1 (processor clock).
2. `SysTick::tick()` — called from `SysTick_Handler` to increment a `uint32_t` tick counter.
3. `SysTick::now()` — returns the current tick count (used to replace `HAL_GetTick()`).
4. `SysTick::delay_ms(uint32_t ms)` — blocking delay using the tick counter.

### Acceptance criteria (22d)

1. `SysTick::configure(16'000'000U / 1'000U - 1U)` (= 15999, for 1 ms ticks at 16 MHz
   processor clock) writes `15999` to `LOAD` at `0xE000E014`, clears `VAL` at
   `0xE000E018`, and sets `CTRL` = `0x7` (ENABLE=1, TICKINT=1, CLKSOURCE=1) at
   `0xE000E010`.
2. `SysTick::tick()` increments an internal counter; `SysTick::now()` returns its value.
3. `SysTick::delay_ms(10)` returns after `SysTick::tick()` has been called 10 times in the
   mock (test with mock tick injection).
4. `bash lint.sh` reports no errors.

---

## `test_project` Conversion

After all four sub-steps are complete:

```cpp
// Before — ST HAL (SystemClock_Config in main.cpp, HAL_MspInit in stm32u0xx_hal_msp.cpp,
//           SysTick_Handler and HAL_Init throughout)
HAL_Init();                           // removed
SystemClock_Config();                 // replaced

// Before SystemClock_Config body:
HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE2);
HAL_RCC_OscConfig(&RCC_OscInitStruct);   // HSI on, no PLL
HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);

// After — ohal
ohal::power::VoltageScaling<STM32U0Family>::set(ohal::power::VoltageRange::Range2);
ohal::flash::AccessControl<STM32U0Family>::set_latency(ohal::flash::Latency::WS1);
ohal::clock::OscillatorController<STM32U0Family>::enable_hsi();
ohal::clock::OscillatorController<STM32U0Family>::set_sysclk(
    {.source  = ohal::clock::OscillatorSource::Hsi,
     .ahb_pre = ohal::clock::AhbPrescaler::Div1,
     .apb_pre = ohal::clock::ApbPrescaler::Div1});
ohal::SysTick::configure(16'000'000U / 1'000U - 1U); // 1 ms tick at 16 MHz (reload = 15999)
```

In `stm32u0xx_it.cpp`:

```cpp
// Before
void SysTick_Handler(void) { HAL_IncTick(); }

// After
void SysTick_Handler(void) { ohal::SysTick::tick(); }
```

Once all HAL calls are replaced: remove the `stm32u0xx_hal.h`, `stm32u0xx_hal_pwr_ex.h`,
`stm32u0xx_hal_rcc.h`, and `stm32u0xx_hal_cortex.h` includes from `main.h`, and remove the
`HAL_MspInit` function body (or its entire file if nothing else remains in it).
