
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

#include <units/physical/si/length.h>

namespace units::typographic {

struct pica_comp : named_scaled_unit<pica_comp,"pica(comp)",no_prefix,ratio<4233333,1000000,-3>,si::metre> {};
struct pica_prn : named_scaled_unit<pica_prn,"pica(prn)",no_prefix,ratio<2108759,500000,-3>,si::metre> {};
struct point_comp : named_scaled_unit<point_comp,"point(comp)",no_prefix,ratio<1763889,500000,-4>,si::metre> {};
struct point_prn  : named_scaled_unit<point_prn,"point(prn)", no_prefix, ratio<1757299,500000,-4>,si::metre> {};

inline namespace literals {

  // pica comp
constexpr auto operator"" pica_comp(unsigned long long l) { return si::length<pica_comp, std::int64_t>(l); }
constexpr auto operator"" pica_comp(long double l) { return si::length<pica_comp, long double>(l); }

  // pica prn
constexpr auto operator"" pica_prn(unsigned long long l) { return si::length<pica_prn, std::int64_t>(l); }
constexpr auto operator"" pica_prn(long double l) { return si::length<pica_prn, long double>(l); }

  // point comp
constexpr auto operator"" point_comp(unsigned long long l) { return si::length<point_comp, std::int64_t>(l); }
constexpr auto operator"" point_comp(long double l) { return si::length<point_comp, long double>(l); }

  // point prn
constexpr auto operator"" point_prn(unsigned long long l) { return si::length<point_prn, std::int64_t>(l); }
constexpr auto operator"" point_prn(long double l) { return si::length<point_prn, long double>(l); }

}  // namespace literals

}  // namespace units::typographic
