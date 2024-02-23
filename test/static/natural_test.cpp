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
#include <mp-units/systems/natural/natural.h>

template<class T>
  requires mp_units::is_scalar<T>
inline constexpr bool mp_units::is_vector<T> = true;

namespace {

using namespace mp_units;
using namespace mp_units::natural::unit_symbols;

static_assert(1 * natural::length[one / GeV] / (1 * natural::time[one / GeV]) == 1 * natural::speed[one]);
static_assert(1 * natural::length[one / GeV] / (1 * natural::time[one / GeV] * (1 * natural::time[one / GeV])) ==
              1 * natural::acceleration[GeV]);
static_assert(1 * natural::mass[GeV] * (1 * natural::velocity[one]) == 1 * natural::momentum[GeV]);
static_assert(1 * natural::mass[GeV] * (1 * natural::acceleration[GeV]) == 1 * natural::force[GeV2]);
static_assert(1 * natural::mass[GeV] * (1 * natural::acceleration[GeV]) * (1 * natural::length[one / GeV]) ==
              1 * natural::energy[GeV]);

}  // namespace
