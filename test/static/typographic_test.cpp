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

#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si.h>
#include <mp-units/systems/typographic.h>
#include <mp-units/systems/yard_pound.h>

namespace {

using namespace mp_units;
using namespace mp_units::typographic;
using namespace mp_units::typographic::unit_symbols;
using namespace mp_units::yard_pound::unit_symbols;

// US typographic units
static_assert(isq::length(1 * pica_us) == isq::length(12 * point_us));
static_assert(isq::length(1 * pc_us) == isq::length(12 * pt_us));

// DTP (PostScript) typographic units
static_assert(isq::length(1 * pica_dtp) == isq::length(12 * point_dtp));
static_assert(isq::length(1 * pc) == isq::length(12 * pt));
static_assert(isq::length(72 * point_dtp) == 1 * in);
static_assert(isq::length(72 * pt) == 1 * in);

// Didot and Cicero (European typographic units)
static_assert(isq::length(1 * cicero) == isq::length(12 * didot_point));
static_assert(isq::length(1 * cc) == isq::length(12 * pt_didot));

// Q and H (Japanese typographic units)
static_assert(isq::length(4 * q) == 1 * si::milli<si::metre>);
static_assert(isq::length(4 * h) == 1 * si::milli<si::metre>);
static_assert(isq::length(1 * q) == isq::length(1 * h));
static_assert(isq::length(1 * Q) == isq::length(1 * H));

// TeX point
static_assert(isq::length(72.27 * tex_point).numerical_value_in(in) == 1);
static_assert(isq::length(72.27 * pt_tex).numerical_value_in(in) == 1);

}  // namespace
