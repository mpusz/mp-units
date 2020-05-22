#include <numeric>
#include <units/random.h>
#include <units/physical/si/length.h>
#include <catch2/catch.hpp>

using namespace units;
using namespace units::physical::si;

TEST_CASE("uniform_int_distribution")
{
  using Rep = std::int64_t;
  using Qty = length<metre, Rep>;

  SECTION("default")
  {
      auto dist = units::uniform_int_distribution<Qty>();
      
      CHECK(dist.a() == Qty::zero());
      CHECK(dist.b() == Qty::max());
  }
  
  SECTION ("parametrized") {
    constexpr Rep a = 5;
    constexpr Rep b = 2;
    
    auto stl_dist = std::uniform_int_distribution(a, b);
    auto units_dist = units::uniform_int_distribution(Qty(a), Qty(b));

    CHECK(units_dist.a() == Qty(stl_dist.a()));
    CHECK(units_dist.b() == Qty(stl_dist.b()));
    CHECK(units_dist.min() == Qty(stl_dist.min()));
    CHECK(units_dist.max() == Qty(stl_dist.max()));
  }
}

TEST_CASE("uniform_real_distribution")
{
  using Rep = long double;
  using Qty = length<metre, Rep>;

  SECTION("default")
  {
      auto dist = units::uniform_real_distribution<Qty>();
      
      CHECK(dist.a() == Qty::zero());
      CHECK(dist.b() == Qty::one());
  }
  
  SECTION ("parametrized") {
    constexpr Rep a = 5.0;
    constexpr Rep b = 2.0;
    
    auto stl_dist = std::uniform_real_distribution(a, b);
    auto units_dist = units::uniform_real_distribution(Qty(a), Qty(b));

    CHECK(units_dist.a() == Qty(stl_dist.a()));
    CHECK(units_dist.b() == Qty(stl_dist.b()));
    CHECK(units_dist.min() == Qty(stl_dist.min()));
    CHECK(units_dist.max() == Qty(stl_dist.max()));
  }
}

TEST_CASE("binomial_distribution")
{
  using Rep = std::int64_t;
  using Qty = length<metre, Rep>;

  SECTION("default")
  {
      auto dist = units::binomial_distribution<Qty>();
      
      CHECK(dist.p() == 0.5);
      CHECK(dist.t() == Qty::one());
  }
  
  SECTION ("parametrized") {
    constexpr Rep t = 5;
    constexpr double p = 0.25;
    
    auto stl_dist = std::binomial_distribution(t, p);
    auto units_dist = units::binomial_distribution(Qty(t), p);

    CHECK(units_dist.p() == stl_dist.p());
    CHECK(units_dist.t() == Qty(stl_dist.t()));
    CHECK(units_dist.min() == Qty(stl_dist.min()));
    CHECK(units_dist.max() == Qty(stl_dist.max()));
  }
}

TEST_CASE("negative_binomial_distribution")
{
  using Rep = std::int64_t;
  using Qty = length<metre, Rep>;

  SECTION("default")
  {
      auto dist = units::negative_binomial_distribution<Qty>();
      
      CHECK(dist.p() == 0.5);
      CHECK(dist.k() == Qty::one());
  }
  
  SECTION ("parametrized") {
    constexpr Rep k = 5;
    constexpr double p = 0.25;
    
    auto stl_dist = std::negative_binomial_distribution(k, p);
    auto units_dist = units::negative_binomial_distribution(Qty(k), p);

    CHECK(units_dist.p() == stl_dist.p());
    CHECK(units_dist.k() == Qty(stl_dist.k()));
    CHECK(units_dist.min() == Qty(stl_dist.min()));
    CHECK(units_dist.max() == Qty(stl_dist.max()));
  }
}

TEST_CASE("geometric_distribution")
{
  using Rep = std::int64_t;
  using Qty = length<metre, Rep>;

  SECTION("default")
  {
      auto dist = units::geometric_distribution<Qty>();
      
      CHECK(dist.p() == 0.5);
  }
  
  SECTION ("parametrized") {
    constexpr double p = 0.25;
    
    auto stl_dist = std::geometric_distribution<Rep>(p);
    auto units_dist = units::geometric_distribution<Qty>(p);

    CHECK(units_dist.p() == stl_dist.p());
    CHECK(units_dist.min() == Qty(stl_dist.min()));
    CHECK(units_dist.max() == Qty(stl_dist.max()));
  }
}

