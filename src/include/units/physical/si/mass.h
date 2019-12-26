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

struct gram : named_unit<gram, "g", prefix> {};
struct kilogram : prefixed_unit<kilogram, kilo, gram> {};
struct tonne : named_scaled_unit<tonne, "t", prefix, ratio<1'0000>, kilogram> {};

struct dim_mass : physical::dim_mass<kilogram> {};

template<Unit U, Scalar Rep = double>
using mass = quantity<dim_mass, U, Rep>;

inline namespace literals {

// g
constexpr auto operator""g(unsigned long long l) { return mass<gram, std::int64_t>(l); }
constexpr auto operator""g(long double l) { return mass<gram, long double>(l); }

// kg
constexpr auto operator""kg(unsigned long long l) { return mass<kilogram, std::int64_t>(l); }
constexpr auto operator""kg(long double l) { return mass<kilogram, long double>(l); }

// t
constexpr auto operator""t(unsigned long long l) { return mass<tonne, std::int64_t>(l); }
constexpr auto operator""t(long double l) { return mass<tonne, long double>(l); }

}  // namespace literals

}  // namespace units::si
