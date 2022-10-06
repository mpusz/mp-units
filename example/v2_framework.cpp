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

#include <units/si/si.h>


template<typename T, typename Expr>
constexpr bool is_of_type(Expr)
{
  return std::is_same_v<Expr, T>;
}

namespace {

using namespace units;
using namespace units::si::unit_symbols;

constexpr auto power = 5 * si::power[W];
static_assert(is_of_type<quantity<reference<struct isq::power_dim, struct si::watt>{}, int>>(power));

constexpr auto speed = 5 * si::speed[m / s];
static_assert(
  is_of_type<quantity<reference<struct isq::speed_dim, derived_unit<struct si::metre, per<struct si::second>>>{}, int>>(
    speed));

constexpr auto q = 10 * si::length[m] / (2 * si::time[s]);
static_assert(is_of_type<quantity<reference<derived_dimension<struct isq::length_dim, per<struct isq::time_dim>>,
                                            derived_unit<struct si::metre, per<struct si::second>>>{},
                                  int>>(q));

constexpr auto distance = 5 * si::speed[m / s] * (5 * si::time[s]);

static_assert(is_of_type<quantity<reference<struct isq::length_dim, struct si::metre>{}, int>>(distance));

constexpr auto dimensionless = 20 * si::speed[m / s] / (10 * si::length[m]) * (5 * si::time[s]);

static_assert(is_of_type<quantity<reference<struct one_dim, struct one>{}, int>>(dimensionless));


// constexpr auto q1 = 10 * si::length[m] / (2 * si::time[s]) + 5 * si::speed[m / s];
// static_assert(is_of_type<quantity<reference<derived_dimension<struct isq::length_dim, per<struct isq::time_dim>>,
//                                             derived_unit<struct si::metre, per<struct si::second>>>{},
//                                   int>>(q1));

// static_assert(quantity_of<decltype(120 * si::length[km] / (2 * si::time[h])), isq::speed_dim>);
// static_assert(quantity_of<decltype(120 * si::length[km] / (2 * si::time[h])), si::speed[km / h]>);
// static_assert(!quantity_of<decltype(120 * si::length[km] / (2 * si::time[h])), si::speed[m / s]>);


// quantity<reference<speed_dim, derived_unit<si::metre, per<si::second>>>, int> s = 5 * speed[m / s];
// quantity<reference<derived_dimension<length_dim, per<time_dim>>, derived_unit<metre, per<second>>>, int> q =
//   10 * length[m] / (2 * si::time[s]);

// auto q1 = 10 * length[m] / (2 * si::time[s]) + 5 * speed[m / s];     // should this be allowed?
// bool b1 = (10 * length[m] / (2 * si::time[s]) == 5 * speed[m / s]);  // should this be allowed?

// auto q2 = 10 / (2 * si::time[s]) + 5 * frequency[Hz];     // should this be allowed?
// bool b2 = (10 / (2 * si::time[s]) == 5 * frequency[Hz]);  // should this be allowed?

// auto q3 = 5 * activity[Bq] + 5 * frequency[Hz];     // should this be allowed?
// auto b3 = (5 * activity[Bq] == 5 * frequency[Hz]);  // should this be allowed?

// auto q4 = 5 * activity[Bq] + 10 / (2 * si::time[s]) + 5 * frequency[Hz];  // should this be allowed?

// auto q5 = 120 * length[km] / (2 * si::time[h]);  // not speed
// auto q6 = quantity_cast<dim_speed>(120 * length[km] / (2 * si::time[h]));
// auto q7 = quantity_cast<speed[m / s]>(120 * length[km] / (2 * si::time[h]));
// quantity<speed[km / h]> s = q5;  // should this implicit conversion be allowed?

}  // namespace

