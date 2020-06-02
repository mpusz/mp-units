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

#include <units/physical/si/volume.h>
#include <units/physical/international/length.h>

namespace units::physical::international {

struct cubic_foot : deduced_unit<cubic_foot, si::dim_volume, international::foot> {};

//barrel
//https://en.wikipedia.org/wiki/Volume_units_used_in_petroleum_engineering
struct barrel : named_scaled_unit<barrel,"stb", no_prefix, ratio<1'000'000,6'289'811> , si::cubic_metre> {};

inline namespace literals {

// ft3
constexpr auto operator"" q_ft3(unsigned long long l) { return si::volume<cubic_foot, std::int64_t>(l); }
constexpr auto operator"" q_ft3(long double l) { return si::volume<cubic_foot, long double>(l); }

// ft3
constexpr auto operator"" q_stb(unsigned long long l) { return si::volume<barrel, std::int64_t>(l); }
constexpr auto operator"" q_stb(long double l) { return si::volume<barrel, long double>(l); }

}  // namespace literals

}  // namespace units::physical::international
