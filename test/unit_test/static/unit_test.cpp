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

#include "units/unit.h"
#include "units/bits/equivalent.h"
#include "units/physical/si/prefixes.h"

namespace {

using namespace units;
using namespace units::physical;

template<typename T, typename U>
inline constexpr bool compare = DOWNCAST_MODE != 0 ? std::is_same_v<T, U> : (std::is_same_v<T, U> || units::equivalent<T, U>);

struct metre : named_unit<metre, "m", si::prefix> {};
struct centimetre : prefixed_unit<centimetre, si::centi, metre> {};
struct kilometre : prefixed_unit<kilometre, si::kilo, metre> {};
struct yard : named_scaled_unit<yard, "yd", no_prefix, ratio(9'144, 1, -4), metre> {};
struct foot : named_scaled_unit<foot, "ft", no_prefix, ratio(1, 3), yard> {};
struct dim_length : base_dimension<"length", metre> {};

struct second : named_unit<second, "s", si::prefix> {};
struct hour : named_scaled_unit<hour, "h", no_prefix, ratio(36, 1, 2), second> {};
struct dim_time : base_dimension<"time", second> {};

struct kelvin : named_unit<kelvin, "K", no_prefix> {};
// struct kilokelvin : prefixed_unit<kilokelvin, si::kilo, kelvin> {};  // should not compile (prefix not allowed for this reference unit)

struct metre_per_second : unit<metre_per_second> {};
struct dim_speed : derived_dimension<dim_speed, metre_per_second, units::exp<dim_length, 1>, units::exp<dim_time, -1>> {};
struct kilometre_per_hour : deduced_unit<kilometre_per_hour, dim_speed, kilometre, hour> {};

static_assert(compare<downcast<scaled_unit<ratio(1), metre>>, metre>);
static_assert(compare<downcast<scaled_unit<ratio(1, 1, -2), metre>>, centimetre>);
static_assert(compare<downcast<scaled_unit<ratio(yard::ratio.num, yard::ratio.den, yard::ratio.exp), metre>>, yard>);
static_assert(compare<downcast<scaled_unit<yard::ratio * ratio(1, 3), metre>>, foot>);
static_assert(compare<downcast<scaled_unit<kilometre::ratio / hour::ratio, metre_per_second>>, kilometre_per_hour>);

static_assert(centimetre::symbol == "cm");
static_assert(kilometre::symbol == "km");
static_assert(kilometre_per_hour::symbol == "km/h");

}  // namespace
