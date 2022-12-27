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

#include <units/angular/angular.h>
#include <units/isq/isq.h>
#include <units/quantity_spec.h>

namespace units::isq_angle {

using namespace isq;

// ISQ angular derived quantities redefinition
QUANTITY_SPEC(cotes_angle, angular::angle);  // 1 rad
QUANTITY_SPEC(arc_length, length*(angular::angle / cotes_angle));
QUANTITY_SPEC(angular_measure, cotes_angle* arc_length / radius);
QUANTITY_SPEC(rotational_displacement, cotes_angle* path_length / radius);
inline constexpr auto angular_displacement = rotational_displacement;
QUANTITY_SPEC(phase_angle, angular_measure);
QUANTITY_SPEC(solid_angular_measure, pow<2>(cotes_angle) * area / pow<2>(radius));
QUANTITY_SPEC(angular_velocity, angular_displacement / duration, quantity_character::vector);
QUANTITY_SPEC(angular_acceleration, angular_velocity / duration);
QUANTITY_SPEC(rotation, rotational_displacement);
QUANTITY_SPEC(angular_frequency, phase_angle / duration);
QUANTITY_SPEC(angular_repetency, cotes_angle / wavelength);
inline constexpr auto angular_wavenumber = angular_repetency;
QUANTITY_SPEC(phase_coefficient, phase_angle / path_length);
QUANTITY_SPEC(propagation_coefficient, cotes_angle / length);
QUANTITY_SPEC(angular_momentum, position_vector* momentum / cotes_angle);  // vector
QUANTITY_SPEC(moment_of_inertia, angular_momentum / angular_velocity, quantity_character::tensor);
QUANTITY_SPEC(moment_of_force, position_vector* force / cotes_angle);  // vector
QUANTITY_SPEC(torque, moment_of_force, quantity_character::scalar);
QUANTITY_SPEC(angular_impulse, moment_of_force* time);  // vector
QUANTITY_SPEC(loss_angle, angular_measure);

}  // namespace units::isq_angle
