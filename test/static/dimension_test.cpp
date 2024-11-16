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

using dimension_one_ = struct dimension_one;

// clang-format off
inline constexpr struct length_ final : base_dimension<"L"> {} length;
inline constexpr struct mass_ final : base_dimension<"M"> {} mass;
inline constexpr struct time_ final : base_dimension<"T"> {} time;

inline constexpr auto my_length1 = length;
inline constexpr auto my_length2 = length;

QUANTITY_SPEC_(q_time, time);
inline constexpr struct second_ final : named_unit<"s", kind_of<q_time>> {} second;

inline constexpr auto frequency = inverse(time);
inline constexpr auto action = inverse(time);
inline constexpr auto area = length * length;
inline constexpr auto volume = area * length;
inline constexpr auto speed = length / time;
inline constexpr auto acceleration = speed / time;
inline constexpr auto force = mass * acceleration;
inline constexpr auto moment_of_force = length * force;
inline constexpr auto torque = moment_of_force;
inline constexpr auto pressure = force / area;
inline constexpr auto stress = pressure;
inline constexpr auto strain = stress / stress;
inline constexpr auto power = force * speed;
inline constexpr auto efficiency = power / power;
inline constexpr auto energy = force * length;
// clang-format on

// concepts verification
static_assert(detail::BaseDimension<length_>);
static_assert(!detail::BaseDimension<MP_UNITS_NONCONST_TYPE(frequency)>);
static_assert(Dimension<length_>);
static_assert(Dimension<MP_UNITS_NONCONST_TYPE(frequency)>);

static_assert(detail::BaseDimension<decltype(speed * time)>);  // length

// derived dimension expression template syntax verification
static_assert(is_of_type<inverse(time), derived_dimension<dimension_one_, per<time_>>>);
static_assert(is_of_type<dimension_one / inverse(time), time_>);

static_assert(is_of_type<dimension_one * time, time_>);
static_assert(is_of_type<time * dimension_one, time_>);
static_assert(is_of_type<dimension_one * inverse(time), derived_dimension<dimension_one_, per<time_>>>);
static_assert(is_of_type<inverse(time) * dimension_one, derived_dimension<dimension_one_, per<time_>>>);

static_assert(is_of_type<length * time, derived_dimension<length_, time_>>);
static_assert(is_of_type<length * length, derived_dimension<mp_units::power<length_, 2>>>);

static_assert(is_of_type<length * length * time, derived_dimension<mp_units::power<length_, 2>, time_>>);
static_assert(is_of_type<length * time * length, derived_dimension<mp_units::power<length_, 2>, time_>>);

static_assert(is_of_type<length*(time* length), derived_dimension<mp_units::power<length_, 2>, time_>>);
static_assert(is_of_type<time*(length* length), derived_dimension<mp_units::power<length_, 2>, time_>>);

static_assert(is_of_type<inverse(time) * length, derived_dimension<length_, per<time_>>>);
static_assert(is_of_type<inverse(time) * time, dimension_one_>);

static_assert(is_of_type<time / dimension_one, time_>);
static_assert(is_of_type<inverse(time) / dimension_one, derived_dimension<dimension_one_, per<time_>>>);

static_assert(is_of_type<length / time * time, length_>);
static_assert(
  is_of_type<inverse(time) * inverse(time), derived_dimension<dimension_one_, per<mp_units::power<time_, 2>>>>);
static_assert(is_of_type<inverse(time* time), derived_dimension<dimension_one_, per<mp_units::power<time_, 2>>>>);
static_assert(is_of_type<dimension_one / inverse(time* time), derived_dimension<mp_units::power<time_, 2>>>);

static_assert(is_of_type<length / time * inverse(time), derived_dimension<length_, per<mp_units::power<time_, 2>>>>);
static_assert(is_of_type<length / time*(length / time),
                         derived_dimension<mp_units::power<length_, 2>, per<mp_units::power<time_, 2>>>>);
static_assert(is_of_type<length / time*(time / length), dimension_one_>);

static_assert(is_of_type<speed / acceleration, time_>);
static_assert(is_of_type<acceleration / speed, derived_dimension<dimension_one_, per<time_>>>);
static_assert(is_of_type<speed * speed / length, derived_dimension<length_, per<mp_units::power<time_, 2>>>>);
static_assert(is_of_type<inverse(speed* speed) * length, derived_dimension<mp_units::power<time_, 2>, per<length_>>>);

static_assert(is_of_type<(length * length) * (time * time),
                         derived_dimension<mp_units::power<length_, 2>, mp_units::power<time_, 2>>>);
static_assert(is_of_type<(time * time) * (length * length),
                         derived_dimension<mp_units::power<length_, 2>, mp_units::power<time_, 2>>>);

static_assert(is_of_type<length * time * time, derived_dimension<length_, mp_units::power<time_, 2>>>);
static_assert(
  is_of_type<mass / length / time / time, derived_dimension<mass_, per<length_, mp_units::power<time_, 2>>>>);
