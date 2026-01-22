<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# Core System

**Namespace:** `mp_units`

**Module:** `mp_units.core`

**Header:** `<mp-units/core.h>`

## Dimensions

| Name | Symbol |
|------|:------:|
| <span id="dimension_one"></span>`dimension_one` | 1 |

## Quantities

| Quantity | Character | Dimension | is_kind | Kind of | Parent | Equation | Hierarchy |
|----------|:---------:|:---------:|:-------:|:-------:|:------:|----------|:---------:|
| <span id="dimensionless"></span><code>dimensionless</code> | Real | 1 | ✓ | <code>[dimensionless](core.md#dimensionless)</code> | — | — | [view](../hierarchies/dimensionless.md) |

## Units

| Unit Name | Symbol | unit_symbol | Definition |
|-----------|:------:|:-----------:|------------|
| <span id="one"></span><code>one</code> | 1 | — | — |
| <span id="parts_per_million"></span><code>parts_<wbr>per_<wbr>million</code> | ppm | — | <code>mag_ratio<1, 1'000'000> * <wbr>[one](core.md#one)</code> |
| <span id="per_mille"></span><code>per_<wbr>mille</code> | ‰ (%o) | — | <code>mag_ratio<1, 1000> * <wbr>[one](core.md#one)</code> |
| <span id="percent"></span><code>percent</code> | % | — | <code>mag_ratio<1, 100> * <wbr>[one](core.md#one)</code> |
| <span id="pi"></span><code>pi</code> | π (pi) | — | <code>mag<pi_c> * <wbr>[one](core.md#one)</code> |
| <span id="ppm"></span><code>ppm</code> | — | — | alias to [mp_<wbr>units::parts_<wbr>per_<wbr>million](core.md#parts_per_million) |
| <span id="π"></span><code>π</code> | — | — | alias to [mp_<wbr>units::pi](core.md#pi) |
