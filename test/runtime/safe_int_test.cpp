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

#include <catch2/catch_test_macros.hpp>
#include <mp-units/bits/hacks.h>
#include <mp-units/constrained.h>
#include <mp-units/safe_int.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si.h>
#include <mp-units/systems/yard_pound.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#endif

using namespace mp_units;
using namespace mp_units::si::unit_symbols;
using namespace mp_units::yard_pound::unit_symbols;

// ============================================================================
// safe_int with throw_policy (default on hosted)
// ============================================================================

TEST_CASE("safe_int with throw_policy", "[safe_int][throw]")
{
  using T = safe_int<int>;
  const auto int_max = std::numeric_limits<int>::max();
  const auto int_min = std::numeric_limits<int>::min();

  SECTION("normal arithmetic does not throw")
  {
    REQUIRE(T{3} + T{4} == T{7});
    REQUIRE(T{10} - T{3} == T{7});
    REQUIRE(T{3} * T{4} == T{12});
    REQUIRE(T{12} / T{4} == T{3});
    REQUIRE(T{10} % T{3} == T{1});
    REQUIRE(-T{5} == T{-5});

    T x{5};
    REQUIRE(++x == T{6});
    REQUIRE(x++ == T{6});
    REQUIRE(x == T{7});
    REQUIRE(--x == T{6});
    REQUIRE(x-- == T{6});
    REQUIRE(x == T{5});
  }

  SECTION("addition overflow throws")
  {
    REQUIRE_THROWS_AS(T{int_max} + T{1}, std::overflow_error);
    REQUIRE_THROWS_AS(T{int_min} + T{-1}, std::overflow_error);
  }

  SECTION("subtraction overflow throws")
  {
    REQUIRE_THROWS_AS(T{int_min} - T{1}, std::overflow_error);
    REQUIRE_THROWS_AS(T{int_max} - T{-1}, std::overflow_error);
  }

  SECTION("multiplication overflow throws")
  {
    REQUIRE_THROWS_AS(T{int_max} * T{2}, std::overflow_error);
    REQUIRE_THROWS_AS(T{int_min} * T{-1}, std::overflow_error);
  }

  SECTION("division overflow throws")
  {
    REQUIRE_THROWS_AS(T{1} / T{0}, std::overflow_error);
    REQUIRE_THROWS_AS(T{int_min} / T{-1}, std::overflow_error);
  }

  SECTION("modulo by zero throws") { REQUIRE_THROWS_AS(T{10} % T{0}, std::overflow_error); }

  SECTION("negation overflow throws") { REQUIRE_THROWS_AS(-T{int_min}, std::overflow_error); }

  SECTION("negation of short INT16_MIN does not overflow (promotes to int)")
  {
    // -(-32768) as short would overflow short, but unary - promotes to int first,
    // so the result is int(32768) which is in range — no exception expected.
    using S = safe_int<std::int16_t>;
    REQUIRE(-S{std::numeric_limits<std::int16_t>::min()} == safe_int<int>{32768});
  }

  SECTION("increment overflow throws")
  {
    T x{int_max};
    REQUIRE_THROWS_AS(++x, std::overflow_error);
  }

  SECTION("decrement overflow throws")
  {
    T x{int_min};
    REQUIRE_THROWS_AS(--x, std::overflow_error);
  }

  SECTION("unsigned overflow throws")
  {
    using U = safe_int<unsigned>;
    const auto uint_max = std::numeric_limits<unsigned>::max();
    REQUIRE_THROWS_AS(U{uint_max} + U{1u}, std::overflow_error);
    REQUIRE_THROWS_AS(U{0u} - U{1u}, std::overflow_error);
    REQUIRE_THROWS_AS(U{uint_max} * U{2u}, std::overflow_error);
  }

  SECTION("throw message contains 'overflow'")
  {
    try {
      [[maybe_unused]] auto r = T{int_max} + T{1};
      FAIL("expected exception not thrown");
    } catch (const std::overflow_error& e) {
      REQUIRE(std::string(e.what()).find("overflow") != std::string::npos);
    }
  }
}

