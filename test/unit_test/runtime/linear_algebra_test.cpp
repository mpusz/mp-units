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
#include <mp-units/customization_points.h>
#include <mp-units/format.h>
#include <mp-units/math.h>
#include <mp-units/ostream.h>
#include <mp-units/systems/isq/mechanics.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si/si.h>
#include <cmath>
#include <matrix>

template<typename Rep = double>
using vector = STD_LA::fixed_size_column_vector<Rep, 3>;

template<class Rep>
inline constexpr bool mp_units::treat_as_floating_point<vector<Rep>> = mp_units::treat_as_floating_point<Rep>;

template<typename Rep>
inline constexpr bool mp_units::is_vector<vector<Rep>> = true;

template<typename Rep>
std::ostream& operator<<(std::ostream& os, const vector<Rep>& v)
{
  os << "|";
  for (auto i = 0U; i < v.size(); ++i) {
    os << MP_UNITS_STD_FMT::format(" {:>9}", v(i));
  }
  os << " |";
  return os;
}

namespace {

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

template<QuantitySpec auto QS, QuantityOf<QS> Q>
  requires(Q::quantity_spec.character == quantity_character::vector) && (QS.character == quantity_character::scalar)
[[nodiscard]] constexpr QuantityOf<QS> auto get_magnitude(const Q& q)
{
  const auto& v = q.numerical_value_ref_in(q.unit);
  return hypot(v(0) * QS[Q::unit], v(1) * QS[Q::unit], v(2) * QS[Q::unit]);
}

template<QuantitySpec auto QS, QuantityOf<QS> T>
  requires(T::quantity_spec.character == quantity_character::vector) && (QS.character == quantity_character::scalar)
[[nodiscard]] constexpr QuantityOf<QS> auto get_magnitude(const vector<T>& v)
{
  return hypot(QS(v(0)), QS(v(1)), QS(v(2)));
}

template<typename T, typename U>
[[nodiscard]] constexpr vector<decltype(T{} * U{})> cross_product(const vector<T>& a, const vector<U>& b)
{
  return {a(1) * b(2) - a(2) * b(1), a(2) * b(0) - a(0) * b(2), a(0) * b(1) - a(1) * b(0)};
}

template<Quantity Q1, Quantity Q2>
  requires is_vector<typename Q1::rep> && is_vector<typename Q2::rep> &&
           requires(typename Q1::rep v1, typename Q2::rep v2) { cross_product(v1, v2); }
[[nodiscard]] constexpr QuantityOf<Q1::quantity_spec * Q2::quantity_spec> auto cross_product(const Q1& q1, const Q2& q2)
{
  return cross_product(q1.numerical_value_ref_in(q1.unit), q2.numerical_value_ref_in(q2.unit)) *
         (Q1::reference * Q2::reference);
}

}  // namespace

