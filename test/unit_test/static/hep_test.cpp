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

#include <units/systems/hep/hep.h>
#include <units/systems/isq/mechanics.h>
#include <units/systems/isq/space_and_time.h>
#include <units/systems/si/si.h>

template<class T>
  requires units::is_scalar<T>
inline constexpr bool units::is_vector<T> = true;

namespace {

using namespace units;
using namespace units::hep::unit_symbols;
using namespace units::si::unit_symbols;

// mass
static_assert(isq::mass[eV / c2](1'000) == isq::mass[keV / c2](1));

// momentum
static_assert(isq::momentum[eV / c](1'000'000) == isq::momentum[MeV / c](1));

// area
static_assert(isq::area[b](1e28) == isq::area[m2](1.));

}  // namespace
