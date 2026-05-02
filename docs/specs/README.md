# Device-Family Register Specifications

This directory contains structured YAML specifications for microcontroller device families from any
vendor. Each spec captures memory-map and peripheral register/field metadata directly from the
vendor reference manual.

## Directory Layout

```text
docs/specs/
├── README.md                  ← this file
├── schema.json                ← common JSON Schema for family spec files
├── future_improvements.md     ← options for cross-family/cross-vendor settings sharing
├── common/
│   └── arch/
│       └── {arch}.yml         ← one architecture-level spec per processor architecture
└── {vendor}/                  ← one subdirectory per vendor (e.g. stm32, nxp, nordic)
    └── {family}.yml           ← one spec file per device family
```

**Vendor directory naming:** use the vendor's product-line prefix in lowercase (e.g. `stm32` for
STMicroelectronics STM32, `nrf` for Nordic nRF, `lpc` for NXP LPC).

## Format Overview

Every spec file is a YAML document validated against `docs/specs/schema.json`. The top-level keys
are:

| Key            | Required | Description                                                       |
| -------------- | -------- | ----------------------------------------------------------------- |
| `spec-version` | ✓        | Spec format version using semver (e.g. `"1.0.0"`)                 |
| `vendor`       | ✓        | Chip vendor name (e.g. 'STMicroelectronics')                      |
| `family`       | ✓        | Family name and sub-family list                                   |
| `architecture` | ✓        | Processor architecture and word size (inline summary)             |
| `arch-ref`     |          | Reference to an architecture-level spec (see below)               |
| `reference`    | ✓        | Reference manual document identifier and revision                 |
| `memory`       |          | Memory map with address ranges and sub-family applicability       |
| `definitions`  |          | Reusable settings blocks (referenced by YAML anchors in the spec) |
| `peripherals`  |          | Peripheral blocks with register and bit-field descriptions        |

## Key Concepts

### Spec version

Every spec must declare the format version as the first key:

```yaml
spec-version: "1.0.0"
```

