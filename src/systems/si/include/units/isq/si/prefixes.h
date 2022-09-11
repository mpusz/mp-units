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

#pragma once

#include <units/prefix.h>

namespace units::isq::si {

// clang-format off
struct yocto  : prefix<yocto, "y",             mag_power<10, -24>()> {};
struct zepto  : prefix<zepto, "z",             mag_power<10, -21>()> {};
struct atto   : prefix<atto,  "a",             mag_power<10, -18>()> {};
struct femto  : prefix<femto, "f",             mag_power<10, -15>()> {};
struct pico   : prefix<pico,  "p",             mag_power<10, -12>()> {};
struct nano   : prefix<nano,  "n",             mag_power<10, -9>()> {};
struct micro  : prefix<micro, basic_symbol_text{"\u00b5", "u"}, mag_power<10, -6>()> {};
struct milli  : prefix<milli, "m",             mag_power<10, -3>()> {};
struct centi  : prefix<centi, "c",             mag_power<10, -2>()> {};
struct deci   : prefix<deci,  "d",             mag_power<10, -1>()> {};
struct deca   : prefix<deca,  "da",            mag_power<10, 1>()> {};
struct hecto  : prefix<hecto, "h",             mag_power<10, 2>()> {};
struct kilo   : prefix<kilo,  "k",             mag_power<10, 3>()> {};
struct mega   : prefix<mega,  "M",             mag_power<10, 6>()> {};
struct giga   : prefix<giga,  "G",             mag_power<10, 9>()> {};
struct tera   : prefix<tera,  "T",             mag_power<10, 12>()> {};
struct peta   : prefix<peta,  "P",             mag_power<10, 15>()> {};
struct exa    : prefix<exa,   "E",             mag_power<10, 18>()> {};
struct zetta  : prefix<zetta, "Z",             mag_power<10, 21>()> {};
struct yotta  : prefix<yotta, "Y",             mag_power<10, 24>()> {};
// clang-format on

}  // namespace units::isq::si
