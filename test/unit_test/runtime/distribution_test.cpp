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
#include <mp-units/ostream.h>
#include <mp-units/random.h>
#include <mp-units/systems/si/unit_symbols.h>
#include <mp-units/systems/si/units.h>
#include <array>
#include <initializer_list>
#include <random>
#include <vector>


using namespace mp_units;

TEST_CASE("uniform_int_distribution")
{
  using rep = std::int64_t;
  using q = quantity<isq::length[si::metre], rep>;

  SECTION("default")
  {
    auto dist = mp_units::uniform_int_distribution<q>();

    CHECK(dist.a() == q::zero());
    CHECK(dist.b() == q::max());
  }

  SECTION("parametrized")
  {
    constexpr rep a = 2;
    constexpr rep b = 5;

    auto stl_dist = std::uniform_int_distribution(a, b);
    auto units_dist = mp_units::uniform_int_distribution(a * si::metre, b * si::metre);

    CHECK(units_dist.a() == stl_dist.a() * si::metre);
    CHECK(units_dist.b() == stl_dist.b() * si::metre);
    CHECK(units_dist.min() == stl_dist.min() * si::metre);
    CHECK(units_dist.max() == stl_dist.max() * si::metre);
  }
}

TEST_CASE("uniform_real_distribution")
{
  using rep = long double;
  using q = quantity<isq::length[si::metre], rep>;

  SECTION("default")
  {
    auto dist = mp_units::uniform_real_distribution<q>();

    CHECK(dist.a() == q::zero());
    CHECK(dist.b() == q::one());
  }

  SECTION("parametrized")
  {
    constexpr rep a = 2.0;
    constexpr rep b = 5.0;

    auto stl_dist = std::uniform_real_distribution(a, b);
    auto units_dist = mp_units::uniform_real_distribution(a * si::metre, b * si::metre);

    CHECK(units_dist.a() == stl_dist.a() * si::metre);
    CHECK(units_dist.b() == stl_dist.b() * si::metre);
    CHECK(units_dist.min() == stl_dist.min() * si::metre);
    CHECK(units_dist.max() == stl_dist.max() * si::metre);
  }
}

TEST_CASE("binomial_distribution")
{
  using rep = std::int64_t;
  using q = quantity<isq::length[si::metre], rep>;

  SECTION("default")
  {
    auto dist = mp_units::binomial_distribution<q>();

    CHECK(dist.p() == 0.5);
    CHECK(dist.t() == q::one());
  }

  SECTION("parametrized")
  {
    constexpr rep t = 5;
    constexpr double p = 0.25;

    auto stl_dist = std::binomial_distribution(t, p);
    auto units_dist = mp_units::binomial_distribution(t * si::metre, p);

    CHECK(units_dist.p() == stl_dist.p());
    CHECK(units_dist.t() == stl_dist.t() * si::metre);
    CHECK(units_dist.min() == stl_dist.min() * si::metre);
    CHECK(units_dist.max() == stl_dist.max() * si::metre);
  }
}

TEST_CASE("negative_binomial_distribution")
{
  using rep = std::int64_t;
  using q = quantity<isq::length[si::metre], rep>;

  SECTION("default")
  {
    auto dist = mp_units::negative_binomial_distribution<q>();

    CHECK(dist.p() == 0.5);
    CHECK(dist.k() == q::one());
  }

  SECTION("parametrized")
  {
    constexpr rep k = 5;
    constexpr double p = 0.25;

    auto stl_dist = std::negative_binomial_distribution(k, p);
    auto units_dist = mp_units::negative_binomial_distribution(k * si::metre, p);

    CHECK(units_dist.p() == stl_dist.p());
    CHECK(units_dist.k() == stl_dist.k() * si::metre);
    CHECK(units_dist.min() == stl_dist.min() * si::metre);
    CHECK(units_dist.max() == stl_dist.max() * si::metre);
  }
}

TEST_CASE("geometric_distribution")
{
  using rep = std::int64_t;
  using q = quantity<isq::length[si::metre], rep>;

  SECTION("default")
  {
    auto dist = mp_units::geometric_distribution<q>();

    CHECK(dist.p() == 0.5);
  }

  SECTION("parametrized")
  {
    constexpr double p = 0.25;

    auto stl_dist = std::geometric_distribution<rep>(p);
    auto units_dist = mp_units::geometric_distribution<q>(p);

    CHECK(units_dist.p() == stl_dist.p());
    CHECK(units_dist.min() == stl_dist.min() * si::metre);
    CHECK(units_dist.max() == stl_dist.max() * si::metre);
  }
}

