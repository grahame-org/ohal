# Step 16 – Additional Peripherals

**Goal:** Extend OHAL beyond GPIO, Timer, and UART by implementing SPI, I2C, ADC, DAC, DMA,
Power/sleep modes, Clocks, and MPU peripherals, following the same pattern established in
Steps 7–9.

**Prerequisites:** [Step 3](step-03-conventional-commits-merge-queue.md) must be in place so
that each new peripheral lands via a conventional commit. [Step 13 (Release
Automation)](step-13-release-automation.md) should be in place so that release-please and
changelog generation cover those commits. [Step 15 (Additional MCU
Families)](step-15-additional-mcu-families.md) should be progressed in parallel: the more MCU
families are supported, the more value each new peripheral provides (since each new peripheral
must be implemented — or explicitly marked unsupported — for every existing family).

**Within this step, implement Clocks first** (section 16.2). Clock enabling is a prerequisite
for all other peripherals to function correctly on real hardware: on STM32 and similar families,
the peripheral bus clock must be enabled before any peripheral register can be accessed. Every
subsequent peripheral implementation in this step references the clock-enable pattern established
in section 16.2.

Each peripheral follows the same three-layer pattern:

1. **Generic interface** (`include/ohal/<peripheral>.hpp`) — enumerations, tag types, primary
   (unimplemented) template, capability trait primary templates.
2. **Platform specialisation** (`platforms/<family>/models/<model>/<peripheral>.hpp`) — partial
   specialisations using `Register<>` and `BitField<>`.
3. **Host unit tests** — mock register addresses, positive and negative-compile tests.

## 16.1 Implementation Order

Implement peripherals in this order to maximise reuse and minimise surprise:

| Order | Peripheral        | Reason                                                                                                                                                                                 |
| ----- | ----------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 1     | **Clocks**        | Prerequisite knowledge: GPIO clocks must be enabled before registers are accessible. Documenting the clock-enable pattern once means all subsequent peripheral steps can reference it. |
| 2     | **SPI**           | Natural extension of the UART pattern already in Step 10; register layout is similar.                                                                                                  |
| 3     | **I2C**           | Independent and widely used; distinct enough register layout to exercise the abstraction further.                                                                                      |
| 4     | **ADC**           | Read-only `DR` register exercises `ReadOnly` `BitField` constraints; introduces conversion trigger concept.                                                                            |
| 5     | **DAC**           | Write-only data registers pair naturally with ADC; exercises `WriteOnly` constraints.                                                                                                  |
| 6     | **DMA**           | Cross-cutting; implement after source peripherals (SPI, UART, ADC, DAC) are in place so DMA request mappings can be described in context.                                              |
| 7     | **Power / Sleep** | Naturally last functional peripheral: putting the MCU to sleep is only safe once all other peripherals are tested.                                                                     |
| 8     | **MPU**           | Security-adjacent; implement after all other peripherals are stable. Architecture-level registers (fixed Cortex-M address) rather than bus-mapped peripheral.                          |

---

## 16.2 Clocks (`ohal::clock`)

### Namespace: `ohal::clock`

Clocks are a prerequisite for most other peripherals. GPIO port clocks must be enabled before
GPIO registers are accessible on STM32 and similar families. The clock abstraction manages this
without runtime overhead.

### Inputs Required (STM32U083 RCC)

- RCC base address: `0x40021000`
- Relevant registers (from RM0503):
  - `CR` `0x00` — clock control register (HSION, HSEON, PLLON, etc.)
  - `CFGR1` `0x1C` — clock configuration register 1 (SW, SWS, HPRE, PPRE)
  - `AHBENR` `0x48` — AHB peripheral clock enable register
  - `APBENR1` `0x4C` — APB1 peripheral clock enable register
  - `APBENR2` `0x50` — APB2 peripheral clock enable register
  - `AHBRSTR` `0x58` — AHB peripheral reset register
  - `APBRSTR1` `0x5C` — APB1 peripheral reset register
  - `APBRSTR2` `0x60` — APB2 peripheral reset register

