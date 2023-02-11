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

#include <linear_algebra.hpp>
// linear_algebra.hpp has to be included first otherwise the header will fail to compile!
#include <catch2/catch_test_macros.hpp>
#include <mp_units/customization_points.h>
#include <mp_units/format.h>
#include <mp_units/iostream.h>
#include <mp_units/systems/isq/mechanics.h>
#include <mp_units/systems/isq/space_and_time.h>
#include <mp_units/systems/si/si.h>
#include <cmath>

namespace STD_LA {

template<class ET, class OT>
std::ostream& operator<<(std::ostream& os, const vector<ET, OT>& v)
{
  os << "|";
  for (auto i = 0U; i < v.size(); ++i) {
    os << STD_FMT::format(" {:>9}", v(i));
  }
  os << " |";
  return os;
}

template<class ET, class OT>
std::ostream& operator<<(std::ostream& os, const matrix<ET, OT>& v)
{
  for (auto i = 0U; i < v.rows(); ++i) {
    os << "|";
    for (auto j = 0U; j < v.columns(); ++j) {
      os << STD_FMT::format(" {:>9}", v(i, j));
    }
    os << (i != v.rows() - 1U ? " |\n" : " |");
  }
  return os;
}

}  // namespace STD_LA

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

template<typename Rep = double>
using vector = std::math::fs_vector<Rep, 3>;

template<typename Rep>
inline constexpr bool mp_units::is_vector<vector<Rep>> = true;

namespace {

template<typename T>
[[nodiscard]] auto get_magnitude(const vector<T>& v)
{
  return std::hypot(v[0], v[1], v[2]);
}

template<typename T, typename U>
[[nodiscard]] vector<decltype(T{} * U{})> cross_product(const vector<T>& a, const vector<U>& b)
{
  return {a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]};
}

template<Quantity Q1, Quantity Q2>
  requires is_vector<typename Q1::rep> && is_vector<typename Q2::rep> &&
           requires(typename Q1::rep v1, typename Q2::rep v2) { cross_product(v1, v2); }
[[nodiscard]] QuantityOf<Q1::reference * Q2::reference> auto cross_product(const Q1& q1, const Q2& q2)
{
  return (Q1::reference * Q2::reference)(cross_product(q1.number(), q2.number()));
}

}  // namespace