TEST_CASE("vector quantity", "[la]")
{
  SECTION("cast of unit")
  {
    SECTION("non-truncating")
    {
      const auto v = vector<int>{3, 2, 1} * isq::position_vector[km];
      CHECK(v.numerical_value_in(m) == vector<int>{3000, 2000, 1000});
    }

    SECTION("truncating")
    {
      const auto v = vector<int>{1001, 1002, 1003} * isq::position_vector[m];
      CHECK(v.force_numerical_value_in(km) == vector<int>{1, 1, 1});
    }
  }

  SECTION("to scalar magnitude")
  {
    const auto v = vector<int>{2, 3, 6} * isq::velocity[km / h];
    const auto speed = get_magnitude<isq::speed>(v);
    CHECK(speed.numerical_value_ref_in(km / h) == 7);
  }

  SECTION("multiply by scalar value")
  {
    const auto v = vector<int>{1, 2, 3} * isq::position_vector[m];

    SECTION("integral")
    {
      SECTION("scalar on LHS") { CHECK((2 * v).numerical_value_in(m) == vector<int>{2, 4, 6}); }
      SECTION("scalar on RHS") { CHECK((v * 2).numerical_value_in(m) == vector<int>{2, 4, 6}); }
    }

    SECTION("floating-point")
    {
      SECTION("scalar on LHS") { CHECK((0.5 * v).numerical_value_in(m) == vector<double>{0.5, 1., 1.5}); }
      SECTION("scalar on RHS") { CHECK((v * 0.5).numerical_value_in(m) == vector<double>{0.5, 1., 1.5}); }
    }
  }

  SECTION("divide by scalar value")
  {
    const auto v = vector<int>{2, 4, 6} * isq::position_vector[m];

    SECTION("integral") { CHECK((v / 2).numerical_value_in(m) == vector<int>{1, 2, 3}); }
    SECTION("floating-point") { CHECK((v / 0.5).numerical_value_in(m) == vector<double>{4., 8., 12.}); }
  }

  SECTION("add")
  {
    const auto v = vector<int>{1, 2, 3} * isq::position_vector[m];

    SECTION("same unit")
    {
      const auto u = vector<int>{3, 2, 1} * isq::position_vector[m];
      CHECK((v + u).numerical_value_in(m) == vector<int>{4, 4, 4});
    }
    SECTION("different units")
    {
      const auto u = vector<int>{3, 2, 1} * isq::position_vector[km];
      CHECK((v + u).numerical_value_in(m) == vector<int>{3001, 2002, 1003});
    }
  }

  SECTION("subtract")
  {
    const auto v = vector<int>{1, 2, 3} * isq::position_vector[m];

    SECTION("same unit")
    {
      const auto u = vector<int>{3, 2, 1} * isq::position_vector[m];
      CHECK((v - u).numerical_value_in(m) == vector<int>{-2, 0, 2});
    }
    SECTION("different units")
    {
      const auto u = vector<int>{3, 2, 1} * isq::position_vector[km];
      CHECK((v - u).numerical_value_in(m) == vector<int>{-2999, -1998, -997});
    }
  }

  SECTION("multiply by scalar quantity")
  {
    const auto v = vector<int>{1, 2, 3} * isq::velocity[m / s];

    SECTION("integral")
    {
      const auto mass = 2 * isq::mass[kg];

      SECTION("derived_quantity_spec")
      {
        SECTION("scalar on LHS") { CHECK((mass * v).numerical_value_in(kg * m / s) == vector<int>{2, 4, 6}); }
        SECTION("scalar on RHS") { CHECK((v * mass).numerical_value_in(kg * m / s) == vector<int>{2, 4, 6}); }
      }
      SECTION("quantity_cast to momentum")
      {
        SECTION("scalar on LHS")
        {
          CHECK(quantity_cast<isq::momentum>(mass * v).numerical_value_in(N * s) == vector<int>{2, 4, 6});
        }
        SECTION("scalar on RHS")
        {
          CHECK(quantity_cast<isq::momentum>(v * mass).numerical_value_in(N * s) == vector<int>{2, 4, 6});
        }
      }
      SECTION("quantity of momentum")
      {
        SECTION("scalar on LHS")
        {
          const quantity<isq::momentum[N * s], vector<int>> momentum = mass * v;
          CHECK(momentum.numerical_value_ref_in(N * s) == vector<int>{2, 4, 6});
        }
        SECTION("scalar on RHS")
        {
          const quantity<isq::momentum[N * s], vector<int>> momentum = v * mass;
          CHECK(momentum.numerical_value_ref_in(N * s) == vector<int>{2, 4, 6});
        }
      }
    }

    SECTION("floating-point")
    {
      const auto mass = 0.5 * isq::mass[kg];

      SECTION("derived_quantity_spec")
      {
        SECTION("scalar on LHS") { CHECK((mass * v).numerical_value_in(kg * m / s) == vector<double>{0.5, 1., 1.5}); }
        SECTION("scalar on RHS") { CHECK((v * mass).numerical_value_in(kg * m / s) == vector<double>{0.5, 1., 1.5}); }
      }
      SECTION("quantity_cast to momentum")
      {
        SECTION("scalar on LHS")
        {
          CHECK(quantity_cast<isq::momentum>(mass * v).numerical_value_in(N * s) == vector<double>{0.5, 1., 1.5});
        }
        SECTION("scalar on RHS")
        {
          CHECK(quantity_cast<isq::momentum>(v * mass).numerical_value_in(N * s) == vector<double>{0.5, 1., 1.5});
        }
      }
      SECTION("quantity of momentum")
      {
        SECTION("scalar on LHS")
        {
          const quantity<isq::momentum[N * s], vector<double>> momentum = mass * v;
          CHECK(momentum.numerical_value_ref_in(N * s) == vector<double>{0.5, 1., 1.5});
        }
        SECTION("scalar on RHS")
        {
          const quantity<isq::momentum[N * s], vector<double>> momentum = v * mass;
          CHECK(momentum.numerical_value_ref_in(N * s) == vector<double>{0.5, 1., 1.5});
        }
      }
    }
  }

  SECTION("divide by scalar quantity")
  {
    const auto pos = vector<int>{30, 20, 10} * isq::position_vector[km];

    SECTION("integral")
    {
      const auto dur = 2 * isq::duration[h];

      SECTION("derived_quantity_spec") { CHECK((pos / dur).numerical_value_in(km / h) == vector<int>{15, 10, 5}); }
      SECTION("quantity_cast to velocity")
      {
        CHECK(quantity_cast<isq::velocity>(pos / dur).numerical_value_in(km / h) == vector<int>{15, 10, 5});
      }
      SECTION("quantity of velocity")
      {
        const quantity<isq::velocity[km / h], vector<int>> v = pos / dur;
        CHECK(v.numerical_value_ref_in(km / h) == vector<int>{15, 10, 5});
      }
    }

    SECTION("floating-point")
    {
      const auto dur = 0.5 * isq::duration[h];

      SECTION("derived_quantity_spec") { CHECK((pos / dur).numerical_value_in(km / h) == vector<double>{60, 40, 20}); }
      SECTION("quantity_cast to velocity")
      {
        CHECK(quantity_cast<isq::velocity>(pos / dur).numerical_value_in(km / h) == vector<double>{60, 40, 20});
      }
      SECTION("quantity of velocity")
      {
        const quantity<isq::velocity[km / h], vector<double>> v = pos / dur;
        CHECK(v.numerical_value_ref_in(km / h) == vector<double>{60, 40, 20});
      }
    }
  }

  SECTION("cross product with a vector quantity")
  {
    const auto r = vector<int>{3, 0, 0} * isq::position_vector[m];
    const auto f = vector<int>{0, 10, 0} * isq::force[N];

    CHECK(cross_product(r, f) == vector<int>{0, 0, 30} * isq::moment_of_force[N * m]);
  }
}