### Generic Interface Sketch

```cpp
// include/ohal/clock.hpp  (excerpt)
namespace ohal::clock {

// Each platform specialisation provides the correct register address and bit offset
// for enabling the clock of a given peripheral.
template <typename Peripheral>
struct Enable {
    static void enable() {
        static_assert(sizeof(Peripheral) == 0,
            "ohal: clock::Enable is not implemented for the selected peripheral.");
    }
};

} // namespace ohal::clock
```

### Recommended Usage Pattern

```cpp
clock::Enable<PortA>::enable();          // enable GPIOA peripheral clock
Pin<PortA, 5>::set_mode(PinMode::Output);
```

---

## 16.3 SPI (`ohal::spi`)

### Namespace: `ohal::spi`

### Inputs Required (STM32U083 SPI1)

- SPI1 base address: `0x40013000`
- Relevant registers (from RM0503):
  - `CR1` `0x00` — control register 1 (mode, baud rate, CPOL, CPHA)
  - `CR2` `0x04` — control register 2 (data size, frame format, DMA enable)
  - `SR` `0x08` — status register (RO: TXE, RXNE, BSY, OVR)
  - `DR` `0x0C` — data register (RW: write = transmit, read = receive)
  - `CRCPR` `0x10` — CRC polynomial register
  - `RXCRCR` `0x14` — RX CRC register (RO)
  - `TXCRCR` `0x18` — TX CRC register (RO)

### Generic Interface Sketch

```cpp
// include/ohal/spi.hpp  (excerpt)
namespace ohal::spi {

enum class ClockPolarity : uint8_t { IdleLow = 0, IdleHigh = 1 };
enum class ClockPhase    : uint8_t { SampleFirstEdge = 0, SampleSecondEdge = 1 };
enum class DataWidth     : uint8_t { Bits8 = 0, Bits16 = 1 };
enum class BitOrder      : uint8_t { MsbFirst = 0, LsbFirst = 1 };

template <typename Instance>
struct Port {
    static_assert(sizeof(Instance) == 0,
        "ohal: spi::Port is not implemented for the selected MCU.");
};

} // namespace ohal::spi
```

### Capability Traits

- `supports_dma_tx<Instance>`, `supports_dma_rx<Instance>` — false by default
- `supports_crc<Instance>` — false by default

---

## 16.4 I2C (`ohal::i2c`)

### Namespace: `ohal::i2c`

### Inputs Required (STM32U083 I2C1)

- I2C1 base address: `0x40005400`
- Relevant registers (from RM0503):
  - `CR1` `0x00` — control register 1 (PE, NOSTRETCH, ANFOFF, DNF)
  - `CR2` `0x04` — control register 2 (address, direction, byte count, START/STOP)
  - `TIMINGR` `0x10` — timing register
  - `ISR` `0x18` — interrupt and status register (RO)
  - `ICR` `0x1C` — interrupt clear register (WO)
  - `RXDR` `0x24` — receive data register (RO)
  - `TXDR` `0x28` — transmit data register (WO)

### Generic Interface Sketch

```cpp
// include/ohal/i2c.hpp  (excerpt)
namespace ohal::i2c {

enum class AddressingMode : uint8_t { SevenBit = 0, TenBit = 1 };

template <typename Instance>
struct Controller {
    static_assert(sizeof(Instance) == 0,
        "ohal: i2c::Controller is not implemented for the selected MCU.");
};

} // namespace ohal::i2c
```

---

## 16.5 ADC (`ohal::adc`)

### Namespace: `ohal::adc`

### Inputs Required (STM32U083 ADC1)

- ADC1 base address: `0x42028000`
- Relevant registers (from RM0503):
  - `ISR` `0x00` — interrupt and status register
  - `CR` `0x08` — control register (ADSTART, ADSTP, ADCAL, ADEN, ADDIS)
  - `CFGR1` `0x0C` — configuration register 1 (resolution, alignment, scan direction)
  - `SMPR` `0x14` — sampling time register
  - `CHSELR` `0x28` — channel selection register
  - `DR` `0x40` — data register (RO)

