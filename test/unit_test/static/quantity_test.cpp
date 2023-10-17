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
#include <mp-units/chrono.h>
#include <mp-units/systems/isq/mechanics.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si/si.h>
#include <limits>
#include <utility>

template<>
inline constexpr bool mp_units::is_vector<int> = true;

namespace {

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

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
  requires !requires { typename Q<isq::position_vector[si::metre], double>; };   // vector representation expected
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

/////////////////
// member types
/////////////////

static_assert(is_same_v<quantity<isq::length[m]>::rep, double>);
static_assert(is_same_v<quantity<isq::length[m], int>::rep, int>);


////////////////////////////
// static member functions
////////////////////////////

static_assert(quantity<isq::length[m], int>::zero().numerical_value_in(m) == 0);
static_assert(quantity<isq::length[m], int>::one().numerical_value_in(m) == 1);
static_assert(quantity<isq::length[m], int>::min().numerical_value_in(m) == std::numeric_limits<int>::lowest());
static_assert(quantity<isq::length[m], int>::max().numerical_value_in(m) == std::numeric_limits<int>::max());
static_assert(quantity<isq::length[m], double>::zero().numerical_value_in(m) == 0.0);
static_assert(quantity<isq::length[m], double>::one().numerical_value_in(m) == 1.0);
static_assert(quantity<isq::length[m], double>::min().numerical_value_in(m) == std::numeric_limits<double>::lowest());
static_assert(quantity<isq::length[m], double>::max().numerical_value_in(m) == std::numeric_limits<double>::max());


//////////////////////////////
// construction from a value
//////////////////////////////

// construction from a value is private
static_assert(!std::constructible_from<quantity<isq::length[m]>, double>);
static_assert(!std::convertible_to<double, quantity<isq::length[m]>>);

static_assert(!std::constructible_from<quantity<isq::length[m], int>, int>);
static_assert(!std::convertible_to<int, quantity<isq::length[m], int>>);

static_assert(!std::constructible_from<quantity<dimensionless[one]>, double>);
static_assert(!std::convertible_to<double, quantity<dimensionless[one]>>);

static_assert(!std::constructible_from<quantity<dimensionless[one]>, int>);
static_assert(!std::convertible_to<int, quantity<dimensionless[one]>>);


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


///////////////////////
// obtaining a number
///////////////////////

static_assert(quantity<isq::length[m], int>(123 * m).numerical_value_in(m) == 123);
static_assert(quantity<isq::length[m], int>(2 * km).numerical_value_in(m) == 2000);
static_assert(quantity<isq::length[km], int>(2 * km).numerical_value_in(km) == 2);
static_assert(quantity<isq::length[km]>(1500 * m).numerical_value_in(km) == 1.5);


///////////////////////////////////
// converting to a different unit
///////////////////////////////////

static_assert(is_of_type<(2. * km).in(m), quantity<si::metre>>);
static_assert(is_of_type<isq::length(2. * km).in(m), quantity<isq::length[m]>>);
static_assert(is_of_type<isq::height(2. * km).in(m), quantity<isq::height[m]>>);

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

template<Representation Rep, Quantity Q, const basic_fixed_string additional_nttp_argument>
struct derived_quantity : quantity<Q::reference, Rep> {
  static constexpr auto reference = Q::reference;
  static constexpr auto quantity_spec = Q::quantity_spec;
  static constexpr auto dimension = Q::dimension;
  static constexpr auto unit = Q::unit;
  using rep = Rep;
  using R = quantity<reference, Rep>;

  derived_quantity() = default;
  constexpr explicit(!std::is_trivial_v<Rep>) derived_quantity(const R& t) : R(t) {}
  constexpr explicit(!std::is_trivial_v<Rep>) derived_quantity(R&& t) : R(std::move(t)) {}

  constexpr derived_quantity& operator=(const R& t)
  {
    R::operator=(t);
    return *this;
  }
  constexpr derived_quantity& operator=(R&& t)
  {
    R::operator=(std::move(t));
    return *this;
  }

