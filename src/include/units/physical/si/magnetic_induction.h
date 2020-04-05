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
#include <units/physical/si/voltage.h>
#include <units/physical/si/time.h>
#include <units/physical/si/length.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::si {

struct tesla : named_unit<tesla, "T", prefix> {};

struct millitesla : prefixed_unit<millitesla, milli, tesla> {};
struct microtesla : prefixed_unit<microtesla, micro, tesla> {};
struct nanotesla : prefixed_unit<nanotesla, nano, tesla> {};
struct picotesla : prefixed_unit<picotesla, pico, tesla> {};

struct gauss : named_scaled_unit<gauss, "G", prefix, ratio<1, 10'000>, tesla> {};

struct dim_magnetic_induction : physical::dim_magnetic_induction<dim_magnetic_induction, tesla, dim_voltage, dim_time, dim_length> {};

template<Unit U, Scalar Rep = double>
using magnetic_induction = quantity<dim_magnetic_induction, U, Rep>;

inline namespace literals {

// T
constexpr auto operator"" q_T(unsigned long long l) { return magnetic_induction<tesla, std::int64_t>(l); }
constexpr auto operator"" q_T(long double l) { return magnetic_induction<tesla, long double>(l); }

// mT
constexpr auto operator"" q_mT(unsigned long long l) { return magnetic_induction<millitesla, std::int64_t>(l); }
constexpr auto operator"" q_mT(long double l) { return magnetic_induction<millitesla, long double>(l); }

// µT
constexpr auto operator"" q_uT(unsigned long long l) { return magnetic_induction<microtesla, std::int64_t>(l); }
constexpr auto operator"" q_uT(long double l) { return magnetic_induction<microtesla, long double>(l); }

// nT
constexpr auto operator"" q_nT(unsigned long long l) { return magnetic_induction<nanotesla, std::int64_t>(l); }
constexpr auto operator"" q_nT(long double l) { return magnetic_induction<nanotesla, long double>(l); }

//pT
constexpr auto operator"" q_pT(unsigned long long l) { return magnetic_induction<picotesla, std::int64_t>(l); }
constexpr auto operator"" q_pT(long double l) { return magnetic_induction<picotesla, long double>(l); }

// G
constexpr auto operator"" q_G(unsigned long long l) { return magnetic_induction<gauss, std::int64_t>(l); }
constexpr auto operator"" q_G(long double l) { return magnetic_induction<gauss, long double>(l); }

}  // namespace literals

} // namespace units