template<class T>
  requires mp_units::is_scalar<T>
inline constexpr bool mp_units::is_vector<T> = true;

TEST_CASE("vector of quantities", "[la]")
{
  SECTION("cast of unit")
  {
    SECTION("non-truncating")
    {
      const vector<quantity<isq::position_vector[km], int>> v = {3 * km, 2 * km, 1 * km};

      CHECK(vector<quantity<isq::position_vector[m], int>>(v) ==
            vector<quantity<isq::position_vector[m], int>>{3000 * m, 2000 * m, 1000 * m});
    }

    // truncating not possible (no way to apply quantity_cast to sub-components of a vector)
  }

  SECTION("to scalar magnitude")
  {
    const vector<quantity<isq::velocity[km / h], int>> v = {2 * km / h, 3 * km / h, 6 * km / h};
    const auto speed = get_magnitude<isq::speed>(v);
    CHECK(speed.numerical_value_ref_in(km / h) == 7);
  }

  SECTION("multiply by scalar value")
  {
    const vector<quantity<isq::position_vector[m], int>> v = {1 * m, 2 * m, 3 * m};

    SECTION("integral")
    {
      const vector<quantity<isq::position_vector[m], int>> result = {2 * m, 4 * m, 6 * m};

      SECTION("scalar on LHS") { CHECK(2 * v == result); }
      SECTION("scalar on RHS") { CHECK(v * 2 == result); }
    }

    SECTION("floating-point")
    {
      const vector<quantity<isq::position_vector[m], double>> result = {0.5 * m, 1. * m, 1.5 * m};

      SECTION("scalar on LHS") { CHECK(0.5 * v == result); }
      SECTION("scalar on RHS") { CHECK(v * 0.5 == result); }
    }
  }

  SECTION("divide by scalar value")
  {
    const vector<quantity<isq::position_vector[m], int>> v = {2 * m, 4 * m, 6 * m};

    SECTION("integral") { CHECK(v / 2 == vector<quantity<isq::position_vector[m], int>>{1 * m, 2 * m, 3 * m}); }
    SECTION("floating-point")
    {
      CHECK(v / 0.5 == vector<quantity<isq::position_vector[m], double>>{4. * m, 8. * m, 12. * m});
    }
  }

  SECTION("add")
  {
    const vector<quantity<isq::position_vector[m], int>> v = {1 * m, 2 * m, 3 * m};

    SECTION("same unit")
    {
      const vector<quantity<isq::position_vector[m], int>> u = {3 * m, 2 * m, 1 * m};

      CHECK(v + u == vector<quantity<isq::position_vector[m], int>>{4 * m, 4 * m, 4 * m});
    }
    SECTION("different units")
    {
      const vector<quantity<isq::position_vector[km], int>> u = {3 * km, 2 * km, 1 * km};

      CHECK(v + u == vector<quantity<isq::position_vector[m], int>>{3001 * m, 2002 * m, 1003 * m});
    }
  }

  SECTION("subtract")
  {
    const vector<quantity<isq::position_vector[m], int>> v = {1 * m, 2 * m, 3 * m};

    SECTION("same unit")
    {
      const vector<quantity<isq::position_vector[m], int>> u = {3 * m, 2 * m, 1 * m};
      CHECK(v - u == vector<quantity<isq::position_vector[m], int>>{-2 * m, 0 * m, 2 * m});
    }
    SECTION("different units")
    {
      const vector<quantity<isq::position_vector[km], int>> u = {3 * km, 2 * km, 1 * km};
      CHECK(v - u == vector<quantity<isq::position_vector[m], int>>{-2999 * m, -1998 * m, -997 * m});
    }
  }

  SECTION("multiply by scalar quantity")
  {
    const vector<quantity<isq::velocity[m / s], int>> v = {1 * m / s, 2 * m / s, 3 * m / s};

    SECTION("integral")
    {
      const auto mass = 2 * isq::mass[kg];
      const auto result = vector<quantity<isq::momentum[N * s], int>>{2 * N * s, 4 * N * s, 6 * N * s};

      SECTION("derived_quantity_spec")
      {
        SECTION("scalar on LHS") { CHECK(mass * v == result); }
        SECTION("scalar on RHS") { CHECK(v * mass == result); }
      }

      // no way to apply quantity_cast to sub-components

      SECTION("quantity of momentum")
      {
        SECTION("scalar on LHS")
        {
          const vector<quantity<isq::momentum[N * s], int>> momentum = mass * v;
          CHECK(momentum == result);
        }
        SECTION("scalar on RHS")
        {
          const vector<quantity<isq::momentum[N * s], int>> momentum = v * mass;
          CHECK(momentum == result);
        }
      }
    }

    SECTION("floating-point")
    {
      const auto mass = 0.5 * isq::mass[kg];
      const auto result = vector<quantity<isq::momentum[N * s], double>>{0.5 * N * s, 1. * N * s, 1.5 * N * s};

      SECTION("derived_quantity_spec")
      {
        SECTION("scalar on LHS") { CHECK(mass * v == result); }
        SECTION("scalar on RHS") { CHECK(v * mass == result); }
      }

      // no way to apply quantity_cast to sub-components

      SECTION("quantity of momentum")
      {
        SECTION("scalar on LHS")
        {
          const vector<quantity<isq::momentum[N * s], double>> momentum = mass * v;
          CHECK(momentum == result);
        }
        SECTION("scalar on RHS")
        {
          const vector<quantity<isq::momentum[N * s], double>> momentum = v * mass;
          CHECK(momentum == result);
        }
      }
    }
  }

  SECTION("divide by scalar quantity")
  {
    const vector<quantity<isq::position_vector[km], int>> pos = {30 * km, 20 * km, 10 * km};

    SECTION("integral")
    {
      const auto dur = 2 * isq::duration[h];

      SECTION("derived_quantity_spec")
      {
        CHECK(pos / dur == vector<quantity<isq::velocity[km / h], int>>{15 * km / h, 10 * km / h, 5 * km / h});
      }

      // no way to apply quantity_cast to sub-components

      SECTION("quantity of velocity")
      {
        const vector<quantity<isq::velocity[km / h], int>> v = pos / dur;
        CHECK(v == vector<quantity<isq::velocity[km / h], int>>{15 * km / h, 10 * km / h, 5 * km / h});
      }
    }

    SECTION("floating-point")
    {
      const auto dur = 0.5 * isq::duration[h];

      SECTION("derived_quantity_spec")
      {
        CHECK(pos / dur == vector<quantity<isq::velocity[km / h], double>>{60. * km / h, 40. * km / h, 20. * km / h});
      }

      // no way to apply quantity_cast to sub-components

      SECTION("quantity of velocity")
      {
        const vector<quantity<isq::velocity[km / h], double>> v = pos / dur;
        CHECK(v == vector<quantity<isq::velocity[km / h], double>>{60. * km / h, 40. * km / h, 20. * km / h});
      }
    }
  }

  SECTION("cross product with a vector of quantities")
  {
    const vector<quantity<isq::position_vector[m], int>> r = {3 * m, 0 * m, 0 * m};
    const vector<quantity<isq::force[N], int>> f = {0 * N, 10 * N, 0 * N};

    CHECK(cross_product(r, f) == vector<quantity<isq::moment_of_force[N * m], int>>{0 * N * m, 0 * N * m, 30 * N * m});
  }
}
