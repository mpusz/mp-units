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

#include <mp-units/systems/imperial/imperial.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si/unit_symbols.h>

namespace {

using namespace mp_units;
using namespace mp_units::imperial;
using namespace mp_units::imperial::unit_symbols;

// https://en.wikipedia.org/wiki/United_States_customary_units#Length

// International
static_assert(17'280 * isq::length[twip] == 1 * isq::length[ft]);
static_assert(12'000 * isq::length[th] == 1 * isq::length[ft]);
static_assert(3 * isq::length[Bc] == 1'000 * isq::length[th]);
static_assert(1 * isq::length[in] == 3 * isq::length[Bc]);
static_assert(1 * isq::length[hh] == 4 * isq::length[in]);
static_assert(1 * isq::length[ft] == 3 * isq::length[hh]);
static_assert(1 * isq::length[yd] == 3 * isq::length[ft]);
static_assert(1 * isq::length[ch] == 22 * isq::length[yd]);
static_assert(1 * isq::length[fur] == 10 * isq::length[ch]);
static_assert(1 * isq::length[mi] == 8 * isq::length[fur]);
static_assert(1 * isq::length[le] == 3 * isq::length[mi]);

// International Nautical
static_assert(1 * isq::length[cb] == 100 * isq::length[ftm]);
static_assert(1 * isq::length[nmi] == 10 * isq::length[cb]);

// survey
static_assert(100 * isq::length[li] == 1 * isq::length[ch]);
static_assert(1 * isq::length[rd] == 25 * isq::length[li]);

// Area
static_assert(1 * isq::area[perch] == 1 * isq::length[rd] * (1 * isq::length[rd]));
static_assert(1 * isq::area[rood] == 1 * isq::length[fur] * (1 * isq::length[rd]));
static_assert(1 * isq::area[acre] == 1 * isq::length[fur] * (1 * isq::length[ch]));

// Volume
static_assert(20 * isq::volume[fl_oz] == 1 * isq::volume[pt]);
static_assert(1 * isq::volume[gi] == 5 * isq::volume[fl_oz]);
static_assert(1 * isq::volume[pt] == 20 * isq::volume[fl_oz]);
static_assert(1 * isq::volume[qt] == 2 * isq::volume[pt]);
static_assert(1 * isq::volume[gal] == 8 * isq::volume[pt]);

// Mass
static_assert(7'000 * isq::mass[gr] == 1 * isq::mass[lb]);
static_assert(256 * isq::mass[dr] == 1 * isq::mass[lb]);
static_assert(1 * isq::mass[oz] == 16 * isq::mass[dr]);
static_assert(1 * isq::mass[lb] == 16 * isq::mass[oz]);
static_assert(1 * isq::mass[st] == 14 * isq::mass[lb]);
static_assert(1 * isq::mass[qr] == 2 * isq::mass[st]);
static_assert(1 * isq::mass[cwt] == 112 * isq::mass[lb]);
static_assert(1 * isq::mass[t] == 20 * isq::mass[cwt]);

}  // namespace
