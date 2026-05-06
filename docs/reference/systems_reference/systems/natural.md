<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# Natural System

**Namespace:** `mp_units::natural`

**Module:** `mp_units.systems`

**Header:** `<mp-units/systems/natural.h>`

## Dimensions

| Name | Symbol |
|------|:------:|
| <span id="dim_energy"></span>`dim_energy` | E |

## Quantities

| Quantity | Character | Dimension | is_kind | non_negative | Kind of | Parent | Equation | Hierarchy |
|----------|:---------:|:---------:|:-------:|:------------:|:-------:|:------:|----------|:---------:|
| <span id="acceleration"></span><code>acceleration</code> | Real | E | — | ✓ | <code>[energy](#energy)</code> | <code>[energy](#energy)</code> | <code>[velocity](#velocity) / <wbr>[duration](#duration)</code> | [view](../hierarchies/energy_natural.md) |
| <span id="angular_measure"></span><code>angular_<wbr>measure</code> | Real | — | ✓ | — | <code>[angular_<wbr>measure](#angular_measure)</code> | <code>[dimensionless](core.md#dimensionless)</code> | — | [view](../hierarchies/dimensionless.md) |
| <span id="dimensionless"></span><code>dimensionless</code> | Real | 1 | ✓ | — | <code>[dimensionless](core.md#dimensionless)</code> | — | — | — |
| <span id="duration"></span><code>duration</code> | Real | E⁻¹ | — | — | <code>[inverse_<wbr>energy](#inverse_energy)</code> | <code>[inverse_<wbr>energy](#inverse_energy)</code> | — | [view](../hierarchies/inverse_energy.md) |
| <span id="energy"></span><code>energy</code> | Real | E | ✓ | ✓ | <code>[energy](#energy)</code> | — | — | [view](../hierarchies/energy_natural.md) |
| <span id="energy_squared"></span><code>energy_<wbr>squared</code> | Real | E² | ✓ | — | <code>[energy_<wbr>squared](#energy_squared)</code> | — | <code>pow<2>([energy](#energy))</code> | [view](../hierarchies/energy_squared.md) |
| <span id="force"></span><code>force</code> | Real | E² | — | — | <code>[energy_<wbr>squared](#energy_squared)</code> | <code>[energy_<wbr>squared](#energy_squared)</code> | <code>[energy_<wbr>squared](#energy_squared)</code> | [view](../hierarchies/energy_squared.md) |
| <span id="inverse_energy"></span><code>inverse_<wbr>energy</code> | Real | E⁻¹ | ✓ | — | <code>[inverse_<wbr>energy](#inverse_energy)</code> | — | <code>inverse([energy](#energy))</code> | [view](../hierarchies/inverse_energy.md) |
| <span id="length"></span><code>length</code> | Real | E⁻¹ | — | — | <code>[inverse_<wbr>energy](#inverse_energy)</code> | <code>[inverse_<wbr>energy](#inverse_energy)</code> | — | [view](../hierarchies/inverse_energy.md) |
| <span id="mass"></span><code>mass</code> | Real | E | — | ✓ | <code>[energy](#energy)</code> | <code>[energy](#energy)</code> | — | [view](../hierarchies/energy_natural.md) |
| <span id="momentum"></span><code>momentum</code> | Real | E | — | ✓ | <code>[energy](#energy)</code> | <code>[energy](#energy)</code> | <code>[mass](#mass) * <wbr>[velocity](#velocity)</code> | [view](../hierarchies/energy_natural.md) |
| <span id="speed"></span><code>speed</code> | Real | — | ✓ | — | <code>[speed](#speed)</code> | <code>[dimensionless](core.md#dimensionless)</code> | <code>[length](#length) / <wbr>[duration](#duration)</code> | [view](../hierarchies/dimensionless.md) |
| <span id="velocity"></span><code>velocity</code> | Real | — | — | — | <code>[speed](#speed)</code> | <code>[speed](#speed)</code> | — | [view](../hierarchies/dimensionless.md) |

## Units

| Unit Name | Symbol | unit_symbol | Definition |
|-----------|:------:|:-----------:|------------|
| <span id="electronvolt"></span><code>electronvolt</code> | eV | — | <code>kind: [energy](#energy)</code> |
| <span id="gigaelectronvolt"></span><code>gigaelectronvolt</code> | — | `GeV` | <code>[si::giga](si.md#giga)<[electronvolt](#electronvolt)></code> |

## Constants

| Name | Symbol | unit_symbol | Definition |
|------|:------:|:------------:|------------|
| <span id="reduced_planck_constant"></span><code>reduced_<wbr>planck_<wbr>constant</code> | \u210f (hbar) | `hbar` | <code>[one](core.md#one)</code> |
| <span id="speed_of_light_in_vacuum"></span><code>speed_<wbr>of_<wbr>light_<wbr>in_<wbr>vacuum</code> | c | `c` | <code>[one](core.md#one)</code> |
