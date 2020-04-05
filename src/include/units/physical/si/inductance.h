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
#include <units/physical/si/magnetic_flux.h>
#include <units/physical/si/current.h>
#include <units/quantity.h>

namespace units::si {

struct henry : named_unit<henry, "H", prefix> {};

struct millihenry : prefixed_unit<millihenry, milli, henry> {};
struct microhenry : prefixed_unit<microhenry, micro, henry> {};
struct nanohenry : prefixed_unit<nanohenry, nano, henry> {};
struct picohenry : prefixed_unit<picohenry, pico, henry> {};

struct dim_inductance : physical::dim_inductance<dim_inductance, henry, dim_magnetic_flux, dim_electric_current> {};

template<Unit U, Scalar Rep = double>
using inductance = quantity<dim_inductance, U, Rep>;

inline namespace literals {

// H
constexpr auto operator"" q_H(unsigned long long l) { return inductance<henry, std::int64_t>(l); }
constexpr auto operator"" q_H(long double l) { return inductance<henry, long double>(l); }

// mH
constexpr auto operator"" q_mH(unsigned long long l) { return inductance<millihenry, std::int64_t>(l); }
constexpr auto operator"" q_mH(long double l) { return inductance<millihenry, long double>(l); }

// µH
constexpr auto operator"" q_uH(unsigned long long l) { return inductance<microhenry, std::int64_t>(l); }
constexpr auto operator"" q_uH(long double l) { return inductance<microhenry, long double>(l); }

// nH
constexpr auto operator"" q_nH(unsigned long long l) { return inductance<nanohenry, std::int64_t>(l); }
constexpr auto operator"" q_nH(long double l) { return inductance<nanohenry, long double>(l); }

// pH
constexpr auto operator"" q_pH(unsigned long long l) { return inductance<picohenry, std::int64_t>(l); }
constexpr auto operator"" q_pH(long double l) { return inductance<picohenry, long double>(l); }

}  // namespace literals

}  // namespace units::si
