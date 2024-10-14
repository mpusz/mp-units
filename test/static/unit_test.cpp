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
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si/prefixes.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <type_traits>
#endif

namespace {

using namespace mp_units;
using namespace mp_units::detail;

using one_ = struct one;
using percent_ = struct percent;

// clang-format off
// prefixes
template<PrefixableUnit U> struct milli_  final : prefixed_unit<"m", mag_power<10, -3>, U{}> {};
template<PrefixableUnit U> struct kilo_   final : prefixed_unit<"k", mag_power<10, 3>, U{}> {};
template<PrefixableUnit auto U> constexpr milli_<MP_UNITS_REMOVE_CONST(decltype(U))> milli;
template<PrefixableUnit auto U> constexpr kilo_<MP_UNITS_REMOVE_CONST(decltype(U))> kilo;

// base units
inline constexpr struct second_ final : named_unit<"s", kind_of<isq::time>> {} second;
inline constexpr struct metre_ final : named_unit<"m", kind_of<isq::length>> {} metre;
inline constexpr struct gram_ final : named_unit<"g", kind_of<isq::mass>> {} gram;
inline constexpr auto kilogram = kilo<gram>;
inline constexpr struct kelvin_ final : named_unit<"K", kind_of<isq::thermodynamic_temperature>> {} kelvin;

// hypothetical natural units for c=1
inline constexpr struct nu_second_ final : named_unit<"s"> {} nu_second;

// derived named units
inline constexpr struct radian_ final : named_unit<"rad", metre / metre, kind_of<isq::angular_measure>> {} radian;
inline constexpr struct revolution_ final : named_unit<"rev", mag<2> * mag<𝜋> * radian> {} revolution;
inline constexpr struct steradian_ final : named_unit<"sr", square(metre) / square(metre), kind_of<isq::solid_angular_measure>> {} steradian;
inline constexpr struct hertz_ final : named_unit<"Hz", inverse(second), kind_of<isq::frequency>> {} hertz;
inline constexpr struct becquerel_ final : named_unit<"Bq", inverse(second), kind_of<isq::activity>> {} becquerel;
inline constexpr struct newton_ final : named_unit<"N", kilogram * metre / square(second)> {} newton;
inline constexpr struct pascal_ final : named_unit<"Pa", newton / square(metre)> {} pascal;
inline constexpr struct joule_ final : named_unit<"J", newton * metre> {} joule;
inline constexpr struct watt_ final : named_unit<"W", joule / second> {} watt;
inline constexpr struct degree_Celsius_ final : named_unit<symbol_text{u8"℃", "`C"}, kelvin> {} degree_Celsius;

inline constexpr struct minute_ final : named_unit<"min", mag<60> * second> {} minute;
inline constexpr struct hour_ final : named_unit<"h", mag<60> * minute> {} hour;
inline constexpr struct degree_ final : named_unit<symbol_text{u8"°", "deg"}, mag<𝜋> / mag<180> * radian> {} degree;

inline constexpr struct yard_ final : named_unit<"yd", mag_ratio<9'144, 10'000> * metre> {} yard;
inline constexpr struct mile_ final : named_unit<"mi", mag<1760> * yard> {} mile;
inline constexpr struct nautical_mile_ final : named_unit<"nmi", mag<1852> * metre> {} nautical_mile;

inline constexpr auto kilometre = kilo<metre>;
inline constexpr auto kilojoule = kilo<joule>;

// physical constant units
inline constexpr struct standard_gravity_ final : named_unit<symbol_text{u8"g₀", "g_0"}, mag_ratio<980'665, 100'000> * metre / square(second)> {} standard_gravity;
inline constexpr struct speed_of_light_in_vacuum_ final : named_unit<"c", mag<299'792'458> * metre / second> {} speed_of_light_in_vacuum;

// clang-format on

// concepts verification
static_assert(Unit<metre_>);
static_assert(Unit<second_>);
static_assert(Unit<nu_second_>);
static_assert(Unit<decltype(kilogram)>);
static_assert(Unit<hertz_>);
static_assert(Unit<newton_>);
static_assert(Unit<minute_>);
static_assert(Unit<decltype(kilo<gram>)>);
static_assert(Unit<decltype(square(metre))>);
static_assert(Unit<decltype(cubic(metre))>);
static_assert(Unit<decltype(mag<60> * second)>);
static_assert(Unit<decltype(second * second)>);
static_assert(Unit<decltype(nu_second * nu_second)>);
static_assert(Unit<decltype(metre / second)>);
static_assert(Unit<decltype(nu_second / nu_second)>);
static_assert(Unit<decltype(kilometre)>);

static_assert(PrefixableUnit<metre_>);
static_assert(PrefixableUnit<hertz_>);
static_assert(PrefixableUnit<newton_>);
static_assert(PrefixableUnit<minute_>);
static_assert(PrefixableUnit<radian_>);
static_assert(!PrefixableUnit<decltype(kilogram)>);
static_assert(!PrefixableUnit<decltype(kilojoule)>);
static_assert(!PrefixableUnit<decltype(kilo<gram>)>);
static_assert(!PrefixableUnit<decltype(square(metre))>);
static_assert(!PrefixableUnit<decltype(cubic(metre))>);
static_assert(!PrefixableUnit<decltype(mag<60> * second)>);
static_assert(!PrefixableUnit<decltype(kilometre)>);

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
static_assert(degree_Celsius != kelvin);
static_assert(equivalent(degree_Celsius, kelvin));

static_assert(is_of_type<radian, radian_>);
static_assert(is_of_type<get_canonical_unit(radian).reference_unit, one_>);
static_assert(get_canonical_unit(radian).mag == mag<1>);

static_assert(is_of_type<degree, degree_>);
static_assert(is_of_type<get_canonical_unit(degree).reference_unit, one_>);
static_assert(get_canonical_unit(degree).mag == mag<𝜋> / mag<180>);
static_assert(convertible(radian, degree));
static_assert(radian != degree);

static_assert(is_of_type<steradian, steradian_>);
static_assert(is_of_type<get_canonical_unit(steradian).reference_unit, one_>);
static_assert(get_canonical_unit(steradian).mag == mag<1>);
static_assert(!convertible(radian, steradian));
static_assert(radian != steradian);

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
static_assert(is_of_type<kilometre, std::remove_const_t<decltype(kilo<metre>)>>);
static_assert(is_of_type<get_canonical_unit(kilometre).reference_unit, metre_>);
static_assert(get_canonical_unit(kilometre).mag == mag<1000>);
static_assert(convertible(kilometre, metre));
static_assert(kilometre != metre);
static_assert(kilometre.symbol == "km");

static_assert(is_of_type<kilojoule, std::remove_const_t<decltype(kilo<joule>)>>);
static_assert(is_of_type<get_canonical_unit(kilojoule).reference_unit,
                         derived_unit<gram_, power<metre_, 2>, per<power<second_, 2>>>>);
static_assert(get_canonical_unit(kilojoule).mag == mag<1'000'000>);
static_assert(convertible(kilojoule, joule));
static_assert(kilojoule != joule);
static_assert(kilojoule.symbol == "kJ");

static_assert(is_of_type<kilo<metre>, kilo_<metre_>>);
static_assert(is_of_type<kilo<joule>, kilo_<joule_>>);

static_assert(
  is_of_type<kilometre / metre, derived_unit<std::remove_const_t<decltype(kilo<metre>)>, per<metre_>>>);  // !!!


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
static_assert(is_of_type<km_2, scaled_unit<mag<2>, kilo_<metre_>>>);
static_assert(is_of_type<get_canonical_unit(km_2).reference_unit, metre_>);
static_assert(get_canonical_unit(km_2).mag == mag<2000>);

constexpr auto kJ_42 = mag<42> * kilo<joule>;
static_assert(is_of_type<kJ_42, scaled_unit<mag<42>, kilo_<joule_>>>);
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
                         derived_unit<gram_, speed_of_light_in_vacuum_, standard_gravity_>>);
