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

#include <units/physical/dimensions.h>
#include <units/physical/si/length.h>
#include <units/quantity.h>

namespace units::physical::fps {

// https://en.wikipedia.org/wiki/Foot_(unit)
struct foot : named_scaled_unit<foot, "ft", no_prefix, ratio<3'048, 1'000, -1>, si::metre> {};

struct inch : named_scaled_unit<inch, "in", no_prefix, ratio<1, 12>, foot> {};

struct yard : named_scaled_unit<yard, "yd", no_prefix, ratio<3, 1>, foot> {};

struct fathom : named_scaled_unit<fathom, "ftm", no_prefix, ratio<6, 1>, foot> {};

struct mile : named_scaled_unit<mile, "mile", no_prefix, ratio<5'280>, foot> {};

struct nautical_mile : named_scaled_unit<nautical_mile, "mi(naut)", no_prefix, ratio<2000>, yard> {};



struct dim_length : physical::dim_length<foot> {};

template<Unit U, Scalar Rep = double>
using length = quantity<dim_length, U, Rep>;

inline namespace literals {

// Inch
constexpr auto operator"" q_in(unsigned long long l) { return length<inch, std::int64_t>(l); }
constexpr auto operator"" q_in(long double l) { return length<inch, long double>(l); }

// Foot
constexpr auto operator"" q_ft(unsigned long long l) { return length<foot, std::int64_t>(l); }
constexpr auto operator"" q_ft(long double l) { return length<foot, long double>(l); }

// Yard
constexpr auto operator"" q_yd(unsigned long long l) { return length<yard, std::int64_t>(l); }
constexpr auto operator"" q_yd(long double l) { return length<yard, long double>(l); }

// Fathom
constexpr auto operator"" q_ftm(unsigned long long l) { return length<fathom, std::int64_t>(l); }
constexpr auto operator"" q_ftm(long double l) { return length<fathom, long double>(l); }

// Mile
constexpr auto operator"" q_mile(unsigned long long l) { return length<mile, std::int64_t>(l); }
constexpr auto operator"" q_mile(long double l) { return length<mile, long double>(l); }

// Nautical mile
constexpr auto operator"" q_naut_mi(unsigned long long l) { return length<nautical_mile, std::int64_t>(l); }
constexpr auto operator"" q_naut_mi(long double l) { return length<nautical_mile, long double>(l); }


}

}  // namespace units::physical::fps