  constexpr operator R&() & noexcept { return *this; }
  constexpr operator const R&() const& noexcept { return *this; }
  constexpr operator R&&() && noexcept { return *this; }
  constexpr operator const R&&() const&& noexcept { return *this; }
};

static_assert(Quantity<derived_quantity<double, quantity<isq::length[m]>, "NTTP type description">>);

constexpr QuantityOf<isq::length> auto get_length_derived_quantity() noexcept
{
  derived_quantity<double, quantity<isq::length[m]>, "NTTP type description"> dist{};
  dist += 1 * m;
  dist = dist + 1 * m;
  dist *= 0.5;
  return dist;
}
static_assert(get_length_derived_quantity() == 1 * m);


/////////
// CTAD
/////////

static_assert(std::is_same_v<decltype(quantity{123 * m})::rep, int>);
static_assert(std::is_same_v<decltype(quantity{123. * m})::rep, double>);
static_assert(quantity{123. * m}.unit == si::metre);
static_assert(quantity{123. * m}.quantity_spec == kind_of<isq::length>);
static_assert(quantity{123. * h}.unit == si::hour);
static_assert(quantity{123. * h}.quantity_spec == kind_of<isq::time>);

using namespace std::chrono_literals;
static_assert(std::is_same_v<decltype(quantity{123s})::rep, std::chrono::seconds::rep>);
static_assert(std::is_same_v<decltype(quantity{123.s})::rep, long double>);
static_assert(quantity{24h}.unit == si::hour);
static_assert(quantity{24h}.quantity_spec == kind_of<isq::time>);


////////////////////////
// assignment operator
////////////////////////

static_assert([] {
  auto l1(1 * m), l2(2 * m);
  return l2 = l1;
}()
                .numerical_value_in(m) == 1);
static_assert([] {
  const auto l1(1 * m);
  auto l2(2 * m);
  return l2 = l1;
}()
                .numerical_value_in(m) == 1);
static_assert([]() {
  auto l1(1 * m), l2(2 * m);
  return l2 = std::move(l1);
}()
                .numerical_value_in(m) == 1);


////////////////////
// unary operators
////////////////////

static_assert((+123 * m).numerical_value_in(m) == 123);
static_assert((-123 * m).numerical_value_in(m) == -123);
static_assert((+(-123 * m)).numerical_value_in(m) == -123);
static_assert((-(-123 * m)).numerical_value_in(m) == 123);

static_assert([](auto v) {
  auto vv = v++;
  return std::pair(v, vv);
}(123 * m) == std::pair(124 * m, 123 * m));
static_assert([](auto v) {
  auto vv = ++v;
  return std::pair(v, vv);
}(123 * m) == std::pair(124 * m, 124 * m));
static_assert([](auto v) {
  auto vv = v--;
  return std::pair(v, vv);
}(123 * m) == std::pair(122 * m, 123 * m));
static_assert([](auto v) {
  auto vv = --v;
  return std::pair(v, vv);
}(123 * m) == std::pair(122 * m, 122 * m));

static_assert(is_same_v<decltype((+(short{0} * m)).numerical_value_in(m)), int>);


////////////////////////
// compound assignment
////////////////////////

// same type
static_assert((1 * m += 1 * m).numerical_value_in(m) == 2);
static_assert((2 * m -= 1 * m).numerical_value_in(m) == 1);
static_assert((1 * m *= 2).numerical_value_in(m) == 2);
static_assert((2 * m /= 2).numerical_value_in(m) == 1);
static_assert((1 * m *= 2 * one).numerical_value_in(m) == 2);
static_assert((2 * m /= 2 * one).numerical_value_in(m) == 1);
static_assert((7 * m %= 2 * m).numerical_value_in(m) == 1);

// different types
static_assert((2.5 * m += 3 * m).numerical_value_in(m) == 5.5);
static_assert((123 * m += 1 * km).numerical_value_in(m) == 1123);
static_assert((5.5 * m -= 3 * m).numerical_value_in(m) == 2.5);
static_assert((1123 * m -= 1 * km).numerical_value_in(m) == 123);
static_assert((2.5 * m *= 3).numerical_value_in(m) == 7.5);
static_assert((7.5 * m /= 3).numerical_value_in(m) == 2.5);
static_assert((2.5 * m *= 3 * one).numerical_value_in(m) == 7.5);
static_assert((7.5 * m /= 3 * one).numerical_value_in(m) == 2.5);
static_assert((3500 * m %= 1 * km).numerical_value_in(m) == 500);

// static_assert((std::uint8_t(255) * m %= 256 * m).numerical_value_in(m) == [] {
//   std::uint8_t ui(255);
//   return ui %= 256;
// }());  // UB
// TODO: Fix
static_assert((std::uint8_t(255) * m %= 257 * m).numerical_value_in(m) != [] {
  std::uint8_t ui(255);
  return ui %= 257;
}());

// TODO ICE
// (https://developercommunity2.visualstudio.com/t/ICE-on-a-constexpr-operator-in-mp-unit/1302907)
#ifndef MP_UNITS_COMP_MSVC
// next two lines trigger conversions warnings
// (warning disabled in CMake for this file)
static_assert((22 * m *= 33.33).numerical_value_in(m) == 733);
static_assert((22 * m /= 3.33).numerical_value_in(m) == 6);
static_assert((22 * m *= 33.33 * one).numerical_value_in(m) == 733);
static_assert((22 * m /= 3.33 * one).numerical_value_in(m) == 6);
#endif

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