static_assert(is_of_type<gram * standard_gravity * speed_of_light_in_vacuum,
                         derived_unit<gram_, speed_of_light_in_vacuum_, standard_gravity_>>);

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
static_assert(is_of_type<km_per_s, derived_unit<kilo_<metre_>, per<second_>>>);
static_assert(is_of_type<get_canonical_unit(km_per_s).reference_unit, derived_unit<metre_, per<second_>>>);
static_assert(get_canonical_unit(km_per_s).mag == mag<1000>);

constexpr auto km_per_h = kilometre / hour;
static_assert(is_of_type<km_per_h, derived_unit<kilo_<metre_>, per<hour_>>>);
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
static_assert(is_of_type<u1, derived_unit<scaled_unit<mag<1000>, kilo_<metre_>>, per<hour_>>>);
static_assert(is_of_type<get_canonical_unit(u1).reference_unit, derived_unit<metre_, per<second_>>>);
static_assert(get_canonical_unit(u1).mag == mag_ratio<1'000'000, 3'600>);

constexpr auto u2 = mag<1000> * (kilometre / hour);
static_assert(is_of_type<u2, scaled_unit<mag<1000>, derived_unit<kilo_<metre_>, per<hour_>>>>);
static_assert(is_of_type<get_canonical_unit(u2).reference_unit, derived_unit<metre_, per<second_>>>);
static_assert(get_canonical_unit(u2).mag == mag_ratio<1'000'000, 3'600>);

constexpr auto u3 = one / hour * (mag<1000> * kilometre);
static_assert(is_of_type<u3, derived_unit<scaled_unit<mag<1000>, kilo_<metre_>>, per<hour_>>>);
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
  requires !requires { s + isq::time[second]; };
  requires !requires { s - isq::time[second]; };
  requires !requires { s < isq::time[second]; };
  requires !requires { isq::time[second] + s; };
  requires !requires { isq::time[second] - s; };
  requires !requires { s + 1 * isq::time[second]; };
  requires !requires { s - 1 * isq::time[second]; };
  requires !requires { s * 1 * isq::time[second]; };
  requires !requires { s / 1 * isq::time[second]; };
  requires !requires { s == 1 * isq::time[second]; };
  requires !requires { s == 1 * isq::time[second]; };
  requires !requires { 1 * isq::time[second] + s; };
  requires !requires { 1 * isq::time[second] - s; };
  requires !requires { 1 * isq::time[second] == s; };
  requires !requires { 1 * isq::time[second] < s; };
};
static_assert(invalid_operations<second>);

// comparisons of the same units
static_assert(second == second);
static_assert(metre / second == metre / second);
static_assert(milli<metre> / milli<second> != si::micro<metre> / si::micro<second>);
static_assert(equivalent(milli<metre> / milli<second>, si::micro<metre> / si::micro<second>));
static_assert(milli<metre> / si::micro<second> != si::micro<metre> / si::nano<second>);
static_assert(equivalent(milli<metre> / si::micro<second>, si::micro<metre> / si::nano<second>));
static_assert(si::micro<metre> / milli<second> != si::nano<metre> / si::micro<second>);
static_assert(equivalent(si::micro<metre> / milli<second>, si::nano<metre> / si::micro<second>));
static_assert(milli<metre> * kilo<metre> != si::deci<metre> * si::deca<metre>);
static_assert(equivalent(milli<metre> * kilo<metre>, si::deci<metre>* si::deca<metre>));
static_assert(kilo<metre> * milli<metre> != si::deca<metre> * si::deci<metre>);
static_assert(equivalent(kilo<metre> * milli<metre>, si::deca<metre>* si::deci<metre>));

// comparisons of equivalent units (named vs unnamed/derived)
static_assert(one / second != hertz);
static_assert(equivalent(one / second, hertz));
static_assert(convertible(one / second, hertz));

// comparisons of equivalent units of different quantities
static_assert(hertz != becquerel);
static_assert(!convertible(hertz, becquerel));

// comparisons of scaled units
static_assert(kilo<metre> == kilometre);
static_assert(mag<1000> * metre != kilo<metre>);
static_assert(equivalent(mag<1000> * metre, kilo<metre>));
static_assert(mag<1000> * metre != kilometre);
static_assert(equivalent(mag<1000> * metre, kilometre));
static_assert(convertible(kilo<metre>, kilometre));
static_assert(convertible(mag<1000> * metre, kilo<metre>));
static_assert(convertible(mag<1000> * metre, kilometre));

static_assert(mag<60> * metre / second != metre / (mag_ratio<1, 60> * second));
static_assert(equivalent(mag<60> * metre / second, metre / (mag_ratio<1, 60> * second)));

static_assert(metre != kilometre);
static_assert(convertible(metre, kilometre));
static_assert(mag<100> * metre != kilometre);
static_assert(convertible(mag<100> * metre, kilometre));
static_assert(milli<metre> != kilometre);
static_assert(convertible(milli<metre>, kilometre));

// comparisons of non-convertible units
static_assert(metre != metre * metre);
static_assert(!convertible(metre, metre* metre));

// one
static_assert(is_of_type<metre / metre, one_>);
static_assert(is_of_type<kilo<metre> / metre, derived_unit<kilo_<metre_>, per<metre_>>>);
static_assert(metre / metre == one);
static_assert(hertz * second != one);
static_assert(equivalent(hertz * second, one));
static_assert(one * one == one);
static_assert(is_of_type<one * one, one_>);
static_assert(one * percent == percent);
static_assert(percent * one == percent);
static_assert(is_of_type<one * percent, percent_>);
static_assert(is_of_type<percent * one, percent_>);

static_assert(hertz != one / second);
static_assert(equivalent(hertz, one / second));
static_assert(newton != kilogram * metre / square(second));
static_assert(equivalent(newton, kilogram* metre / square(second)));
static_assert(joule != kilogram * square(metre) / square(second));
static_assert(equivalent(joule, kilogram* square(metre) / square(second)));
static_assert(joule != newton * metre);
static_assert(equivalent(joule, newton* metre));
static_assert(watt != joule / second);
static_assert(equivalent(watt, joule / second));
static_assert(watt != kilogram * square(metre) / cubic(second));
static_assert(equivalent(watt, kilogram* square(metre) / cubic(second)));

// power
static_assert(is_same_v<decltype(pow<2>(metre)), decltype(metre * metre)>);
static_assert(is_same_v<decltype(pow<2>(kilometre)), decltype(kilometre * kilometre)>);
static_assert(is_same_v<decltype(pow<2>(kilo<metre>)), decltype(kilo<metre> * kilo<metre>)>);
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
static_assert(is_of_type<kilometre * kilometre, derived_unit<power<kilo_<metre_>, 2>>>);

static_assert(is_of_type<pow<2>(kilometre), derived_unit<power<kilo_<metre_>, 2>>>);
static_assert(is_of_type<pow<2>(kilo<metre>), derived_unit<power<kilo_<metre_>, 2>>>);
static_assert(is_of_type<pow<2>(hour), derived_unit<power<hour_, 2>>>);
static_assert(is_of_type<pow<2>(mag<3600>* second), derived_unit<power<scaled_unit<mag<3600>, second_>, 2>>>);

// get_common_unit
static_assert(is_of_type<get_common_unit(gram, gram), gram_>);
static_assert(is_of_type<get_common_unit(kilogram, kilogram), kilo_<gram_>>);
static_assert(is_of_type<get_common_unit(kilo<gram>, kilogram), kilo_<gram_>>);
static_assert(is_of_type<get_common_unit(kilogram, kilo<gram>), kilo_<gram_>>);
static_assert(is_of_type<get_common_unit(mag<1000>* gram, kilogram), kilo_<gram_>>);
static_assert(is_of_type<get_common_unit(kilogram, mag<1000>* gram), kilo_<gram_>>);
static_assert(is_of_type<get_common_unit(one / second, hertz), hertz_>);
static_assert(is_of_type<get_common_unit(hertz, one / second), hertz_>);
static_assert(is_of_type<get_common_unit(gram, kilogram), gram_>);
static_assert(is_of_type<get_common_unit(kilogram, gram), gram_>);
static_assert(is_of_type<get_common_unit(second, hour), second_>);
static_assert(is_of_type<get_common_unit(hour, second), second_>);
static_assert(is_of_type<get_common_unit(minute, hour), minute_>);
static_assert(is_of_type<get_common_unit(hour, minute), minute_>);
static_assert(is_of_type<get_common_unit(kilo<metre>, milli<metre>), milli_<metre_>>);
static_assert(is_of_type<get_common_unit(milli<metre>, kilo<metre>), milli_<metre_>>);
static_assert(is_of_type<get_common_unit(yard, mile), yard_>);
static_assert(is_of_type<get_common_unit(mile, yard), yard_>);
static_assert(
  is_of_type<get_common_unit(speed_of_light_in_vacuum, metre / second), derived_unit<metre_, per<second_>>>);

static_assert(is_of_type<get_common_unit(radian, revolution), radian_>);

// those should return instantiations of the `common_unit` class template
static_assert(is_of_type<get_common_unit(kilometre, mile), common_unit<kilo_<metre_>, mile_>>);
static_assert(is_of_type<get_common_unit(mile, kilometre), common_unit<kilo_<metre_>, mile_>>);
static_assert(is_of_type<get_common_unit(kilometre / hour, metre / second),
                         common_unit<decltype(kilometre / hour), decltype(metre / second)>>);
static_assert(is_of_type<get_common_unit(metre / second, kilometre / hour),
                         common_unit<decltype(kilometre / hour), decltype(metre / second)>>);
static_assert(is_of_type<get_common_unit(radian, degree), common_unit<degree_, radian_>>);

static_assert(
  is_of_type<get_common_unit(mile, kilometre) / second, derived_unit<common_unit<kilo_<metre_>, mile_>, per<second_>>>);

static_assert(
  is_of_type<get_common_unit(kilometre, mile, nautical_mile), common_unit<kilo_<metre_>, mile_, nautical_mile_>>);
static_assert(
  is_of_type<get_common_unit(nautical_mile, mile, kilometre), common_unit<kilo_<metre_>, mile_, nautical_mile_>>);
static_assert(
  is_of_type<get_common_unit(kilometre, nautical_mile, mile), common_unit<kilo_<metre_>, mile_, nautical_mile_>>);

static_assert(is_of_type<get_common_unit(kilometre, get_common_unit(mile, nautical_mile)),
                         common_unit<kilo_<metre_>, mile_, nautical_mile_>>);
static_assert(is_of_type<get_common_unit(nautical_mile, get_common_unit(mile, kilometre)),
                         common_unit<kilo_<metre_>, mile_, nautical_mile_>>);
static_assert(is_of_type<get_common_unit(kilometre, get_common_unit(nautical_mile, mile)),
                         common_unit<kilo_<metre_>, mile_, nautical_mile_>>);

static_assert(is_of_type<get_common_unit(kilometre, mile, kilometre), common_unit<kilo_<metre_>, mile_>>);
static_assert(is_of_type<get_common_unit(mile, kilometre, kilometre), common_unit<kilo_<metre_>, mile_>>);
static_assert(is_of_type<get_common_unit(kilometre, mile, mile), common_unit<kilo_<metre_>, mile_>>);
static_assert(is_of_type<get_common_unit(mile, kilometre, mile), common_unit<kilo_<metre_>, mile_>>);
static_assert(is_of_type<get_common_unit(mile, get_common_unit(kilometre, mile)), common_unit<kilo_<metre_>, mile_>>);
static_assert(
  is_of_type<get_common_unit(kilometre, get_common_unit(kilometre, mile)), common_unit<kilo_<metre_>, mile_>>);

static_assert(is_of_type<get_common_unit(kilometre, mile, metre), common_unit<metre_, mile_>>);

static_assert(is_of_type<get_common_unit(kilometre, mile, milli<metre>), milli_<metre_>>);

// check underlying types
static_assert(std::derived_from<decltype(get_common_unit(kilometre / hour, metre / second)),
                                detail::scaled_unit_impl<mag_ratio<1, 18>, derived_unit<metre_, per<second_>>>>);
static_assert(std::derived_from<decltype(get_common_unit(metre / second, kilometre / hour)),
                                detail::scaled_unit_impl<mag_ratio<1, 18>, derived_unit<metre_, per<second_>>>>);
static_assert(
  std::derived_from<decltype(get_common_unit(kilometre, mile)), detail::scaled_unit_impl<mag_ratio<8, 125>, metre_>>);
static_assert(
  std::derived_from<decltype(get_common_unit(mile, kilometre)), detail::scaled_unit_impl<mag_ratio<8, 125>, metre_>>);
static_assert(std::derived_from<decltype(get_common_unit(kilometre, mile, si::centi<metre>)),
                                detail::scaled_unit_impl<mag_ratio<1, 500>, metre_>>);

}  // namespace
