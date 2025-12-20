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
#include <mp-units/ext/type_traits.h>
#include <mp-units/framework.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <type_traits>
#endif

namespace {

using namespace mp_units;

using dimensionless_ = struct dimensionless;
using dim_one_ = struct dimension_one;

// clang-format off
inline constexpr struct dim_length_ final : base_dimension<"L"> {} dim_length;
inline constexpr struct dim_mass_ final : base_dimension<"M"> {} dim_mass;
inline constexpr struct dim_time_ final : base_dimension<"T"> {} dim_time;
inline constexpr struct dim_electric_current_ final : base_dimension<"I"> {} dim_electric_current;

// quantities specification
QUANTITY_SPEC_(length, dim_length);
QUANTITY_SPEC_(mass, dim_mass);
QUANTITY_SPEC_(time, dim_time);

inline constexpr struct second_ final : named_unit<"s", kind_of<time>> {} second;

QUANTITY_SPEC_(horizontal_length, length);
QUANTITY_SPEC_(height, length);
QUANTITY_SPEC_(width, length);
QUANTITY_SPEC_(radius, width);
QUANTITY_SPEC_(path_length, length);
inline constexpr auto arc_length = path_length;
QUANTITY_SPEC_(distance, path_length);
QUANTITY_SPEC_(wavelength, length);
QUANTITY_SPEC_(position_vector, length, quantity_character::vector);
QUANTITY_SPEC_(displacement, length, quantity_character::vector);
QUANTITY_SPEC_(period_duration, time);
QUANTITY_SPEC_(rotation, dimensionless);
QUANTITY_SPEC_(repetency, inverse(wavelength));
QUANTITY_SPEC_(frequency, inverse(period_duration));
QUANTITY_SPEC_(activity, inverse(time));
QUANTITY_SPEC_(area, pow<2>(length));
QUANTITY_SPEC_(volume, pow<3>(length));
QUANTITY_SPEC_(angular_measure, dimensionless, arc_length / radius, is_kind);
QUANTITY_SPEC_(special_angular_measure, angular_measure);
QUANTITY_SPEC_(rotational_displacement, angular_measure, path_length / radius);
QUANTITY_SPEC_(phase_angle, angular_measure);
QUANTITY_SPEC_(solid_angular_measure, dimensionless, area / pow<2>(radius), is_kind);
QUANTITY_SPEC_(dimensionless_rate, dimensionless / time);
QUANTITY_SPEC_(angular_measure_rate, angular_measure / time);
QUANTITY_SPEC_(solid_angular_measure_rate, solid_angular_measure / time);
QUANTITY_SPEC_(speed, length / time);
QUANTITY_SPEC_(velocity, speed, displacement / time);
QUANTITY_SPEC_(special_speed, speed);
QUANTITY_SPEC_(horizontal_speed, speed, horizontal_length / time);
QUANTITY_SPEC_(rate_of_climb, speed, height / time);
QUANTITY_SPEC_(special_rate_of_climb, rate_of_climb);
QUANTITY_SPEC_(acceleration, velocity / time);
QUANTITY_SPEC_(acceleration_of_free_fall, acceleration);
QUANTITY_SPEC_(mass_density, mass / volume);
QUANTITY_SPEC_(force, mass * acceleration);
QUANTITY_SPEC_(weight, force, mass * acceleration_of_free_fall);
QUANTITY_SPEC_(moment_of_force, position_vector* force);
QUANTITY_SPEC_(torque, moment_of_force, quantity_character::real_scalar);
QUANTITY_SPEC_(pressure, force / area, quantity_character::real_scalar);
QUANTITY_SPEC_(stress, pressure, quantity_character::tensor);
QUANTITY_SPEC_(strain, dimensionless, quantity_character::tensor);
QUANTITY_SPEC_(power, force* velocity, quantity_character::real_scalar);
QUANTITY_SPEC_(efficiency, power / power);
QUANTITY_SPEC_(energy, mass * pow<2>(length) / pow<2>(time));
QUANTITY_SPEC_(mechanical_energy, energy);
QUANTITY_SPEC_(potential_energy, mechanical_energy);
QUANTITY_SPEC_(gravitational_potential_energy, potential_energy, mass * acceleration_of_free_fall * height);
QUANTITY_SPEC_(kinetic_energy, mechanical_energy, mass* pow<2>(speed));
QUANTITY_SPEC_(electric_current, dim_electric_current);
QUANTITY_SPEC_(electric_charge, electric_current* time);
QUANTITY_SPEC_(electric_field_strength, force / electric_charge);  // vector
QUANTITY_SPEC_(electric_potential, electric_field_strength* length, quantity_character::real_scalar);
QUANTITY_SPEC_(voltage, electric_potential);
QUANTITY_SPEC_(electromagnetism_power, power, voltage* electric_current);
QUANTITY_SPEC_(electric_current_phasor, electric_current, quantity_character::complex_scalar);
QUANTITY_SPEC_(voltage_phasor, voltage, quantity_character::complex_scalar);
QUANTITY_SPEC_(active_power, power, inverse(period_duration) * (electromagnetism_power * time));
QUANTITY_SPEC_(complex_power, voltage_phasor* electric_current_phasor);  // separate kind
QUANTITY_SPEC_(apparent_power, complex_power, quantity_character::real_scalar);

// clang-format on

// concepts verification
static_assert(QuantitySpec<length_>);
static_assert(detail::NamedQuantitySpec<length_>);
static_assert(!detail::DerivedQuantitySpec<length_>);
static_assert(!detail::QuantityKindSpec<length_>);

static_assert(QuantitySpec<frequency_>);
static_assert(detail::NamedQuantitySpec<frequency_>);
static_assert(!detail::DerivedQuantitySpec<frequency_>);
static_assert(!detail::QuantityKindSpec<frequency_>);

static_assert(QuantitySpec<decltype(inverse(time))>);
static_assert(!detail::NamedQuantitySpec<decltype(inverse(time))>);
static_assert(detail::DerivedQuantitySpec<decltype(inverse(time))>);
static_assert(!detail::QuantityKindSpec<decltype(inverse(time))>);

static_assert(QuantitySpec<kind_of_<length_>>);
static_assert(!detail::NamedQuantitySpec<kind_of_<length_>>);
static_assert(!detail::DerivedQuantitySpec<kind_of_<length_>>);
static_assert(detail::QuantityKindSpec<kind_of_<length_>>);

static_assert(QuantitySpec<frequency_>);
static_assert(detail::NamedQuantitySpec<frequency_>);
static_assert(!detail::DerivedQuantitySpec<frequency_>);
static_assert(!detail::QuantityKindSpec<frequency_>);

static_assert(QuantitySpec<decltype(inverse(time))>);
static_assert(!detail::NamedQuantitySpec<decltype(inverse(time))>);
static_assert(detail::DerivedQuantitySpec<decltype(inverse(time))>);
static_assert(!detail::QuantityKindSpec<decltype(inverse(time))>);

static_assert(QuantitySpec<decltype(length / time)>);
static_assert(!detail::NamedQuantitySpec<decltype(length / time)>);
static_assert(detail::DerivedQuantitySpec<decltype(length / time)>);
static_assert(!detail::QuantityKindSpec<decltype(length / time)>);

static_assert(QuantitySpec<kind_of_<decltype(length / time)>>);
static_assert(!detail::NamedQuantitySpec<kind_of_<decltype(length / time)>>);
static_assert(detail::DerivedQuantitySpec<kind_of_<decltype(length / time)>>);
static_assert(detail::QuantityKindSpec<kind_of_<decltype(length / time)>>);

static_assert(QuantitySpec<decltype(kind_of<length> / kind_of<time>)>);
static_assert(!detail::NamedQuantitySpec<decltype(kind_of<length> / kind_of<time>)>);
static_assert(detail::DerivedQuantitySpec<decltype(kind_of<length> / kind_of<time>)>);
static_assert(detail::QuantityKindSpec<decltype(kind_of<length> / kind_of<time>)>);

static_assert(QuantitySpec<decltype(kind_of<length> * kind_of<time>)>);
static_assert(!detail::NamedQuantitySpec<decltype(kind_of<length> * kind_of<time>)>);
static_assert(detail::DerivedQuantitySpec<decltype(kind_of<length> * kind_of<time>)>);
static_assert(detail::QuantityKindSpec<decltype(kind_of<length> * kind_of<time>)>);

static_assert(QuantitySpec<decltype(kind_of<length> * time)>);
static_assert(!detail::NamedQuantitySpec<decltype(kind_of<length> * time)>);
static_assert(detail::DerivedQuantitySpec<decltype(kind_of<length> * time)>);
static_assert(!detail::QuantityKindSpec<decltype(kind_of<length> * time)>);

static_assert(QuantitySpec<decltype(length * kind_of<time>)>);
static_assert(!detail::NamedQuantitySpec<decltype(length * kind_of<time>)>);
static_assert(detail::DerivedQuantitySpec<decltype(length * kind_of<time>)>);
static_assert(!detail::QuantityKindSpec<decltype(length * kind_of<time>)>);

// dimensionless
static_assert(QuantitySpec<dimensionless_>);
static_assert(detail::NamedQuantitySpec<dimensionless_>);
static_assert(!detail::DerivedQuantitySpec<dimensionless_>);
static_assert(!detail::QuantityKindSpec<dimensionless_>);

static_assert(QuantitySpec<decltype(length / length)>);
static_assert(detail::NamedQuantitySpec<decltype(length / length)>);
static_assert(!detail::DerivedQuantitySpec<decltype(length / length)>);
static_assert(!detail::QuantityKindSpec<decltype(length / length)>);

static_assert(QuantitySpec<decltype(width / length)>);
static_assert(!detail::NamedQuantitySpec<decltype(width / length)>);
static_assert(detail::DerivedQuantitySpec<decltype(width / length)>);
static_assert(!detail::QuantityKindSpec<decltype(width / length)>);

static_assert(QuantitySpec<decltype(kind_of<length> / kind_of<length>)>);
static_assert(!detail::NamedQuantitySpec<decltype(kind_of<length> / kind_of<length>)>);
static_assert(!detail::DerivedQuantitySpec<decltype(kind_of<length> / kind_of<length>)>);
static_assert(detail::QuantityKindSpec<decltype(kind_of<length> / kind_of<length>)>);

static_assert(QuantitySpec<decltype(kind_of<length> / length)>);
static_assert(detail::NamedQuantitySpec<decltype(kind_of<length> / length)>);
static_assert(!detail::DerivedQuantitySpec<decltype(kind_of<length> / length)>);
static_assert(!detail::QuantityKindSpec<decltype(kind_of<length> / length)>);

static_assert(QuantitySpec<decltype(length / kind_of<length>)>);
static_assert(detail::NamedQuantitySpec<decltype(length / kind_of<length>)>);
static_assert(!detail::DerivedQuantitySpec<decltype(length / kind_of<length>)>);
static_assert(!detail::QuantityKindSpec<decltype(length / kind_of<length>)>);

static_assert(QuantitySpec<decltype(width / kind_of<length>)>);
static_assert(!detail::NamedQuantitySpec<decltype(width / kind_of<length>)>);
static_assert(detail::DerivedQuantitySpec<decltype(width / kind_of<length>)>);
static_assert(!detail::QuantityKindSpec<decltype(width / kind_of<length>)>);

// length
static_assert(QuantitySpec<decltype(speed * time)>);
static_assert(!detail::NamedQuantitySpec<decltype(speed * time)>);
static_assert(detail::DerivedQuantitySpec<decltype(speed * time)>);

// derived QuantitySpec expression template syntax verification
static_assert(!detail::NamedQuantitySpec<decltype(speed * time)>);
static_assert(detail::DerivedQuantitySpec<decltype(speed * time)>);

static_assert(is_of_type<dimensionless * time, time_>);
static_assert(is_of_type<time * dimensionless, time_>);
static_assert(is_of_type<dimensionless*(inverse(time)), derived_quantity_spec<dimensionless_, per<time_>>>);
static_assert(is_of_type<inverse(time) * dimensionless, derived_quantity_spec<dimensionless_, per<time_>>>);

static_assert(is_of_type<length / length, dimensionless_>);
static_assert(is_of_type<pow<2>(length / length), dimensionless_>);
static_assert(is_of_type<pow<2>(length) / pow<2>(length), dimensionless_>);

static_assert(is_of_type<pow<2>(length) / length, length_>);
static_assert(is_of_type<length / pow<2>(length), derived_quantity_spec<dimensionless_, per<length_>>>);

static_assert(is_of_type<length / height, derived_quantity_spec<length_, per<height_>>>);

static_assert(is_of_type<length * time, derived_quantity_spec<length_, time_>>);
static_assert(is_of_type<length * length, derived_quantity_spec<mp_units::power<length_, 2>>>);

static_assert(is_of_type<length * length * time, derived_quantity_spec<mp_units::power<length_, 2>, time_>>);
static_assert(is_of_type<length * time * length, derived_quantity_spec<mp_units::power<length_, 2>, time_>>);

static_assert(is_of_type<length*(time* length), derived_quantity_spec<mp_units::power<length_, 2>, time_>>);
static_assert(is_of_type<time*(length* length), derived_quantity_spec<mp_units::power<length_, 2>, time_>>);

static_assert(is_of_type<inverse(time) * length, derived_quantity_spec<length_, per<time_>>>);
static_assert(is_of_type<length * inverse(time), derived_quantity_spec<length_, per<time_>>>);
static_assert(is_of_type<inverse(time) * time, dimensionless_>);

static_assert(is_of_type<inverse(length) / inverse(width), derived_quantity_spec<width_, per<length_>>>);
static_assert(is_of_type<dimensionless / (time / length), derived_quantity_spec<length_, per<time_>>>);

static_assert(is_of_type<time / dimensionless, time_>);
static_assert(is_of_type<inverse(time) / dimensionless, derived_quantity_spec<dimensionless_, per<time_>>>);

static_assert(is_of_type<length / time * time, length_>);
static_assert(
  is_of_type<inverse(time) * inverse(time), derived_quantity_spec<dimensionless_, per<mp_units::power<time_, 2>>>>);
static_assert(is_of_type<inverse(time* time), derived_quantity_spec<dimensionless_, per<mp_units::power<time_, 2>>>>);
static_assert(is_of_type<inverse(inverse(time* time)), derived_quantity_spec<mp_units::power<time_, 2>>>);

static_assert(
  is_of_type<length / time * inverse(time), derived_quantity_spec<length_, per<mp_units::power<time_, 2>>>>);
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
  is_of_type<inverse(speed* speed) * length, derived_quantity_spec<length_, per<mp_units::power<speed_, 2>>>>);
