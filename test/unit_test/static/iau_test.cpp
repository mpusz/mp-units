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
#include <mp-units/systems/iau/iau.h>
#include <mp-units/systems/si/unit_symbols.h>

/* ************** DERIVED DIMENSIONS THAT INCLUDE UNITS WITH SPECIAL NAMES **************** */

namespace {

using namespace mp_units;
using namespace mp_units::iau;
using namespace mp_units::iau::unit_symbols;
using enum mp_units::quantity_character;

static_assert(isq::time(1 * D) == 86'400 * si::second);
static_assert(isq::time(1 * a) == 365.25 * D);

static_assert(isq::length(1 * au) == 149'597'870'700 * si::metre);
static_assert(isq::length(1 * LD) == 384'399 * si::kilo<si::metre>);
static_assert(isq::length(1 * ly) == 9'460'730'472'580'800 * si::metre);
static_assert(isq::length(10'000'000'000 * A) == 1 * si::metre);

#if __cpp_lib_constexpr_cmath || MP_UNITS_COMP_GCC
// TODO Should the below work for `1 * pc`? If yes, how to extent the type and how to convert it to a floating-point
// representation for comparison purposes?
static_assert(round<si::metre>(isq::length(1.L * pc)) == 30'856'775'814'913'673 * si::metre);
#endif

static_assert(isq::speed(1 * c_0) == 299'792'458 * si::metre / si::second);

}  // namespace
