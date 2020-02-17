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
#include <units/physical/si/current.h>
#include <units/physical/si/power.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::si {

struct volt : named_unit<volt, "V", prefix> {};
struct millivolt : prefixed_unit<millivolt, milli, volt> {};
struct microvolt : prefixed_unit<microvolt, micro, volt> {};
struct nanovolt : prefixed_unit<nanovolt, nano, volt> {};
struct picovolt : prefixed_unit<picovolt, pico, volt> {};

struct dim_voltage : physical::dim_voltage<dim_voltage, volt, dim_power, dim_electric_current> {};

template<Unit U, Scalar Rep = double>
using voltage = quantity<dim_voltage, U, Rep>;

inline namespace literals {

// V
constexpr auto operator"" q_V(unsigned long long l) { return voltage<volt, std::int64_t>(l); }
constexpr auto operator"" q_V(long double l) { return voltage<volt, long double>(l); }

constexpr auto operator"" q_mV(unsigned long long l) { return voltage<millivolt, std::int64_t>(l); }
constexpr auto operator"" q_mV(long double l) { return voltage<millivolt, long double>(l); }

constexpr auto operator"" q_uV(unsigned long long l) { return voltage<microvolt, std::int64_t>(l); }
constexpr auto operator"" q_uV(long double l) { return voltage<microvolt, long double>(l); }

constexpr auto operator"" q_nV(unsigned long long l) { return voltage<nanovolt, std::int64_t>(l); }
constexpr auto operator"" q_nV(long double l) { return voltage<nanovolt, long double>(l); }

constexpr auto operator"" q_pV(unsigned long long l) { return voltage<picovolt, std::int64_t>(l); }
constexpr auto operator"" q_pV(long double l) { return voltage<picovolt, long double>(l); }

}  // namespace literals

}  // namespace units::si
