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

#include "test_tools.h"
#include <mp-units/bits/external/type_traits.h>
#include <mp-units/chrono.h>
#include <mp-units/quantity_point.h>
#include <mp-units/systems/si/unit_symbols.h>
#include <ratio>

namespace {

using namespace mp_units;
using namespace mp_units::si::unit_symbols;
using namespace std::chrono_literals;
using sys_seconds = std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>;
using sys_days =
  std::chrono::time_point<std::chrono::system_clock,
                          std::chrono::duration<long, std::ratio_multiply<std::ratio<24>, std::chrono::hours::period>>>;
template<Unit auto U, typename C, typename Rep = double>
using time_point = quantity_point<U, chrono_point_origin<C>, Rep>;

static_assert(QuantityLike<std::chrono::seconds>);
static_assert(!Quantity<std::chrono::seconds>);
static_assert(!QuantityPoint<std::chrono::seconds>);
static_assert(!QuantityPointLike<std::chrono::seconds>);

static_assert(QuantityPointLike<sys_seconds>);
static_assert(!Quantity<sys_seconds>);
static_assert(!QuantityLike<sys_seconds>);
static_assert(!QuantityPoint<sys_seconds>);

// construction - same rep type
static_assert(
  std::constructible_from<quantity<isq::time[si::second], std::chrono::seconds::rep>, std::chrono::seconds>);
static_assert(std::convertible_to<std::chrono::seconds, quantity<isq::time[si::second], std::chrono::seconds::rep>>);
static_assert(std::constructible_from<quantity<isq::time[si::hour], std::chrono::hours::rep>, std::chrono::hours>);
static_assert(std::convertible_to<std::chrono::hours, quantity<isq::time[si::hour], std::chrono::hours::rep>>);
static_assert(std::constructible_from<quantity<isq::time[si::second], std::chrono::hours::rep>, std::chrono::hours>);
static_assert(std::convertible_to<std::chrono::hours, quantity<isq::time[si::second], std::chrono::hours::rep>>);
static_assert(!std::constructible_from<quantity<isq::time[si::hour], std::chrono::seconds::rep>, std::chrono::seconds>);
static_assert(!std::convertible_to<std::chrono::seconds, quantity<isq::time[si::hour], std::chrono::seconds::rep>>);
static_assert(
  std::constructible_from<time_point<si::second, std::chrono::system_clock, sys_seconds::rep>, sys_seconds>);
static_assert(
  !std::constructible_from<time_point<si::second, std::chrono::steady_clock, sys_seconds::rep>, sys_seconds>);
static_assert(std::convertible_to<sys_seconds, time_point<si::second, std::chrono::system_clock, sys_seconds::rep>>);
static_assert(std::constructible_from<time_point<si::day, std::chrono::system_clock, sys_days::rep>, sys_days>);
static_assert(!std::constructible_from<time_point<si::day, std::chrono::steady_clock, sys_days::rep>, sys_days>);
static_assert(std::convertible_to<sys_days, time_point<si::day, std::chrono::system_clock, sys_days::rep>>);
static_assert(std::constructible_from<time_point<si::second, std::chrono::system_clock, sys_days::rep>, sys_days>);
static_assert(!std::constructible_from<time_point<si::second, std::chrono::steady_clock, sys_days::rep>, sys_days>);
static_assert(std::convertible_to<sys_days, time_point<si::second, std::chrono::system_clock, sys_days::rep>>);
static_assert(!std::constructible_from<time_point<si::day, std::chrono::system_clock, sys_seconds::rep>, sys_seconds>);
static_assert(!std::convertible_to<sys_seconds, time_point<si::day, std::chrono::system_clock, sys_seconds::rep>>);

// construction - different rep type (integral to a floating-point)
static_assert(std::constructible_from<quantity<isq::time[si::second]>, std::chrono::seconds>);
static_assert(std::convertible_to<std::chrono::seconds, quantity<isq::time[si::second]>>);
static_assert(std::constructible_from<quantity<isq::time[si::second]>, std::chrono::hours>);
static_assert(std::convertible_to<std::chrono::hours, quantity<isq::time[si::second]>>);
static_assert(std::constructible_from<quantity<isq::time[si::hour]>, std::chrono::seconds>);
static_assert(std::convertible_to<std::chrono::seconds, quantity<isq::time[si::hour]>>);
static_assert(std::constructible_from<time_point<si::second, std::chrono::system_clock>, sys_seconds>);
static_assert(std::convertible_to<sys_seconds, time_point<si::second, std::chrono::system_clock>>);
static_assert(std::constructible_from<time_point<si::second, std::chrono::system_clock>, sys_days>);
static_assert(std::convertible_to<sys_days, time_point<si::second, std::chrono::system_clock>>);
static_assert(std::constructible_from<time_point<si::day, std::chrono::system_clock>, sys_seconds>);
static_assert(std::convertible_to<sys_seconds, time_point<si::day, std::chrono::system_clock>>);

static_assert(quantity<si::second>{1s} == 1 * s);
static_assert(quantity<isq::time[si::second]>{1s} == 1 * s);
static_assert(quantity<isq::period_duration[si::second]>{1s} == 1 * s);

// CTAD
static_assert(is_of_type<quantity{1s}, quantity<si::second, std::chrono::seconds::rep>>);
static_assert(is_of_type<quantity{1h}, quantity<si::hour, std::chrono::hours::rep>>);
static_assert(
  is_of_type<quantity_point{sys_seconds{1s}}, time_point<si::second, std::chrono::system_clock, sys_seconds::rep>>);
static_assert(is_of_type<quantity_point{sys_days{sys_days::duration{1}}},
                         time_point<si::day, std::chrono::system_clock, sys_days::rep>>);

// units mapping
static_assert(quantity{1ns} == 1 * ns);
static_assert(quantity{1us} == 1 * us);
static_assert(quantity{1ms} == 1 * ms);
static_assert(quantity{1s} == 1 * s);
static_assert(quantity{1min} == 1 * min);
static_assert(quantity{1h} == 1 * h);
static_assert(quantity{std::chrono::days{1}} == 1 * d);
static_assert(quantity{std::chrono::weeks{1}} == 7 * d);
static_assert(quantity{std::chrono::months{1}} == 2629746 * s);
static_assert(quantity{std::chrono::years{1}} == 31556952 * s);

// operators
static_assert(quantity{1s} + 1 * s == 2 * s);
static_assert(quantity{1s} + 1 * min == 61 * s);
static_assert(10 * m / quantity{2s} == 5 * m / s);
static_assert(quantity_point{sys_seconds{1s}} + 1 * s == chrono_point_origin<std::chrono::system_clock> + 2 * s);
static_assert(quantity_point{sys_seconds{1s}} + 1 * min == chrono_point_origin<std::chrono::system_clock> + 61 * s);

// to_chrono_duration
static_assert(to_chrono_duration(1 * s) == 1s);
static_assert(to_chrono_duration(2 * h) == 2h);
static_assert(to_chrono_duration(3 * ns) == 3ns);
static_assert(to_chrono_duration(quantity{1s}) == 1s);
static_assert(to_chrono_duration(quantity{2h}) == 2h);
static_assert(to_chrono_duration(quantity{3ns}) == 3ns);
static_assert(is_same_v<decltype(to_chrono_duration(1 * s))::period, std::ratio<1>>);
static_assert(is_same_v<decltype(to_chrono_duration(2 * h))::period, std::ratio<3600>>);
static_assert(is_same_v<decltype(to_chrono_duration(3 * ns))::period, std::nano>);

// to_chrono_time_point
static_assert(to_chrono_time_point(quantity_point{sys_seconds{1s}}) == sys_seconds{1s});
static_assert(to_chrono_time_point(quantity_point{sys_days{sys_days::duration{1}}}) == sys_days{sys_days::duration{1}});

}  // namespace
