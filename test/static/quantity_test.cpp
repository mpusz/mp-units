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
#include <mp-units/bits/hacks.h>
#include <mp-units/ext/fixed_string.h>
#include <mp-units/ext/type_traits.h>
#include <mp-units/systems/isq/electromagnetism.h>
#include <mp-units/systems/isq/mechanics.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si.h>
#if MP_UNITS_HOSTED
#include <mp-units/cartesian_vector.h>
#include <mp-units/math.h>
#endif
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <concepts>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>
#if MP_UNITS_HOSTED
#include <chrono>
#include <complex>
#endif
#endif

namespace {

using namespace mp_units;
using namespace mp_units::si::unit_symbols;
#if MP_UNITS_HOSTED
using namespace std::complex_literals;
using v = cartesian_vector<double>;
#endif

//////////////////////////////
// quantity class invariants
//////////////////////////////

static_assert(sizeof(quantity<si::metre>) == sizeof(double));
static_assert(sizeof(quantity<isq::length[m]>) == sizeof(double));
static_assert(sizeof(quantity<si::metre, short>) == sizeof(short));
static_assert(sizeof(quantity<isq::length[m], short>) == sizeof(short));

template<template<auto, typename> typename Q>
concept invalid_types = requires {
  requires !requires { typename Q<isq::dim_length, double>; };  // dimension instead of reference
  requires !requires { typename Q<isq::length, double>; };      // quantity_spec instead of reference
  requires !requires { typename Q<isq::length[m], bool>; };     // bool is not a valid representation type
  requires !requires { typename Q<isq::length[m], quantity<isq::length[m]>>; };  // quantity used as Rep
#if MP_UNITS_HOSTED
  requires !requires {
    typename Q<isq::position_vector[si::metre], std::complex<double>>;
  };  // vector representation expected
  requires !requires {
    typename Q<isq::length[si::metre], cartesian_vector<double>>;
  };  // scalar representation expected
  requires !requires { typename Q<isq::voltage[V], std::complex<double>>; };  // incompatible character
  requires !requires { typename Q<isq::voltage_phasor[V], double>; };         // incompatible character
#endif
};
static_assert(invalid_types<quantity>);

static_assert(std::is_trivially_default_constructible_v<quantity<isq::length[m]>>);
static_assert(std::is_trivially_copy_constructible_v<quantity<isq::length[m]>>);
static_assert(std::is_trivially_move_constructible_v<quantity<isq::length[m]>>);
static_assert(std::is_trivially_copy_assignable_v<quantity<isq::length[m]>>);
static_assert(std::is_trivially_move_assignable_v<quantity<isq::length[m]>>);
static_assert(std::is_trivially_destructible_v<quantity<isq::length[m]>>);

static_assert(std::is_nothrow_default_constructible_v<quantity<isq::length[m]>>);
static_assert(std::is_nothrow_copy_constructible_v<quantity<isq::length[m]>>);
static_assert(std::is_nothrow_move_constructible_v<quantity<isq::length[m]>>);
static_assert(std::is_nothrow_copy_assignable_v<quantity<isq::length[m]>>);
static_assert(std::is_nothrow_move_assignable_v<quantity<isq::length[m]>>);
static_assert(std::is_nothrow_destructible_v<quantity<isq::length[m]>>);

static_assert(std::is_trivially_copyable_v<quantity<isq::length[m]>>);
static_assert(std::is_standard_layout_v<quantity<isq::length[m]>>);

static_assert(std::default_initializable<quantity<isq::length[m]>>);
static_assert(std::move_constructible<quantity<isq::length[m]>>);
static_assert(std::copy_constructible<quantity<isq::length[m]>>);
static_assert(std::equality_comparable<quantity<isq::length[m]>>);
static_assert(std::totally_ordered<quantity<isq::length[m]>>);
static_assert(std::regular<quantity<isq::length[m]>>);

static_assert(std::three_way_comparable<quantity<isq::length[m]>>);


//////////////////
// member values
//////////////////

static_assert(quantity<si::metre>::reference == si::metre);
static_assert(quantity<si::metre>::quantity_spec == kind_of<isq::length>);
static_assert(quantity<si::metre>::dimension == isq::dim_length);
static_assert(quantity<si::metre>::unit == si::metre);

static_assert(quantity<isq::length[m]>::reference == isq::length[m]);
static_assert(quantity<isq::length[m]>::quantity_spec == isq::length);
static_assert(quantity<isq::length[m]>::dimension == isq::dim_length);
static_assert(quantity<isq::length[m]>::unit == si::metre);

static_assert(quantity<one>::reference == one);
static_assert(quantity<one>::quantity_spec == kind_of<dimensionless>);
static_assert(quantity<one>::dimension == dimension_one);
static_assert(quantity<one>::unit == one);

static_assert(quantity<dimensionless[one]>::reference == dimensionless[one]);
static_assert(quantity<dimensionless[one]>::quantity_spec == dimensionless);
static_assert(quantity<dimensionless[one]>::dimension == dimension_one);
static_assert(quantity<dimensionless[one]>::unit == one);

static_assert(quantity<si::radian>::reference == si::radian);
static_assert(quantity<si::radian>::quantity_spec == kind_of<isq::angular_measure>);
static_assert(quantity<si::radian>::dimension == dimension_one);
static_assert(quantity<si::radian>::unit == si::radian);

static_assert(quantity<isq::angular_measure[rad]>::reference == isq::angular_measure[rad]);
static_assert(quantity<isq::angular_measure[rad]>::quantity_spec == isq::angular_measure);
static_assert(quantity<isq::angular_measure[rad]>::dimension == dimension_one);
static_assert(quantity<isq::angular_measure[rad]>::unit == si::radian);

/////////////////
// member types
/////////////////

static_assert(is_same_v<quantity<isq::length[m]>::rep, double>);
static_assert(is_same_v<quantity<isq::length[m], int>::rep, int>);

[[maybe_unused]] volatile std::int16_t vint = 123;
static_assert(is_same_v<decltype(quantity(vint * m))::rep, std::int16_t>);
static_assert(is_same_v<decltype(quantity(vint, m))::rep, std::int16_t>);


////////////////////////////
// static member functions
////////////////////////////

static_assert(quantity<isq::length[m], int>::zero().numerical_value_in(m) == 0);
static_assert(quantity<isq::length[m], int>::min().numerical_value_in(m) == std::numeric_limits<int>::lowest());
static_assert(quantity<isq::length[m], int>::max().numerical_value_in(m) == std::numeric_limits<int>::max());
static_assert(quantity<isq::length[m], double>::zero().numerical_value_in(m) == 0.0);
static_assert(quantity<isq::length[m], double>::min().numerical_value_in(m) == std::numeric_limits<double>::lowest());
static_assert(quantity<isq::length[m], double>::max().numerical_value_in(m) == std::numeric_limits<double>::max());


/////////////////////////////////////////////////
// no construction from value (unless unit one)
/////////////////////////////////////////////////

// construction from a value is private
static_assert(!std::constructible_from<quantity<isq::length[m]>, double>);
static_assert(!std::convertible_to<double, quantity<isq::length[m]>>);

static_assert(!std::constructible_from<quantity<isq::length[m], int>, int>);
static_assert(!std::convertible_to<int, quantity<isq::length[m], int>>);

static_assert(std::constructible_from<quantity<one>, double>);
static_assert(std::convertible_to<double, quantity<one>>);

static_assert(std::constructible_from<quantity<one>, int>);
static_assert(std::convertible_to<int, quantity<one>>);

static_assert(std::constructible_from<quantity<dimensionless[one]>, double>);
static_assert(std::convertible_to<double, quantity<dimensionless[one]>>);

static_assert(std::constructible_from<quantity<dimensionless[one]>, int>);
static_assert(std::convertible_to<int, quantity<dimensionless[one]>>);


///////////////////////////////////////
// construction from another quantity
///////////////////////////////////////

// conversion only between convertible quantities
static_assert(std::constructible_from<quantity<isq::length[m]>, quantity<isq::length[m]>>);
static_assert(std::convertible_to<quantity<isq::length[m]>, quantity<isq::length[m]>>);
static_assert(std::constructible_from<quantity<isq::length[m]>, quantity<isq::distance[m]>>);
static_assert(std::convertible_to<quantity<isq::distance[m]>, quantity<isq::length[m]>>);
static_assert(std::constructible_from<quantity<isq::length[m]>, quantity<isq::length[km]>>);
static_assert(std::convertible_to<quantity<isq::length[km]>, quantity<isq::length[m]>>);

#if MP_UNITS_HOSTED
static_assert(std::constructible_from<quantity<si::volt, std::complex<double>>,
                                      quantity<isq::voltage_phasor[V], std::complex<double>>>);
static_assert(std::convertible_to<quantity<isq::voltage_phasor[V], std::complex<double>>,
                                  quantity<si::volt, std::complex<double>>>);
static_assert(std::constructible_from<quantity<isq::voltage_phasor[V], std::complex<double>>,
                                      quantity<si::volt, std::complex<double>>>);
static_assert(std::convertible_to<quantity<si::volt, std::complex<double>>,
                                  quantity<isq::voltage_phasor[V], std::complex<double>>>);

static_assert(std::constructible_from<quantity<si::metre, cartesian_vector<double>>,
                                      quantity<isq::position_vector[m], cartesian_vector<double>>>);
static_assert(std::convertible_to<quantity<isq::position_vector[m], cartesian_vector<double>>,
                                  quantity<si::metre, cartesian_vector<double>>>);
static_assert(std::constructible_from<quantity<isq::position_vector[m], cartesian_vector<double>>,
                                      quantity<si::metre, cartesian_vector<double>>>);
static_assert(std::convertible_to<quantity<si::metre, cartesian_vector<double>>,
                                  quantity<isq::position_vector[m], cartesian_vector<double>>>);
#endif


// conversion between different quantities not allowed
static_assert(!std::constructible_from<quantity<isq::length[m]>, quantity<isq::time[s]>>);
static_assert(!std::convertible_to<quantity<isq::time[s]>, quantity<isq::length[m]>>);
static_assert(!std::constructible_from<quantity<isq::length[m]>, quantity<isq::speed[m / s]>>);
static_assert(!std::convertible_to<quantity<isq::speed[m / s]>, quantity<isq::length[m]>>);

// implicit conversion from another quantity only if non-truncating
static_assert(std::constructible_from<quantity<isq::length[m]>, quantity<isq::length[m], int>>);  // int -> double OK
static_assert(std::convertible_to<quantity<isq::length[m], int>, quantity<isq::length[m]>>);      // int -> double OK

static_assert(!std::constructible_from<quantity<isq::length[m], int>,
                                       quantity<isq::length[m]>>);  // truncating double -> int not allowed
static_assert(!std::convertible_to<quantity<isq::length[m]>,
                                   quantity<isq::length[m], int>>);  // truncating double -> int not allowed

static_assert(std::constructible_from<quantity<isq::length[m], int>,
                                      quantity<isq::length[km], int>>);  // kilometre<int> -> metre<int> OK
static_assert(std::convertible_to<quantity<isq::length[km], int>,
                                  quantity<isq::length[m], int>>);  // kilometre<int> -> metre<int> OK

static_assert(!std::constructible_from<quantity<isq::length[km], int>,
                                       quantity<isq::length[m], int>>);  // truncating metre<int> ->
                                                                         // kilometre<int> not allowed
static_assert(
  !std::convertible_to<quantity<isq::length[m], int>,
                       quantity<isq::length[km], int>>);  // truncating metre<int> -> kilometre<int> not allowed

// converting to double always OK
static_assert(std::constructible_from<quantity<isq::length[m]>, quantity<isq::length[km], int>>);
static_assert(std::convertible_to<quantity<isq::length[km], int>, quantity<isq::length[m]>>);
static_assert(std::constructible_from<quantity<isq::length[km]>, quantity<isq::length[m], int>>);
static_assert(std::convertible_to<quantity<isq::length[m], int>, quantity<isq::length[km]>>);

static_assert(!std::convertible_to<quantity<isq::angular_measure[rad]>, quantity<dimensionless[one]>>);
static_assert(!std::convertible_to<quantity<isq::angular_measure[rad]>, quantity<one>>);
static_assert(!std::convertible_to<quantity<rad>, quantity<one>>);
static_assert(!std::convertible_to<quantity<isq::angular_measure[one]>, quantity<dimensionless[one]>>);
static_assert(!std::convertible_to<quantity<isq::angular_measure[one]>, quantity<one>>);

// explicit-construction only
static_assert(std::constructible_from<quantity<isq::distance[m]>, quantity<isq::length[m]>>);
static_assert(!std::convertible_to<quantity<isq::length[m]>, quantity<isq::distance[m]>>);


///////////////////////
// obtaining a number
///////////////////////

static_assert(quantity<isq::length[m], int>(123 * m).numerical_value_in(m) == 123);
static_assert(quantity<isq::length[m], int>(2 * km).numerical_value_in(m) == 2000);
static_assert(quantity<isq::length[km], int>(2 * km).numerical_value_in(km) == 2);
static_assert(quantity<isq::length[km]>(1500 * m).numerical_value_in(km) == 1.5);


////////////////////////////////////////////////////////
// explicit conversion to a number (when unit is one)
////////////////////////////////////////////////////////

static_assert(!std::convertible_to<quantity<one>, double>);
static_assert(std::constructible_from<double, quantity<one>>);
static_assert(!std::convertible_to<quantity<one>, int>);
static_assert(std::constructible_from<int, quantity<one>>);
static_assert(!std::convertible_to<quantity<one, int>, double>);
static_assert(std::constructible_from<double, quantity<one, int>>);
static_assert(!std::convertible_to<quantity<isq::rotation[one]>, double>);
static_assert(std::constructible_from<double, quantity<isq::rotation[one]>>);
static_assert(!std::convertible_to<quantity<isq::rotation[one]>, int>);
static_assert(std::constructible_from<int, quantity<isq::rotation[one]>>);
static_assert(!std::convertible_to<quantity<isq::rotation[one], int>, double>);
static_assert(std::constructible_from<double, quantity<isq::rotation[one], int>>);
#if MP_UNITS_HOSTED
static_assert(!std::convertible_to<quantity<one, std::complex<double>>, std::complex<double>>);
static_assert(std::constructible_from<std::complex<double>, quantity<one, std::complex<double>>>);
static_assert(!std::convertible_to<quantity<one, double>, std::complex<double>>);
static_assert(std::constructible_from<std::complex<double>, quantity<one, double>>);
static_assert(!std::convertible_to<quantity<one, cartesian_vector<double>>, cartesian_vector<double>>);
static_assert(std::constructible_from<cartesian_vector<double>, quantity<one, cartesian_vector<double>>>);
static_assert(!std::convertible_to<quantity<one, double>, cartesian_vector<double>>);
static_assert(std::constructible_from<cartesian_vector<double>, quantity<one, double>>);
#endif

static_assert(!std::convertible_to<quantity<rad>, double>);
static_assert(!std::constructible_from<double, quantity<rad>>);
static_assert(!std::convertible_to<quantity<rad>, int>);
static_assert(!std::constructible_from<int, quantity<rad>>);
static_assert(!std::convertible_to<quantity<rad, int>, double>);
static_assert(!std::constructible_from<double, quantity<rad, int>>);
static_assert(!std::convertible_to<quantity<isq::angular_measure[one]>, double>);
static_assert(!std::constructible_from<double, quantity<isq::angular_measure[one]>>);
static_assert(!std::convertible_to<quantity<isq::angular_measure[one]>, int>);
static_assert(!std::constructible_from<int, quantity<isq::angular_measure[one]>>);
static_assert(!std::convertible_to<quantity<isq::angular_measure[one], int>, double>);
static_assert(!std::constructible_from<double, quantity<isq::angular_measure[one], int>>);

///////////////////////////////////
// converting to a different unit
///////////////////////////////////

static_assert(is_of_type<(2. * km).in(m), quantity<si::metre>>);
static_assert(is_of_type<isq::length(2. * km).in(m), quantity<isq::length[m]>>);
static_assert(is_of_type<isq::height(2. * km).in(m), quantity<isq::height[m]>>);

static_assert(is_of_type<(2 * km).in<double>(m), quantity<si::metre>>);
static_assert(is_of_type<isq::length(2 * km).in<double>(m), quantity<isq::length[m]>>);
static_assert(is_of_type<isq::height(2 * km).in<double>(m), quantity<isq::height[m]>>);

static_assert(is_of_type<(2 * m).in<double>(), quantity<si::metre>>);
static_assert(is_of_type<isq::length(2 * m).in<double>(), quantity<isq::length[m]>>);
static_assert(is_of_type<isq::height(2 * m).in<double>(), quantity<isq::height[m]>>);

static_assert(quantity<isq::length[km]>(2. * km).in(km).numerical_value_in(km) == 2.);
static_assert(quantity<isq::length[km]>(2. * km).in(m).numerical_value_in(m) == 2000.);
static_assert(quantity<isq::length[m]>(2000. * m).in(km).numerical_value_in(km) == 2.);
static_assert(quantity<isq::length[km], int>(2 * km).in(km).numerical_value_in(km) == 2);
static_assert(quantity<isq::length[km], int>(2 * km).in(m).numerical_value_in(m) == 2000);

static_assert(is_of_type<(2. * km).force_in(m), quantity<si::metre>>);
static_assert(is_of_type<isq::length(2. * km).force_in(m), quantity<isq::length[m]>>);
static_assert(is_of_type<isq::height(2. * km).force_in(m), quantity<isq::height[m]>>);

static_assert(quantity<isq::length[km]>(2. * km).force_in(km).numerical_value_in(km) == 2.);
static_assert(quantity<isq::length[km]>(2. * km).force_in(m).numerical_value_in(m) == 2000.);
static_assert(quantity<isq::length[m]>(2000. * m).force_in(km).numerical_value_in(km) == 2.);
static_assert(quantity<isq::length[km], int>(2 * km).force_in(km).numerical_value_in(km) == 2);
static_assert(quantity<isq::length[km], int>(2 * km).force_in(m).numerical_value_in(m) == 2000);
static_assert(quantity<isq::length[m], int>(2000 * m).force_in(km).numerical_value_in(km) == 2);

static_assert((15. * m).in(nm).numerical_value_in(m) == 15.);
static_assert((15'000. * nm).in(m).numerical_value_in(nm) == 15'000.);

// check if unit conversion works - don't bother about the actual result
static_assert((1. * rad + 1. * deg).in(rad) != 0 * rad);
static_assert((1. * rad + 1. * deg).in(deg) != 0 * deg);
static_assert((1. * rad + 1. * deg).in(one) != 0 * one);

#if MP_UNITS_HOSTED
static_assert(((2.f + 1if) * isq::voltage_phasor[V]).in(mV).numerical_value_in(mV) == 2000.f + 1000if);
static_assert(((2.f + 1if) * isq::voltage_phasor[V]).in(mV).numerical_value_in(V) == 2.f + 1if);
static_assert(((2. + 1i) * isq::voltage_phasor[V]).in(mV).numerical_value_in(mV) == 2000. + 1000i);
static_assert(((2. + 1i) * isq::voltage_phasor[V]).in(mV).numerical_value_in(V) == 2. + 1i);
static_assert(((2.L + 1il) * isq::voltage_phasor[V]).in(mV).numerical_value_in(mV) == 2000.L + 1000il);
static_assert(((2.L + 1il) * isq::voltage_phasor[V]).in(mV).numerical_value_in(V) == 2.L + 1il);

static_assert((v{1., 2., 3.} * isq::position_vector[m]).in(mm).numerical_value_in(mm) == v{1000., 2000., 3000.});
static_assert((v{1., 2., 3.} * isq::position_vector[m]).in(mm).numerical_value_in(m) == v{1., 2., 3.});
#endif

template<template<auto, typename> typename Q>
concept invalid_unit_conversion = requires {
  requires !requires { Q<isq::length[m], int>(2000 * m).in(km); };     // truncating conversion
  requires !requires { Q<isq::length[m], int>(2 * m).in(s); };         // unit of a different quantity & dimension
  requires !requires { Q<isq::frequency[Hz], int>(60 * Hz).in(Bq); };  // unit of a different kind (same dimension)

  requires !requires { Q<isq::length[m], int>(2 * m).force_in(s); };  // unit of a different quantity & dimension
  requires !requires {
    Q<isq::frequency[Hz], int>(60 * Hz).force_in(Bq);
  };  // unit of a different kind (same dimension)
};
static_assert(invalid_unit_conversion<quantity>);

static_assert(quantity<isq::length[km]>(2. * km).numerical_value_in(km) == 2.);
static_assert(quantity<isq::length[km]>(2. * km).numerical_value_in(m) == 2000.);
static_assert(quantity<isq::length[m]>(2000. * m).numerical_value_in(km) == 2.);
static_assert(quantity<isq::length[km], int>(2 * km).numerical_value_in(km) == 2);
static_assert(quantity<isq::length[km], int>(2 * km).numerical_value_in(m) == 2000);

template<template<auto, typename> typename Q>
concept invalid_getter_with_unit_conversion = requires {
  requires !requires { Q<isq::length[m], int>(2000 * m).numerical_value_in(km); };  // truncating conversion
  requires !requires { Q<isq::length[m], int>(2 * m).numerical_value_in(s); };      // invalid unit
};
static_assert(invalid_getter_with_unit_conversion<quantity>);


///////////////////////////////////////
// derived quantities
///////////////////////////////////////

template<Reference auto R, basic_fixed_string additional_nttp_argument,
         RepresentationOf<get_quantity_spec(R)> Rep = double>
struct child_quantity : quantity<R, Rep> {
  using quantity_type = quantity<R, Rep>;
  static constexpr auto reference = R;
  static constexpr auto quantity_spec = quantity_type::quantity_spec;
  static constexpr auto dimension = quantity_type::dimension;
  static constexpr auto unit = quantity_type::unit;
  using rep = Rep;

  child_quantity() = default;
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  constexpr explicit(false) child_quantity(const quantity_type& t) : quantity_type(t) {}
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  constexpr explicit(false) child_quantity(quantity_type&& t) : quantity_type(std::move(t)) {}

  constexpr child_quantity& operator=(const quantity_type& q)
  {
    quantity_type::operator=(q);
    return *this;
  }
  constexpr child_quantity& operator=(quantity_type&& q)
  {
    quantity_type::operator=(std::move(q));
    return *this;
  }
  // NOLINTBEGIN(google-explicit-constructor, hicpp-explicit-conversions)
  constexpr explicit(false) operator quantity_type&() & noexcept { return *this; }
  constexpr explicit(false) operator const quantity_type&() const& noexcept { return *this; }
  constexpr explicit(false) operator quantity_type&&() && noexcept { return *this; }
  constexpr explicit(false) operator const quantity_type&&() const&& noexcept { return *this; }
  // NOLINTEND(google-explicit-constructor, hicpp-explicit-conversions)
};

static_assert(Quantity<child_quantity<isq::length[m], "NTTP type description">>);

constexpr QuantityOf<isq::length> auto get_length_child_quantity() noexcept
{
  child_quantity<isq::length[m], "NTTP type description"> dist{};
  dist += 1 * m;
  dist = dist + 1 * m;
  dist *= 0.5;
  return dist;
}
static_assert(get_length_child_quantity() == 1 * m);


/////////
// CTAD
/////////

static_assert(std::is_same_v<decltype(quantity{123 * m})::rep, int>);
static_assert(std::is_same_v<decltype(quantity{123. * m})::rep, double>);
static_assert(quantity{123. * m}.unit == si::metre);
static_assert(quantity{123. * m}.quantity_spec == kind_of<isq::length>);
static_assert(quantity{123. * h}.unit == si::hour);
static_assert(quantity{123. * h}.quantity_spec == kind_of<isq::time>);
static_assert(std::is_same_v<decltype(quantity{123})::rep, int>);
static_assert(std::is_same_v<decltype(quantity{123.})::rep, double>);
static_assert(quantity{123}.unit == one);
static_assert(quantity{123}.quantity_spec == kind_of<dimensionless>);

#if MP_UNITS_HOSTED
static_assert(quantity{123. + 1i}.unit == one);
static_assert(quantity{123. + 1i}.quantity_spec == kind_of<dimensionless>);

static_assert(quantity{v{1., 2., 3}}.unit == one);
static_assert(quantity{v{1., 2., 3}}.quantity_spec == kind_of<dimensionless>);

using namespace std::chrono_literals;
static_assert(std::is_same_v<decltype(quantity{123s})::rep, std::chrono::seconds::rep>);
static_assert(std::is_same_v<decltype(quantity{123.s})::rep, long double>);
static_assert(quantity{24h}.unit == si::hour);
static_assert(quantity{24h}.quantity_spec == kind_of<isq::time>);
#endif

////////////////////////
// assignment operator
////////////////////////

static_assert([] {
  auto l1 = 1 * m, l2 = 2 * m;
  return l2 = l1;
}()
                .numerical_value_in(m) == 1);
static_assert([] {
  const auto l1 = 1 * m;
  auto l2 = 2 * m;
  return l2 = l1;
}()
                .numerical_value_in(m) == 1);
static_assert([]() {
  auto l1 = 1 * m, l2 = 2 * m;
  return l2 = std::move(l1);  // NOLINT(*-move-const-arg)
}()
                .numerical_value_in(m) == 1);

static_assert([]() {
  quantity q(1);
  return q = 2;
}()
                .numerical_value_in(one) == 2);

////////////////////
// unary operators
////////////////////

static_assert((+123 * m).numerical_value_in(m) == 123);
static_assert((-123 * m).numerical_value_in(m) == -123);
static_assert((+(-123 * m)).numerical_value_in(m) == -123);
static_assert((-(-123 * m)).numerical_value_in(m) == 123);

static_assert([](auto val) {
  const auto vv = val++;  // NOLINT(bugprone-inc-dec-in-conditions)
  return std::pair(val, vv);
}(123 * m) == std::pair(124 * m, 123 * m));
static_assert([](auto val) {
  const auto vv = ++val;  // NOLINT(bugprone-inc-dec-in-conditions)
  return std::pair(val, vv);
}(123 * m) == std::pair(124 * m, 124 * m));
static_assert([](auto val) {
  const auto vv = val--;  // NOLINT(bugprone-inc-dec-in-conditions)
  return std::pair(val, vv);
}(123 * m) == std::pair(122 * m, 123 * m));
static_assert([](auto val) {
  const auto vv = --val;  // NOLINT(bugprone-inc-dec-in-conditions)
  return std::pair(val, vv);
}(123 * m) == std::pair(122 * m, 122 * m));

static_assert(is_same_v<decltype((+(short{0} * m)).numerical_value_in(m)), int>);


////////////////////////
// compound assignment
////////////////////////

// same type
static_assert([q = 1 * m]() mutable { return q += 1 * m; }().numerical_value_in(m) == 2);
static_assert([q = 2 * m]() mutable { return q -= 1 * m; }().numerical_value_in(m) == 1);
static_assert([q = 1 * m]() mutable { return q *= 2; }().numerical_value_in(m) == 2);
static_assert([q = 2 * m]() mutable { return q /= 2; }().numerical_value_in(m) == 1);
static_assert([q = 1 * m]() mutable { return q *= 2 * one; }().numerical_value_in(m) == 2);
static_assert([q = 2 * m]() mutable { return q /= 2 * one; }().numerical_value_in(m) == 1);
static_assert([q = 7 * m]() mutable { return q %= 2 * m; }().numerical_value_in(m) == 1);

#if MP_UNITS_HOSTED
static_assert([q = (1. + 1i) * V]() mutable { return q += (1. + 1i) * V; }().numerical_value_in(V) == 2. + 2i);
static_assert([q = (2. + 2i) * V]() mutable { return q -= (1. + 1i) * V; }().numerical_value_in(V) == 1. + 1i);
static_assert([q = (1. + 1i) * V]() mutable { return q += 1. * V; }().numerical_value_in(V) == 2. + 1i);
static_assert([q = (2. + 2i) * V]() mutable { return q -= 1. * V; }().numerical_value_in(V) == 1. + 2i);
static_assert([q = (1. + 1i) * V]() mutable { return q *= 2.; }().numerical_value_in(V) == 2. + 2i);
static_assert([q = (2. + 2i) * V]() mutable { return q /= 2.; }().numerical_value_in(V) == 1. + 1i);
static_assert([q = (1. + 1i) * V]() mutable { return q *= 2. * one; }().numerical_value_in(V) == 2. + 2i);
static_assert([q = (2. + 2i) * V]() mutable { return q /= 2. * one; }().numerical_value_in(V) == 1. + 1i);
static_assert([q = (1. + 1i) * V]() mutable { return q *= (2. + 1i) * one; }().numerical_value_in(V) ==
              (1. + 1i) * (2. + 1i));
static_assert([q = (2. + 2i) * V]() mutable { return q /= (2. + 1i) * one; }().numerical_value_in(V) ==
              (2. + 2i) / (2. + 1i));

static_assert([q = v{1., 2., 3.} * m]() mutable { return q += v{1., 2., 3.} * m; }().numerical_value_in(m) ==
              v{2., 4., 6.});
static_assert([q = v{2., 4., 6.} * m]() mutable { return q -= v{1., 2., 3.} * m; }().numerical_value_in(m) ==
              v{1., 2., 3.});
static_assert([q = v{1., 2., 3.} * m]() mutable { return q *= 2.; }().numerical_value_in(m) == v{2., 4., 6.});
static_assert([q = v{2., 4., 6.} * m]() mutable { return q /= 2.; }().numerical_value_in(m) == v{1., 2., 3.});
static_assert([q = v{1., 2., 3.} * m]() mutable { return q *= 2. * one; }().numerical_value_in(m) == v{2., 4., 6.});
static_assert([q = v{2., 4., 6.} * m]() mutable { return q /= 2. * one; }().numerical_value_in(m) == v{1., 2., 3.});
#endif

// different representation types
static_assert([q = 2.5 * m]() mutable { return q += 3 * m; }().numerical_value_in(m) == 5.5);
static_assert([q = 5.5 * m]() mutable { return q -= 3 * m; }().numerical_value_in(m) == 2.5);
static_assert([q = 2.5 * m]() mutable { return q *= 3; }().numerical_value_in(m) == 7.5);
static_assert([q = 7.5 * m]() mutable { return q /= 3; }().numerical_value_in(m) == 2.5);
static_assert([q = 2.5 * m]() mutable { return q *= 3 * one; }().numerical_value_in(m) == 7.5);
static_assert([q = 7.5 * m]() mutable { return q /= 3 * one; }().numerical_value_in(m) == 2.5);

// different units
static_assert([q = 1 * m]() mutable { return q += 1 * km; }().numerical_value_in(m) == 1001);
static_assert([q = 2000 * m]() mutable { return q -= 1 * km; }().numerical_value_in(m) == 1000);
static_assert([q = 3500 * m]() mutable { return q %= 1 * km; }().numerical_value_in(m) == 500);
#if MP_UNITS_HOSTED
static_assert([q = (1000. + 1000i) * V]() mutable { return q += (1. + 1i) * kV; }().numerical_value_in(V) ==
              2000. + 2000i);
static_assert([q = (2000. + 2000i) * V]() mutable { return q -= (1. + 1i) * kV; }().numerical_value_in(V) ==
              1000. + 1000i);
static_assert([q = (v{1000., 2000., 3000.} * m)]() mutable { return q += v{1., 2., 3.} * km; }().numerical_value_in(
                m) == v{2000., 4000., 6000.});
static_assert([q = (v{2000., 4000., 6000.} * m)]() mutable { return q -= v{1., 2., 3.} * km; }().numerical_value_in(
                m) == v{1000., 2000., 3000.});
#endif

// convertible quantity types
static_assert([q = isq::length(1 * m)]() mutable { return q += isq::height(1 * m); }().numerical_value_in(m) == 2);
static_assert([q = isq::length(2 * m)]() mutable { return q -= isq::height(1 * m); }().numerical_value_in(m) == 1);
static_assert([q = isq::length(7 * m)]() mutable { return q %= isq::height(2 * m); }().numerical_value_in(m) == 1);

static_assert([q = std::uint8_t{255} * m]() mutable { return q %= 256 * m; }().numerical_value_in(m) ==
              [] {
                std::uint8_t ui(255);
                return ui %= 256;
              }());
static_assert([q = std::uint8_t{255} * m]() mutable { return q %= 257 * m; }().numerical_value_in(m) ==
              [] {
                std::uint8_t ui(255);
                return ui %= 257;
              }());

// lack of consistency with binary operator
static_assert(
  is_of_type<1 * (isq::length / isq::time)[m / s] + 1 * isq::speed[m / s], quantity<isq::speed[m / s], int>>);
static_assert(is_of_type<[q = 1 * (isq::length / isq::time)[m / s]]() mutable { return q += 1 * isq::speed[m / s]; }(),
                         quantity<(isq::length / isq::time)[m / s], int>>);

template<template<auto, typename> typename Q>
concept invalid_compound_assignments = requires() {
  // truncating not allowed
  requires !requires(Q<isq::length[m], int> l) { l += 2.5 * m; };
  requires !requires(Q<isq::length[m], int> l) { l -= 2.5 * m; };
  requires !requires(Q<isq::length[km], int> l) { l += 2 * isq::length[m]; };
  requires !requires(Q<isq::length[km], int> l) { l -= 2 * isq::length[m]; };
  requires !requires(Q<isq::length[km], int> l) { l %= 2 * isq::length[m]; };
  requires !requires(Q<isq::length[km], int> l) { l %= 2 * percent; };
  requires !requires(Q<isq::length[km], int> l) { l %= 2. * percent; };
  requires !requires(Q<isq::length[m], int> l) { l *= 2.5; };
  requires !requires(Q<isq::length[m], int> l) { l /= 2.5; };
  requires !requires(Q<isq::length[m], int> l) { l *= 2.5 * one; };
  requires !requires(Q<isq::length[m], int> l) { l /= 2.5 * one; };

  // compound assignment with a non-convertible quantity not allowed
  requires !requires(Q<isq::height[m], int> l) { l += 2 * isq::length[m]; };
  requires !requires(Q<isq::height[m], int> l) { l -= 2 * isq::length[m]; };
  requires !requires(Q<isq::height[m], int> l) { l %= 2 * isq::length[m]; };

  // dimensionless quantities with a unit different than `one`
  requires !requires(Q<isq::length[km], int> l) { l *= 1 * (km / m); };
  requires !requires(Q<isq::length[km], int> l) { l /= 1 * (km / m); };
  requires !requires(Q<isq::length[km], int> l) { l %= 1 * (km / m); };

  // only quantities can be added or subtracted
  requires !requires(Q<isq::length[m], int> l) { l += 2; };
  requires !requires(Q<isq::length[m], int> l) { l -= 2; };

  // compound multiply/divide by another quantity not allowed
  requires !requires(Q<isq::length[m], int> l) { l *= 2 * m; };
  requires !requires(Q<isq::length[m], int> l) { l /= 2 * m; };

  // modulo operations on a floating point representation not allowed
  requires !requires(Q<isq::length[m], double> l) { l %= 2.; };
  requires !requires(Q<isq::length[m], double> l) { l %= 2; };
  requires !requires(Q<isq::length[m], double> l) { l %= 2. * m; };
  requires !requires(Q<isq::length[m], double> l) { l %= 2 * m; };
  requires !requires(Q<isq::length[m], int> l) { l %= 2. * m; };

#if MP_UNITS_HOSTED
  // modulo operations on a complex representation type not allowed
  requires !requires(Q<si::volt, std::complex<double>> l) { l %= (2. + 2i) * V; };

  // modulo operations on a vector representation type not allowed
  requires !requires(Q<si::metre, cartesian_vector<double>> l) { l %= v{2.} * m; };

  // arithmetics of vector and scalar quantities not allowed
  requires !requires(Q<si::metre, cartesian_vector<double>> l) { l += 2. * m; };
  requires !requires(Q<si::metre, cartesian_vector<double>> l) { l -= 2. * m; };

  // multiplication and division of vector quantities not allowed
  requires !requires(Q<si::metre, cartesian_vector<double>> l) { l *= v{1., 2., 3.}; };
  requires !requires(Q<si::metre, cartesian_vector<double>> l) { l /= v{1., 2., 3.}; };
  requires !requires(Q<si::metre, cartesian_vector<double>> l) { l *= v{1., 2., 3.} * one; };
  requires !requires(Q<si::metre, cartesian_vector<double>> l) { l /= v{1., 2., 3.} * one; };
  requires !requires(Q<si::metre, cartesian_vector<double>> l) { l *= v{1., 2., 3.} * m; };
  requires !requires(Q<si::metre, cartesian_vector<double>> l) { l /= v{1., 2., 3.} * m; };
#endif

  // no unit constants
  requires !requires(Q<isq::length[m], int> l) { l += m; };
  requires !requires(Q<isq::length[m], int> l) { l -= m; };
  requires !requires(Q<isq::length[m], int> l) { l *= m; };
  requires !requires(Q<isq::length[m], int> l) { l /= m; };
  requires !requires(Q<isq::length[m], int> l) { l %= m; };
};
static_assert(invalid_compound_assignments<quantity>);


////////////////////
// binary operators
////////////////////

template<template<auto, typename> typename Q>
concept invalid_binary_operations = requires {
  // no crossdimensional addition and subtraction
  requires !requires { 1 * s + Q<isq::length[m], int>(1 * m); };
  requires !requires { 1 * s - Q<isq::length[m], int>(1 * m); };

  // no floating-point modulo
  requires !requires(Q<isq::length[m], double> a) { a % 2 * m; };
  requires !requires(Q<isq::length[m], double> a) { 2 * m % a; };
  requires !requires(Q<isq::length[m], double> a) { a % 2; };
  requires !requires(Q<isq::length[m], double> a, Q<isq::length[m], double> b) { a % b; };
  requires !requires(Q<isq::length[m], double> a, Q<isq::length[m], int> b) { a % b; };
  requires !requires(Q<isq::length[m], double> a, Q<isq::length[m], int> b) { b % a; };

#if MP_UNITS_HOSTED
  // no complex modulo
  requires !requires(Q<si::volt, std::complex<double>> a) { a % ((2. + 2i) * V); };
  // no vector modulo
  requires !requires(Q<si::metre, cartesian_vector<double>> a) { a % (v{2.} * m); };
#endif

  // unit constants
  requires !requires { Q<isq::length[m], int>(1) + m; };
  requires !requires { Q<isq::length[m], int>(1) - m; };
  requires !requires { Q<isq::length[m], int>(1) % m; };
  requires !requires { m + Q<isq::length[m], int>(1); };
  requires !requires { m - Q<isq::length[m], int>(1); };
  requires !requires { m % Q<isq::length[m], int>(1); };
};
static_assert(invalid_binary_operations<quantity>);

// same representation type
static_assert(is_of_type<1 * m + 1 * m, quantity<si::metre, int>>);
static_assert(is_of_type<1 * m + 1 * km, quantity<si::metre, int>>);
static_assert(is_of_type<1 * km + 1 * m, quantity<si::metre, int>>);

static_assert(is_of_type<1 * m + isq::length(1 * m), quantity<isq::length[m], int>>);
static_assert(is_of_type<1 * m + isq::length(1 * km), quantity<isq::length[m], int>>);
static_assert(is_of_type<1 * km + isq::length(1 * m), quantity<isq::length[m], int>>);

static_assert(is_of_type<isq::length(1 * m) + 1 * m, quantity<isq::length[m], int>>);
static_assert(is_of_type<isq::length(1 * m) + 1 * km, quantity<isq::length[m], int>>);
static_assert(is_of_type<isq::length(1 * km) + 1 * m, quantity<isq::length[m], int>>);

static_assert(is_of_type<1 * m - 1 * m, quantity<si::metre, int>>);
static_assert(is_of_type<1 * km - 1 * m, quantity<si::metre, int>>);
static_assert(is_of_type<1 * m - 1 * km, quantity<si::metre, int>>);

static_assert(is_of_type<1 * m - isq::length(1 * m), quantity<isq::length[m], int>>);
static_assert(is_of_type<1 * m - isq::length(1 * km), quantity<isq::length[m], int>>);
static_assert(is_of_type<1 * km - isq::length(1 * m), quantity<isq::length[m], int>>);

static_assert(is_of_type<isq::length(1 * m) - 1 * m, quantity<isq::length[m], int>>);
static_assert(is_of_type<isq::length(1 * m) - 1 * km, quantity<isq::length[m], int>>);
static_assert(is_of_type<isq::length(1 * km) - 1 * m, quantity<isq::length[m], int>>);

static_assert(is_of_type<1 * N * (1 * m), quantity<derived_unit<struct si::metre, struct si::newton>{}, int>>);

static_assert(is_of_type<1 * m * 1, quantity<si::metre, int>>);
static_assert(is_of_type<1 * m * (1 * one), quantity<si::metre, int>>);
static_assert(is_of_type<1 * m * (1 * percent), quantity<derived_unit<struct percent, struct si::metre>{}, int>>);

static_assert(is_of_type<1 * (1 * m), quantity<si::metre, int>>);
static_assert(is_of_type<1 * one * (1 * m), quantity<si::metre, int>>);
static_assert(is_of_type<1 * percent * (1 * m), quantity<derived_unit<struct percent, struct si::metre>{}, int>>);

static_assert(is_of_type<1 * m / (1 * s), quantity<derived_unit<struct si::metre, per<struct si::second>>{}, int>>);
static_assert(is_of_type<1 * m / (1 * m), quantity<one, int>>);
static_assert(
  is_of_type<1 * km / (1 * m), quantity<derived_unit<si::kilo_<struct si::metre>, per<struct si::metre>>{}, int>>);

static_assert(is_of_type<1 * m / 1, quantity<si::metre, int>>);
static_assert(is_of_type<1 * m / (1 * one), quantity<si::metre, int>>);
static_assert(is_of_type<1 * m / (1 * percent), quantity<derived_unit<struct si::metre, per<struct percent>>{}, int>>);

static_assert(is_of_type<1 / (1 * s), quantity<derived_unit<struct one, per<struct si::second>>{}, int>>);
static_assert(is_of_type<1 / s, quantity<derived_unit<struct one, per<struct si::second>>{}, int>>);
static_assert(is_of_type<1 * one / (1 * s), quantity<derived_unit<struct one, per<struct si::second>>{}, int>>);
static_assert(is_of_type<1 * percent / (1 * s), quantity<derived_unit<struct percent, per<struct si::second>>{}, int>>);

static_assert(is_of_type<4 * m % (2 * m), quantity<si::metre, int>>);
static_assert(is_of_type<1'234 * m % (1 * km), quantity<si::metre, int>>);
static_assert(is_of_type<1 * km % (300 * m), quantity<si::metre, int>>);
static_assert(is_of_type<4 * one % (2 * one), quantity<one, int>>);

// check for integral types promotion
static_assert(is_same_v<decltype(std::uint8_t{0} * m + std::uint8_t{0} * m)::rep, int>);
static_assert(is_same_v<decltype(std::uint8_t{0} * m - std::uint8_t{0} * m)::rep, int>);
static_assert((std::uint8_t{128} * m + std::uint8_t{128} * m).numerical_value_in(m) ==
              std::uint8_t{128} + std::uint8_t{128});
static_assert((std::uint8_t{0} * m - std::uint8_t{1} * m).numerical_value_in(m) == std::uint8_t{0} - std::uint8_t{1});

static_assert(
  is_same_v<decltype((std::uint8_t{0} * m) % (std::uint8_t{0} * m))::rep, decltype(std::uint8_t{0} % std::uint8_t{0})>);

// different representation types
static_assert(is_of_type<1. * m + 1 * m, quantity<si::metre, double>>);
static_assert(is_of_type<1 * m + 1. * km, quantity<si::metre, double>>);
static_assert(is_of_type<1 * km + 1. * m, quantity<si::metre, double>>);

static_assert(is_of_type<1 * m - 1. * m, quantity<si::metre, double>>);
static_assert(is_of_type<1. * km - 1 * m, quantity<si::metre, double>>);
static_assert(is_of_type<1. * m - 1 * km, quantity<si::metre, double>>);

static_assert(is_of_type<1. * N * (1 * m), quantity<derived_unit<struct si::metre, struct si::newton>{}, double>>);

static_assert(is_of_type<1 * m * 1., quantity<si::metre, double>>);
static_assert(is_of_type<1 * m * (1. * one), quantity<si::metre, double>>);
static_assert(is_of_type<1 * m * (1. * percent), quantity<derived_unit<struct percent, struct si::metre>{}, double>>);

static_assert(is_of_type<1 * (1. * m), quantity<si::metre, double>>);
static_assert(is_of_type<1. * one * (1 * m), quantity<si::metre, double>>);
static_assert(is_of_type<1 * percent * (1. * m), quantity<derived_unit<struct percent, struct si::metre>{}, double>>);

static_assert(is_of_type<1 * m / (1. * s), quantity<derived_unit<struct si::metre, per<struct si::second>>{}, double>>);
static_assert(is_of_type<1. * m / (1 * m), quantity<one, double>>);
static_assert(
  is_of_type<1. * km / (1 * m), quantity<derived_unit<si::kilo_<struct si::metre>, per<struct si::metre>>{}, double>>);

static_assert(is_of_type<1. * m / 1, quantity<si::metre, double>>);
static_assert(is_of_type<1 * m / (1. * one), quantity<si::metre, double>>);
static_assert(
  is_of_type<1 * m / (1. * percent), quantity<derived_unit<struct si::metre, per<struct percent>>{}, double>>);

static_assert(is_of_type<1 / (1. * s), quantity<derived_unit<struct one, per<struct si::second>>{}, double>>);
static_assert(is_of_type<1. / s, quantity<derived_unit<struct one, per<struct si::second>>{}, double>>);
static_assert(is_of_type<1. * one / (1 * s), quantity<derived_unit<struct one, per<struct si::second>>{}, double>>);
static_assert(
  is_of_type<1 * percent / (1. * s), quantity<derived_unit<struct percent, per<struct si::second>>{}, double>>);

// different units
static_assert(is_of_type<1 * m + 1 * km, quantity<si::metre, int>>);
static_assert(is_of_type<1. * m + 1 * km, quantity<si::metre, double>>);
static_assert(is_of_type<1 * m + 1. * km, quantity<si::metre, double>>);
static_assert(is_of_type<1. * m + 1. * km, quantity<si::metre, double>>);

static_assert(is_of_type<1 * km + 1 * m, quantity<si::metre, int>>);
static_assert(is_of_type<1. * km + 1 * m, quantity<si::metre, double>>);
static_assert(is_of_type<1 * km + 1. * m, quantity<si::metre, double>>);
static_assert(is_of_type<1. * km + 1. * m, quantity<si::metre, double>>);

static_assert(is_of_type<1 * m - 1 * km, quantity<si::metre, int>>);
static_assert(is_of_type<1. * m - 1 * km, quantity<si::metre, double>>);
static_assert(is_of_type<1 * m - 1. * km, quantity<si::metre, double>>);
static_assert(is_of_type<1. * m - 1. * km, quantity<si::metre, double>>);

static_assert(is_of_type<1 * km - 1 * m, quantity<si::metre, int>>);
static_assert(is_of_type<1. * km - 1 * m, quantity<si::metre, double>>);
static_assert(is_of_type<1 * km - 1. * m, quantity<si::metre, double>>);
static_assert(is_of_type<1. * km - 1. * m, quantity<si::metre, double>>);

static_assert(is_of_type<1 * m % (1 * km), quantity<si::metre, int>>);

// different dimensions
static_assert(is_of_type<1 * m / s * (1 * s), quantity<si::metre, int>>);
static_assert(is_of_type<1 * m / s * (1 * h),
                         quantity<derived_unit<struct si::hour, struct si::metre, per<struct si::second>>{}, int>>);
static_assert(is_of_type<1 * m * (1 * min), quantity<derived_unit<struct si::minute, struct si::metre>{}, int>>);
static_assert(is_of_type<1 * s * (1 * Hz), quantity<derived_unit<struct si::hertz, struct si::second>{}, int>>);
static_assert(is_of_type<1 / (1 * min), quantity<derived_unit<struct one, per<struct si::minute>>{}, int>>);
static_assert(is_of_type<1 / (1 * Hz), quantity<derived_unit<struct one, per<struct si::hertz>>{}, int>>);
static_assert(is_of_type<1 / (1 * km), quantity<derived_unit<struct one, per<si::kilo_<struct si::metre>>>{}, int>>);
static_assert(is_of_type<1 / min, quantity<derived_unit<struct one, per<struct si::minute>>{}, int>>);
static_assert(is_of_type<1 / Hz, quantity<derived_unit<struct one, per<struct si::hertz>>{}, int>>);
static_assert(is_of_type<1 / km, quantity<derived_unit<struct one, per<si::kilo_<struct si::metre>>>{}, int>>);
static_assert(
  is_of_type<1 * km / (1 * m), quantity<derived_unit<si::kilo_<struct si::metre>, per<struct si::metre>>{}, int>>);
static_assert(is_of_type<1 * m / (1 * s), quantity<derived_unit<struct si::metre, per<struct si::second>>{}, int>>);
static_assert(is_of_type<1 * m / (1 * min), quantity<derived_unit<struct si::metre, per<struct si::minute>>{}, int>>);
static_assert(is_of_type<1 * min / (1 * m), quantity<derived_unit<struct si::minute, per<struct si::metre>>{}, int>>);

static_assert((1 * m + 1 * m).numerical_value_in(m) == 2);
static_assert((1 * m + 1 * km).numerical_value_in(m) == 1001);
static_assert((1 * km + 1 * m).numerical_value_in(m) == 1001);
static_assert((2 * m - 1 * m).numerical_value_in(m) == 1);
static_assert((1 * km - 1 * m).numerical_value_in(m) == 999);
static_assert((2 * m * 2).numerical_value_in(m) == 4);
static_assert((2 * m * (2 * one)).numerical_value_in(m) == 4);
static_assert((2 * m * (2 * percent)).numerical_value_in(percent * m) == 4);
static_assert((3 * 3 * m).numerical_value_in(m) == 9);
static_assert(((3 * one) * (3 * m)).numerical_value_in(m) == 9);
static_assert(((3 * percent) * (3 * m)).numerical_value_in(percent * m) == 9);
static_assert((4 * m / 2).numerical_value_in(m) == 2);
static_assert((4 * m / (2 * one)).numerical_value_in(m) == 2);
static_assert((4 * m / (2 * percent)).numerical_value_in(m / percent) == 2);
static_assert((4 * km / (2 * m)).numerical_value_in(km / m) == 2);
static_assert((4000 * m / (2 * m)).numerical_value_in(one) == 2000);

static_assert((1.5 * m + 1 * m).numerical_value_in(m) == 2.5);
static_assert((1.5 * m + 1 * km).numerical_value_in(m) == 1001.5);
static_assert((1.5 * km + 1 * m).numerical_value_in(m) == 1501);
static_assert((2.5 * m - 1 * m).numerical_value_in(m) == 1.5);
static_assert((1.5 * km - 1 * m).numerical_value_in(m) == 1499);
static_assert((2.5 * m * 2).numerical_value_in(m) == 5);
static_assert((2.5 * m * (2 * one)).numerical_value_in(m) == 5);
static_assert((2.5 * m * (2 * percent)).numerical_value_in(m * percent) == 5);
static_assert((2.5L * (2 * m)).numerical_value_in(m) == 5);
static_assert((2.5L * one * (2 * m)).numerical_value_in(m) == 5);
static_assert((2.5L * percent * (2 * m)).numerical_value_in(m * percent) == 5);
static_assert((5. * m / 2).numerical_value_in(m) == 2.5);
static_assert((5. * m / (2 * one)).numerical_value_in(m) == 2.5);
static_assert((5. * m / (2 * percent)).numerical_value_in(m / percent) == 2.5);
static_assert((5. * km / (2 * m)).numerical_value_in(km / m) == 2.5);
static_assert((5000. * m / (2 * m)).numerical_value_in(one) == 2500);

static_assert((1 * m + 1.5 * m).numerical_value_in(m) == 2.5);
static_assert((1 * m + 1.5 * km).numerical_value_in(m) == 1501);
static_assert((1 * km + 1.5 * m).numerical_value_in(m) == 1001.5);
static_assert((2 * m - 1.5 * m).numerical_value_in(m) == 0.5);
static_assert((1 * km - 1.5 * m).numerical_value_in(m) == 998.5);
static_assert((2 * m * 2.5L).numerical_value_in(m) == 5);
static_assert((2 * m * (2.5L * one)).numerical_value_in(m) == 5);
static_assert((2 * m * (2.5L * percent)).numerical_value_in(m * percent) == 5);
static_assert((2 * 2.5 * m).numerical_value_in(m) == 5);
static_assert((2 * one * (2.5 * m)).numerical_value_in(m) == 5);
static_assert((2 * percent * (2.5 * m)).numerical_value_in(m * percent) == 5);
static_assert((5 * m / 2.5L).numerical_value_in(m) == 2);
static_assert((5 * m / (2.5L * one)).numerical_value_in(m) == 2);
static_assert((5 * m / (2.5L * percent)).numerical_value_in(m / percent) == 2);
static_assert((5 * km / (2.5 * m)).numerical_value_in(km / m) == 2);
static_assert((5000 * m / (2.5 * m)).numerical_value_in(one) == 2000);

static_assert((7 * m % (2 * m)).numerical_value_in(m) == 1);
static_assert((7 * km % (2000 * m)).numerical_value_in(m) == 1000);
static_assert((1300 * m % (1 * km)).numerical_value_in(m) == 300);
static_assert((7 * one % (2 * one)).numerical_value_in(one) == 1);

static_assert((10 * m2 * (10 * m2)) / (50 * m2) == 2 * m2);

static_assert((10 * km / (5 * m)).numerical_value_in(km / m) == 2);
static_assert((10 * km / (5 * m)).numerical_value_in(one) == 2000);
static_assert((10 * s * (2 * kHz)).numerical_value_in(s * kHz) == 20);

// commutativity and associativity
static_assert(10 * isq::length[si::metre] / (2 * isq::time[s]) + 5 * isq::speed[m / s] == 10 * isq::speed[m / s]);
static_assert(5 * isq::speed[m / s] + 10 * isq::length[m] / (2 * isq::time[s]) == 10 * isq::speed[m / s]);
static_assert(10 * isq::length[m] / (2 * isq::time[s]) - 5 * isq::speed[m / s] == 0 * isq::speed[m / s]);
static_assert(5 * isq::speed[m / s] - 10 * isq::length[m] / (2 * isq::time[s]) == 0 * isq::speed[m / s]);

static_assert(
  is_of_type<10 * isq::length[m] / (2 * isq::time[s]) + 5 * isq::speed[m / s], quantity<isq::speed[m / s], int>>);
static_assert(
  is_of_type<5 * isq::speed[m / s] + 10 * isq::length[m] / (2 * isq::time[s]), quantity<isq::speed[m / s], int>>);
static_assert(
  is_of_type<10 * isq::length[m] / (2 * isq::time[s]) - 5 * isq::speed[m / s], quantity<isq::speed[m / s], int>>);
static_assert(
  is_of_type<5 * isq::speed[m / s] - 10 * isq::length[m] / (2 * isq::time[s]), quantity<isq::speed[m / s], int>>);

static_assert(10 / (2 * isq::time[s]) + 5 * isq::frequency[Hz] == 10 * isq::frequency[Hz]);
static_assert(5 * isq::frequency[Hz] + 10 / (2 * isq::time[s]) == 10 * isq::frequency[Hz]);
static_assert(10 / (2 * isq::time[s]) - 5 * isq::frequency[Hz] == 0 * isq::frequency[Hz]);
static_assert(5 * isq::frequency[Hz] - 10 / (2 * isq::time[s]) == 0 * isq::frequency[Hz]);

static_assert(
  is_of_type<10 / (2 * isq::period_duration[s]) + 5 * isq::frequency[Hz], quantity<isq::frequency[Hz], int>>);
static_assert(
  is_of_type<5 * isq::frequency[Hz] + 10 / (2 * isq::period_duration[s]), quantity<isq::frequency[Hz], int>>);
static_assert(
  is_of_type<10 / (2 * isq::period_duration[s]) - 5 * isq::frequency[Hz], quantity<isq::frequency[Hz], int>>);
static_assert(
  is_of_type<5 * isq::frequency[Hz] - 10 / (2 * isq::period_duration[s]), quantity<isq::frequency[Hz], int>>);

static_assert(
  is_of_type<isq::speed(1 * m / s) + isq::length(1. * m) / isq::time(1. * s), quantity<isq::speed[m / s], double>>);
static_assert(
  is_of_type<isq::length(1. * m) / isq::time(1. * s) + isq::speed(1 * m / s), quantity<isq::speed[m / s], double>>);
static_assert(
  is_of_type<isq::speed(1 * m / s) - isq::length(1. * m) / isq::time(1. * s), quantity<isq::speed[m / s], double>>);
static_assert(
  is_of_type<isq::length(1. * m) / isq::time(1. * s) - isq::speed(1 * m / s), quantity<isq::speed[m / s], double>>);

#if MP_UNITS_HOSTED
static_assert(is_same_v<decltype((isq::mass(1 * kg) * pow<2>(isq::length(1 * m) / isq::time(1 * s))).in(J) +
                                 isq::energy(1 * kg * m2 / s2)),
                        quantity<isq::energy[J], int>>);
static_assert(is_same_v<decltype(isq::energy(1 * kg * m2 / s2) +
                                 (isq::mass(1 * kg) * pow<2>(isq::length(1 * m) / isq::time(1 * s))).in(J)),
                        quantity<isq::energy[J], int>>);
static_assert(is_same_v<decltype((isq::mass(1 * kg) * pow<2>(isq::length(1 * m) / isq::time(1 * s)))
                                   .in(J)-isq::energy(1 * kg * m2 / s2)),
                        quantity<isq::energy[J], int>>);
static_assert(is_same_v<decltype(isq::energy(1 * kg * m2 / s2) -
                                 (isq::mass(1 * kg) * pow<2>(isq::length(1 * m) / isq::time(1 * s))).in(J)),
                        quantity<isq::energy[J], int>>);
#endif

static_assert(is_of_type<child_quantity<si::metre, "L">(1. * m) + 1 * m, quantity<si::metre>>);
static_assert(is_of_type<1 * m + child_quantity<si::metre, "L">(1. * m), quantity<si::metre>>);
static_assert(is_of_type<child_quantity<si::metre, "L">(1. * m) - 1 * m, quantity<si::metre>>);
static_assert(is_of_type<1 * m - child_quantity<si::metre, "L">(1. * m), quantity<si::metre>>);

// Different named dimensions
template<typename... Ts>
consteval bool invalid_arithmetic(Ts... ts)
{
  return !requires { (... + ts); } && !requires { (... - ts); };
}
static_assert(invalid_arithmetic(5 * isq::activity[Bq], 5 * isq::frequency[Hz]));
static_assert(invalid_arithmetic(5 * isq::activity[Bq], 10 / (2 * isq::time[s]), 5 * isq::frequency[Hz]));

// irrational conversion factors require floating point representation
static_assert(invalid_arithmetic(1 * rad, 1 * deg));
static_assert(is_of_type<1. * rad + 1 * deg, quantity<common_unit<struct si::degree, struct si::radian>{}, double>>);
static_assert(is_of_type<1 * rad + 1. * deg, quantity<common_unit<struct si::degree, struct si::radian>{}, double>>);
static_assert(is_of_type<1. * rad + 1. * deg, quantity<common_unit<struct si::degree, struct si::radian>{}, double>>);

// Physical constants
static_assert(1 * si::si2019::speed_of_light_in_vacuum + 10 * isq::speed[m / s] == 299'792'468 * isq::speed[m / s]);

// Implicit conversions allowed between quantities of `convertible` references
[[maybe_unused]] constexpr quantity<isq::speed[km / h]> speed = 120 * isq::length[km] / (2 * isq::time[h]);

// dimensionless
static_assert([q = 3 * one]() mutable { return q *= 2 * one; }() == 6 * one);
static_assert([q = 6 * one]() mutable { return q /= 2 * one; }() == 3 * one);
static_assert(1 * one + 1 * one == 2 * one);
static_assert(2 * one - 1 * one == 1 * one);
static_assert(2 * one * (2 * one) == 4 * one);
static_assert(2 * (2 * one) == 4 * one);
static_assert(2 * one * 2 == 4 * one);
static_assert(4 * one / (2 * one) == 2 * one);
static_assert(4 / (2 * one) == 2 * one);
static_assert(4 * one / 2 == 2 * one);
static_assert(4 * one % (2 * one) == 0 * one);

#if MP_UNITS_HOSTED
static_assert([q = (3. + 3i) * one]() mutable { return q *= (2. + 2i) * one; }() == (3. + 3i) * (2. + 2i) * one);
static_assert([q = (6. + 6i) * one]() mutable { return q /= (2. + 2i) * one; }() == (6. + 6i) / (2. + 2i) * one);
static_assert((1. + 1i) * one + (1. + 1i) * one == (2. + 2i) * one);
static_assert((2. + 2i) * one - (1. + 1i) * one == (1. + 1i) * one);
static_assert((2. + 2i) * one * (2. * one) == (4. + 4i) * one);
static_assert((2. + 2i) * ((2. + 2i) * one) == (2. + 2i) * (2. + 2i) * one);
static_assert((2. + 2i) * one * (2. + 2i) == (2. + 2i) * (2. + 2i) * one);
static_assert((4. + 4i) * one / ((2. + 2i) * one) == (4. + 4i) / (2. + 2i) * one);
static_assert((4. + 4i) / ((2. + 2i) * one) == (4. + 4i) / (2. + 2i) * one);
static_assert((4. + 4i) * one / (2. + 2i) == (4. + 4i) / (2. + 2i) * one);

static_assert(v{1., 2., 3.} * one + v{1., 2., 3.} * one == v{2., 4., 6.} * one);
static_assert(v{2., 4., 6.} * one - v{1., 2., 3.} * one == v{1., 2., 3.} * one);
#endif

static_assert(1 * one + 1 == 2);
static_assert(1 + 1 * one == 2);
static_assert(2 * one - 1 == 1);
static_assert(2 - 1 * one == 1);
static_assert(1 * one + 1.23 == 2.23);
static_assert(1 + 1.23 * one == 2.23);
static_assert(2.23 * one - 1 == 1.23);
static_assert(2.23 - 1 * one == 1.23);
static_assert(4 * one % (2) == 0);
static_assert(4 % (2 * one) == 0);

#if MP_UNITS_HOSTED
static_assert((1. + 1i) * one + (1. + 1i) == 2. + 2i);
static_assert((1. + 1i) + (1. + 1i) * one == 2. + 2i);
static_assert((2. + 2i) * one - (1. + 1i) == 1. + 1i);
static_assert((2. + 2i) - (1. + 1i) * one == 1. + 1i);
static_assert(1. * one + (1. + 1i) == (2. + 1i));
static_assert(1. + (1. + 1i) * one == (2. + 1i));
static_assert(2. * one - (1. + 1i) == (1. - 1i));
static_assert(2. - (1. + 1i) * one == (1. - 1i));

static_assert(v{1., 2., 3.} * one + v{1., 2., 3.} == v{2., 4., 6.});
static_assert(v{1., 2., 3.} + v{1., 2., 3.} * one == v{2., 4., 6.});
static_assert(v{2., 4., 6.} * one - v{1., 2., 3.} == v{1., 2., 3.});
static_assert(v{2., 4., 6.} - v{1., 2., 3.} * one == v{1., 2., 3.});
#endif

static_assert(2 * rad * (2 * rad) == 4 * pow<2>(rad));

// modulo arithmetics
static_assert(5 * h % (120 * min) == 60 * min);
static_assert(300 * min % (2 * h) == 60 * min);
static_assert(300 * min % (120 * min) == 60 * min);

constexpr auto quotient_remainder_theorem(auto q1, auto q2)
{
  auto quotient = q1 / q2;
  auto reminder = q1 % q2;
  auto q = quotient * q2 + reminder;
  return q;
}

// this works only if two quantities have the same unit
static_assert(quotient_remainder_theorem(7 * m, 3 * m) == 7 * m);
static_assert(quotient_remainder_theorem(3'000 * m, 400 * m) == 3'000 * m);

static_assert(is_same_v<decltype(0 * one + 0.0 * one), decltype(0.0 * one)>);
static_assert(is_same_v<decltype(0 * one - 0.0 * one), decltype(0.0 * one)>);
static_assert(is_same_v<decltype(0.0 * one + 0 * one), decltype(0.0 * one)>);
static_assert(is_same_v<decltype(0.0 * one - 0 * one), decltype(0.0 * one)>);

static_assert(1 * one - 30 * percent == (100 - 30) * percent);
static_assert(1 * one + 30 * percent == (100 + 30) * percent);

static_assert(is_same_v<decltype(std::uint8_t{0} * one + std::uint8_t{0} * one)::rep, int>);
static_assert(is_same_v<decltype(std::uint8_t{0} * one - std::uint8_t{0} * one)::rep, int>);
static_assert((std::uint8_t{128} * one + std::uint8_t{128} * one).numerical_value_in(one) ==
              std::uint8_t{128} + std::uint8_t{128});
static_assert((std::uint8_t{0} * one - std::uint8_t{1} * one).numerical_value_in(one) ==
              std::uint8_t{0} - std::uint8_t{1});
static_assert(is_same_v<decltype(std::uint8_t{0} * one % (std::uint8_t{0} * one))::rep,
                        decltype(std::uint8_t{0} % std::uint8_t{0})>);

static_assert(2 * one * (1 * m) == 2 * m);
static_assert(2 * one / (1 * m) == 2 / (1 * m));

#if MP_UNITS_HOSTED

///////////////////////
// complex quantities
///////////////////////

static_assert((1. + 1i) * V + (1. + 1i) * V == (2. + 2i) * V);
static_assert((1. + 1i) * V + 1. * V == (2. + 1i) * V);
static_assert((1000. + 1000i) * V + (1. + 1i) * kV == (2000. + 2000i) * V);
static_assert((1000. + 1000i) * V + 1. * kV == (2000. + 1000i) * V);
static_assert((2. + 2i) * V - (1. + 1i) * V == (1. + 1i) * V);
static_assert((2. + 2i) * V - 1. * V == (1. + 2i) * V);
static_assert((2000. + 2000i) * V - (1. + 1i) * kV == (1000. + 1000i) * V);
static_assert((2000. + 2000i) * V - 1. * kV == (1000. + 2000i) * V);
static_assert((1. + 1i) * V + isq::voltage_phasor((1. + 1i) * V) == (2. + 2i) * V);
static_assert(isq::voltage_phasor((1. + 1i) * V) + 1. * V == (2. + 1i) * V);
static_assert((2. + 2i) * V - isq::voltage_phasor((1. + 1i) * V) == (1. + 1i) * V);
static_assert(isq::voltage_phasor((2. + 2i) * V) - 1. * V == (1. + 2i) * V);

static_assert((1. + 1i) * V * 2. == (2. + 2i) * V);
static_assert((1. + 1i) * V * (2. * one) == (2. + 2i) * V);
static_assert((2. + 2i) * V / 2. == (1. + 1i) * V);
static_assert((2. + 2i) * V / (2. * one) == (1. + 1i) * V);
static_assert((1. + 1i) * V * (2. + 1i) == (1. + 1i) * (2. + 1i) * V);
static_assert((2. + 2i) * V / (2. + 1i) == (2. + 2i) / (2. + 1i) * V);
static_assert((1. + 1i) * V * (2. + 1i) * one == (1. + 1i) * (2. + 1i) * V);
static_assert((2. + 2i) * V / (2. + 1i) * one == (2. + 2i) / (2. + 1i) * V);
static_assert((1. + 1i) * V * (2. + 1i) * A == (1. + 1i) * (2. + 1i) * V * A);
static_assert((2. + 2i) * V / ((2. + 1i) * A) == (2. + 2i) / (2. + 1i) * V / A);

//////////////////////
// vector quantities
//////////////////////

static_assert(v{1., 2., 3.} * m + v{1., 2., 3.} * m == v{2., 4., 6.} * m);
static_assert(v{1000., 2000., 3000.} * m + v{1., 2., 3.} * km == v{2000., 4000., 6000.} * m);
static_assert(v{2., 4., 6.} * m - v{1., 2., 3.} * m == v{1., 2., 3.} * m);
static_assert(v{2000., 4000., 6000.} * m - v{1., 2., 3.} * km == v{1000., 2000., 3000.} * m);
static_assert(v{1., 2., 3.} * m + isq::displacement(v{1., 2., 3.} * m) == v{2., 4., 6.} * m);
static_assert(v{2., 4., 6.} * m - isq::displacement(v{1., 2., 3.} * m) == v{1., 2., 3.} * m);

static_assert(v{1., 2., 3.} * m * 2. == v{2., 4., 6.} * m);
static_assert(v{1., 2., 3.} * m * (2. * one) == v{2., 4., 6.} * m);
static_assert(v{1., 2., 3.} * m * (2. * s) == v{2., 4., 6.} * m * s);
static_assert(v{2., 4., 6.} * m / 2. == v{1., 2., 3.} * m);
static_assert(v{2., 4., 6.} * m / (2. * one) == v{1., 2., 3.} * m);
static_assert(v{2., 4., 6.} * m / (2. * s) == v{1., 2., 3.} * m / s);
#endif


///////////////////////
// equality operators
///////////////////////

static_assert(std::equality_comparable_with<quantity<si::metre>, quantity<si::metre>>);
static_assert(std::equality_comparable_with<quantity<si::metre>, quantity<si::metre, int>>);
static_assert(std::equality_comparable_with<quantity<si::metre>, quantity<si::kilo<si::metre>, int>>);
static_assert(std::equality_comparable_with<quantity<si::metre, int>, quantity<si::kilo<si::metre>, int>>);
static_assert(std::equality_comparable_with<quantity<isq::length[si::metre]>, quantity<si::metre>>);
static_assert(std::equality_comparable_with<quantity<isq::length[si::metre]>, quantity<si::metre, int>>);
static_assert(std::equality_comparable_with<quantity<isq::length[si::metre]>, quantity<si::kilo<si::metre>, int>>);
static_assert(std::equality_comparable_with<quantity<isq::width[si::metre]>, quantity<si::metre>>);
static_assert(std::equality_comparable_with<quantity<isq::width[si::metre]>, quantity<si::metre, int>>);
static_assert(std::equality_comparable_with<quantity<isq::width[si::metre]>, quantity<si::kilo<si::metre>, int>>);
static_assert(std::equality_comparable_with<quantity<isq::width[si::metre]>, quantity<isq::height[si::metre]>>);
static_assert(std::equality_comparable_with<quantity<isq::width[si::metre]>, quantity<isq::height[si::metre], int>>);
static_assert(
  std::equality_comparable_with<quantity<isq::width[si::metre]>, quantity<isq::height[si::kilo<si::metre>], int>>);

template<auto M>
concept no_crossdimensional_equality = requires {
  requires !requires { 1 * s == 1 * M; };
  requires !requires { 1 * s != 1 * M; };
};
static_assert(no_crossdimensional_equality<si::metre>);

// same type
static_assert(123 * m == 123 * m);
static_assert(321 * m != 123 * m);
static_assert(!(123 * m == 321 * m));
static_assert(!(123 * m != 123 * m));

// different types
static_assert(123. * m == 123 * m);
static_assert(321. * m != 123 * m);
static_assert(!(123. * m == 321 * m));
static_assert(!(123. * m != 123 * m));

static_assert(123 * km == 123'000 * m);
static_assert(321 * km != 123'000 * m);
static_assert(!(123 * km == 321'000 * m));
static_assert(!(123 * km != 123'000 * m));

// Named and derived dimensions (same units)
static_assert(10 * isq::length[m] / (2 * isq::time[s]) == 5 * isq::speed[m / s]);
static_assert(5 * isq::speed[m / s] == 10 * isq::length[m] / (2 * isq::time[s]));

// Same named dimension & different but equivalent unit
static_assert(10 * isq::frequency[one / s] == 10 * isq::frequency[Hz]);
static_assert(10 * isq::frequency[Hz] == 10 * isq::frequency[one / s]);

// Named and derived dimensions (different but equivalent units)
static_assert(10 / (2 * isq::time[s]) == 5 * isq::frequency[Hz]);
static_assert(5 * isq::frequency[Hz] == 10 / (2 * isq::time[s]));
static_assert(5 * isq::force[N] * (2 * isq::length[m]) == 10 * isq::mechanical_energy[J]);
static_assert(10 * isq::mechanical_energy[J] == 5 * isq::force[N] * (2 * isq::length[m]));

// Physical constants
static_assert(1 * si::si2019::speed_of_light_in_vacuum == 299'792'458 * isq::speed[m / s]);

// Different named dimensions
template<Reference auto R1, Reference auto R2>
constexpr bool invalid_comparison = !requires { 2 * R1 == 2 * R2; } && !requires { 2 * R2 == 2 * R1; };
static_assert(invalid_comparison<isq::activity[Bq], isq::frequency[Hz]>);

#if MP_UNITS_HOSTED
static_assert((1. + 1i) * one == 1. + 1i);
static_assert(1. + 1i != (2. + 2i) * one);
static_assert(v{1., 2., 3.} * one == v{1., 2., 3.});
static_assert(v{1., 2., 3.} != v{3., 2., 1.} * one);
#endif

///////////////////////
// ordering operators
///////////////////////

template<auto M>
concept no_crossdimensional_ordering = requires {
  requires !requires { 1 * s < 1 * M; };
  requires !requires { 1 * s > 1 * M; };
  requires !requires { 1 * s <= 1 * M; };
  requires !requires { 1 * s >= 1 * M; };
};
static_assert(no_crossdimensional_ordering<si::metre>);

// same type
static_assert(123 * m < 321 * m);
static_assert(123 * m <= 123 * m);
static_assert(123 * m <= 321 * m);
static_assert(321 * m > 123 * m);
static_assert(123 * m >= 123 * m);
static_assert(321 * m >= 123 * m);
static_assert(!(321 * m < 123 * m));
static_assert(!(123 * m < 123 * m));
static_assert(!(321 * m <= 123 * m));
static_assert(!(123 * m > 321 * m));
static_assert(!(123 * m > 123 * m));
static_assert(!(123 * m >= 321 * m));

// different types
static_assert(123. * m < 321 * m);
static_assert(123. * m <= 123 * m);
static_assert(123. * m <= 321 * m);
static_assert(321. * m > 123 * m);
static_assert(123. * m >= 123 * m);
static_assert(321. * m >= 123 * m);
static_assert(!(321. * m < 123 * m));
static_assert(!(123. * m < 123 * m));
static_assert(!(321. * m <= 123 * m));
static_assert(!(123. * m > 321 * m));
static_assert(!(123. * m > 123 * m));
static_assert(!(123. * m >= 321 * m));

static_assert(123 * km < 321'000 * m);
static_assert(123 * km <= 123'000 * m);
static_assert(123 * km <= 321'000 * m);
static_assert(321 * km > 123'000 * m);
static_assert(123 * km >= 123'000 * m);
static_assert(321 * km >= 123'000 * m);
static_assert(!(321 * km < 123'000 * m));
static_assert(!(123 * km < 123'000 * m));
static_assert(!(321 * km <= 123'000 * m));
static_assert(!(123 * km > 321'000 * m));
static_assert(!(123 * km > 123'000 * m));
static_assert(!(123 * km >= 321'000 * m));

static_assert(1 * one < 2);
static_assert(1 < 2 * one);


//////////////////
// dimensionless
//////////////////

static_assert(is_of_type<10 * km / (5 * km), quantity<one, int>>);

static_assert((50. * m / (100. * m)).numerical_value_in(percent) == 50);
static_assert(50. * m / (100. * m) == 50 * percent);

static_assert((50. * percent).numerical_value_in(one) == 0.5);


//////////////////
// common_type
//////////////////

static_assert(
  is_same_v<std::common_type_t<quantity<isq::speed[m / s], int>, quantity<(isq::length / isq::time)[m / s], double>>,
            quantity<isq::speed[m / s], double>>);
static_assert(is_same_v<std::common_type_t<quantity<(isq::mass * pow<2>(isq::length / isq::time))[J], double>,
                                           quantity<isq::energy[kg * m2 / s2], double>>,
                        quantity<isq::energy[J], double>>);

static_assert(is_same_v<std::common_type_t<quantity<one, int>, int>, quantity<one, int>>);
static_assert(is_same_v<std::common_type_t<quantity<one, int>, double>, quantity<one, double>>);
static_assert(is_same_v<std::common_type_t<quantity<one, double>, int>, quantity<one, double>>);
static_assert(is_same_v<std::common_type_t<quantity<isq::angular_measure[one], int>, int>,
                        quantity<isq::angular_measure[one], int>>);
static_assert(is_same_v<std::common_type_t<quantity<isq::angular_measure[one], int>, double>,
                        quantity<isq::angular_measure[one], double>>);
static_assert(is_same_v<std::common_type_t<quantity<isq::angular_measure[one], double>, int>,
                        quantity<isq::angular_measure[one], double>>);


//////////////////
// value_cast
//////////////////

static_assert(value_cast<m>(2 * km).numerical_value_in(m) == 2000);
static_assert(value_cast<km>(2000 * m).numerical_value_in(km) == 2);
static_assert(value_cast<km / h>(2000.0 * m / (3600.0 * s)).numerical_value_in(km / h) == 2);

static_assert(value_cast<int>(1.23 * m).numerical_value_in(m) == 1);
static_assert(value_cast<km, int>(1.23 * m).numerical_value_in(km) == 0);
static_assert(value_cast<int, km>(1.23 * m).numerical_value_in(km) == 0);

static_assert((2 * km).force_in(m).numerical_value_in(m) == 2000);
static_assert((2000 * m).force_in(km).numerical_value_in(km) == 2);
static_assert((2000.0 * m / (3600.0 * s)).force_in(km / h).numerical_value_in(km / h) == 2);

static_assert((1.23 * m).force_in<int>().numerical_value_in(m) == 1);
static_assert((1.23 * m).force_in<int>(km).numerical_value_in(km) == 0);

//////////////////
// quantity_cast
//////////////////

static_assert(is_of_type<quantity_cast<isq::distance>(1 * m), quantity<isq::distance[m], int>>);
static_assert(is_of_type<quantity_cast<isq::distance>(isq::length(1 * m)), quantity<isq::distance[m], int>>);
static_assert(is_of_type<quantity_cast<kind_of<isq::length>>(isq::length(1 * m)), quantity<si::metre, int>>);
static_assert(is_of_type<quantity_cast<kind_of<isq::length>>(isq::distance(1 * m)), quantity<si::metre, int>>);

static_assert(is_of_type<quantity_cast<dimensionless>(1. * isq::angular_measure[one]), quantity<dimensionless[one]>>);
static_assert(
  is_of_type<quantity_cast<dimensionless>((1. * isq::angular_measure[rad]).in(one)), quantity<dimensionless[one]>>);
static_assert(is_of_type<quantity_cast<dimensionless>((1. * rad).in(one)), quantity<dimensionless[one]>>);

// lvalue references in quantity_cast
namespace lvalue_tests {
constexpr quantity<m, int> lvalue_q = 1 * m;
static_assert(is_of_type<quantity_cast<isq::distance>(lvalue_q), quantity<isq::distance[m], int>>);
}  // namespace lvalue_tests

template<template<auto, typename> typename Q>
concept invalid_quantity_cast = requires {
  requires !requires { quantity_cast<dimensionless>(Q<rad, double>(42. * rad)); };
  requires !requires { quantity_cast<dimensionless>(Q<isq::angular_measure[rad], double>(42. * rad)); };
};
static_assert(invalid_quantity_cast<quantity>);

// QuantityOf
static_assert(QuantityOf<quantity<isq::length[m]>, isq::length>);
static_assert(QuantityOf<quantity<isq::width[m]>, isq::length>);
static_assert(QuantityOf<quantity<isq::position_vector[m], int>, isq::length>);
static_assert(!QuantityOf<quantity<isq::length[m]>, isq::width>);
static_assert(QuantityOf<quantity<m>, isq::width>);
static_assert(QuantityOf<quantity<m>, isq::position_vector>);
static_assert(QuantityOf<quantity<kind_of<isq::length>[m]>, isq::width>);
static_assert(QuantityOf<quantity<kind_of<isq::length>[m]>, isq::position_vector>);
static_assert(!QuantityOf<quantity<isq::width[m]>, isq::altitude>);

static_assert(QuantityOf<quantity<isq::speed[m / s]>, isq::speed>);
static_assert(QuantityOf<quantity<isq::speed[m / s]>, isq::length / isq::time>);
static_assert(QuantityOf<quantity<m / s>, isq::length / isq::time>);
static_assert(QuantityOf<quantity<kind_of<isq::speed>[m / s]>, isq::length / isq::time>);
static_assert(!QuantityOf<quantity<isq::speed[m / s]>, isq::distance / isq::duration>);
static_assert(!QuantityOf<quantity<isq::speed[m / s]>, isq::width / isq::duration>);
static_assert(QuantityOf<quantity<m / s>, isq::width / isq::duration>);
static_assert(QuantityOf<quantity<kind_of<isq::speed>[m / s]>, isq::width / isq::duration>);
static_assert(!QuantityOf<quantity<isq::speed[m / s]>, isq::displacement / isq::duration>);
static_assert(QuantityOf<quantity<m / s>, isq::displacement / isq::duration>);
static_assert(QuantityOf<quantity<kind_of<isq::speed>[m / s]>, isq::displacement / isq::duration>);
static_assert(QuantityOf<quantity<isq::velocity[m / s], int>, isq::displacement / isq::duration>);

static_assert(QuantityOf<decltype(10 * m), isq::height>);                        // kind of
static_assert(QuantityOf<decltype(10 * kind_of<isq::length>[m]), isq::height>);  // kind of
static_assert(!QuantityOf<decltype(10 * isq::length[m]), isq::height>);          // different kinds
static_assert(!QuantityOf<decltype(10 * isq::width[m]), isq::height>);           // different kinds
static_assert(QuantityOf<decltype(10 * isq::speed[m / s]), isq::speed>);
static_assert(QuantityOf<decltype(20 * isq::length[m] / (2 * isq::time[s])), isq::speed>);  // derived unnamed quantity

// overflowing unit conversions
template<auto Q>
concept overflowing_unit_conversion = requires {
  requires !requires { quantity<si::metre, std::int8_t>(Q); };
  requires !requires { quantity<si::milli<si::metre>, std::int16_t>(Q); };
  requires !requires { Q.in(si::metre); };
  requires !requires { Q.force_in(si::metre); };
  requires !requires { Q + std::int8_t(1) * nm; };  // promotion to int
  requires !requires { Q - std::int8_t(1) * nm; };  // promotion to int
  requires !requires { Q % std::int8_t(1) * m; };
  requires !requires { Q == std::int8_t(1) * m; };
  requires !requires { Q < std::int8_t(1) * m; };
  requires !requires { typename std::common_type_t<decltype(Q), quantity<si::metre, std::int8_t>>; };
};
static_assert(overflowing_unit_conversion<std::int8_t(1) * km>);

}  // namespace