TEST_CASE("vector quantity", "[la]")
{
  SECTION("cast of unit")
  {
    SECTION("non-truncating")
    {
      const quantity<isq::position_vector[km], vector<int>> v{vector<int>{3, 2, 1}};
      CHECK(v[m].number() == vector<int>{3000, 2000, 1000});
    }

    SECTION("truncating")
    {
      const quantity<isq::position_vector[m], vector<int>> v{vector<int>{1001, 1002, 1003}};
      CHECK(value_cast<km>(v).number() == vector<int>{1, 1, 1});
    }
  }

  SECTION("to scalar magnitude")
  {
    const quantity<isq::velocity[km / h], vector<int>> v{vector<int>{2, 3, 6}};
    const auto speed = get_magnitude(v.number()) * isq::speed[v.unit];  // TODO can we do better here?
    CHECK(speed.number() == 7);
  }

  SECTION("multiply by scalar value")
  {
    const quantity<isq::position_vector[m], vector<int>> v{vector<int>{1, 2, 3}};

    SECTION("integral")
    {
      SECTION("scalar on LHS") { CHECK((2 * v).number() == vector<int>{2, 4, 6}); }
      SECTION("scalar on RHS") { CHECK((v * 2).number() == vector<int>{2, 4, 6}); }
    }

    SECTION("floating-point")
    {
      SECTION("scalar on LHS") { CHECK((0.5 * v).number() == vector<double>{0.5, 1., 1.5}); }
      SECTION("scalar on RHS") { CHECK((v * 0.5).number() == vector<double>{0.5, 1., 1.5}); }
    }
  }

  SECTION("divide by scalar value")
  {
    const quantity<isq::position_vector[m], vector<int>> v{vector<int>{2, 4, 6}};

    SECTION("integral") { CHECK((v / 2).number() == vector<int>{1, 2, 3}); }
    SECTION("floating-point") { CHECK((v / 0.5).number() == vector<double>{4., 8., 12.}); }
  }

  SECTION("add")
  {
    const quantity<isq::position_vector[m], vector<int>> v{vector<int>{1, 2, 3}};

    SECTION("same unit")
    {
      const quantity<isq::position_vector[m], vector<int>> u{vector<int>{3, 2, 1}};
      CHECK((v + u).number() == vector<int>{4, 4, 4});
    }
    SECTION("different units")
    {
      const quantity<isq::position_vector[km], vector<int>> u{vector<int>{3, 2, 1}};
      CHECK((v + u).number() == vector<int>{3001, 2002, 1003});
    }
  }

  SECTION("subtract")
  {
    const quantity<isq::position_vector[m], vector<int>> v{vector<int>{1, 2, 3}};

    SECTION("same unit")
    {
      const quantity<isq::position_vector[m], vector<int>> u{vector<int>{3, 2, 1}};
      CHECK((v - u).number() == vector<int>{-2, 0, 2});
    }
    SECTION("different units")
    {
      const quantity<isq::position_vector[km], vector<int>> u{vector<int>{3, 2, 1}};
      CHECK((v - u).number() == vector<int>{-2999, -1998, -997});
    }
  }

  SECTION("multiply by scalar quantity")
  {
    const quantity<isq::velocity[m / s], vector<int>> v{vector<int>{1, 2, 3}};

    SECTION("integral")
    {
      const auto mass = 2 * isq::mass[kg];

      SECTION("derived_quantity_spec")
      {
        SECTION("scalar on LHS") { CHECK((mass * v).number() == vector<int>{2, 4, 6}); }
        SECTION("scalar on RHS") { CHECK((v * mass).number() == vector<int>{2, 4, 6}); }
      }
      SECTION("quantity_cast to momentum")
      {
        SECTION("scalar on LHS") { CHECK(quantity_cast<isq::momentum>(mass * v).number() == vector<int>{2, 4, 6}); }
        SECTION("scalar on RHS") { CHECK(quantity_cast<isq::momentum>(v * mass).number() == vector<int>{2, 4, 6}); }
      }
      SECTION("quantity of momentum")
      {
        SECTION("scalar on LHS")
        {
          const quantity<isq::momentum[kg * m / s], vector<int>> momentum = mass * v;
          CHECK(momentum.number() == vector<int>{2, 4, 6});
        }
        SECTION("scalar on RHS")
        {
          const quantity<isq::momentum[kg * m / s], vector<int>> momentum = v * mass;
          CHECK(momentum.number() == vector<int>{2, 4, 6});
        }
      }
    }

    SECTION("floating-point")
    {
      const auto mass = 0.5 * isq::mass[kg];

      SECTION("derived_quantity_spec")
      {
        SECTION("scalar on LHS") { CHECK((mass * v).number() == vector<double>{0.5, 1., 1.5}); }
        SECTION("scalar on RHS") { CHECK((v * mass).number() == vector<double>{0.5, 1., 1.5}); }
      }
      SECTION("quantity_cast to momentum")
      {
        SECTION("scalar on LHS")
        {
          CHECK(quantity_cast<isq::momentum>(mass * v).number() == vector<double>{0.5, 1., 1.5});
        }
        SECTION("scalar on RHS")
        {
          CHECK(quantity_cast<isq::momentum>(v * mass).number() == vector<double>{0.5, 1., 1.5});
        }
      }
      SECTION("quantity of momentum")
      {
        SECTION("scalar on LHS")
        {
          const quantity<isq::momentum[kg * m / s], vector<double>> momentum = mass * v;
          CHECK(momentum.number() == vector<double>{0.5, 1., 1.5});
        }
        SECTION("scalar on RHS")
        {
          const quantity<isq::momentum[kg * m / s], vector<double>> momentum = v * mass;
          CHECK(momentum.number() == vector<double>{0.5, 1., 1.5});
        }
      }
    }
  }

  SECTION("divide by scalar quantity")
  {
    const quantity<isq::position_vector[km], vector<int>> pos{vector<int>{30, 20, 10}};

    SECTION("integral")
    {
      const auto dur = 2 * isq::duration[h];

      SECTION("derived_quantity_spec") { CHECK((pos / dur).number() == vector<int>{15, 10, 5}); }
      SECTION("quantity_cast to velocity")
      {
        CHECK(quantity_cast<isq::velocity>(pos / dur).number() == vector<int>{15, 10, 5});
      }
      SECTION("quantity of velocity")
      {
        const quantity<isq::velocity[km / h], vector<int>> v = pos / dur;
        CHECK(v.number() == vector<int>{15, 10, 5});
      }
    }

    SECTION("floating-point")
    {
      const auto dur = 0.5 * isq::duration[h];

      SECTION("derived_quantity_spec") { CHECK((pos / dur).number() == vector<double>{60, 40, 20}); }
      SECTION("quantity_cast to velocity")
      {
        CHECK(quantity_cast<isq::velocity>(pos / dur).number() == vector<double>{60, 40, 20});
      }
      SECTION("quantity of velocity")
      {
        const quantity<isq::velocity[km / h], vector<double>> v = pos / dur;
        CHECK(v.number() == vector<double>{60, 40, 20});
      }
    }
  }

  SECTION("cross product with a vector quantity")
  {
    const quantity<isq::position_vector[m], vector<int>> r{vector<int>{3, 0, 0}};
    const quantity<isq::force[N], vector<int>> f{vector<int>{0, 10, 0}};

    CHECK(cross_product(r, f) == isq::moment_of_force[N * m](vector<int>{0, 0, 30}));
  }
}
