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
#include <units/isq/dimensions/pressure.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/cgs/area.h>
#include <units/isq/si/cgs/force.h>
#include <units/isq/si/prefixes.h>
#include <units/unit.h>

namespace units::isq::si::cgs {

struct barye : named_unit<barye, "Ba", si::prefix> {};

struct dim_pressure : isq::dim_pressure<dim_pressure, barye, dim_force, dim_area> {};

template<UnitOf<dim_pressure> U, Representation Rep = double>
using pressure = quantity<dim_pressure, U, Rep>;

#ifdef UNITS_LITERALS

inline namespace literals {

// Ba
constexpr auto operator"" _q_Ba(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return pressure<barye, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Ba(long double l) { return pressure<barye, long double>(l); }

}  // namespace literals

#endif // UNITS_LITERALS

namespace references {

inline constexpr auto Ba = reference<dim_pressure, barye>{};

}  // namespace references

}  // namespace units::isq::si::cgs
