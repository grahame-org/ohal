# Future Improvements: Cross-Family and Cross-Vendor Settings Sharing

The current spec format stores all `definitions.settings` blocks inside individual spec files. This
works well for a single family but leads to duplication when the same settings encoding appears
across multiple families or vendors. This document describes the problem and evaluates options for
addressing it.

## Problem

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

## Constraints

The spec format is plain YAML. YAML 1.2 (and YAML 1.1) have no built-in cross-file `include` or
`$ref`. Any cross-file sharing therefore requires either tooling support or a change in the
validation strategy.

## Options

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

### Option D: Spec format versioning

Add a `spec-version` field to every spec file so that tooling can handle multiple format revisions
gracefully as the format evolves:

```yaml
spec-version: "1.0"
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

## Recommendation

1. **Near-term:** Adopt **Option D** (spec-version field) immediately so that format evolution is
   tracked from the start.

2. **Medium-term:** Implement **Option A** (common definitions library) once more than one vendor's
   specs exist and duplication is observed. Keep the merge step simple: a small Python script that
   deep-merges `common/*.yml` definitions into a spec's `definitions.settings` before running
   `check-jsonschema`.

3. **Long-term:** Consider **Option C** (per-vendor schema extensions) once vendor-specific
   constraints (e.g. address range validity, peripheral naming conventions) are worth enforcing.

**Option B** (canonical names in schema) is the most powerful approach for enforcing consistency
but requires custom validator logic that goes beyond standard JSON Schema; defer until there is a
clear need.