TEST_CASE("poisson_distribution")
{
  using Rep = std::int64_t;
  using Qty = length<metre, Rep>;

  SECTION("default")
  {
      auto dist = units::poisson_distribution<Qty>();
      
      CHECK(dist.mean() == 1.0);
  }
  
  SECTION ("parametrized") {
    constexpr double mean = 5.0;
    
    auto stl_dist = std::poisson_distribution<Rep>(mean);
    auto units_dist = units::poisson_distribution<Qty>(mean);

    CHECK(units_dist.mean() == stl_dist.mean());
    CHECK(units_dist.min() == Qty(stl_dist.min()));
    CHECK(units_dist.max() == Qty(stl_dist.max()));
  }
}

TEST_CASE("exponential_distribution")
{
  using Rep = long double;
  using Qty = length<metre, Rep>;

  SECTION("default")
  {
      auto dist = units::exponential_distribution<Qty>();
      
      CHECK(dist.lambda() == 1.0);
  }
  
  SECTION ("parametrized") {
    constexpr double lambda = 2.0;
    
    auto stl_dist = std::exponential_distribution<Rep>(lambda);
    auto units_dist = units::exponential_distribution<Qty>(lambda);

    CHECK(units_dist.lambda() == stl_dist.lambda());
    CHECK(units_dist.min() == Qty(stl_dist.min()));
    CHECK(units_dist.max() == Qty(stl_dist.max()));
  }
}

TEST_CASE("gamma_distribution")
{
  using Rep = long double;
  using Qty = length<metre, Rep>;

  SECTION("default")
  {
      auto dist = units::gamma_distribution<Qty>();
      
      CHECK(dist.alpha() == 1.0);
      CHECK(dist.beta() == 1.0);
  }
  
  SECTION ("parametrized") {
    constexpr double alpha = 5.0;
    constexpr double beta = 2.0;
    
    auto stl_dist = std::gamma_distribution<Rep>(alpha, beta);
    auto units_dist = units::gamma_distribution<Qty>(alpha, beta);

    CHECK(units_dist.alpha() == stl_dist.alpha());
    CHECK(units_dist.beta() == stl_dist.beta());
    CHECK(units_dist.min() == Qty(stl_dist.min()));
    CHECK(units_dist.max() == Qty(stl_dist.max()));
  }
}

TEST_CASE("weibull_distribution")
{
  using Rep = long double;
  using Qty = length<metre, Rep>;

  SECTION("default")
  {
      auto dist = units::weibull_distribution<Qty>();
      
      CHECK(dist.a() == 1.0);
      CHECK(dist.b() == 1.0);
  }
  
  SECTION ("parametrized") {
    constexpr Rep a = 5.0;
    constexpr Rep b = 2.0;
    
    auto stl_dist = std::weibull_distribution(a, b);
    auto units_dist = units::weibull_distribution<Qty>(a, b);

    CHECK(units_dist.a() == stl_dist.a());
    CHECK(units_dist.b() == stl_dist.b());
    CHECK(units_dist.min() == Qty(stl_dist.min()));
    CHECK(units_dist.max() == Qty(stl_dist.max()));
  }
}

TEST_CASE("extreme_value_distribution")
{
  using Rep = long double;
  using Qty = length<metre, Rep>;

  SECTION("default")
  {
      auto dist = units::extreme_value_distribution<Qty>();
      
      CHECK(dist.a() == Qty::zero());
      CHECK(dist.b() == 1.0);
  }
  
  SECTION ("parametrized") {
    constexpr Rep a = 5.0;
    constexpr Rep b = 2.0;
    
    auto stl_dist = std::extreme_value_distribution(a, b);
    auto units_dist = units::extreme_value_distribution<Qty>(Qty(a), b);

    CHECK(units_dist.a() == Qty(stl_dist.a()));
    CHECK(units_dist.b() == stl_dist.b());
    CHECK(units_dist.min() == Qty(stl_dist.min()));
    CHECK(units_dist.max() == Qty(stl_dist.max()));
  }
}