namespace units::si {

// derived unit expression template syntax verification
static_assert(is_of_type<derived_unit<struct one, per<struct second>>>(1 / second));
static_assert(is_of_type<struct second>(1 / (1 / second)));

static_assert(is_of_type<struct second>(one * second));
static_assert(is_of_type<struct second>(second * one));
static_assert(is_of_type<derived_unit<struct one, per<struct second>>>(one * (1 / second)));
static_assert(is_of_type<derived_unit<struct one, per<struct second>>>(1 / second * one));

static_assert(is_of_type<derived_unit<struct metre, struct second>>(metre * second));
static_assert(is_of_type<derived_unit<units::power<struct metre, 2>>>(metre * metre));

static_assert(is_of_type<derived_unit<units::power<struct metre, 2>, struct second>>(metre * metre * second));
static_assert(is_of_type<derived_unit<units::power<struct metre, 2>, struct second>>(metre * second * metre));

static_assert(is_of_type<derived_unit<units::power<struct metre, 2>, struct second>>(metre * (second * metre)));
static_assert(is_of_type<derived_unit<units::power<struct metre, 2>, struct second>>(second * (metre * metre)));

static_assert(is_of_type<derived_unit<struct metre, per<struct second>>>(1 / second * metre));
static_assert(is_of_type<struct one>(1 / second * second));

static_assert(is_of_type<struct second>(second / one));
static_assert(is_of_type<derived_unit<struct one, per<struct second>>>(1 / second / one));

static_assert(is_of_type<struct metre>(metre / second * second));
static_assert(is_of_type<derived_unit<struct one, per<units::power<struct second, 2>>>>(1 / second * (1 / second)));
static_assert(is_of_type<derived_unit<struct one, per<units::power<struct second, 2>>>>(1 / (second * second)));
static_assert(is_of_type<derived_unit<units::power<struct second, 2>>>(1 / (1 / (second * second))));

static_assert(is_of_type<derived_unit<struct metre, per<units::power<struct second, 2>>>>(metre / second *
                                                                                          (1 / second)));
static_assert(is_of_type<derived_unit<units::power<struct metre, 2>, per<units::power<struct second, 2>>>>(
  metre / second * (metre / second)));
static_assert(is_of_type<struct one>(metre / second * (second / metre)));

static_assert(is_of_type<derived_unit<struct watt, per<struct joule>>>(watt / joule));
static_assert(is_of_type<derived_unit<struct joule, per<struct watt>>>(joule / watt));

// comparisons of equivalent units
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


// Bq + Hz should not compile

// Bq + Hz + 1/s should compile?


}  // namespace units::si

namespace units {

template<typename T, Dimension auto D, Unit auto U>
inline constexpr bool is_exactly_quantity_of =
  is_same_v<decltype(T::dimension), decltype(D)> && is_same_v<decltype(T::unit), decltype(U)>;

}

namespace units::isq::si {

// quantity tests
// static_assert(
//   is_exactly_quantity_of<decltype(4 * length[km] / (2 * length[m])), one_dim, derived_unit<kilometre, per<metre>>>);

// static_assert(QuantityOf<decltype(4 * length[km] / (2 * length[m])), one_dim, derived_unit<kilometre, per<metre>>);
// static_assert(QuantityOf<decltype(4 * length[km] / (2 * length[m])), one_dim, derived_unit<metre, per<millimetre>>);
// // TODO Should this compile?

}  // namespace units::isq::si

// using namespace units;
// using namespace units::si;
// using namespace units::si::unit_symbols;

// /* Frequency */ auto freq1 = 20 * frequency[Hz];
// // /* Frequency */ auto freq2 = 20 / (1 * si::time[s]);
// quantity<frequency[Hz]> freq3(20);
// quantity<frequency[1 / s]> freq4(20);
// quantity<dimensionless[one] / si::time[s]> freq5(20);

// /* Speed */ auto speed1 = 20 * speed[m / s];
// /* Speed */ auto speed2 = 20 * (length[m] / si::time[s]);
// quantity<speed[km / s]> speed3(20);
// quantity<length[m] / si::time[s]> speed4(20);

template<typename T>
void print();

// constexpr auto avg_speed(quantity<length[km]> d, quantity<si::time[h]> t) { return d / t; }

int main()
{
  // print<decltype(speed)>();
  // print<decltype(freq1)>();
  // // print<decltype(freq2)>();
  // print<decltype(freq3)>();
  // print<decltype(freq4)>();
  // print<decltype(freq5)>();

  // print<decltype(speed1)>();
  // print<decltype(speed2)>();
  // print<decltype(speed3)>();
  // print<decltype(speed4)>();
}


// 1 * joule + 1 * erg ???

// joule * erg???
// joule / erg???

// auto d1 = 42 * isq::length_dim[si::kilo<si::metre>];
// auto d2 = 42 * isq::length_dim[cgs::centimetre];

// auto s1 = 42 * isq::speed_dim[si::metre / si::second];
// auto s2 = 42 * isq::speed_dim[cgs::centimetre / si::second];
// auto e1 = 42 * isq::energy_dim[si::joule];
// auto e2 = 42 * isq::energy_dim[cgs::erg];
// auto e2_bad = 42 * isq::energy_dim[cgs::erg / si::second];
// auto p1 = 42 * isq::power_dim[si::watt];
// auto p2 = 42 * isq::power_dim[cgs::erg / si::second];

// type of Rep{1} * (mag<ratio(662'607'015, 100'000'000)> * mag_power<10, -34> * energy[joule] * time[second])
// and inline constexpr auto planck_constant = Rep{1} * mag_planck * energy[joule] * time[second];
