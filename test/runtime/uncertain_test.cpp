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
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <mp-units/compat_macros.h>
#include <mp-units/ext/format.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cmath>
#include <numbers>
#include <sstream>
#include <type_traits>
#endif
#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/systems/iau.h>
#include <mp-units/systems/si.h>
#include <mp-units/utility/uncertain.h>
#endif

using namespace mp_units;
using namespace mp_units::si::unit_symbols;
using namespace Catch::Matchers;
using mp_units::utility::measurement_of;
using mp_units::utility::uncertain;

TEST_CASE("uncertain observers", "[uncertain]")
{
  const uncertain val{10.0, 0.1};
  CHECK(val.value() == 10.0);
  CHECK(val.uncertainty() == 0.1);
  CHECK_THAT(val.relative_uncertainty(), WithinRel(0.01));
  CHECK_THAT(val.lower_bound(), WithinRel(9.9));
  CHECK_THAT(val.upper_bound(), WithinRel(10.1));

  const uncertain<double> exact = 5.0;
  CHECK(exact.value() == 5.0);
  CHECK(exact.uncertainty() == 0.0);
}

TEST_CASE("uncertainty propagation for independent values", "[uncertain]")
{
  const uncertain lhs{3.0, 0.3};
  const uncertain rhs{4.0, 0.4};

  SECTION("addition adds uncertainties in quadrature")
  {
    const auto res = lhs + rhs;
    CHECK(res.value() == 7.0);
    CHECK_THAT(res.uncertainty(), WithinRel(0.5));
  }

  SECTION("subtraction adds uncertainties in quadrature")
  {
    const auto res = rhs - lhs;
    CHECK(res.value() == 1.0);
    CHECK_THAT(res.uncertainty(), WithinRel(0.5));
  }

  SECTION("multiplication combines the partial derivatives in quadrature")
  {
    const auto res = lhs * rhs;
    CHECK(res.value() == 12.0);
    CHECK_THAT(res.uncertainty(), WithinRel(12.0 * std::hypot(0.1, 0.1)));
  }

  SECTION("division combines the partial derivatives in quadrature")
  {
    const auto res = lhs / rhs;
    CHECK(res.value() == 0.75);
    CHECK_THAT(res.uncertainty(), WithinRel(0.75 * std::hypot(0.1, 0.1)));
  }

  SECTION("a zero central value is handled without NaN")
  {
    // a measurement consistent with zero is ordinary; the relative-uncertainty form would divide
    // by zero here, so the absolute form must be used
    const uncertain zero{0.0, 0.1};
    const auto product = zero * rhs;
    CHECK(product.value() == 0.0);
    CHECK_THAT(product.uncertainty(), WithinRel(4.0 * 0.1));

    const auto quotient = zero / rhs;
    CHECK(quotient.value() == 0.0);
    CHECK_THAT(quotient.uncertainty(), WithinRel(0.1 / 4.0));
  }

  SECTION("an exact scalar scales the uncertainty")
  {
    CHECK((lhs * 2.0).value() == 6.0);
    CHECK_THAT((lhs * 2.0).uncertainty(), WithinRel(0.6));
    CHECK_THAT((2.0 * lhs).uncertainty(), WithinRel(0.6));
    CHECK_THAT((lhs / 2.0).uncertainty(), WithinRel(0.15));
    CHECK_THAT((12.0 / rhs).uncertainty(), WithinRel(3.0 * 0.1));
  }
}

