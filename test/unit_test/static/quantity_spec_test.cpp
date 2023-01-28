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

#include "test_tools.h"
#include <mp_units/quantity.h>
#include <mp_units/quantity_spec.h>
#include <mp_units/reference.h>
#include <mp_units/unit.h>

namespace {

using namespace mp_units;

using dimensionless_ = struct dimensionless;
using dim_one_ = struct dimension_one;

// clang-format off
inline constexpr struct dim_length_ : base_dimension<"L"> {} dim_length;
inline constexpr struct dim_mass_ : base_dimension<"M"> {} dim_mass;
inline constexpr struct dim_time_ : base_dimension<"T"> {} dim_time;

// quantities specification
QUANTITY_SPEC_(length, dim_length);
QUANTITY_SPEC_(mass, dim_mass);
QUANTITY_SPEC_(time, dim_time);

inline constexpr struct second_ : named_unit<"s", time> {} second;

QUANTITY_SPEC_(height, length);
QUANTITY_SPEC_(path_length, length);
QUANTITY_SPEC_(distance, path_length);
QUANTITY_SPEC_(position_vector, length, quantity_character::vector);
QUANTITY_SPEC_(period_duration, time);

QUANTITY_SPEC_(frequency, 1 / period_duration, kind_of<frequency_>());
QUANTITY_SPEC_(action, 1 / time, kind_of<action_>());
QUANTITY_SPEC_(area, pow<2>(length));
QUANTITY_SPEC_(volume, pow<3>(length));
QUANTITY_SPEC_(velocity, position_vector / time);
QUANTITY_SPEC_(speed, distance / time);
QUANTITY_SPEC_(acceleration, velocity / time);
QUANTITY_SPEC_(force, mass * acceleration);
QUANTITY_SPEC_(moment_of_force, position_vector* force);
QUANTITY_SPEC_(torque, moment_of_force, quantity_character::scalar);
QUANTITY_SPEC_(pressure, force / area, quantity_character::scalar);
QUANTITY_SPEC_(stress, pressure, quantity_character::tensor);
QUANTITY_SPEC_(strain, dimensionless, quantity_character::tensor);
QUANTITY_SPEC_(power, force* velocity, quantity_character::scalar);
QUANTITY_SPEC_(efficiency, power / power);
QUANTITY_SPEC_(energy, force * length, kind_of<energy_>());
QUANTITY_SPEC_(potential_energy, mass* acceleration* height, kind_of<energy>());
QUANTITY_SPEC_(kinetic_energy, mass* pow<2>(speed), kind_of<energy>());
// clang-format on

// concepts verification
static_assert(QuantitySpec<length_>);
static_assert(BaseQuantitySpec<length_>);
static_assert(NamedQuantitySpec<length_>);
static_assert(!DerivedQuantitySpec<length_>);

static_assert(QuantitySpec<frequency_>);
static_assert(!BaseQuantitySpec<frequency_>);
static_assert(NamedQuantitySpec<frequency_>);
static_assert(!DerivedQuantitySpec<frequency_>);

static_assert(QuantitySpec<decltype(1 / time)>);
static_assert(!BaseQuantitySpec<decltype(1 / time)>);
static_assert(!NamedQuantitySpec<decltype(1 / time)>);
static_assert(DerivedQuantitySpec<decltype(1 / time)>);

static_assert(QuantitySpec<dimensionless_>);
static_assert(!BaseQuantitySpec<dimensionless_>);
static_assert(NamedQuantitySpec<dimensionless_>);
static_assert(!DerivedQuantitySpec<dimensionless_>);

// dimensionless
static_assert(QuantitySpec<decltype(length / length)>);
static_assert(!BaseQuantitySpec<decltype(length / length)>);
static_assert(NamedQuantitySpec<decltype(length / length)>);
static_assert(!DerivedQuantitySpec<decltype(length / length)>);

// length
static_assert(QuantitySpec<decltype(speed * time)>);
static_assert(!BaseQuantitySpec<decltype(speed * time)>);
static_assert(!NamedQuantitySpec<decltype(speed * time)>);
static_assert(DerivedQuantitySpec<decltype(speed * time)>);

// derived QuantitySpec expression template syntax verification
static_assert(is_of_type<1 / time, derived_quantity_spec<dimensionless_, per<time_>>>);
static_assert(is_of_type<1 / (1 / time), time_>);

static_assert(is_of_type<dimensionless * time, time_>);
static_assert(is_of_type<time * dimensionless, time_>);
static_assert(is_of_type<dimensionless * (1 / time), derived_quantity_spec<dimensionless_, per<time_>>>);
static_assert(is_of_type<1 / time * dimensionless, derived_quantity_spec<dimensionless_, per<time_>>>);

static_assert(is_of_type<length * time, derived_quantity_spec<length_, time_>>);
static_assert(is_of_type<length * length, derived_quantity_spec<mp_units::power<length_, 2>>>);

static_assert(is_of_type<length * length * time, derived_quantity_spec<mp_units::power<length_, 2>, time_>>);
static_assert(is_of_type<length * time * length, derived_quantity_spec<mp_units::power<length_, 2>, time_>>);

static_assert(is_of_type<length*(time* length), derived_quantity_spec<mp_units::power<length_, 2>, time_>>);
static_assert(is_of_type<time*(length* length), derived_quantity_spec<mp_units::power<length_, 2>, time_>>);

static_assert(is_of_type<1 / time * length, derived_quantity_spec<length_, per<time_>>>);
static_assert(is_of_type<1 / time * time, dimensionless_>);

static_assert(is_of_type<time / dimensionless, time_>);
static_assert(is_of_type<1 / time / dimensionless, derived_quantity_spec<dimensionless_, per<time_>>>);

static_assert(is_of_type<length / time * time, length_>);
static_assert(is_of_type<1 / time * (1 / time), derived_quantity_spec<dimensionless_, per<mp_units::power<time_, 2>>>>);
static_assert(is_of_type<1 / (time * time), derived_quantity_spec<dimensionless_, per<mp_units::power<time_, 2>>>>);
static_assert(is_of_type<1 / (1 / (time * time)), derived_quantity_spec<mp_units::power<time_, 2>>>);

static_assert(is_of_type<length / time * (1 / time), derived_quantity_spec<length_, per<mp_units::power<time_, 2>>>>);
static_assert(is_of_type<length / time*(length / time),
                         derived_quantity_spec<mp_units::power<length_, 2>, per<mp_units::power<time_, 2>>>>);
static_assert(is_of_type<length / time*(time / length), dimensionless_>);

static_assert(is_of_type<speed / acceleration, derived_quantity_spec<speed_, per<acceleration_>>>);
static_assert(is_of_type<(speed / acceleration).dimension, dim_time_>);
static_assert(is_of_type<acceleration / speed, derived_quantity_spec<acceleration_, per<speed_>>>);
static_assert(is_of_type<(acceleration / speed).dimension, derived_dimension<dim_one_, per<dim_time_>>>);
static_assert(is_of_type<speed * speed / length, derived_quantity_spec<mp_units::power<speed_, 2>, per<length_>>>);
static_assert(
  is_of_type<(speed * speed / length).dimension, derived_dimension<dim_length_, per<mp_units::power<dim_time_, 2>>>>);
static_assert(
  is_of_type<1 / (speed * speed) * length, derived_quantity_spec<length_, per<mp_units::power<speed_, 2>>>>);
static_assert(is_of_type<(1 / (speed * speed) * length).dimension,
                         derived_dimension<mp_units::power<dim_time_, 2>, per<dim_length_>>>);

static_assert(is_of_type<(length * length) * (time * time),
                         derived_quantity_spec<mp_units::power<length_, 2>, mp_units::power<time_, 2>>>);
static_assert(is_of_type<(time * time) * (length * length),
                         derived_quantity_spec<mp_units::power<length_, 2>, mp_units::power<time_, 2>>>);

static_assert(is_of_type<length * time * time, derived_quantity_spec<length_, mp_units::power<time_, 2>>>);
static_assert(
  is_of_type<mass / length / time / time, derived_quantity_spec<mass_, per<length_, mp_units::power<time_, 2>>>>);
static_assert(
  is_of_type<mass / (length * time * time), derived_quantity_spec<mass_, per<length_, mp_units::power<time_, 2>>>>);
static_assert(
  is_of_type<mass / length / (time * time), derived_quantity_spec<mass_, per<length_, mp_units::power<time_, 2>>>>);

static_assert(is_of_type<force / area, derived_quantity_spec<force_, per<area_>>>);
static_assert(
  is_of_type<(force / area).dimension, derived_dimension<dim_mass_, per<dim_length_, mp_units::power<dim_time_, 2>>>>);

template<auto& t>
concept invalid_operations = requires {
  requires !requires { t < t; };
  requires !requires { t / 2; };
  requires !requires { 2 * t; };
  requires !requires { t * 2; };
  requires !requires { t + 2; };
  requires !requires { 2 + t; };
  requires !requires { t + t; };
  requires !requires { t - 2; };
  requires !requires { 2 - t; };
  requires !requires { t - t; };
  requires !requires { t == 2; };
  requires !requires { 2 == t; };
  requires !requires { t < 2; };
  requires !requires { 2 < t; };
  requires !requires { t + time[second]; };
  requires !requires { t - time[second]; };
  requires !requires { t* time[second]; };
  requires !requires { t / time[second]; };
  requires !requires { t == time[second]; };
  requires !requires { t < time[second]; };
  requires !requires { time[second] + t; };
  requires !requires { time[second] - t; };
  requires !requires { time[second] * t; };
  requires !requires { time[second] / t; };
  requires !requires { time[second] == t; };
  requires !requires { time[second] < t; };
  requires !requires { t + 1 * time[second]; };
  requires !requires { t - 1 * time[second]; };
  requires !requires { t * 1 * time[second]; };
  requires !requires { t / 1 * time[second]; };
  requires !requires { t == 1 * time[second]; };
  requires !requires { t == 1 * time[second]; };
  requires !requires { 1 * time[second] + t; };
  requires !requires { 1 * time[second] - t; };
  requires !requires { 1 * time[second] * t; };
  requires !requires { 1 * time[second] == t; };
  requires !requires { 1 * time[second] < t; };
};
static_assert(invalid_operations<time>);

// get_kind
static_assert(get_kind(length) == length);
static_assert(get_kind(distance) == length);
static_assert(get_kind(time) == time);
static_assert(get_kind(period_duration) == time);
static_assert(get_kind(length / time) == length / time);
static_assert(get_kind(speed) == length / time);
static_assert(get_kind(height / time) == length / time);
static_assert(get_kind(1 / time) == 1 / time);
static_assert(get_kind(1 / period_duration) == 1 / time);
static_assert(get_kind(frequency) == frequency);
static_assert(get_kind(mass * frequency) == mass * frequency);
static_assert(get_kind(moment_of_force) == mass * pow<2>(length) / pow<2>(time));
static_assert(get_kind(energy) == energy);
static_assert(get_kind(potential_energy) == energy);
static_assert(get_kind(kinetic_energy) == energy);

// comparisons of the same dimensions
static_assert(length == length);
static_assert(speed == speed);

// comparisons of equivalent dimensions (named vs unnamed/derived)
static_assert(length / length == dimensionless);

static_assert(1 / time != frequency);
static_assert(interconvertible(1 / time, frequency));
static_assert(1 / frequency != time);
static_assert(interconvertible(1 / frequency, time));
static_assert(frequency * time != dimensionless);
static_assert(interconvertible(frequency * time, dimensionless));
static_assert(is_of_type<common_quantity_spec(1 / time, frequency), frequency_>);
static_assert(is_of_type<common_quantity_spec(frequency, 1 / time), frequency_>);

static_assert(length * length != area);
static_assert(interconvertible(length * length, area));
static_assert(length * length != volume);
static_assert(!interconvertible(length * length, volume));
static_assert(area / length != length);
static_assert(interconvertible(area / length, length));
static_assert(is_of_type<common_quantity_spec(length* length, area), area_>);
static_assert(is_of_type<common_quantity_spec(area, length* length), area_>);

static_assert(length * length * length != volume);
static_assert(interconvertible(length * length * length, volume));
static_assert(area * length != volume);
static_assert(interconvertible(area * length, volume));
static_assert(volume / length != area);
static_assert(interconvertible(volume / length, area));
static_assert(volume / length / length != length);
static_assert(interconvertible(volume / length / length, length));
static_assert(area * area / length != volume);
static_assert(interconvertible(area * area / length, volume));
static_assert(area * (area / length) != volume);
static_assert(interconvertible(area * (area / length), volume));
static_assert(volume / (length * length) != length);
static_assert(interconvertible(volume / (length * length), length));

// TODO Can we improve the below so the `position_vector / time` is convertible only to `velocity` but not `speed`?
static_assert(length / time != speed);
static_assert(interconvertible(length / time, speed));
static_assert(position_vector / time != speed);
static_assert(interconvertible(position_vector / time, speed));
static_assert(length / time != velocity);
static_assert(interconvertible(length / time, velocity));
static_assert(position_vector / time != velocity);
static_assert(interconvertible(position_vector / time, velocity));

static_assert(length * time != speed);
static_assert(!interconvertible(length * time, speed));
static_assert(length / time / time != speed);
static_assert(!interconvertible(length / time / time, speed));
static_assert(length / speed != time);
static_assert(interconvertible(length / speed, time));
static_assert(speed * time != length);
static_assert(interconvertible(speed * time, length));
static_assert(is_of_type<common_quantity_spec(length / time, speed), speed_>);
static_assert(is_of_type<common_quantity_spec(speed, length / time), speed_>);
static_assert(is_of_type<common_quantity_spec(length / time, length / time), decltype(length / time)>);
static_assert(is_of_type<common_quantity_spec(length / time, 1 / (time / length)), decltype(length / time)>);

static_assert(length / time / time != acceleration);
static_assert(interconvertible(length / time / time, acceleration));
static_assert(position_vector / time / time != acceleration);
static_assert(interconvertible(position_vector / time / time, acceleration));
static_assert(position_vector / (time * time) != acceleration);
static_assert(interconvertible(position_vector / (time * time), acceleration));
static_assert(velocity / time != acceleration);
static_assert(interconvertible(velocity / time, acceleration));
static_assert(velocity / acceleration != time);
static_assert(interconvertible(velocity / acceleration, time));  // TODO should the vector character cancel here?
static_assert(acceleration * time != velocity);
static_assert(interconvertible(acceleration * time, velocity));
static_assert(acceleration * (time * time) != position_vector);
static_assert(interconvertible(acceleration * (time * time), position_vector));
static_assert(acceleration / speed != frequency);
static_assert(interconvertible(acceleration / velocity, frequency));  // TODO should the vector character cancel here?

// comparison of convertible named dimensions
static_assert(velocity != speed);
static_assert(!interconvertible(speed, velocity));

// comparison of convertible unnamed dimensions
static_assert(is_of_type<mass * acceleration, derived_quantity_spec<acceleration_, mass_>>);
static_assert(is_of_type<(mass * acceleration).dimension,
                         derived_dimension<dim_length_, dim_mass_, per<mp_units::power<dim_time_, 2>>>>);
static_assert(is_of_type<acceleration * mass, derived_quantity_spec<acceleration_, mass_>>);
static_assert(is_of_type<(acceleration * mass).dimension,
                         derived_dimension<dim_length_, dim_mass_, per<mp_units::power<dim_time_, 2>>>>);
static_assert(mass * acceleration == acceleration * mass);
static_assert(interconvertible(mass * acceleration, acceleration* mass));

// comparisons of equivalent but not convertible dimensions
static_assert(energy != torque);
static_assert(!interconvertible(energy, torque));

static_assert(force * length != energy);
static_assert(force * length != torque);
static_assert(force * position_vector != energy);
static_assert(force * position_vector != torque);
static_assert(interconvertible(force * length, energy));
static_assert(interconvertible(force * position_vector, energy));
static_assert(interconvertible(force * length, torque));
static_assert(interconvertible(force * position_vector, torque));

template<auto T1, auto T2>
concept no_common_type = requires {
  requires !requires { typename std::common_type_t<decltype(T1), decltype(T2)>; };
  requires !requires { typename std::common_type_t<decltype(T2), decltype(T1)>; };
};
static_assert(no_common_type<energy, torque>);

static_assert(frequency != action);
static_assert(!interconvertible(frequency, action));
static_assert(no_common_type<frequency, action>);

// dimensionless
static_assert(power / power != efficiency);
static_assert(interconvertible(power / power, efficiency));
static_assert(dimensionless != efficiency);

static_assert(efficiency != strain);
static_assert(!interconvertible(efficiency, strain));

static_assert(stress / stress != strain);
static_assert(stress / stress != efficiency);
static_assert(interconvertible(stress / stress, strain));
static_assert(interconvertible(stress / stress, efficiency));

// comparison of not equivalent dimensions
static_assert(length != time);
static_assert(!interconvertible(length, time));

static_assert(acceleration != speed);
static_assert(!interconvertible(acceleration, speed));

// power
static_assert(is_of_type<pow<2>(length), derived_quantity_spec<mp_units::power<length_, 2>>>);
static_assert(is_of_type<pow<1, 2>(length), derived_quantity_spec<mp_units::power<length_, 1, 2>>>);
static_assert(is_of_type<pow<1, 2>(length* length), length_>);
static_assert(is_of_type<pow<1, 3>(length* length* length), length_>);
static_assert(is_of_type<pow<1, 3>(length* length), derived_quantity_spec<mp_units::power<length_, 2, 3>>>);
static_assert(is_of_type<pow<1, 2>(length / time),
                         derived_quantity_spec<mp_units::power<length_, 1, 2>, per<mp_units::power<time_, 1, 2>>>>);
static_assert(
  is_of_type<pow<1, 2>(length / (time * time)), derived_quantity_spec<mp_units::power<length_, 1, 2>, per<time_>>>);

static_assert(is_same_v<decltype(pow<2>(length)), decltype(length * length)>);
static_assert(is_same_v<decltype(pow<2>(length / time)), decltype(length * length / time / time)>);

// derived kinds
QUANTITY_SPEC(rate_of_climb, height / time);
static_assert(rate_of_climb != speed);
static_assert(!interconvertible(rate_of_climb, speed));
static_assert(rate_of_climb != distance / time);
static_assert(interconvertible(rate_of_climb, distance / time));  // !!!
static_assert(rate_of_climb != length / time);
static_assert(interconvertible(rate_of_climb, length / time));  // !!!

}  // namespace