The version uses [semantic versioning](https://semver.org/) (`MAJOR.MINOR.PATCH`): increment the
patch version for corrections, the minor version for backward-compatible additions, and the major
version for breaking changes. Pre-release and build-metadata suffixes (e.g. `1.0.0-alpha.1`) are
also permitted. Tooling can use this field to warn about or reject stale spec files.

### Vendor

Every spec must declare the chip vendor so that the spec is self-describing independently of its
directory location:

```yaml
vendor: STMicroelectronics
```

### Architecture reference

Family specs that belong to a well-known processor architecture can declare an `arch-ref` key
pointing to an architecture-level spec file in `docs/specs/common/arch/`:

```yaml
arch-ref: cortex-m0plus
```

The corresponding file (`docs/specs/common/arch/cortex-m0plus.yml`) is the canonical source of
truth for:

- Architecture metadata: word size, endianness, ISA name
- Private Peripheral Bus (PPB) memory regions (e.g. NVIC, SysTick, SCB address ranges)
- Settings encodings that are identical on every device implementing this architecture (e.g.
  the standard ARM Cortex-M GPIO mode encoding `gpio-mode`)

Because YAML anchors cannot span files, architecture-generic settings that are used as YAML
aliases within a family spec (e.g. `*gpio-mode`) must also be defined in `definitions.settings` of
that family spec. Those local copies are annotated with a comment noting that the canonical
definition lives in the architecture spec.

### Sub-family applicability

Many registers and memory regions differ between sub-families (e.g. flash size). The `sub-families`
key on a memory region or register lists which sub-families the entry applies to.

### References

`reference` blocks provide back-links to the vendor reference manual:

```yaml
reference:
  sections: [7.4, 7.4.1] # for peripherals (list of sections)
  section: 7.4.1 # for individual registers (single section)
  figures: [2]
  tables: [41]
```

### Memory map

Each entry in `memory.map` is a named address range:

```yaml
memory:
  map:
    - name: Main flash memory
      sub-families: [stm32u031]
      start: 0x08000000
      end: 0x0801FFFF
    - name: reserved
      sub-families: [stm32u031]
      start: 0x08020000
      end: 0x1FFFD7FF
```

### Peripherals

Peripherals are listed under `peripherals`. Each item is a single-key mapping whose key is the
peripheral name and whose value contains `registers`:

```yaml
peripherals:
  - gpio:
      reference:
        sections: [7.4]
      registers:
        - MODER:
            reference:
              section: 7.4.1
            offset: 0x00
            fields:
              - name: MODE15
                msb: 31
                lsb: 30
                width: 2
                access: rw
                settings: *gpio-mode
```

### Register fields

Each field has:

| Key        | Type                 | Description                                         |
| ---------- | -------------------- | --------------------------------------------------- |
| `name`     | string               | Field name from the reference manual                |
| `msb`      | integer              | Most-significant bit position (inclusive, 0-based)  |
| `lsb`      | integer              | Least-significant bit position (inclusive, 0-based) |
| `width`    | integer              | Field width in bits (must equal `msb - lsb + 1`)    |
| `access`   | `rw` \| `ro` \| `wo` | Read/write, read-only, or write-only                |
| `note`     | string (optional)    | Free-text annotation                                |
| `settings` | mapping or `~`       | Enumerated bit-pattern values (see below)           |

The `access` values are:

- `rw` — read/write
- `ro` — read-only
- `wo` — write-only (hardware ignores the read value)

### Settings

`settings` is a mapping from binary bit-pattern strings to human-readable descriptions:

```yaml
settings:
  "00": input mode
  "01": general purpose output mode
  "10": alternate function mode
  "11": analogue mode
```

Use `settings: ~` (null) for reserved fields that have no documented settings.

Keys **must** be quoted strings (e.g. `"00"`, `"10"`) even though they look numeric, to prevent YAML
1.1 parsers from converting them to integers.

### Reducing duplication with YAML anchors

When the same settings block repeats across many fields (e.g. all 16 bits in a GPIO mode register
share identical encoding), define the block once under `definitions` using a YAML anchor and
reference it elsewhere with a YAML alias:

```yaml
definitions:
  settings:
    gpio-mode: &gpio-mode # anchor definition
      "00": input mode
      "01": general purpose output mode
      "10": alternate function mode
      "11": analogue mode

peripherals:
  - gpio:
      registers:
        - MODER:
            fields:
              - name: MODE15
                msb: 31
                lsb: 30
                width: 2
                access: rw
                settings: *gpio-mode # alias reference
              - name: MODE14
                # ...
                settings: *gpio-mode
```

### Register access sequences

Some registers require a specific access sequence (e.g. the GPIO lock register). Document these
under `sequence`, which is an object with a required `description` and an ordered `steps` list:

```yaml
- LCKR:
    sequence:
      description: >-
        Lock sequence that must be followed exactly to freeze the port configuration.
        Accepts one caller argument: lock-bits, a 16-bit mask (LCK0–LCK15) indicating
        which port pins to lock.
      steps:
        - op: write
          note: Write LCKK=1 with the desired lock-bits
          fields:
            LCKK: 1
            LCK: write-arg
        - op: write
          note: Write LCKK=0 with the same lock-bits
          fields:
            LCKK: 0
            LCK: write-arg
        - op: write
          note: Write LCKK=1 with the same lock-bits again
          fields:
            LCKK: 1
            LCK: write-arg
        - op: read
          note: Read the register to complete the lock sequence (result discarded)
        - op: read
          optional: true
          note: Optional — LCKK reads back as 1 when the lock is active
          expect:
            LCKK: 1
    fields:
      # ...
```

#### Step fields

Each step has a required `op` key (`read` or `write`) and the following optional keys:

| Key        | Applies to | Description                                                                                                                                     |
| ---------- | ---------- | ----------------------------------------------------------------------------------------------------------------------------------------------- |
| `note`     | any        | Human-readable explanation of the step, including timing or ordering constraints                                                                |
| `optional` | any        | If `true` the step may be omitted. Defaults to `false`                                                                                          |
| `fields`   | `write`    | Field-name → value mapping. Values are non-negative integers (bit pattern for that field, within its bit width) or `"write-arg"` (caller value) |
| `expect`   | `read`     | Field-name → expected-value mapping. Consumers should treat a mismatch as a sequence failure. Omit on read steps where the result is discarded  |

`write-arg` indicates that the caller supplies the value for that field. The sequence `description`
must document what each `write-arg` represents so that consumers can implement the sequence
correctly. `fields` must not appear on `read` steps; `expect` must not appear on `write` steps.

## Validation

Specs are validated against `docs/specs/schema.json` as part of CI (see `lint.sh` and the `lint.yml`
workflow).

To validate locally:

```sh
pip install check-jsonschema
check-jsonschema --schemafile docs/specs/schema.json docs/specs/stm32/stm32u0.yml
```
