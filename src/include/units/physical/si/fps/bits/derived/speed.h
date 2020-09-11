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

#ifndef MP_UNITS_SYSTEM_SI_FPS

#error "Please do not include this file directly. Use `units/physical/si/fps/fps.h` to prevent potential ODR violation issues."

#endif

#include <units/physical/dimensions.h>
#include <units/physical/si/fps/base/length.h>
#include <units/physical/si/fps/base/time.h>
#include <units/quantity.h>

namespace units::physical::si::fps {

struct foot_per_second : unit<foot_per_second> {};
struct dim_speed : physical::dim_speed<dim_speed, foot_per_second, dim_length, dim_time> {};

template<Unit U, ScalableNumber Rep = double>
using speed = quantity<dim_speed, U, Rep>;

struct mile_per_hour : deduced_unit<mile_per_hour, dim_speed, mile, hour>{};

struct nautical_mile_per_hour : named_deduced_unit<nautical_mile_per_hour, dim_speed, "knot", no_prefix, nautical_mile, hour>{};

struct knot : alias_unit<nautical_mile_per_hour, "knot", no_prefix> {};


inline namespace literals {

constexpr auto operator"" _q_ft_per_s(unsigned long long l) { return speed<foot_per_second, std::int64_t>(l); }
constexpr auto operator"" _q_ft_per_s(long double l) { return speed<foot_per_second, long double>(l); }

constexpr auto operator"" _q_mph(unsigned long long l) { return speed<mile_per_hour, std::int64_t>(l); }
constexpr auto operator"" _q_mph(long double l) { return speed<mile_per_hour, long double>(l); }

constexpr auto operator"" _q_knot(unsigned long long l) { return speed<knot, std::int64_t>(l); }
constexpr auto operator"" _q_knot(long double l) { return speed<knot, long double>(l); }


}  // namespace literals

}  // namespace units::physical::si::fps
