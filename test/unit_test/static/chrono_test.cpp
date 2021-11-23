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

#include <units/bits/external/type_traits.h>
#include <units/chrono.h>
#include <units/isq/si/length.h>
#include <units/isq/si/speed.h>
#include <units/quantity_point.h>
#include <ratio>

namespace {

using namespace units;
using namespace units::isq;
using namespace units::isq::si::literals;
using namespace std::chrono_literals;
using sys_seconds = std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>;
using sys_days = std::chrono::time_point<std::chrono::system_clock,
  std::chrono::duration<long, std::ratio_multiply<std::ratio<24>, std::chrono::hours::period>>>;
template<typename C, typename U, typename Rep = double> using time_point = quantity_point<clock_origin<C>, U, Rep>;

static_assert(QuantityLike<std::chrono::seconds>);
static_assert(QuantityPointLike<sys_seconds>);

// construction - same rep type
static_assert(std::constructible_from<si::time<si::second, std::chrono::seconds::rep>, std::chrono::seconds>);
static_assert(!std::convertible_to<std::chrono::seconds, si::time<si::second, std::chrono::seconds::rep>>);
static_assert(std::constructible_from<si::time<si::hour, std::chrono::hours::rep>, std::chrono::hours>);
static_assert(!std::convertible_to<std::chrono::hours, si::time<si::hour, std::chrono::hours::rep>>);
static_assert(std::constructible_from<si::time<si::second, std::chrono::hours::rep>, std::chrono::hours>);
static_assert(!std::convertible_to<std::chrono::hours, si::time<si::second, std::chrono::hours::rep>>);
static_assert(!std::constructible_from<si::time<si::hour, std::chrono::seconds::rep>, std::chrono::seconds>);
static_assert(!std::convertible_to<std::chrono::seconds, si::time<si::hour, std::chrono::seconds::rep>>);
static_assert(std::constructible_from<time_point<std::chrono::system_clock, si::second, sys_seconds::rep>, sys_seconds>);
static_assert(!std::constructible_from<time_point<std::chrono::steady_clock, si::second, sys_seconds::rep>, sys_seconds>);
static_assert(!std::convertible_to<sys_seconds, time_point<std::chrono::system_clock, si::second, sys_seconds::rep>>);
static_assert(std::constructible_from<time_point<std::chrono::system_clock, si::day, sys_days::rep>, sys_days>);
static_assert(!std::constructible_from<time_point<std::chrono::steady_clock, si::day, sys_days::rep>, sys_days>);
static_assert(!std::convertible_to<sys_days, time_point<std::chrono::system_clock, si::day, sys_days::rep>>);
static_assert(std::constructible_from<time_point<std::chrono::system_clock, si::second, sys_days::rep>, sys_days>);
static_assert(!std::constructible_from<time_point<std::chrono::steady_clock, si::second, sys_days::rep>, sys_days>);
static_assert(!std::convertible_to<sys_days, time_point<std::chrono::system_clock, si::second, sys_days::rep>>);
static_assert(!std::constructible_from<time_point<std::chrono::system_clock, si::day, sys_seconds::rep>, sys_seconds>);
static_assert(!std::convertible_to<sys_seconds, time_point<std::chrono::system_clock, si::day, sys_seconds::rep>>);

// construction - different rep type (integral to a floating-point)
static_assert(std::constructible_from<si::time<si::second>, std::chrono::seconds>);
static_assert(!std::convertible_to<std::chrono::seconds, si::time<si::second>>);
static_assert(std::constructible_from<si::time<si::second>, std::chrono::hours>);
static_assert(!std::convertible_to<std::chrono::hours, si::time<si::second>>);
static_assert(std::constructible_from<si::time<si::hour>, std::chrono::seconds>);
static_assert(!std::convertible_to<std::chrono::seconds, si::time<si::hour>>);
static_assert(std::constructible_from<time_point<std::chrono::system_clock, si::second>, sys_seconds>);
static_assert(!std::convertible_to<sys_seconds, time_point<std::chrono::system_clock, si::second>>);
static_assert(std::constructible_from<time_point<std::chrono::system_clock, si::second>, sys_days>);
static_assert(!std::convertible_to<sys_days, time_point<std::chrono::system_clock, si::second>>);
static_assert(std::constructible_from<time_point<std::chrono::system_clock, si::day>, sys_seconds>);
static_assert(!std::convertible_to<sys_seconds, time_point<std::chrono::system_clock, si::day>>);


// CTAD
static_assert(is_same_v<decltype(quantity{1s}), si::time<si::second, std::chrono::seconds::rep>>);
static_assert(is_same_v<decltype(quantity{1h}), si::time<si::hour, std::chrono::hours::rep>>);
static_assert(is_same_v<decltype(quantity_point{sys_seconds{1s}}), time_point<std::chrono::system_clock, si::second, sys_seconds::rep>>);
static_assert(is_same_v<decltype(quantity_point{sys_days{sys_days::duration{1}}}), time_point<std::chrono::system_clock, si::day, sys_days::rep>>);

// operators
static_assert(quantity{1s} + 1_q_s == 2_q_s);
static_assert(quantity{1s} + 1_q_min == 61_q_s);
static_assert(10_q_m / quantity{2s} == 5_q_m_per_s);
static_assert(quantity_point{sys_seconds{1s}} + 1_q_s == time_point<std::chrono::system_clock, si::second>{2_q_s});
static_assert(quantity_point{sys_seconds{1s}} + 1_q_min == time_point<std::chrono::system_clock, si::second>{61_q_s});

// to_duration
static_assert(to_std_duration(1_q_s) == 1s);
static_assert(to_std_duration(2_q_h) == 2h);
static_assert(to_std_duration(3_q_ns) == 3ns);
static_assert(is_same_v<decltype(to_std_duration(1_q_s))::period, std::ratio<1>>);
static_assert(is_same_v<decltype(to_std_duration(2_q_h))::period, std::ratio<3600>>);
static_assert(is_same_v<decltype(to_std_duration(3_q_ns))::period, std::nano>);

}  // namespace
