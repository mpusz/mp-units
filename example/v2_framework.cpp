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

// #include <units/concepts.h>
#include <units/dimension.h>

namespace units::isq {

inline constexpr struct length_dim : base_dimension<"L"> {
} length_dim;
inline constexpr struct mass_dim : base_dimension<"M"> {
} mass_dim;
inline constexpr struct time_dim : base_dimension<"T"> {
} time_dim;
inline constexpr struct electric_current_dim : base_dimension<"I"> {
} electric_current_dim;
// TODO Should the below use basic_symbol_text? How to name it for ASCII?
inline constexpr struct thermodynamic_temperature_dim : base_dimension<"Θ"> {
} thermodynamic_temperature_dim;
inline constexpr struct amount_of_substance_dim : base_dimension<"N"> {
} amount_of_substance_dim;
inline constexpr struct luminous_intensity_dim : base_dimension<"J"> {
} luminous_intensity_dim;

inline constexpr struct frequency_dim : decltype(1 / time_dim) {
} frequency_dim;
inline constexpr struct area_dim : decltype(length_dim * length_dim) {
} area_dim;
inline constexpr struct volume_dim : decltype(area_dim * length_dim) {
} volume_dim;
inline constexpr struct speed_dim : decltype(length_dim / time_dim) {
} speed_dim;
inline constexpr struct acceleration_dim : decltype(speed_dim / time_dim) {
} acceleration_dim;

// inline constexpr auto speed = length / time;

// using speed_dim = decltype(length_dim / time_dim);
// inline constexpr speed_dim speed_dim;

// template<typename T>
// concept Length = QuantityOf<T, length_dim>;

}  // namespace units::isq

#include <units/isq/si/prefixes.h>
#include <units/unit.h>

namespace units {

namespace isq::si {

// length units
inline constexpr struct metre : named_unit<"m"> {
} metre;
inline constexpr struct kilometre : kilo<metre> {
} kilometre;

inline constexpr struct astronomical_unit : named_scaled_unit<"au", mag<149'597'870'700>(), metre> {
} astronomical_unit;

// area units
inline constexpr struct square_metre : derived_unit<decltype(metre * metre)> {
} square_metre;

// volume units
inline constexpr struct cubic_metre : derived_unit<decltype(metre * metre * metre)> {
} cubic_metre;

// time units
inline constexpr struct second : named_unit<"s"> {
} second;
inline constexpr struct minute : named_scaled_unit<"min", mag<60>(), second> {
} minute;
inline constexpr struct hour : named_scaled_unit<"h", mag<60>(), minute> {
} hour;
inline constexpr struct day : named_scaled_unit<"d", mag<24>(), hour> {
} day;

// not time units!
// TODO should those be provided for other scaled units like ms, h, ...
inline constexpr struct second_squared : derived_unit<decltype(second * second)> {
} second_squared;
inline constexpr struct second_cubed : derived_unit<decltype(second * second * second)> {
} second_cubed;

// mass units
inline constexpr struct gram : named_unit<"g"> {
} gram;
inline constexpr struct kilogram : kilo<gram> {
} kilogram;
inline constexpr struct tonne : named_scaled_unit<"t", mag<1000>(), gram> {
} tonne;

// other units
inline constexpr struct hertz : named_unit<"Hz", 1 / second> {
} hertz;
inline constexpr struct newton : named_unit<"N", kilogram * metre / second_squared> {
} newton;
inline constexpr struct pascal : named_unit<"Pa", kilogram / (metre * second_squared)> {
} pascal;
inline constexpr struct joule : named_unit<"J", newton * metre> {
} joule;
inline constexpr struct watt : named_unit<"W", joule / second> {
} watt;

namespace unit_symbols {

inline namespace length_units {

inline constexpr auto m = metre;
inline constexpr auto km = kilometre;
inline constexpr auto au = astronomical_unit;

}  // namespace length_units

inline namespace area_units {

inline constexpr auto m2 = square_metre;

}

inline namespace volume_units {

inline constexpr auto m3 = cubic_metre;

}

inline namespace time_units {

inline constexpr auto s = second;
inline constexpr auto min = minute;
inline constexpr auto h = hour;
inline constexpr auto d = day;

inline constexpr auto s2 = second_squared;

}  // namespace time_units

inline namespace mass_units {

inline constexpr auto g = gram;
inline constexpr auto kg = kilogram;
inline constexpr auto t = tonne;

}  // namespace mass_units

inline namespace frequency_units {

inline constexpr auto Hz = hertz;

}

inline namespace force_units {

inline constexpr auto N = newton;

}

inline namespace pressure_units {

inline constexpr auto Pa = pascal;

}

inline namespace energy_units {

inline constexpr auto J = joule;

}

inline namespace power_units {

inline constexpr auto W = watt;

}

}  // namespace unit_symbols

}  // namespace isq::si
}  // namespace units

