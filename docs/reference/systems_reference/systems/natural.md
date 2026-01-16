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
| <span id="acceleration"></span>`acceleration` | Real | E | `natural::energy` | `natural::energy` | — | [view](../hierarchies/energy_natural.md) |
| <span id="angular_measure"></span>`angular_measure` | Real | — | `dimensionless` | `dimensionless` | — | [view](../hierarchies/dimensionless.md) |
| <span id="dimensionless"></span>`dimensionless` | Real | 1 | `dimensionless` | — | — | [view](../hierarchies/dimensionless.md) |
| <span id="energy"></span>`energy` | Real | E | `natural::energy` | — | — | [view](../hierarchies/energy_natural.md) |
| <span id="energy_squared"></span>`energy_squared` | Real | E² | `natural::energy_squared` | — | `pow<2>(energy)` | [view](../hierarchies/energy_squared.md) |
| <span id="force"></span>`force` | Real | E² | `natural::energy_squared` | `natural::energy_squared` | `energy_squared` | [view](../hierarchies/energy_squared.md) |
| <span id="inverse_energy"></span>`inverse_energy` | Real | E⁻¹ | `natural::inverse_energy` | — | `inverse(energy)` | [view](../hierarchies/inverse_energy.md) |
| <span id="length"></span>`length` | Real | E⁻¹ | `natural::inverse_energy` | `natural::inverse_energy` | — | [view](../hierarchies/inverse_energy.md) |
| <span id="mass"></span>`mass` | Real | E | `natural::energy` | `natural::energy` | — | [view](../hierarchies/energy_natural.md) |
| <span id="momentum"></span>`momentum` | Real | E | `natural::energy` | `natural::energy` | — | [view](../hierarchies/energy_natural.md) |
| <span id="speed"></span>`speed` | Real | — | `dimensionless` | `dimensionless` | — | [view](../hierarchies/dimensionless.md) |
| <span id="temperature"></span>`temperature` | Real | E | `natural::energy` | `natural::energy` | — | [view](../hierarchies/energy_natural.md) |
| <span id="time"></span>`time` | Real | E⁻¹ | `natural::inverse_energy` | `natural::inverse_energy` | — | [view](../hierarchies/inverse_energy.md) |
| <span id="velocity"></span>`velocity` | Real | — | `dimensionless` | `natural::speed` | — | [view](../hierarchies/dimensionless.md) |

## Units

| Unit Name | Symbol | unit_symbol | Definition |
|-----------|:------:|:-----------:|------------|
| <span id="electronvolt"></span>`electronvolt` | eV | — | kind: `energy` |
| <span id="gigaelectronvolt"></span>`gigaelectronvolt` | — | `GeV` | `si::giga<electronvolt>` |
| <span id="speed_of_light"></span>`speed_of_light` | — | — | alias to `mp_units::one` |
