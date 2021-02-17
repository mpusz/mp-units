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

#include <units/physical/si/base/length.h>
#include <units/quantity.h>

namespace units::physical::si::cgs {

using si::centimetre;

struct dim_length : physical::dim_length<centimetre> {};

template<UnitOf<dim_length> U, QuantityValue Rep = double>
using length = quantity<dim_length, U, Rep>;

inline namespace literals {

// cm
consteval auto operator"" _q_cm(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return length<centimetre, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_cm(long double l) { return length<centimetre, long double>(l); }

}  // namespace literals

namespace unit_constants {

inline constexpr auto cm = length<centimetre, one_rep>{};

}  // namespace unit_constants

}  // namespace units::physical::si::cgs
