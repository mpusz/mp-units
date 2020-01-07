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
#include <units/physical/si/current.h>
#include <units/physical/si/resistance.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::si {

struct ohm : named_unit<ohm, "R", prefix> {};
struct milli_ohm : prefixed_unit<milli_ohm, milli, ohm> {};
struct kilo_ohm : prefixed_unit<kilo_ohm, kilo, ohm> {};
struct mega_ohm : prefixed_unit<mega_ohm, mega, ohm> {};

struct dim_resistance : physical::dim_resistance<dim_resistance, ohm, dim_voltage, dim_electric_current> {};

template<Unit U, Scalar Rep = double>
using resistance = quantity<dim_resistance, U, Rep>;

inline namespace literals {

// V
constexpr auto operator""R(unsigned long long l) { return resistance<ohm, std::int64_t>(l); }
constexpr auto operator""R(long double l) { return resistance<ohm, long double>(l); }

constexpr auto operator""mR(unsigned long long l) { return resistance<milli_ohm, std::int64_t>(l); }
constexpr auto operator""mR(long double l) { return resistance<milli_ohm, long double>(l); }

constexpr auto operator""kR(unsigned long long l) { return resistance<kilo_ohm, std::int64_t>(l); }
constexpr auto operator""kR(long double l) { return resistance<kilo_ohm, long double>(l); }

constexpr auto operator""MR(unsigned long long l) { return resistance<mega_ohm, std::int64_t>(l); }
constexpr auto operator""MR(long double l) { return resistance<mega_ohm, long double>(l); }

}  // namespace literals

}  // namespace units::si
