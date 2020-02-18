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
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::si {

struct metre : named_unit<metre, "m", prefix> {};
struct millimetre : prefixed_unit<millimetre, milli, metre> {};
struct centimetre : prefixed_unit<centimetre, centi, metre> {};
struct decimetre : prefixed_unit<decimetre, deci, metre> {};
struct hectometre : prefixed_unit<hectometre, hecto, metre> {};
struct kilometre : prefixed_unit<kilometre, kilo, metre> {};
struct femtometre : prefixed_unit<femtometre, femto, metre> {};

struct astronomical_unit : named_scaled_unit<astronomical_unit, "au", no_prefix, ratio<149'597'870'700>, metre> {};

struct dim_length : physical::dim_length<metre> {};

template<Unit U, Scalar Rep = double>
using length = quantity<dim_length, U, Rep>;

inline namespace literals {

// m
constexpr auto operator"" q_m(unsigned long long l) { return length<metre, std::int64_t>(l); }
constexpr auto operator"" q_m(long double l) { return length<metre, long double>(l); }

// fm
constexpr auto operator"" q_fm(unsigned long long l) { return length<femtometre, std::int64_t>(l); }
constexpr auto operator"" q_fm(long double l) { return length<femtometre, long double>(l); }

// mm
constexpr auto operator"" q_mm(unsigned long long l) { return length<millimetre, std::int64_t>(l); }
constexpr auto operator"" q_mm(long double l) { return length<millimetre, long double>(l); }

// cm
constexpr auto operator"" q_cm(unsigned long long l) { return length<centimetre, std::int64_t>(l); }
constexpr auto operator"" q_cm(long double l) { return length<centimetre, long double>(l); }

// dm
constexpr auto operator"" q_dm(unsigned long long l) { return length<decimetre, std::int64_t>(l); }
constexpr auto operator"" q_dm(long double l) { return length<decimetre, long double>(l); }

// hm
constexpr auto operator"" q_hm(unsigned long long l) { return length<hectometre, std::int64_t>(l); }
constexpr auto operator"" q_hm(long double l) { return length<hectometre, long double>(l); }

// km
constexpr auto operator"" q_km(unsigned long long l) { return length<kilometre, std::int64_t>(l); }
constexpr auto operator"" q_km(long double l) { return length<kilometre, long double>(l); }

// au
constexpr auto operator"" q_au(unsigned long long l) { return length<astronomical_unit, std::int64_t>(l); }
constexpr auto operator"" q_au(long double l) { return length<astronomical_unit, long double>(l); }

}  // namespace literals

}  // namespace units::si
