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

// IWYU pragma: begin_exports
#include <units/isq/dimensions/area.h>
#include <units/isq/si/area.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/cgs/length.h>
#include <units/unit.h>

namespace units::isq::si::cgs {

using si::square_centimetre;

struct dim_area : isq::dim_area<dim_area, square_centimetre, dim_length> {};

template<UnitOf<dim_area> U, Representation Rep = double>
using area = quantity<dim_area, U, Rep>;

#ifdef UNITS_LITERALS

inline namespace literals {

// cm2
constexpr auto operator"" _q_cm2(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return area<square_centimetre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_cm2(long double l) { return area<square_centimetre, long double>(l); }

}  // namespace literals

#endif // UNITS_LITERALS

#ifdef UNITS_REFERENCES

namespace references {

inline constexpr auto cm2 = reference<dim_area, square_centimetre>{};

}  // namespace references

#endif // UNITS_REFERENCES

}  // namespace units::isq::si::cgs
