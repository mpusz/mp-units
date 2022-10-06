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

#include <units/isq/space_and_time.h>
#include <units/reference.h>
#include <units/si/units.h>

namespace units::si {

// clang-format off
inline constexpr struct length : system_reference<isq::length_dim, metre> {} length;
inline constexpr struct curvature : system_reference<isq::curvature_dim, 1 / metre> {} curvature;
inline constexpr struct area : system_reference<isq::area_dim, square<metre>> {} area;
inline constexpr struct volume : system_reference<isq::volume_dim, cubic<metre>> {} volume;
inline constexpr struct angular_measure : system_reference<isq::angular_measure_dim, radian> {} angular_measure;
inline constexpr struct angular_displacement : system_reference<isq::angular_displacement_dim, radian> {} angular_displacement;
inline constexpr struct phase_angle : system_reference<isq::phase_angle_dim, radian> {} phase_angle;
inline constexpr struct solid_angular_measure : system_reference<isq::solid_angular_measure_dim, steradian> {} solid_angular_measure;
inline constexpr struct time : system_reference<isq::time_dim, second> {} time;  // TODO called duration in ISO 80000
// TODO there is also a velocity in ISO 80000
inline constexpr struct speed : system_reference<isq::speed_dim, metre / second> {} speed;
inline constexpr struct acceleration : system_reference<isq::acceleration_dim, metre / square<second>> {} acceleration;
inline constexpr struct angular_velocity : system_reference<isq::angular_velocity_dim, radian / second> {} angular_velocity;
inline constexpr struct angular_acceleration : system_reference<isq::angular_acceleration_dim, radian / square<second>> {} angular_acceleration;
inline constexpr struct period_duration : system_reference<isq::period_duration_dim, second> {} period_duration;
inline constexpr struct time_constant : system_reference<isq::time_constant_dim, second> {} time_constant;
inline constexpr struct rotation : system_reference<isq::rotation_dim, one> {} rotation;
inline constexpr struct frequency : system_reference<isq::frequency_dim, hertz> {} frequency;
inline constexpr struct rotational_frequency : system_reference<isq::rotational_frequency_dim, hertz> {} rotational_frequency;
inline constexpr struct angular_frequency : system_reference<isq::angular_frequency_dim, radian / second> {} angular_frequency;
inline constexpr struct wavelength : system_reference<isq::wavelength_dim, metre> {} wavelength;
inline constexpr struct repetency : system_reference<isq::repetency_dim, 1 / metre> {} repetency;
inline constexpr struct wave_vector : system_reference<isq::wave_vector_dim, 1 / metre> {} wave_vector;
inline constexpr struct angular_repetency : system_reference<isq::angular_repetency_dim, 1 / metre> {} angular_repetency;
inline constexpr struct phase_velocity : system_reference<isq::phase_velocity_dim, metre / second> {} phase_velocity;
inline constexpr struct damping_coefficient : system_reference<isq::damping_coefficient_dim, 1 / second> {} damping_coefficient;
inline constexpr struct logarithmic_decrement : system_reference<isq::logarithmic_decrement_dim, one> {} logarithmic_decrement;
inline constexpr struct attenuation : system_reference<isq::attenuation_dim, 1 / metre> {} attenuation;
inline constexpr struct phase_coefficient : system_reference<isq::phase_coefficient_dim, radian / metre> {} phase_coefficient;
inline constexpr struct propagation_coefficient : system_reference<isq::propagation_coefficient_dim, 1 / metre> {} propagation_coefficient;
// clang-format on

}  // namespace units::si
