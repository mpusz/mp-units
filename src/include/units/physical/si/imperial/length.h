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

#include <units/physical/si/international/length.h>

namespace units::physical::si::imperial {

// https://en.wikipedia.org/wiki/Chain_(unit)
struct chain : named_scaled_unit<chain, "ch", no_prefix, ratio(22, 1), si::international::yard> {};

// https://en.wikipedia.org/wiki/Rod_(unit)
struct rod : named_scaled_unit<rod, "rd", no_prefix, ratio(1, 4), chain> {};

inline namespace literals {

constexpr auto operator"" _q_ch(unsigned long long l) { return si::length<chain, std::int64_t>(l); }
constexpr auto operator"" _q_ch(long double l) { return si::length<chain, long double>(l); }

constexpr auto operator"" _q_rd(unsigned long long l) { return si::length<rod, std::int64_t>(l); }
constexpr auto operator"" _q_rd(long double l) { return si::length<rod, long double>(l); }

}  // namespace literals

}  // namespace units::physical::si::imperial
