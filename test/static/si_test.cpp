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

#include <mp-units/systems/si.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <limits>
#include <type_traits>
#endif

namespace {

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

// prefixes
static_assert(1 * m == 10 * dm);
static_assert(1 * m == 100 * cm);
static_assert(1 * m == 1000 * mm);
static_assert(1 * m == 1'000'000 * um);
static_assert(1 * m == 1'000'000'000 * nm);
static_assert(1 * m == 1'000'000'000'000 * pm);
static_assert(1 * m == 1'000'000'000'000'000 * fm);
static_assert(1 * m == 1'000'000'000'000'000'000 * am);
static_assert(1 * mm == 1'000'000'000'000'000'000 * zm);
static_assert(1 * um == 1'000'000'000'000'000'000 * ym);
static_assert(1 * nm == 1'000'000'000'000'000'000 * rm);
static_assert(1 * pm == 1'000'000'000'000'000'000 * qm);

static_assert(1 * dam == 10 * m);
static_assert(1 * hm == 100 * m);
static_assert(1 * km == 1000 * m);
static_assert(1 * Mm == 1'000'000 * m);
static_assert(1 * Gm == 1'000'000'000 * m);
static_assert(1 * Tm == 1'000'000'000'000 * m);
static_assert(1 * Pm == 1'000'000'000'000'000 * m);
static_assert(1 * Em == 1'000'000'000'000'000'000 * m);
static_assert(1 * Zm == 1'000'000'000'000'000'000 * km);
static_assert(1 * Ym == 1'000'000'000'000'000'000 * Mm);
static_assert(1 * Rm == 1'000'000'000'000'000'000 * Gm);
static_assert(1 * Qm == 1'000'000'000'000'000'000 * Tm);

// check for invalid prefixes
template<template<typename U> typename prefix, auto V1>
concept can_not_be_prefixed =
  Unit<MP_UNITS_REMOVE_CONST(decltype(V1))> && !requires { typename prefix<MP_UNITS_REMOVE_CONST(decltype(V1))>; };

static_assert(can_not_be_prefixed<si::milli_, si::kilogram>);
static_assert(can_not_be_prefixed<si::milli_, si::hectare>);
static_assert(can_not_be_prefixed<si::milli_, si::kilo<si::metre>>);
static_assert(can_not_be_prefixed<si::milli_, si::metre / si::second>);
static_assert(can_not_be_prefixed<si::milli_, mag_ratio<1, 60> * si::degree>);

// coherent SI units are checked in isq_test.cpp

// non-coherent units
static_assert(1 * min == 60 * s);
static_assert(1 * h == 60 * min);
static_assert(1 * d == 24 * h);

static_assert(1 * au == 149'597'870'700 * m);

static_assert(60. * arcmin == 1. * deg);
static_assert(60. * arcsec == 1. * arcmin);

static_assert(1 * a == 100 * m2);
static_assert(1 * ha == 100 * a);

static_assert(1 * l == 1 * cubic(dm));

static_assert(1 * t == 1000 * kg);

// invoke_with_prefixed
#if MP_UNITS_HOSTED

constexpr auto validate_prefix = [](auto expected_number, Unit auto expected_unit, Quantity auto qty, auto unit,
                                    si::prefix_range range = si::prefix_range::engineering,
                                    int min_integral_digits = 1) {
  return si::invoke_with_prefixed(
    [&](auto q) {
      if (q.unit != expected_unit) return false;
      const auto actual = q.numerical_value_in(expected_unit);
      using std::abs;
      using value_type = decltype(actual);
      constexpr auto epsilon = std::numeric_limits<value_type>::epsilon() * 100;
      return abs(actual - expected_number) <= epsilon * abs(expected_number) ||
             abs(actual - expected_number) <= epsilon;
    },
    qty, unit, range, min_integral_digits);
};

// Engineering mode (default) - powers of 1000
static_assert(validate_prefix(1.5, si::volt, 1.5 * V, si::volt));
static_assert(validate_prefix(1.5, si::kilo<si::volt>, 1500.0 * V, si::volt));
static_assert(validate_prefix(1.5, si::mega<si::volt>, 1'500'000.0 * V, si::volt));
static_assert(validate_prefix(1.5, si::giga<si::volt>, 1'500'000'000.0 * V, si::volt));
static_assert(validate_prefix(1.5, si::milli<si::volt>, 0.0015 * V, si::volt));
static_assert(validate_prefix(1.5, si::micro<si::volt>, 0.000'001'5 * V, si::volt));
static_assert(validate_prefix(1.5, si::nano<si::volt>, 0.000'000'001'5 * V, si::volt));

// Engineering mode - values in range [1.0, 1000)
static_assert(validate_prefix(1.0, si::kilo<si::volt>, 1000.0 * V, si::volt));
static_assert(validate_prefix(999.0, si::volt, 999.0 * V, si::volt));
static_assert(validate_prefix(1.0, si::milli<si::volt>, 0.001 * V, si::volt));

// Full mode - includes all intermediate prefixes
static_assert(validate_prefix(4.56, si::hecto<si::metre>, 456.0 * m, si::metre, si::prefix_range::full));
static_assert(validate_prefix(1.5, si::deca<si::metre>, 15.0 * m, si::metre, si::prefix_range::full));
static_assert(validate_prefix(1.5, si::deci<si::metre>, 0.15 * m, si::metre, si::prefix_range::full));
static_assert(validate_prefix(1.5, si::centi<si::metre>, 0.015 * m, si::metre, si::prefix_range::full));

// Full mode vs engineering mode comparison
static_assert(validate_prefix(456.0, si::metre, 456.0 * m, si::metre, si::prefix_range::engineering));
static_assert(validate_prefix(4.56, si::hecto<si::metre>, 456.0 * m, si::metre, si::prefix_range::full));

// Zero values always use base unit
static_assert(validate_prefix(0.0, si::volt, 0.0 * V, si::volt));
static_assert(validate_prefix(0.0, si::metre, 0.0 * m, si::metre, si::prefix_range::full));

// Different physical quantities
static_assert(validate_prefix(1.234, si::milli<si::ampere>, 0.001'234 * A, si::ampere));
static_assert(validate_prefix(4.7, si::kilo<si::ohm>, 4700.0 * ohm, si::ohm));
static_assert(validate_prefix(470.0, si::nano<si::farad>, 0.000'000'47 * F, si::farad));
static_assert(validate_prefix(2.5, si::kilo<si::watt>, 2500.0 * W, si::watt));

// min_integral_digits parameter
static_assert(validate_prefix(12.5, si::metre, 12.5 * m, si::metre, si::prefix_range::engineering, 1));
static_assert(validate_prefix(12.5, si::metre, 12.5 * m, si::metre, si::prefix_range::engineering, 2));
static_assert(validate_prefix(1.25, si::kilo<si::metre>, 1250.0 * m, si::metre, si::prefix_range::engineering, 1));
static_assert(validate_prefix(1.250, si::metre, 1.250 * m, si::metre, si::prefix_range::engineering, 1));
static_assert(validate_prefix(125.0, si::milli<si::metre>, 0.125 * m, si::metre, si::prefix_range::engineering, 3));

// Boundary cases - exactly at prefix thresholds
static_assert(validate_prefix(1.0, si::kilo<si::metre>, 1000.0 * m, si::metre));
static_assert(validate_prefix(1.0, si::mega<si::metre>, 1'000'000.0 * m, si::metre));
static_assert(validate_prefix(1.0, si::milli<si::metre>, 0.001 * m, si::metre));
static_assert(validate_prefix(1.0, si::micro<si::metre>, 0.000'001 * m, si::metre));

// Large prefixes
static_assert(validate_prefix(1.5, si::tera<si::watt>, 1'500'000'000'000.0 * W, si::watt));
static_assert(validate_prefix(1.5, si::peta<si::watt>, 1'500'000'000'000'000.0 * W, si::watt));

// Small prefixes
static_assert(validate_prefix(1.5, si::pico<si::second>, 0.000'000'000'001'5 * s, si::second));
static_assert(validate_prefix(1.5, si::femto<si::second>, 0.000'000'000'000'001'5 * s, si::second));

// Negative values
static_assert(validate_prefix(-1.5, si::kilo<si::volt>, -1500.0 * V, si::volt));
static_assert(validate_prefix(-1.5, si::milli<si::volt>, -0.0015 * V, si::volt));

#endif  // MP_UNITS_HOSTED

}  // namespace
