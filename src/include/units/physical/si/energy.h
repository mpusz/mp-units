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
#include <units/physical/si/force.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::si {

struct joule : named_unit<joule, "J", prefix> {};
struct millijoule : prefixed_unit<millijoule, milli, joule> {};
struct kilojoule : prefixed_unit<kilojoule, kilo, joule> {};
struct megajoule : prefixed_unit<megajoule, mega, joule> {};
struct gigajoule : prefixed_unit<gigajoule, giga, joule> {};

struct electronvolt : named_scaled_unit<electronvolt, "eV", prefix, ratio<1'602'176'634, 1'000'000'000, -19>, joule> {};
struct gigaelectronvolt : prefixed_unit<gigaelectronvolt, giga, electronvolt> {};

struct dim_energy : physical::dim_energy<dim_energy, joule, dim_force, dim_length> {};

template<Unit U, Scalar Rep = double>
using energy = quantity<dim_energy, U, Rep>;

inline namespace literals {

// J
constexpr auto operator"" q_J(unsigned long long l) { return energy<joule, std::int64_t>(l); }
constexpr auto operator"" q_J(long double l) { return energy<joule, long double>(l); }

// mJ
constexpr auto operator"" q_mJ(unsigned long long l) { return energy<millijoule, std::int64_t>(l); }
constexpr auto operator"" q_mJ(long double l) { return energy<millijoule, long double>(l); }

// kJ
constexpr auto operator"" q_kJ(unsigned long long l) { return energy<kilojoule, std::int64_t>(l); }
constexpr auto operator"" q_kJ(long double l) { return energy<kilojoule, long double>(l); }

// MJ
constexpr auto operator"" q_MJ(unsigned long long l) { return energy<megajoule, std::int64_t>(l); }
constexpr auto operator"" q_MJ(long double l) { return energy<megajoule, long double>(l); }

// GJ
constexpr auto operator"" q_GJ(unsigned long long l) { return energy<gigajoule, std::int64_t>(l); }
constexpr auto operator"" q_GJ(long double l) { return energy<gigajoule, long double>(l); }

// eV
constexpr auto operator"" q_eV(unsigned long long l) { return energy<electronvolt, std::int64_t>(l); }
constexpr auto operator"" q_eV(long double l) { return energy<electronvolt, long double>(l); }

// GeV
constexpr auto operator"" q_GeV(unsigned long long l) { return energy<gigaelectronvolt, std::int64_t>(l); }
constexpr auto operator"" q_GeV(long double l) { return energy<gigaelectronvolt, long double>(l); }

}  // namespace literals

}  // namespace units::si
