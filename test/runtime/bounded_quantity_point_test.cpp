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
#include <mp-units/bits/config.h>
#include <mp-units/framework.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si.h>
#include <mp-units/utility/constrained.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cmath>
#include <limits>
#include <stdexcept>
#endif

using namespace mp_units;
using namespace mp_units::utility;
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

TEST_CASE("check_non_negative propagates through relative_point_origin - mutating operators", "[bounded][non_negative]")
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
    CHECK(pt.delta_from(average_height_origin).numerical_value_in(m) == -1600.0);
  }
}

// ============================================================================
// NaN is outside every range, so the checking policies reject it.
// ============================================================================

namespace {

constexpr double not_a_number = std::numeric_limits<double>::quiet_NaN();

}  // namespace

TEST_CASE("check_in_range reports NaN", "[bounded][check][nan]")
{
  CHECK_THROWS_AS(qp_check(not_a_number * test_angle_check[deg], check_origin), std::domain_error);
}

// A user-defined checking policy only tests the value. The library reports a value it rejects.
// Here a domain that is not an interval: the allowed bands of a transmitter.
namespace {

template<Quantity Q>
struct check_in_bands {
  Q min;  // first band: [min, first_band_max]
  Q first_band_max;
  Q second_band_min;  // second band: [second_band_min, max]
  Q max;

  template<Quantity V>
  [[nodiscard]] constexpr bool operator()(const V& v) const
  {
    return (v >= V{min} && v <= V{first_band_max}) || (v >= V{second_band_min} && v <= V{max});
  }
};

#if !MP_UNITS_HAS_AGGREGATE_CTAD
template<Quantity Q>
check_in_bands(Q, Q, Q, Q) -> check_in_bands<Q>;
#endif

inline constexpr struct tuning_zero final :
    absolute_point_origin<isq::frequency, check_in_bands{2.4 * GHz, 2.4835 * GHz, 5.725 * GHz, 5.875 * GHz}> {
} tuning_zero;

using tx_frequency = quantity_point<isq::frequency[GHz], tuning_zero, constrained<double, throw_policy>>;

inline constexpr struct heading_zero final :
    absolute_point_origin<isq::angular_measure, check_in_range{0. * deg, exclusive_bound{360. * deg}}> {
} heading_zero;

using heading = quantity_point<si::degree, heading_zero, constrained<double, throw_policy>>;

}  // namespace

TEST_CASE("a user-defined checking policy's rejection is reported", "[bounded][check]")
{
  CHECK_NOTHROW(tx_frequency(2.45 * GHz, tuning_zero));
  CHECK_THROWS_AS(tx_frequency(3. * GHz, tuning_zero), std::domain_error);
  CHECK_THROWS_AS(tx_frequency(not_a_number * GHz, tuning_zero), std::domain_error);
}

TEST_CASE("wrap_to_range and reflect_in_range turn a non-finite value into NaN", "[bounded][nan]")
{
  constexpr double inf = std::numeric_limits<double>::infinity();
  constexpr wrap_to_range wrap{0. * deg, 360. * deg};
  constexpr reflect_in_range reflect{-90. * deg, 90. * deg};
  CHECK(std::isnan(wrap(inf * deg).numerical_value_in(deg)));
  CHECK(std::isnan(wrap(-inf * deg).numerical_value_in(deg)));
  CHECK(std::isnan(wrap(not_a_number * deg).numerical_value_in(deg)));
  CHECK(std::isnan(reflect(inf * deg).numerical_value_in(deg)));
  CHECK(std::isnan(reflect(not_a_number * deg).numerical_value_in(deg)));
}

TEST_CASE("wrap_to_range never returns its excluded max", "[bounded][wrap]")
{
  // -1e-14° is 360° - 1e-14°, which rounds to 360° in a double: the same point as 0°.
  CHECK(wrap_to_range{0. * deg, 360. * deg}(-1e-14 * deg) == 0. * deg);
}

TEST_CASE("check_in_range reports a value at an excluded bound", "[bounded][check]")
{
  CHECK_NOTHROW(heading(359.5 * deg, heading_zero));
  CHECK_THROWS_AS(heading(360. * deg, heading_zero), std::domain_error);
}