#include <units/reference.h>

namespace units {

inline constexpr struct dimensionless : system_reference<dimensionless, one_dim, one> {
} dimensionless;

}  // namespace units

namespace units::isq::si {

inline constexpr struct length : system_reference<length, length_dim, metre> {
} length;
inline constexpr struct time : system_reference<time, time_dim, second> {
} time;
inline constexpr struct frequency : system_reference<frequency, frequency_dim, hertz> {
} frequency;
inline constexpr struct area : system_reference<area, area_dim, square_metre> {
} area;
inline constexpr struct volume : system_reference<volume, volume_dim, cubic_metre> {
} volume;
inline constexpr struct speed : system_reference<speed, speed_dim, metre / second> {
} speed;
inline constexpr struct acceleration : system_reference<acceleration, acceleration_dim, metre / second / second> {
} acceleration;

}  // namespace units::isq::si


template<auto V, typename T>
inline constexpr bool is_of_type = std::is_same_v<std::remove_cvref_t<decltype(V)>, T>;

namespace units::isq {

// derived dimension expression template syntax verification
static_assert(is_of_type<1 / time_dim, derived_dimension<struct one_dim, per<struct time_dim>>>);
static_assert(is_of_type<1 / (1 / time_dim), struct time_dim>);

static_assert(is_of_type<one_dim * time_dim, struct time_dim>);
static_assert(is_of_type<time_dim * one_dim, struct time_dim>);
static_assert(is_of_type<one_dim * (1 / time_dim), derived_dimension<struct one_dim, per<struct time_dim>>>);
static_assert(is_of_type<1 / time_dim * one_dim, derived_dimension<struct one_dim, per<struct time_dim>>>);

static_assert(is_of_type<length_dim * time_dim, derived_dimension<struct length_dim, struct time_dim>>);
static_assert(is_of_type<length_dim * length_dim, derived_dimension<power<struct length_dim, 2>>>);

static_assert(
  is_of_type<length_dim * length_dim * time_dim, derived_dimension<power<struct length_dim, 2>, struct time_dim>>);
static_assert(
  is_of_type<length_dim * time_dim * length_dim, derived_dimension<power<struct length_dim, 2>, struct time_dim>>);

static_assert(
  is_of_type<length_dim*(time_dim* length_dim), derived_dimension<power<struct length_dim, 2>, struct time_dim>>);
static_assert(
  is_of_type<time_dim*(length_dim* length_dim), derived_dimension<power<struct length_dim, 2>, struct time_dim>>);

static_assert(is_of_type<1 / time_dim * length_dim, derived_dimension<struct length_dim, per<struct time_dim>>>);
static_assert(is_of_type<1 / time_dim * time_dim, struct one_dim>);

static_assert(is_of_type<time_dim / one_dim, struct time_dim>);
static_assert(is_of_type<1 / time_dim / one_dim, derived_dimension<struct one_dim, per<struct time_dim>>>);

static_assert(is_of_type<length_dim / time_dim * time_dim, struct length_dim>);
static_assert(
  is_of_type<1 / time_dim * (1 / time_dim), derived_dimension<struct one_dim, per<power<struct time_dim, 2>>>>);
static_assert(is_of_type<1 / (time_dim * time_dim), derived_dimension<struct one_dim, per<power<struct time_dim, 2>>>>);
static_assert(is_of_type<1 / (1 / (time_dim * time_dim)), derived_dimension<power<struct time_dim, 2>>>);

static_assert(is_of_type<length_dim / time_dim * (1 / time_dim),
                         derived_dimension<struct length_dim, per<power<struct time_dim, 2>>>>);
static_assert(is_of_type<length_dim / time_dim*(length_dim / time_dim),
                         derived_dimension<power<struct length_dim, 2>, per<power<struct time_dim, 2>>>>);
static_assert(is_of_type<length_dim / time_dim*(time_dim / length_dim), struct one_dim>);

static_assert(is_of_type<speed_dim / acceleration_dim, struct time_dim>);
static_assert(is_of_type<acceleration_dim / speed_dim, derived_dimension<struct one_dim, per<struct time_dim>>>);
static_assert(
  is_of_type<speed_dim * speed_dim / length_dim, derived_dimension<struct length_dim, per<power<struct time_dim, 2>>>>);
static_assert(is_of_type<1 / (speed_dim * speed_dim) * length_dim,
                         derived_dimension<power<struct time_dim, 2>, per<struct length_dim>>>);

namespace si {

// comparisons of equivalent dimensions
static_assert(length_dim / length_dim == one_dim);

static_assert(1 / time_dim == frequency_dim);
static_assert(1 / frequency_dim == time_dim);
static_assert(frequency_dim * time_dim == one_dim);

static_assert(length_dim * length_dim == area_dim);
static_assert(length_dim * length_dim != volume_dim);
static_assert(area_dim / length_dim == length_dim);

static_assert(length_dim * length_dim * length_dim == volume_dim);
static_assert(area_dim * length_dim == volume_dim);
static_assert(volume_dim / length_dim == area_dim);
static_assert(volume_dim / length_dim / length_dim == length_dim);
static_assert(area_dim * area_dim / length_dim == volume_dim);
static_assert(area_dim * (area_dim / length_dim) == volume_dim);
static_assert(volume_dim / (length_dim * length_dim) == length_dim);

static_assert(length_dim / time_dim == speed_dim);
static_assert(length_dim * time_dim != speed_dim);
static_assert(length_dim / time_dim / time_dim != speed_dim);
static_assert(length_dim / speed_dim == time_dim);
static_assert(speed_dim * time_dim == length_dim);

static_assert(length_dim / time_dim / time_dim == acceleration_dim);
static_assert(length_dim / (time_dim * time_dim) == acceleration_dim);
static_assert(speed_dim / time_dim == acceleration_dim);
static_assert(speed_dim / acceleration_dim == time_dim);
static_assert(acceleration_dim * time_dim == speed_dim);
static_assert(acceleration_dim * (time_dim * time_dim) == length_dim);
static_assert(acceleration_dim / speed_dim == frequency_dim);

}  // namespace si

}  // namespace units::isq


