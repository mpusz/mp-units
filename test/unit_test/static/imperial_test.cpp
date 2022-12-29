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

#include <units/imperial/imperial.h>
#include <units/isq/space_and_time.h>
#include <units/si/unit_symbols.h>

namespace {

using namespace units;
using namespace units::imperial;
using namespace units::imperial::unit_symbols;

/* ************** BASE DIMENSIONS **************** */

// https://en.wikipedia.org/wiki/United_States_customary_units#Length

// International
static_assert(isq::length[twip](17'280) == isq::length[ft](1));
static_assert(isq::length[th](12'000) == isq::length[ft](1));
static_assert(isq::length[Bc](3) == isq::length[th](1'000));
static_assert(isq::length[in](1) == isq::length[Bc](3));
static_assert(isq::length[hh](1) == isq::length[in](4));
static_assert(isq::length[ft](1) == isq::length[hh](3));
static_assert(isq::length[yd](1) == isq::length[ft](3));
static_assert(isq::length[ch](1) == isq::length[yd](22));
static_assert(isq::length[fur](1) == isq::length[ch](10));
static_assert(isq::length[mi](1) == isq::length[fur](8));
static_assert(isq::length[le](1) == isq::length[mi](3));

// International Nautical
static_assert(isq::length[cb](1) == isq::length[ftm](100));
static_assert(isq::length[nmi](1) == isq::length[cb](10));

// survey
static_assert(isq::length[li](100) == isq::length[ch](1));
static_assert(isq::length[rd](1) == isq::length[li](25));

// Area
static_assert(isq::area[perch](1) == isq::length[rd](1) * isq::length[rd](1));
static_assert(isq::area[rood](1) == isq::length[fur](1) * isq::length[rd](1));
static_assert(isq::area[acre](1) == isq::length[fur](1) * isq::length[ch](1));

// Volume
static_assert(isq::volume[fl_oz](20) == isq::volume[pt](1));
static_assert(isq::volume[gi](1) == isq::volume[fl_oz](5));
static_assert(isq::volume[pt](1) == isq::volume[fl_oz](20));
static_assert(isq::volume[qt](1) == isq::volume[pt](2));
static_assert(isq::volume[gal](1) == isq::volume[pt](8));

// Mass
static_assert(isq::mass[gr](7'000) == isq::mass[lb](1));
static_assert(isq::mass[dr](256) == isq::mass[lb](1));
static_assert(isq::mass[oz](1) == isq::mass[dr](16));
static_assert(isq::mass[lb](1) == isq::mass[oz](16));
static_assert(isq::mass[st](1) == isq::mass[lb](14));
static_assert(isq::mass[qr](1) == isq::mass[st](2));
static_assert(isq::mass[cwt](1) == isq::mass[lb](112));
static_assert(isq::mass[t](1) == isq::mass[cwt](20));

}  // namespace
