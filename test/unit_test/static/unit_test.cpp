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

// clang-format off
// base units
inline constexpr struct second_ : named_unit<"s"> {} second;
inline constexpr struct metre_ : named_unit<"m"> {} metre;
inline constexpr struct gram_ : named_unit<"g"> {} gram;
inline constexpr struct kilogram_ : decltype(kilo<gram>) {} kilogram;

// derived named units
inline constexpr struct radian_ : named_unit<"rad", metre / metre> {} radian;
inline constexpr struct steradian_ : named_unit<"sr", square<metre> / square<metre>> {} steradian;
inline constexpr struct hertz_ : named_unit<"Hz", 1 / second> {} hertz;
inline constexpr struct becquerel : named_unit<"Bq", 1 / second> {} becquerel;
inline constexpr struct newton_ : named_unit<"N", kilogram * metre / square<second>> {} newton;
inline constexpr struct pascal_ : named_unit<"Pa", newton / square<metre>> {} pascal;
inline constexpr struct joule_ : named_unit<"J", newton * metre> {} joule;
inline constexpr struct watt_ : named_unit<"W", joule / second> {} watt;

inline constexpr struct minute_ : named_unit<"min", mag<60> * second> {} minute;
inline constexpr struct hour_ : named_unit<"h", mag<60> * minute> {} hour;
inline constexpr struct day_ : named_unit<"d", mag<24> * hour> {} day;
inline constexpr struct astronomical_unit_ : named_unit<"au", mag<149'597'870'700> * metre> {} astronomical_unit;
inline constexpr struct degree_ : named_unit<basic_symbol_text{"°", "deg"}, mag_pi / mag<180> * radian> {} degree;
inline constexpr struct are_ : named_unit<"a", square<deca<metre>>> {} are;
inline constexpr struct hectare_ : decltype(hecto<are>) {} hectare;
inline constexpr struct litre_ : named_unit<"l", cubic<deci<metre>>> {} litre;
inline constexpr struct tonne_ : named_unit<"t", mag<1000> * kilogram> {} tonne;
inline constexpr struct dalton_ : named_unit<"Da", mag<ratio{16'605'390'666'050, 10'000'000'000'000}> * mag_power<10, -27> * kilogram> {} dalton;
inline constexpr struct electronvolt_ : named_unit<"eV", mag<ratio{1'602'176'634, 1'000'000'000}> * mag_power<10, -19> * joule> {} electronvolt;

inline constexpr struct kilometre_ : decltype(kilo<metre>) {} kilometre;
// clang-format on

}

// concepts verification
static_assert(Unit<metre_>);
static_assert(Unit<kilogram_>);
static_assert(Unit<hertz_>);
static_assert(Unit<newton_>);
static_assert(Unit<minute_>);
static_assert(Unit<decltype(kilo<gram>)>);
static_assert(Unit<decltype(square<metre>)>);
static_assert(Unit<decltype(cubic<metre>)>);
static_assert(Unit<decltype(mag<60> * second)>);
static_assert(Unit<kilometre_>);

static_assert(NamedUnit<metre_>);
static_assert(NamedUnit<kilogram_>);
static_assert(NamedUnit<hertz_>);
static_assert(NamedUnit<newton_>);
static_assert(NamedUnit<minute_>);
static_assert(!NamedUnit<decltype(kilo<gram>)>);
static_assert(!NamedUnit<decltype(square<metre>)>);
static_assert(!NamedUnit<decltype(cubic<metre>)>);
static_assert(!NamedUnit<decltype(mag<60> * second)>);
static_assert(!NamedUnit<kilometre_>);


static_assert(kilo<metre> == kilometre);
static_assert(mag<1000> * metre == kilo<metre>);
static_assert(mag<1000> * metre == kilometre);
static_assert(equivalent<kilo<metre>, kilometre>);
static_assert(equivalent<mag<1000> * metre, kilo<metre>>);
static_assert(equivalent<mag<1000> * metre, kilometre>);

static_assert(metre != kilometre);
static_assert(mag<100> * metre != kilometre);
static_assert(milli<metre> != kilometre);
static_assert(!equivalent<metre, kilometre>);
static_assert(!equivalent<mag<100> * metre, kilometre>);
static_assert(!equivalent<milli<metre>, kilometre>);

static_assert(1 / second != hertz);
static_assert(becquerel != hertz);
static_assert(equivalent<1 / second, hertz>);
static_assert(!equivalent<becquerel, hertz>);

using namespace units;
using namespace units::isq;

struct metre : named_unit<metre, "m"> {};
struct centimetre : prefixed_unit<centimetre, si::centi, metre> {};
struct kilometre : prefixed_unit<kilometre, si::kilo, metre> {};
struct yard : named_scaled_unit<yard, "yd", mag<ratio{9'144, 10'000}>(), metre> {};
struct foot : named_scaled_unit<foot, "ft", mag<ratio(1, 3)>(), yard> {};
struct dim_length : base_dimension<"length", metre> {};

struct second : named_unit<second, "s"> {};
struct hour : named_scaled_unit<hour, "h", mag<3600>(), second> {};
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
static_assert(compare<downcast<scaled_unit<mag<1>(), metre>>, metre>);
static_assert(compare<downcast<scaled_unit<mag<ratio(1, 100)>(), metre>>, centimetre>);
static_assert(compare<downcast<scaled_unit<yard::mag, metre>>, yard>);
static_assert(compare<downcast<scaled_unit<yard::mag / mag<3>(), metre>>, foot>);
static_assert(compare<downcast<scaled_unit<kilometre::mag / hour::mag, metre_per_second>>, kilometre_per_hour>);

static_assert(centimetre::symbol == "cm");
static_assert(kilometre::symbol == "km");
static_assert(kilometre_per_hour::symbol == "km/h");

}  // namespace
