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

#include <units/physical/dimensions/power.h>
#include <units/physical/si/fps/derived/energy.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::physical::si::fps {

struct foot_poundal_per_second : unit<foot_poundal_per_second> {};

struct dim_power : physical::dim_power<dim_power, foot_poundal_per_second, dim_energy, dim_time> {};

struct foot_pound_force_per_second : deduced_unit<foot_pound_force_per_second, dim_power, foot_pound_force, second> {};

struct horse_power : named_scaled_unit<horse_power, "hp", no_prefix, ratio(550), foot_pound_force_per_second> {};

template<UnitOf<dim_power> U, QuantityValue Rep = double>
using power = quantity<dim_power, U, Rep>;

inline namespace literals {

// foot pound force per second
constexpr auto operator"" _q_ft_pdl_per_s(unsigned long long l) { return power<foot_poundal_per_second, std::int64_t>(l); }
constexpr auto operator"" _q_ft_pdl_per_s(long double l) { return power<foot_poundal_per_second, long double>(l); }

// foot pound force per second
constexpr auto operator"" _q_ft_lbf_per_s(unsigned long long l) { return power<foot_pound_force_per_second, std::int64_t>(l); }
constexpr auto operator"" _q_ft_lbf_per_s(long double l) { return power<foot_pound_force_per_second, long double>(l); }

// horse power
constexpr auto operator"" _q_hp(unsigned long long l) { return power<horse_power, std::int64_t>(l); }
constexpr auto operator"" _q_hp(long double l) { return power<horse_power, long double>(l); }

}  // namespace literals

namespace unit_constants {

inline constexpr auto hp = power<horse_power, detail::one_rep>{};

}  // namespace unit_constants

}  // namespace units::physical::si::fps
