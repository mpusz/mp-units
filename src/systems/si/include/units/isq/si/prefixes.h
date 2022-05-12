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
struct yocto  : units::prefix<yocto, "y",             ratio(1, 1, -24)> {};
struct zepto  : units::prefix<zepto, "z",             ratio(1, 1, -21)> {};
struct atto   : units::prefix<atto,  "a",             ratio(1, 1, -18)> {};
struct femto  : units::prefix<femto, "f",             ratio(1, 1, -15)> {};
struct pico   : units::prefix<pico,  "p",             ratio(1, 1, -12)> {};
struct nano   : units::prefix<nano,  "n",             ratio(1, 1,  -9)> {};
struct micro  : units::prefix<micro, basic_symbol_text{"\u00b5", "u"}, ratio(1, 1,  -6)> {};
struct milli  : units::prefix<milli, "m",             ratio(1, 1,  -3)> {};
struct centi  : units::prefix<centi, "c",             ratio(1, 1,  -2)> {};
struct deci   : units::prefix<deci,  "d",             ratio(1, 1,  -1)> {};
struct deca   : units::prefix<deca,  "da",            ratio(1, 1,   1)> {};
struct hecto  : units::prefix<hecto, "h",             ratio(1, 1,   2)> {};
struct kilo   : units::prefix<kilo,  "k",             ratio(1, 1,   3)> {};
struct mega   : units::prefix<mega,  "M",             ratio(1, 1,   6)> {};
struct giga   : units::prefix<giga,  "G",             ratio(1, 1,   9)> {};
struct tera   : units::prefix<tera,  "T",             ratio(1, 1,  12)> {};
struct peta   : units::prefix<peta,  "P",             ratio(1, 1,  15)> {};
struct exa    : units::prefix<exa,   "E",             ratio(1, 1,  18)> {};
struct zetta  : units::prefix<zetta, "Z",             ratio(1, 1,  21)> {};
struct yotta  : units::prefix<yotta, "Y",             ratio(1, 1,  24)> {};
// clang-format on

}  // namespace units::isq::si