TEST_CASE("poisson_distribution")
{
  using rep = std::int64_t;
  using q = quantity<isq::length[si::metre], rep>;

  SECTION("default")
  {
    auto dist = mp_units::poisson_distribution<q>();

    CHECK(dist.mean() == 1.0);
  }

  SECTION("parametrized")
  {
    constexpr double mean = 5.0;

    auto stl_dist = std::poisson_distribution<rep>(mean);
    auto units_dist = mp_units::poisson_distribution<q>(mean);

    CHECK(units_dist.mean() == stl_dist.mean());
    CHECK(units_dist.min() == stl_dist.min() * si::metre);
    CHECK(units_dist.max() == stl_dist.max() * si::metre);
  }
}

TEST_CASE("exponential_distribution")
{
  using rep = long double;
  using q = quantity<isq::length[si::metre], rep>;

  SECTION("default")
  {
    auto dist = mp_units::exponential_distribution<q>();

    CHECK(dist.lambda() == 1.0);
  }

  SECTION("parametrized")
  {
    constexpr double lambda = 2.0;

    auto stl_dist = std::exponential_distribution<rep>(lambda);
    auto units_dist = mp_units::exponential_distribution<q>(lambda);

    CHECK(units_dist.lambda() == stl_dist.lambda());
    CHECK(units_dist.min() == stl_dist.min() * si::metre);
    CHECK(units_dist.max() == stl_dist.max() * si::metre);
  }
}

TEST_CASE("gamma_distribution")
{
  using rep = long double;
  using q = quantity<isq::length[si::metre], rep>;

  SECTION("default")
  {
    auto dist = mp_units::gamma_distribution<q>();

    CHECK(dist.alpha() == 1.0);
    CHECK(dist.beta() == 1.0);
  }

  SECTION("parametrized")
  {
    constexpr double alpha = 5.0;
    constexpr double beta = 2.0;

    auto stl_dist = std::gamma_distribution<rep>(alpha, beta);
    auto units_dist = mp_units::gamma_distribution<q>(alpha, beta);

    CHECK(units_dist.alpha() == stl_dist.alpha());
    CHECK(units_dist.beta() == stl_dist.beta());
    CHECK(units_dist.min() == stl_dist.min() * si::metre);
    CHECK(units_dist.max() == stl_dist.max() * si::metre);
  }
}

TEST_CASE("weibull_distribution")
{
  using rep = long double;
  using q = quantity<isq::length[si::metre], rep>;

  SECTION("default")
  {
    auto dist = mp_units::weibull_distribution<q>();

    CHECK(dist.a() == 1.0);
    CHECK(dist.b() == 1.0);
  }

  SECTION("parametrized")
  {
    constexpr rep a = 5.0;
    constexpr rep b = 2.0;

    auto stl_dist = std::weibull_distribution(a, b);
    auto units_dist = mp_units::weibull_distribution<q>(a, b);

    CHECK(units_dist.a() == stl_dist.a());
    CHECK(units_dist.b() == stl_dist.b());
    CHECK(units_dist.min() == stl_dist.min() * si::metre);
    CHECK(units_dist.max() == stl_dist.max() * si::metre);
  }
}

TEST_CASE("extreme_value_distribution")
{
  using rep = long double;
  using q = quantity<isq::length[si::metre], rep>;

  SECTION("default")
  {
    auto dist = mp_units::extreme_value_distribution<q>();

    CHECK(dist.a() == q::zero());
    CHECK(dist.b() == 1.0);
  }

  SECTION("parametrized")
  {
    constexpr rep a = 5.0;
    constexpr rep b = 2.0;

    auto stl_dist = std::extreme_value_distribution(a, b);
    auto units_dist = mp_units::extreme_value_distribution<q>(a * si::metre, b);

    CHECK(units_dist.a() == stl_dist.a() * si::metre);
    CHECK(units_dist.b() == stl_dist.b());
    CHECK(units_dist.min() == stl_dist.min() * si::metre);
    CHECK(units_dist.max() == stl_dist.max() * si::metre);
  }
}