// ============================================================================
// Converting constructor overflow detection
// ============================================================================

TEST_CASE("safe_int converting constructor", "[safe_int][convert][throw]")
{
  SECTION("raw integer narrowing overflow")
  {
    // int → short: value too large
    REQUIRE_THROWS_AS(safe_int<short>(int{40'000}), std::overflow_error);
    REQUIRE_THROWS_AS(safe_int<short>(int{-40'000}), std::overflow_error);

    // int → short: value fits — no throw
    REQUIRE(safe_int<short>(int{100}).value() == 100);
    REQUIRE(safe_int<short>(int{-100}).value() == -100);
    REQUIRE(safe_int<short>(int{32'767}).value() == 32'767);
    REQUIRE(safe_int<short>(int{-32'768}).value() == -32'768);

    // unsigned → short: value too large
    REQUIRE_THROWS_AS(safe_int<short>(40'000u), std::overflow_error);

    // int → uint8_t: value out of range
    REQUIRE_THROWS_AS(safe_int<std::uint8_t>(int{256}), std::overflow_error);
    REQUIRE_THROWS_AS(safe_int<std::uint8_t>(int{-1}), std::overflow_error);

    // int → unsigned: negative value
    REQUIRE_THROWS_AS(safe_int<unsigned>(int{-1}), std::overflow_error);

    // unsigned → int: value too large
    REQUIRE_THROWS_AS(safe_int<int>(std::numeric_limits<unsigned>::max()), std::overflow_error);
  }

  SECTION("safe_int<U> narrowing overflow")
  {
    // safe_int<int> → safe_int<short>: value too large
    REQUIRE_THROWS_AS(safe_int<short>(safe_int<int>{40'000}), std::overflow_error);
    REQUIRE_THROWS_AS(safe_int<short>(safe_int<int>{-40'000}), std::overflow_error);

    // safe_int<int> → safe_int<short>: value fits — no throw
    REQUIRE(safe_int<short>(safe_int<int>{100}).value() == 100);

    // safe_int<unsigned> → safe_int<int>: value too large
    REQUIRE_THROWS_AS(safe_int<int>(safe_int<unsigned>{std::numeric_limits<unsigned>::max()}), std::overflow_error);

    // safe_int<int> → safe_int<unsigned>: negative value
    REQUIRE_THROWS_AS(safe_int<unsigned>(safe_int<int>{-1}), std::overflow_error);
  }
}

// ============================================================================
// safe_int as a quantity representation type
// ============================================================================

TEST_CASE("safe_int as quantity representation", "[safe_int][quantity]")
{
  SECTION("basic arithmetic does not throw")
  {
    quantity<isq::length[m], safe_int<int>> d1 = safe_int{100} * m;
    quantity<isq::length[m], safe_int<int>> d2 = safe_int{50} * m;

    REQUIRE((d1 + d2).numerical_value_in(m) == 150);
    REQUIRE((d1 - d2).numerical_value_in(m) == 50);
    REQUIRE((d1 * 2).numerical_value_in(m) == 200);
    REQUIRE((d1 / 2).numerical_value_in(m) == 50);
  }

  SECTION("unit conversion does not throw when safe")
  {
    quantity<isq::length[m], safe_int<int>> distance = safe_int{2} * m;
    quantity<isq::length[mm], safe_int<int>> distance_mm = distance;
    REQUIRE(distance_mm.numerical_value_in(mm) == 2000);
  }

  SECTION("arithmetic overflow throws")
  {
    quantity<isq::length[m], safe_int<int>> large = safe_int{std::numeric_limits<int>::max()} * m;
    REQUIRE_THROWS_AS(large + safe_int{1} * m, std::overflow_error);
  }

  SECTION("cross-unit arithmetic")
  {
    quantity<isq::length[m], safe_int<std::int16_t>> meters = safe_int<std::int16_t>{20} * m;
    quantity<isq::length[ft], safe_int<std::int16_t>> feet = safe_int<std::int16_t>{100} * ft;

    auto total = meters + feet;
    REQUIRE(total.force_in(m).numerical_value_in(m) > 0);
  }

  SECTION("cross-unit overflow throws")
  {
    quantity<isq::length[m], safe_int<std::int16_t>> meters = safe_int<std::int16_t>{30'000} * m;
    quantity<isq::length[ft], safe_int<std::int16_t>> feet = safe_int<std::int16_t>{10'000} * ft;

    // Adding meters + feet works (promoted to safe_int<int>)
    auto total = meters + feet;
    REQUIRE(total.force_in(m).numerical_value_in(m) > 0);

    // But forcing back to int16_t overflows (30000 + ~3048 = ~33048 > 32767)
    REQUIRE_THROWS_AS(total.force_in<safe_int<std::int16_t>>(m), std::overflow_error);
  }

  SECTION("multiplication overflow throws")
  {
    quantity<isq::length[m], safe_int<int>> dist = safe_int{100'000} * m;
    quantity<isq::length[m], safe_int<int>> width = safe_int{100'000} * m;
    REQUIRE_THROWS_AS(dist * width, std::overflow_error);
  }

  SECTION("scalar multiplication overflow throws")
  {
    quantity<isq::length[m], safe_int<int>> dist = safe_int{std::numeric_limits<int>::max()} * m;
    REQUIRE_THROWS_AS(dist * 2, std::overflow_error);
  }

  SECTION(".in(unit) overflow throws")
  {
    // 2,200,000 m → µm requires ×1,000,000; product 2.2×10¹² overflows int32_t
    quantity<isq::length[m], safe_i32> dist = safe_i32{2'200'000} * m;
    REQUIRE_THROWS_AS(dist.in(um), std::overflow_error);

    // sanity: a value that fits must not throw
    quantity<isq::length[m], safe_i32> small = safe_i32{2} * m;
    REQUIRE_NOTHROW(small.in(um));
    REQUIRE(small.in(um).numerical_value_in(um) == 2'000'000);
  }

  SECTION("automatic common-unit scaling overflow throws")
  {
    // 1,500,000,000 m + 1,000,000 km: converting km→common-unit requires ×10⁶ which overflows int32_t
    quantity<isq::length[m], safe_i32> dist_m = safe_i32{1'500'000'000} * m;
    quantity<isq::length[km], safe_i32> dist_km = safe_i32{1'000'000} * km;
    REQUIRE_THROWS_AS(dist_m + dist_km, std::overflow_error);

    // sanity: values that don't overflow must not throw
    quantity<isq::length[m], safe_i32> small_m = safe_i32{100} * m;
    quantity<isq::length[km], safe_i32> small_km = safe_i32{1} * km;
    REQUIRE_NOTHROW(small_m + small_km);
  }

  SECTION("convenience aliases work")
  {
    quantity<isq::length[m], safe_i32> dist = safe_i32{100} * m;
    REQUIRE(dist.numerical_value_in(m) == 100);

    quantity<isq::time[s], safe_i16> time = safe_i16{10} * s;
    auto speed = dist / time;
    REQUIRE(speed.numerical_value_in(m / s) == 10);
  }

  SECTION("comparison operations work")
  {
    quantity<isq::length[m], safe_int<int>> d1 = safe_int{100} * m;
    quantity<isq::length[m], safe_int<int>> d2 = safe_int{50} * m;

    REQUIRE(d1 > d2);
    REQUIRE(d2 < d1);
    REQUIRE(d1 >= safe_int{100} * m);
    REQUIRE(d2 <= safe_int{100} * m);
    REQUIRE(d1 == safe_int{100} * m);
    REQUIRE(d1 != d2);
  }
}

// ============================================================================
// Scalar op overflow: safe_int<T> op U and U op safe_int<T>
// ============================================================================

TEST_CASE("safe_int * scalar operations", "[safe_int][scalar][throw]")
{
  using T = safe_int<int>;
  const auto int_max = std::numeric_limits<int>::max();
  const auto int_min = std::numeric_limits<int>::min();

  SECTION("addition overflow throws")
  {
    REQUIRE_THROWS_AS(T{int_max} + 1, std::overflow_error);
    REQUIRE_THROWS_AS(1 + T{int_max}, std::overflow_error);
    REQUIRE_THROWS_AS(T{int_min} + (-1), std::overflow_error);
    REQUIRE_THROWS_AS((-1) + T{int_min}, std::overflow_error);
  }

  SECTION("subtraction overflow throws")
  {
    REQUIRE_THROWS_AS(T{int_min} - 1, std::overflow_error);     // INT_MIN - 1 underflows
    REQUIRE_THROWS_AS(1 - T{int_min}, std::overflow_error);     // 1 - INT_MIN = INT_MAX+2 overflows
    REQUIRE_THROWS_AS(T{int_max} - (-1), std::overflow_error);  // INT_MAX - (-1) = INT_MAX+1 overflows
    REQUIRE_THROWS_AS((-2) - T{int_max}, std::overflow_error);  // -2 - INT_MAX < INT_MIN overflows
  }

  SECTION("multiplication overflow throws")
  {
    REQUIRE_THROWS_AS(T{int_max} * 2, std::overflow_error);
    REQUIRE_THROWS_AS(2 * T{int_max}, std::overflow_error);
  }

  SECTION("division overflow throws")
  {
    REQUIRE_THROWS_AS(T{1} / 0, std::overflow_error);           // divide by zero
    REQUIRE_THROWS_AS(1 / T{0}, std::overflow_error);           // divide by zero
    REQUIRE_THROWS_AS(T{int_min} / (-1), std::overflow_error);  // INT_MIN / -1
    REQUIRE_THROWS_AS(int_min / T{-1}, std::overflow_error);    // INT_MIN / -1
  }

  SECTION("modulo by zero throws")
  {
    REQUIRE_THROWS_AS(T{10} % 0, std::overflow_error);
    REQUIRE_THROWS_AS(10 % T{0}, std::overflow_error);
  }
}

// ============================================================================
// Cross-wrapper overflow: constrained<U,CP> op safe_int<T,EP>
// The integral-winner overloads delegate to safe_int ops, so overflow is detected.
// ============================================================================

TEST_CASE("safe_int * constrained operations", "[safe_int][cross-wrapper][throw]")
{
  using C = constrained<int>;
  using S = safe_int<int>;
  const auto int_max = std::numeric_limits<int>::max();
  const auto int_min = std::numeric_limits<int>::min();

  SECTION("addition overflow throws")
  {
    REQUIRE_THROWS_AS(C{int_max} + S{1}, std::overflow_error);
    REQUIRE_THROWS_AS(S{int_max} + C{1}, std::overflow_error);
  }

  SECTION("subtraction overflow throws")
  {
    REQUIRE_THROWS_AS(C{int_min} - S{1}, std::overflow_error);
    REQUIRE_THROWS_AS(S{int_min} - C{1}, std::overflow_error);
  }

  SECTION("multiplication overflow throws")
  {
    REQUIRE_THROWS_AS(C{int_max} * S{2}, std::overflow_error);
    REQUIRE_THROWS_AS(S{int_max} * C{2}, std::overflow_error);
  }

  SECTION("division overflow throws")
  {
    REQUIRE_THROWS_AS(C{1} / S{0}, std::overflow_error);
    REQUIRE_THROWS_AS(S{1} / C{0}, std::overflow_error);
    REQUIRE_THROWS_AS(C{int_min} / S{-1}, std::overflow_error);
    REQUIRE_THROWS_AS(S{int_min} / C{-1}, std::overflow_error);
  }

  SECTION("modulo by zero throws")
  {
    REQUIRE_THROWS_AS(C{10} % S{0}, std::overflow_error);
    REQUIRE_THROWS_AS(S{10} % C{0}, std::overflow_error);
  }
}
