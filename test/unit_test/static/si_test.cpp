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

#include <mp-units/math.h>
#include <mp-units/systems/si/si.h>

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
template<template<auto U> typename prefix, auto V1>
concept can_not_be_prefixed = Unit<std::remove_const_t<decltype(V1)>> && !requires { typename prefix<V1>; };

static_assert(can_not_be_prefixed<si::milli_, si::degree_Celsius>);
static_assert(can_not_be_prefixed<si::milli_, si::minute>);
static_assert(can_not_be_prefixed<si::milli_, si::hour>);
static_assert(can_not_be_prefixed<si::milli_, si::day>);
static_assert(can_not_be_prefixed<si::milli_, si::kilogram>);
static_assert(can_not_be_prefixed<si::milli_, si::hectare>);
static_assert(can_not_be_prefixed<si::milli_, si::kilo<si::metre>>);
static_assert(can_not_be_prefixed<si::milli_, si::metre / si::second>);
static_assert(can_not_be_prefixed<si::milli_, mag<ratio{1, 60}> * si::degree>);

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

}  // namespace
