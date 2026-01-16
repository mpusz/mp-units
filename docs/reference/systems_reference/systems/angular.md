<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# Angular System

**Namespace:** `mp_units::angular`

**Module:** `mp_units.systems`

**Header:** `<mp-units/systems/angular.h>`

**Secondary Headers:**

- `<mp-units/systems/angular/math.h>`
- `<mp-units/systems/angular/units.h>`

## Dimensions

| Name | Symbol |
|------|:------:|
| <span id="dim_angle"></span>`dim_angle` | α |

## Quantities

| Quantity | Character | Dimension | Kind of | Parent | Equation | Hierarchy |
|----------|:---------:|:---------:|:-------:|:------:|----------|:---------:|
| <span id="angle"></span>`angle` | Real | α | `angular::angle` | — | — | [view](../hierarchies/angle.md) |
| <span id="solid_angle"></span>`solid_angle` | Real | α² | `angular::solid_angle` | — | `pow<2>(angle)` | [view](../hierarchies/solid_angle.md) |

## Units

| Unit Name | Symbol | unit_symbol | Definition |
|-----------|:------:|:-----------:|------------|
| <span id="degree"></span>`degree` | ° (deg) | `deg` | `mag_ratio<1, 360> * revolution` |
| <span id="gradian"></span>`gradian` | ᵍ (grad) | `grad` | `mag_ratio<1, 400> * revolution` |
| <span id="radian"></span>`radian` | rad | `rad` | kind: `angle` |
| <span id="revolution"></span>`revolution` | rev | `rev` | `mag<2> * π * radian` |
| <span id="steradian"></span>`steradian` | sr | `sr` | `square(radian)` |
