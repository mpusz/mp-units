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
#include <mp-units/systems/isq/si_quantities.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/quantity_spec.h>
#endif

MP_UNITS_EXPORT
namespace mp_units::isq {

QUANTITY_SPEC(height, length);
inline constexpr auto depth = height;
inline constexpr auto altitude = height;
QUANTITY_SPEC(thickness, width);
QUANTITY_SPEC(diameter, width);
QUANTITY_SPEC(distance, path_length);
QUANTITY_SPEC(radial_distance, distance);
QUANTITY_SPEC(position_vector, length, quantity_character::vector);
QUANTITY_SPEC(displacement, length, quantity_character::vector);
QUANTITY_SPEC(radius_of_curvature, radius);
QUANTITY_SPEC(curvature, inverse(radius_of_curvature));
QUANTITY_SPEC(volume, pow<3>(length));
QUANTITY_SPEC(rotational_displacement, angular_measure, path_length / radius);
inline constexpr auto angular_displacement = rotational_displacement;
QUANTITY_SPEC(phase_angle, angular_measure);
QUANTITY_SPEC(speed, length / time);                         // differs from ISO 80000
QUANTITY_SPEC(velocity, speed, position_vector / duration);  // vector  // differs from ISO 80000
QUANTITY_SPEC(acceleration, velocity / duration);            // vector
QUANTITY_SPEC(acceleration_of_free_fall, acceleration);      // not in ISO 80000
QUANTITY_SPEC(angular_velocity, angular_displacement / duration, quantity_character::vector);
QUANTITY_SPEC(angular_acceleration, angular_velocity / duration);
QUANTITY_SPEC(time_constant, duration);
QUANTITY_SPEC(rotation, dimensionless);
QUANTITY_SPEC(rotational_frequency, rotation / duration);
QUANTITY_SPEC(angular_frequency, phase_angle / duration);
QUANTITY_SPEC(wavelength, length);
QUANTITY_SPEC(repetency, inverse(wavelength));
inline constexpr auto wavenumber = repetency;
QUANTITY_SPEC(wave_vector, repetency, quantity_character::vector);
QUANTITY_SPEC(angular_repetency, inverse(wavelength));
inline constexpr auto angular_wavenumber = angular_repetency;
QUANTITY_SPEC(phase_velocity, angular_frequency / angular_repetency);
inline constexpr auto phase_speed = phase_velocity;
QUANTITY_SPEC(group_velocity, angular_frequency / angular_repetency);
inline constexpr auto group_speed = group_velocity;
QUANTITY_SPEC(damping_coefficient, inverse(time_constant));
QUANTITY_SPEC(logarithmic_decrement, dimensionless, damping_coefficient* period_duration);
QUANTITY_SPEC(attenuation, inverse(distance));
inline constexpr auto extinction = attenuation;
QUANTITY_SPEC(phase_coefficient, phase_angle / path_length);
QUANTITY_SPEC(propagation_coefficient, inverse(length));  // γ = α + iβ where α denotes attenuation
                                                          // and β the phase coefficient of a plane wave

}  // namespace mp_units::isq
