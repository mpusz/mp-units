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

#include "almost_equals.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <limits>
#include <type_traits>
#endif
#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/framework.h>
#include <mp-units/math.h>
#include <mp-units/systems/hep.h>
#endif

using namespace mp_units;

TEST_CASE("hep::beta_from_gamma and hep::gamma_from_beta", "[hep][math]")
{
  SECTION("a particle at rest") { REQUIRE(hep::beta_from_gamma(hep::lorentz_factor(1. * one)) == 0. * one); }

  SECTION("gamma 1.25 is beta 0.6")
  {
    REQUIRE_THAT(hep::beta_from_gamma(hep::lorentz_factor(1.25 * one)),
                 AlmostEquals(hep::relativistic_beta(0.6 * one)));
    REQUIRE_THAT(hep::gamma_from_beta(hep::relativistic_beta(0.6 * one)),
                 AlmostEquals(hep::lorentz_factor(1.25 * one)));
  }

  // From β to γ and back is well conditioned. The other way is not: a β close to 1 keeps few
  // digits of 1 − β, so γ → β → γ loses about γ² ulps whatever the formula.
  SECTION("beta survives the round trip")
  {
    const quantity beta = hep::relativistic_beta(0.999 * one);
    REQUIRE_THAT(hep::beta_from_gamma(hep::gamma_from_beta(beta)), AlmostEquals(beta));
  }

  SECTION("the results are typed")
  {
    STATIC_REQUIRE(QuantityOf<decltype(hep::beta_from_gamma(hep::lorentz_factor(2. * one))), hep::relativistic_beta>);
    STATIC_REQUIRE(QuantityOf<decltype(hep::gamma_from_beta(hep::relativistic_beta(0.5 * one))), hep::lorentz_factor>);
    STATIC_REQUIRE(std::is_same_v<decltype(hep::beta_from_gamma(hep::lorentz_factor(2.f * one)))::rep, float>);
  }
}

// The preconditions are `!(gamma < 1)` and `!(beta < 0 || beta >= 1)`, not `gamma >= 1` and
// `0 <= beta && beta < 1`. Every comparison with a NaN is false, so the positive forms report a NaN
// as out of range. Rewriting either predicate the positive way makes these sections abort.
TEST_CASE("hep::beta_from_gamma and hep::gamma_from_beta propagate a NaN", "[hep][math][nan]")
{
  const double nan = std::numeric_limits<double>::quiet_NaN();

  SECTION("beta_from_gamma") { REQUIRE(isnan(hep::beta_from_gamma(hep::lorentz_factor(nan * one)))); }

  SECTION("gamma_from_beta") { REQUIRE(isnan(hep::gamma_from_beta(hep::relativistic_beta(nan * one)))); }
}
