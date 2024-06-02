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
#include <mp-units/systems/si/prefixes.h>
#include <type_traits>

namespace {

using namespace mp_units;
using namespace mp_units::detail;

using one_ = struct one;
using percent_ = struct percent;

// base dimensions
// clang-format off
inline constexpr struct dim_length_ : base_dimension<"L"> {} dim_length;
inline constexpr struct dim_mass_ : base_dimension<"M"> {} dim_mass;
inline constexpr struct dim_time_ : base_dimension<"T"> {} dim_time;
inline constexpr struct dim_thermodynamic_temperature_ : base_dimension<symbol_text{u8"Θ", "O"}> {} dim_thermodynamic_temperature;

// quantities specification
QUANTITY_SPEC_(length, dim_length);
QUANTITY_SPEC_(mass, dim_mass);
QUANTITY_SPEC_(time, dim_time);
QUANTITY_SPEC_(thermodynamic_temperature, dim_thermodynamic_temperature);

// base units
inline constexpr struct second_ : named_unit<"s", kind_of<time>> {} second;
inline constexpr struct metre_ : named_unit<"m", kind_of<length>> {} metre;
inline constexpr struct gram_ : named_unit<"g", kind_of<mass>> {} gram;
inline constexpr struct kilogram_ : decltype(si::kilo<gram>) {} kilogram;
inline constexpr struct kelvin_ : named_unit<"K", kind_of<thermodynamic_temperature>> {} kelvin;

// hypothetical natural units for c=1
inline constexpr struct nu_second_ : named_unit<"s"> {} nu_second;

// derived named units
inline constexpr struct radian_ : named_unit<"rad", metre / metre> {} radian;
inline constexpr struct steradian_ : named_unit<"sr", square(metre) / square(metre)> {} steradian;
inline constexpr struct hertz_ : named_unit<"Hz", inverse(second)> {} hertz;
inline constexpr struct becquerel_ : named_unit<"Bq", inverse(second)> {} becquerel;
inline constexpr struct newton_ : named_unit<"N", kilogram * metre / square(second)> {} newton;
inline constexpr struct pascal_ : named_unit<"Pa", newton / square(metre)> {} pascal;
inline constexpr struct joule_ : named_unit<"J", newton * metre> {} joule;
inline constexpr struct watt_ : named_unit<"W", joule / second> {} watt;
inline constexpr struct degree_Celsius_ : named_unit<symbol_text{u8"°C", "`C"}, kelvin> {} degree_Celsius;

inline constexpr struct minute_ : named_unit<"min", mag<60> * second> {} minute;
inline constexpr struct hour_ : named_unit<"h", mag<60> * minute> {} hour;
inline constexpr struct degree_ : named_unit<symbol_text{u8"°", "deg"}, mag_pi / mag<180> * radian> {} degree;

inline constexpr struct yard_ : named_unit<"yd", mag_ratio<9'144, 10'000> * metre> {} yard;
inline constexpr struct mile_ : named_unit<"mi", mag<1760> * yard> {} mile;

inline constexpr struct kilometre_ : decltype(si::kilo<metre>) {} kilometre;
inline constexpr struct kilojoule_ : decltype(si::kilo<joule>) {} kilojoule;

// physical constant units
inline constexpr struct standard_gravity_ : named_unit<symbol_text{u8"g₀", "g_0"}, mag_ratio<980'665, 100'000> * metre / square(second)> {} standard_gravity;
inline constexpr struct speed_of_light_in_vacuum_ : named_unit<"c", mag<299'792'458> * metre / second> {} speed_of_light_in_vacuum;

// clang-format on

// concepts verification
static_assert(Unit<metre_>);
static_assert(Unit<second_>);
static_assert(Unit<nu_second_>);
static_assert(Unit<kilogram_>);
static_assert(Unit<hertz_>);
static_assert(Unit<newton_>);
static_assert(Unit<minute_>);
static_assert(Unit<decltype(si::kilo<gram>)>);
static_assert(Unit<decltype(square(metre))>);
static_assert(Unit<decltype(cubic(metre))>);
static_assert(Unit<decltype(mag<60> * second)>);
static_assert(Unit<decltype(second * second)>);
static_assert(Unit<decltype(nu_second * nu_second)>);
static_assert(Unit<decltype(metre / second)>);
static_assert(Unit<decltype(nu_second / nu_second)>);
static_assert(Unit<kilometre_>);

static_assert(detail::NamedUnit<metre_>);
static_assert(detail::NamedUnit<hertz_>);
static_assert(detail::NamedUnit<newton_>);
static_assert(detail::NamedUnit<minute_>);
static_assert(detail::NamedUnit<radian_>);
static_assert(!detail::NamedUnit<kilogram_>);
static_assert(!detail::NamedUnit<kilojoule_>);
static_assert(!detail::NamedUnit<decltype(si::kilo<gram>)>);
static_assert(!detail::NamedUnit<decltype(square(metre))>);
static_assert(!detail::NamedUnit<decltype(cubic(metre))>);
static_assert(!detail::NamedUnit<decltype(mag<60> * second)>);
static_assert(!detail::NamedUnit<kilometre_>);

// named unit
static_assert(is_of_type<metre, metre_>);
static_assert(is_of_type<get_canonical_unit(metre).reference_unit, metre_>);
static_assert(get_canonical_unit(metre).mag == mag<1>);
static_assert(convertible(metre, metre));
static_assert(!convertible(metre, second));
static_assert(metre == metre);
static_assert(metre != second);

static_assert(is_of_type<degree_Celsius, degree_Celsius_>);
static_assert(is_of_type<get_canonical_unit(degree_Celsius).reference_unit, kelvin_>);
static_assert(get_canonical_unit(degree_Celsius).mag == mag<1>);
static_assert(convertible(degree_Celsius, kelvin));
static_assert(degree_Celsius == kelvin);

static_assert(is_of_type<radian, radian_>);
static_assert(is_of_type<get_canonical_unit(radian).reference_unit, one_>);
static_assert(get_canonical_unit(radian).mag == mag<1>);

static_assert(is_of_type<degree, degree_>);
static_assert(is_of_type<get_canonical_unit(degree).reference_unit, one_>);
static_assert(get_canonical_unit(degree).mag == mag_pi / mag<180>);
static_assert(convertible(radian, degree));
static_assert(radian != degree);

static_assert(is_of_type<steradian, steradian_>);
static_assert(is_of_type<get_canonical_unit(steradian).reference_unit, one_>);
static_assert(get_canonical_unit(steradian).mag == mag<1>);
static_assert(convertible(radian, steradian));  // !!!
static_assert(radian == steradian);             // !!!

static_assert(is_of_type<minute, minute_>);
static_assert(is_of_type<get_canonical_unit(minute).reference_unit, second_>);
static_assert(get_canonical_unit(minute).mag == mag<60>);
static_assert(convertible(minute, second));
static_assert(minute != second);

static_assert(is_of_type<hour, hour_>);
static_assert(is_of_type<get_canonical_unit(hour).reference_unit, second_>);
static_assert(get_canonical_unit(hour).mag == mag<3600>);
static_assert(convertible(hour, second));

static_assert(convertible(hour, minute));
static_assert(convertible(hour, hour));
static_assert(hour != second);
static_assert(hour != minute);
static_assert(hour == hour);

static_assert(is_of_type<newton, newton_>);
static_assert(
  is_of_type<get_canonical_unit(newton).reference_unit, derived_unit<gram_, metre_, per<power<second_, 2>>>>);
static_assert(get_canonical_unit(newton).mag == mag<1000>);  // !!! (because of kilogram)
static_assert(convertible(newton, newton));
static_assert(newton == newton);

static_assert(is_of_type<joule, joule_>);
static_assert(
  is_of_type<get_canonical_unit(joule).reference_unit, derived_unit<gram_, power<metre_, 2>, per<power<second_, 2>>>>);
static_assert(get_canonical_unit(joule).mag == mag<1000>);  // !!! (because of kilogram)
static_assert(convertible(joule, joule));
static_assert(joule == joule);
static_assert(joule != newton);

static_assert(is_of_type<nu_second / nu_second, one_>);

// constant_unit
static_assert(is_of_type<standard_gravity, standard_gravity_>);
static_assert(
  is_of_type<get_canonical_unit(standard_gravity).reference_unit, derived_unit<metre_, per<power<second_, 2>>>>);
static_assert(get_canonical_unit(standard_gravity).mag == mag_ratio<980'665, 100'000>);
static_assert(convertible(standard_gravity, standard_gravity));
static_assert(convertible(standard_gravity, metre / square(second)));
static_assert(standard_gravity == standard_gravity);
static_assert(standard_gravity != metre / square(second));  // magnitude is different
static_assert(standard_gravity.symbol == symbol_text{u8"g₀", "g_0"});

// prefixed_unit
static_assert(is_of_type<kilometre, kilometre_>);
static_assert(is_of_type<get_canonical_unit(kilometre).reference_unit, metre_>);
static_assert(get_canonical_unit(kilometre).mag == mag<1000>);
static_assert(convertible(kilometre, metre));
static_assert(kilometre != metre);
static_assert(kilometre.symbol == "km");

static_assert(is_of_type<kilojoule, kilojoule_>);
static_assert(is_of_type<get_canonical_unit(kilojoule).reference_unit,
                         derived_unit<gram_, power<metre_, 2>, per<power<second_, 2>>>>);
static_assert(get_canonical_unit(kilojoule).mag == mag<1'000'000>);
static_assert(convertible(kilojoule, joule));
static_assert(kilojoule != joule);
static_assert(kilojoule.symbol == "kJ");

static_assert(is_of_type<si::kilo<metre>, si::kilo_<metre_>>);
static_assert(is_of_type<si::kilo<joule>, si::kilo_<joule_>>);

static_assert(is_of_type<kilometre * metre, derived_unit<kilometre_, metre_>>);       // !!!
static_assert(is_of_type<kilometre / metre, derived_unit<kilometre_, per<metre_>>>);  // !!!


// prefixes
static_assert(si::quecto<metre>.symbol == "qm");
static_assert(si::ronto<metre>.symbol == "rm");
static_assert(si::yocto<metre>.symbol == "ym");
static_assert(si::zepto<metre>.symbol == "zm");
static_assert(si::atto<metre>.symbol == "am");
static_assert(si::femto<metre>.symbol == "fm");
static_assert(si::pico<metre>.symbol == "pm");
static_assert(si::nano<metre>.symbol == "nm");
static_assert(si::micro<metre>.symbol == symbol_text{u8"µm", "um"});
static_assert(si::milli<metre>.symbol == "mm");
static_assert(si::centi<metre>.symbol == "cm");
static_assert(si::deci<metre>.symbol == "dm");
static_assert(si::deca<metre>.symbol == "dam");
static_assert(si::hecto<metre>.symbol == "hm");
static_assert(si::kilo<metre>.symbol == "km");
static_assert(si::mega<metre>.symbol == "Mm");
static_assert(si::giga<metre>.symbol == "Gm");
static_assert(si::tera<metre>.symbol == "Tm");
static_assert(si::peta<metre>.symbol == "Pm");
static_assert(si::exa<metre>.symbol == "Em");
static_assert(si::zetta<metre>.symbol == "Zm");
static_assert(si::yotta<metre>.symbol == "Ym");
static_assert(si::ronna<metre>.symbol == "Rm");
static_assert(si::quetta<metre>.symbol == "Qm");

// scaled_unit
constexpr auto m_1 = mag<1> * metre;
static_assert(is_of_type<m_1, metre_>);
static_assert(is_of_type<get_canonical_unit(m_1).reference_unit, metre_>);
static_assert(get_canonical_unit(m_1).mag == mag<1>);

constexpr auto m_2 = mag<2> * metre;
static_assert(is_of_type<m_2, scaled_unit<mag<2>, metre_>>);
static_assert(is_of_type<get_canonical_unit(m_2).reference_unit, metre_>);
static_assert(get_canonical_unit(m_2).mag == mag<2>);

constexpr auto km_2 = mag<2> * kilometre;
static_assert(is_of_type<km_2, scaled_unit<mag<2>, kilometre_>>);
static_assert(is_of_type<get_canonical_unit(km_2).reference_unit, metre_>);
static_assert(get_canonical_unit(km_2).mag == mag<2000>);

constexpr auto kJ_42 = mag<42> * si::kilo<joule>;
static_assert(is_of_type<kJ_42, scaled_unit<mag<42>, si::kilo_<joule_>>>);
static_assert(
  is_of_type<get_canonical_unit(kJ_42).reference_unit, derived_unit<gram_, power<metre_, 2>, per<power<second_, 2>>>>);
static_assert(get_canonical_unit(kJ_42).mag == mag<42'000'000>);


// derived unit expression template syntax verification
static_assert(is_of_type<inverse(second), derived_unit<one_, per<second_>>>);
static_assert(is_of_type<one / (inverse(second)), second_>);

static_assert(is_of_type<one * second, second_>);
static_assert(is_of_type<second * one, second_>);
static_assert(is_of_type<one * inverse(second), derived_unit<one_, per<second_>>>);
static_assert(is_of_type<one / second * one, derived_unit<one_, per<second_>>>);

static_assert(is_of_type<metre * second, derived_unit<metre_, second_>>);
static_assert(is_of_type<metre * metre, derived_unit<power<metre_, 2>>>);
static_assert(is_of_type<square(metre), derived_unit<power<metre_, 2>>>);
static_assert(is_of_type<cubic(metre), derived_unit<power<metre_, 3>>>);
static_assert(is_of_type<square(metre) * metre, derived_unit<power<metre_, 3>>>);
static_assert(is_of_type<metre * square(metre), derived_unit<power<metre_, 3>>>);
static_assert(is_of_type<square(metre) / metre, metre_>);
static_assert(is_of_type<cubic(metre) / metre, derived_unit<power<metre_, 2>>>);
static_assert(is_of_type<cubic(metre) / square(metre), metre_>);

static_assert(is_of_type<metre / second, derived_unit<metre_, per<second_>>>);
static_assert(is_of_type<metre / square(second), derived_unit<metre_, per<power<second_, 2>>>>);
static_assert(is_of_type<metre / square(second) / second, derived_unit<metre_, per<power<second_, 3>>>>);

static_assert(is_of_type<metre * metre * second, derived_unit<power<metre_, 2>, second_>>);
static_assert(is_of_type<metre * second * metre, derived_unit<power<metre_, 2>, second_>>);

static_assert(is_of_type<metre*(second* metre), derived_unit<power<metre_, 2>, second_>>);
static_assert(is_of_type<second*(metre* metre), derived_unit<power<metre_, 2>, second_>>);

static_assert(is_of_type<one / second * metre, derived_unit<metre_, per<second_>>>);
static_assert(is_of_type<one / second * second, one_>);

static_assert(is_of_type<second / one, second_>);
static_assert(is_of_type<one / second / one, derived_unit<one_, per<second_>>>);

static_assert(is_of_type<metre / second * second, metre_>);
static_assert(is_of_type<one / second * inverse(second), derived_unit<one_, per<power<second_, 2>>>>);
static_assert(is_of_type<one / (second * second), derived_unit<one_, per<power<second_, 2>>>>);
static_assert(is_of_type<one / inverse(second* second), derived_unit<power<second_, 2>>>);

static_assert(is_of_type<metre / second * inverse(second), derived_unit<metre_, per<power<second_, 2>>>>);
static_assert(is_of_type<metre / second*(metre / second), derived_unit<power<metre_, 2>, per<power<second_, 2>>>>);
static_assert(is_of_type<metre / second*(second / metre), one_>);

static_assert(is_of_type<watt / joule, derived_unit<watt_, per<joule_>>>);
static_assert(is_of_type<joule / watt, derived_unit<joule_, per<watt_>>>);

static_assert(is_of_type<one / second, derived_unit<one_, per<second_>>>);
static_assert(is_of_type<one / inverse(second), second_>);
static_assert(is_of_type<one / inverse(second), second_>);

static_assert(is_of_type<inverse(pascal), derived_unit<one_, per<pascal_>>>);
static_assert(is_of_type<inverse(gram) * metre * square(second), derived_unit<metre_, power<second_, 2>, per<gram_>>>);
static_assert(
  is_of_type<inverse(gram / (metre * square(second))), derived_unit<metre_, power<second_, 2>, per<gram_>>>);
static_assert(is_of_type<one*(metre* square(second) / gram), derived_unit<metre_, power<second_, 2>, per<gram_>>>);
static_assert(is_of_type<one * metre * square(second) / gram, derived_unit<metre_, power<second_, 2>, per<gram_>>>);
static_assert(is_of_type<(metre * square(second) / gram) * one, derived_unit<metre_, power<second_, 2>, per<gram_>>>);
static_assert(is_of_type<metre * square(second) / gram * one, derived_unit<metre_, power<second_, 2>, per<gram_>>>);

static_assert(is_of_type<standard_gravity * gram, derived_unit<gram_, standard_gravity_>>);
static_assert(is_of_type<gram * standard_gravity, derived_unit<gram_, standard_gravity_>>);
static_assert(is_of_type<standard_gravity / gram, derived_unit<standard_gravity_, per<gram_>>>);
static_assert(is_of_type<gram / standard_gravity, derived_unit<gram_, per<standard_gravity_>>>);
static_assert(is_of_type<standard_gravity * gram / standard_gravity, gram_>);
static_assert(is_of_type<speed_of_light_in_vacuum * gram * standard_gravity,
                         derived_unit<speed_of_light_in_vacuum_, gram_, standard_gravity_>>);
static_assert(is_of_type<gram * standard_gravity * speed_of_light_in_vacuum,
                         derived_unit<speed_of_light_in_vacuum_, gram_, standard_gravity_>>);

static_assert(is_same_v<decltype(inverse(second) * metre), decltype(metre / second)>);
static_assert(is_same_v<decltype(metre * inverse(second)), decltype(metre / second)>);
static_assert(is_same_v<decltype((metre / second) * inverse(second)), decltype(metre / second / second)>);
static_assert(is_same_v<decltype((metre / second) * inverse(second)), decltype(metre / (second * second))>);
static_assert(is_same_v<decltype((metre / second) * inverse(second)), decltype(metre / square(second))>);


// derived unit normalization
constexpr auto m_per_s = metre / second;
static_assert(is_of_type<get_canonical_unit(m_per_s).reference_unit, derived_unit<metre_, per<second_>>>);
static_assert(get_canonical_unit(m_per_s).mag == mag<1>);

constexpr auto km_per_s = kilometre / second;
static_assert(is_of_type<km_per_s, derived_unit<kilometre_, per<second_>>>);
static_assert(is_of_type<get_canonical_unit(km_per_s).reference_unit, derived_unit<metre_, per<second_>>>);
static_assert(get_canonical_unit(km_per_s).mag == mag<1000>);

constexpr auto km_per_h = kilometre / hour;
static_assert(is_of_type<km_per_h, derived_unit<kilometre_, per<hour_>>>);
static_assert(is_of_type<get_canonical_unit(km_per_h).reference_unit, derived_unit<metre_, per<second_>>>);
static_assert(get_canonical_unit(km_per_h).mag == mag_ratio<1000, 3600>);

static_assert(is_of_type<get_canonical_unit(inverse(metre)).reference_unit, derived_unit<one_, per<metre_>>>);
static_assert(is_of_type<get_canonical_unit(inverse(hertz)).reference_unit, second_>);

static_assert(
  is_of_type<get_canonical_unit(pascal).reference_unit, derived_unit<gram_, per<metre_, power<second_, 2>>>>);
static_assert(
  is_of_type<get_canonical_unit(one / pascal).reference_unit, derived_unit<metre_, power<second_, 2>, per<gram_>>>);

static_assert(
  is_of_type<get_canonical_unit(standard_gravity).reference_unit, derived_unit<metre_, per<power<second_, 2>>>>);
static_assert(get_canonical_unit(standard_gravity).mag == mag_ratio<980'665, 100'000>);
static_assert(is_of_type<get_canonical_unit(standard_gravity* gram).reference_unit,
                         derived_unit<gram_, metre_, per<power<second_, 2>>>>);
static_assert(is_of_type<get_canonical_unit(standard_gravity / speed_of_light_in_vacuum).reference_unit,
                         derived_unit<one_, per<second_>>>);

// operations commutativity
constexpr auto u1 = mag<1000> * kilometre / hour;
static_assert(is_of_type<u1, scaled_unit<mag<1000>, derived_unit<kilometre_, per<hour_>>>>);
static_assert(is_of_type<get_canonical_unit(u1).reference_unit, derived_unit<metre_, per<second_>>>);
static_assert(get_canonical_unit(u1).mag == mag_ratio<1'000'000, 3'600>);

constexpr auto u2 = mag<1000> * (kilometre / hour);
static_assert(is_of_type<u2, scaled_unit<mag<1000>, derived_unit<kilometre_, per<hour_>>>>);
static_assert(is_of_type<get_canonical_unit(u2).reference_unit, derived_unit<metre_, per<second_>>>);
static_assert(get_canonical_unit(u2).mag == mag_ratio<1'000'000, 3'600>);

constexpr auto u3 = one / hour * (mag<1000> * kilometre);
static_assert(is_of_type<u3, scaled_unit<mag<1000>, derived_unit<kilometre_, per<hour_>>>>);
static_assert(is_of_type<get_canonical_unit(u3).reference_unit, derived_unit<metre_, per<second_>>>);
static_assert(get_canonical_unit(u3).mag == mag_ratio<1'000'000, 3'600>);

template<auto& s>
concept invalid_operations = requires {
  requires !requires { s < s; };
  requires !requires { s / 2; };
  requires !requires { s * 2; };
  requires !requires { s + 2; };
  requires !requires { 2 + s; };
  requires !requires { s + s; };
  requires !requires { s - 2; };
  requires !requires { 2 - s; };
  requires !requires { s - s; };
  requires !requires { s == 2; };
  requires !requires { 2 == s; };
  requires !requires { s < 2; };
  requires !requires { 2 < s; };
  requires !requires { s + time[second]; };
  requires !requires { s - time[second]; };
  requires !requires { s < time[second]; };
  requires !requires { time[second] + s; };
  requires !requires { time[second] - s; };
  requires !requires { s + 1 * time[second]; };
  requires !requires { s - 1 * time[second]; };
  requires !requires { s * 1 * time[second]; };
  requires !requires { s / 1 * time[second]; };
  requires !requires { s == 1 * time[second]; };
  requires !requires { s == 1 * time[second]; };
  requires !requires { 1 * time[second] + s; };
  requires !requires { 1 * time[second] - s; };
  requires !requires { 1 * time[second] == s; };
  requires !requires { 1 * time[second] < s; };
};
static_assert(invalid_operations<second>);

// comparisons of the same units
static_assert(second == second);
static_assert(metre / second == metre / second);
static_assert(si::milli<metre> / si::milli<second> == si::micro<metre> / si::micro<second>);
static_assert(si::milli<metre> / si::micro<second> == si::micro<metre> / si::nano<second>);
static_assert(si::micro<metre> / si::milli<second> == si::nano<metre> / si::micro<second>);
static_assert(si::milli<metre> * si::kilo<metre> == si::deci<metre> * si::deca<metre>);
static_assert(si::kilo<metre> * si::milli<metre> == si::deca<metre> * si::deci<metre>);

// comparisons of equivalent units (named vs unnamed/derived)
static_assert(one / second == hertz);
static_assert(convertible(one / second, hertz));

// comparisons of equivalent units of different quantities
static_assert(hertz == becquerel);
static_assert(convertible(hertz, becquerel));

// comparisons of scaled units
static_assert(si::kilo<metre> == kilometre);
static_assert(mag<1000> * metre == si::kilo<metre>);
static_assert(mag<1000> * metre == kilometre);
static_assert(convertible(si::kilo<metre>, kilometre));
static_assert(convertible(mag<1000> * metre, si::kilo<metre>));
static_assert(convertible(mag<1000> * metre, kilometre));

static_assert(mag<60> * metre / second == metre / (mag_ratio<1, 60> * second));

static_assert(metre != kilometre);
static_assert(convertible(metre, kilometre));
static_assert(mag<100> * metre != kilometre);
static_assert(convertible(mag<100> * metre, kilometre));
static_assert(si::milli<metre> != kilometre);
static_assert(convertible(si::milli<metre>, kilometre));

// comparisons of non-convertible units
static_assert(metre != metre * metre);
static_assert(!convertible(metre, metre* metre));

// one
static_assert(is_of_type<metre / metre, one_>);
static_assert(is_of_type<si::kilo<metre> / metre, derived_unit<si::kilo_<metre_>, per<metre_>>>);
static_assert(metre / metre == one);
static_assert(hertz * second == one);
static_assert(one * one == one);
static_assert(is_of_type<one * one, one_>);
static_assert(one * percent == percent);
static_assert(percent * one == percent);
static_assert(is_of_type<one * percent, percent_>);
static_assert(is_of_type<percent * one, percent_>);

static_assert(hertz == one / second);
static_assert(newton == kilogram * metre / square(second));
static_assert(joule == kilogram * square(metre) / square(second));
static_assert(joule == newton * metre);
static_assert(watt == joule / second);
static_assert(watt == kilogram * square(metre) / cubic(second));

// power
static_assert(is_same_v<decltype(pow<2>(metre)), decltype(metre * metre)>);
static_assert(is_same_v<decltype(pow<2>(kilometre)), decltype(kilometre * kilometre)>);
static_assert(is_same_v<decltype(pow<2>(si::kilo<metre>)), decltype(si::kilo<metre> * si::kilo<metre>)>);
static_assert(is_same_v<decltype(pow<2>(hour)), decltype(hour * hour)>);
static_assert(is_same_v<decltype(pow<2>(mag<3600> * second)), decltype((mag<3600> * second) * (mag<3600> * second))>);
static_assert(is_same_v<decltype(pow<2>(metre / second)), decltype(metre * metre / second / second)>);
static_assert(is_same_v<decltype(pow<2>(kilometre / hour)), decltype(kilometre * kilometre / hour / hour)>);

static_assert(is_of_type<pow<0>(metre), one_>);
static_assert(is_of_type<pow<1>(metre), metre_>);
static_assert(is_of_type<pow<2, 2>(metre), metre_>);
static_assert(is_of_type<pow<2>(one), one_>);
static_assert(is_of_type<pow<2>(percent), derived_unit<power<percent_, 2>>>);
static_assert(is_of_type<pow<2>(radian), derived_unit<power<radian_, 2>>>);
static_assert(is_of_type<pow<2>(metre), derived_unit<power<metre_, 2>>>);
static_assert(is_of_type<pow<1, 2>(metre), derived_unit<power<metre_, 1, 2>>>);
static_assert(is_of_type<pow<1, 2>(metre* metre), metre_>);
static_assert(is_of_type<pow<1, 3>(metre* metre* metre), metre_>);
static_assert(is_of_type<pow<1, 3>(metre* metre), derived_unit<power<metre_, 2, 3>>>);
static_assert(is_of_type<pow<1, 2>(metre / second), derived_unit<power<metre_, 1, 2>, per<power<second_, 1, 2>>>>);
static_assert(is_of_type<pow<1, 2>(metre / (second * second)), derived_unit<power<metre_, 1, 2>, per<second_>>>);
static_assert(is_of_type<kilometre * kilometre, derived_unit<power<kilometre_, 2>>>);

static_assert(is_of_type<pow<2>(kilometre), derived_unit<power<kilometre_, 2>>>);
static_assert(is_of_type<pow<2>(si::kilo<metre>), derived_unit<power<si::kilo_<metre_>, 2>>>);
static_assert(is_of_type<pow<2>(hour), derived_unit<power<hour_, 2>>>);
static_assert(
  is_of_type<pow<2>(mag<3600>* second), scaled_unit<mag<3600> * mag<3600>, derived_unit<power<second_, 2>>>>);

// common_unit
static_assert(is_of_type<common_unit(gram, gram), gram_>);
static_assert(is_of_type<common_unit(kilogram, kilogram), kilogram_>);
static_assert(is_of_type<common_unit(si::kilo<gram>, kilogram), kilogram_>);
static_assert(is_of_type<common_unit(kilogram, si::kilo<gram>), kilogram_>);
static_assert(is_of_type<common_unit(mag<1000>* gram, kilogram), kilogram_>);
static_assert(is_of_type<common_unit(kilogram, mag<1000>* gram), kilogram_>);
static_assert(is_of_type<common_unit(one / second, hertz), hertz_>);
static_assert(is_of_type<common_unit(hertz, one / second), hertz_>);
static_assert(is_of_type<common_unit(gram, kilogram), gram_>);
static_assert(is_of_type<common_unit(kilogram, gram), gram_>);
static_assert(is_of_type<common_unit(second, hour), second_>);
static_assert(is_of_type<common_unit(hour, second), second_>);
static_assert(is_of_type<common_unit(minute, hour), minute_>);
static_assert(is_of_type<common_unit(hour, minute), minute_>);
static_assert(
  is_of_type<common_unit(si::kilo<metre>, si::milli<metre>), std::remove_const_t<decltype(si::milli<metre>)>>);
static_assert(
  is_of_type<common_unit(si::milli<metre>, si::kilo<metre>), std::remove_const_t<decltype(si::milli<metre>)>>);
static_assert(is_of_type<common_unit(yard, mile), yard_>);
static_assert(is_of_type<common_unit(mile, yard), yard_>);
// TODO The below have long/unreadable magnitude types
static_assert(is_of_type<common_unit(kilometre / hour, metre / second),
                         scaled_unit<mag_ratio<1, 18>, derived_unit<metre_, per<second_>>>>);
static_assert(is_of_type<common_unit(metre / second, kilometre / hour),
                         scaled_unit<mag_ratio<1, 18>, derived_unit<metre_, per<second_>>>>);
static_assert(is_of_type<common_unit(kilometre, mile), scaled_unit<mag_ratio<8, 125>, metre_>>);
static_assert(is_of_type<common_unit(mile, kilometre), scaled_unit<mag_ratio<8, 125>, metre_>>);
static_assert(is_of_type<common_unit(speed_of_light_in_vacuum, metre / second), derived_unit<metre_, per<second_>>>);

}  // namespace
