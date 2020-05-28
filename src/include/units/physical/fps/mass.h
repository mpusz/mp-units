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
#include <units/physical/si/mass.h>
#include <units/quantity.h>

namespace units::physical::fps {

// https://en.wikipedia.org/wiki/Pound_(mass)
struct pound : named_scaled_unit<pound, "lb", no_prefix, ratio<45'359'237, 100'000'000>, si::kilogram> {};

struct dim_mass : physical::dim_mass<pound> {};

template<Unit U, Scalar Rep = double>
using mass = quantity<dim_mass, U, Rep>;

inline namespace literals {

// g
constexpr auto operator"" q_lb(unsigned long long l) { return mass<pound, std::int64_t>(l); }
constexpr auto operator"" q_lb(long double l) { return mass<pound, long double>(l); }

}

}  // namespace units::physical::fps
