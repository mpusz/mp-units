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

namespace {

using namespace units;
using namespace units::si::unit_symbols;

// clang-format off
inline constexpr struct activity_dim : decltype(1 / isq::time_dim) {} activity_dim;
inline constexpr struct activity : system_reference<activity_dim, si::becquerel> {} activity;
// clang-format on

// check for invalid prefixes

template<template<auto U> typename prefix, Unit auto V1>
concept can_not_be_prefixed = !requires { typename prefix<V1>; };

static_assert(can_not_be_prefixed<si::milli_, si::degree_Celsius>);
static_assert(can_not_be_prefixed<si::milli_, si::minute>);
static_assert(can_not_be_prefixed<si::milli_, si::hour>);
static_assert(can_not_be_prefixed<si::milli_, si::day>);
static_assert(can_not_be_prefixed<si::milli_, si::kilogram>);
static_assert(can_not_be_prefixed<si::milli_, si::hectare>);
static_assert(can_not_be_prefixed<si::milli_, si::metre / si::second>);

// Named quantity/dimension and unit
static_assert(
  is_same_v<decltype(5 * si::power[W]), quantity<reference<struct isq::power_dim, struct si::watt>{}, int>>);

// Named quantity/dimension and derived (unnamed) unit
static_assert(
  is_same_v<decltype(5 * si::speed[m / s]),
            quantity<reference<struct isq::speed_dim, derived_unit<struct si::metre, per<struct si::second>>>{}, int>>);

// Derived (unnamed) quantity/dimension and derived (unnamed) unit
static_assert(is_same_v<decltype(10 * si::length[m] / (2 * si::time[s])),
                        quantity<reference<derived_dimension<struct isq::length_dim, per<struct isq::time_dim>>,
                                           derived_unit<struct si::metre, per<struct si::second>>>{},
                                 int>>);

// Base quantity as a result of dimensional transformation
static_assert(is_same_v<decltype(5 * si::speed[m / s] * (5 * si::time[s])),
                        quantity<reference<struct isq::length_dim, struct si::metre>{}, int>>);

// Dimensionless
static_assert(is_same_v<decltype(20 * si::speed[m / s] / (10 * si::length[m]) * (5 * si::time[s])),
                        quantity<reference<struct one_dim, struct one>{}, int>>);

// Comparisons

// Same dimension type & different unit
// static_assert(1000 * si::length[m] == 1 * si::length[km]);

// Named and derived dimensions (same units)
static_assert(10 * si::length[m] / (2 * si::time[s]) == 5 * si::speed[m / s]);
static_assert(5 * si::speed[m / s] == 10 * si::length[m] / (2 * si::time[s]));

// Same named dimension & different but equivalent unit
static_assert(10 * si::frequency[1 / s] == 10 * si::frequency[Hz]);
static_assert(10 * si::frequency[Hz] == 10 * si::frequency[1 / s]);

// Named and derived dimensions (different but equivalent units)
static_assert(10 / (2 * si::time[s]) == 5 * si::frequency[Hz]);
static_assert(5 * si::frequency[Hz] == 10 / (2 * si::time[s]));
static_assert(5 * si::force[N] * (2 * si::length[m]) == 10 * si::energy[J]);
static_assert(10 * si::energy[J] == 5 * si::force[N] * (2 * si::length[m]));

// Different named dimensions
template<Reference auto R1, Reference auto R2>
concept invalid_comparison = !requires { 2 * R1 == 2 * R2; } && !requires { 2 * R2 == 2 * R1; };
static_assert(invalid_comparison<activity[Bq], si::frequency[Hz]>);

// Arithmetics

// Named and derived dimensions (same units)
static_assert(10 * si::length[m] / (2 * si::time[s]) + 5 * si::speed[m / s] == 10 * si::speed[m / s]);
static_assert(5 * si::speed[m / s] + 10 * si::length[m] / (2 * si::time[s]) == 10 * si::speed[m / s]);
static_assert(10 * si::length[m] / (2 * si::time[s]) - 5 * si::speed[m / s] == 0 * si::speed[m / s]);
static_assert(5 * si::speed[m / s] - 10 * si::length[m] / (2 * si::time[s]) == 0 * si::speed[m / s]);
static_assert(
  is_same_v<decltype(10 * si::length[m] / (2 * si::time[s]) + 5 * si::speed[m / s]),
            quantity<reference<struct isq::speed_dim, derived_unit<struct si::metre, per<struct si::second>>>{}, int>>);
static_assert(
  is_same_v<decltype(5 * si::speed[m / s] + 10 * si::length[m] / (2 * si::time[s])),
            quantity<reference<struct isq::speed_dim, derived_unit<struct si::metre, per<struct si::second>>>{}, int>>);
static_assert(
  is_same_v<decltype(10 * si::length[m] / (2 * si::time[s]) - 5 * si::speed[m / s]),
            quantity<reference<struct isq::speed_dim, derived_unit<struct si::metre, per<struct si::second>>>{}, int>>);
static_assert(
  is_same_v<decltype(5 * si::speed[m / s] - 10 * si::length[m] / (2 * si::time[s])),
            quantity<reference<struct isq::speed_dim, derived_unit<struct si::metre, per<struct si::second>>>{}, int>>);

// Named and derived dimensions (different units)
static_assert(10 / (2 * si::time[s]) + 5 * si::frequency[Hz] == 10 * si::frequency[Hz]);
static_assert(5 * si::frequency[Hz] + 10 / (2 * si::time[s]) == 10 * si::frequency[Hz]);
static_assert(10 / (2 * si::time[s]) - 5 * si::frequency[Hz] == 0 * si::frequency[Hz]);
static_assert(5 * si::frequency[Hz] - 10 / (2 * si::time[s]) == 0 * si::frequency[Hz]);
static_assert(is_same_v<decltype(10 / (2 * si::time[s]) + 5 * si::frequency[Hz]),
                        quantity<reference<struct isq::frequency_dim, struct si::hertz>{}, int>>);
static_assert(is_same_v<decltype(5 * si::frequency[Hz] + 10 / (2 * si::time[s])),
                        quantity<reference<struct isq::frequency_dim, struct si::hertz>{}, int>>);
static_assert(is_same_v<decltype(10 / (2 * si::time[s]) - 5 * si::frequency[Hz]),
                        quantity<reference<struct isq::frequency_dim, struct si::hertz>{}, int>>);
static_assert(is_same_v<decltype(5 * si::frequency[Hz] - 10 / (2 * si::time[s])),
                        quantity<reference<struct isq::frequency_dim, struct si::hertz>{}, int>>);

// Different named dimensions
template<typename... Ts>
consteval bool invalid_arithmetic(Ts... ts)
{
  return !requires { (... + ts); } && !requires { (... - ts); };
}
static_assert(invalid_arithmetic(5 * activity[Bq], 5 * si::frequency[Hz]));
static_assert(invalid_arithmetic(5 * activity[Bq], 10 / (2 * si::time[s]), 5 * si::frequency[Hz]));

// Implicit conversions allowed between quantities of `convertible` references
constexpr quantity<si::speed[km / h]> speed = 120 * si::length[km] / (2 * si::time[h]);

// Explicit casts allow changing all or only a part of the type
static_assert(
  std::is_same_v<
    decltype(quantity_cast<isq::speed_dim>(120 * si::length[km] / (2 * si::time[h]))),
    quantity<reference<struct isq::speed_dim,
                       derived_unit<std::remove_const_t<decltype(si::kilo<si::metre>)>, per<struct si::hour>>>{},
             int>>);
auto q3 = quantity_cast<m / s>(120 * si::length[km] / (2 * si::time[h]));
auto q4 = quantity_cast<si::speed[m / s]>(120 * si::length[km] / (2 * si::time[h]));
auto q5 = quantity_cast<double>(120 * si::length[km] / (2 * si::time[h]));
auto q6 = quantity_cast<quantity<si::speed[m / s], double>>(120 * si::length[km] / (2 * si::time[h]));

// cast 1 / time_dim to use Hz

// static_assert(quantity_of<decltype(60 * si::speed[km / h]), isq::speed_dim>);
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


// quantity_cast on equivalent dimensions