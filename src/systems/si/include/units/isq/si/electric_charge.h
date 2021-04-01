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
#include <units/isq/dimensions/electric_charge.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/electric_current.h>
#include <units/isq/si/time.h>
#include <units/unit.h>

namespace units::isq::si {

struct coulomb : named_unit<coulomb, "C", prefix> {};

struct dim_electric_charge : isq::dim_electric_charge<dim_electric_charge, coulomb, dim_time, dim_electric_current> {};

template<UnitOf<dim_electric_charge> U, Representation Rep = double>
using electric_charge = quantity<dim_electric_charge, U, Rep>;

#if UNITS_UDLS

inline namespace literals {

// C
constexpr auto operator"" _q_C(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return electric_charge<coulomb, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_C(long double l) { return electric_charge<coulomb, long double>(l); }

}  // namespace literals

#endif // UNITS_UDLS

namespace references {

inline constexpr auto C = reference<dim_electric_charge, coulomb>{};

}  // namespace references

}  // namespace units::isq::si
