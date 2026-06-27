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
#include <mp-units/framework.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si.h>
#include <mp-units/utility/constrained.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <stdexcept>
#endif

using namespace mp_units;
using namespace mp_units::utility;

#if MP_UNITS_HOSTED

// ============================================================================
// Error policies and violation handler
// ============================================================================

// CHECK_THROWS_AS expands to a try/catch where the catch body is unreachable on MSVC Debug
// because `on_constraint_violation` is [[noreturn]] and the optimizer doesn't elide the
// dead branch; suppress the resulting C4702 around these specific cases.
MP_UNITS_DIAGNOSTIC_PUSH
MP_UNITS_DIAGNOSTIC_IGNORE_UNREACHABLE
TEST_CASE("constrained error policies", "[constrained]")
{
  SECTION("throw_policy throws std::domain_error on violation")
  {
    CHECK_THROWS_AS(throw_policy::on_constraint_violation("test"), std::domain_error);
  }

  SECTION("constraint_violation_handler delegates to throw_policy")
  {
    using T = constrained<double, throw_policy>;
    CHECK_THROWS_AS(constraint_violation_handler<T>::on_violation("test"), std::domain_error);
  }
}
MP_UNITS_DIAGNOSTIC_POP

// ============================================================================
// constrained<double, throw_policy> as quantity_point rep with check_in_range.
// Only the throwing (out-of-bounds) paths are tested here; in-bounds paths
// are covered by static_assert tests in test/static/bounded_quantity_point_test.cpp.
// ============================================================================

namespace {

QUANTITY_SPEC(geo_latitude_qs, isq::angular_measure);

inline constexpr struct geo_equator final :
    absolute_point_origin<geo_latitude_qs,
                          check_in_range{-90 * mp_units::si::unit_symbols::deg, 90 * mp_units::si::unit_symbols::deg}> {
} geo_equator;

using safe_double = constrained<double, throw_policy>;
using latitude_qp = quantity_point<geo_latitude_qs[si::unit_symbols::deg], geo_equator, safe_double>;

}  // namespace

TEST_CASE("constrained as quantity_point representation", "[constrained][quantity_point]")
{
  using namespace si::unit_symbols;

  SECTION("out-of-range construction throws")
  {
    CHECK_THROWS_AS(latitude_qp(91.0 * geo_latitude_qs[deg], geo_equator), std::domain_error);
    CHECK_THROWS_AS(latitude_qp(-91.0 * geo_latitude_qs[deg], geo_equator), std::domain_error);
    CHECK_THROWS_AS(latitude_qp(180.0 * geo_latitude_qs[deg], geo_equator), std::domain_error);
  }

  SECTION("arithmetic that moves value out of range throws")
  {
    auto pt = latitude_qp(80.0 * geo_latitude_qs[deg], geo_equator);
    CHECK_THROWS_AS(pt += 20.0 * geo_latitude_qs[deg], std::domain_error);  // 80 + 20 = 100 > 90
  }
}

#endif  // MP_UNITS_HOSTED
