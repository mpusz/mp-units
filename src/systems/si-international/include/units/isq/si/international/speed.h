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
#include <units/isq/dimensions/speed.h>
#include <units/quantity.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/international/length.h>
#include <units/isq/si/speed.h>
#include <units/unit.h>

namespace units::isq::si::international {

struct mile_per_hour : deduced_unit<mile_per_hour, si::dim_speed, si::international::mile, si::hour> {};

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// mi/h
constexpr auto operator"" _q_mi_per_h(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return si::speed<mile_per_hour, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_mi_per_h(long double l) { return si::speed<mile_per_hour, long double>(l); }

}  // namespace literals

#endif // UNITS_NO_LITERALS

}  // namespace units::isq::si::international

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::international::inline speed {

template<Representation Rep = double> using mi_per_h = units::isq::si::speed<units::isq::si::international::mile_per_hour, Rep>;

}  // namespace units::aliases::isq::si::international::inline speed

#endif // UNITS_NO_ALIASES
