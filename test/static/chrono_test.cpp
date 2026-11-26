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
#include <mp-units/ext/type_traits.h>
#include <mp-units/framework/quantity_point.h>  // IWYU pragma: keep
#include <mp-units/systems/isq/si_quantities.h>
#include <mp-units/systems/si.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <chrono>
#include <concepts>
#include <ratio>
#include <type_traits>
#endif

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

static_assert(detail::QuantityLike<std::chrono::seconds>);
static_assert(!Quantity<std::chrono::seconds>);
static_assert(!QuantityPoint<std::chrono::seconds>);
static_assert(!detail::QuantityPointLike<std::chrono::seconds>);

static_assert(detail::QuantityPointLike<sys_seconds>);
static_assert(!Quantity<sys_seconds>);
static_assert(!detail::QuantityLike<sys_seconds>);
static_assert(!QuantityPoint<sys_seconds>);

// construction - same rep type
static_assert(
  std::constructible_from<quantity<isq::duration[si::second], std::chrono::seconds::rep>, std::chrono::seconds>);
static_assert(
  std::convertible_to<std::chrono::seconds, quantity<isq::duration[si::second], std::chrono::seconds::rep>>);
static_assert(std::constructible_from<quantity<isq::duration[si::hour], std::chrono::hours::rep>, std::chrono::hours>);
static_assert(std::convertible_to<std::chrono::hours, quantity<isq::duration[si::hour], std::chrono::hours::rep>>);
static_assert(
  std::constructible_from<quantity<isq::duration[si::second], std::chrono::hours::rep>, std::chrono::hours>);
static_assert(std::convertible_to<std::chrono::hours, quantity<isq::duration[si::second], std::chrono::hours::rep>>);
static_assert(
  !std::constructible_from<quantity<isq::duration[si::hour], std::chrono::seconds::rep>, std::chrono::seconds>);
static_assert(!std::convertible_to<std::chrono::seconds, quantity<isq::duration[si::hour], std::chrono::seconds::rep>>);
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
static_assert(std::constructible_from<quantity<isq::duration[si::second]>, std::chrono::seconds>);
static_assert(std::convertible_to<std::chrono::seconds, quantity<isq::duration[si::second]>>);
static_assert(std::constructible_from<quantity<isq::duration[si::second]>, std::chrono::hours>);
static_assert(std::convertible_to<std::chrono::hours, quantity<isq::duration[si::second]>>);
static_assert(std::constructible_from<quantity<isq::duration[si::hour]>, std::chrono::seconds>);
static_assert(std::convertible_to<std::chrono::seconds, quantity<isq::duration[si::hour]>>);
static_assert(std::constructible_from<time_point<si::second, std::chrono::system_clock>, sys_seconds>);
static_assert(std::convertible_to<sys_seconds, time_point<si::second, std::chrono::system_clock>>);
static_assert(std::constructible_from<time_point<si::second, std::chrono::system_clock>, sys_days>);
static_assert(std::convertible_to<sys_days, time_point<si::second, std::chrono::system_clock>>);
static_assert(std::constructible_from<time_point<si::day, std::chrono::system_clock>, sys_seconds>);
static_assert(std::convertible_to<sys_seconds, time_point<si::day, std::chrono::system_clock>>);

static_assert(quantity<si::second>{1s} == 1 * s);
static_assert(quantity<isq::duration[si::second]>{1s} == 1 * s);
static_assert(quantity<isq::period_duration[si::second]>{1s} == 1 * s);

// `std::chrono::duration` is quantity-like, so it compares against `zero` as a quantity does
static_assert(0s == zero);
static_assert(1s > zero);
static_assert(-1h < zero);
static_assert(zero != 1min);
template<typename T>
constexpr bool is_positive(const T& value)
{
  return value > zero;
}
static_assert(is_positive(1s) && is_positive(1 * s));
static_assert(!is_positive(-1s) && !is_positive(0 * s));

// a `std::chrono::time_point` is a point, so it can't be compared against `zero`
// `zero` converts to a `std::chrono::duration`, so the two have a common reference, and the
// `std::ranges` algorithms accept it
constexpr std::chrono::seconds no_time = zero;
static_assert(no_time == 0s);
static_assert(std::chrono::milliseconds{zero} == 0ms);
static_assert(std::equality_comparable_with<std::chrono::seconds, zero_t>);
static_assert(std::totally_ordered_with<std::chrono::hours, zero_t>);
static_assert(!std::convertible_to<zero_t, sys_seconds>);

template<typename T>
concept zero_comparable = requires(T val) { val == zero; };
static_assert(zero_comparable<std::chrono::seconds>);
static_assert(!zero_comparable<sys_seconds>);

// CTAD
static_assert(is_of_type<quantity{1s}, quantity<si::second, std::chrono::seconds::rep>>);
static_assert(is_of_type<quantity{1h}, quantity<si::hour, std::chrono::hours::rep>>);
static_assert(
  is_of_type<quantity_point{sys_seconds{1s}}, time_point<si::second, std::chrono::system_clock, sys_seconds::rep>>);
static_assert(is_of_type<quantity_point{sys_days{sys_days::duration{1}}},
                         time_point<si::day, std::chrono::system_clock, sys_days::rep>>);

// conversion to chrono
static_assert(
  std::constructible_from<std::chrono::seconds, quantity<isq::duration[si::second], std::chrono::seconds::rep>>);
