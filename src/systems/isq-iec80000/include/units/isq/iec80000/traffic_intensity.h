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

#include <units/base_dimension.h>
#include <units/unit.h>

// IWYU pragma: begin_exports
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

namespace units::isq::iec80000 {

struct erlang : named_unit<erlang, "E", no_prefix> {};

struct dim_traffic_intensity : base_dimension<"A", erlang> {};

template<typename T>
concept TrafficIntensity = QuantityOf<T, dim_traffic_intensity>;

template<UnitOf<dim_traffic_intensity> U, Representation Rep = double>
using traffic_intensity = quantity<dim_traffic_intensity, U, Rep>;

#if UNITS_UDLS

inline namespace literals {

constexpr auto operator"" _q_E(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return traffic_intensity<erlang, std::int64_t>(static_cast<std::int64_t>(l)); }

}  // namespace literals

#endif // UNITS_UDLS

namespace references {

inline constexpr auto E = reference<dim_traffic_intensity, erlang>{};

}  // namespace references

}  // namespace units::isq::iec80000
