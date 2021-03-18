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

#include <units/isq/dimensions/torque.h>
#include <units/isq/si/energy.h>
#include <units/generic/angle.h>
#include <units/isq/si/prefixes.h>
#include <units/quantity.h>

namespace units::isq::si {

struct newton_metre_per_radian : unit<newton_metre_per_radian> {};

struct dim_torque : isq::dim_torque<dim_torque, newton_metre_per_radian, dim_force, dim_length, dim_angle<>> {};

template<UnitOf<dim_torque> U, QuantityValue Rep = double>
using torque = quantity<dim_torque, U, Rep>;

inline namespace literals {

// Nm
constexpr auto operator"" _q_Nm_per_rad(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return torque<newton_metre_per_radian, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Nm_per_rad(long double l) { return torque<newton_metre_per_radian, long double>(l); }

}  // namespace literals

namespace references {

inline constexpr auto Nm_per_rad = reference<dim_torque, newton_metre_per_radian>{};

}  // namespace references

}  // namespace units::isq::si