namespace units::isq::si {

// derived unit expression template syntax verification
static_assert(is_of_type<1 / second, derived_unit<struct one, per<struct second>>>);
static_assert(is_of_type<1 / (1 / second), struct second>);

static_assert(is_of_type<one * second, struct second>);
static_assert(is_of_type<second * one, struct second>);
static_assert(is_of_type<one * (1 / second), derived_unit<struct one, per<struct second>>>);
static_assert(is_of_type<1 / second * one, derived_unit<struct one, per<struct second>>>);

static_assert(is_of_type<metre * second, derived_unit<struct metre, struct second>>);
static_assert(is_of_type<metre * metre, derived_unit<power<struct metre, 2>>>);

static_assert(is_of_type<metre * metre * second, derived_unit<power<struct metre, 2>, struct second>>);
static_assert(is_of_type<metre * second * metre, derived_unit<power<struct metre, 2>, struct second>>);

static_assert(is_of_type<metre*(second* metre), derived_unit<power<struct metre, 2>, struct second>>);
static_assert(is_of_type<second*(metre* metre), derived_unit<power<struct metre, 2>, struct second>>);

static_assert(is_of_type<1 / second * metre, derived_unit<struct metre, per<struct second>>>);
static_assert(is_of_type<1 / second * second, struct one>);

static_assert(is_of_type<second / one, struct second>);
static_assert(is_of_type<1 / second / one, derived_unit<struct one, per<struct second>>>);

static_assert(is_of_type<metre / second * second, struct metre>);
static_assert(is_of_type<1 / second * (1 / second), derived_unit<struct one, per<power<struct second, 2>>>>);
static_assert(is_of_type<1 / (second * second), derived_unit<struct one, per<power<struct second, 2>>>>);
static_assert(is_of_type<1 / (1 / (second * second)), derived_unit<power<struct second, 2>>>);

static_assert(is_of_type<metre / second * (1 / second), derived_unit<struct metre, per<power<struct second, 2>>>>);
static_assert(
  is_of_type<metre / second*(metre / second), derived_unit<power<struct metre, 2>, per<power<struct second, 2>>>>);
static_assert(is_of_type<metre / second*(second / metre), struct one>);

static_assert(is_of_type<watt / joule, derived_unit<struct watt, per<struct joule>>>);
static_assert(is_of_type<joule / watt, derived_unit<struct joule, per<struct watt>>>);

// comparisons of equivalent units
static_assert(metre / metre == one);
static_assert(metre * metre == square_metre);
static_assert(second * second == second_squared);
static_assert(second * second * second == second_cubed);
static_assert(second * (second * second) == second_cubed);
static_assert(second_squared * second == second_cubed);
static_assert(second * second_squared == second_cubed);

static_assert(1 / second * metre == metre / second);
static_assert(metre * (1 / second) == metre / second);
static_assert((metre / second) * (1 / second) == metre / second / second);
static_assert((metre / second) * (1 / second) == metre / (second * second));
static_assert((metre / second) * (1 / second) == metre / second_squared);

static_assert(hertz == 1 / second);
static_assert(newton == kilogram * metre / second_squared);
static_assert(joule == kilogram * square_metre / second_squared);
static_assert(joule == newton * metre);
static_assert(watt == joule / second);
static_assert(watt == kilogram * square_metre / second_cubed);

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


// Bq + Hz should not compile

// Bq + Hz + 1/s should compile?


}  // namespace units::isq::si