### Generic Interface Sketch

```cpp
// include/ohal/adc.hpp  (excerpt)
namespace ohal::adc {

enum class Resolution : uint8_t { Bits12 = 0, Bits10 = 1, Bits8 = 2, Bits6 = 3 };
enum class Alignment  : uint8_t { Right = 0, Left = 1 };

template <typename Instance>
struct Converter {
    static_assert(sizeof(Instance) == 0,
        "ohal: adc::Converter is not implemented for the selected MCU.");
};

} // namespace ohal::adc
```

---

## 16.6 DAC (`ohal::dac`)

### Namespace: `ohal::dac`

### Inputs Required (STM32U083 DAC1)

- DAC1 base address: `0x40007400`
- Relevant registers (from RM0503):
  - `CR` `0x00` — control register (EN1/EN2, TEN, TSEL, WAVE, MAMP, DMAEN)
  - `SWTRIGR` `0x04` — software trigger register (WO)
  - `DHR12R1` `0x08` — channel 1 12-bit right-aligned data register
  - `DHR12L1` `0x0C` — channel 1 12-bit left-aligned data register
  - `DHR8R1` `0x10` — channel 1 8-bit data register
  - `DOR1` `0x2C` — channel 1 output data register (RO)

### Generic Interface Sketch

```cpp
// include/ohal/dac.hpp  (excerpt)
namespace ohal::dac {

enum class Alignment : uint8_t { Right12Bit = 0, Left12Bit = 1, Right8Bit = 2 };

template <typename Instance, uint8_t Channel>
struct Output {
    static_assert(sizeof(Instance) == 0,
        "ohal: dac::Output is not implemented for the selected MCU.");
};

} // namespace ohal::dac
```

---

## 16.7 DMA (`ohal::dma`)

### Namespace: `ohal::dma`

DMA is a cross-cutting concern: DMA channels are shared between peripherals (SPI, I2C, UART,
ADC, DAC). The design must express DMA channel ownership and transfer direction without runtime
allocation.

### Inputs Required (STM32U083 DMA1)

- DMA1 base address: `0x40020000`
- DMAMUX1 base address: `0x40020800`
- DMAMUX role: DMAMUX routes peripheral DMA requests to DMA channels. Before enabling a DMA
  channel, configure the corresponding DMAMUX channel request selection register (for example,
  `DMAMUX_CxCR`) so the intended peripheral request (such as SPI1_TX/SPI1_RX, I2C, ADC, UART)
  is connected to that DMA channel.
- Per-channel registers (channel N at `base + 0x08 + (N-1)*0x14`):
  - `CCR` `0x00` — channel configuration register (EN, DIR, CIRC, MINC, MSIZE, PSIZE, PL)
  - `CNDTR` `0x04` — number of data to transfer register
  - `CPAR` `0x08` — peripheral address register
  - `CMAR` `0x0C` — memory address register
- Status registers at DMA1 base:
  - `ISR` `0x00` — interrupt status register (RO)
  - `IFCR` `0x04` — interrupt flag clear register (WO)

### Generic Interface Sketch

```cpp
// include/ohal/dma.hpp  (excerpt)
namespace ohal::dma {

enum class Direction : uint8_t {
    PeripheralToMemory = 0, MemoryToPeripheral = 1, MemoryToMemory = 2
};
enum class DataWidth  : uint8_t { Byte = 0, HalfWord = 1, Word = 2 };
enum class Priority   : uint8_t { Low = 0, Medium = 1, High = 2, VeryHigh = 3 };

template <typename Controller, uint8_t Channel>
struct Stream {
    static_assert(sizeof(Controller) == 0,
        "ohal: dma::Stream is not implemented for the selected MCU.");
};

} // namespace ohal::dma
```