static_assert(is_of_type<(inverse(speed * speed) * length).dimension,
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

static_assert(is_of_type<mass * acceleration, derived_quantity_spec<acceleration_, mass_>>);
static_assert(is_of_type<(mass * acceleration).dimension,
                         derived_dimension<dim_length_, dim_mass_, per<mp_units::power<dim_time_, 2>>>>);
static_assert(is_of_type<acceleration * mass, derived_quantity_spec<acceleration_, mass_>>);
static_assert(is_of_type<(acceleration * mass).dimension,
                         derived_dimension<dim_length_, dim_mass_, per<mp_units::power<dim_time_, 2>>>>);

static_assert(is_of_type<kind_of<length> / kind_of<time>, kind_of_<derived_quantity_spec<length_, per<time_>>>>);
static_assert(is_of_type<kind_of<length / time>, kind_of_<derived_quantity_spec<length_, per<time_>>>>);

// power
static_assert(is_of_type<pow<0>(length), dimensionless_>);
static_assert(is_of_type<pow<1>(length), length_>);
static_assert(is_of_type<pow<2, 2>(length), length_>);
static_assert(is_of_type<pow<2>(dimensionless), dimensionless_>);
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

static_assert(dimensionless * dimensionless == dimensionless);
static_assert(is_of_type<dimensionless * dimensionless, dimensionless_>);


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
  requires !requires { t * time[second]; };
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

// comparisons
static_assert(mass * acceleration == acceleration * mass);
static_assert(length / length == width / width);
static_assert(length / length == time / time);
static_assert(length * time / time == length);

static_assert(length != time);
static_assert(length != width);
static_assert(acceleration != speed);

static_assert(frequency != activity);
static_assert(efficiency != strain);
static_assert(dimensionless != efficiency);
static_assert(power / power != efficiency);
static_assert(stress / stress != strain);
static_assert(stress / stress != efficiency);

static_assert(rate_of_climb != speed);
static_assert(rate_of_climb != distance / time);
static_assert(rate_of_climb != length / time);

static_assert(velocity != speed);
static_assert(energy != torque);

static_assert(inverse(time) != frequency);
static_assert(inverse(frequency) != time);
static_assert(frequency * time != dimensionless);
static_assert(length * length != area);
static_assert(length * length != volume);
static_assert(area / length != length);
static_assert(length * length * length != volume);
static_assert(area * length != volume);
static_assert(volume / length != area);
static_assert(volume / length / length != length);
static_assert(area * area / length != volume);
static_assert(area * (area / length) != volume);
static_assert(volume / (length * length) != length);
static_assert(force * length != energy);
static_assert(force * length != torque);
static_assert(force * position_vector != energy);
static_assert(force * position_vector != torque);
static_assert(length / time != speed);
static_assert(displacement / time != speed);
static_assert(length / time != velocity);
static_assert(displacement / time != velocity);

static_assert(length * time / period_duration != time);
static_assert(length * height / width != length);

static_assert(length * time != speed);
static_assert(length / time / time != speed);
static_assert(length / speed != time);
static_assert(speed * time != length);

static_assert(length / time / time != acceleration);
static_assert(displacement / time / time != acceleration);
static_assert(displacement / (time * time) != acceleration);
static_assert(velocity / time != acceleration);
static_assert(velocity / acceleration != time);
static_assert(acceleration * time != velocity);
static_assert(acceleration * (time * time) != displacement);
static_assert(acceleration / speed != frequency);

// get_kind
static_assert(get_kind(length) == kind_of<length>);
static_assert(get_kind(distance) == kind_of<length>);
static_assert(get_kind(time) == kind_of<time>);
static_assert(get_kind(period_duration) == kind_of<time>);
static_assert(get_kind(length / time) == kind_of<length / time>);
static_assert(get_kind(speed) == kind_of<speed>);
static_assert(get_kind(height / time) == kind_of<length / time>);
static_assert(get_kind(inverse(time)) == kind_of<inverse(time)>);
static_assert(get_kind(inverse(period_duration)) == kind_of<inverse(time)>);
static_assert(get_kind(frequency) == kind_of<frequency>);
static_assert(get_kind(mass * frequency) == kind_of<mass * frequency>);
static_assert(get_kind(moment_of_force) == kind_of<moment_of_force>);
static_assert(get_kind(energy) == kind_of<energy>);
static_assert(get_kind(potential_energy) == kind_of<energy>);
static_assert(get_kind(kinetic_energy) == kind_of<energy>);
static_assert(get_kind(pow<1, 2>(area)) == kind_of<pow<1, 2>(area)>);
static_assert(get_kind(dimensionless) == kind_of<dimensionless>);
static_assert(get_kind(angular_measure) == kind_of<angular_measure>);
static_assert(get_kind(phase_angle) == kind_of<angular_measure>);
static_assert(get_kind(rotational_displacement) == kind_of<angular_measure>);

// comparisons of the same dimensions
static_assert(length == length);
static_assert(speed == speed);

// comparisons of equivalent dimensions (named vs unnamed/derived)
static_assert(length / length == dimensionless);

using namespace detail;

// defines_equation
static_assert(defines_equation(speed));
static_assert(defines_equation(rate_of_climb));
static_assert(defines_equation(velocity));
static_assert(!defines_equation(special_speed));
static_assert(!defines_equation(special_rate_of_climb));

static_assert(defines_equation(energy));
static_assert(defines_equation(kinetic_energy));
static_assert(defines_equation(gravitational_potential_energy));
static_assert(!defines_equation(mechanical_energy));
static_assert(!defines_equation(potential_energy));

// get_complexity
static_assert(get_complexity(length) == 0);
static_assert(get_complexity(pow<4>(length)) == 0);
static_assert(get_complexity(dimensionless) == 0);
static_assert(get_complexity(angular_measure) == 1);
static_assert(get_complexity(solid_angular_measure) == 2);
static_assert(get_complexity(length / time) == 0);
static_assert(get_complexity(mass * length / time) == 0);
static_assert(get_complexity(frequency) == 1);
static_assert(get_complexity(area) == 1);
static_assert(get_complexity(pow<2>(area)) == 1);
static_assert(get_complexity(volume) == 1);
static_assert(get_complexity(speed) == 1);
static_assert(get_complexity(special_speed) == 1);
static_assert(get_complexity(horizontal_speed) == 1);
static_assert(get_complexity(special_rate_of_climb) == 1);
static_assert(get_complexity(velocity) == 1);
static_assert(get_complexity(acceleration) == 2);
static_assert(get_complexity(force) == 3);
static_assert(get_complexity(dimensionless_rate) == 1);
static_assert(get_complexity(angular_measure_rate) == 2);
static_assert(get_complexity(solid_angular_measure_rate) == 3);

static_assert(get_complexity(acceleration * time) == 2);
static_assert(get_complexity(acceleration / time) == 2);
static_assert(get_complexity(speed * area) == 1);
static_assert(get_complexity(acceleration * area) == 2);
static_assert(get_complexity(acceleration * force) == 3);
static_assert(get_complexity(speed / frequency) == 1);
static_assert(get_complexity(acceleration / frequency) == 2);
static_assert(get_complexity(force / frequency) == 3);
static_assert(get_complexity(area / acceleration) == 2);
static_assert(get_complexity(area / force) == 3);
static_assert(get_complexity(speed * area / frequency) == 1);

// explode
static_assert(explode(frequency).equation == inverse(period_duration));
static_assert(explode(speed).equation == length / time);
static_assert(explode(special_speed).equation == length / time);
static_assert(explode(horizontal_speed).equation == horizontal_length / time);
static_assert(explode(rate_of_climb).equation == height / time);
static_assert(explode(special_rate_of_climb).equation == height / time);
static_assert(explode(velocity).equation == displacement / time);

using enum specs_convertible_result;

static_assert(have_common_base(width, length));
static_assert(have_common_base(width, height));
static_assert(have_common_base(angular_measure, dimensionless));
static_assert(have_common_base(angular_measure, solid_angular_measure));

static_assert(convertible_common_base(width, length) == yes);
static_assert(convertible_common_base(length, width) == explicit_conversion);
static_assert(convertible_common_base(width, height) == cast);
static_assert(convertible_common_base(angular_measure, dimensionless) == explicit_conversion_beyond_kind);
static_assert(convertible_common_base(dimensionless, angular_measure) == explicit_conversion);
static_assert(convertible_common_base(angular_measure, solid_angular_measure) == no);

static_assert(are_ingredients_convertible(length / time, length / time) == yes);
static_assert(are_ingredients_convertible(mass * length / time, mass* length / time) == yes);
static_assert(are_ingredients_convertible(area / length, area / length) == yes);

static_assert(are_ingredients_convertible(volume * length, area* area) == yes);
static_assert(are_ingredients_convertible(pow<4>(length), area* area) == yes);
static_assert(are_ingredients_convertible(dimensionless / (length * length), dimensionless / area) == yes);

static_assert(are_ingredients_convertible(width * height, length* length) == yes);
static_assert(are_ingredients_convertible(dimensionless / (width * height), dimensionless / (length * length)) == yes);

static_assert(are_ingredients_convertible(dimensionless / (width * height), dimensionless / area) == yes);

static_assert(are_ingredients_convertible(length * length, width* height) == explicit_conversion);
static_assert(are_ingredients_convertible(dimensionless / (length * length), dimensionless / (width * height)) ==
              explicit_conversion);

static_assert(are_ingredients_convertible(dimensionless / area, dimensionless / (length * length)) == yes);


// different dimensions
static_assert(convertible(mass, length) == no);
static_assert(convertible(speed, length) == no);
static_assert(convertible(length, speed) == no);
static_assert(convertible(energy, speed) == no);
static_assert(convertible(length, kind_of<time>) == no);
static_assert(convertible(kind_of<time>, length) == no);
static_assert(convertible(energy, kind_of<speed>) == no);
static_assert(convertible(kind_of<speed>, energy) == no);

// the same types
static_assert(convertible(length, length) == yes);
static_assert(convertible(width, width) == yes);
static_assert(convertible(energy, energy) == yes);
static_assert(convertible(kind_of<length>, kind_of<length>) == yes);
static_assert(convertible(kind_of<energy>, kind_of<energy>) == yes);
static_assert(convertible(get_kind(moment_of_force), get_kind(moment_of_force)) == yes);

// converting to a different branch
static_assert(convertible(height, width) == cast);
static_assert(convertible(potential_energy, kinetic_energy) == cast);
static_assert(convertible(kinetic_energy, potential_energy) == cast);
static_assert(convertible(rate_of_climb, velocity) == cast);
static_assert(convertible(rate_of_climb, horizontal_speed) == cast);

// converting to a different kind
static_assert(convertible(frequency, activity) == no);
static_assert(convertible(activity, frequency) == no);
static_assert(convertible(energy, moment_of_force) == no);
static_assert(convertible(energy, torque) == no);
static_assert(convertible(angular_measure, solid_angular_measure) == no);

// upcasting same hierarchy branch
static_assert(convertible(width, length) == yes);
static_assert(convertible(path_length, length) == yes);
static_assert(convertible(distance, length) == yes);
static_assert(convertible(distance, path_length) == yes);
static_assert(convertible(special_speed, speed) == yes);
static_assert(convertible(rate_of_climb, speed) == yes);
static_assert(convertible(special_rate_of_climb, speed) == yes);
static_assert(convertible(velocity, speed) == yes);
static_assert(convertible(potential_energy, energy) == yes);
static_assert(convertible(kinetic_energy, energy) == yes);

// upcasting beyond the hierarchy/kind
static_assert(convertible(angular_measure, dimensionless) == explicit_conversion_beyond_kind);
static_assert(convertible(angular_measure * time / period_duration, dimensionless) == explicit_conversion_beyond_kind);

// upcasting to the derived quantity
static_assert(convertible(frequency, inverse(time)) == yes);
static_assert(convertible(speed, length / time) == yes);
static_assert(convertible(speed, length / time) == yes);
static_assert(convertible(velocity, length / time) == yes);
static_assert(convertible(rate_of_climb, length / time) == yes);
static_assert(convertible(rate_of_climb, height / time) == yes);
static_assert(convertible(gravitational_potential_energy, mass* acceleration* length) == yes);
static_assert(convertible(angular_measure, arc_length / radius) == yes);
static_assert(convertible(solid_angular_measure, area / pow<2>(radius)) == yes);

// downcasting same hierarchy branch
static_assert(convertible(length, width) == explicit_conversion);
static_assert(convertible(path_length, distance) == explicit_conversion);
static_assert(convertible(length, distance) == explicit_conversion);
static_assert(convertible(path_length, distance) == explicit_conversion);
static_assert(convertible(speed, special_speed) == explicit_conversion);
static_assert(convertible(speed, rate_of_climb) == explicit_conversion);
static_assert(convertible(speed, special_rate_of_climb) == explicit_conversion);
static_assert(convertible(rate_of_climb, special_rate_of_climb) == explicit_conversion);
static_assert(convertible(energy, potential_energy) == explicit_conversion);
static_assert(convertible(energy, kinetic_energy) == explicit_conversion);
static_assert(convertible(dimensionless, rotation) == explicit_conversion);
static_assert(convertible(dimensionless, rotational_displacement) == explicit_conversion);

// derived quantities to type
static_assert(convertible(inverse(frequency), time) == yes);
static_assert(convertible(inverse(period_duration), frequency) == yes);
static_assert(convertible(length * length, area) == yes);
static_assert(convertible(length / time, speed) == yes);
static_assert(convertible(displacement / time, speed) == yes);
static_assert(convertible(displacement / time, velocity) == yes);
static_assert(convertible(height / time, speed) == yes);
static_assert(convertible(height / time, rate_of_climb) == yes);
static_assert(convertible(area / length, length) == yes);
static_assert(convertible(length * length * length, volume) == yes);
static_assert(convertible(area * length, volume) == yes);
static_assert(convertible(volume / length, area) == yes);
static_assert(convertible(volume / area, length) == yes);
static_assert(convertible(volume / length / length, length) == yes);
static_assert(convertible(area * area / length, volume) == yes);
static_assert(convertible(area * (area / length), volume) == yes);
static_assert(convertible(volume / (length * length), length) == yes);
static_assert(convertible(length / speed, time) == yes);
static_assert(convertible(speed * time, length) == yes);
static_assert(convertible(displacement / time / time, acceleration) == yes);
static_assert(convertible(displacement / (time * time), acceleration) == yes);
static_assert(convertible(velocity / time, acceleration) == yes);
static_assert(convertible(velocity / acceleration, time) == yes);
static_assert(convertible(acceleration * time, velocity) == yes);
static_assert(convertible(acceleration * (time * time), displacement) == yes);
static_assert(convertible(mass * pow<2>(length) / pow<2>(time), energy) == yes);
static_assert(convertible(force * length, energy) == yes);
static_assert(convertible(force * position_vector, moment_of_force) == yes);
static_assert(convertible(width * height, area) == yes);
static_assert(convertible(pow<1, 2>(area), length) == yes);
static_assert(convertible(length, pow<1, 2>(area)) == yes);
static_assert(convertible(mass * acceleration_of_free_fall * height, gravitational_potential_energy) == yes);
static_assert(convertible(mass * pow<2>(length) / pow<2>(time), kinetic_energy) == yes);
static_assert(convertible(arc_length / radius, angular_measure) == yes);
static_assert(convertible(area / pow<2>(radius), solid_angular_measure) == yes);

// additional dimensionless remainder
static_assert(convertible(length / speed, time) == yes);

// derived quantities to more constrained type
static_assert(convertible(inverse(time), frequency) == explicit_conversion);
static_assert(convertible(length / time / time, acceleration) == explicit_conversion);
static_assert(convertible(length / time, velocity) == explicit_conversion);
static_assert(convertible(length / time, rate_of_climb) == explicit_conversion);
static_assert(convertible(acceleration / velocity, frequency) == explicit_conversion);
static_assert(convertible(force * length, torque) == explicit_conversion);
static_assert(convertible(mass * acceleration * length, gravitational_potential_energy) == explicit_conversion);
static_assert(convertible(length / radius, angular_measure) == explicit_conversion);
static_assert(convertible(length / length, angular_measure) == explicit_conversion);
static_assert(convertible(arc_length / length, angular_measure) == explicit_conversion);
static_assert(convertible(area / pow<2>(length), solid_angular_measure) == explicit_conversion);

// derived quantities to more specialized type
static_assert(convertible(force * position_vector, torque) == explicit_conversion);
static_assert(convertible(length / time, special_speed) == explicit_conversion);
static_assert(convertible(height / time, special_rate_of_climb) == explicit_conversion);
static_assert(convertible(mass * pow<2>(length) / pow<2>(time), mechanical_energy) == explicit_conversion);
static_assert(convertible(mass * pow<2>(length) / pow<2>(time), potential_energy) == explicit_conversion);
static_assert(convertible(mass * pow<2>(length) / pow<2>(time), gravitational_potential_energy) == explicit_conversion);

// quantities derived from dimensionless
static_assert(convertible(power / power, efficiency) == explicit_conversion);
static_assert(convertible(stress / stress, strain) == explicit_conversion);
static_assert(convertible(stress / stress, efficiency) == explicit_conversion);

// specialized dimensionless to dimensionless and back
static_assert(convertible(height / width, dimensionless) == yes);
static_assert(convertible(dimensionless, height / width) == explicit_conversion);

// derived with dimensionless remainder after explosion
// TODO revise that
static_assert(convertible(distance / speed, time) == yes);

// derived quantities to incompatible type
static_assert(convertible(height / time, velocity) == cast);
static_assert(convertible(displacement / time, rate_of_climb) == cast);

// type to compatible derived
static_assert(convertible(distance, speed* time) == yes);

// type to more specialized derived quantity
static_assert(convertible(speed, height / time) == explicit_conversion);
static_assert(convertible(speed, displacement / time) == explicit_conversion);

// type to a derived quantity on a different branch
static_assert(convertible(velocity, height / time) == cast);
static_assert(convertible(rate_of_climb, displacement / time) == cast);

// derived quantities requiring explosion to a type
static_assert(convertible(acceleration * time, velocity) == yes);
static_assert(convertible(acceleration * period_duration, velocity) == yes);
static_assert(convertible(velocity * time / period_duration, velocity) == yes);
static_assert(convertible(mass * acceleration_of_free_fall * height / weight, height) == yes);

// derived quantities to more generic derived compatible type
static_assert(convertible(inverse(width * height), inverse(area)) == yes);
static_assert(convertible(path_length * distance, pow<2>(path_length)) == yes);

// derived to compatible derived
static_assert(convertible(inverse(length * length), inverse(area)) == yes);
static_assert(convertible(velocity * time, acceleration* pow<2>(time)) == yes);
static_assert(convertible(height / period_duration, length / time) == yes);
static_assert(convertible(height / width, length / length) == yes);
static_assert(convertible(height * width, length* length) == yes);
static_assert(convertible(inverse(path_length * distance), inverse(pow<2>(path_length))) == yes);
static_assert(convertible(path_length * period_duration, length* time) == yes);

static_assert(convertible(volume * length, pow<2>(area)) == yes);
static_assert(convertible(pow<4>(length), pow<2>(area)) == yes);
static_assert(convertible(pow<2>(radius), pow<2>(length)) == yes);

static_assert(convertible(height * time / period_duration, length) == yes);
static_assert(convertible(angular_measure * time / period_duration, arc_length / radius) == yes);
static_assert(convertible(special_angular_measure * solid_angular_measure, angular_measure* solid_angular_measure) ==
              yes);

// derived to more specialized derived
static_assert(convertible(length / time, height / period_duration) == explicit_conversion);
static_assert(convertible(length * length, height* width) == explicit_conversion);
static_assert(convertible(length * time, radius* period_duration) == explicit_conversion);

// derived to incompatible specialized derived
static_assert(convertible(height / time, distance / time) == cast);
static_assert(convertible(path_length * time, radius* period_duration) == cast);

// when more than one possible combination is present
// TODO revise that
static_assert(convertible(width * height, pow<2>(height)) == cast);
static_assert(convertible(inverse(width * height), inverse(pow<2>(height))) == cast);
static_assert(convertible(width * distance, path_length* width) == yes);
static_assert(convertible(height * distance, path_length* height) == cast);
static_assert(convertible(width * length, length* height) == explicit_conversion);
static_assert(convertible(length * distance, path_length* height) == cast);
static_assert(convertible(width * distance, width* path_length) == yes);
static_assert(convertible(length * distance, height* path_length) == cast);

// kind to its type
static_assert(convertible(kind_of<length>, length) == yes);
static_assert(convertible(kind_of<length>, width) == yes);
static_assert(convertible(kind_of<length>, position_vector) == yes);
static_assert(convertible(kind_of<frequency>, frequency) == yes);
static_assert(convertible(kind_of<speed>, velocity) == yes);
static_assert(convertible(kind_of<energy>, energy) == yes);
static_assert(convertible(kind_of<energy>, potential_energy) == yes);
static_assert(convertible(kind_of<energy>, kinetic_energy) == yes);
static_assert(convertible(kind_of<dimensionless>, rotation) == yes);
static_assert(convertible(kind_of<dimensionless>, angular_measure) == yes);
static_assert(convertible(kind_of<dimensionless>, rotational_displacement) == yes);

// kind to a type of a different dimension
static_assert(convertible(kind_of<length>, mass) == no);
static_assert(convertible(kind_of<length>, speed) == no);
static_assert(convertible(kind_of<energy>, length) == no);
static_assert(convertible(kind_of<energy>, speed) == no);

// kind to a type of another kind but the same dimension
static_assert(convertible(kind_of<energy>, moment_of_force) == no);
static_assert(convertible(kind_of<activity>, frequency) == no);
static_assert(convertible(kind_of<frequency>, activity) == no);

// derived kind to a compatible type
static_assert(convertible(kind_of<length / time>, speed) == yes);
static_assert(convertible(kind_of<length / time>, velocity) == yes);
static_assert(convertible(kind_of<length / pow<2>(time)>, acceleration) == yes);
static_assert(convertible(kind_of<inverse(time)>, frequency) == yes);
static_assert(convertible(kind_of<inverse(time)>, activity) == yes);
static_assert(convertible(kind_of<mass * pow<2>(length) / pow<2>(time)>, energy) == yes);
static_assert(convertible(kind_of<mass * pow<2>(length) / pow<2>(time)>, moment_of_force) == yes);
static_assert(convertible(kind_of<frequency * time / length>, repetency) == yes);

// type to a kind of a different kind
static_assert(convertible(mass, kind_of<length>) == no);
static_assert(convertible(speed, kind_of<length>) == no);
static_assert(convertible(length, kind_of<energy>) == no);
static_assert(convertible(speed, kind_of<energy>) == no);
static_assert(convertible(moment_of_force, kind_of<energy>) == no);
static_assert(convertible(frequency, kind_of<activity>) == no);
static_assert(convertible(activity, kind_of<frequency>) == no);

// converting type to a kind
static_assert(convertible(length, kind_of<length>) == yes);
static_assert(convertible(width, kind_of<length>) == yes);
static_assert(convertible(frequency, kind_of<frequency>) == yes);
static_assert(convertible(frequency, kind_of<inverse(time)>) == yes);
static_assert(convertible(frequency, kind_of<activity>) == no);
static_assert(convertible(energy, kind_of<energy>) == yes);
static_assert(convertible(potential_energy, kind_of<energy>) == yes);
static_assert(convertible(kinetic_energy, kind_of<energy>) == yes);
static_assert(convertible(rotation, kind_of<dimensionless>) == yes);
static_assert(convertible(angular_measure, kind_of<dimensionless>) == explicit_conversion_beyond_kind);
static_assert(convertible(rotational_displacement, kind_of<dimensionless>) == explicit_conversion_beyond_kind);

// converting derived type to a kind
static_assert(convertible(inverse(period_duration), kind_of<frequency>) == yes);
static_assert(convertible(inverse(time), kind_of<frequency>) == explicit_conversion);
static_assert(convertible(length / time, kind_of<speed>) == yes);
static_assert(convertible(length / pow<2>(time), kind_of<acceleration>) == explicit_conversion);

// converting kind to a kind
static_assert(convertible(kind_of<dimensionless>, kind_of<angular_measure>) == yes);
static_assert(convertible(kind_of<angular_measure>, kind_of<dimensionless>) == explicit_conversion_beyond_kind);

// converting derived kind to a kind
static_assert(convertible(kind_of<inverse(time)>, kind_of<frequency>) == yes);
static_assert(convertible(kind_of<length / time>, kind_of<speed>) == yes);
static_assert(convertible(kind_of<length / pow<2>(time)>, kind_of<acceleration>) == yes);

// converting type to a derived kind
static_assert(convertible(speed, kind_of<length / time>) == yes);
static_assert(convertible(velocity, kind_of<length / time>) == yes);
static_assert(convertible(energy, kind_of<mass * pow<2>(length) / pow<2>(time)>) == yes);
static_assert(convertible(mass_density, kind_of<mass / pow<3>(length)>) == yes);

// kinds of different dimensions
static_assert(convertible(kind_of<mass>, kind_of<length>) == no);
static_assert(convertible(kind_of<energy>, kind_of<length>) == no);
static_assert(convertible(kind_of<length>, kind_of<energy>) == no);
static_assert(convertible(kind_of<frequency>, kind_of<energy>) == no);

// derived quantities to dimensionless
static_assert(convertible(frequency * period_duration, dimensionless) == yes);
static_assert(convertible(frequency * time, dimensionless) == yes);
static_assert(convertible(length / length, dimensionless) == yes);
static_assert(convertible(length / width, dimensionless) == yes);
static_assert(convertible(active_power / apparent_power, dimensionless) == yes);

static_assert(convertible(efficiency, strain) == cast);

// nested kinds
static_assert(convertible(dimensionless, dimensionless) == yes);
static_assert(convertible(angular_measure, dimensionless) == explicit_conversion_beyond_kind);
static_assert(convertible(solid_angular_measure, dimensionless) == explicit_conversion_beyond_kind);

static_assert(convertible(kind_of<dimensionless>, dimensionless) == yes);
static_assert(convertible(kind_of<angular_measure>, dimensionless) == explicit_conversion_beyond_kind);
static_assert(convertible(kind_of<solid_angular_measure>, dimensionless) == explicit_conversion_beyond_kind);

static_assert(convertible(dimensionless, kind_of<dimensionless>) == yes);
static_assert(convertible(angular_measure, kind_of<dimensionless>) == explicit_conversion_beyond_kind);
static_assert(convertible(solid_angular_measure, kind_of<dimensionless>) == explicit_conversion_beyond_kind);

static_assert(convertible(kind_of<dimensionless>, kind_of<dimensionless>) == yes);
static_assert(convertible(kind_of<angular_measure>, kind_of<dimensionless>) == explicit_conversion_beyond_kind);
static_assert(convertible(kind_of<solid_angular_measure>, kind_of<dimensionless>) == explicit_conversion_beyond_kind);

static_assert(convertible(angular_measure, solid_angular_measure) == no);
static_assert(convertible(angular_measure, kind_of<solid_angular_measure>) == no);
static_assert(convertible(kind_of<angular_measure>, kind_of<solid_angular_measure>) == no);

static_assert(convertible(dimensionless, angular_measure) == explicit_conversion);
static_assert(convertible(dimensionless, solid_angular_measure) == explicit_conversion);

static_assert(convertible(dimensionless, kind_of<angular_measure>) == explicit_conversion);
static_assert(convertible(dimensionless, kind_of<solid_angular_measure>) == explicit_conversion);

static_assert(convertible(kind_of<dimensionless>, angular_measure) == yes);
static_assert(convertible(kind_of<dimensionless>, solid_angular_measure) == yes);

static_assert(convertible(kind_of<dimensionless>, kind_of<angular_measure>) == yes);
static_assert(convertible(kind_of<dimensionless>, kind_of<solid_angular_measure>) == yes);

// derived nested kinds
static_assert(convertible(dimensionless / time, dimensionless / time) == yes);
static_assert(convertible(angular_measure / time, dimensionless / time) == explicit_conversion_beyond_kind);
static_assert(convertible(solid_angular_measure / time, dimensionless / time) == explicit_conversion_beyond_kind);

static_assert(convertible(kind_of<dimensionless / time>, dimensionless / time) == yes);
static_assert(convertible(kind_of<angular_measure / time>, dimensionless / time) == explicit_conversion_beyond_kind);
static_assert(convertible(kind_of<solid_angular_measure / time>, dimensionless / time) ==
              explicit_conversion_beyond_kind);

static_assert(convertible(dimensionless / time, kind_of<dimensionless / time>) == yes);
static_assert(convertible(angular_measure / time, kind_of<dimensionless / time>) == explicit_conversion_beyond_kind);
static_assert(convertible(solid_angular_measure / time, kind_of<dimensionless / time>) ==
              explicit_conversion_beyond_kind);

static_assert(convertible(kind_of<dimensionless / time>, kind_of<dimensionless / time>) == yes);
static_assert(convertible(kind_of<angular_measure / time>, kind_of<dimensionless / time>) ==
              explicit_conversion_beyond_kind);
static_assert(convertible(kind_of<solid_angular_measure / time>, kind_of<dimensionless / time>) ==
              explicit_conversion_beyond_kind);

static_assert(convertible(angular_measure / time, solid_angular_measure / time) == no);
static_assert(convertible(angular_measure / time, kind_of<solid_angular_measure / time>) == no);
static_assert(convertible(kind_of<angular_measure / time>, kind_of<solid_angular_measure / time>) == no);

static_assert(convertible(dimensionless / time, angular_measure / time) == explicit_conversion);
static_assert(convertible(dimensionless / time, solid_angular_measure / time) == explicit_conversion);

static_assert(convertible(dimensionless / time, kind_of<angular_measure / time>) == explicit_conversion);
static_assert(convertible(dimensionless / time, kind_of<solid_angular_measure / time>) == explicit_conversion);

static_assert(convertible(kind_of<dimensionless / time>, angular_measure / time) == yes);
static_assert(convertible(kind_of<dimensionless / time>, solid_angular_measure / time) == yes);

static_assert(convertible(kind_of<dimensionless / time>, kind_of<angular_measure / time>) == yes);
static_assert(convertible(kind_of<dimensionless / time>, kind_of<solid_angular_measure / time>) == yes);

// named derived nested kinds
static_assert(convertible(dimensionless_rate, dimensionless_rate) == yes);
static_assert(convertible(angular_measure_rate, dimensionless_rate) == no);
static_assert(convertible(solid_angular_measure_rate, dimensionless_rate) == no);

static_assert(convertible(kind_of<dimensionless_rate>, dimensionless_rate) == yes);
static_assert(convertible(kind_of<angular_measure_rate>, dimensionless_rate) == no);
static_assert(convertible(kind_of<solid_angular_measure_rate>, dimensionless_rate) == no);

static_assert(convertible(dimensionless_rate, kind_of<dimensionless_rate>) == yes);
static_assert(convertible(angular_measure_rate, kind_of<dimensionless_rate>) == no);
static_assert(convertible(solid_angular_measure_rate, kind_of<dimensionless_rate>) == no);

static_assert(convertible(kind_of<dimensionless_rate>, kind_of<dimensionless_rate>) == yes);
static_assert(convertible(kind_of<angular_measure_rate>, kind_of<dimensionless_rate>) == no);
static_assert(convertible(kind_of<solid_angular_measure_rate>, kind_of<dimensionless_rate>) == no);

static_assert(convertible(angular_measure_rate, solid_angular_measure_rate) == no);
static_assert(convertible(angular_measure_rate, kind_of<solid_angular_measure_rate>) == no);
static_assert(convertible(kind_of<angular_measure_rate>, kind_of<solid_angular_measure_rate>) == no);

static_assert(convertible(dimensionless_rate, angular_measure_rate) == no);
static_assert(convertible(dimensionless_rate, solid_angular_measure_rate) == no);

static_assert(convertible(dimensionless_rate, kind_of<angular_measure_rate>) == no);
static_assert(convertible(dimensionless_rate, kind_of<solid_angular_measure_rate>) == no);

static_assert(convertible(kind_of<dimensionless_rate>, angular_measure_rate) == no);
static_assert(convertible(kind_of<dimensionless_rate>, solid_angular_measure_rate) == no);

static_assert(convertible(kind_of<dimensionless_rate>, kind_of<angular_measure_rate>) == no);
static_assert(convertible(kind_of<dimensionless_rate>, kind_of<solid_angular_measure_rate>) == no);

static_assert(convertible(dimensionless_rate, dimensionless / time) == yes);
static_assert(convertible(angular_measure_rate, dimensionless / time) == explicit_conversion_beyond_kind);
static_assert(convertible(solid_angular_measure_rate, dimensionless / time) == explicit_conversion_beyond_kind);

static_assert(convertible(kind_of<dimensionless_rate>, dimensionless / time) == yes);
static_assert(convertible(kind_of<angular_measure_rate>, dimensionless / time) == explicit_conversion_beyond_kind);
static_assert(convertible(kind_of<solid_angular_measure_rate>, dimensionless / time) ==
              explicit_conversion_beyond_kind);

static_assert(convertible(dimensionless_rate, kind_of<dimensionless / time>) == yes);
static_assert(convertible(angular_measure_rate, kind_of<dimensionless / time>) == explicit_conversion_beyond_kind);
static_assert(convertible(solid_angular_measure_rate, kind_of<dimensionless / time>) ==
              explicit_conversion_beyond_kind);

static_assert(convertible(kind_of<dimensionless_rate>, kind_of<dimensionless / time>) == yes);
static_assert(convertible(kind_of<angular_measure_rate>, kind_of<dimensionless / time>) ==
              explicit_conversion_beyond_kind);
static_assert(convertible(kind_of<solid_angular_measure_rate>, kind_of<dimensionless / time>) ==
              explicit_conversion_beyond_kind);

static_assert(convertible(angular_measure_rate, solid_angular_measure / time) == no);
static_assert(convertible(angular_measure_rate, kind_of<solid_angular_measure / time>) == no);
static_assert(convertible(kind_of<angular_measure_rate>, kind_of<solid_angular_measure / time>) == no);

static_assert(convertible(dimensionless_rate, angular_measure / time) == explicit_conversion);
static_assert(convertible(dimensionless_rate, kind_of<angular_measure / time>) == explicit_conversion);
static_assert(convertible(dimensionless_rate, solid_angular_measure / time) == explicit_conversion);
static_assert(convertible(dimensionless_rate, kind_of<solid_angular_measure / time>) == explicit_conversion);

static_assert(convertible(kind_of<dimensionless_rate>, angular_measure / time) == yes);
static_assert(convertible(kind_of<dimensionless_rate>, solid_angular_measure / time) == yes);

static_assert(convertible(kind_of<dimensionless_rate>, kind_of<angular_measure / time>) == yes);
static_assert(convertible(kind_of<dimensionless_rate>, kind_of<solid_angular_measure / time>) == yes);

static_assert(convertible(dimensionless / time, dimensionless_rate) == yes);
static_assert(convertible(angular_measure / time, dimensionless_rate) == yes);
static_assert(convertible(solid_angular_measure / time, dimensionless_rate) == yes);

static_assert(convertible(kind_of<dimensionless / time>, dimensionless_rate) == yes);
static_assert(convertible(kind_of<angular_measure / time>, dimensionless_rate) == yes);
static_assert(convertible(kind_of<solid_angular_measure / time>, dimensionless_rate) == yes);

static_assert(convertible(dimensionless / time, kind_of<dimensionless_rate>) == yes);
static_assert(convertible(angular_measure / time, kind_of<dimensionless_rate>) == yes);
static_assert(convertible(solid_angular_measure / time, kind_of<dimensionless_rate>) == yes);

static_assert(convertible(kind_of<dimensionless / time>, kind_of<dimensionless_rate>) == yes);
static_assert(convertible(kind_of<angular_measure / time>, kind_of<dimensionless_rate>) == yes);
static_assert(convertible(kind_of<solid_angular_measure / time>, kind_of<dimensionless_rate>) == yes);

static_assert(convertible(angular_measure / time, solid_angular_measure_rate) == no);
static_assert(convertible(angular_measure / time, kind_of<solid_angular_measure_rate>) == no);
static_assert(convertible(kind_of<angular_measure / time>, kind_of<solid_angular_measure_rate>) == no);

static_assert(convertible(dimensionless / time, angular_measure_rate) == explicit_conversion);
static_assert(convertible(dimensionless / time, solid_angular_measure_rate) == explicit_conversion);

static_assert(convertible(dimensionless / time, kind_of<angular_measure_rate>) == explicit_conversion);
static_assert(convertible(dimensionless / time, kind_of<solid_angular_measure_rate>) == explicit_conversion);

static_assert(convertible(kind_of<dimensionless / time>, angular_measure_rate) == yes);
static_assert(convertible(kind_of<dimensionless / time>, solid_angular_measure_rate) == yes);

static_assert(convertible(kind_of<dimensionless / time>, kind_of<angular_measure_rate>) == yes);
static_assert(convertible(kind_of<dimensionless / time>, kind_of<solid_angular_measure_rate>) == yes);

// get_common_quantity_spec
static_assert(get_common_quantity_spec(length, length) == length);
static_assert(get_common_quantity_spec(kind_of<length>, kind_of<length>) == kind_of<length>);
static_assert(get_common_quantity_spec(kind_of<length>, length) == length);
static_assert(get_common_quantity_spec(length, kind_of<length>) == length);
static_assert(get_common_quantity_spec(width, kind_of<length>) == width);
static_assert(get_common_quantity_spec(kind_of<length>, width) == width);
static_assert(get_common_quantity_spec(kind_of<dimensionless>, kind_of<angular_measure>) == kind_of<angular_measure>);
static_assert(get_common_quantity_spec(kind_of<dimensionless>, angular_measure) == angular_measure);

static_assert(get_common_quantity_spec(width, height) == length);
static_assert(get_common_quantity_spec(distance, path_length) == path_length);
static_assert(get_common_quantity_spec(potential_energy, kinetic_energy) == mechanical_energy);

static_assert(get_common_quantity_spec(length / time, length / time) == length / time);
static_assert(get_common_quantity_spec(length / time, inverse(time / length)) == length / time);

static_assert(get_common_quantity_spec(speed, length / time) == speed);
static_assert(get_common_quantity_spec(length / time, speed) == speed);
static_assert(get_common_quantity_spec(area, length* length) == area);
static_assert(get_common_quantity_spec(length * length, area) == area);
static_assert(get_common_quantity_spec(frequency, inverse(time)) == inverse(time));
static_assert(get_common_quantity_spec(inverse(time), frequency) == inverse(time));
static_assert(get_common_quantity_spec(frequency, inverse(period_duration)) == frequency);
static_assert(get_common_quantity_spec(inverse(period_duration), frequency) == frequency);
static_assert(get_common_quantity_spec(kinetic_energy, mass* pow<2>(length) / pow<2>(time)) == kinetic_energy);
static_assert(get_common_quantity_spec(mass * pow<2>(length) / pow<2>(time), kinetic_energy) == kinetic_energy);
static_assert(get_common_quantity_spec(gravitational_potential_energy, mass* acceleration_of_free_fall* height) ==
              gravitational_potential_energy);
static_assert(get_common_quantity_spec(mass * acceleration_of_free_fall * height, gravitational_potential_energy) ==
              gravitational_potential_energy);

static_assert(get_common_quantity_spec(gravitational_potential_energy, mass* acceleration* length) ==
              mass * acceleration * length);
static_assert(get_common_quantity_spec(mass * acceleration * length, gravitational_potential_energy) ==
              mass * acceleration * length);

template<auto T1, auto T2>
concept no_common_quantity_spec = requires {
  requires !requires { get_common_quantity_spec(T1, T2); };
  requires !requires { get_common_quantity_spec(T2, T1); };
};
static_assert(no_common_quantity_spec<energy, torque>);
static_assert(no_common_quantity_spec<kind_of<energy>, torque>);
static_assert(no_common_quantity_spec<frequency, activity>);
static_assert(no_common_quantity_spec<kind_of<frequency>, activity>);
static_assert(no_common_quantity_spec<frequency, kind_of<activity>>);
static_assert(no_common_quantity_spec<kind_of<frequency>, kind_of<activity>>);
static_assert(no_common_quantity_spec<dimensionless, angular_measure>);
static_assert(no_common_quantity_spec<dimensionless, kind_of<angular_measure>>);
static_assert(no_common_quantity_spec<angular_measure, solid_angular_measure>);
static_assert(no_common_quantity_spec<kind_of<angular_measure>, solid_angular_measure>);
static_assert(no_common_quantity_spec<angular_measure, kind_of<solid_angular_measure>>);
static_assert(no_common_quantity_spec<kind_of<angular_measure>, kind_of<solid_angular_measure>>);

}  // namespace
