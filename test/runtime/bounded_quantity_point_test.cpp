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
#include <mp-units/constrained.h>
#include <mp-units/framework.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <stdexcept>
#endif

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

#if MP_UNITS_HOSTED

// ============================================================================
// check_in_range tests — out-of-bounds violations must throw.
//
// Using constrained<double, throw_policy> as the rep ensures that
// constraint_violation_handler is called unconditionally (regardless of
// MP_UNITS_API_CONTRACTS), and that handler throws std::domain_error.
// This is the only behaviour that cannot be covered by static_assert tests.
// ============================================================================

namespace {

QUANTITY_SPEC(test_angle_check, isq::angular_measure);

inline constexpr struct check_origin final :
    absolute_point_origin<test_angle_check, check_in_range{-90 * deg, 90 * deg}> {
} check_origin;

using safe_double = constrained<double, throw_policy>;
using qp_check = quantity_point<test_angle_check[deg], check_origin, safe_double>;

}  // namespace

TEST_CASE("check_in_range throws on construction outside bounds", "[bounded][check]")
{
  CHECK_THROWS_AS(qp_check(91.0 * test_angle_check[deg], check_origin), std::domain_error);
  CHECK_THROWS_AS(qp_check(-91.0 * test_angle_check[deg], check_origin), std::domain_error);
  CHECK_THROWS_AS(qp_check(180.0 * test_angle_check[deg], check_origin), std::domain_error);
  CHECK_THROWS_AS(qp_check(-180.0 * test_angle_check[deg], check_origin), std::domain_error);
}

TEST_CASE("check_in_range does not throw within bounds", "[bounded][check]")
{
  CHECK_NOTHROW(qp_check(0.0 * test_angle_check[deg], check_origin));
  CHECK_NOTHROW(qp_check(45.0 * test_angle_check[deg], check_origin));
  CHECK_NOTHROW(qp_check(90.0 * test_angle_check[deg], check_origin));
  CHECK_NOTHROW(qp_check(-90.0 * test_angle_check[deg], check_origin));
}

TEST_CASE("check_in_range throws on arithmetic that crosses bounds", "[bounded][check]")
{
  SECTION("operator+= crosses max")
  {
    auto pt = qp_check(80.0 * test_angle_check[deg], check_origin);
    CHECK_THROWS_AS(pt += 20.0 * test_angle_check[deg], std::domain_error);
  }

  SECTION("operator-= crosses min")
  {
    auto pt = qp_check(-80.0 * test_angle_check[deg], check_origin);
    CHECK_THROWS_AS(pt -= 20.0 * test_angle_check[deg], std::domain_error);
  }

  SECTION("operator++ crosses max")
  {
    using qp_check_int = quantity_point<test_angle_check[deg], check_origin, constrained<int, throw_policy>>;
    auto pt = qp_check_int(90 * test_angle_check[deg], check_origin);
    CHECK_THROWS_AS(++pt, std::domain_error);
  }

  SECTION("operator-- crosses min")
  {
    using qp_check_int = quantity_point<test_angle_check[deg], check_origin, constrained<int, throw_policy>>;
    auto pt = qp_check_int(-90 * test_angle_check[deg], check_origin);
    CHECK_THROWS_AS(--pt, std::domain_error);
  }
}

// ============================================================================
// check_non_negative propagates through relative_point_origin.
//
// A relative_point_origin derived from a non_negative natural_point_origin
// inherits check_non_negative bounds.  Values whose absolute position goes
// negative trigger a contract violation.
//
// Using constrained<double, throw_policy> as the rep guarantees a throw
// regardless of MP_UNITS_API_CONTRACTS build settings — same technique as the
// check_in_range tests above.
// ============================================================================

namespace {

QUANTITY_SPEC(avg_height_qs, isq::height);

// natural_point_origin<avg_height_qs> auto-receives check_non_negative (isq::height
// is non_negative).  This relative origin at +1700 m inherits that constraint.
inline constexpr struct average_height_origin final :
    relative_point_origin<natural_point_origin<avg_height_qs> + 1700.0 * m> {
} average_height_origin;

using qp_avg_safe = quantity_point<avg_height_qs[m], average_height_origin, constrained<double, throw_policy>>;

}  // namespace

TEST_CASE("check_non_negative propagates through relative_point_origin", "[bounded][non_negative]")
{
  // Values whose absolute height >= 0 are valid (no throw).
  CHECK_NOTHROW(qp_avg_safe(0.0 * avg_height_qs[m], average_height_origin));
  CHECK_NOTHROW(qp_avg_safe(500.0 * avg_height_qs[m], average_height_origin));
  CHECK_NOTHROW(qp_avg_safe(-1500.0 * avg_height_qs[m], average_height_origin));  // 200 m absolute
  CHECK_NOTHROW(qp_avg_safe(-1700.0 * avg_height_qs[m], average_height_origin));  // 0 m absolute (boundary)

  // Values whose absolute height < 0 trigger check_non_negative.
  CHECK_THROWS_AS(qp_avg_safe(-1701.0 * avg_height_qs[m], average_height_origin), std::domain_error);
  CHECK_THROWS_AS(qp_avg_safe(-2000.0 * avg_height_qs[m], average_height_origin), std::domain_error);
}

TEST_CASE("check_non_negative propagates through relative_point_origin — mutating operators", "[bounded][non_negative]")
{
  SECTION("operator-= crosses absolute zero")
  {
    auto pt = qp_avg_safe(-1700.0 * avg_height_qs[m], average_height_origin);  // at 0 m absolute
    CHECK_THROWS_AS(pt -= 1.0 * avg_height_qs[m], std::domain_error);
  }

  SECTION("operator-= stays non-negative")
  {
    auto pt = qp_avg_safe(-1500.0 * avg_height_qs[m], average_height_origin);  // 200 m absolute
    CHECK_NOTHROW(pt -= 100.0 * avg_height_qs[m]);                             // 100 m absolute -- still >= 0
    CHECK(pt.quantity_from(average_height_origin).numerical_value_in(m) == -1600.0);
  }
}

#endif  // MP_UNITS_HOSTED
