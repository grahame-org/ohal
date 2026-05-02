# Future Improvements

## Specification Granularity Levels

The longer-term plan is for specifications to cover not just memory maps and peripheral register
layouts but also device-model-specific information such as alternate-function pin mappings. To keep
the format manageable and the data reusable, three granularity levels should be kept separate:

| Level            | What it describes                                                                                          | Expected file                                       |
| ---------------- | ---------------------------------------------------------------------------------------------------------- | --------------------------------------------------- |
| **Architecture** | Word size, endianness, register access type taxonomy, core peripherals (NVIC, SysTick, SCB)                | `docs/specs/common/arch/{arch}.yml`                 |
| **Family**       | Memory map, peripheral register layouts, clock gating structure, sub-family differences                    | `docs/specs/{vendor}/{family}.yml` (current format) |
| **Device model** | Specific part number, package, pin count, alternate-function table, available peripheral instances, errata | `docs/specs/{vendor}/models/{part-number}.yml`      |

### Why three levels?

- **Architecture** data is entirely vendor-independent. The ARM Cortex-M0+ access-type taxonomy
  (`rc_w1`, `w1s`, etc.), NVIC register layout, and word size apply identically to STM32, NXP LPC,
  Nordic nRF, and any other Cortex-M0+ device. Storing it once avoids duplication and drift.

- **Family** data is shared across all models in a family. The STM32U0 GPIO register layout (MODER,
  OTYPER, OSPEEDR, …) is the same for every stm32u031, stm32u073, and stm32u083 part. Clock gating
  register names (e.g. `RCC_IOPENR.GPIOAEN`) are family-level even though each model may expose a
  different subset of GPIO ports. This is what the current spec file captures.

- **Device model** data is specific to a single part number or package variant. The alternate-function
  mapping for PA0 differs between a 32-pin UFQFPN and a 48-pin LQFP package of the same family.
  Errata are tied to silicon revision. The list of available GPIO ports (and hence which peripheral
  instances exist) changes between stm32u031C4 and stm32u031K4. Conflating this with family-level
  data would force the family spec to enumerate every part-specific variant.

### Proposed directory layout

```text
docs/specs/
├── schema.json                     ← common JSON Schema (family-level)
├── schema-model.json               ← JSON Schema for device-model spec files ✅
├── future_improvements.md
├── common/
│   └── arch/
│       └── cortex-m0plus.yml       ← architecture-level definitions ✅
└── stm32/
    ├── stm32u0.yml                 ← family spec (current)
    └── models/
        ├── stm32u031c4.yml         ← model spec: UFQFPN32, 256 KB flash ✅ (template)
        ├── stm32u031k4.yml         ← model spec: 32-pin UFQFPN, 256 KB flash
        └── stm32u073rc.yml         ← model spec: 64-pin, 256 KB flash
```

### Cross-level references

A model spec references its parent family spec via a `family-ref` key. Tooling can then merge the
two levels when generating code or documentation:

```yaml
spec-version: "1.0.0"
vendor: STMicroelectronics
family-ref: stm32u0 # links back to stm32u0.yml
model: stm32u031c4
package: UFQFPN32
flash-kb: 256
sram-kb: 12
pin-count: 32
```

### Mapping the identified coverage gaps to levels

The 11 coverage gaps documented below can now be allocated to the correct level:

| Gap | Title                                         | Level                                                            |
| --- | --------------------------------------------- | ---------------------------------------------------------------- |
| 1   | Peripheral instances and base-address binding | Family ✅                                                        |
| 2   | Register reset values                         | Family                                                           |
| 3   | Extended access-type taxonomy                 | Architecture                                                     |
| 4   | Reserved fields and write-zero constraint     | Architecture                                                     |
| 5   | Sub-family conditional registers/fields       | Family                                                           |
| 6   | Alternate function pin-mapping table          | **Model**                                                        |
| 7   | Clock gating and power-domain metadata        | Family                                                           |
| 8   | Interrupt mapping                             | Family (base IRQ numbers) + **Model** (availability per package) |
| 9   | Errata and silicon-revision annotations       | **Model**                                                        |
| 10  | Register-array and stride notation            | Family                                                           |
| 11  | Peripheral description field                  | Family                                                           |

Gaps marked **Model** should be deferred until `schema-model.json` and the `models/` directory
structure are introduced. The remaining gaps can be addressed incrementally in the existing family
spec format.

---

## Cross-Family and Cross-Vendor Settings Sharing

The current spec format stores all `definitions.settings` blocks inside individual spec files. This
works well for a single family but leads to duplication when the same settings encoding appears
across multiple families or vendors. This document describes the problem and evaluates options for
addressing it.

### Problem

Many peripheral settings encodings are identical across device families and even across vendors,
because they implement the same IP (e.g. ARM Cortex-M GPIO) or follow the same de-facto convention:

