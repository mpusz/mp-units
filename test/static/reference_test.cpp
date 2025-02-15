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
#include <mp-units/framework.h>
#include <mp-units/systems/si/prefixes.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cstdint>
#include <type_traits>
#endif

namespace {

using namespace mp_units;

using dimensionless_ = struct dimensionless;
using one_ = struct one;

// base dimensions
// clang-format off
inline constexpr struct dim_length_ final : base_dimension<"L"> {} dim_length;
inline constexpr struct dim_mass_ final : base_dimension<"M"> {} dim_mass;
inline constexpr struct dim_time_ final : base_dimension<"T"> {} dim_time;

// quantities specification
QUANTITY_SPEC_(length, dim_length);
QUANTITY_SPEC_(mass, dim_mass);
QUANTITY_SPEC_(time, dim_time);

QUANTITY_SPEC_(width, length);
QUANTITY_SPEC_(radius, width);
QUANTITY_SPEC_(arc_length, length);

QUANTITY_SPEC_(frequency, inverse(time));
QUANTITY_SPEC_(activity, inverse(time));
QUANTITY_SPEC_(area, length* length);
QUANTITY_SPEC_(angular_measure, dimensionless, arc_length / radius, is_kind);
QUANTITY_SPEC_(solid_angular_measure, dimensionless, area / pow<2>(radius), is_kind);
QUANTITY_SPEC_(speed, length / time);
QUANTITY_SPEC_(acceleration, speed / time);
QUANTITY_SPEC_(force, mass* acceleration);
QUANTITY_SPEC_(power, force* speed);
QUANTITY_SPEC_(storage_capacity, dimensionless, is_kind);

// base units
inline constexpr struct second_ final : named_unit<"s", kind_of<time>> {} second;
inline constexpr struct metre_ final : named_unit<"m", kind_of<length>> {} metre;
inline constexpr struct gram_ final : named_unit<"g", kind_of<mass>> {} gram;
inline constexpr auto kilogram = si::kilo<gram>;

namespace nu {
// hypothetical natural system of units for c=1

inline constexpr struct second_ final : named_unit<"s"> {} second;
inline constexpr struct minute_ final : named_unit<"min", mag<60> * second> {} minute;

inline constexpr struct time : system_reference<time_{}, second> {} time;
inline constexpr struct length : system_reference<length_{}, second> {} length;
inline constexpr struct speed : system_reference<speed_{}, second / second> {} speed;

}

// derived named units
inline constexpr struct radian_ final : named_unit<"rad", metre / metre, kind_of<angular_measure>> {} radian;
inline constexpr struct steradian_ final : named_unit<"sr", square(metre) / square(metre), kind_of<solid_angular_measure>> {} steradian;
inline constexpr struct hertz_ final : named_unit<"Hz", inverse(second), kind_of<frequency>> {} hertz;
inline constexpr struct becquerel_ final : named_unit<"Bq", inverse(second), kind_of<activity>> {} becquerel;
inline constexpr struct newton_ final : named_unit<"N", kilogram * metre / square(second)> {} newton;
inline constexpr struct joule_ final : named_unit<"J", newton * metre> {} joule;
inline constexpr struct watt_ final : named_unit<"W", joule / second> {} watt;

inline constexpr struct minute_ final : named_unit<"min", mag<60> * second> {} minute;
inline constexpr struct hour_ final : named_unit<"h", mag<60> * minute> {} hour;
inline constexpr auto kilometre = si::kilo<metre>;

inline constexpr struct bit_ final : named_unit<"bit", one, kind_of<storage_capacity>> {} bit;
// clang-format on


static_assert(is_of_type<length[metre], reference<length_, metre_>>);
static_assert(is_of_type<kind_of<length>[metre], metre_>);

static_assert(
  is_of_type<(length / time)[metre / second], reference<decltype(length / time), decltype(metre / second)>>);
static_assert(is_of_type<(kind_of<length> / kind_of<time>)[metre / second], derived_unit<metre_, per<second_>>>);

// Unit as a reference
static_assert(is_of_type<42 * metre, quantity<metre, int>>);
static_assert(quantity<metre, int>::quantity_spec == kind_of<length>);
static_assert(is_of_type<42 * square(metre), quantity<square(metre), int>>);
static_assert(quantity<square(metre), int>::quantity_spec == kind_of<pow<2>(length)>);
static_assert(is_of_type<42 * metre / second, quantity<metre / second, int>>);
static_assert(quantity<metre / second, int>::quantity_spec == kind_of<length / time>);
static_assert(is_of_type<42 * newton, quantity<newton, int>>);
static_assert(quantity<newton, int>::quantity_spec == kind_of<mass * length / pow<2>(time)>);
static_assert(is_of_type<42 * hertz, quantity<hertz, int>>);
static_assert(quantity<hertz, int>::quantity_spec == kind_of<frequency>);

// Named quantity/dimension and unit
static_assert(is_of_type<5 * power[watt], quantity<reference<power_, watt_>{}, int>>);

// Named quantity/dimension and derived (unnamed) unit
static_assert(
  is_of_type<5 * speed[metre / second], quantity<reference<speed_, derived_unit<metre_, per<second_>>>{}, int>>);

// Derived (unnamed) quantity/dimension and derived (unnamed) unit
static_assert(
  is_of_type<
    10 * length[metre] / (2 * time[second]),
    quantity<reference<derived_quantity_spec<length_, per<time_>>, derived_unit<metre_, per<second_>>>{}, int>>);

// Base quantity as a result of dimensional transformation
static_assert(is_of_type<5 * speed[metre / second] * (5 * time[second]),
                         quantity<reference<derived_quantity_spec<speed_, time_>, metre_>{}, int>>);

// dimensionless
static_assert(is_of_type<20 * speed[metre / second] / (10 * length[metre]) * (5 * time[second]),
                         quantity<reference<derived_quantity_spec<speed_, time_, per<length_>>, one_>{}, int>>);

template<auto s>
concept invalid_operations = requires {
  requires !requires { s / 2; };
  requires !requires { s * 2; };
  requires !requires { s + 2; };
  requires !requires { 2 + s; };
  requires !requires { s + s; };
  requires !requires { s - 2; };
  requires !requires { 2 - s; };
  requires !requires { s - s; };
  requires !requires { s < s; };
  requires !requires { s + 1 * time[second]; };
  requires !requires { s - 1 * time[second]; };
  requires !requires { s * 1 * time[second]; };
  requires !requires { s / 1 * time[second]; };
  requires !requires { s == 1 * time[second]; };
  requires !requires { s < 1 * time[second]; };
  requires !requires { 1 * time[second] + s; };
  requires !requires { 1 * time[second] - s; };
  requires !requires { 1 * time[second] == s; };
  requires !requires { 1 * time[second] < s; };
};
static_assert(invalid_operations<time[second]>);

static_assert(is_of_type<2 / second, quantity<derived_unit<one_, per<second_>>{}, int>>);
static_assert(
  is_of_type<
    2 / time[second],
    quantity<reference<derived_quantity_spec<dimensionless_, per<time_>>, derived_unit<one_, per<second_>>>{}, int>>);
static_assert(
  is_of_type<1 * time[second] * second, quantity<reference<decltype(pow<2>(time)), decltype(pow<2>(second))>{}, int>>);
static_assert(is_of_type<1 * time[second] * time[second],
                         quantity<reference<decltype(pow<2>(time)), decltype(pow<2>(second))>{}, int>>);
static_assert(is_of_type<1 * time[second] / second, quantity<reference<dimensionless_, one_>{}, int>>);
static_assert(is_of_type<1 * time[second] / time[second], quantity<reference<dimensionless_, one_>{}, int>>);

static_assert(
  is_of_type<
    1 * inverse(time[second]),
    quantity<reference<derived_quantity_spec<dimensionless_, per<time_>>, derived_unit<one_, per<second_>>>{}, int>>);

static_assert(
  is_of_type<
    2 * length[metre] / (1 * time[second]),
    quantity<reference<derived_quantity_spec<length_, per<time_>>, derived_unit<metre_, per<second_>>>{}, int>>);
static_assert(
  is_of_type<
    2 * (length[metre] / time[second]),
    quantity<reference<derived_quantity_spec<length_, per<time_>>, derived_unit<metre_, per<second_>>>{}, int>>);
static_assert(
  is_of_type<2 * (speed[metre / second]), quantity<reference<speed_, derived_unit<metre_, per<second_>>>{}, int>>);

constexpr auto m_per_s = speed[metre / second];
static_assert(is_of_type<2 * m_per_s, quantity<reference<speed_, derived_unit<metre_, per<second_>>>{}, int>>);

static_assert(is_of_type<120 * length[kilometre] / (2 * time[hour]),
                         quantity<reference<derived_quantity_spec<length_, per<time_>>,
                                            derived_unit<MP_UNITS_NONCONST_TYPE(si::kilo<metre>), per<hour_>>>{},
                                  int>>);
static_assert(120 * length[kilometre] / (2 * time[hour]) == 60 * speed[kilometre / hour]);
static_assert(is_of_type<[] {
  const auto distance = 120;
  const auto duration = 2;
  return distance * length[kilometre] / (duration * time[hour]);
}(),
                         quantity<reference<derived_quantity_spec<length_, per<time_>>,
                                            derived_unit<MP_UNITS_NONCONST_TYPE(si::kilo<metre>), per<hour_>>>{},
                                  int>>);
static_assert(is_of_type<std::int64_t{120} * length[kilometre] / (2 * time[hour]),
                         quantity<reference<derived_quantity_spec<length_, per<time_>>,
                                            derived_unit<MP_UNITS_NONCONST_TYPE(si::kilo<metre>), per<hour_>>>{},
                                  std::int64_t>>);
static_assert(is_of_type<120.L * length[kilometre] / (2 * time[hour]),
                         quantity<reference<derived_quantity_spec<length_, per<time_>>,
                                            derived_unit<MP_UNITS_NONCONST_TYPE(si::kilo<metre>), per<hour_>>>{},
                                  long double>>);

static_assert(is_of_type<1. / 4 * area[square(metre)], decltype(1. * area[square(metre)] / 4)>);
static_assert(1. / 4 * area[square(metre)] == 1. * area[square(metre)] / 4);

// Natural Units
static_assert(is_of_type<42 * nu::time[nu::second], quantity<reference<time_, nu::second_>{}, int>>);
static_assert(is_of_type<42 * nu::time[nu::minute], quantity<reference<time_, nu::minute_>{}, int>>);
static_assert(is_of_type<42 * nu::length[nu::second], quantity<reference<length_, nu::second_>{}, int>>);
static_assert(is_of_type<42 * nu::length[nu::minute], quantity<reference<length_, nu::minute_>{}, int>>);
static_assert(is_of_type<42 * (nu::length[nu::second] / nu::time[nu::second]),
                         quantity<reference<derived_quantity_spec<length_, per<time_>>, one_>{}, int>>);
static_assert(is_of_type<42 * nu::length[nu::second] / (42 * nu::time[nu::second]),
                         quantity<reference<derived_quantity_spec<length_, per<time_>>, one_>{}, int>>);
static_assert(is_of_type<42 * nu::speed[nu::second / nu::second], quantity<reference<speed_, one_>{}, int>>);
static_assert(is_of_type<42 * nu::speed[one], quantity<reference<speed_, one_>{}, int>>);
static_assert(
  is_of_type<
    42 * mass[kilogram] * (1 * nu::length[nu::second]) / (1 * nu::time[nu::second]),
    quantity<reference<derived_quantity_spec<length_, mass_, per<time_>>, MP_UNITS_NONCONST_TYPE(si::kilo<gram>)>{},
             int>>);

// Mixed-systems quantities
static_assert(
  is_of_type<
    42 * metre / nu::time[nu::second],
    quantity<reference<derived_quantity_spec<length_, per<time_>>, derived_unit<metre_, per<nu::second_>>>{}, int>>);

template<auto dim, auto unit>
concept invalid_nu_unit = !requires { dim[unit]; };

static_assert(invalid_nu_unit<time, nu::second>);
static_assert(invalid_nu_unit<nu::time, second>);
static_assert(invalid_nu_unit<length / time, nu::second / nu::second>);
static_assert(invalid_nu_unit<speed, nu::second / nu::second>);
static_assert(invalid_nu_unit<speed, nu::second / second>);
static_assert(invalid_nu_unit<mass * length / time, kilogram * nu::second / nu::second>);
static_assert(invalid_nu_unit<force, kilogram * nu::second / nu::second>);

// mixing associated units and references
static_assert(second != time[second]);
static_assert(time[second] != second);
static_assert(second * second != time[second] * time[second]);
static_assert(second * time[second] == time[second] * second);
static_assert(is_of_type<second * time[second], decltype(time[second] * second)>);
static_assert(is_of_type<second * time[second], decltype(time[second] * time[second])>);
static_assert(metre / second != length[metre] / time[second]);
static_assert(metre / time[second] == length[metre] / second);
static_assert(is_of_type<metre / time[second], decltype(length[metre] / second)>);
static_assert(is_of_type<metre / time[second], decltype(length[metre] / time[second])>);
static_assert(kind_of<length>[metre] == metre);

template<auto QS, auto U>
concept invalid_unit = !requires { QS[U]; };

static_assert(invalid_unit<frequency, becquerel>);
static_assert(invalid_unit<activity, hertz>);

static_assert(invalid_unit<dimensionless, radian>);
static_assert(invalid_unit<dimensionless, steradian>);
static_assert(invalid_unit<dimensionless, bit>);
static_assert(invalid_unit<angular_measure, steradian>);
static_assert(invalid_unit<angular_measure, bit>);
static_assert(invalid_unit<solid_angular_measure, radian>);
static_assert(invalid_unit<solid_angular_measure, bit>);
static_assert(invalid_unit<storage_capacity, radian>);
static_assert(invalid_unit<storage_capacity, steradian>);

static_assert(is_of_type<get_common_reference(dimensionless[one], one), reference<dimensionless_, one_>>);
static_assert(is_of_type<get_common_reference(radian, one), radian_>);
static_assert(is_of_type<get_common_reference(one, radian), radian_>);
static_assert(is_of_type<get_common_reference(angular_measure[radian], one), reference<angular_measure_, radian_>>);
static_assert(is_of_type<get_common_reference(one, angular_measure[radian]), reference<angular_measure_, radian_>>);

template<auto R1, auto R2>
concept no_common_reference = requires {
  requires !requires { get_common_reference(R1, R2); };
  requires !requires { get_common_reference(R2, R1); };
};

static_assert(no_common_reference<hertz, becquerel>);
static_assert(no_common_reference<frequency[hertz], becquerel>);
static_assert(no_common_reference<hertz, activity[becquerel]>);
static_assert(no_common_reference<frequency[hertz], activity[becquerel]>);
static_assert(no_common_reference<radian, steradian>);
static_assert(no_common_reference<angular_measure[radian], steradian>);
static_assert(no_common_reference<radian, solid_angular_measure[steradian]>);
static_assert(no_common_reference<angular_measure[radian], solid_angular_measure[steradian]>);
static_assert(no_common_reference<radian, dimensionless[one]>);
static_assert(no_common_reference<angular_measure[radian], dimensionless[one]>);

// addition of various dimensionless quantities
static_assert(is_of_type<1 * radian + 1 * one, quantity<radian, int>>);
static_assert(is_of_type<angular_measure(1 * radian) + 1 * one, quantity<angular_measure[radian], int>>);
static_assert(is_of_type<1 * steradian + 1 * one, quantity<steradian, int>>);
static_assert(
  is_of_type<solid_angular_measure(1 * steradian) + 1 * one, quantity<solid_angular_measure[steradian], int>>);

// subtraction of various dimensionless quantities
static_assert(is_of_type<1 * radian - 1 * one, quantity<radian, int>>);
static_assert(is_of_type<angular_measure(1 * radian) - 1 * one, quantity<angular_measure[radian], int>>);
static_assert(is_of_type<1 * steradian - 1 * one, quantity<steradian, int>>);
static_assert(
  is_of_type<solid_angular_measure(1 * steradian) - 1 * one, quantity<solid_angular_measure[steradian], int>>);

// comparison of various dimensionless quantities
static_assert(1 * radian == 1 * one);
static_assert(angular_measure(1 * radian) == 1 * one);

static_assert(1 * steradian == 1 * one);
static_assert(solid_angular_measure(1 * steradian) == 1 * one);

// invalid operations on dimensionless quantities
template<auto Q1, auto Q2>
concept invalid_addition = requires {
  requires !requires { Q1 + Q2; };
  requires !requires { Q2 + Q1; };
};

static_assert(invalid_addition<1 * hertz, 1 * becquerel>);
static_assert(invalid_addition<1 * radian, 1 * steradian>);
static_assert(invalid_addition<1 * radian, 1 * bit>);
static_assert(invalid_addition<frequency(1 * hertz), activity(1 * becquerel)>);
static_assert(invalid_addition<angular_measure(1 * radian), solid_angular_measure(1 * steradian)>);
static_assert(invalid_addition<angular_measure(1 * radian), storage_capacity(1 * bit)>);
static_assert(invalid_addition<1 * radian, dimensionless(1 * one)>);
static_assert(invalid_addition<angular_measure(1 * radian), dimensionless(1 * one)>);

template<auto Q1, auto Q2>
concept invalid_subtraction = requires {
  requires !requires { Q1 - Q2; };
  requires !requires { Q2 - Q1; };
};

static_assert(invalid_subtraction<1 * hertz, 1 * becquerel>);
static_assert(invalid_subtraction<1 * radian, 1 * steradian>);
static_assert(invalid_subtraction<1 * radian, 1 * bit>);
static_assert(invalid_subtraction<frequency(1 * hertz), activity(1 * becquerel)>);
static_assert(invalid_subtraction<angular_measure(1 * radian), solid_angular_measure(1 * steradian)>);
static_assert(invalid_subtraction<angular_measure(1 * radian), storage_capacity(1 * bit)>);
static_assert(invalid_subtraction<1 * radian, dimensionless(1 * one)>);
static_assert(invalid_subtraction<angular_measure(1 * radian), dimensionless(1 * one)>);

template<auto Q1, auto Q2>
concept invalid_comparison = requires {
  requires !requires { Q1 == Q2; };
  requires !requires { Q2 == Q1; };
};

static_assert(invalid_comparison<1 * hertz, 1 * becquerel>);
static_assert(invalid_comparison<1 * radian, 1 * steradian>);
static_assert(invalid_comparison<1 * radian, 1 * bit>);
static_assert(invalid_comparison<frequency(1 * hertz), activity(1 * becquerel)>);
static_assert(invalid_comparison<angular_measure(1 * radian), solid_angular_measure(1 * steradian)>);
static_assert(invalid_comparison<angular_measure(1 * radian), storage_capacity(1 * bit)>);
static_assert(invalid_comparison<1 * radian, dimensionless(1 * one)>);
static_assert(invalid_comparison<angular_measure(1 * radian), dimensionless(1 * one)>);

// make_reference
static_assert(is_of_type<make_reference(length, metre), reference<length_, metre_>>);
static_assert(is_of_type<make_reference(width, metre), reference<width_, metre_>>);
static_assert(is_of_type<make_reference(kind_of<length>, metre), metre_>);
static_assert(is_of_type<make_reference(get_quantity_spec(metre), metre), metre_>);
static_assert(is_of_type<make_reference(get_quantity_spec(hertz), hertz), hertz_>);
static_assert(is_of_type<make_reference(kind_of<frequency>, hertz), hertz_>);
static_assert(is_of_type<make_reference(get_quantity_spec(watt), watt), watt_>);
static_assert(is_of_type<make_reference(kind_of<power>, watt), reference<kind_of_<power_>, watt_>>);

}  // namespace
