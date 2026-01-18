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
| <span id="angle"></span><code>angle</code> | Real | α | <code>[angle](#angle)</code> | — | — | [view](../hierarchies/angle.md) |
| <span id="solid_angle"></span><code>solid_<wbr>angle</code> | Real | α² | <code>[solid_<wbr>angle](#solid_angle)</code> | — | <code>pow<2>([angle](#angle))</code> | [view](../hierarchies/solid_angle.md) |

## Units

| Unit Name | Symbol | unit_symbol | Definition |
|-----------|:------:|:-----------:|------------|
| <span id="degree"></span><code>degree</code> | ° (deg) | `deg` | <code>mag_ratio<1, 360> * <wbr>[revolution](#revolution)</code> |
| <span id="gradian"></span><code>gradian</code> | ᵍ (grad) | `grad` | <code>mag_ratio<1, 400> * <wbr>[revolution](#revolution)</code> |
| <span id="radian"></span><code>radian</code> | rad | `rad` | <code>kind: [angle](#angle)</code> |
| <span id="revolution"></span><code>revolution</code> | rev | `rev` | <code>mag<2> * <wbr>[π](core.md#π) * <wbr>[radian](#radian)</code> |
| <span id="steradian"></span><code>steradian</code> | sr | `sr` | <code>square([radian](#radian))</code> |
