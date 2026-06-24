// The MIT License (MIT)
//
// Copyright (c) 2018 Mateusz Pusz
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <mp-units/bits/module_macros.h>

// IWYU pragma: begin_exports
#include <mp-units/systems/isq/si_quantities.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework/quantity_spec.h>
#endif
// IWYU pragma: end_exports

MP_UNITS_EXPORT
namespace mp_units::isq {

// WORKAROUND for V2: altitude and depth are signed vertical coordinates (position above/below
// a reference plane), while height is the unsigned magnitude. The correct V3 hierarchy would be
// length -> height -> point_for<height> {altitude, depth}, but V2 lacks point_for<>.
// To avoid special-case overrides and enable implicit height->altitude conversions in affine
// space operations, we temporarily reverse the hierarchy: altitude and depth are children of
// length (signed coordinates), and height is a child of altitude (explicitly non-negative).
// This will be corrected in V3 when point_for<> becomes available.
QUANTITY_SPEC(altitude, length);  // signed vertical coordinate (can be negative)
// depth is defined in ISQ but explicitly not supported
// (it should not be a distinct quantity, it just needs a dedicated origin with projection)

// Note that `depth` intentionally does not appear here as a `quantity_spec`. Depth represents a
// vertical position measured *downward* from the ocean surface — an axis-inverted form of altitude.
// Axis direction is a property of the coordinate system, not of the physical quantity: a vertical
// position is a vertical position regardless of which way the axis points. Modeling `depth` as a
// sibling `quantity_spec` of `altitude` would allow `point<depth> - point<altitude>` to silently
// yield a wrong-signed result, because both specs share the same delta type (`height`) yet measure
// in opposite directions. The correct model is to define `ocean_surface` as an
// `absolute_point_origin<isq::altitude>` whose `frame_projection` negates the value (see
// [Runtime frame projections](#frame-proj)). No `depth` quantity spec is needed — axis direction
// belongs in the frame, not in the quantity. For the full argument against encoding axis direction
// as a distinct quantity type, see [Why not model bounded variants as distinct quantities in the
// hierarchy?](#bounds-no-hierarchy).

// inline constexpr auto depth = altitude;
QUANTITY_SPEC(height, altitude, non_negative);  // unsigned magnitude; child of altitude for implicit conversion

// HACK: Override is_non_negative() for signed coordinates.
// Even though altitude and depth inherit from non-negative length, they represent signed positions.
[[nodiscard]] consteval bool is_non_negative(decltype(altitude)) { return false; }

QUANTITY_SPEC(thickness, width);
QUANTITY_SPEC(diameter, width);
QUANTITY_SPEC(distance, path_length);
QUANTITY_SPEC(radial_distance, distance);
QUANTITY_SPEC(displacement, length, quantity_tensor_order::vector);
QUANTITY_SPEC(position_vector, displacement);
QUANTITY_SPEC(radius_of_curvature, radius);
QUANTITY_SPEC(curvature, inverse(radius_of_curvature), non_negative);
QUANTITY_SPEC(volume, pow<3>(length), non_negative);
QUANTITY_SPEC(rotational_displacement, angular_measure, path_length / radius);
inline constexpr auto angular_displacement = rotational_displacement;
QUANTITY_SPEC(phase_angle, angular_measure, is_kind);
QUANTITY_SPEC(speed, length / time, non_negative);        // differs from ISO 80000
QUANTITY_SPEC(velocity, speed, displacement / duration);  // vector  // differs from ISO 80000
QUANTITY_SPEC(acceleration, velocity / duration);         // vector
QUANTITY_SPEC(acceleration_of_free_fall, acceleration);   // not in ISO 80000
QUANTITY_SPEC(angular_velocity, angular_displacement / duration, quantity_tensor_order::vector);
QUANTITY_SPEC(angular_acceleration, angular_velocity / duration);
QUANTITY_SPEC(time_constant, duration);
QUANTITY_SPEC(rotation, dimensionless);
QUANTITY_SPEC(rotational_frequency, rotation / duration);
QUANTITY_SPEC(angular_frequency, phase_angle / duration);
QUANTITY_SPEC(wavelength, length);
QUANTITY_SPEC(repetency, inverse(wavelength), non_negative);
inline constexpr auto wavenumber = repetency;
QUANTITY_SPEC(wave_vector, repetency, quantity_tensor_order::vector);
QUANTITY_SPEC(angular_repetency, inverse(wavelength), non_negative);
inline constexpr auto angular_wavenumber = angular_repetency;
QUANTITY_SPEC(phase_speed, angular_frequency / angular_repetency);
QUANTITY_SPEC(group_speed, angular_frequency / angular_repetency);
QUANTITY_SPEC(damping_coefficient, inverse(time_constant), non_negative);
QUANTITY_SPEC(logarithmic_decrement, dimensionless, damping_coefficient* period_duration);
QUANTITY_SPEC(attenuation, inverse(distance), non_negative);
inline constexpr auto extinction = attenuation;
QUANTITY_SPEC(phase_coefficient, phase_angle / path_length);
QUANTITY_SPEC(propagation_coefficient, inverse(length), non_negative);  // γ = α + iβ where α denotes attenuation
                                                                        // and β the phase coefficient of a plane wave

}  // namespace mp_units::isq
