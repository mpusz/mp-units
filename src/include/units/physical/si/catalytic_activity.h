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
#include <units/physical/si/substance.h>
#include <units/physical/si/time.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::si {

struct katal : named_unit<katal, "kat", prefix> {};
struct nanokatal : prefixed_unit<nanokatal, nano, katal> {};

struct enzyme_unit : named_scaled_unit<enzyme_unit, "U", prefix, ratio<1, 60, -6>, katal> {};

struct dim_catalytic_activity : physical::dim_catalytic_activity<dim_catalytic_activity, katal, dim_time, dim_substance> {};

template<Unit U, Scalar Rep = double>
using catalytic_activity = quantity<dim_catalytic_activity, U, Rep>;

inline namespace literals {

// kat
constexpr auto operator"" q_kat(unsigned long long l) { return catalytic_activity<katal, std::int64_t>(l); }
constexpr auto operator"" q_kat(long double l) { return catalytic_activity<katal, long double>(l); }

// nkat
constexpr auto operator"" q_nkat(unsigned long long l) { return catalytic_activity<nanokatal, std::int64_t>(l); }
constexpr auto operator"" q_nkat(long double l) { return catalytic_activity<nanokatal, long double>(l); }

// U
constexpr auto operator"" q_U(unsigned long long l) { return catalytic_activity<enzyme_unit, std::int64_t>(l); }
constexpr auto operator"" q_U(long double l) { return catalytic_activity<enzyme_unit, long double>(l); }

}  // namespace literals

}  // namespace units::si

