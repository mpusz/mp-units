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

#include <units/dimension.h>
#include <units/isq/base_dimensions.h>

namespace units::isq {

// clang-format off
// inline constexpr struct length_dim : base_dimension<"L"> {} length_dim;
inline constexpr struct curvature_dim : decltype(1 / length_dim) {} curvature_dim;
inline constexpr struct area_dim : decltype(length_dim * length_dim) {} area_dim;
inline constexpr struct volume_dim : decltype(length_dim * length_dim * length_dim) {} volume_dim;
inline constexpr struct angular_measure_dim : decltype(length_dim / length_dim) {} angular_measure_dim;
inline constexpr struct angular_displacement_dim : decltype(length_dim / length_dim) {} angular_displacement_dim;
inline constexpr struct phase_angle_dim : decltype(length_dim / length_dim) {} phase_angle_dim;
inline constexpr struct solid_angular_measure_dim : decltype(area_dim / (length_dim * length_dim)) {} solid_angular_measure_dim;
// inline constexpr struct time_dim : base_dimension<"T"> {} time_dim;  // TODO called duration in ISO 80000
// TODO there is also a velocity in ISO 80000
inline constexpr struct speed_dim : decltype(length_dim / time_dim) {} speed_dim;
inline constexpr struct acceleration_dim : decltype(speed_dim / time_dim) {} acceleration_dim;
inline constexpr struct angular_velocity_dim : decltype(angular_displacement_dim / time_dim) {} angular_velocity_dim;
inline constexpr struct angular_acceleration_dim : decltype(angular_velocity_dim / time_dim) {} angular_acceleration_dim;
inline constexpr struct period_duration_dim : time_dim {} period_duration_dim;
inline constexpr struct time_constant_dim : time_dim {} time_constant_dim;
inline constexpr struct rotation_dim : angular_displacement_dim {} rotation_dim;
inline constexpr struct frequency_dim : decltype(1 / time_dim) {} frequency_dim;
inline constexpr struct rotational_frequency_dim : decltype(rotation_dim / time_dim) {} rotational_frequency_dim;
inline constexpr struct angular_frequency_dim : decltype(angular_measure_dim / time_dim) {} angular_frequency_dim;
inline constexpr struct wavelength_dim : length_dim {} wavelength_dim;
inline constexpr struct repetency_dim : decltype(1 / wavelength_dim) {} repetency_dim;
inline constexpr struct wave_vector_dim : decltype(1 / length_dim) {} wave_vector_dim;
inline constexpr struct angular_repetency_dim : decltype(1 / wavelength_dim) {} angular_repetency_dim;
inline constexpr struct phase_velocity_dim : decltype(angular_frequency_dim / angular_repetency_dim) {} phase_velocity_dim;
inline constexpr struct damping_coefficient_dim : decltype(1 / time_constant_dim) {} damping_coefficient_dim;
inline constexpr struct logarithmic_decrement_dim : decltype(damping_coefficient_dim * period_duration_dim) {} logarithmic_decrement_dim;
inline constexpr struct attenuation_dim : decltype(1 / length_dim) {} attenuation_dim;
inline constexpr struct phase_coefficient_dim : decltype(phase_angle_dim / length_dim) {} phase_coefficient_dim;
inline constexpr struct propagation_coefficient_dim : decltype(1 / length_dim) {} propagation_coefficient_dim;
// clang-format on

}  // namespace units::isq


// inline constexpr struct force_dim : decltype(mass_dim * acceleration_dim) {} force_dim;
// inline constexpr struct energy_dim : decltype(force_dim * length_dim) {} energy_dim;
// inline constexpr struct power_dim : decltype(force_dim * speed_dim) {} power_dim;
