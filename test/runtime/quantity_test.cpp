// The MIT License (MIT)
//
// Copyright (c) 2024 Chip Hogg
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

#include <catch2/catch_test_macros.hpp>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <algorithm>
#include <atomic>
#include <chrono>
#include <functional>
#include <limits>
#include <numbers>
#include <vector>
#endif
#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/math.h>
#include <mp-units/systems/si.h>
#endif

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

namespace {

template<typename T>
constexpr bool within_4_ulps(T a, T b)
{
  static_assert(std::is_floating_point_v<T>);
  auto walk_ulps = [](T x, int n) {
    while (n > 0) {
      x = std::nextafter(x, std::numeric_limits<T>::infinity());
      --n;
    }
    while (n < 0) {
      x = std::nextafter(x, -std::numeric_limits<T>::infinity());
      ++n;
    }
    return x;
  };

  return (walk_ulps(a, -4) <= b) && (b <= walk_ulps(a, 4));
}

}  // namespace

TEST_CASE("quantity operations", "[quantity]")
{
  // conversion requiring radical magnitudes
  SECTION("unit conversions support radical magnitudes")
  {
    REQUIRE(within_4_ulps(sqrt((1.0 * m) * (1.0 * km)).numerical_value_in(m), std::sqrt(1000.0)));
  }

  // Reproducing issue #474 exactly:
  SECTION("Issue 474 is fixed")
  {
    constexpr auto val_issue_474 =
      8.0 * si::si2019::boltzmann_constant * 1000.0 * K / (std::numbers::pi * 10 * non_si::unit_symbols::Da);
    REQUIRE(within_4_ulps(sqrt(val_issue_474).numerical_value_in(m / s),
                          std::sqrt(val_issue_474.numerical_value_in(m * m / s / s))));
  }

  SECTION("Volatile representation type")
  {
    volatile std::int16_t vint = 123;
    REQUIRE(quantity(vint * m).numerical_value_in(m) == 123);
  }
}

TEST_CASE("zero survives being passed to a function", "[quantity][zero]")
{
  // a literal `0` stops being a compile-time zero inside the algorithm, while `zero` carries its
  // value in its type, so these work outside constant evaluation
  std::vector<quantity<si::metre, int>> v = {3 * m, 0 * m, -2 * m, 0 * m};
  quantity<si::metre, int> q = v.back();

  CHECK(std::find(v.begin(), v.end(), zero) - v.begin() == 1);
  CHECK(std::count(v.begin(), v.end(), zero) == 2);
  CHECK(std::ranges::find(v, zero) - v.begin() == 1);
  CHECK(std::ranges::count(v, zero) == 2);
  CHECK(std::count_if(v.begin(), v.end(), [](const auto& len) { return len < zero; }) == 1);
  CHECK_FALSE(std::less<>{}(q, zero));
  CHECK(std::ranges::less{}(v[2], zero));
  CHECK(std::ranges::equal_to{}(q, zero));

  q = 5 * m;
  q = zero;
  CHECK(q == 0 * m);
}

TEST_CASE("a NaN is unordered against zero", "[zero][nan]")
{
  // as against `0.`: every ordering of a NaN is false, including the ones that would be the
  // negation of each other
  const quantity not_a_number = std::numeric_limits<double>::quiet_NaN() * m;
  CHECK_FALSE(not_a_number < zero);
  CHECK_FALSE(not_a_number >= zero);
  CHECK_FALSE(std::is_eq(not_a_number <=> zero));
  CHECK(not_a_number != zero);
}

TEST_CASE("zero survives being passed to a function for durations", "[zero]")
{
  std::vector<std::chrono::seconds> durations = {std::chrono::seconds{3}, std::chrono::seconds{0}};
  CHECK(std::ranges::find(durations, zero) - durations.begin() == 1);
  CHECK(std::ranges::count(durations, zero) == 1);
  CHECK_FALSE(std::ranges::less{}(durations.front(), zero));
}
