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

#include <mp-units/systems/natural.h>

namespace {

using namespace mp_units;
using namespace mp_units::natural::unit_symbols;

// In natural units, all quantities are expressed in powers of GeV
// Quantity type safety is maintained through proper hierarchy

// Test quantity hierarchy - mass is a kind of energy
constexpr quantity test_mass = natural::mass(1. * GeV);
static_assert(QuantityOf<decltype(test_mass), natural::energy>);

// Test inverse_energy hierarchy - time and length are both inverse_energy
constexpr quantity test_time = natural::time(1. / GeV);
static_assert(QuantityOf<decltype(test_time), natural::inverse_energy>);
constexpr quantity test_length = natural::length(1. / GeV);
static_assert(QuantityOf<decltype(test_length), natural::inverse_energy>);

// Dimensional relationships still hold at the unit level
static_assert(1. / GeV / (1. / GeV) == 1. * one);                 // length/time = speed
static_assert(1. / GeV / ((1. / GeV) * (1. / GeV)) == 1. * GeV);  // length/time² = acceleration

// All energy-dimension quantities are compatible
static_assert(1. * GeV * (1. * one) == 1. * GeV);               // mass * velocity = momentum (dimensionally)
static_assert(1. * GeV * (1. * GeV) == 1. * GeV2);              // mass * acceleration = force
static_assert(1. * GeV * (1. * GeV) * (1. / GeV) == 1. * GeV);  // force * length = energy

// Quantity hierarchy tests
using namespace mp_units::detail;
using enum specs_convertible_result;

static_assert(convertible(natural::mass, natural::energy) == yes);
static_assert(convertible(natural::energy, natural::mass) == explicit_conversion);
static_assert(convertible(natural::energy, natural::momentum) == explicit_conversion);
static_assert(convertible(natural::mass, natural::momentum) == cast);

static_assert(convertible(natural::time, natural::inverse_energy) == yes);
static_assert(convertible(natural::inverse_energy, natural::time) == explicit_conversion);
static_assert(convertible(natural::inverse_energy, natural::length) == explicit_conversion);
static_assert(convertible(natural::time, natural::length) == cast);

// Dimensionless quantity tests
static_assert(convertible(natural::velocity, natural::speed) == yes);
static_assert(convertible(natural::speed, natural::velocity) == explicit_conversion);
static_assert(convertible(natural::speed, dimensionless) == yes);
static_assert(convertible(natural::angular_measure, dimensionless) == yes);
static_assert(convertible(natural::speed, natural::angular_measure) == cast);

// Acceleration tests (acceleration has dimension energy in natural units)
static_assert(convertible(natural::acceleration, natural::energy) == yes);
static_assert(convertible(natural::energy, natural::acceleration) == explicit_conversion);

// Force tests
static_assert(convertible(natural::force, natural::energy_squared) == yes);
static_assert(convertible(natural::energy_squared, natural::force) == explicit_conversion);

// Cross-hierarchy tests (should never convert)
static_assert(convertible(natural::energy, natural::inverse_energy) == no);
static_assert(convertible(natural::energy, natural::energy_squared) == no);
static_assert(convertible(natural::energy, dimensionless) == no);

}  // namespace