TEST_CASE("normal_distribution")
{
  using Rep = long double;
  using Qty = length<metre, Rep>;

  SECTION("default")
  {
      auto dist = units::normal_distribution<Qty>();
      
      CHECK(dist.mean() == Qty::zero());
      CHECK(dist.stddev() == Qty::one());
  }

  SECTION("parametrized")
  {
    constexpr Rep mean = 5.0;
    constexpr Rep stddev = 2.0;
    
    auto stl_dist = std::normal_distribution(mean, stddev);
    auto units_dist = units::normal_distribution(Qty(mean), Qty(stddev));
    
    CHECK(units_dist.mean() == Qty(stl_dist.mean()));
    CHECK(units_dist.stddev() == Qty(stl_dist.stddev()));
    CHECK(units_dist.min() == Qty(stl_dist.min()));
    CHECK(units_dist.max() == Qty(stl_dist.max()));
  }
}

TEST_CASE("lognormal_distribution")
{
  using Rep = long double;
  using Qty = length<metre, Rep>;

  SECTION("default")
  {
      auto dist = units::lognormal_distribution<Qty>();
      
      CHECK(dist.m() == Qty::zero());
      CHECK(dist.s() == Qty::one());
  }

  SECTION("parametrized")
  {
    constexpr Rep m = 5.0;
    constexpr Rep s = 2.0;
    
    auto stl_dist = std::lognormal_distribution(m, s);
    auto units_dist = units::lognormal_distribution(Qty(m), Qty(s));
    
    CHECK(units_dist.m() == Qty(stl_dist.m()));
    CHECK(units_dist.s() == Qty(stl_dist.s()));
    CHECK(units_dist.min() == Qty(stl_dist.min()));
    CHECK(units_dist.max() == Qty(stl_dist.max()));
  }
}

TEST_CASE("chi_squared_distribution")
{
  using Rep = long double;
  using Qty = length<metre, Rep>;

  SECTION("default")
  {
      auto dist = units::chi_squared_distribution<Qty>();
      
      CHECK(dist.n() == 1.0);
  }

  SECTION("parametrized")
  {
    constexpr Rep n = 5.0;
    
    auto stl_dist = std::chi_squared_distribution(n);
    auto units_dist = units::chi_squared_distribution<Qty>(n);
    
    CHECK(units_dist.n() == stl_dist.n());
    CHECK(units_dist.min() == Qty(stl_dist.min()));
    CHECK(units_dist.max() == Qty(stl_dist.max()));
  }
}

TEST_CASE("cauchy_distribution")
{
  using Rep = long double;
  using Qty = length<metre, Rep>;

  SECTION("default")
  {
      auto dist = units::cauchy_distribution<Qty>();
      
      CHECK(dist.a() == Qty::zero());
      CHECK(dist.b() == Qty::one());
  }
  
  SECTION ("parametrized") {
    constexpr Rep a = 5.0;
    constexpr Rep b = 2.0;
    
    auto stl_dist = std::cauchy_distribution(a, b);
    auto units_dist = units::cauchy_distribution(Qty(a), Qty(b));

    CHECK(units_dist.a() == Qty(stl_dist.a()));
    CHECK(units_dist.b() == Qty(stl_dist.b()));
    CHECK(units_dist.min() == Qty(stl_dist.min()));
    CHECK(units_dist.max() == Qty(stl_dist.max()));
  }
}

TEST_CASE("fisher_f_distribution")
{
  using Rep = long double;
  using Qty = length<metre, Rep>;

  SECTION("default")
  {
      auto dist = units::fisher_f_distribution<Qty>();
      
      CHECK(dist.m() == 1.0);
      CHECK(dist.n() == 1.0);
  }
  
  SECTION ("parametrized") {
    constexpr Rep m = 5.0;
    constexpr Rep n = 2.0;
    
    auto stl_dist = std::fisher_f_distribution<Rep>(m, n);
    auto units_dist = units::fisher_f_distribution<Qty>(m, n);

    CHECK(units_dist.m() == stl_dist.m());
    CHECK(units_dist.n() == stl_dist.n());
    CHECK(units_dist.min() == Qty(stl_dist.min()));
    CHECK(units_dist.max() == Qty(stl_dist.max()));
  }
}