template<typename T, Dimension auto D, Unit auto U>
inline constexpr bool is_exactly_quantity_of =
  is_same_v<decltype(T::dimension), decltype(D)> && is_same_v<decltype(T::unit), decltype(U)>;

namespace units::isq::si {

// quantity tests
static_assert(
  is_exactly_quantity_of<decltype(4 * length[km] / (2 * length[m])), one_dim, derived_unit<kilometre, per<metre>>>);

// static_assert(QuantityOf<decltype(4 * length[km] / (2 * length[m])), one_dim, derived_unit<kilometre, per<metre>>);
// static_assert(QuantityOf<decltype(4 * length[km] / (2 * length[m])), one_dim, derived_unit<metre, per<millimetre>>);
// // TODO Should this compile?

}  // namespace units::isq::si


using namespace units;
using namespace units::isq::si;
using namespace units::isq::si::unit_symbols;

/* Frequency */ auto freq1 = 20 * frequency[Hz];
// /* Frequency */ auto freq2 = 20 / (1 * isq::si::time[s]);
quantity<frequency[Hz]> freq3(20);
quantity<frequency[1 / s]> freq4(20);
quantity<dimensionless[one] / isq::si::time[s]> freq5(20);

/* Speed */ auto speed1 = 20 * speed[m / s];
/* Speed */ auto speed2 = 20 * (length[m] / isq::si::time[s]);
quantity<speed[m / s]> speed3(20);
quantity<length[m] / isq::si::time[s]> speed4(20);

template<typename T>
void print();

// constexpr auto avg_speed(quantity<length[km]> d, quantity<isq::si::time[h]> t) { return d / t; }

int main()
{
  print<decltype(freq1)>();
  // print<decltype(freq2)>();
  print<decltype(freq3)>();
  print<decltype(freq4)>();
  print<decltype(freq5)>();

  print<decltype(speed1)>();
  print<decltype(speed2)>();
  print<decltype(speed3)>();
  print<decltype(speed4)>();
}
