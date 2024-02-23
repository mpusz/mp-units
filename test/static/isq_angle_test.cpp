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

#include <mp-units/systems/isq_angle/isq_angle.h>
#include <mp-units/systems/si/unit_symbols.h>

namespace {

using namespace mp_units;
using namespace mp_units::si::unit_symbols;
using namespace mp_units::angular::unit_symbols;
using mp_units::angular::unit_symbols::rad;
using mp_units::angular::unit_symbols::sr;
using enum mp_units::quantity_character;

[[nodiscard]] consteval bool verify(QuantitySpec auto q, quantity_character ch, Unit auto... units)
{
  return q.character == ch && (... && requires { q[units]; });
}

static_assert(verify(isq_angle::cotes_angle_constant, scalar, rad));
static_assert(verify(isq_angle::angular_measure, scalar, rad));
static_assert(verify(isq_angle::rotational_displacement, scalar, rad));
static_assert(verify(isq_angle::angular_displacement, scalar, rad));
static_assert(verify(isq_angle::solid_angular_measure, scalar, sr));
static_assert(verify(isq_angle::angular_velocity, vector, rad / s));
static_assert(verify(isq_angle::angular_acceleration, vector, rad / s2));
static_assert(verify(isq_angle::rotation, scalar, rad));
static_assert(verify(isq_angle::angular_frequency, scalar, rad / s));
static_assert(verify(isq_angle::angular_repetency, scalar, rad / m));
static_assert(verify(isq_angle::angular_wavenumber, scalar, rad / m));
static_assert(verify(isq_angle::phase_coefficient, scalar, rad / m));
static_assert(verify(isq_angle::propagation_coefficient, scalar, rad / m));
static_assert(verify(isq_angle::moment_of_inertia, tensor, kg* m2 / rad2));
static_assert(verify(isq_angle::angular_momentum, vector, J* s / rad));
static_assert(verify(isq_angle::moment_of_force, vector, J / rad));
static_assert(verify(isq_angle::torque, scalar, J / rad));
static_assert(verify(isq_angle::angular_impulse, vector, J* s / rad));
static_assert(verify(isq_angle::loss_angle, scalar, rad));

}  // namespace