TEST_CASE("student_t_distribution")
{
  using Rep = long double;
  using Qty = length<metre, Rep>;

  SECTION("default")
  {
      auto dist = units::student_t_distribution<Qty>();
      
      CHECK(dist.n() == 1.0);
  }
  
  SECTION ("parametrized") {
    constexpr Rep n = 2.0;
    
    auto stl_dist = std::student_t_distribution<Rep>(n);
    auto units_dist = units::student_t_distribution<Qty>(n);

    CHECK(units_dist.n() == stl_dist.n());
    CHECK(units_dist.min() == Qty(stl_dist.min()));
    CHECK(units_dist.max() == Qty(stl_dist.max()));
  }
}

TEST_CASE("discrete_distribution")
{
  using Rep = std::int64_t;
  using Qty = length<metre, Rep>;

  SECTION("default")
  {
      auto stl_dist = std::discrete_distribution<Rep>();
      auto units_dist = units::discrete_distribution<Qty>();
      
      CHECK(units_dist.min() == Qty(stl_dist.min()));
      CHECK(units_dist.max() == Qty(stl_dist.max()));
      CHECK(units_dist.probabilities() == stl_dist.probabilities());
  }
  
  SECTION ("parametrized_input_it") {
    constexpr std::array<double, 3> weights = {1.0, 2.0, 3.0};
    
    auto stl_dist = std::discrete_distribution<Rep>(weights.cbegin(), weights.cend());
    auto units_dist = units::discrete_distribution<Qty>(weights.cbegin(), weights.cend());

    CHECK(units_dist.probabilities() == stl_dist.probabilities());
  }

  SECTION ("parametrized_initializer_list") {
    std::initializer_list<double> weights = {1.0, 2.0, 3.0};
    
    auto stl_dist = std::discrete_distribution<Rep>(weights);
    auto units_dist = units::discrete_distribution<Qty>(weights);

    CHECK(units_dist.probabilities() == stl_dist.probabilities());
  }

  SECTION ("parametrized_range") {
    constexpr std::size_t count = 3;
    constexpr double xmin = 1, xmax = 3;
    
    auto stl_dist = std::discrete_distribution<Rep>(count, xmin, xmax, [](double val) { return val; });
    auto units_dist = units::discrete_distribution<Qty>(count, xmin, xmax, [](double val) { return val; });

    CHECK(units_dist.probabilities() == stl_dist.probabilities());
  }
}

TEST_CASE("piecewise_constant_distribution")
{
  using Rep = long double;
  using Qty = length<metre, Rep>;

  std::vector<Rep> intervals_rep_vec = {1.0, 2.0, 3.0};
  std::vector<Qty> intervals_qty_vec = {1.0q_m, 2.0q_m, 3.0q_m};

  SECTION("default")
  {
      auto stl_dist = std::piecewise_constant_distribution<Rep>();
      auto units_dist = units::piecewise_constant_distribution<Qty>();
      
      CHECK(units_dist.min() == Qty(stl_dist.min()));
      CHECK(units_dist.max() == Qty(stl_dist.max()));
      CHECK(stl_dist.intervals().size() == 2);
      CHECK(units_dist.intervals().size() == 2);
      CHECK(stl_dist.densities().size() == 1);
      CHECK(units_dist.densities().size() == 1);
  }
  
  SECTION ("parametrized_input_it") {
    constexpr std::array<Rep, 3> intervals_rep = {1.0, 2.0, 3.0};
    constexpr std::array<Qty, 3> intervals_qty = {1.0q_m, 2.0q_m, 3.0q_m};
    constexpr std::array<Rep, 3> weights = {1.0, 2.0, 3.0};
    
    auto stl_dist = std::piecewise_constant_distribution<Rep>(intervals_rep.cbegin(), intervals_rep.cend(), weights.cbegin());
    auto units_dist = units::piecewise_constant_distribution<Qty>(intervals_qty.cbegin(), intervals_qty.cend(), weights.cbegin());

    CHECK(stl_dist.intervals() == intervals_rep_vec);
    CHECK(units_dist.intervals() == intervals_qty_vec);
    CHECK(units_dist.densities() == stl_dist.densities());
  }

  SECTION ("parametrized_initializer_list") {
    std::initializer_list<Rep> intervals_rep = {1.0, 2.0, 3.0};
    std::initializer_list<Qty> intervals_qty = {1.0q_m, 2.0q_m, 3.0q_m};
    
    auto stl_dist = std::piecewise_constant_distribution<Rep>(intervals_rep, [](Rep val) { return val; });
    auto units_dist = units::piecewise_constant_distribution<Qty>(intervals_qty, [](Qty qty) { return qty.count(); });

    CHECK(units_dist.intervals() == intervals_qty_vec);
    CHECK(units_dist.densities() == stl_dist.densities());
  }

  SECTION ("parametrized_range") {
    constexpr std::size_t nw = 2;
    constexpr Rep xmin_rep = 1.0, xmax_rep = 3.0;
    constexpr Qty xmin_qty = 1.0q_m, xmax_qty = 3.0q_m;
    
    auto stl_dist = std::piecewise_constant_distribution<Rep>(nw, xmin_rep, xmax_rep, [](Rep val) { return val; });
    auto units_dist = units::piecewise_constant_distribution<Qty>(nw, xmin_qty, xmax_qty, [](Qty qty) { return qty.count(); });

    CHECK(units_dist.intervals() == intervals_qty_vec);
    CHECK(units_dist.densities() == stl_dist.densities());
  }
}