### Design Note

Peripheral register addresses are still compile-time constants (for example via the selected
controller/channel implementation and `Register<>::address`). Memory addresses are runtime values
(pointer to buffer), so `start()` takes the memory address and count as runtime parameters. This
is the one deliberate exception to the "no runtime data" principle: DMA inherently operates on
runtime memory buffers.

---

## 16.8 Power and Sleep Modes (`ohal::power`)

### Namespace: `ohal::power`

### Inputs Required (STM32U083 PWR)

- PWR base address: `0x40007000`
- Relevant registers (from RM0503):
  - `CR1` `0x00` — power control register 1 (LPMS, VOS)
  - `CR2` `0x04` — power control register 2 (PVDE, PLS)
  - `CR3` `0x08` — power control register 3 (EWUP1..5)
  - `SR1` `0x10` — power status register 1 (WUF, SBF, PVDO)
  - `SCR` `0x18` — status clear register (WO)

### Generic Interface Sketch

```cpp
// include/ohal/power.hpp  (excerpt)
namespace ohal::power {

enum class SleepMode : uint8_t {
    Sleep = 0, Stop0 = 1, Stop1 = 2, Standby = 3, Shutdown = 4
};

template <typename Family>
struct Controller {
    static_assert(sizeof(Family) == 0,
        "ohal: power::Controller is not implemented for the selected MCU.");
};

} // namespace ohal::power
```

---

## 16.9 MPU (`ohal::mpu`)

### Namespace: `ohal::mpu`

The Cortex-M Memory Protection Unit is implementation-dependent: it is common on Cortex-M3,
M4, and M7 devices, and optional on some baseline cores such as Cortex-M0+. Its registers are
at fixed core-level addresses defined by the ARM architecture (`0xE000ED90`), not in the
normal peripheral bus address space. PIC18 and AVR MCUs do not have an MPU; use
`has_mpu<Family>` to model support for each MCU family.

### Register Map (ARM architecture — common to all Cortex-M cores with MPU)

| Register | Address      | Description                                         |
| -------- | ------------ | --------------------------------------------------- |
| `TYPE`   | `0xE000ED90` | MPU Type Register (RO: number of regions)           |
| `CTRL`   | `0xE000ED94` | MPU Control Register (ENABLE, HFNMIENA, PRIVDEFENA) |
| `RNR`    | `0xE000ED98` | MPU Region Number Register                          |
| `RBAR`   | `0xE000ED9C` | MPU Region Base Address Register                    |
| `RASR`   | `0xE000EDA0` | MPU Region Attribute and Size Register              |

### Generic Interface Sketch

```cpp
// include/ohal/mpu.hpp  (excerpt)
namespace ohal::mpu {

// Raw MPU RASR.SIZE field encoding (not literal byte counts):
//   encoded = log2(region_size_bytes) - 1
//   region_size_bytes = 2^(encoded + 1)
enum class RegionSize : uint8_t {
    Bytes32 = 0x04, Bytes64 = 0x05, /* ... encoded values ... */ KB256 = 0x11, MB512 = 0x1C, GB4 = 0x1F
};

enum class AccessPermission : uint8_t {
    NoAccess = 0, PrivOnly = 1, PrivRW_UserRO = 2, FullAccess = 3
};

template <uint8_t RegionNum>
struct Region {
    static_assert(RegionNum < 8u, "ohal: Cortex-M MPU supports at most 8 regions.");
};

// Capability trait: false for MCUs without an MPU (e.g. some Cortex-M0, PIC18, AVR)
template <typename Family>
struct has_mpu : std::false_type {};

} // namespace ohal::mpu
```

### Design Note

Because MPU registers are at fixed core-level addresses, this abstraction does not need
per-family platform specialisations for the register addresses themselves. However,
`has_mpu<Family>` must still be specialised to `true_type` for each family whose MCUs include
an MPU, to prevent compilation errors on MCUs that lack one.
