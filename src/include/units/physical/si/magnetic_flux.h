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
#include <units/physical/si/magnetic_induction.h>
#include <units/physical/si/area.h>
#include <units/quantity.h>

namespace units::si {

struct weber : named_unit<weber, "Wb", prefix> {};

struct milliweber : prefixed_unit<milliweber, milli, weber> {};
struct microweber : prefixed_unit<microweber, micro, weber> {};
struct nanoweber : prefixed_unit<nanoweber, nano, weber> {};
struct picoweber : prefixed_unit<picoweber, pico, weber> {};

struct dim_magnetic_flux : physical::dim_magnetic_flux<dim_magnetic_flux, weber, dim_magnetic_induction, dim_area> {};

template<Unit U, Scalar Rep = double>
using magnetic_flux = quantity<dim_magnetic_flux, U, Rep>;

inline namespace literals {

// Wb
constexpr auto operator"" q_Wb(unsigned long long l) { return magnetic_flux<weber, std::int64_t>(l); }
constexpr auto operator"" q_Wb(long double l) { return magnetic_flux<weber, long double>(l); }

// mWb
constexpr auto operator"" q_mWb(unsigned long long l) { return magnetic_flux<milliweber, std::int64_t>(l); }
constexpr auto operator"" q_mWb(long double l) { return magnetic_flux<milliweber, long double>(l); }

// µWb
constexpr auto operator"" q_uWb(unsigned long long l) { return magnetic_flux<microweber, std::int64_t>(l); }
constexpr auto operator"" q_uWb(long double l) { return magnetic_flux<microweber, long double>(l); }

// nWb
constexpr auto operator"" q_nWb(unsigned long long l) { return magnetic_flux<nanoweber, std::int64_t>(l); }
constexpr auto operator"" q_nWb(long double l) { return magnetic_flux<nanoweber, long double>(l); }

// pWb
constexpr auto operator"" q_pWb(unsigned long long l) { return magnetic_flux<picoweber, std::int64_t>(l); }
constexpr auto operator"" q_pWb(long double l) { return magnetic_flux<picoweber, long double>(l); }

}  // namespace literals

}  // namespace units::si
