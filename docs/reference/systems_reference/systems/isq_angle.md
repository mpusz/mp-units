<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# ISQ Angle System

**Namespace:** `mp_units::isq_angle`

**Module:** `mp_units.systems`

**Header:** `<mp-units/systems/isq_angle.h>`

## Quantities

| Quantity | Character | Dimension | Traits | Kind of | Parent | Equation | Hierarchy |
|----------|:---------:|:---------:|:------:|:-------:|:------:|----------|:---------:|
| <span id="angular_acceleration"></span><code>angular_<wbr>acceleration</code> | vector | αT⁻² | kind | <code>[angular_<wbr>acceleration](#angular_acceleration)</code> | — | <code>[angular_<wbr>velocity](#angular_velocity) / <wbr>[isq::duration](isq.md#duration)</code> | [view](../hierarchies/angular_acceleration_isq_angle.md) |
| <span id="angular_displacement"></span><code>angular_<wbr>displacement</code> | scalar | α |  | <code>[angular::angle](angular.md#angle)</code> | <code>[angular::angle](angular.md#angle)</code> | alias to [rotational_<wbr>displacement](#rotational_displacement) | [view](../hierarchies/angle.md) |
| <span id="angular_frequency"></span><code>angular_<wbr>frequency</code> | scalar | αT⁻¹ | kind | <code>[angular_<wbr>frequency](#angular_frequency)</code> | — | <code>[phase_<wbr>angle](#phase_angle) / <wbr>[isq::duration](isq.md#duration)</code> | [view](../hierarchies/angular_frequency_isq_angle.md) |
| <span id="angular_impulse"></span><code>angular_<wbr>impulse</code> | vector | L²Mα⁻¹T⁻¹ | kind | <code>[angular_<wbr>impulse](#angular_impulse)</code> | — | <code>[moment_<wbr>of_<wbr>force](#moment_of_force) * <wbr>[isq::duration](isq.md#duration)</code> | [view](../hierarchies/angular_impulse.md) |
| <span id="angular_measure"></span><code>angular_<wbr>measure</code> | scalar | α |  | <code>[angular::angle](angular.md#angle)</code> | <code>[angular::angle](angular.md#angle)</code> | <code>[cotes_<wbr>angle_<wbr>constant](#cotes_angle_constant) * <wbr>[isq::arc_<wbr>length](isq.md#arc_length) / <wbr>[isq::radius](isq.md#radius)</code> | [view](../hierarchies/angle.md) |
| <span id="angular_momentum"></span><code>angular_<wbr>momentum</code> | vector | L²Mα⁻¹T⁻¹ | kind | <code>[angular_<wbr>momentum](#angular_momentum)</code> | — | <code>[isq::position_<wbr>vector](isq.md#position_vector) * <wbr>[isq::momentum](isq.md#momentum) / <wbr>[cotes_<wbr>angle_<wbr>constant](#cotes_angle_constant)</code> | [view](../hierarchies/angular_momentum_isq_angle.md) |
| <span id="angular_repetency"></span><code>angular_<wbr>repetency</code> | scalar | αL⁻¹ | kind | <code>[angular_<wbr>repetency](#angular_repetency)</code> | — | <code>[cotes_<wbr>angle_<wbr>constant](#cotes_angle_constant) / <wbr>[isq::wavelength](isq.md#wavelength)</code> | [view](../hierarchies/angular_repetency_isq_angle.md) |
| <span id="angular_velocity"></span><code>angular_<wbr>velocity</code> | vector | αT⁻¹ | kind | <code>[angular_<wbr>velocity](#angular_velocity)</code> | — | <code>[angular_<wbr>displacement](#angular_displacement) / <wbr>[isq::duration](isq.md#duration)</code> | [view](../hierarchies/angular_velocity_isq_angle.md) |
| <span id="angular_wavenumber"></span><code>angular_<wbr>wavenumber</code> | scalar | αL⁻¹ | kind | <code>[angular_<wbr>repetency](#angular_repetency)</code> | — | alias to [angular_<wbr>repetency](#angular_repetency) | [view](../hierarchies/angular_repetency_isq_angle.md) |
| <span id="cotes_angle_constant"></span><code>cotes_<wbr>angle_<wbr>constant</code> | scalar | α |  | <code>[angular::angle](angular.md#angle)</code> | <code>[angular::angle](angular.md#angle)</code> | — | [view](../hierarchies/angle.md) |
| <span id="loss_angle"></span><code>loss_<wbr>angle</code> | scalar | α |  | <code>[angular::angle](angular.md#angle)</code> | <code>[angular_<wbr>measure](#angular_measure)</code> | — | [view](../hierarchies/angle.md) |
| <span id="moment_of_force"></span><code>moment_<wbr>of_<wbr>force</code> | vector | L²Mα⁻¹T⁻² | kind | <code>[moment_<wbr>of_<wbr>force](#moment_of_force)</code> | — | <code>[isq::position_<wbr>vector](isq.md#position_vector) * <wbr>[isq::force](isq.md#force) / <wbr>[cotes_<wbr>angle_<wbr>constant](#cotes_angle_constant)</code> | [view](../hierarchies/moment_of_force_isq_angle.md) |
| <span id="moment_of_inertia"></span><code>moment_<wbr>of_<wbr>inertia</code> | tensor | L²Mα⁻² | kind | <code>[moment_<wbr>of_<wbr>inertia](#moment_of_inertia)</code> | — | <code>[angular_<wbr>momentum](#angular_momentum) / <wbr>[angular_<wbr>velocity](#angular_velocity)</code> | [view](../hierarchies/moment_of_inertia_isq_angle.md) |
| <span id="phase_angle"></span><code>phase_<wbr>angle</code> | scalar | α |  | <code>[angular::angle](angular.md#angle)</code> | <code>[angular_<wbr>measure](#angular_measure)</code> | — | [view](../hierarchies/angle.md) |
| <span id="phase_coefficient"></span><code>phase_<wbr>coefficient</code> | scalar | αL⁻¹ | kind | <code>[phase_<wbr>coefficient](#phase_coefficient)</code> | — | <code>[phase_<wbr>angle](#phase_angle) / <wbr>[isq::path_<wbr>length](isq.md#path_length)</code> | [view](../hierarchies/phase_coefficient_isq_angle.md) |
| <span id="propagation_coefficient"></span><code>propagation_<wbr>coefficient</code> | scalar | αL⁻¹ | kind | <code>[propagation_<wbr>coefficient](#propagation_coefficient)</code> | — | <code>[cotes_<wbr>angle_<wbr>constant](#cotes_angle_constant) / <wbr>[isq::length](isq.md#length)</code> | [view](../hierarchies/propagation_coefficient_isq_angle.md) |
| <span id="rotation"></span><code>rotation</code> | scalar | α |  | <code>[angular::angle](angular.md#angle)</code> | <code>[rotational_<wbr>displacement](#rotational_displacement)</code> | — | [view](../hierarchies/angle.md) |
| <span id="rotational_displacement"></span><code>rotational_<wbr>displacement</code> | scalar | α |  | <code>[angular::angle](angular.md#angle)</code> | <code>[angular::angle](angular.md#angle)</code> | <code>[cotes_<wbr>angle_<wbr>constant](#cotes_angle_constant) * <wbr>[isq::path_<wbr>length](isq.md#path_length) / <wbr>[isq::radius](isq.md#radius)</code> | [view](../hierarchies/angle.md) |
| <span id="solid_angular_measure"></span><code>solid_<wbr>angular_<wbr>measure</code> | scalar | α² | kind | <code>[solid_<wbr>angular_<wbr>measure](#solid_angular_measure)</code> | — | <code>pow<2>([cotes_<wbr>angle_<wbr>constant](#cotes_angle_constant)) * <wbr>[isq::area](isq.md#area) / <wbr>pow<2>([isq::radius](isq.md#radius))</code> | [view](../hierarchies/solid_angular_measure.md) |
| <span id="torque"></span><code>torque</code> | scalar | L²Mα⁻¹T⁻² |  | <code>[moment_<wbr>of_<wbr>force](#moment_of_force)</code> | <code>[moment_<wbr>of_<wbr>force](#moment_of_force)</code> | — | [view](../hierarchies/moment_of_force_isq_angle.md) |