  // TODO: accept non-truncating argument
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

static_assert(is_of_type<1 * N * (1 * m), quantity<derived_unit<struct si::newton, struct si::metre>{}, int>>);

static_assert(is_of_type<1 * m * 1, quantity<si::metre, int>>);
static_assert(is_of_type<1 * m * (1 * one), quantity<si::metre, int>>);
static_assert(is_of_type<1 * m * (1 * percent), quantity<derived_unit<struct percent, struct si::metre>{}, int>>);

static_assert(is_of_type<1 * (1 * m), quantity<si::metre, int>>);
static_assert(is_of_type<1 * one * (1 * m), quantity<si::metre, int>>);
static_assert(is_of_type<1 * percent * (1 * m), quantity<derived_unit<struct percent, struct si::metre>{}, int>>);

static_assert(is_of_type<1 * m / (1 * s), quantity<derived_unit<struct si::metre, per<struct si::second>>{}, int>>);
static_assert(is_of_type<1 * m / (1 * m), quantity<one, int>>);
static_assert(is_of_type<1 * km / (1 * m), quantity<derived_unit<si::kilo_<si::metre>, per<struct si::metre>>{}, int>>);

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
static_assert(is_same_v<decltype(std::uint8_t(0) * m + std::uint8_t(0) * m)::rep, int>);
static_assert(is_same_v<decltype(std::uint8_t(0) * m - std::uint8_t(0) * m)::rep, int>);
static_assert((std::uint8_t(128) * m + std::uint8_t(128) * m).numerical_value_in(m) ==
              std::uint8_t(128) + std::uint8_t(128));
static_assert((std::uint8_t(0) * m - std::uint8_t(1) * m).numerical_value_in(m) == std::uint8_t(0) - std::uint8_t(1));

static_assert(
  is_same_v<decltype((std::uint8_t(0) * m) % (std::uint8_t(0) * m))::rep, decltype(std::uint8_t(0) % std::uint8_t(0))>);

// different representation types
static_assert(is_of_type<1. * m + 1 * m, quantity<si::metre, double>>);
static_assert(is_of_type<1 * m + 1. * km, quantity<si::metre, double>>);
static_assert(is_of_type<1 * km + 1. * m, quantity<si::metre, double>>);

static_assert(is_of_type<1 * m - 1. * m, quantity<si::metre, double>>);
static_assert(is_of_type<1. * km - 1 * m, quantity<si::metre, double>>);
static_assert(is_of_type<1. * m - 1 * km, quantity<si::metre, double>>);

static_assert(is_of_type<1. * N * (1 * m), quantity<derived_unit<struct si::newton, struct si::metre>{}, double>>);

static_assert(is_of_type<1 * m * 1., quantity<si::metre, double>>);
static_assert(is_of_type<1 * m * (1. * one), quantity<si::metre, double>>);
static_assert(is_of_type<1 * m * (1. * percent), quantity<derived_unit<struct percent, struct si::metre>{}, double>>);

static_assert(is_of_type<1 * (1. * m), quantity<si::metre, double>>);
static_assert(is_of_type<1. * one * (1 * m), quantity<si::metre, double>>);
static_assert(is_of_type<1 * percent * (1. * m), quantity<derived_unit<struct percent, struct si::metre>{}, double>>);

static_assert(is_of_type<1 * m / (1. * s), quantity<derived_unit<struct si::metre, per<struct si::second>>{}, double>>);
static_assert(is_of_type<1. * m / (1 * m), quantity<one, double>>);
static_assert(
  is_of_type<1. * km / (1 * m), quantity<derived_unit<si::kilo_<si::metre>, per<struct si::metre>>{}, double>>);

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
static_assert(is_of_type<1 * m * (1 * min), quantity<derived_unit<struct si::metre, struct si::minute>{}, int>>);
static_assert(is_of_type<1 * s * (1 * Hz), quantity<derived_unit<struct si::hertz, struct si::second>{}, int>>);
static_assert(is_of_type<1 / (1 * min), quantity<derived_unit<struct one, per<struct si::minute>>{}, int>>);
static_assert(is_of_type<1 / (1 * Hz), quantity<derived_unit<struct one, per<struct si::hertz>>{}, int>>);
static_assert(is_of_type<1 / (1 * km), quantity<derived_unit<struct one, per<si::kilo_<si::metre>>>{}, int>>);
static_assert(is_of_type<1 / min, quantity<derived_unit<struct one, per<struct si::minute>>{}, int>>);
static_assert(is_of_type<1 / Hz, quantity<derived_unit<struct one, per<struct si::hertz>>{}, int>>);
static_assert(is_of_type<1 / km, quantity<derived_unit<struct one, per<si::kilo_<si::metre>>>{}, int>>);
static_assert(is_of_type<1 * km / (1 * m), quantity<derived_unit<si::kilo_<si::metre>, per<struct si::metre>>{}, int>>);
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

// Different named dimensions
template<typename... Ts>
consteval bool invalid_arithmetic(Ts... ts)
{
  return !requires { (... + ts); } && !requires { (... - ts); };
}
static_assert(invalid_arithmetic(5 * isq::activity[Bq], 5 * isq::frequency[Hz]));
static_assert(invalid_arithmetic(5 * isq::activity[Bq], 10 / (2 * isq::time[s]), 5 * isq::frequency[Hz]));

// Physical constants
static_assert(1 * si::si2019::speed_of_light_in_vacuum + 10 * isq::speed[m / s] == 299'792'468 * isq::speed[m / s]);

// Implicit conversions allowed between quantities of `convertible` references
[[maybe_unused]] constexpr quantity<isq::speed[km / h]> speed = 120 * isq::length[km] / (2 * isq::time[h]);

// dimensionless
static_assert((3 * one *= 2 * one) == 6 * one);
static_assert((6 * one /= 2 * one) == 3 * one);
static_assert(1 * one + 1 * one == 2 * one);
static_assert(2 * one - 1 * one == 1 * one);
static_assert(2 * one * (2 * one) == 4 * one);
static_assert(2 * (2 * one) == 4 * one);
static_assert(2 * one * 2 == 4 * one);
static_assert(4 * one / (2 * one) == 2 * one);
static_assert(4 / (2 * one) == 2 * one);
static_assert(4 * one / 2 == 2 * one);
static_assert(4 * one % (2 * one) == 0 * one);

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

static_assert(is_same_v<decltype(std::uint8_t(0) * one + std::uint8_t(0) * one)::rep, int>);
static_assert(is_same_v<decltype(std::uint8_t(0) * one - std::uint8_t(0) * one)::rep, int>);
static_assert((std::uint8_t(128) * one + std::uint8_t(128) * one).numerical_value_in(one) ==
              std::uint8_t(128) + std::uint8_t(128));
static_assert((std::uint8_t(0) * one - std::uint8_t(1) * one).numerical_value_in(one) ==
              std::uint8_t(0) - std::uint8_t(1));
static_assert(is_same_v<decltype(std::uint8_t(0) * one % (std::uint8_t(0) * one))::rep,
                        decltype(std::uint8_t(0) % std::uint8_t(0))>);

static_assert(2 * one * (1 * m) == 2 * m);
static_assert(2 * one / (1 * m) == 2 / (1 * m));


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
template</*Reference*/ auto R1, /*Reference*/ auto R2>  // TODO Use `Reference` when Clang supports it.
concept invalid_comparison = !requires { 2 * R1 == 2 * R2; } && !requires { 2 * R2 == 2 * R1; };
static_assert(invalid_comparison<isq::activity[Bq], isq::frequency[Hz]>);


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


//////////////////
// dimensionless
//////////////////

static_assert(is_of_type<10 * km / (5 * km), quantity<one, int>>);

static_assert((50. * m / (100. * m)).numerical_value_in(percent) == 50);
static_assert(50. * m / (100. * m) == 50 * percent);

static_assert((50. * percent).numerical_value_in(one) == 0.5);


//////////////////
// value_cast
//////////////////

static_assert(value_cast<m>(2 * km).numerical_value_in(m) == 2000);
static_assert(value_cast<km>(2000 * m).numerical_value_in(km) == 2);
static_assert(value_cast<int>(1.23 * m).numerical_value_in(m) == 1);
static_assert(value_cast<km / h>(2000.0 * m / (3600.0 * s)).numerical_value_in(km / h) == 2);

static_assert((2 * km).force_in(m).numerical_value_in(m) == 2000);
static_assert((2000 * m).force_in(km).numerical_value_in(km) == 2);
static_assert((2000.0 * m / (3600.0 * s)).force_in(km / h).numerical_value_in(km / h) == 2);

//////////////////
// quantity_cast
//////////////////

static_assert(is_of_type<quantity_cast<isq::distance>(1 * m), quantity<isq::distance[m], int>>);
static_assert(is_of_type<quantity_cast<isq::distance>(isq::length(1 * m)), quantity<isq::distance[m], int>>);
static_assert(is_of_type<quantity_cast<kind_of<isq::length>>(isq::length(1 * m)), quantity<si::metre, int>>);
static_assert(is_of_type<quantity_cast<kind_of<isq::length>>(isq::distance(1 * m)), quantity<si::metre, int>>);


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
static_assert(!QuantityOf<quantity<isq::speed[m / s]>, isq::position_vector / isq::duration>);
static_assert(QuantityOf<quantity<m / s>, isq::position_vector / isq::duration>);
static_assert(QuantityOf<quantity<kind_of<isq::speed>[m / s]>, isq::position_vector / isq::duration>);
static_assert(QuantityOf<quantity<isq::velocity[m / s], int>, isq::position_vector / isq::duration>);

static_assert(QuantityOf<decltype(10 * isq::speed[m / s] / (2 * isq::length[m]) * (4 * isq::time[s])), dimension_one>);
static_assert(QuantityOf<decltype(10 * m), isq::height>);                        // kind of
static_assert(QuantityOf<decltype(10 * kind_of<isq::length>[m]), isq::height>);  // kind of
static_assert(!QuantityOf<decltype(10 * isq::length[m]), isq::height>);          // different kinds
static_assert(!QuantityOf<decltype(10 * isq::width[m]), isq::height>);           // different kinds
static_assert(QuantityOf<decltype(10 * isq::speed[m / s]), isq::speed>);
static_assert(QuantityOf<decltype(20 * isq::length[m] / (2 * isq::time[s])), isq::speed>);  // derived unnamed quantity

}  // namespace