TEST_CASE("normal_distribution")
{
  using rep = long double;
  using q = quantity<isq::length[si::metre], rep>;

  SECTION("default")
  {
    auto dist = mp_units::normal_distribution<q>();

    CHECK(dist.mean() == q::zero());
    CHECK(dist.stddev() == q::one());
  }

  SECTION("parametrized")
  {
    constexpr rep mean = 5.0;
    constexpr rep stddev = 2.0;

    auto stl_dist = std::normal_distribution(mean, stddev);
    auto units_dist = mp_units::normal_distribution(mean * si::metre, stddev * si::metre);

    CHECK(units_dist.mean() == stl_dist.mean() * si::metre);
    CHECK(units_dist.stddev() == stl_dist.stddev() * si::metre);
    CHECK(units_dist.min() == stl_dist.min() * si::metre);
    CHECK(units_dist.max() == stl_dist.max() * si::metre);
  }
}

TEST_CASE("lognormal_distribution")
{
  using rep = long double;
  using q = quantity<isq::length[si::metre], rep>;

  SECTION("default")
  {
    auto dist = mp_units::lognormal_distribution<q>();

    CHECK(dist.m() == q::zero());
    CHECK(dist.s() == q::one());
  }

  SECTION("parametrized")
  {
    constexpr rep m = 5.0;
    constexpr rep s = 2.0;

    auto stl_dist = std::lognormal_distribution(m, s);
    auto units_dist = mp_units::lognormal_distribution(m * si::metre, s * si::metre);

    CHECK(units_dist.m() == stl_dist.m() * si::metre);
    CHECK(units_dist.s() == stl_dist.s() * si::metre);
    CHECK(units_dist.min() == stl_dist.min() * si::metre);
    CHECK(units_dist.max() == stl_dist.max() * si::metre);
  }
}

TEST_CASE("chi_squared_distribution")
{
  using rep = long double;
  using q = quantity<isq::length[si::metre], rep>;

  SECTION("default")
  {
    auto dist = mp_units::chi_squared_distribution<q>();

    CHECK(dist.n() == 1.0);
  }

  SECTION("parametrized")
  {
    constexpr rep n = 5.0;

    auto stl_dist = std::chi_squared_distribution(n);
    auto units_dist = mp_units::chi_squared_distribution<q>(n);

    CHECK(units_dist.n() == stl_dist.n());
    CHECK(units_dist.min() == stl_dist.min() * si::metre);
    CHECK(units_dist.max() == stl_dist.max() * si::metre);
  }
}

TEST_CASE("cauchy_distribution")
{
  using rep = long double;
  using q = quantity<isq::length[si::metre], rep>;

  SECTION("default")
  {
    auto dist = mp_units::cauchy_distribution<q>();

    CHECK(dist.a() == q::zero());
    CHECK(dist.b() == q::one());
  }

  SECTION("parametrized")
  {
    constexpr rep a = 5.0;
    constexpr rep b = 2.0;

    auto stl_dist = std::cauchy_distribution(a, b);
    auto units_dist = mp_units::cauchy_distribution(a * si::metre, b * si::metre);

    CHECK(units_dist.a() == stl_dist.a() * si::metre);
    CHECK(units_dist.b() == stl_dist.b() * si::metre);
    CHECK(units_dist.min() == stl_dist.min() * si::metre);
    CHECK(units_dist.max() == stl_dist.max() * si::metre);
  }
}

TEST_CASE("fisher_f_distribution")
{
  using rep = long double;
  using q = quantity<isq::length[si::metre], rep>;

  SECTION("default")
  {
    auto dist = mp_units::fisher_f_distribution<q>();

    CHECK(dist.m() == 1.0);
    CHECK(dist.n() == 1.0);
  }

  SECTION("parametrized")
  {
    constexpr rep m = 5.0;
    constexpr rep n = 2.0;

    auto stl_dist = std::fisher_f_distribution<rep>(m, n);
    auto units_dist = mp_units::fisher_f_distribution<q>(m, n);

    CHECK(units_dist.m() == stl_dist.m());
    CHECK(units_dist.n() == stl_dist.n());
    CHECK(units_dist.min() == stl_dist.min() * si::metre);
    CHECK(units_dist.max() == stl_dist.max() * si::metre);
  }
}

TEST_CASE("student_t_distribution")
{
  using rep = long double;
  using q = quantity<isq::length[si::metre], rep>;

  SECTION("default")
  {
    auto dist = mp_units::student_t_distribution<q>();

    CHECK(dist.n() == 1.0);
  }

  SECTION("parametrized")
  {
    constexpr rep n = 2.0;

    auto stl_dist = std::student_t_distribution<rep>(n);
    auto units_dist = mp_units::student_t_distribution<q>(n);

    CHECK(units_dist.n() == stl_dist.n());
    CHECK(units_dist.min() == stl_dist.min() * si::metre);
    CHECK(units_dist.max() == stl_dist.max() * si::metre);
  }
}

