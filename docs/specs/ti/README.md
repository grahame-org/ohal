# TI Spec Conventions

This document records Texas Instruments–specific conventions used in the OHAL spec files under
`docs/specs/ti/`. Read it alongside `docs/specs/README.md`, which describes the general spec
format.

## MSP430 port-letter mapping

The OHAL C++ headers map MSP430 port numbers to letters (A = P1, B = P2, …). The model specs
follow the same mapping so that port identifiers match the schema constraint (`^[A-Z]$`):

| MSP430 port | OHAL letter |
| ----------- | ----------- |
| P1          | A           |
| P2          | B           |
| P3          | C           |
| P4          | D           |
| P5          | E           |
| P6          | F           |

Pin names in model specs therefore take the form `P{letter}{bit}` (e.g. `PA2`, `PB7`).

## MSP430 alternate-function key encoding

MSP430 pin function selection is controlled by two registers (`PxSEL0`, `PxSEL1`) that combine to
form a 2-bit `SEL[1:0]` value, plus the `PxDIR` direction register for functions that are
input-only or output-only. The `functions` keys in model specs encode these hardware bits
directly:

| Key              | `SEL[1:0]` | `DIR` | Meaning                                                    |
| ---------------- | ---------- | ----- | ---------------------------------------------------------- |
| `"SEL=01"`       | `01`       | any   | Module function 1, direction-independent                   |
| `"SEL=01,DIR=0"` | `01`       | 0     | Module function 1, input only                              |
| `"SEL=01,DIR=1"` | `01`       | 1     | Module function 1, output only                             |
| `"SEL=10"`       | `10`       | any   | Module function 2, direction-independent                   |
| `"SEL=10,DIR=0"` | `10`       | 0     | Module function 2, input only                              |
| `"SEL=10,DIR=1"` | `10`       | 1     | Module function 2, output only                             |
| `"SEL=11"`       | `11`       | any   | Module function 3 / analog, direction-independent          |
| `"JTAG"`         | —          | —     | JTAG override (active regardless of SEL when JTAG enabled) |

GPIO mode (`SEL=00`) is the reset default and is **not** listed as a `functions` entry, consistent
with STM32 model specs which do not list `MODER=00`.

## MSP430 comparator peripheral naming

> **Note — easy-to-confuse signal names:** the TI comparator peripheral uses both a digit `0`/`1`
> and a capital letter `O` (for "Output") in its signal names. These look almost identical in many
> fonts.

The three signal classes for comparator instance _n_ are:

| Signal name | Character  | Meaning            |
| ----------- | ---------- | ------------------ |
| `COMPn.0`   | digit `0`  | Comparator input 0 |
| `COMPn.1`   | digit `1`  | Comparator input 1 |
| `COMPn.O`   | letter `O` | Comparator output  |

When reviewing or authoring TI model specs, verify carefully which character is intended before
flagging `COMPn.O` as a typo.
