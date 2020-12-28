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
#include "units/math.h"
#include "units/physical/si/cgs/derived/speed.h"
#include "units/physical/si/derived/area.h"
#include "units/physical/si/derived/frequency.h"
#include "units/physical/si/derived/speed.h"
#include "units/physical/si/derived/volume.h"
#include "units/physical/si/fps/derived/speed.h"
#include <chrono>
#include <complex>
#include <mutex>
#include <string>
#include <utility>

namespace {

using namespace units;
using namespace units::physical::si;


//////////////////////////////
// quantity class invariants
//////////////////////////////

static_assert(sizeof(length<metre>) == sizeof(double));
static_assert(sizeof(length<metre, short>) == sizeof(short));

#if COMP_GCC != 10 || COMP_GCC_MINOR > 2
template<template<typename, typename, typename> typename Q>
concept invalid_types = requires {
  requires !requires { typename Q<dim_length, second, int>; };  // unit of a different dimension
  requires !requires { typename Q<dim_length, metre, length<metre>>; };  // quantity used as Rep
  requires !requires { typename Q<metre, dim_length, double>; };  // reordered arguments
  requires !requires { typename Q<metre, double, dim_length>; };  // reordered arguments
};
static_assert(invalid_types<quantity>);
#endif

static_assert(std::is_trivially_default_constructible_v<length<metre>>);
static_assert(std::is_trivially_copy_constructible_v<length<metre>>);
static_assert(std::is_trivially_move_constructible_v<length<metre>>);
static_assert(std::is_trivially_copy_assignable_v<length<metre>>);
static_assert(std::is_trivially_move_assignable_v<length<metre>>);
static_assert(std::is_trivially_destructible_v<length<metre>>);

static_assert(std::is_nothrow_default_constructible_v<length<metre>>);
static_assert(std::is_nothrow_copy_constructible_v<length<metre>>);
static_assert(std::is_nothrow_move_constructible_v<length<metre>>);
static_assert(std::is_nothrow_copy_assignable_v<length<metre>>);
static_assert(std::is_nothrow_move_assignable_v<length<metre>>);
static_assert(std::is_nothrow_destructible_v<length<metre>>);

static_assert(std::is_trivially_copyable_v<length<metre>>);
static_assert(std::is_standard_layout_v<length<metre>>);

static_assert(std::default_initializable<length<metre>>);
static_assert(std::move_constructible<length<metre>>);
static_assert(std::copy_constructible<length<metre>>);
static_assert(std::equality_comparable<length<metre>>);
static_assert(std::totally_ordered<length<metre>>);
static_assert(std::regular<length<metre>>);

static_assert(std::three_way_comparable<length<metre>>);


//////////////////
// member types
//////////////////

static_assert(is_same_v<length<metre, int>::dimension, dim_length>);
static_assert(is_same_v<fps::length<fps::mile>::dimension, fps::dim_length>);
static_assert(is_same_v<length<metre, int>::unit, metre>);
static_assert(is_same_v<fps::length<fps::mile>::unit, fps::mile>);
static_assert(is_same_v<length<metre, int>::rep, int>);
static_assert(is_same_v<fps::length<fps::mile>::rep, double>);


////////////////////////////
// static member functions
////////////////////////////

static_assert(length<metre, int>::zero().count() == 0);
static_assert(length<metre, int>::min().count() == std::numeric_limits<int>::lowest());
static_assert(length<metre, int>::max().count() == std::numeric_limits<int>::max());
static_assert(length<metre, double>::zero().count() == 0.0);
static_assert(length<metre, double>::min().count() == std::numeric_limits<double>::lowest());
static_assert(length<metre, double>::max().count() == std::numeric_limits<double>::max());


//////////////////////////////
// construction from a value
//////////////////////////////

// only explicit construction from a value
static_assert(std::constructible_from<length<metre>, double>);
static_assert(!std::convertible_to<double, length<metre>>);

static_assert(std::constructible_from<length<metre>, float>);
static_assert(!std::convertible_to<float, length<metre>>);

static_assert(std::constructible_from<length<metre, float>, double>);  // truncating implicit conversions double -> float allowed
static_assert(!std::convertible_to<double, length<metre, float>>);

static_assert(std::constructible_from<length<metre>, int>);
static_assert(!std::convertible_to<int, length<metre>>);

static_assert(std::constructible_from<length<metre>, short>);
static_assert(!std::convertible_to<short, length<metre>>);

static_assert(std::constructible_from<length<metre, short>, int>);  // truncating implicit conversions int -> short allowed
static_assert(!std::convertible_to<int, length<metre, short>>);

// exception, implicit construction from a value allowed for a dimensionless quantity
static_assert(std::constructible_from<dimensionless<one>, double>);
static_assert(std::convertible_to<double, dimensionless<one>>);

static_assert(std::constructible_from<dimensionless<one>, float>);
static_assert(std::convertible_to<float, dimensionless<one>>);

static_assert(std::constructible_from<dimensionless<one, float>, double>);
static_assert(std::convertible_to<double, dimensionless<one, float>>);

static_assert(std::constructible_from<dimensionless<one>, int>);
static_assert(std::convertible_to<int, dimensionless<one>>);

static_assert(std::constructible_from<dimensionless<one>, short>);
static_assert(std::convertible_to<short, dimensionless<one>>);

static_assert(std::constructible_from<dimensionless<one, short>, int>);
static_assert(std::convertible_to<int, dimensionless<one, short>>);

// but only if a dimensionless quantity has a ratio(1)
static_assert(std::constructible_from<dimensionless<percent>, double>);
static_assert(!std::convertible_to<double, dimensionless<percent>>);

static_assert(std::constructible_from<dimensionless<percent>, float>);
static_assert(!std::convertible_to<float, dimensionless<percent>>);

static_assert(std::constructible_from<dimensionless<percent, float>, double>);  // truncating implicit conversions double -> float allowed
static_assert(!std::convertible_to<double, dimensionless<percent, float>>);

static_assert(std::constructible_from<dimensionless<percent>, int>);
static_assert(!std::convertible_to<int, dimensionless<percent>>);

static_assert(std::constructible_from<dimensionless<percent>, short>);
static_assert(!std::convertible_to<short, dimensionless<percent>>);

static_assert(std::constructible_from<dimensionless<percent, short>, int>);  // truncating implicit conversions int -> short allowed
static_assert(!std::convertible_to<int, dimensionless<percent, short>>);

// floating-point to integral truncating conversion not allowed
static_assert(!std::constructible_from<length<metre, int>, double>);
static_assert(!std::convertible_to<double, length<metre, int>>);

static_assert(!std::constructible_from<dimensionless<one, int>, double>);
static_assert(!std::convertible_to<double, dimensionless<one, int>>);

static_assert(length<metre, int>().count() == 0); // value initialization
static_assert(length<metre, int>(1).count() == 1);
static_assert(length<metre, double>(1.0).count() == 1.0);
static_assert(length<metre, double>(1).count() == 1.0);
static_assert(length<metre, double>(3.14).count() == 3.14);


///////////////////////////////////////
// construction from another quantity
///////////////////////////////////////

// conversion only between equivalent dimensions
static_assert(std::constructible_from<length<metre>, length<metre>>);
static_assert(std::convertible_to<length<metre>, length<metre>>);
static_assert(std::constructible_from<length<centimetre>, cgs::length<cgs::centimetre>>);
static_assert(std::convertible_to<cgs::length<cgs::centimetre>, length<centimetre>>);
static_assert(std::constructible_from<fps::length<fps::foot>, cgs::length<cgs::centimetre>>);
static_assert(std::convertible_to<cgs::length<cgs::centimetre>, fps::length<fps::foot>>);

// conversion between different dimensions not allowed
static_assert(!std::constructible_from<length<metre>, physical::si::time<second>>);
static_assert(!std::convertible_to<physical::si::time<second>, length<metre>>);
static_assert(!std::constructible_from<length<metre>, speed<metre_per_second>>);
static_assert(!std::convertible_to<speed<metre_per_second>, length<metre>>);

// implicit conversion from another quantity only if non-truncating
static_assert(std::constructible_from<length<metre>, length<metre, int>>);  // int -> double OK
static_assert(std::convertible_to<length<metre, int>, length<metre>>);  // int -> double OK

static_assert(!std::constructible_from<length<metre, int>, length<metre>>);  // truncating double -> int not allowed
static_assert(!std::convertible_to<length<metre>, length<metre, int>>);  // truncating double -> int not allowed

static_assert(std::constructible_from<length<metre, int>, length<kilometre, int>>);  // kilometre<int> -> metre<int> OK
static_assert(std::convertible_to<length<kilometre, int>, length<metre, int>>);  // kilometre<int> -> metre<int> OK

static_assert(!std::constructible_from<length<kilometre, int>, length<metre, int>>);  // truncating metre<int> -> kilometre<int> not allowed
static_assert(!std::convertible_to<length<metre, int>, length<kilometre, int>>);  // truncating metre<int> -> kilometre<int> not allowed

// converting to double always OK
static_assert(std::constructible_from<length<metre>, length<kilometre, int>>);  
static_assert(std::convertible_to<length<kilometre, int>, length<metre>>);
static_assert(std::constructible_from<length<kilometre>, length<metre, int>>);
static_assert(std::convertible_to<length<metre, int>, length<kilometre>>);

static_assert(length<metre, int>(123_q_m).count() == 123);
static_assert(length<kilometre, int>(2_q_km).count() == 2);
static_assert(length<metre, int>(2_q_km).count() == 2000);
static_assert(length<kilometre>(1500_q_m).count() == 1.5);


/////////
// CTAD
/////////

static_assert(is_same_v<decltype(quantity{length<metre, int>(123)}), length<metre, int>>);
static_assert(is_same_v<decltype(quantity{speed<metre_per_second>(123)}), speed<metre_per_second>>);
// static_assert(is_same_v<decltype(length{length<metre, int>(123)}), length<metre, int>>);   // TODO gcc ICE
static_assert(is_same_v<decltype(quantity{123_q_m}), length<metre, std::int64_t>>);
static_assert(is_same_v<decltype(quantity{1}), dimensionless<one, int>>);
static_assert(is_same_v<decltype(quantity{1.23}), dimensionless<one, double>>);


////////////////////////
// assignment operator
////////////////////////

static_assert([]() { length<metre, int> l1(1), l2(2); return l2 = l1; }().count() == 1);
static_assert([]() { length<metre, int> l1(1), l2(2); return l2 = std::move(l1); }().count() == 1);


////////////////////
// unary operators
////////////////////

static_assert((+123_q_m).count() == 123);
static_assert((-123_q_m).count() == -123);
static_assert((+(-123_q_m)).count() == -123);
static_assert((-(-123_q_m)).count() == 123);

static_assert([](auto v) { auto vv = v++; return std::pair(v, vv); }(123_q_m) == std::pair(124_q_m, 123_q_m));
static_assert([](auto v) { auto vv = ++v; return std::pair(v, vv); }(123_q_m) == std::pair(124_q_m, 124_q_m));
static_assert([](auto v) { auto vv = v--; return std::pair(v, vv); }(123_q_m) == std::pair(122_q_m, 123_q_m));
static_assert([](auto v) { auto vv = --v; return std::pair(v, vv); }(123_q_m) == std::pair(122_q_m, 122_q_m));


////////////////////////
// compound assignment
////////////////////////

// same type
static_assert((1_q_m += 1_q_m).count() == 2);
static_assert((2_q_m -= 1_q_m).count() == 1);
static_assert((1_q_m *= 2).count() == 2);
static_assert((2_q_m /= 2).count() == 1);
static_assert((7_q_m %= 2).count() == 1);
static_assert((7_q_m %= 2_q_m).count() == 1);

// different types
static_assert((2.5_q_m += 3_q_m).count() == 5.5);
static_assert((123_q_m += 1_q_km).count() == 1123);
static_assert((5.5_q_m -= 3_q_m).count() == 2.5);
static_assert((1123_q_m -= 1_q_km).count() == 123);
static_assert((2.5_q_m *= 3).count() == 7.5);
static_assert((7.5_q_m /= 3).count() == 2.5);
static_assert((3500_q_m %= 1_q_km).count() == 500);

template<typename Metre>
concept invalid_compound_assignments = requires() {
  // truncating not allowed
  requires !requires(length<Metre, int> l) { l += 2.5_q_m; };
  requires !requires(length<Metre, int> l) { l -= 2.5_q_m; };
  requires !requires(length<kilometre, int> l) { l += length<Metre, int>(2); };
  requires !requires(length<kilometre, int> l) { l -= length<Metre, int>(2); };
  requires !requires(length<kilometre, int> l) { l %= length<Metre, int>(2); };

  // only quantities can be added or subtracted
  requires !requires(length<Metre, int> l) { l += 2; };
  requires !requires(length<Metre, int> l) { l -= 2; };

  // compound multiply/divide by another quantity not allowed
  requires !requires(length<Metre, int> l) { l *= 2_q_m; };
  requires !requires(length<Metre, int> l) { l /= 2_q_m; };

  // modulo operations on a floating point representation not allowed
  requires !requires(length<Metre, double> l) { l %= 2.; };
  requires !requires(length<Metre, double> l) { l %= 2; };
  requires !requires(length<Metre, double> l) { l %= 2._q_m; };
  requires !requires(length<Metre, double> l) { l %= 2_q_m; };
  requires !requires(length<Metre, int> l) { l %= 2._q_m; };
};
static_assert(invalid_compound_assignments<metre>);


////////////////////
// binary operators
////////////////////

template<typename Metre>
concept invalid_binary_operations = requires {
  // no crossdimensional addition and subtraction
  requires !requires { 1_q_s + length<Metre, int>(1); };
  requires !requires { 1_q_s - length<Metre, int>(1); };

  // no floating-point modulo
  requires !requires(length<Metre, double> a) { a % 2_q_m; };
  requires !requires(length<Metre, double> a) { 2_q_m % a; };
  requires !requires(length<Metre, double> a) { a % 2; };
  requires !requires(length<Metre, double> a, length<Metre, double> b) { a % b; };
  requires !requires(length<Metre, double> a, length<Metre, int> b) { a % b; };
  requires !requires(length<Metre, double> a, length<Metre, int> b) { b % a; };
};
static_assert(invalid_binary_operations<metre>);

// same representation type
static_assert(is_same_v<decltype(1_q_m + 1_q_m), length<metre, std::int64_t>>);
static_assert(is_same_v<decltype(1_q_m - 1_q_m), length<metre, std::int64_t>>);
static_assert(is_same_v<decltype(1_q_m * 1), length<metre, std::int64_t>>);
static_assert(is_same_v<decltype(1_q_m * quantity{1}), length<metre, std::int64_t>>);
static_assert(is_same_v<decltype(1 * 1_q_m), length<metre, std::int64_t>>);
static_assert(is_same_v<decltype(quantity{1} * 1_q_m), length<metre, std::int64_t>>);
static_assert(is_same_v<decltype(1_q_m / 1), length<metre, std::int64_t>>);
static_assert(is_same_v<decltype(1_q_m / quantity{1}), length<metre, std::int64_t>>);
static_assert(is_same_v<decltype(1_q_m % 1), length<metre, std::int64_t>>);
static_assert(is_same_v<decltype(1_q_m % quantity{1}), length<metre, std::int64_t>>);
static_assert(is_same_v<decltype(1_q_m % 1_q_m), length<metre, std::int64_t>>);
static_assert(compare<decltype(1_q_m * dimensionless<percent, std::int64_t>(1)), length<centimetre, std::int64_t>>);
static_assert(compare<decltype(dimensionless<percent, std::int64_t>(1) * 1_q_m), length<centimetre, std::int64_t>>);
static_assert(compare<decltype(1_q_m / dimensionless<percent, std::int64_t>(1)), length<hectometre, std::int64_t>>);
static_assert(compare<decltype(1_q_m % dimensionless<percent, std::int64_t>(1)), length<centimetre, std::int64_t>>);
static_assert(compare<decltype(1_q_m * 1_q_m), area<square_metre, std::int64_t>>);
static_assert(compare<decltype(1_q_m / 1_q_m), dimensionless<one, std::int64_t>>);
static_assert(compare<decltype(1 / 1_q_s), frequency<hertz, std::int64_t>>);
static_assert(compare<decltype(quantity{1} / 1_q_s), frequency<hertz, std::int64_t>>);
static_assert(compare<decltype(dimensionless<percent, std::int64_t>(1) / 1_q_s), frequency<scaled_unit<ratio(1, 100), hertz>, std::int64_t>>);

// different representation types
static_assert(is_same_v<decltype(1_q_m + 1._q_m), length<metre, long double>>);
static_assert(is_same_v<decltype(1_q_m - 1._q_m), length<metre, long double>>);
static_assert(is_same_v<decltype(1_q_m * 1.L), length<metre, long double>>);  // TODO should we address fundamental types implicit truncating conversions with concepts?
static_assert(is_same_v<decltype(1 * 1._q_m), length<metre, long double>>);
static_assert(is_same_v<decltype(1_q_m * quantity{1.L}), length<metre, long double>>);  // TODO should we address fundamental types implicit truncating conversions with concepts?
static_assert(is_same_v<decltype(quantity{1} * 1._q_m), length<metre, long double>>);
static_assert(is_same_v<decltype(1_q_m / 1.L), length<metre, long double>>);
static_assert(is_same_v<decltype(1_q_m / quantity{1.L}), length<metre, long double>>);
static_assert(compare<decltype(1_q_m * dimensionless<percent, long double>(1)), length<centimetre, long double>>);
static_assert(compare<decltype(dimensionless<percent, std::int64_t>(1) * 1._q_m), length<centimetre, long double>>);
static_assert(compare<decltype(1_q_m * 1._q_m), area<square_metre, long double>>);
static_assert(compare<decltype(1_q_m / dimensionless<percent, long double>(1)), length<hectometre, long double>>);
static_assert(compare<decltype(1_q_m / 1._q_m), dimensionless<one, long double>>);
static_assert(compare<decltype(1 / 1._q_s), frequency<hertz, long double>>);
static_assert(compare<decltype(quantity{1} / 1._q_s), frequency<hertz, long double>>);
static_assert(compare<decltype(dimensionless<percent, std::int64_t>(1) / 1._q_s), frequency<scaled_unit<ratio(1, 100), hertz>, long double>>);
static_assert(compare<decltype(1_q_m % short(1)), length<metre, std::int64_t>>);
static_assert(compare<decltype(1_q_m % quantity{short(1)}), length<metre, std::int64_t>>);
static_assert(compare<decltype(1_q_m % dimensionless<percent, short>(1)), length<centimetre, std::int64_t>>);
static_assert(compare<decltype(1_q_m % length<metre, short>(1)), length<metre, std::int64_t>>);

static_assert(is_same_v<decltype(1._q_m + 1_q_m), length<metre, long double>>);
static_assert(is_same_v<decltype(1._q_m - 1_q_m), length<metre, long double>>);
static_assert(is_same_v<decltype(1._q_m * 1), length<metre, long double>>);
static_assert(is_same_v<decltype(1.L * 1_q_m), length<metre, long double>>);
static_assert(is_same_v<decltype(1._q_m * quantity{1}), length<metre, long double>>);
static_assert(is_same_v<decltype(quantity{1.L} * 1_q_m), length<metre, long double>>);
static_assert(is_same_v<decltype(1._q_m / 1), length<metre, long double>>);
static_assert(is_same_v<decltype(1._q_m / quantity{1}), length<metre, long double>>);
static_assert(compare<decltype(1._q_m * dimensionless<percent, std::int64_t>(1)), length<centimetre, long double>>);
static_assert(compare<decltype(dimensionless<percent, long double>(1) * 1_q_m), length<centimetre, long double>>);
static_assert(compare<decltype(1._q_m / dimensionless<percent, std::int64_t>(1)), length<hectometre, long double>>);
static_assert(compare<decltype(1._q_m * 1_q_m), area<square_metre, long double>>);
static_assert(compare<decltype(1._q_m / 1_q_m), dimensionless<one, long double>>);
static_assert(compare<decltype(1.L / 1_q_s), frequency<hertz, long double>>);
static_assert(compare<decltype(quantity{1.L} / 1_q_s), frequency<hertz, long double>>);
static_assert(compare<decltype(dimensionless<percent, long double>(1) / 1_q_s), frequency<scaled_unit<ratio(1, 100), hertz>, long double>>);

// different units
static_assert(is_same_v<decltype(1_q_m + 1_q_km), length<metre, std::int64_t>>);
static_assert(is_same_v<decltype(1._q_m + 1_q_km), length<metre, long double>>);
static_assert(is_same_v<decltype(1_q_m + 1._q_km), length<metre, long double>>);
static_assert(is_same_v<decltype(1._q_m + 1._q_km), length<metre, long double>>);

static_assert(is_same_v<decltype(1_q_km + 1_q_m), length<metre, std::int64_t>>);
static_assert(is_same_v<decltype(1._q_km + 1_q_m), length<metre, long double>>);
static_assert(is_same_v<decltype(1_q_km + 1._q_m), length<metre, long double>>);
static_assert(is_same_v<decltype(1._q_km + 1._q_m), length<metre, long double>>);

static_assert(is_same_v<decltype(1_q_m - 1_q_km), length<metre, std::int64_t>>);
static_assert(is_same_v<decltype(1._q_m - 1_q_km), length<metre, long double>>);
static_assert(is_same_v<decltype(1_q_m - 1._q_km), length<metre, long double>>);
static_assert(is_same_v<decltype(1._q_m - 1._q_km), length<metre, long double>>);

static_assert(is_same_v<decltype(1_q_km - 1_q_m), length<metre, std::int64_t>>);
static_assert(is_same_v<decltype(1._q_km - 1_q_m), length<metre, long double>>);
static_assert(is_same_v<decltype(1_q_km - 1._q_m), length<metre, long double>>);
static_assert(is_same_v<decltype(1._q_km - 1._q_m), length<metre, long double>>);

static_assert(is_same_v<decltype(1_q_m % 1_q_km), length<metre, std::int64_t>>);
static_assert(is_same_v<decltype(1_q_km % 1_q_m), length<metre, std::int64_t>>);

// different dimensions
static_assert(compare<decltype(1_q_m_per_s * 1_q_s), length<metre, std::int64_t>>);
static_assert(compare<decltype(1_q_m_per_s * 1_q_h), length<scaled_unit<ratio(36, 1, 2), metre>, std::int64_t>>);
static_assert(compare<decltype(1_q_m * 1_q_min), quantity<unknown_dimension<exponent<dim_length, 1>, exponent<dim_time, 1>>, scaled_unit<ratio(60), unknown_coherent_unit>, std::int64_t>>);
static_assert(compare<decltype(1_q_s * 1_q_Hz), dimensionless<one, std::int64_t>>);
static_assert(compare<decltype(1 / 1_q_min), frequency<scaled_unit<ratio(1, 60), hertz>, std::int64_t>>);
static_assert(compare<decltype(1 / 1_q_Hz), physical::si::time<second, std::int64_t>>);
static_assert(compare<decltype(1 / 1_q_km), quantity<unknown_dimension<exponent<dim_length, -1>>, scaled_unit<ratio(1, 1, -3), unknown_coherent_unit>, std::int64_t>>);
static_assert(compare<decltype(1_q_km / 1_q_m), dimensionless<scaled_unit<ratio(1000), one>, std::int64_t>>);
static_assert(compare<decltype(1_q_m / 1_q_s), speed<metre_per_second, std::int64_t>>);
static_assert(compare<decltype(1_q_m / 1_q_min), speed<scaled_unit<ratio(1, 60), metre_per_second>, std::int64_t>>);
static_assert(compare<decltype(1_q_min / 1_q_m), quantity<unknown_dimension<exponent<dim_length, -1>, exponent<dim_time, 1>>, scaled_unit<ratio(60), unknown_coherent_unit>, std::int64_t>>);

static_assert((1_q_m + 1_q_m).count() == 2);
static_assert((1_q_m + 1_q_km).count() == 1001);
static_assert((1_q_km + 1_q_m).count() == 1001);
static_assert((2_q_m - 1_q_m).count() == 1);
static_assert((1_q_km - 1_q_m).count() == 999);
static_assert((2_q_m * 2).count() == 4);
static_assert((2_q_m * quantity{2}).count() == 4);
static_assert((2_q_m * dimensionless<percent, int>(2)).count() == 4);
static_assert((3 * 3_q_m).count() == 9);
static_assert((quantity{3} * 3_q_m).count() == 9);
static_assert((dimensionless<percent, int>(3) * 3_q_m).count() == 9);
static_assert((4_q_m / 2).count() == 2);
static_assert((4_q_m / quantity{2}).count() == 2);
static_assert((4_q_m / dimensionless<percent, int>(2)).count() == 2);
static_assert((4_q_km / 2_q_m).count() == 2);
static_assert((4000_q_m / 2_q_m).count() == 2000);

static_assert((1.5_q_m + 1_q_m).count() == 2.5);
static_assert((1.5_q_m + 1_q_km).count() == 1001.5);
static_assert((1.5_q_km + 1_q_m).count() == 1501);
static_assert((2.5_q_m - 1_q_m).count() == 1.5);
static_assert((1.5_q_km - 1_q_m).count() == 1499);
static_assert((2.5_q_m * 2).count() == 5);
static_assert((2.5_q_m * quantity{2}).count() == 5);
static_assert((2.5_q_m * dimensionless<percent, int>(2)).count() == 5);
static_assert((2.5L * 2_q_m).count() == 5);
static_assert((quantity{2.5L} * 2_q_m).count() == 5);
static_assert((dimensionless<percent, long double>(2.5L) * 2_q_m).count() == 5);
static_assert((5._q_m / 2).count() == 2.5);
static_assert((5._q_m / quantity{2}).count() == 2.5);
static_assert((5._q_m / dimensionless<percent, int>(2)).count() == 2.5);
static_assert((5._q_km / 2_q_m).count() == 2.5);
static_assert((5000._q_m / 2_q_m).count() == 2500);

static_assert((1_q_m + 1.5_q_m).count() == 2.5);
static_assert((1_q_m + 1.5_q_km).count() == 1501);
static_assert((1_q_km + 1.5_q_m).count() == 1001.5);
static_assert((2_q_m - 1.5_q_m).count() == 0.5);
static_assert((1_q_km - 1.5_q_m).count() == 998.5);
static_assert((2_q_m * 2.5L).count() == 5);
static_assert((2_q_m * quantity{2.5L}).count() == 5);
static_assert((2_q_m * dimensionless<percent, long double>(2.5L)).count() == 5);
static_assert((2 * 2.5_q_m).count() == 5);
static_assert((quantity{2} * 2.5_q_m).count() == 5);
static_assert((dimensionless<percent, int>(2) * 2.5_q_m).count() == 5);
static_assert((5_q_m / 2.5L).count() == 2);
static_assert((5_q_m / quantity{2.5L}).count() == 2);
static_assert((5_q_m / dimensionless<percent, long double>(2.5L)).count() == 2);
static_assert((5_q_km / 2.5_q_m).count() == 2);
static_assert((5000_q_m / 2.5_q_m).count() == 2000);

static_assert((7_q_m % 2).count() == 1);
static_assert((7_q_m % quantity{2}).count() == 1);
static_assert((7_q_m % dimensionless<percent, int>(2)).count() == 1);
static_assert((7_q_m % 2_q_m).count() == 1);
static_assert((7_q_km % 2000_q_m).count() == 1000);

static_assert((10_q_km2 * 10_q_km2) / 50_q_km2 == 2_q_km2);

static_assert((10_q_km / 5_q_m).count() == 2);
static_assert(dimensionless<one>(10_q_km / 5_q_m).count() == 2000);

#if UNITS_DOWNCAST_MODE == 0
static_assert(quantity_cast<dim_one, one>(10_q_km / 5_q_m).count() == 2000);
#else
static_assert(quantity_cast<one>(10_q_km / 5_q_m).count() == 2000);
#endif

static_assert((10_q_s * 2_q_kHz).count() == 20);


// dimensionless

static_assert(quantity{1} + quantity{1} == 2);
static_assert(1 + quantity{1} == 2);
static_assert(quantity{1} + 1 == 2);
static_assert(quantity{2} - quantity{1} == 1);
static_assert(2 - quantity{1} == 1);
static_assert(quantity{2} - 1 == 1);
static_assert(quantity{2} * quantity{2} == 4);
static_assert(2 * quantity{2} == 4);
static_assert(quantity{2} * 2 == 4);
static_assert(quantity{4} / quantity{2} == 2);
static_assert(4 / quantity{2} == 2);
static_assert(quantity{4} / 2 == 2);
static_assert(quantity{4} % quantity{2} == 0);
static_assert(4 % quantity{2} == 0);
static_assert(quantity{4} % 2 == 0);


///////////////////////
// equality operators
///////////////////////

template<typename Metre>
concept no_crossdimensional_equality = requires {
  requires !requires { 1_q_s == length<Metre, int>(1); };
  requires !requires { 1_q_s != length<Metre, int>(1); };
};
static_assert(no_crossdimensional_equality<metre>);

// same type
static_assert(length<metre, int>(123) == length<metre, int>(123));
static_assert(length<metre, int>(321) != length<metre, int>(123));
static_assert(!(length<metre, int>(123) == length<metre, int>(321)));
static_assert(!(length<metre, int>(123) != length<metre, int>(123)));

// different types
static_assert(length<metre, double>(123) == length<metre, int>(123));
static_assert(length<metre, double>(321) != length<metre, int>(123));
static_assert(!(length<metre, double>(123) == length<metre, int>(321)));
static_assert(!(length<metre, double>(123) != length<metre, int>(123)));

static_assert(length<kilometre, int>(123) == length<metre, int>(123000));
static_assert(length<kilometre, int>(321) != length<metre, int>(123000));
static_assert(!(length<kilometre, int>(123) == length<metre, int>(321000)));
static_assert(!(length<kilometre, int>(123) != length<metre, int>(123000)));

// dimensionless

static_assert(quantity{123} == 123);
static_assert(quantity{321} != 123);
static_assert(123 == quantity{123});
static_assert(123 != quantity{321});


///////////////////////
// ordering operators
///////////////////////

template<typename Metre>
concept no_crossdimensional_ordering = requires {
  requires !requires { 1_q_s < length<Metre, int>(1); };
  requires !requires { 1_q_s > length<Metre, int>(1); };
  requires !requires { 1_q_s <= length<Metre, int>(1); };
  requires !requires { 1_q_s >= length<Metre, int>(1); };
};
static_assert(no_crossdimensional_ordering<metre>);

// same type
static_assert(length<metre, int>(123) < length<metre, int>(321));
static_assert(length<metre, int>(123) <= length<metre, int>(123));
static_assert(length<metre, int>(123) <= length<metre, int>(321));
static_assert(length<metre, int>(321) > length<metre, int>(123));
static_assert(length<metre, int>(123) >= length<metre, int>(123));
static_assert(length<metre, int>(321) >= length<metre, int>(123));
static_assert(!(length<metre, int>(321) < length<metre, int>(123)));
static_assert(!(length<metre, int>(123) < length<metre, int>(123)));
static_assert(!(length<metre, int>(321) <= length<metre, int>(123)));
static_assert(!(length<metre, int>(123) > length<metre, int>(321)));
static_assert(!(length<metre, int>(123) > length<metre, int>(123)));
static_assert(!(length<metre, int>(123) >= length<metre, int>(321)));

// different types
static_assert(length<metre, double>(123) < length<metre, int>(321));
static_assert(length<metre, double>(123) <= length<metre, int>(123));
static_assert(length<metre, double>(123) <= length<metre, int>(321));
static_assert(length<metre, double>(321) > length<metre, int>(123));
static_assert(length<metre, double>(123) >= length<metre, int>(123));
static_assert(length<metre, double>(321) >= length<metre, int>(123));
static_assert(!(length<metre, double>(321) < length<metre, int>(123)));
static_assert(!(length<metre, double>(123) < length<metre, int>(123)));
static_assert(!(length<metre, double>(321) <= length<metre, int>(123)));
static_assert(!(length<metre, double>(123) > length<metre, int>(321)));
static_assert(!(length<metre, double>(123) > length<metre, int>(123)));
static_assert(!(length<metre, double>(123) >= length<metre, int>(321)));

static_assert(length<kilometre, int>(123) < length<metre, int>(321000));
static_assert(length<kilometre, int>(123) <= length<metre, int>(123000));
static_assert(length<kilometre, int>(123) <= length<metre, int>(321000));
static_assert(length<kilometre, int>(321) > length<metre, int>(123000));
static_assert(length<kilometre, int>(123) >= length<metre, int>(123000));
static_assert(length<kilometre, int>(321) >= length<metre, int>(123000));
static_assert(!(length<kilometre, int>(321) < length<metre, int>(123000)));
static_assert(!(length<kilometre, int>(123) < length<metre, int>(123000)));
static_assert(!(length<kilometre, int>(321) <= length<metre, int>(123000)));
static_assert(!(length<kilometre, int>(123) > length<metre, int>(321000)));
static_assert(!(length<kilometre, int>(123) > length<metre, int>(123000)));
static_assert(!(length<kilometre, int>(123) >= length<metre, int>(321000)));

// dimensionless

static_assert(quantity{123} < 321);
static_assert(quantity{123} <= 123);
static_assert(quantity{123} <= 321);
static_assert(quantity{321} > 123);
static_assert(quantity{123} >= 123);
static_assert(quantity{321} >= 123);

static_assert(123 < quantity{321});
static_assert(123 <= quantity{123});
static_assert(123 <= quantity{321});
static_assert(321 > quantity{123});
static_assert(123 >= quantity{123});
static_assert(321 >= quantity{123});


//////////////////
// dimensionless
//////////////////

static_assert(std::equality_comparable_with<dimensionless<one>, int>);
static_assert(std::equality_comparable_with<dimensionless<one>, double>);
static_assert(std::equality_comparable_with<dimensionless<one, int>, int>);
static_assert(!std::equality_comparable_with<dimensionless<one, int>, double>);

template<typename Int>
concept invalid_dimensionless_operations = requires {
  requires !requires(dimensionless<one, Int> d) { d + 1.23; };
  requires !requires(dimensionless<one, Int> d) { 1.23 + d; };
  requires !requires(dimensionless<percent, Int> d) { 1 + d; };
  requires !requires(dimensionless<percent, Int> d) { d + 1; };
};
static_assert(invalid_dimensionless_operations<int>);

static_assert(compare<decltype(10_q_km / 5_q_km), quantity<dim_one, one, std::int64_t>>);

#if UNITS_DOWNCAST_MODE == 0
static_assert(quantity_cast<dim_one, percent>(50._q_m / 100._q_m).count() == 50);
#else
static_assert(quantity_cast<percent>(50._q_m / 100._q_m).count() == 50);
#endif
static_assert(50._q_m / 100._q_m == dimensionless<percent>(50));

static_assert(dimensionless<one>(dimensionless<percent>(50)).count() == 0.5);


////////////////
// alias units
////////////////

static_assert(compare<decltype(2_q_l + 2_q_ml), volume<cubic_centimetre, std::int64_t>>);
static_assert(2_q_l + 2_q_ml == 2002_q_cm3);
static_assert(2_q_l + 2_q_ml == 2002_q_ml);
static_assert(2_q_l + 2_q_cm3 == 2002_q_ml);
static_assert(2_q_dm3 + 2_q_cm3 == 2002_q_ml);


//////////////////
// quantity_cast
//////////////////

static_assert(compare<decltype(quantity_cast<scaled_unit<ratio(1), metre>>(2_q_km))::unit, metre>);

static_assert(quantity_cast<length<metre, int>>(2_q_km).count() == 2000);
static_assert(quantity_cast<length<kilometre, int>>(2000_q_m).count() == 2);
static_assert(quantity_cast<length<metre, int>>(1.23_q_m).count() == 1);
static_assert(quantity_cast<metre>(2_q_km).count() == 2000);
static_assert(quantity_cast<kilometre>(2000_q_m).count() == 2);
static_assert(quantity_cast<int>(1.23_q_m).count() == 1);
static_assert(quantity_cast<dim_speed, kilometre_per_hour>(2000.0_q_m / 3600.0_q_s).count() == 2);


////////////////
// downcasting
////////////////

#if UNITS_DOWNCAST_MODE == 0

static_assert(is_same_v<decltype(10_q_m / 5_q_s), quantity<unknown_dimension<units::exponent<dim_length, 1>, units::exponent<dim_time, -1>>, scaled_unit<ratio(1), unknown_coherent_unit>, std::int64_t>>);
static_assert(is_same_v<decltype(1_q_mm + 1_q_km), length<scaled_unit<ratio(1, 1, -3), metre>, std::int64_t>>);

#else

static_assert(is_same_v<decltype(10_q_m / 5_q_s), speed<metre_per_second, std::int64_t>>);
static_assert(is_same_v<decltype(1_q_mm + 1_q_km), length<millimetre, std::int64_t>>);

#endif

}  // namespace
