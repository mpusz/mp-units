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

namespace units::si {

struct cubic_metre : unit<cubic_metre> {};
struct dim_volume : physical::dim_volume<dim_volume, cubic_metre, dim_length> {};

struct cubic_millimetre : deduced_unit<cubic_millimetre, dim_volume, millimetre> {};
struct cubic_centimetre : deduced_unit<cubic_centimetre, dim_volume, centimetre> {};
struct cubic_kilometre : deduced_unit<cubic_kilometre, dim_volume, kilometre> {};

struct litre : deduced_unit<litre, dim_volume, decimetre> {};


template<Unit U, Scalar Rep = double>
using volume = quantity<dim_volume, U, Rep>;

inline namespace literals {

// mm3
constexpr auto operator""mm3(unsigned long long l) { return volume<cubic_millimetre, std::int64_t>(l); }
constexpr auto operator""mm3(long double l) { return volume<cubic_millimetre, long double>(l); }

// cm3
constexpr auto operator""cm3(unsigned long long l) { return volume<cubic_centimetre, std::int64_t>(l); }
constexpr auto operator""cm3(long double l) { return volume<cubic_centimetre, long double>(l); }

// m3
constexpr auto operator""m3(unsigned long long l) { return volume<cubic_metre, std::int64_t>(l); }
constexpr auto operator""m3(long double l) { return volume<cubic_metre, long double>(l); }

// km3
constexpr auto operator""km3(unsigned long long l) { return volume<cubic_kilometre, std::int64_t>(l); }
constexpr auto operator""km3(long double l) { return volume<cubic_kilometre, long double>(l); }

// l
constexpr auto operator""_l(unsigned long long l) { return volume<litre, std::int64_t>(l); }
constexpr auto operator""_l(long double l) { return volume<litre, long double>(l); }

}  // namespace literals

}  // namespace units::si
