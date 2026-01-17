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

| Quantity | Character | Dimension | Kind of | Parent | Equation | Hierarchy |
|----------|:---------:|:---------:|:-------:|:------:|----------|:---------:|
| <span id="acceleration"></span><code>acceleration</code> | Real | E | <code>[energy](#energy)</code> | <code>[energy](#energy)</code> | — | [view](../hierarchies/energy_natural.md) |
| <span id="angular_measure"></span><code>angular_<wbr>measure</code> | Real | — | <code>[dimensionless](core.md#dimensionless)</code> | <code>[dimensionless](core.md#dimensionless)</code> | — | [view](../hierarchies/dimensionless.md) |
| <span id="dimensionless"></span><code>dimensionless</code> | Real | 1 | <code>[dimensionless](core.md#dimensionless)</code> | — | — | [view](../hierarchies/dimensionless.md) |
| <span id="energy"></span><code>energy</code> | Real | E | <code>[energy](#energy)</code> | — | — | [view](../hierarchies/energy_natural.md) |
| <span id="energy_squared"></span><code>energy_<wbr>squared</code> | Real | E² | <code>[energy_<wbr>squared](#energy_squared)</code> | — | <code>pow<2>([energy](#energy))</code> | [view](../hierarchies/energy_squared.md) |
| <span id="force"></span><code>force</code> | Real | E² | <code>[energy_<wbr>squared](#energy_squared)</code> | <code>[energy_<wbr>squared](#energy_squared)</code> | <code>[energy_<wbr>squared](#energy_squared)</code> | [view](../hierarchies/energy_squared.md) |
| <span id="inverse_energy"></span><code>inverse_<wbr>energy</code> | Real | E⁻¹ | <code>[inverse_<wbr>energy](#inverse_energy)</code> | — | <code>inverse([energy](#energy))</code> | [view](../hierarchies/inverse_energy.md) |
| <span id="length"></span><code>length</code> | Real | E⁻¹ | <code>[inverse_<wbr>energy](#inverse_energy)</code> | <code>[inverse_<wbr>energy](#inverse_energy)</code> | — | [view](../hierarchies/inverse_energy.md) |
| <span id="mass"></span><code>mass</code> | Real | E | <code>[energy](#energy)</code> | <code>[energy](#energy)</code> | — | [view](../hierarchies/energy_natural.md) |
| <span id="momentum"></span><code>momentum</code> | Real | E | <code>[energy](#energy)</code> | <code>[energy](#energy)</code> | — | [view](../hierarchies/energy_natural.md) |
| <span id="speed"></span><code>speed</code> | Real | — | <code>[dimensionless](core.md#dimensionless)</code> | <code>[dimensionless](core.md#dimensionless)</code> | — | [view](../hierarchies/dimensionless.md) |
| <span id="temperature"></span><code>temperature</code> | Real | E | <code>[energy](#energy)</code> | <code>[energy](#energy)</code> | — | [view](../hierarchies/energy_natural.md) |
| <span id="time"></span><code>time</code> | Real | E⁻¹ | <code>[inverse_<wbr>energy](#inverse_energy)</code> | <code>[inverse_<wbr>energy](#inverse_energy)</code> | — | [view](../hierarchies/inverse_energy.md) |
| <span id="velocity"></span><code>velocity</code> | Real | — | <code>[dimensionless](core.md#dimensionless)</code> | <code>[speed](#speed)</code> | — | [view](../hierarchies/dimensionless.md) |

## Units

| Unit Name | Symbol | unit_symbol | Definition |
|-----------|:------:|:-----------:|------------|
| <span id="electronvolt"></span><code>electronvolt</code> | eV | — | <code>kind: [energy](#energy)</code> |
| <span id="gigaelectronvolt"></span><code>gigaelectronvolt</code> | — | `GeV` | <code>[si::giga](si.md#giga)<[electronvolt](#electronvolt)></code> |
| <span id="speed_of_light"></span><code>speed_<wbr>of_<wbr>light</code> | — | — | alias to [mp_<wbr>units::one](core.md#one) |