static_assert(
  std::convertible_to<quantity<isq::duration[si::second], std::chrono::seconds::rep>, std::chrono::seconds>);
static_assert(std::constructible_from<std::chrono::hours, quantity<isq::duration[si::hour], std::chrono::hours::rep>>);
static_assert(std::convertible_to<quantity<isq::duration[si::hour], std::chrono::hours::rep>, std::chrono::hours>);
static_assert(
  std::constructible_from<sys_seconds, time_point<si::second, std::chrono::system_clock, sys_seconds::rep>>);
static_assert(std::convertible_to<time_point<si::second, std::chrono::system_clock, sys_seconds::rep>, sys_seconds>);

// units mapping
static_assert(quantity{1ns} == 1 * ns);
static_assert(quantity{1us} == 1 * us);
static_assert(quantity{1ms} == 1 * ms);
static_assert(quantity{1s} == 1 * s);
static_assert(quantity{1min} == 1 * min);
static_assert(quantity{1h} == 1 * h);
static_assert(quantity{std::chrono::days{1}} == 1 * d);
static_assert(quantity{std::chrono::weeks{1}} == 7 * d);
static_assert(quantity{std::chrono::months{1}} == 2'629'746 * s);
static_assert(quantity{std::chrono::years{1}} == 31'556'952 * s);

// conversion from chrono
static_assert(quantity<ns>{1ns} == 1 * ns);
static_assert(quantity<ns>{1s} == 1 * s);
static_assert(quantity<s>{1ms} == 1 * ms);

// conversion to chrono
static_assert(std::chrono::nanoseconds(quantity{1 * ns}) == 1ns);
static_assert(std::chrono::nanoseconds(quantity{1 * s}) == 1s);

// operators
static_assert([q = 1 * s]() mutable { return q += quantity{1s}; }() == 2 * s);
static_assert([q = 2 * s]() mutable { return q -= quantity{1s}; }() == 1 * s);
static_assert(quantity{1s} + 1 * s == 2 * s);
static_assert(quantity{1s} + 1 * min == 61 * s);
static_assert(1 * s + quantity{1s} == 2 * s);
static_assert(1 * min + quantity{1s} == 61 * s);
static_assert(10 * m / quantity{2s} == 5 * m / s);
static_assert(quantity_point{sys_seconds{1s}} + 1 * s == chrono_point_origin<std::chrono::system_clock> + 2 * s);
static_assert(quantity_point{sys_seconds{1s}} + 1 * min == chrono_point_origin<std::chrono::system_clock> + 61 * s);

template<typename... Ts>
consteval bool invalid_arithmetic(Ts... ts)
{
  return !requires { (... + ts); } && !requires { (... - ts); };
}
static_assert(invalid_arithmetic(1s, 1 * s));
static_assert(invalid_arithmetic(1 * s, 1s));

// to_chrono_duration
static_assert(to_chrono_duration(1 * s) == 1s);
static_assert(to_chrono_duration(2 * h) == 2h);
static_assert(to_chrono_duration(3 * ns) == 3ns);
static_assert(to_chrono_duration(quantity{1s}) == 1s);
static_assert(to_chrono_duration(quantity{2h}) == 2h);
static_assert(to_chrono_duration(quantity{3ns}) == 3ns);
static_assert(std::is_same_v<decltype(to_chrono_duration(1 * s))::period, std::ratio<1>>);
static_assert(std::is_same_v<decltype(to_chrono_duration(2 * h))::period, std::ratio<3600>>);
static_assert(std::is_same_v<decltype(to_chrono_duration(3 * ns))::period, std::nano>);

// to_chrono_time_point
static_assert(to_chrono_time_point(quantity_point{sys_seconds{1s}}) == sys_seconds{1s});
static_assert(to_chrono_time_point(quantity_point{sys_days{sys_days::duration{1}}}) == sys_days{sys_days::duration{1}});

}  // namespace

// A scalar type customized for `std::chrono` keeps the zero of its `duration_values`
// specialization in `representation_values`, even where that zero isn't `Rep(0)`.
namespace {

struct biased_ticks {
  long count = 0;
  constexpr biased_ticks() = default;
  constexpr explicit biased_ticks(long ticks) : count(ticks) {}
  friend constexpr bool operator==(biased_ticks, biased_ticks) = default;
};

}  // namespace

template<>
struct std::chrono::duration_values<biased_ticks> {
  static constexpr biased_ticks zero() noexcept { return biased_ticks{100}; }
  static constexpr biased_ticks min() noexcept { return biased_ticks{-1000}; }
  static constexpr biased_ticks max() noexcept { return biased_ticks{1000}; }
};

static_assert(mp_units::representation_values<biased_ticks>::zero() == biased_ticks{100});

// ... and so does a scalar type that can't be constructed from an `int`
namespace {

struct tagged_ticks {
  struct from_count_t {};
  long count = 0;
  constexpr tagged_ticks() = default;
  constexpr tagged_ticks(from_count_t, long ticks) : count(ticks) {}
  friend constexpr bool operator==(tagged_ticks, tagged_ticks) = default;
};

}  // namespace

template<>
struct std::chrono::duration_values<tagged_ticks> {
  static constexpr tagged_ticks zero() noexcept { return {}; }
};

static_assert(!std::constructible_from<tagged_ticks, int>);
static_assert(mp_units::representation_values<tagged_ticks>::zero() == tagged_ticks{});
static_assert(mp_units::representation_values<biased_ticks>::min() == biased_ticks{-1000});
