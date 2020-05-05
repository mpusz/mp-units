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
#include <units/physical/si/resistance.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::si {

struct siemens : named_unit<siemens, "S", prefix> {};
struct millisiemens : prefixed_unit<millisiemens, milli, siemens> {};
struct microsiemens : prefixed_unit<microsiemens, micro, siemens> {};
struct nanosiemens : prefixed_unit<nanosiemens, nano, siemens> {};

struct dim_conductance : physical::dim_conductance<dim_conductance, siemens, dim_resistance> {};

template<Unit U, Scalar Rep = double>
using conductance = quantity<dim_conductance, U, Rep>;

inline namespace literals {

// R
constexpr auto operator"" q_S(unsigned long long l) { return conductance<siemens, std::int64_t>(l); }
constexpr auto operator"" q_S(long double l) { return conductance<siemens, long double>(l); }

// mS
constexpr auto operator"" q_mS(unsigned long long l) { return conductance<millisiemens, std::int64_t>(l); }
constexpr auto operator"" q_mS(long double l) { return conductance<millisiemens, long double>(l); }

// µS
constexpr auto operator"" q_uS(unsigned long long l) { return conductance<microsiemens, std::int64_t>(l); }
constexpr auto operator"" q_uS(long double l) { return conductance<microsiemens, long double>(l); }

// nS
constexpr auto operator"" q_nS(unsigned long long l) { return conductance<nanosiemens, std::int64_t>(l); }
constexpr auto operator"" q_nS(long double l) { return conductance<nanosiemens, long double>(l); }

}  // namespace literals

}  // namespace units::si

