<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# ISQ Angle System

**Namespace:** `mp_units::isq_angle`

**Module:** `mp_units.systems`

**Header:** `<mp-units/systems/isq_angle.h>`

## Quantities

| Quantity | Character | Dimension | Kind of | Parent | Equation | Hierarchy |
|----------|:---------:|:---------:|:-------:|:------:|----------|:---------:|
| <span id="angular_acceleration"></span>`angular_acceleration` | Vector | αT⁻² | `isq_angle::angular_acceleration` | — | `angular_velocity / duration` | [view](../hierarchies/angular_acceleration_isq_angle.md) |
| <span id="angular_displacement"></span>`angular_displacement` | Real | α | `angular::angle` | `angular::angle` | alias to `rotational_displacement` | [view](../hierarchies/angle.md) |
| <span id="angular_frequency"></span>`angular_frequency` | Real | αT⁻¹ | `isq_angle::angular_frequency` | — | `phase_angle / duration` | [view](../hierarchies/angular_frequency_isq_angle.md) |
| <span id="angular_impulse"></span>`angular_impulse` | Vector | L²Mα⁻¹T⁻¹ | `isq_angle::angular_impulse` | — | `moment_of_force * time` | [view](../hierarchies/angular_impulse_isq_angle.md) |
| <span id="angular_measure"></span>`angular_measure` | Real | α | `angular::angle` | `angular::angle` | `cotes_angle_constant * arc_length / radius` | [view](../hierarchies/angle.md) |
| <span id="angular_momentum"></span>`angular_momentum` | Vector | L²Mα⁻¹T⁻¹ | `isq_angle::angular_momentum` | — | `position_vector * momentum / cotes_angle_constant` | [view](../hierarchies/angular_momentum_isq_angle.md) |
| <span id="angular_repetency"></span>`angular_repetency` | Real | αL⁻¹ | `isq_angle::angular_repetency` | — | `cotes_angle_constant / wavelength` | [view](../hierarchies/angular_repetency_isq_angle.md) |
| <span id="angular_velocity"></span>`angular_velocity` | Vector | αT⁻¹ | `isq_angle::angular_velocity` | — | `angular_displacement / duration` | [view](../hierarchies/angular_velocity_isq_angle.md) |
| <span id="angular_wavenumber"></span>`angular_wavenumber` | Real | αL⁻¹ | `isq_angle::angular_repetency` | — | alias to `angular_repetency` | [view](../hierarchies/angular_repetency_isq_angle.md) |
| <span id="cotes_angle_constant"></span>`cotes_angle_constant` | Real | α | `angular::angle` | `angular::angle` | — | [view](../hierarchies/angle.md) |
| <span id="loss_angle"></span>`loss_angle` | Real | α | `angular::angle` | `isq_angle::angular_measure` | — | [view](../hierarchies/angle.md) |
| <span id="moment_of_force"></span>`moment_of_force` | Vector | L²Mα⁻¹T⁻² | `isq_angle::moment_of_force` | — | `position_vector * force / cotes_angle_constant` | [view](../hierarchies/moment_of_force_isq_angle.md) |
| <span id="moment_of_inertia"></span>`moment_of_inertia` | Tensor | L²Mα⁻² | `isq_angle::moment_of_inertia` | — | `angular_momentum / angular_velocity` | [view](../hierarchies/moment_of_inertia_isq_angle.md) |
| <span id="phase_angle"></span>`phase_angle` | Real | α | `angular::angle` | `isq_angle::angular_measure` | — | [view](../hierarchies/angle.md) |
| <span id="phase_coefficient"></span>`phase_coefficient` | Real | αL⁻¹ | `isq_angle::phase_coefficient` | — | `phase_angle / path_length` | [view](../hierarchies/phase_coefficient_isq_angle.md) |
| <span id="propagation_coefficient"></span>`propagation_coefficient` | Real | αL⁻¹ | `isq_angle::propagation_coefficient` | — | `cotes_angle_constant / length` | [view](../hierarchies/propagation_coefficient_isq_angle.md) |
| <span id="rotation"></span>`rotation` | Real | α | `angular::angle` | `isq_angle::rotational_displacement` | — | [view](../hierarchies/angle.md) |
| <span id="rotational_displacement"></span>`rotational_displacement` | Real | α | `angular::angle` | `angular::angle` | `cotes_angle_constant * path_length / radius` | [view](../hierarchies/angle.md) |
| <span id="solid_angular_measure"></span>`solid_angular_measure` | Real | α² | `isq_angle::solid_angular_measure` | — | `pow<2>(cotes_angle_constant) * area / pow<2>(radius)` | [view](../hierarchies/solid_angular_measure.md) |
| <span id="torque"></span>`torque` | Real | L²Mα⁻¹T⁻² | `isq_angle::moment_of_force` | `isq_angle::moment_of_force` | — | [view](../hierarchies/moment_of_force_isq_angle.md) |
