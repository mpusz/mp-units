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
#include <units/physical/si/electric_charge.h>
#include <units/physical/si/voltage.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::si {

struct farad : named_unit<farad, "F", prefix> {};

struct millifarad : prefixed_unit<millifarad, milli, farad> {};
struct microfarad : prefixed_unit<microfarad, micro, farad> {};
struct nanofarad : prefixed_unit<nanofarad, nano, farad> {};
struct picofarad : prefixed_unit<picofarad, pico, farad> {};

struct dim_capacitance : physical::dim_capacitance<dim_capacitance, farad, dim_electric_charge, dim_voltage> {};

template<Unit U, Scalar Rep = double>
using capacitance = quantity<dim_capacitance, U, Rep>;

inline namespace literals {

// F
constexpr auto operator""F(unsigned long long l) { return capacitance<farad, std::int64_t>(l); }
constexpr auto operator""_F(long double l) { return capacitance<farad, long double>(l); }

constexpr auto operator""mF(unsigned long long l) { return capacitance<millifarad, std::int64_t>(l); }
constexpr auto operator""mF(long double l) { return capacitance<millifarad, long double>(l); }

constexpr auto operator""uF(unsigned long long l) { return capacitance<microfarad, std::int64_t>(l); }
constexpr auto operator""uF(long double l) { return capacitance<microfarad, long double>(l); }

constexpr auto operator""nF(unsigned long long l) { return capacitance<nanofarad, std::int64_t>(l); }
constexpr auto operator""nF(long double l) { return capacitance<nanofarad, long double>(l); }

constexpr auto operator""pF(unsigned long long l) { return capacitance<picofarad, std::int64_t>(l); }
constexpr auto operator""pF(long double l) { return capacitance<picofarad, long double>(l); }

}  // namespace literals

}  // namespace units::si
