
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

/*
 international units of length are defined in terms of the si metre
*/

namespace units::international {

//international yard
//https://en.wikipedia.org/wiki/International_yard_and_pound
struct yard : named_scaled_unit<yard, "yd", no_prefix, ratio<9'144, 1'000,-1>, si::metre> {};

//international foot
//https://en.wikipedia.org/wiki/Foot_(unit)#International_foot
struct foot : named_scaled_unit<foot, "ft", no_prefix, ratio<3'048,1'000,-1>, si::metre> {};

//https://en.wikipedia.org/wiki/Fathom#International_fathom
struct fathom : named_scaled_unit<fathom, "fathom",no_prefix,ratio<1'828,1'000>,si::metre > {};

//international inch
//https://en.wikipedia.org/wiki/Inch#Equivalences
struct inch : named_scaled_unit<inch, "in", no_prefix, ratio<254,100,-2>, si::metre> {};

//intrnational mile
//https://en.wikipedia.org/wiki/Mile#International_mile
struct mile : named_scaled_unit<mile, "mi", no_prefix, ratio<25'146,15'625,3>, si::metre> {};

//international nautical mile
//https://en.wikipedia.org/wiki/Mile#Nautical_mile
struct nautical_mile : named_scaled_unit<nautical_mile,"mi(naut)",no_prefix, ratio<463,250,3>,si::metre> {};

//TODO thou
//https://en.wikipedia.org/wiki/Thousandth_of_an_inch
//struct thou : named_scaled_unit<thou,"thou",no_prefix,ratio<127,50,-5>,si::metre> {};
// different name for thou
//https://en.wikipedia.org/wiki/Thousandth_of_an_inch
struct mil : named_scaled_unit<mil,"mil",no_prefix,ratio<127,50,-5>,si::metre> {};

inline namespace literals {

// yd
constexpr auto operator"" yd(unsigned long long l) { return si::length<yard, std::int64_t>(l); }
constexpr auto operator"" yd(long double l) { return si::length<yard, long double>(l); }

// ft
constexpr auto operator"" ft(unsigned long long l) { return si::length<foot, std::int64_t>(l); }
constexpr auto operator"" ft(long double l) { return si::length<foot, long double>(l); }

// fathom
constexpr auto operator"" fathom(unsigned long long l) { return si::length<fathom, std::int64_t>(l); }
constexpr auto operator"" fathom(long double l) { return si::length<fathom, long double>(l); }

// in
constexpr auto operator"" in(unsigned long long l) { return si::length<inch, std::int64_t>(l); }
constexpr auto operator"" in(long double l) { return si::length<inch, long double>(l); }

// mi
constexpr auto operator"" mi(unsigned long long l) { return si::length<mile, std::int64_t>(l); }
constexpr auto operator"" mi(long double l) { return si::length<mile, long double>(l); }

// mi_naut
constexpr auto operator"" naut_mi(unsigned long long l) { return si::length<nautical_mile, std::int64_t>(l); }
constexpr auto operator"" naut_mi(long double l) { return si::length<nautical_mile, long double>(l); }

//TODO thou
//constexpr auto operator"" thou(unsigned long long l) { return si::length<thou, std::int64_t>(l); }
//constexpr auto operator"" thou(long double l) { return si::length<thou, long double>(l); }

// mil
constexpr auto operator"" mil(unsigned long long l) { return si::length<mil, std::int64_t>(l); }
constexpr auto operator"" mil(long double l) { return si::length<mil, long double>(l); }

} // namespace literals

} //namespace units::international