TEST_CASE("discrete_distribution")
{
  using rep = std::int64_t;
  using q = quantity<isq::length[si::metre], rep>;

  SECTION("default")
  {
    auto stl_dist = std::discrete_distribution<rep>();
    auto units_dist = mp_units::discrete_distribution<q>();

    CHECK(units_dist.min() == stl_dist.min() * si::metre);
    CHECK(units_dist.max() == stl_dist.max() * si::metre);
    CHECK(units_dist.probabilities() == stl_dist.probabilities());
  }

  SECTION("parametrized_input_it")
  {
    constexpr std::array<double, 3> weights = {1.0, 2.0, 3.0};

    auto stl_dist = std::discrete_distribution<rep>(weights.cbegin(), weights.cend());
    auto units_dist = mp_units::discrete_distribution<q>(weights.cbegin(), weights.cend());

    CHECK(units_dist.probabilities() == stl_dist.probabilities());
  }

  SECTION("parametrized_initializer_list")
  {
    std::initializer_list<double> weights = {1.0, 2.0, 3.0};

    auto stl_dist = std::discrete_distribution<rep>(weights);
    auto units_dist = mp_units::discrete_distribution<q>(weights);

    CHECK(units_dist.probabilities() == stl_dist.probabilities());
  }

  SECTION("parametrized_range")
  {
    constexpr std::size_t count = 3;
    constexpr double xmin = 1, xmax = 3;

    auto stl_dist = std::discrete_distribution<rep>(count, xmin, xmax, [](double val) { return val; });
    auto units_dist = mp_units::discrete_distribution<q>(count, xmin, xmax, [](double val) { return val; });

    CHECK(units_dist.probabilities() == stl_dist.probabilities());
  }
}

TEST_CASE("piecewise_constant_distribution")
{
  using rep = long double;
  using q = quantity<isq::length[si::metre], rep>;

  std::vector<rep> intervals_rep_vec = {1.0, 2.0, 3.0};
  std::vector<q> intervals_qty_vec = {1.0 * isq::length[si::metre], 2.0 * isq::length[si::metre],
                                      3.0 * isq::length[si::metre]};

  SECTION("default")
  {
    auto stl_dist = std::piecewise_constant_distribution<rep>();
    auto units_dist = mp_units::piecewise_constant_distribution<q>();

    CHECK(units_dist.min() == stl_dist.min() * si::metre);
    CHECK(units_dist.max() == stl_dist.max() * si::metre);
    CHECK(stl_dist.intervals().size() == 2);
    CHECK(units_dist.intervals().size() == 2);
    CHECK(stl_dist.densities().size() == 1);
    CHECK(units_dist.densities().size() == 1);
  }

  SECTION("parametrized_input_it")
  {
    constexpr std::array<rep, 3> intervals_rep = {1.0, 2.0, 3.0};
    constexpr std::array<q, 3> intervals_qty = {1.0 * isq::length[si::metre], 2.0 * isq::length[si::metre],
                                                3.0 * isq::length[si::metre]};
    constexpr std::array<rep, 3> weights = {1.0, 2.0, 3.0};

    auto stl_dist =
      std::piecewise_constant_distribution<rep>(intervals_rep.cbegin(), intervals_rep.cend(), weights.cbegin());
    auto units_dist =
      mp_units::piecewise_constant_distribution<q>(intervals_qty.cbegin(), intervals_qty.cend(), weights.cbegin());

    CHECK(stl_dist.intervals() == intervals_rep_vec);
    CHECK(units_dist.intervals() == intervals_qty_vec);
    CHECK(units_dist.densities() == stl_dist.densities());
  }

  SECTION("parametrized_initializer_list")
  {
    std::initializer_list<rep> intervals_rep = {1.0, 2.0, 3.0};
    std::initializer_list<q> intervals_qty = {1.0 * isq::length[si::metre], 2.0 * isq::length[si::metre],
                                              3.0 * isq::length[si::metre]};

    auto stl_dist = std::piecewise_constant_distribution<rep>(intervals_rep, [](rep val) { return val; });
    auto units_dist = mp_units::piecewise_constant_distribution<q>(
      intervals_qty, [](q qty) { return qty.numerical_value_ref_in(qty.unit); });

    CHECK(units_dist.intervals() == intervals_qty_vec);
    CHECK(units_dist.densities() == stl_dist.densities());
  }

  SECTION("parametrized_range")
  {
    constexpr std::size_t nw = 2;
    constexpr rep xmin_rep = 1.0, xmax_rep = 3.0;
    constexpr q xmin_qty = 1.0 * isq::length[si::metre], xmax_qty = 3.0 * isq::length[si::metre];

    auto stl_dist = std::piecewise_constant_distribution<rep>(nw, xmin_rep, xmax_rep, [](rep val) { return val; });
    auto units_dist = mp_units::piecewise_constant_distribution<q>(
      nw, xmin_qty, xmax_qty, [](q qty) { return qty.numerical_value_ref_in(qty.unit); });

    CHECK(units_dist.intervals() == intervals_qty_vec);
    CHECK(units_dist.densities() == stl_dist.densities());
  }
}