| Settings block | Families sharing it                                            |
| -------------- | -------------------------------------------------------------- |
| `gpio-mode`    | All ARM Cortex-M devices with standard GPIO (STM32, NXP, etc.) |
| `output-type`  | STM32Lx, STM32Ux, STM32Hx, and others                          |
| `ospeed`       | STM32Lx, STM32Ux, STM32Hx, and others                          |
| `pupdr`        | Most ARM Cortex-M GPIO implementations                         |
| `bit-value`    | Universal (any single-bit field that is simply set or reset)   |

When the same settings block is copy-pasted into every spec file, a typo or update must be applied
to every copy. A shared definitions mechanism would provide a single source of truth.

### Constraints

The spec format is plain YAML. YAML 1.2 (and YAML 1.1) have no built-in cross-file `include` or
`$ref`. Any cross-file sharing therefore requires either tooling support or a change in the
validation strategy.

### Options

### Option A: Common definitions YAML library (recommended starting point)

Create a `docs/specs/common/` directory with YAML files that define widely-shared settings:

```text
docs/specs/
├── common/
│   ├── gpio.yml       ← GPIO mode, output-type, ospeed, pull-up/down, bit-value
│   └── spi.yml        ← SPI mode, clock polarity, etc.
└── stm32/
    └── stm32u0.yml
```

A pre-validation script (or a custom `check-jsonschema` hook) merges the common definitions into
each spec before schema validation. Individual specs retain their own `definitions.settings` for
family-specific encodings and may override common definitions.

**Pros:**

- Single source of truth for shared settings
- Easy to audit which families use which common definitions
- Common definitions are human-readable YAML

**Cons:**

- Requires a merge/preprocessing step before validation; YAML parsers alone cannot resolve
  cross-file aliases
- Authors must know which definitions are available in `common/`

---

### Option B: Canonical settings names enforced by the schema

Define a registry of well-known settings names (e.g. `cortex-m-gpio-mode`, `bit-value`) and their
expected values in the JSON Schema using `$defs`. Validation tools would check that any settings
block using a canonical name matches the registered definition exactly.

```json
"$defs": {
  "canonical-settings": {
    "cortex-m-gpio-mode": {
      "00": "input mode",
      "01": "general purpose output mode",
      "10": "alternate function mode",
      "11": "analogue mode"
    }
  }
}
```

**Pros:**

- No preprocessing step; validation is schema-driven
- Catches accidental divergence from canonical definitions

**Cons:**

- JSON Schema cannot currently enforce that a YAML alias resolves to a specific named definition;
  this would require custom validator logic
- The schema grows large as the canonical registry grows

---

### Option C: Per-vendor schema extension

Each vendor subdirectory could provide a `schema.json` that `allOf`-references the common schema
and adds vendor-specific constraints (e.g. family name prefix patterns, valid peripheral names).

```json
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "allOf": [{ "$ref": "../schema.json" }],
  "properties": {
    "vendor": { "const": "STMicroelectronics" },
    "family": {
      "properties": {
        "name": { "pattern": "^stm32" }
      }
    }
  }
}
```

The `lint.sh` validation step would look for a vendor-specific `schema.json` first and fall back to
the common `docs/specs/schema.json` when none exists.

**Pros:**

- Enables vendor-specific validation without modifying the common schema
- Schema composition is idiomatic JSON Schema

**Cons:**

- Each vendor must maintain a schema extension
- Does not directly solve cross-file settings sharing

---

### Option D: Spec format versioning ✅ (implemented)