TEST_CASE("uncertain math functions", "[uncertain]")
{
  SECTION("sqrt")
  {
    const auto res = sqrt(uncertain{4.0, 0.4});
    CHECK(res.value() == 2.0);
    CHECK_THAT(res.uncertainty(), WithinRel(0.1));
  }

  SECTION("pow")
  {
    const auto res = pow(uncertain{2.0, 0.1}, 3.0);
    CHECK(res.value() == 8.0);
    CHECK_THAT(res.uncertainty(), WithinRel(3.0 * 8.0 / 2.0 * 0.1));
  }

  SECTION("exp and log are inverse-consistent")
  {
    const uncertain arg{2.0, 0.02};
    const auto exp_result = exp(arg);
    CHECK_THAT(exp_result.value(), WithinRel(std::exp(2.0)));
    CHECK_THAT(exp_result.uncertainty(), WithinRel(std::exp(2.0) * 0.02));
    const auto log_result = log(exp_result);
    CHECK_THAT(log_result.value(), WithinRel(2.0));
    CHECK_THAT(log_result.uncertainty(), WithinRel(0.02));
  }

  SECTION("abs preserves the uncertainty")
  {
    const auto res = abs(-uncertain{3.0, 0.3});
    CHECK(res.value() == 3.0);
    CHECK(res.uncertainty() == 0.3);
  }

  SECTION("cbrt")
  {
    const auto res = cbrt(uncertain{8.0, 0.6});
    CHECK_THAT(res.value(), WithinRel(2.0));
    CHECK_THAT(res.uncertainty(), WithinRel(0.6 / (3.0 * 4.0)));
  }

  SECTION("log10 and log2")
  {
    const auto res10 = log10(uncertain{100.0, 1.0});
    CHECK_THAT(res10.value(), WithinRel(2.0));
    CHECK_THAT(res10.uncertainty(), WithinRel(1.0 / (100.0 * std::log(10.0))));

    const auto res2 = log2(uncertain{8.0, 0.1});
    CHECK_THAT(res2.value(), WithinRel(3.0));
    CHECK_THAT(res2.uncertainty(), WithinRel(0.1 / (8.0 * std::log(2.0))));
  }
}

TEST_CASE("uncertain trigonometric functions", "[uncertain]")
{
  SECTION("sin and cos propagate through the derivative")
  {
    const uncertain arg{0.5, 0.01};
    const auto sin_result = sin(arg);
    CHECK_THAT(sin_result.value(), WithinRel(std::sin(0.5)));
    CHECK_THAT(sin_result.uncertainty(), WithinRel(std::abs(std::cos(0.5)) * 0.01));

    const auto cos_result = cos(arg);
    CHECK_THAT(cos_result.value(), WithinRel(std::cos(0.5)));
    CHECK_THAT(cos_result.uncertainty(), WithinRel(std::abs(std::sin(0.5)) * 0.01));
  }

  SECTION("sin has zero sensitivity at its extremum")
  {
    // at x = π/2 the derivative of sin vanishes, so the first-order uncertainty does too
    const auto res = sin(uncertain{std::numbers::pi / 2, 0.01});
    CHECK_THAT(res.value(), WithinRel(1.0));
    CHECK_THAT(res.uncertainty(), WithinAbs(0.0, 1e-15));
  }

  SECTION("tan")
  {
    const auto res = tan(uncertain{0.5, 0.01});
    CHECK_THAT(res.value(), WithinRel(std::tan(0.5)));
    CHECK_THAT(res.uncertainty(), WithinRel(0.01 / (std::cos(0.5) * std::cos(0.5))));
  }

  SECTION("inverse trigonometric functions")
  {
    const auto asin_result = asin(uncertain{0.5, 0.01});
    CHECK_THAT(asin_result.value(), WithinRel(std::asin(0.5)));
    CHECK_THAT(asin_result.uncertainty(), WithinRel(0.01 / std::sqrt(1.0 - 0.25)));

    const auto acos_result = acos(uncertain{0.5, 0.01});
    CHECK_THAT(acos_result.value(), WithinRel(std::acos(0.5)));
    CHECK_THAT(acos_result.uncertainty(), WithinRel(0.01 / std::sqrt(1.0 - 0.25)));

    const auto atan_result = atan(uncertain{0.5, 0.01});
    CHECK_THAT(atan_result.value(), WithinRel(std::atan(0.5)));
    CHECK_THAT(atan_result.uncertainty(), WithinRel(0.01 / 1.25));
  }

  SECTION("atan2 combines both uncertainties")
  {
    const uncertain y{3.0, 0.03};
    const uncertain x{4.0, 0.04};
    const auto res = atan2(y, x);
    CHECK_THAT(res.value(), WithinRel(std::atan2(3.0, 4.0)));
    CHECK_THAT(res.uncertainty(), WithinRel(std::hypot(4.0 * 0.03, 3.0 * 0.04) / 25.0));
  }

  SECTION("sin round-trips through asin")
  {
    const uncertain arg{0.3, 0.005};
    const auto round_tripped = asin(sin(arg));
    CHECK_THAT(round_tripped.value(), WithinRel(0.3));
    CHECK_THAT(round_tripped.uncertainty(), WithinRel(0.005));
  }
}