TEST_CASE("check_non_negative reports NaN", "[bounded][non_negative][nan]")
{
  CHECK_THROWS_AS(qp_avg_safe(not_a_number * avg_height_qs[m], average_height_origin), std::domain_error);
}

// ============================================================================
// is_within_bounds and try_make hand a violation back as a value: the
// representation's policy is never invoked, whether it throws or terminates.
// ============================================================================

namespace {

inline constexpr struct trim_zero final :
    absolute_point_origin<isq::angular_measure, check_in_range{-10. * deg, 10. * deg}> {
} trim_zero;

using trim_angle_throwing = quantity_point<si::degree, trim_zero, constrained<double, throw_policy>>;
using trim_angle_terminating = quantity_point<si::degree, trim_zero, constrained<double, terminate_policy>>;

inline constexpr struct trim_centre final :
    relative_point_origin<quantity_point<si::degree, trim_zero, double>{4. * deg, trim_zero},
                          check_in_range{-5. * deg, 5. * deg}> {
} trim_centre;

inline constexpr struct trim_centre_plus_one final : relative_point_origin<trim_centre + 1. * deg> {
} trim_centre_plus_one;

using trim_offset2 = quantity_point<si::degree, trim_centre_plus_one, constrained<double, terminate_policy>>;

}  // namespace

TEST_CASE("is_within_bounds rejects NaN through a relative origin without own bounds", "[bounded][nan]")
{
  CHECK_FALSE(trim_offset2::is_within_bounds(not_a_number * deg, trim_centre_plus_one));
  CHECK(trim_offset2::is_within_bounds(4. * deg, trim_centre_plus_one));
}

TEST_CASE("a throwing handler leaves a point unchanged", "[bounded][handler]")
{
  const trim_angle_throwing at_max{10. * deg, trim_zero};
  const trim_angle_throwing at_min{-10. * deg, trim_zero};

  trim_angle_throwing p = at_max;
  CHECK_THROWS_AS(p += 1. * deg, std::domain_error);
  CHECK(p == at_max);
  CHECK_THROWS_AS(++p, std::domain_error);
  CHECK(p == at_max);
  CHECK_THROWS_AS(p++, std::domain_error);
  CHECK(p == at_max);

  p = at_min;
  CHECK_THROWS_AS(p -= 1. * deg, std::domain_error);
  CHECK(p == at_min);
  CHECK_THROWS_AS(--p, std::domain_error);
  CHECK(p == at_min);
  CHECK_THROWS_AS(p--, std::domain_error);
  CHECK(p == at_min);
}

#if MP_UNITS_HAS_EXPECTED

TEST_CASE("try_make does not invoke a throwing policy", "[bounded][try_make]")
{
  CHECK_NOTHROW(trim_angle_throwing::try_make(11. * deg, trim_zero));
  CHECK_NOTHROW(trim_angle_throwing::try_make(not_a_number * deg, trim_zero));
  CHECK_FALSE(trim_angle_throwing::try_make(11. * deg, trim_zero).has_value());
  CHECK_FALSE(trim_angle_throwing::try_make(not_a_number * deg, trim_zero).has_value());
  CHECK(trim_angle_throwing::try_make(10. * deg, trim_zero).has_value());
}

TEST_CASE("try_make does not invoke a terminating policy", "[bounded][try_make]")
{
  // Reaching the checks at all shows nothing aborted.
  CHECK_FALSE(trim_angle_terminating::try_make(-11. * deg, trim_zero).has_value());
  CHECK_FALSE(trim_angle_terminating::try_make(not_a_number * deg, trim_zero).has_value());
  CHECK(trim_angle_terminating::try_make(-10. * deg, trim_zero).has_value());
}

TEST_CASE("try_make rejects NaN through a relative origin without own bounds", "[bounded][try_make][nan]")
{
  CHECK_FALSE(trim_offset2::try_make(not_a_number * deg, trim_centre_plus_one).has_value());
  CHECK(trim_offset2::try_make(4. * deg, trim_centre_plus_one).has_value());
}

#endif  // MP_UNITS_HAS_EXPECTED

#endif  // MP_UNITS_HOSTED
