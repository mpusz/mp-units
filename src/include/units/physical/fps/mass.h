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

struct grain : named_scaled_unit<grain, "gr", no_prefix, ratio<1, 7000>, pound>{};

struct dram : named_scaled_unit<dram, "dr", no_prefix, ratio<1, 256>, pound>{};

struct ounce : named_scaled_unit<ounce, "oz", no_prefix, ratio<1, 16>, pound>{};

struct stone : named_scaled_unit<stone, "st", no_prefix, ratio<14, 1>, pound>{};

struct quarter : named_scaled_unit<quarter, "qr", no_prefix, ratio<28, 1>, pound>{};

struct hundredweight : named_scaled_unit<hundredweight, "cwt", no_prefix, ratio<112, 1>, pound>{};

struct short_ton : named_scaled_unit<short_ton, "ton (short)", no_prefix, ratio<2'000, 1>, pound>{};

struct long_ton : named_scaled_unit<long_ton, "ton (long)", no_prefix, ratio<2'240, 1>, pound>{};

inline namespace literals {

// Grain
constexpr auto operator"" q_gr(unsigned long long l) { return mass<grain, std::int64_t>(l); }
constexpr auto operator"" q_gr(long double l) { return mass<grain, long double>(l); }

// Dram
constexpr auto operator"" q_dr(unsigned long long l) { return mass<dram, std::int64_t>(l); }
constexpr auto operator"" q_dr(long double l) { return mass<dram, long double>(l); }

// Ounce
constexpr auto operator"" q_oz(unsigned long long l) { return mass<ounce, std::int64_t>(l); }
constexpr auto operator"" q_oz(long double l) { return mass<ounce, long double>(l); }

// Pound
constexpr auto operator"" q_lb(unsigned long long l) { return mass<pound, std::int64_t>(l); }
constexpr auto operator"" q_lb(long double l) { return mass<pound, long double>(l); }

// Stone
constexpr auto operator"" q_st(unsigned long long l) { return mass<stone, std::int64_t>(l); }
constexpr auto operator"" q_st(long double l) { return mass<stone, long double>(l); }

// Quarter
constexpr auto operator"" q_qr(unsigned long long l) { return mass<quarter, std::int64_t>(l); }
constexpr auto operator"" q_qr(long double l) { return mass<quarter, long double>(l); }

// Hundredweight
constexpr auto operator"" q_cwt(unsigned long long l) { return mass<hundredweight, std::int64_t>(l); }
constexpr auto operator"" q_cwt(long double l) { return mass<hundredweight, long double>(l); }

// Short ton
constexpr auto operator"" q_ston(unsigned long long l) { return mass<short_ton, std::int64_t>(l); }
constexpr auto operator"" q_ston(long double l) { return mass<short_ton, long double>(l); }

// Long Ton
constexpr auto operator"" q_lton(unsigned long long l) { return mass<long_ton, std::int64_t>(l); }
constexpr auto operator"" q_lton(long double l) { return mass<long_ton, long double>(l); }


}

}  // namespace units::physical::fps
