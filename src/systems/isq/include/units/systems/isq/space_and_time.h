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

#include <units/systems/isq/base_quantities.h>
#include <units/quantity_spec.h>

namespace units::isq {

QUANTITY_SPEC(width, length);
inline constexpr auto breadth = width;
QUANTITY_SPEC(height, length);
inline constexpr auto depth = height;
inline constexpr auto altitude = height;
QUANTITY_SPEC(thickness, width);
QUANTITY_SPEC(diameter, width);
// QUANTITY_SPEC(radius, mag<ratio{1, 2}> * diameter); // TODO should we support that?
QUANTITY_SPEC(radius, width);
QUANTITY_SPEC(path_length, length);
inline constexpr auto arc_length = path_length;
QUANTITY_SPEC(distance, path_length);
QUANTITY_SPEC(radial_distance, distance);
QUANTITY_SPEC(position_vector, length, quantity_character::vector);
QUANTITY_SPEC(displacement, length, quantity_character::vector);
QUANTITY_SPEC(radius_of_curvature, radius);
QUANTITY_SPEC(curvature, 1 / radius_of_curvature);
QUANTITY_SPEC(area, pow<2>(length));
QUANTITY_SPEC(volume, pow<3>(length));
QUANTITY_SPEC(angular_measure, arc_length / radius);
QUANTITY_SPEC(rotational_displacement, path_length / radius);
inline constexpr auto angular_displacement = rotational_displacement;
QUANTITY_SPEC(phase_angle, angular_measure);
QUANTITY_SPEC(solid_angular_measure, area / pow<2>(radius));
inline constexpr auto duration = time;
QUANTITY_SPEC(velocity, position_vector / duration);  // vector
QUANTITY_SPEC(speed, distance / duration);            // TODO length, path_length?
QUANTITY_SPEC(acceleration, velocity / duration);     // vector
QUANTITY_SPEC(angular_velocity, angular_displacement / duration, quantity_character::vector);
QUANTITY_SPEC(angular_acceleration, angular_velocity / duration);
QUANTITY_SPEC(period_duration, duration);
inline constexpr auto period = period_duration;
QUANTITY_SPEC(time_constant, duration);
QUANTITY_SPEC(rotation, rotational_displacement);
QUANTITY_SPEC(frequency, 1 / period_duration);
QUANTITY_SPEC(rotational_frequency, rotation / duration);
QUANTITY_SPEC(angular_frequency, phase_angle / duration);
QUANTITY_SPEC(wavelength, length);
QUANTITY_SPEC(repetency, 1 / wavelength);
inline constexpr auto wavenumber = repetency;
QUANTITY_SPEC(wave_vector, repetency, quantity_character::vector);
QUANTITY_SPEC(angular_repetency, 1 / wavelength);
inline constexpr auto angular_wavenumber = angular_repetency;
QUANTITY_SPEC(phase_velocity, angular_frequency / angular_repetency);
inline constexpr auto phase_speed = phase_velocity;
QUANTITY_SPEC(group_velocity, angular_frequency / angular_repetency);
inline constexpr auto group_speed = group_velocity;
QUANTITY_SPEC(damping_coefficient, 1 / time_constant);
QUANTITY_SPEC(logarithmic_decrement, damping_coefficient* period_duration);
QUANTITY_SPEC(attenuation, 1 / distance);
inline constexpr auto extinction = attenuation;
QUANTITY_SPEC(phase_coefficient, phase_angle / path_length);
QUANTITY_SPEC(propagation_coefficient, 1 / length);  // γ = α + iβ where α denotes attenuation
                                                     // and β the phase coefficient of a plane wave

}  // namespace units::isq