static_assert(
  is_of_type<mass / (length * time * time), derived_dimension<mass_, per<length_, mp_units::power<time_, 2>>>>);
static_assert(
  is_of_type<mass / length / (time * time), derived_dimension<mass_, per<length_, mp_units::power<time_, 2>>>>);
static_assert(is_of_type<force / area, derived_dimension<mass_, per<length_, mp_units::power<time_, 2>>>>);

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
  requires !requires { t + q_time[second]; };
  requires !requires { t - q_time[second]; };
  requires !requires { t* q_time[second]; };
  requires !requires { t / q_time[second]; };
  requires !requires { t == q_time[second]; };
  requires !requires { t < q_time[second]; };
  requires !requires { q_time[second] + t; };
  requires !requires { q_time[second] - t; };
  requires !requires { q_time[second] * t; };
  requires !requires { q_time[second] / t; };
  requires !requires { q_time[second] == t; };
  requires !requires { q_time[second] < t; };
  requires !requires { t + 1 * q_time[second]; };
  requires !requires { t - 1 * q_time[second]; };
  requires !requires { t * 1 * q_time[second]; };
  requires !requires { t / 1 * q_time[second]; };
  requires !requires { t == 1 * q_time[second]; };
  requires !requires { t == 1 * q_time[second]; };
  requires !requires { 1 * q_time[second] + t; };
  requires !requires { 1 * q_time[second] - t; };
  requires !requires { 1 * q_time[second] * t; };
  requires !requires { 1 * q_time[second] == t; };
  requires !requires { 1 * q_time[second] < t; };
};
static_assert(invalid_operations<time>);

// comparisons of the same dimensions
static_assert(length == length);
static_assert(length == my_length1);
static_assert(my_length1 == my_length2);
static_assert(speed == speed);

// comparisons of equivalent dimensions (named vs unnamed/derived)
static_assert(length / length == dimension_one);

static_assert(inverse(time) == frequency);
static_assert(inverse(frequency) == time);
static_assert(frequency * time == dimension_one);

static_assert(length * length == area);
static_assert(length * length != volume);
static_assert(area / length == length);

static_assert(length * length * length == volume);
static_assert(area * length == volume);
static_assert(volume / length == area);
static_assert(volume / length / length == length);
static_assert(area * area / length == volume);
static_assert(area * (area / length) == volume);
static_assert(volume / (length * length) == length);

static_assert(length / time == speed);
static_assert(length * time != speed);
static_assert(length / time / time != speed);
static_assert(length / speed == time);
static_assert(speed * time == length);

static_assert(length / time / time == acceleration);
static_assert(length / (time * time) == acceleration);
static_assert(speed / time == acceleration);
static_assert(speed / acceleration == time);
static_assert(acceleration * time == speed);
static_assert(acceleration * (time * time) == length);
static_assert(acceleration / speed == frequency);

// comparison of convertible unnamed dimensions
static_assert(is_of_type<mass * acceleration, derived_dimension<length_, mass_, per<mp_units::power<time_, 2>>>>);
static_assert(is_of_type<acceleration * mass, derived_dimension<length_, mass_, per<mp_units::power<time_, 2>>>>);
static_assert(mass * acceleration == acceleration * mass);

// comparisons of equivalent but not convertible dimensions
static_assert(energy == torque);

static_assert(force * length == energy);
static_assert(force * length == torque);

static_assert(frequency == action);

// dimension_one
static_assert(power / power == efficiency);
static_assert(dimension_one == efficiency);

static_assert(efficiency == strain);

static_assert(stress / stress == strain);
static_assert(stress / stress == efficiency);

// comparison of not equivalent dimensions
static_assert(length != time);

static_assert(acceleration != speed);

// power
static_assert(is_of_type<pow<0>(length), dimension_one_>);
static_assert(is_of_type<pow<1>(length), length_>);
static_assert(is_of_type<pow<2, 2>(length), length_>);
static_assert(is_of_type<pow<2>(dimension_one), dimension_one_>);
static_assert(is_of_type<pow<2>(length), derived_dimension<mp_units::power<length_, 2>>>);
static_assert(is_of_type<pow<1, 2>(length), derived_dimension<mp_units::power<length_, 1, 2>>>);
static_assert(is_of_type<pow<1, 2>(length* length), length_>);
static_assert(is_of_type<pow<1, 3>(length* length* length), length_>);
static_assert(is_of_type<pow<1, 3>(length* length), derived_dimension<mp_units::power<length_, 2, 3>>>);
static_assert(is_of_type<pow<1, 2>(length / time),
                         derived_dimension<mp_units::power<length_, 1, 2>, per<mp_units::power<time_, 1, 2>>>>);
static_assert(
  is_of_type<pow<1, 2>(length / (time * time)), derived_dimension<mp_units::power<length_, 1, 2>, per<time_>>>);

static_assert(is_same_v<decltype(pow<2>(length)), decltype(length * length)>);
static_assert(is_same_v<decltype(pow<2>(length / time)), decltype(length * length / time / time)>);

}  // namespace
