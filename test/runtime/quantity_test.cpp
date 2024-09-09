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
#include <atomic>
#include <numbers>
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

// conversion requiring radical magnitudes
TEST_CASE("unit conversions support radical magnitudes", "[conversion][radical]")
{
  REQUIRE(within_4_ulps(sqrt((1.0 * m) * (1.0 * km)).numerical_value_in(m), sqrt(1000.0)));
}

// Reproducing issue #474 exactly:
TEST_CASE("Issue 474 is fixed", "[conversion][radical]")
{
  constexpr auto val_issue_474 = 8.0 * si::si2019::boltzmann_constant * 1000.0 * K / (std::numbers::pi * 10 * Da);
  REQUIRE(within_4_ulps(sqrt(val_issue_474).numerical_value_in(m / s),
                        sqrt(val_issue_474.numerical_value_in(m * m / s / s))));
}

TEST_CASE("Volatile representation type", "[volatile]")
{
  volatile std::int16_t vint = 123;
  REQUIRE(quantity(vint * m).numerical_value_in(m) == 123);
}
