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

namespace units::us {

struct yard : named_scaled_unit<yard, "yd", no_prefix, ratio<9'144, 10'000>, si::metre> {};
struct foot : named_scaled_unit<foot, "ft", no_prefix, ratio<1, 3>, yard> {};
struct inch : named_scaled_unit<inch, "in", no_prefix, ratio<1, 12>, foot> {};
struct mile : named_scaled_unit<mile, "mi", no_prefix, ratio<1'760>, yard> {};

inline namespace literals {

// yd
constexpr auto operator"" yd(unsigned long long l) { return si::length<yard, std::int64_t>(l); }
constexpr auto operator"" yd(long double l) { return si::length<yard, long double>(l); }

// ft
constexpr auto operator"" ft(unsigned long long l) { return si::length<foot, std::int64_t>(l); }
constexpr auto operator"" ft(long double l) { return si::length<foot, long double>(l); }

// in
constexpr auto operator"" in(unsigned long long l) { return si::length<inch, std::int64_t>(l); }
constexpr auto operator"" in(long double l) { return si::length<inch, long double>(l); }

// mi
constexpr auto operator"" mi(unsigned long long l) { return si::length<mile, std::int64_t>(l); }
constexpr auto operator"" mi(long double l) { return si::length<mile, long double>(l); }

}  // namespace literals

}  // namespace units::us
