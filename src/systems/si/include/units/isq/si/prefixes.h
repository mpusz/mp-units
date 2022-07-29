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
struct yocto  : prefix<yocto, "y",             pow<-24>(mag<10>())> {};
struct zepto  : prefix<zepto, "z",             pow<-21>(mag<10>())> {};
struct atto   : prefix<atto,  "a",             pow<-18>(mag<10>())> {};
struct femto  : prefix<femto, "f",             pow<-15>(mag<10>())> {};
struct pico   : prefix<pico,  "p",             pow<-12>(mag<10>())> {};
struct nano   : prefix<nano,  "n",             pow<-9>(mag<10>())> {};
struct micro  : prefix<micro, basic_symbol_text{"\u00b5", "u"}, pow<-6>(mag<10>())> {};
struct milli  : prefix<milli, "m",             pow<-3>(mag<10>())> {};
struct centi  : prefix<centi, "c",             pow<-2>(mag<10>())> {};
struct deci   : prefix<deci,  "d",             pow<-1>(mag<10>())> {};
struct deca   : prefix<deca,  "da",            pow<1>(mag<10>())> {};
struct hecto  : prefix<hecto, "h",             pow<2>(mag<10>())> {};
struct kilo   : prefix<kilo,  "k",             pow<3>(mag<10>())> {};
struct mega   : prefix<mega,  "M",             pow<6>(mag<10>())> {};
struct giga   : prefix<giga,  "G",             pow<9>(mag<10>())> {};
struct tera   : prefix<tera,  "T",             pow<12>(mag<10>())> {};
struct peta   : prefix<peta,  "P",             pow<15>(mag<10>())> {};
struct exa    : prefix<exa,   "E",             pow<18>(mag<10>())> {};
struct zetta  : prefix<zetta, "Z",             pow<21>(mag<10>())> {};
struct yotta  : prefix<yotta, "Y",             pow<24>(mag<10>())> {};
// clang-format on

}  // namespace units::isq::si
