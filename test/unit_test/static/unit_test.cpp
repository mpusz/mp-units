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
#include <units/base_dimension.h>
#include <units/bits/equivalent.h>
#include <units/bits/external/downcasting.h>
#include <units/derived_dimension.h>
#include <units/isq/si/prefixes.h>
#include <units/unit.h>

namespace {

using namespace units;
using namespace units::isq;

struct metre : named_unit<metre, "m"> {};
struct centimetre : prefixed_unit<centimetre, si::centi, metre> {};
struct kilometre : prefixed_unit<kilometre, si::kilo, metre> {};
struct yard : named_scaled_unit<yard, "yd", as_magnitude<ratio(9'144, 1, -4)>(), metre> {};
struct foot : named_scaled_unit<foot, "ft", as_magnitude<ratio(1, 3)>(), yard> {};
struct dim_length : base_dimension<"length", metre> {};

struct second : named_unit<second, "s"> {};
struct hour : named_scaled_unit<hour, "h", as_magnitude<ratio(36, 1, 2)>(), second> {};
struct dim_time : base_dimension<"time", second> {};

struct kelvin : named_unit<kelvin, "K"> {};

#if !UNITS_COMP_MSVC
static_assert([]<Prefix P>(P) {
  return !requires { typename prefixed_unit<struct kilokilometre, P, kilometre>; };
}(si::kilo{}));  // no prefix allowed
#endif

struct metre_per_second : derived_unit<metre_per_second> {};
struct dim_speed :
    derived_dimension<dim_speed, metre_per_second, units::exponent<dim_length, 1>, units::exponent<dim_time, -1>> {};
struct kilometre_per_hour : derived_scaled_unit<kilometre_per_hour, dim_speed, kilometre, hour> {};

static_assert(equivalent<metre::named_unit, metre>);
static_assert(equivalent<metre::scaled_unit, metre>);
static_assert(compare<downcast<scaled_unit<as_magnitude<1>(), metre>>, metre>);
static_assert(compare<downcast<scaled_unit<as_magnitude<ratio(1, 1, -2)>(), metre>>, centimetre>);
static_assert(compare<downcast<scaled_unit<yard::mag, metre>>, yard>);
static_assert(compare<downcast<scaled_unit<yard::mag / as_magnitude<3>(), metre>>, foot>);
static_assert(compare<downcast<scaled_unit<kilometre::mag / hour::mag, metre_per_second>>, kilometre_per_hour>);

static_assert(centimetre::symbol == "cm");
static_assert(kilometre::symbol == "km");
static_assert(kilometre_per_hour::symbol == "km/h");

}  // namespace
