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
#include "units/physical/si/prefixes.h"

namespace {

using namespace units;

struct metre : named_unit<metre, "m", si::prefix> {};
struct centimetre : prefixed_unit<centimetre, si::centi, metre> {};
struct kilometre : prefixed_unit<kilometre, si::kilo, metre> {};
struct yard : named_scaled_unit<yard, "yd", no_prefix, ratio<9'144, 10'000>, metre> {};
struct foot : named_scaled_unit<foot, "ft", no_prefix, ratio<1, 3>, yard> {};
struct dim_length : base_dimension<"length", metre> {};

struct second : named_unit<second, "s", si::prefix> {};
struct hour : named_scaled_unit<hour, "h", no_prefix, ratio<3600>, second> {};
struct dim_time : base_dimension<"time", second> {};

struct kelvin : named_unit<kelvin, "K", no_prefix> {};
// struct kilokelvin : prefixed_unit<kilokelvin, si::kilo, kelvin> {};  // should not compile (prefix not allowed for this reference unit)

struct metre_per_second : unit<metre_per_second> {};
struct dim_velocity : derived_dimension<dim_velocity, metre_per_second, exp<dim_length, 1>, exp<dim_time, -1>> {};
struct kilometre_per_hour : deduced_unit<kilometre_per_hour, dim_velocity, kilometre, hour> {};

static_assert(std::is_same_v<downcast<scaled_unit<metre, ratio<1>>>, metre>);
static_assert(std::is_same_v<downcast<scaled_unit<metre, ratio<1, 100>>>, centimetre>);
static_assert(std::is_same_v<downcast<scaled_unit<metre, ratio<yard::ratio::num, yard::ratio::den>>>, yard>);
static_assert(std::is_same_v<downcast<scaled_unit<metre, ratio_multiply<typename yard::ratio, ratio<1, 3>>>>, foot>);
static_assert(std::is_same_v<downcast<scaled_unit<metre_per_second, ratio_divide<typename kilometre::ratio, typename hour::ratio>>>, kilometre_per_hour>);

static_assert(centimetre::symbol == "cm");
static_assert(kilometre::symbol == "km");
static_assert(kilometre_per_hour::symbol == "km/h");

}  // namespace
