
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

namespace units::physical::international {

// international yard
// https://en.wikipedia.org/wiki/International_yard_and_pound
struct yard : named_scaled_unit<yard, "yd", no_prefix, ratio(9'144, 1'000, -1), si::metre> {};

// international foot
// https://en.wikipedia.org/wiki/Foot_(unit)#International_foot
struct foot : named_scaled_unit<foot, "ft", no_prefix, ratio(1, 3), yard> {};

// https://en.wikipedia.org/wiki/Fathom#International_fathom
struct fathom : named_scaled_unit<fathom, "fathom", no_prefix, ratio(2), yard> {};

// international inch
// https://en.wikipedia.org/wiki/Inch#Equivalences
struct inch : named_scaled_unit<inch, "in", no_prefix, ratio(1, 36), yard> {};

// intrnational mile
// https://en.wikipedia.org/wiki/Mile#International_mile
struct mile : named_scaled_unit<mile, "mi", no_prefix, ratio(25'146, 15'625), si::kilometre> {};

// international nautical mile
// https://en.wikipedia.org/wiki/Nautical_mile
struct nautical_mile : named_scaled_unit<nautical_mile, "mi(naut)", no_prefix, ratio(1852), si::metre> {};

// thou
// https://en.wikipedia.org/wiki/Thousandth_of_an_inch
struct thou : named_scaled_unit<thou, "thou", no_prefix, ratio(1, 1000), inch> {};

// mil - different name for thou
// https://en.wikipedia.org/wiki/Thousandth_of_an_inch
using mil = thou;

inline namespace literals {

// yd
constexpr auto operator"" _q_yd(unsigned long long l) { return si::length<yard, std::int64_t>(l); }
constexpr auto operator"" _q_yd(long double l) { return si::length<yard, long double>(l); }

// ft
constexpr auto operator"" _q_ft(unsigned long long l) { return si::length<foot, std::int64_t>(l); }
constexpr auto operator"" _q_ft(long double l) { return si::length<foot, long double>(l); }

// fathom
constexpr auto operator"" _q_fathom(unsigned long long l) { return si::length<fathom, std::int64_t>(l); }
constexpr auto operator"" _q_fathom(long double l) { return si::length<fathom, long double>(l); }

// in
constexpr auto operator"" _q_in(unsigned long long l) { return si::length<inch, std::int64_t>(l); }
constexpr auto operator"" _q_in(long double l) { return si::length<inch, long double>(l); }

// mi
constexpr auto operator"" _q_mi(unsigned long long l) { return si::length<mile, std::int64_t>(l); }
constexpr auto operator"" _q_mi(long double l) { return si::length<mile, long double>(l); }

// mi_naut
constexpr auto operator"" _q_naut_mi(unsigned long long l) { return si::length<nautical_mile, std::int64_t>(l); }
constexpr auto operator"" _q_naut_mi(long double l) { return si::length<nautical_mile, long double>(l); }

// thou
constexpr auto operator"" _q_thou(unsigned long long l) { return si::length<thou, std::int64_t>(l); }
constexpr auto operator"" _q_thou(long double l) { return si::length<thou, long double>(l); }

// mil
constexpr auto operator"" _q_mil(unsigned long long l) { return si::length<mil, std::int64_t>(l); }
constexpr auto operator"" _q_mil(long double l) { return si::length<mil, long double>(l); }

}  // namespace literals

}  // namespace units::physical::international
