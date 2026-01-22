// The MIT License (MIT)
//
// Copyright (c) 2021 Mateusz Pusz
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
#include <mp-units/systems/hep.h>
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>

namespace {

using namespace mp_units;
using namespace mp_units::hep::unit_symbols;
using namespace mp_units::si::unit_symbols;
using mp_units::hep::unit_symbols::h;

// length
static_assert(isq::length(1 * fm) == isq::length(1e-15 * m));

// mass
static_assert(isq::mass(1'000 * eV / c2) == isq::mass(1 * keV / c2));

// atomic mass unit relationship
static_assert(approx_equal(isq::mass(1 * u), 1.660'539'066'605e-27 * kg));

// momentum
static_assert(isq::momentum(1'000'000 * eV / c) == isq::momentum(1 * MeV / c));

// area
static_assert(isq::area(1e28 * b) == isq::area(1. * m2));

// fundamental constants
static_assert(approx_equal(isq::speed(1 * c), 299'792'458. * m / s));
static_assert(approx_equal(isq::action(1 * h), 6.626'070'15e-34 * J * s));
static_assert(isq::action(1 * hbar) == isq::action(1 * h / (mag<2> * π)));
static_assert(approx_equal(isq::electric_charge(1 * e), 1.602'176'634e-19 * C, 1e-6));

}  // namespace
