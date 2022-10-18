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

#include <units/si/prefixes.h>
#include <units/unit.h>

namespace {

using namespace units;
using namespace units::detail;

template<auto V, typename T>
inline constexpr bool is_of_type = std::is_same_v<std::remove_cvref_t<decltype(V)>, T>;

using one_ = struct one;

// clang-format off
// base units
inline constexpr struct second_ : named_unit<"s"> {} second;
inline constexpr struct metre_ : named_unit<"m"> {} metre;
inline constexpr struct gram_ : named_unit<"g"> {} gram;
inline constexpr struct kilogram_ : decltype(si::kilo<gram>) {} kilogram;
inline constexpr struct kelvin_ : named_unit<"K"> {} kelvin;

// derived named units
inline constexpr struct radian_ : named_unit<"rad", metre / metre> {} radian;
inline constexpr struct steradian_ : named_unit<"sr", square<metre> / square<metre>> {} steradian;
inline constexpr struct hertz_ : named_unit<"Hz", 1 / second> {} hertz;
inline constexpr struct becquerel : named_unit<"Bq", 1 / second> {} becquerel;
inline constexpr struct newton_ : named_unit<"N", kilogram * metre / square<second>> {} newton;
inline constexpr struct pascal_ : named_unit<"Pa", newton / square<metre>> {} pascal;
inline constexpr struct joule_ : named_unit<"J", newton * metre> {} joule;
inline constexpr struct watt_ : named_unit<"W", joule / second> {} watt;
inline constexpr struct degree_Celsius_ : named_unit<basic_symbol_text{"\u00B0C", "`C"}, kelvin> {} degree_Celsius;

inline constexpr struct minute_ : named_unit<"min", mag<60> * second> {} minute;
inline constexpr struct hour_ : named_unit<"h", mag<60> * minute> {} hour;
inline constexpr struct day_ : named_unit<"d", mag<24> * hour> {} day;
inline constexpr struct astronomical_unit_ : named_unit<"au", mag<149'597'870'700> * metre> {} astronomical_unit;
inline constexpr struct degree_ : named_unit<basic_symbol_text{"°", "deg"}, mag_pi / mag<180> * radian> {} degree;
inline constexpr struct are_ : named_unit<"a", square<si::deca<metre>>> {} are;
inline constexpr struct hectare_ : decltype(si::hecto<are>) {} hectare;
inline constexpr struct litre_ : named_unit<"l", cubic<si::deci<metre>>> {} litre;
inline constexpr struct tonne_ : named_unit<"t", mag<1000> * kilogram> {} tonne;
inline constexpr struct dalton_ : named_unit<"Da", mag<ratio{16'605'390'666'050, 10'000'000'000'000}> * mag_power<10, -27> * kilogram> {} dalton;
inline constexpr struct electronvolt_ : named_unit<"eV", mag<ratio{1'602'176'634, 1'000'000'000}> * mag_power<10, -19> * joule> {} electronvolt;

inline constexpr struct kilometre_ : decltype(si::kilo<metre>) {} kilometre;
inline constexpr struct kilojoule_ : decltype(si::kilo<joule>) {} kilojoule;
// clang-format on

// concepts verification
static_assert(Unit<metre_>);
static_assert(Unit<kilogram_>);
static_assert(Unit<hertz_>);
static_assert(Unit<newton_>);
static_assert(Unit<minute_>);
static_assert(Unit<decltype(si::kilo<gram>)>);
static_assert(Unit<decltype(square<metre>)>);
static_assert(Unit<decltype(cubic<metre>)>);
static_assert(Unit<decltype(mag<60> * second)>);
static_assert(Unit<kilometre_>);

static_assert(NamedUnit<metre_>);
static_assert(NamedUnit<hertz_>);
static_assert(NamedUnit<newton_>);
static_assert(NamedUnit<minute_>);
static_assert(NamedUnit<radian_>);
static_assert(!NamedUnit<kilogram_>);
static_assert(!NamedUnit<kilojoule_>);
static_assert(!NamedUnit<hectare_>);
static_assert(!NamedUnit<decltype(si::kilo<gram>)>);
static_assert(!NamedUnit<decltype(square<metre>)>);
static_assert(!NamedUnit<decltype(cubic<metre>)>);
static_assert(!NamedUnit<decltype(mag<60> * second)>);
static_assert(!NamedUnit<kilometre_>);

template<typename T>
constexpr bool print();

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
static_assert(convertible(minute, second));
static_assert(minute != second);

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

static_assert(is_of_type<si::kilo<metre>, si::kilo_<metre>>);
static_assert(is_of_type<si::kilo<joule>, si::kilo_<joule>>);


// prefixes
static_assert(si::yocto<metre>.symbol == "ym");
static_assert(si::zepto<metre>.symbol == "zm");
static_assert(si::atto<metre>.symbol == "am");
static_assert(si::femto<metre>.symbol == "fm");
static_assert(si::pico<metre>.symbol == "pm");
static_assert(si::nano<metre>.symbol == "nm");
static_assert(si::micro<metre>.symbol == basic_symbol_text{"µm", "um"});
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


// scaled_unit
constexpr auto u1 = mag<1> * metre;
static_assert(is_of_type<u1, scaled_unit<mag<1>, metre_>>);
static_assert(is_of_type<get_canonical_unit(u1).reference_unit, metre_>);
static_assert(get_canonical_unit(u1).mag == mag<1>);

constexpr auto u2 = mag<2> * kilometre;
static_assert(is_of_type<u2, scaled_unit<mag<2>, kilometre_>>);
static_assert(is_of_type<get_canonical_unit(u2).reference_unit, metre_>);
static_assert(get_canonical_unit(u2).mag == mag<2000>);

constexpr auto u3 = mag<42> * si::kilo<joule>;
static_assert(is_of_type<u3, scaled_unit<mag<42>, si::kilo_<joule>>>);
static_assert(
  is_of_type<get_canonical_unit(u3).reference_unit, derived_unit<gram_, power<metre_, 2>, per<power<second_, 2>>>>);
static_assert(get_canonical_unit(u3).mag == mag<42'000'000>);


// derived unit expression template syntax verification
static_assert(is_of_type<1 / second, derived_unit<one_, per<second_>>>);
static_assert(is_of_type<1 / (1 / second), second_>);

static_assert(is_of_type<one * second, second_>);
static_assert(is_of_type<second * one, second_>);
static_assert(is_of_type<one * (1 / second), derived_unit<one_, per<second_>>>);
static_assert(is_of_type<1 / second * one, derived_unit<one_, per<second_>>>);

static_assert(is_of_type<metre * second, derived_unit<metre_, second_>>);
static_assert(is_of_type<metre * metre, derived_unit<power<metre_, 2>>>);

static_assert(is_of_type<metre * metre * second, derived_unit<power<metre_, 2>, second_>>);
static_assert(is_of_type<metre * second * metre, derived_unit<power<metre_, 2>, second_>>);

static_assert(is_of_type<metre*(second* metre), derived_unit<power<metre_, 2>, second_>>);
static_assert(is_of_type<second*(metre* metre), derived_unit<power<metre_, 2>, second_>>);

static_assert(is_of_type<1 / second * metre, derived_unit<metre_, per<second_>>>);
static_assert(is_of_type<1 / second * second, one_>);

static_assert(is_of_type<second / one, second_>);
static_assert(is_of_type<1 / second / one, derived_unit<one_, per<second_>>>);

static_assert(is_of_type<metre / second * second, metre_>);
static_assert(is_of_type<1 / second * (1 / second), derived_unit<one_, per<power<second_, 2>>>>);
static_assert(is_of_type<1 / (second * second), derived_unit<one_, per<power<second_, 2>>>>);
static_assert(is_of_type<1 / (1 / (second * second)), derived_unit<power<second_, 2>>>);

static_assert(is_of_type<metre / second * (1 / second), derived_unit<metre_, per<power<second_, 2>>>>);
static_assert(is_of_type<metre / second*(metre / second), derived_unit<power<metre_, 2>, per<power<second_, 2>>>>);
static_assert(is_of_type<metre / second*(second / metre), one_>);

static_assert(is_of_type<watt / joule, derived_unit<watt_, per<joule_>>>);
static_assert(is_of_type<joule / watt, derived_unit<joule_, per<watt_>>>);


// derived unit normalization
constexpr auto u4 = metre / second;
static_assert(is_of_type<get_canonical_unit(u4).reference_unit, derived_unit<metre_, per<second_>>>);
static_assert(get_canonical_unit(u4).mag == mag<1>);

constexpr auto u5 = kilometre / second;
static_assert(is_of_type<u5, derived_unit<kilometre_, per<second_>>>);
static_assert(is_of_type<get_canonical_unit(u5).reference_unit, derived_unit<metre_, per<second_>>>);
static_assert(get_canonical_unit(u5).mag == mag<1000>);

constexpr auto u6 = kilometre / hour;
static_assert(is_of_type<u6, derived_unit<kilometre_, per<hour_>>>);
static_assert(is_of_type<get_canonical_unit(u6).reference_unit, derived_unit<metre_, per<second_>>>);
static_assert(get_canonical_unit(u6).mag == mag<ratio{1000, 3600}>);

constexpr auto u7 = mag<1000> * kilometre / hour;
static_assert(is_of_type<u7, derived_unit<scaled_unit<mag<1000>, kilometre_>, per<hour_>>>);
static_assert(is_of_type<get_canonical_unit(u7).reference_unit, derived_unit<metre_, per<second_>>>);
static_assert(get_canonical_unit(u7).mag == mag<ratio{1'000'000, 3'600}>);

constexpr auto u8 = mag<1000> * (kilometre / hour);
static_assert(is_of_type<u8, scaled_unit<mag<1000>, derived_unit<kilometre_, per<hour_>>>>);
static_assert(is_of_type<get_canonical_unit(u8).reference_unit, derived_unit<metre_, per<second_>>>);
static_assert(get_canonical_unit(u8).mag == mag<ratio{1'000'000, 3'600}>);

constexpr auto u9 = 1 / hour * (mag<1000> * kilometre);
static_assert(is_of_type<u9, derived_unit<scaled_unit<mag<1000>, kilometre_>, per<hour_>>>);
static_assert(is_of_type<get_canonical_unit(u9).reference_unit, derived_unit<metre_, per<second_>>>);
static_assert(get_canonical_unit(u9).mag == mag<ratio{1'000'000, 3'600}>);

// comparisons of the same units
static_assert(second == second);
static_assert(metre / second == metre / second);

// comparisons of equivalent units (named vs unnamed/derived)
static_assert(1 / second == hertz);
static_assert(convertible(1 / second, hertz));

// comparisons of equivalent but not convertible units
static_assert(hertz == becquerel);
static_assert(convertible(hertz, becquerel));

// comparisons of scaled units
static_assert(si::kilo<metre> == kilometre);
static_assert(mag<1000> * metre == si::kilo<metre>);
static_assert(mag<1000> * metre == kilometre);
static_assert(convertible(si::kilo<metre>, kilometre));
static_assert(convertible(mag<1000> * metre, si::kilo<metre>));
static_assert(convertible(mag<1000> * metre, kilometre));

static_assert(metre != kilometre);
static_assert(convertible(metre, kilometre));
static_assert(mag<100> * metre != kilometre);
static_assert(convertible(mag<100> * metre, kilometre));
static_assert(si::milli<metre> != kilometre);
static_assert(convertible(si::milli<metre>, kilometre));

// one
static_assert(metre / metre == one);
// static_assert(metre * metre == square_metre);
// static_assert(second * second == second_squared);
// static_assert(second * second * second == second_cubed);
// static_assert(second * (second * second) == second_cubed);
// static_assert(second_squared * second == second_cubed);
// static_assert(second * second_squared == second_cubed);

// static_assert(1 / second * metre == metre / second);
// static_assert(metre * (1 / second) == metre / second);
// static_assert((metre / second) * (1 / second) == metre / second / second);
// static_assert((metre / second) * (1 / second) == metre / (second * second));
// static_assert((metre / second) * (1 / second) == metre / second_squared);

// static_assert(hertz == 1 / second);
// static_assert(newton == kilogram * metre / second_squared);
// static_assert(joule == kilogram * square_metre / second_squared);
// static_assert(joule == newton * metre);
// static_assert(watt == joule / second);
// static_assert(watt == kilogram * square_metre / second_cubed);

// static_assert(1 / frequency_dim == second);
// static_assert(frequency_dim * second == one);

// static_assert(metre * metre == area_dim);
// static_assert(metre * metre != volume_dim);
// static_assert(area_dim / metre == metre);

// static_assert(metre * metre * metre == volume_dim);
// static_assert(area_dim * metre == volume_dim);
// static_assert(volume_dim / metre == area_dim);
// static_assert(volume_dim / metre / metre == metre);
// static_assert(area_dim * area_dim / metre == volume_dim);
// static_assert(area_dim * (area_dim / metre) == volume_dim);
// static_assert(volume_dim / (metre * metre) == metre);

// static_assert(metre / second == speed_dim);
// static_assert(metre * second != speed_dim);
// static_assert(metre / second / second != speed_dim);
// static_assert(metre / speed_dim == second);
// static_assert(speed_dim * second == metre);

// static_assert(metre / second / second == acceleration_dim);
// static_assert(metre / (second * second) == acceleration_dim);
// static_assert(speed_dim / second == acceleration_dim);
// static_assert(speed_dim / acceleration_dim == second);
// static_assert(acceleration_dim * second == speed_dim);
// static_assert(acceleration_dim * (second * second) == metre);
// static_assert(acceleration_dim / speed_dim == frequency_dim);


// milli<metre> / milli<second> == micro<metre> / micro<second>;
// milli<metre> * kilo<metre> == deci<metre> * deca<metre>;

// Bq + Hz should not compile

// Bq + Hz + 1/s should compile?


// using namespace units;
// using namespace units::isq;

// struct metre : named_unit<metre, "m"> {};
// struct centimetre : prefixed_unit<centimetre, si::centi, metre> {};
// struct kilometre : prefixed_unit<kilometre, si::kilo, metre> {};
// struct yard : named_scaled_unit<yard, "yd", mag<ratio{9'144, 10'000}>(), metre> {};
// struct foot : named_scaled_unit<foot, "ft", mag<ratio(1, 3)>(), yard> {};
// struct dim_length : base_dimension<"length", metre> {};

// struct second : named_unit<second, "s"> {};
// struct hour : named_scaled_unit<hour, "h", mag<3600>(), second> {};
// struct dim_time : base_dimension<"time", second> {};

// struct kelvin : named_unit<kelvin, "K"> {};

// #if !UNITS_COMP_MSVC
// static_assert([]<Prefix P>(P) {
//   return !requires { typename prefixed_unit<struct kilokilometre, P, kilometre>; };
// }(si::kilo{}));  // no prefix allowed
// #endif

// struct metre_per_second : derived_unit<metre_per_second> {};
// struct dim_speed :
//     derived_dimension<dim_speed, metre_per_second, units::exponent<dim_length, 1>, units::exponent<dim_time, -1>> {};
// struct kilometre_per_hour : derived_scaled_unit<kilometre_per_hour, dim_speed, kilometre, hour> {};

// static_assert(equivalent<metre::named_unit, metre>);
// static_assert(equivalent<metre::scaled_unit, metre>);
// static_assert(compare<downcast<scaled_unit<mag<1>(), metre>>, metre>);
// static_assert(compare<downcast<scaled_unit<mag<ratio(1, 100)>(), metre>>, centimetre>);
// static_assert(compare<downcast<scaled_unit<yard::mag, metre>>, yard>);
// static_assert(compare<downcast<scaled_unit<yard::mag / mag<3>(), metre>>, foot>);
// static_assert(compare<downcast<scaled_unit<kilometre::mag / hour::mag, metre_per_second>>, kilometre_per_hour>);

// static_assert(centimetre::symbol == "cm");
// static_assert(kilometre::symbol == "km");
// static_assert(kilometre_per_hour::symbol == "km/h");


// static_assert(si::metre != si::kilometre);
// static_assert(!equivalent(si::metre, si::kilometre));
// static_assert(convertible(si::metre, si::kilometre));


}  // namespace
