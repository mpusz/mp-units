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

// inline constexpr struct length : base_dimension<"L"> {} length;
DERIVED_DIMENSION(curvature, decltype(1 / length));
DERIVED_DIMENSION(area, decltype(length * length));
DERIVED_DIMENSION(volume, decltype(length * length * length));
DERIVED_DIMENSION(angular_measure, decltype(length / length));
DERIVED_DIMENSION(angular_displacement, decltype(length / length));
DERIVED_DIMENSION(phase_angle, decltype(length / length));
inline constexpr struct solid_angular_measure : decltype(area / (length * length)) {
} solid_angular_measure;
// inline constexpr struct time : base_dimension<"T"> {} time;  // TODO called duration in ISO 80000
// TODO there is also a velocity in ISO 80000
DERIVED_DIMENSION(speed, decltype(length / time));
DERIVED_DIMENSION(acceleration, decltype(speed / time));
DERIVED_DIMENSION(angular_velocity, decltype(angular_displacement / time));
DERIVED_DIMENSION(angular_acceleration, decltype(angular_velocity / time));
inline constexpr struct period_duration : time {
} period_duration;
inline constexpr struct time_constant : time {
} time_constant;
inline constexpr struct rotation : angular_displacement {
} rotation;
DERIVED_DIMENSION(frequency, decltype(1 / time));
DERIVED_DIMENSION(rotational_frequency, decltype(rotation / time));
DERIVED_DIMENSION(angular_frequency, decltype(angular_measure / time));
inline constexpr struct wavelength : length {
} wavelength;
DERIVED_DIMENSION(repetency, decltype(1 / wavelength));
DERIVED_DIMENSION(wave_vector, decltype(1 / length));
DERIVED_DIMENSION(angular_repetency, decltype(1 / wavelength));
DERIVED_DIMENSION(phase_velocity, decltype(angular_frequency / angular_repetency));
DERIVED_DIMENSION(damping_coefficient, decltype(1 / time_constant));
DERIVED_DIMENSION(logarithmic_decrement, decltype(damping_coefficient * period_duration));
DERIVED_DIMENSION(attenuation, decltype(1 / length));
DERIVED_DIMENSION(phase_coefficient, decltype(phase_angle / length));
DERIVED_DIMENSION(propagation_coefficient, decltype(1 / length));

}  // namespace units::isq