TEST_CASE("piecewise_linear_distribution")
{
  using Rep = long double;
  using Qty = length<metre, Rep>;

  std::vector<Rep> intervals_rep_vec = {1.0, 2.0, 3.0};
  std::vector<Qty> intervals_qty_vec = {1.0q_m, 2.0q_m, 3.0q_m};

  SECTION("default")
  {
      auto stl_dist = std::piecewise_linear_distribution<Rep>();
      auto units_dist = units::piecewise_linear_distribution<Qty>();
      
      CHECK(units_dist.min() == Qty(stl_dist.min()));
      CHECK(units_dist.max() == Qty(stl_dist.max()));
      CHECK(stl_dist.intervals().size() == 2);
      CHECK(units_dist.intervals().size() == 2);
      CHECK(stl_dist.densities().size() == 2);
      CHECK(units_dist.densities().size() == 2);
  }
  
  SECTION ("parametrized_input_it") {
    constexpr std::array<Rep, 3> intervals_rep = {1.0, 2.0, 3.0};
    constexpr std::array<Qty, 3> intervals_qty = {1.0q_m, 2.0q_m, 3.0q_m};
    constexpr std::array<Rep, 3> weights = {1.0, 2.0, 3.0};
    
    auto stl_dist = std::piecewise_linear_distribution<Rep>(intervals_rep.cbegin(), intervals_rep.cend(), weights.cbegin());
    auto units_dist = units::piecewise_linear_distribution<Qty>(intervals_qty.cbegin(), intervals_qty.cend(), weights.cbegin());

    CHECK(stl_dist.intervals() == intervals_rep_vec);
    CHECK(units_dist.intervals() == intervals_qty_vec);
    CHECK(units_dist.densities() == stl_dist.densities());
  }

  SECTION ("parametrized_initializer_list") {
    std::initializer_list<Rep> intervals_rep = {1.0, 2.0, 3.0};
    std::initializer_list<Qty> intervals_qty = {1.0q_m, 2.0q_m, 3.0q_m};
    
    auto stl_dist = std::piecewise_linear_distribution<Rep>(intervals_rep, [](Rep val) { return val; });
    auto units_dist = units::piecewise_linear_distribution<Qty>(intervals_qty, [](Qty qty) { return qty.count(); });

    CHECK(units_dist.intervals() == intervals_qty_vec);
    CHECK(units_dist.densities() == stl_dist.densities());
  }

  SECTION ("parametrized_range") {
    constexpr std::size_t nw = 2;
    constexpr Rep xmin_rep = 1.0, xmax_rep = 3.0;
    constexpr Qty xmin_qty = 1.0q_m, xmax_qty = 3.0q_m;
    
    auto stl_dist = std::piecewise_linear_distribution<Rep>(nw, xmin_rep, xmax_rep, [](Rep val) { return val; });
    auto units_dist = units::piecewise_linear_distribution<Qty>(nw, xmin_qty, xmax_qty, [](Qty qty) { return qty.count(); });

    CHECK(units_dist.intervals() == intervals_qty_vec);
    CHECK(units_dist.densities() == stl_dist.densities());
  }
}