A `spec-version` field has been added to every spec file. The field is required by the schema and
uses [semantic versioning](https://semver.org/) (`MAJOR.MINOR.PATCH`):

```yaml
spec-version: "1.0.0"
vendor: STMicroelectronics
# ...
```

This is orthogonal to the sharing options above and should be adopted regardless of which option is
chosen.

**Pros:**

- Enables non-breaking format evolution
- Tooling can warn about deprecated keys without failing validation

**Cons:**

- Requires all existing spec files to be updated when a version field is added
- Version number management adds overhead

---

### Recommendation

1. **Near-term:** ~~Adopt **Option D** (spec-version field) immediately so that format evolution is
   tracked from the start.~~ ✅ Implemented — `spec-version: "1.0.0"` (semver) is now required by the schema.

2. **Medium-term:** Implement **Option A** (common definitions library) once more than one vendor's
   specs exist and duplication is observed. Keep the merge step simple: a small Python script that
   deep-merges `common/*.yml` definitions into a spec's `definitions.settings` before running
   `check-jsonschema`.

3. **Long-term:** Consider **Option C** (per-vendor schema extensions) once vendor-specific
   constraints (e.g. address range validity, peripheral naming conventions) are worth enforcing.

**Option B** (canonical names in schema) is the most powerful approach for enforcing consistency
but requires custom validator logic that goes beyond standard JSON Schema; defer until there is a
clear need.

---

## Identified Spec Coverage Gaps

The following areas are not yet captured in the current `stm32u0.yml` specification. Peripherals
other than GPIO have been deliberately excluded for now and will be added in future updates. The
items below are therefore _format-level_ gaps — things the spec format itself should eventually
be able to express regardless of which peripheral is being described.

### 1. Peripheral instances and base-address binding ✅ (implemented)

The `peripherals` section defines a peripheral's register layout once (e.g. `gpio`), and the
`instances` list inside each peripheral entry binds that layout to the concrete memory-map
instances (e.g. GPIOA at `0x50000000`, GPIOB at `0x50000400`, …). This removes the need for
consumers to manually correlate the two sections.

### 2. Register reset values

No reset value is recorded for registers. This is essential for:

- Code generation tools that emit `constexpr` register-reset helpers
- Driver correctness checks that verify hardware is in a known state after reset
- Documentation that makes the power-on behaviour explicit

**Proposed addition:** an optional `reset-value` (hex string) field on each register entry.

### 3. Extended access-type taxonomy

The current access types (`rw`, `ro`, `wo`) are insufficient for status registers. Many STM32
and other ARM Cortex-M registers use:

| Access code | Meaning                                     |
| ----------- | ------------------------------------------- |
| `rc_w1`     | Read; clear by writing 1                    |
| `rc_w0`     | Read; clear by writing 0                    |
| `w1s`       | Write 1 to set (read returns current value) |
| `w1c`       | Write 1 to clear                            |
| `rs`        | Read/set (writing has no effect)            |
| `t`         | Toggle (write 1 to toggle current value)    |

Without these access types, driver code generation is incomplete and consumers cannot determine
the correct RMW strategy for each field.

**Proposed addition:** expand the `access` enum in the schema to include the above codes, and
document each in the README.

### 4. Reserved fields and write-zero constraint

Reserved bits within a register are partially expressed (e.g. `RESERVED` fields in LCKR), but
there is no way to specify the required write behaviour (`write-zero`, `write-as-read`,
`write-any`). Incorrect writes to reserved bits can cause undefined hardware behaviour.

**Proposed addition:** an optional `reserved-write` field on `RESERVED`-named register entries:

```yaml
- name: RESERVED
  msb: 31
  lsb: 17
  access: ro
  reserved-write: write-zero
```

### 5. Sub-family conditional registers and fields

The `sub-families` applicability key exists in the memory map but is absent from individual
registers and fields. Some STM32U0 registers or bit fields may be absent or have different reset
values on stm32u031 versus stm32u073/u083.

**Proposed addition:** allow an optional `sub-families` list on register and field entries,
mirroring the memory-map convention.

### 6. Alternate function pin-mapping table

The `afsel` settings block names AF0–AF15 symbolically but does not record the actual function
assigned to each (alternate function, pin) pair (e.g. PA0+AF1 = `TIM2_CH1`). This table is the
primary information a developer needs when selecting GPIO alternate functions.

**Proposed addition:** a separate top-level `pin-functions` section (or a `functions` key inside
each GPIO instance) that maps `{pin, af-number}` → function-name string, cross-referenced to the
reference manual's alternate-function table.

### 7. Clock gating and power-domain metadata

No information is recorded about which RCC enable bit must be set before a peripheral can be
accessed, or which power domain (VDD, VDDIO, independent) the peripheral belongs to. This is
needed for correct peripheral initialisation sequences and low-power mode driver code.

**Proposed addition:** an optional `clock` mapping on each peripheral instance:

```yaml
instances:
  - name: GPIOA
    base: 0x50000000
    clock:
      bus: AHB
      enable-register: RCC_IOPENR
      enable-bit: GPIOAEN
```

### 8. Interrupt mapping

No way to express which interrupt lines a peripheral raises or what conditions trigger them. This
is needed to generate NVIC priority and enable configuration.

**Proposed addition:** an optional `interrupts` list on each peripheral instance:

```yaml
interrupts:
  - name: EXTI0_1
    irq-number: 5
    trigger: rising or falling edge on EXTI lines 0–1
```

### 9. Errata and silicon-revision annotations

Known hardware bugs that affect driver implementation are not captured. Without this information,
spec consumers cannot generate workarounds automatically.

**Proposed addition:** an optional top-level `errata` section listing known issues with their
affected silicon revisions, an informal description, and a workaround reference:

```yaml
errata:
  - id: STM32U0-ERRATA-001
    title: LCKR lock sequence may fail when …
    affected-revisions: [rev-A]
    workaround: Insert a DSB instruction between the second and third LCKR writes.
    reference: STM32U0 Errata sheet ES0561 §2.3.1
```

### 10. Register-array and stride notation

DMA, timers, and other peripherals expose arrays of identical register sets (e.g. DMA channels
0–7 each with CCR/CNDTR/CPAR/CMAR at a fixed stride). The current format has no way to express
this; each channel would have to be listed individually.

**Proposed addition:** an optional `count` and `stride` on a register-group entry, allowing
concise description of repeated register blocks.

### 11. Peripheral description field

The `gpio` peripheral entry has no human-readable `description` field. Adding one (mirroring the
`note` field available on individual registers and fields) would make the spec self-contained as
reference documentation without requiring the reader to consult the reference manual.