TEST_CASE("piecewise_linear_distribution")
{
  using rep = long double;
  using q = quantity<isq::length[si::metre], rep>;

  std::vector<rep> intervals_rep_vec = {1.0, 2.0, 3.0};
  std::vector<q> intervals_qty_vec = {1.0 * isq::length[si::metre], 2.0 * isq::length[si::metre],
                                      3.0 * isq::length[si::metre]};

  SECTION("default")
  {
    auto stl_dist = std::piecewise_linear_distribution<rep>();
    auto units_dist = mp_units::piecewise_linear_distribution<q>();

    CHECK(units_dist.min() == stl_dist.min() * si::metre);
    CHECK(units_dist.max() == stl_dist.max() * si::metre);
    CHECK(stl_dist.intervals().size() == 2);
    CHECK(units_dist.intervals().size() == 2);
    CHECK(stl_dist.densities().size() == 2);
    CHECK(units_dist.densities().size() == 2);
  }

  SECTION("parametrized_input_it")
  {
    constexpr std::array<rep, 3> intervals_rep = {1.0, 2.0, 3.0};
    constexpr std::array<q, 3> intervals_qty = {1.0 * isq::length[si::metre], 2.0 * isq::length[si::metre],
                                                3.0 * isq::length[si::metre]};
    constexpr std::array<rep, 3> weights = {1.0, 2.0, 3.0};

    auto stl_dist =
      std::piecewise_linear_distribution<rep>(intervals_rep.cbegin(), intervals_rep.cend(), weights.cbegin());
    auto units_dist =
      mp_units::piecewise_linear_distribution<q>(intervals_qty.cbegin(), intervals_qty.cend(), weights.cbegin());

    CHECK(stl_dist.intervals() == intervals_rep_vec);
    CHECK(units_dist.intervals() == intervals_qty_vec);
    CHECK(units_dist.densities() == stl_dist.densities());
  }

  SECTION("parametrized_initializer_list")
  {
    std::initializer_list<rep> intervals_rep = {1.0, 2.0, 3.0};
    std::initializer_list<q> intervals_qty = {1.0 * isq::length[si::metre], 2.0 * isq::length[si::metre],
                                              3.0 * isq::length[si::metre]};

    auto stl_dist = std::piecewise_linear_distribution<rep>(intervals_rep, [](rep val) { return val; });
    auto units_dist = mp_units::piecewise_linear_distribution<q>(
      intervals_qty, [](q qty) { return qty.numerical_value_ref_in(qty.unit); });

    CHECK(units_dist.intervals() == intervals_qty_vec);
    CHECK(units_dist.densities() == stl_dist.densities());
  }

  SECTION("parametrized_range")
  {
    constexpr std::size_t nw = 2;
    constexpr rep xmin_rep = 1.0, xmax_rep = 3.0;
    constexpr q xmin_qty = 1.0 * isq::length[si::metre], xmax_qty = 3.0 * isq::length[si::metre];

    auto stl_dist = std::piecewise_linear_distribution<rep>(nw, xmin_rep, xmax_rep, [](rep val) { return val; });
    auto units_dist = mp_units::piecewise_linear_distribution<q>(
      nw, xmin_qty, xmax_qty, [](q qty) { return qty.numerical_value_ref_in(qty.unit); });

    CHECK(units_dist.intervals() == intervals_qty_vec);
    CHECK(units_dist.densities() == stl_dist.densities());
  }
}
