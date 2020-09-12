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

#include <units/physical/dimensions/luminance.h>
#include <units/physical/si/base/length.h>
#include <units/physical/si/base/luminous_intensity.h>
#include <units/quantity.h>

namespace units::physical::si {

struct candela_per_metre_sq : unit<candela_per_metre_sq> {};
struct dim_luminance : physical::dim_luminance<dim_luminance, candela_per_metre_sq, dim_luminous_intensity, dim_length> {};

template<Unit U, ScalableNumber Rep = double>
using luminance = quantity<dim_luminance, U, Rep>;

inline namespace literals {

// cd/m²
constexpr auto operator"" _q_cd_per_m2(unsigned long long l) { return luminance<candela_per_metre_sq, std::int64_t>(l); }
constexpr auto operator"" _q_cd_per_m2(long double l) { return luminance<candela_per_metre_sq, long double>(l); }

}  // namespace literals

}  // namespace units::physical::si