TEST_CASE("uncertain as a quantity representation type", "[uncertain]")
{
  const quantity length = uncertain{10.0, 0.1} * m;
  const quantity width = uncertain{5.0, 0.05} * m;

  SECTION("arithmetic propagates through quantities")
  {
    const quantity area = length * width;
    CHECK(area.numerical_value_in(m2).value() == 50.0);
    CHECK_THAT(area.numerical_value_in(m2).uncertainty(), WithinRel(50.0 * std::hypot(0.01, 0.01)));
  }

  SECTION("unit conversion scales the value and the uncertainty together")
  {
    const quantity in_mm = length.in(mm);
    CHECK(in_mm.numerical_value_in(mm).value() == 10'000.0);
    CHECK_THAT(in_mm.numerical_value_in(mm).uncertainty(), WithinRel(100.0));
    CHECK_THAT(in_mm.numerical_value_in(mm).relative_uncertainty(),
               WithinRel(length.numerical_value_in(m).relative_uncertainty()));
  }

  SECTION("an exact quantity joins propagation as value ± 0")
  {
    const quantity total = length + 5.0 * m;
    CHECK(total.numerical_value_in(m).value() == 15.0);
    CHECK_THAT(total.numerical_value_in(m).uncertainty(), WithinRel(0.1));
  }
}

TEST_CASE("measurement_of a measured constant", "[uncertain]")
{
  const quantity G_measured = measurement_of(iau::newtonian_constant_of_gravitation);

  SECTION("in its own unit the constant is exactly 1")
  {
    // the uncertainty is relational: it belongs to the conversion factor between units, not to
    // the count of the constant's own unit
    const auto& val = G_measured.numerical_value_ref_in(G_measured.unit);
    CHECK(val.value() == 1.0);
    CHECK(val.uncertainty() == 0.0);
  }

  SECTION("conversion to SI materializes the relative uncertainty")
  {
    const quantity in_si = G_measured.in(m3 / kg / s2);
    CHECK_THAT(in_si.numerical_value_in(m3 / kg / s2).value(), WithinRel(6.6743e-11));
    CHECK_THAT(in_si.numerical_value_in(m3 / kg / s2).relative_uncertainty(), WithinRel(2.2e-5));
  }

  SECTION("cancellation against the constant unit stays symbolic and exact")
  {
    const quantity ratio = G_measured / (1.0 * iau::newtonian_constant_of_gravitation);
    // parenthesized: Catch2's decomposer would otherwise call the consteval unit `operator==` at runtime
    CHECK((ratio.unit == one));
    CHECK(ratio.numerical_value_in(one).value() == 1.0);
    CHECK(ratio.numerical_value_in(one).uncertainty() == 0.0);
  }

  SECTION("solar mass derived at the quantity level propagates G's uncertainty")
  {
    const quantity solar_mass = (1.0 * iau::nominal_solar_mass_parameter) / G_measured;
    CHECK_THAT(solar_mass.numerical_value_in(kg).value(), WithinRel(1.98841e30, 1e-4));
    CHECK_THAT(solar_mass.numerical_value_in(kg).relative_uncertainty(), WithinRel(2.2e-5));
  }

  SECTION("a unit defined from a measured constant embeds its central value")
  {
    // `iau::solar_mass` is defined as (GM)_sun/G, so its magnitude carries G's central value.
    // "two solar masses" is therefore an exact statement, and the conversion to kilograms uses
    // the sanctioned value with no uncertainty attached for an exact representation type.
    const quantity mass = 2.0 * iau::solar_mass;
    CHECK_THAT(mass.numerical_value_in(kg), WithinRel(3.97682e30, 1e-4));
    static_assert(std::is_same_v<decltype(mass)::rep, double>);
  }
}

TEST_CASE("conversion factors built from measured constants carry their uncertainty", "[uncertain]")
{
  const quantity mass = 2.0 * iau::solar_mass;

  SECTION("value_cast to an uncertain representation folds the factor's uncertainty in")
  {
    const quantity in_kg = value_cast<kg, uncertain<double>>(mass);
    CHECK_THAT(in_kg.numerical_value_in(kg).value(), WithinRel(3.97682e30, 1e-4));
    // solar mass = (GM)_sun/G, so the kg conversion inherits exactly G's relative uncertainty
    CHECK_THAT(in_kg.numerical_value_in(kg).relative_uncertainty(), WithinRel(2.2e-5));
  }

  SECTION("in<Rep>(unit) spells the same opt-in")
  {
    const quantity in_kg = mass.in<uncertain<double>>(kg);
    CHECK_THAT(in_kg.numerical_value_in(kg).value(), WithinRel(3.97682e30, 1e-4));
    CHECK_THAT(in_kg.numerical_value_in(kg).relative_uncertainty(), WithinRel(2.2e-5));
  }

  SECTION("an uncertain representation folds the factor in on a plain in(unit)")
  {
    const quantity counted = uncertain{2.0, 0.1} * iau::solar_mass;
    const quantity in_kg = counted.in(kg);
    // the value's own 5% and the factor's 2.2e-5 combine in quadrature
    CHECK_THAT(in_kg.numerical_value_in(kg).relative_uncertainty(), WithinRel(std::hypot(0.05, 2.2e-5)));
  }

  SECTION("a shared measured constant cancels, exactly as its central value does")
  {
    // both solar and terrestrial masses are defined as (GM)ᴺ/G, so their ratio is exact by
    // definition and no uncertainty may be reported for the conversion
    const quantity counted = uncertain<double>{2.0} * iau::solar_mass;
    const quantity in_earth_masses = counted.in(iau::terrestrial_mass);
    CHECK(in_earth_masses.numerical_value_in(iau::terrestrial_mass).uncertainty() == 0.0);
    CHECK_THAT(in_earth_masses.numerical_value_in(iau::terrestrial_mass).value(),
               WithinRel(2.0 * 1.3271244e20 / 3.986004e14));
  }

  SECTION("the direction of the conversion does not matter")
  {
    const quantity in_kg = uncertain<double>{3.97682e30} * kg;
    const quantity in_solar_masses = in_kg.in(iau::solar_mass);
    CHECK_THAT(in_solar_masses.numerical_value_in(iau::solar_mass).relative_uncertainty(), WithinRel(2.2e-5));
  }

  SECTION("a fractional power of a measured constant scales its uncertainty contribution")
  {
    const quantity q = uncertain<double>{1.0} * sqrt(iau::newtonian_constant_of_gravitation);
    const quantity converted = q.in(sqrt(m3 / kg / s2));
    CHECK_THAT(converted.numerical_value_in(sqrt(m3 / kg / s2)).relative_uncertainty(), WithinRel(0.5 * 2.2e-5));
  }

  SECTION("an exact representation type stays exact")
  {
    const quantity in_kg = mass.in(kg);
    static_assert(std::is_same_v<decltype(in_kg)::rep, double>);
    CHECK_THAT(in_kg.numerical_value_in(kg), WithinRel(3.97682e30, 1e-4));
  }

  SECTION("implicit conversions in mixed-unit arithmetic fold the factor's uncertainty too")
  {
    // no `.in()` is spelled here; the common-unit machinery converts the solar-mass operand,
    // and that conversion's factor carries G's uncertainty
    const quantity solar = uncertain<double>{1.0} * iau::solar_mass;
    const quantity in_kg = uncertain<double>{1.0e30} * kg;
    const quantity sum = solar + in_kg;
    const auto expected_sigma = 1.98841e30 * 2.2e-5;
    CHECK_THAT(sum.numerical_value_in(kg).value(), WithinRel(2.98841e30, 1e-4));
    CHECK_THAT(sum.numerical_value_in(kg).uncertainty(), WithinRel(expected_sigma, 1e-3));
  }
}

TEST_CASE("uncertain text output", "[uncertain]")
{
  SECTION("stream insertion")
  {
    std::ostringstream os;
    os << uncertain{2.5, 0.25};
    CHECK(os.str() == "2.5 ± 0.25");
  }

  SECTION("formatting")
  {
    CHECK(MP_UNITS_STD_FMT::format("{}", uncertain{2.5, 0.25}) == "2.5 ± 0.25");
    CHECK(MP_UNITS_STD_FMT::format("{}", uncertain{2.5, 0.25} * m) == "2.5 ± 0.25 m");
  }
}
