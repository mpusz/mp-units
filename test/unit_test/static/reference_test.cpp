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
#include <units/dimension.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/si/prefixes.h>
#include <units/unit.h>

namespace {

using namespace units;
using namespace units::detail;

using dimension_one_ = struct dimension_one;
using one_ = struct one;

// base dimensions
BASE_DIMENSION_(length, "L");
BASE_DIMENSION_(time, "T");
BASE_DIMENSION_(mass, "M");

DERIVED_DIMENSION_(frequency, decltype(1 / time));
DERIVED_DIMENSION_(action, decltype(1 / time));
DERIVED_DIMENSION_(area, decltype(length * length));
DERIVED_DIMENSION_(volume, decltype(area * length));
DERIVED_DIMENSION_(speed, decltype(length / time));
DERIVED_DIMENSION_(acceleration, decltype(speed / time));
DERIVED_DIMENSION_(force, decltype(mass * acceleration));
DERIVED_DIMENSION_(moment_of_force, decltype(length * force));
DERIVED_DIMENSION_(torque, decltype(moment_of_force));
DERIVED_DIMENSION_(power, decltype(force * speed));
DERIVED_DIMENSION_(efficiency, decltype(power / power));
DERIVED_DIMENSION_(energy, decltype(force * length));

// clang-format off
// base units
inline constexpr struct second_ : named_unit<"s", time> {} second;
inline constexpr struct metre_ : named_unit<"m", length> {} metre;
inline constexpr struct gram_ : named_unit<"g", mass> {} gram;
inline constexpr struct kilogram_ : decltype(si::kilo<gram>) {} kilogram;

namespace nu {
// hypothetical natural system of units for c=1

inline constexpr struct second_ : named_unit<"s"> {} second;
inline constexpr struct minute_ : named_unit<"min", mag<60> * second> {} minute;
inline constexpr struct gram_ : named_unit<"g"> {} gram;
inline constexpr struct kilogram_ : decltype(si::kilo<gram>) {} kilogram;

inline constexpr struct time : system_reference<time_{}, second> {} time;
inline constexpr struct length : system_reference<length_{}, second> {} length;
inline constexpr struct speed : system_reference<speed_{}, second / second> {} speed;
inline constexpr struct force : system_reference<force_{}, kilogram / second> {} force;

}

// derived named units
inline constexpr struct radian_ : named_unit<"rad", metre / metre> {} radian;
inline constexpr struct steradian_ : named_unit<"sr", square<metre> / square<metre>> {} steradian;
inline constexpr struct hertz_ : named_unit<"Hz", 1 / second> {} hertz;
inline constexpr struct becquerel_ : named_unit<"Bq", 1 / second> {} becquerel;
inline constexpr struct newton_ : named_unit<"N", kilogram * metre / square<second>> {} newton;
inline constexpr struct pascal_ : named_unit<"Pa", newton / square<metre>> {} pascal;
inline constexpr struct joule_ : named_unit<"J", newton * metre> {} joule;
inline constexpr struct watt_ : named_unit<"W", joule / second> {} watt;

inline constexpr struct minute_ : named_unit<"min", mag<60> * second> {} minute;
inline constexpr struct hour_ : named_unit<"h", mag<60> * minute> {} hour;
inline constexpr struct kilometre_ : decltype(si::kilo<metre>) {} kilometre;
// clang-format on

// Named quantity/dimension and unit
static_assert(is_same_v<decltype(5 * power[watt]), quantity<reference<power_, watt_>{}, int>>);

// Named quantity/dimension and derived (unnamed) unit
static_assert(is_same_v<decltype(5 * speed[metre / second]),
                        quantity<reference<speed_, derived_unit<metre_, per<second_>>>{}, int>>);

// Derived (unnamed) quantity/dimension and derived (unnamed) unit
static_assert(
  is_same_v<decltype(10 * length[metre] / (2 * time[second])),
            quantity<reference<derived_dimension<length_, per<time_>>, derived_unit<metre_, per<second_>>>{}, int>>);

// Base quantity as a result of dimensional transformation
static_assert(
  is_same_v<decltype(5 * speed[metre / second] * (5 * time[second])), quantity<reference<length_, metre_>{}, int>>);

// dimension_one
static_assert(is_same_v<decltype(20 * speed[metre / second] / (10 * length[metre]) * (5 * time[second])),
                        quantity<reference<dimension_one_, one_>{}, int>>);

template<auto s>
concept invalid_operations = requires {
                               requires !requires { 2 / s; };
                               requires !requires { s / 2; };
                               requires !requires { s * 2; };
                               requires !requires { s + 2; };
                               requires !requires { 2 + s; };
                               requires !requires { s + s; };
                               requires !requires { s - 2; };
                               requires !requires { 2 - s; };
                               requires !requires { s - s; };
                               requires !requires { s == s; };
                               requires !requires { s < s; };
                               requires !requires { s + 1 * time[second]; };
                               requires !requires { s - 1 * time[second]; };
                               requires !requires { s * 1 * time[second]; };
                               requires !requires { s / 1 * time[second]; };
                               requires !requires { s == 1 * time[second]; };
                               requires !requires { s < 1 * time[second]; };
                               requires !requires { 1 * time[second] + s; };
                               requires !requires { 1 * time[second] - s; };
                               requires !requires { 1 * time[second] * s; };
                               requires !requires { 1 * time[second] / s; };
                               requires !requires { 1 * time[second] == s; };
                               requires !requires { 1 * time[second] < s; };
                             };
static_assert(invalid_operations<time[second]>);

static_assert(
  is_same_v<decltype(2 * length[metre] / (1 * time[second])),
            quantity<reference<derived_dimension<length_, per<time_>>, derived_unit<metre_, per<second_>>>{}, int>>);
static_assert(
  is_same_v<decltype(2 * (length[metre] / time[second])),
            quantity<reference<derived_dimension<length_, per<time_>>, derived_unit<metre_, per<second_>>>{}, int>>);
static_assert(is_same_v<decltype(2 * (speed[metre / second])),
                        quantity<reference<speed_, derived_unit<metre_, per<second_>>>{}, int>>);

constexpr auto m_per_s = speed[metre / second];
static_assert(is_same_v<decltype(2 * m_per_s), quantity<reference<speed_, derived_unit<metre_, per<second_>>>{}, int>>);

static_assert(
  is_same_v<decltype(120 * length[kilometre] / (2 * time[hour])),
            quantity<reference<derived_dimension<length_, per<time_>>, derived_unit<kilometre_, per<hour_>>>{}, int>>);
static_assert(120 * length[kilometre] / (2 * time[hour]) == 60 * speed[kilometre / hour]);
static_assert(
  is_same_v<decltype([] {
              const auto distance = 120;
              const auto duration = 2;
              return distance * length[kilometre] / (duration * time[hour]);
            }()),
            quantity<reference<derived_dimension<length_, per<time_>>, derived_unit<kilometre_, per<hour_>>>{}, int>>);
static_assert(
  is_same_v<
    decltype(std::int64_t{120} * length[kilometre] / (2 * time[hour])),
    quantity<reference<derived_dimension<length_, per<time_>>, derived_unit<kilometre_, per<hour_>>>{}, std::int64_t>>);
static_assert(
  is_same_v<
    decltype(120.L * length[kilometre] / (2 * time[hour])),
    quantity<reference<derived_dimension<length_, per<time_>>, derived_unit<kilometre_, per<hour_>>>{}, long double>>);

static_assert(is_same_v<decltype(1. / 4 * area[square<metre>]), decltype(1. * area[square<metre>] / 4)>);
static_assert(1. / 4 * area[square<metre>] == 1. * area[square<metre>] / 4);

// Natural Units
static_assert(is_same_v<decltype(42 * nu::time[nu::second]), quantity<reference<time_, nu::second_>{}, int>>);
static_assert(is_same_v<decltype(42 * nu::time[nu::minute]), quantity<reference<time_, nu::minute_>{}, int>>);
static_assert(is_same_v<decltype(42 * nu::length[nu::second]), quantity<reference<length_, nu::second_>{}, int>>);
static_assert(is_same_v<decltype(42 * nu::length[nu::minute]), quantity<reference<length_, nu::minute_>{}, int>>);
static_assert(is_same_v<decltype(42 * (nu::length[nu::second] / nu::time[nu::second])),
                        quantity<reference<derived_dimension<length_, per<time_>>, one_>{}, int>>);
static_assert(is_same_v<decltype(42 * nu::length[nu::second] / (42 * nu::time[nu::second])),
                        quantity<reference<derived_dimension<length_, per<time_>>, one_>{}, int>>);
static_assert(is_same_v<decltype(42 * nu::speed[nu::second / nu::second]), quantity<reference<speed_, one_>{}, int>>);
static_assert(is_same_v<decltype(42 * nu::speed[one]), quantity<reference<speed_, one_>{}, int>>);
static_assert(is_same_v<decltype(42 * mass[kilogram] * (1 * nu::length[nu::second]) / (1 * nu::time[nu::second])),
                        quantity<reference<derived_dimension<length_, mass_, per<time_>>, kilogram_>{}, int>>);

template<auto dim, auto unit>
concept invalid_nu_unit = !requires { dim[unit]; };

static_assert(invalid_nu_unit<time, nu::second>);
static_assert(invalid_nu_unit<nu::time, second>);
static_assert(invalid_nu_unit<length / time, nu::second / nu::second>);
static_assert(invalid_nu_unit<speed, nu::second / nu::second>);
static_assert(invalid_nu_unit<speed, nu::second / second>);
static_assert(invalid_nu_unit<mass * length / time, kilogram * nu::second / nu::second>);
static_assert(invalid_nu_unit<force, kilogram * nu::second / nu::second>);

}  // namespace
