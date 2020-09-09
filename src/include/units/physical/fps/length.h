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
struct foot : named_scaled_unit<foot, "ft", no_prefix, ratio(3'048, 1'000, -1), si::metre> {};

struct inch : named_scaled_unit<inch, "in", no_prefix, ratio(1, 12), foot> {};

// thousandth of an inch
struct thousandth : named_scaled_unit<thousandth, "thou", no_prefix, ratio(1, 1'000), inch> {};
struct thou : alias_unit<thousandth, "thou", no_prefix>{};
struct mil : alias_unit<thousandth, "mil", no_prefix>{};

struct yard : named_scaled_unit<yard, "yd", si::prefix, ratio(3, 1), foot> {};

struct fathom : named_scaled_unit<fathom, "ftm", no_prefix, ratio(6, 1), foot> {};

struct kiloyard : prefixed_unit<kiloyard, si::kilo, yard> {};

struct mile : named_scaled_unit<mile, "mile", no_prefix, ratio(5'280), foot> {};

struct nautical_mile : named_scaled_unit<nautical_mile, "mi(naut)", no_prefix, ratio(2'000), yard> {};



struct dim_length : physical::dim_length<foot> {};

template<Unit U, ScalableNumber Rep = double>
using length = quantity<dim_length, U, Rep>;

inline namespace literals {

// Thousandth
constexpr auto operator"" _q_thou(unsigned long long l) { return length<thousandth, std::int64_t>(l); }
constexpr auto operator"" _q_thou(long double l) { return length<thousandth, long double>(l); }
constexpr auto operator"" _q_mil(unsigned long long l) { return length<thousandth, std::int64_t>(l); }
constexpr auto operator"" _q_mil(long double l) { return length<thousandth, long double>(l); }


// Inch
constexpr auto operator"" _q_in(unsigned long long l) { return length<inch, std::int64_t>(l); }
constexpr auto operator"" _q_in(long double l) { return length<inch, long double>(l); }

// Foot
constexpr auto operator"" _q_ft(unsigned long long l) { return length<foot, std::int64_t>(l); }
constexpr auto operator"" _q_ft(long double l) { return length<foot, long double>(l); }

// Yard
constexpr auto operator"" _q_yd(unsigned long long l) { return length<yard, std::int64_t>(l); }
constexpr auto operator"" _q_yd(long double l) { return length<yard, long double>(l); }

// Fathom
constexpr auto operator"" _q_ftm(unsigned long long l) { return length<fathom, std::int64_t>(l); }
constexpr auto operator"" _q_ftm(long double l) { return length<fathom, long double>(l); }

// Kiloyard
constexpr auto operator"" _q_kyd(unsigned long long l) { return length<kiloyard, std::int64_t>(l); }
constexpr auto operator"" _q_kyd(long double l) { return length<kiloyard, long double>(l); }

// Mile
constexpr auto operator"" _q_mile(unsigned long long l) { return length<mile, std::int64_t>(l); }
constexpr auto operator"" _q_mile(long double l) { return length<mile, long double>(l); }

// Nautical mile
constexpr auto operator"" _q_naut_mi(unsigned long long l) { return length<nautical_mile, std::int64_t>(l); }
constexpr auto operator"" _q_naut_mi(long double l) { return length<nautical_mile, long double>(l); }


}

}  // namespace units::physical::fps
