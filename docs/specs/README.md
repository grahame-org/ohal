# Device-Family Register Specifications

This directory contains structured YAML specifications for STM32 device families. Each spec captures
memory-map and peripheral register/field metadata directly from the vendor reference manual.

## Directory Layout

```text
docs/specs/
├── README.md           ← this file
└── stm32/
    ├── schema.json     ← JSON Schema for all STM32 spec files
    └── stm32u0.yml     ← STM32U0 family specification
```

## Format Overview

Every spec file is a YAML document validated against `docs/specs/stm32/schema.json`. The top-level
keys are:

| Key            | Required | Description                                                       |
| -------------- | -------- | ----------------------------------------------------------------- |
| `family`       | ✓        | Family name and sub-family list                                   |
| `architecture` | ✓        | Processor architecture and word size                              |
| `reference`    | ✓        | Reference manual document identifier and revision                 |
| `memory`       |          | Memory map with address ranges and sub-family applicability       |
| `definitions`  |          | Reusable settings blocks (referenced by YAML anchors in the spec) |
| `peripherals`  |          | Peripheral blocks with register and bit-field descriptions        |

## Key Concepts

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
under `sequence`:

```yaml
- LCKR:
    sequence:
      - write:
          - LCKK: 1
            LCKR: bits 15:0
      - write:
          - LCKK: 0
            LCKR: bits 15:0
      - write:
          - LCKK: 1
            LCKR: bits 15:0
      - read:
          - LCKR: bits 31:0
    fields:
      # ...
```

## Validation

Specs are validated against `docs/specs/stm32/schema.json` as part of CI (see `lint.sh` and the
`lint.yml` workflow).

To validate locally:

```sh
pip install check-jsonschema
check-jsonschema --schemafile docs/specs/stm32/schema.json docs/specs/stm32/stm32u0.yml
```
