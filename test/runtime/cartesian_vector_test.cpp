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
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <mp-units/compat_macros.h>
#include <mp-units/ext/format.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <complex>
#include <sstream>
#endif
#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/cartesian_vector.h>
#endif

using namespace mp_units;
using namespace Catch::Matchers;
using namespace std::complex_literals;

// element-type conversions follow the library's non-truncating rule (like a quantity rep), not the
// language narrowing rule: a floating-point target or a widening is implicit, while a
// floating-point -> integer element conversion is explicit (truncating). Same-field conversions
// (incl. integer narrowing) stay implicit, matching `implicitly_scalable` with no unit scaling.
static_assert(std::convertible_to<cartesian_vector<float>, cartesian_vector<double>>);    // widen: implicit
static_assert(std::convertible_to<cartesian_vector<double>, cartesian_vector<float>>);    // FP target: implicit
static_assert(std::convertible_to<cartesian_vector<int>, cartesian_vector<double>>);      // int->FP: implicit
static_assert(std::convertible_to<cartesian_vector<int>, cartesian_vector<long long>>);   // int->int: implicit
static_assert(!std::convertible_to<cartesian_vector<double>, cartesian_vector<int>>);     // FP->int: explicit
static_assert(std::constructible_from<cartesian_vector<int>, cartesian_vector<double>>);  // ...but constructible

// the per-axis dimension is queryable at compile time, both as a value and as a call
static_assert(cartesian_vector<double>::extent == 3);  // default N
static_assert(cartesian_vector<double, 2>::extent == 2);
static_assert(cartesian_vector<double, 3>::extent() == 3);

// operations close at a single dimension N: mixing 2D and 3D is ill-formed (no implicit padding).
// Negative tests via named helper concepts asserted with static_assert (the `invalid_types` idiom).
namespace {
template<typename A, typename B>
concept addable = requires(A a, B b) { a + b; };
template<typename A, typename B>
concept subtractable = requires(A a, B b) { a - b; };
template<typename A, typename B>
concept equality_comparable = requires(A a, B b) { a == b; };
template<typename A, typename B>
concept scalar_producible = requires(A a, B b) { scalar_product(a, b); };
template<typename A, typename B>
concept vector_producible = requires(A a, B b) { vector_product(a, b); };
}  // namespace
static_assert(!addable<cartesian_vector<double, 2>, cartesian_vector<double, 3>>);
static_assert(!subtractable<cartesian_vector<double, 2>, cartesian_vector<double, 3>>);
static_assert(!equality_comparable<cartesian_vector<double, 2>, cartesian_vector<double, 3>>);
static_assert(!scalar_producible<cartesian_vector<double, 2>, cartesian_vector<double, 3>>);
static_assert(!vector_producible<cartesian_vector<double, 2>, cartesian_vector<double, 3>>);
static_assert(!std::constructible_from<cartesian_vector<double, 3>, cartesian_vector<double, 2>>);

TEST_CASE("cartesian_vector operations", "[vector]")
{
  SECTION("cartesian_vector initialization and access")
  {
    SECTION("no arguments")
    {
      cartesian_vector<double> v;
      REQUIRE(v[0] == 0);
      REQUIRE(v[1] == 0);
      REQUIRE(v[2] == 0);
    }

    SECTION("zero arguments")
    {
      cartesian_vector<double> v{};
      REQUIRE(v[0] == 0);
      REQUIRE(v[1] == 0);
      REQUIRE(v[2] == 0);
    }

    SECTION("one argument")
    {
      cartesian_vector<double> v{1.0};  // explicit 3D: a single component cannot deduce a vector
      REQUIRE(v[0] == 1.0);
      REQUIRE(v[1] == 0);
      REQUIRE(v[2] == 0);
    }

    SECTION("two arguments")
    {
      cartesian_vector<double> v{1.0, 2.0};  // explicit 3D, third component zero-padded
      REQUIRE(v[0] == 1.0);
      REQUIRE(v[1] == 2.0);
      REQUIRE(v[2] == 0);
    }

    SECTION("deduced dimension")
    {
      cartesian_vector v2{1.0, 2.0};  // two components -> 2D
      static_assert(std::is_same_v<decltype(v2), cartesian_vector<double, 2>>);
      REQUIRE(v2[0] == 1.0);
      REQUIRE(v2[1] == 2.0);

      cartesian_vector v3{1.0, 2.0, 3.0};  // three components -> 3D
      static_assert(std::is_same_v<decltype(v3), cartesian_vector<double, 3>>);
    }

    SECTION("all arguments")
    {
      cartesian_vector v{1.0, 2.0, 3.0};
      REQUIRE(v[0] == 1.0);
      REQUIRE(v[1] == 2.0);
      REQUIRE(v[2] == 3.0);
    }

    SECTION("convertible arguments")
    {
      cartesian_vector<double> v{1, 2, 3};
      REQUIRE(v[0] == 1.0);
      REQUIRE(v[1] == 2.0);
      REQUIRE(v[2] == 3.0);
    }
  }

  SECTION("convertibility from another vector")
  {
    cartesian_vector v1{1, 2, 3};

    SECTION("construction")
    {
      cartesian_vector<double> v2 = v1;
      REQUIRE(v2[0] == 1.0);
      REQUIRE(v2[1] == 2.0);
      REQUIRE(v2[2] == 3.0);
    }

    SECTION("assignment")
    {
      cartesian_vector<double> v2{3.0, 2.0, 1.0};
      v2 = v1;
      REQUIRE(v2[0] == 1.0);
      REQUIRE(v2[1] == 2.0);
      REQUIRE(v2[2] == 3.0);
    }
  }

  SECTION("cartesian_vector compound assignment addition")
  {
    cartesian_vector v1{1.0, 2.0, 3.0};
    cartesian_vector v2{4.0, 5.0, 6.0};
    v1 += v2;
    REQUIRE(v1[0] == 5.0);
    REQUIRE(v1[1] == 7.0);
    REQUIRE(v1[2] == 9.0);
  }

  SECTION("cartesian_vector compound assignment subtraction")
  {
    cartesian_vector v1{4.0, 5.0, 6.0};
    cartesian_vector v2{1.0, 2.0, 3.0};
    v1 -= v2;
    REQUIRE(v1[0] == 3.0);
    REQUIRE(v1[1] == 3.0);
    REQUIRE(v1[2] == 3.0);
  }

  SECTION("cartesian_vector compound assignment scalar multiplication")
  {
    cartesian_vector v{1.0, 2.0, 3.0};
    v *= 2.0;
    REQUIRE(v[0] == 2.0);
    REQUIRE(v[1] == 4.0);
    REQUIRE(v[2] == 6.0);
  }

  SECTION("cartesian_vector compound assignment scalar division")
  {
    cartesian_vector v{2.0, 4.0, 6.0};
    v /= 2.0;
    REQUIRE(v[0] == 1.0);
    REQUIRE(v[1] == 2.0);
    REQUIRE(v[2] == 3.0);
  }

  SECTION("cartesian_vector addition")
  {
    SECTION("double + double")
    {
      cartesian_vector v1{1.0, 2.0, 3.0};
      cartesian_vector v2{4.0, 5.0, 6.0};
      cartesian_vector result = v1 + v2;
      REQUIRE(result[0] == 5.0);
      REQUIRE(result[1] == 7.0);
      REQUIRE(result[2] == 9.0);
    }

    SECTION("double + int")
    {
      cartesian_vector v1{1.0, 2.0, 3.0};
      cartesian_vector v2{4, 5, 6};
      cartesian_vector result = v1 + v2;
      REQUIRE(result[0] == 5.0);
      REQUIRE(result[1] == 7.0);
      REQUIRE(result[2] == 9.0);
    }

    SECTION("int + double")
    {
      cartesian_vector v1{1, 2, 3};
      cartesian_vector v2{4.0, 5.0, 6.0};
      cartesian_vector result = v1 + v2;
      REQUIRE(result[0] == 5.0);
      REQUIRE(result[1] == 7.0);
      REQUIRE(result[2] == 9.0);
    }

    SECTION("int + int")
    {
      cartesian_vector v1{1, 2, 3};
      cartesian_vector v2{4, 5, 6};
      cartesian_vector result = v1 + v2;
      REQUIRE(result[0] == 5);
      REQUIRE(result[1] == 7);
      REQUIRE(result[2] == 9);
    }
  }

  SECTION("cartesian_vector subtraction")
  {
    SECTION("double - double")
    {
      cartesian_vector v1{4.0, 5.0, 6.0};
      cartesian_vector v2{1.0, 2.0, 3.0};
      cartesian_vector result = v1 - v2;
      REQUIRE(result[0] == 3.0);
      REQUIRE(result[1] == 3.0);
      REQUIRE(result[2] == 3.0);
    }

    SECTION("double - int")
    {
      cartesian_vector v1{4.0, 5.0, 6.0};
      cartesian_vector v2{1, 2, 3};
      cartesian_vector result = v1 - v2;
      REQUIRE(result[0] == 3.0);
      REQUIRE(result[1] == 3.0);
      REQUIRE(result[2] == 3.0);
    }

    SECTION("int - double")
    {
      cartesian_vector v1{4, 5, 6};
      cartesian_vector v2{1.0, 2.0, 3.0};
      cartesian_vector result = v1 - v2;
      REQUIRE(result[0] == 3.0);
      REQUIRE(result[1] == 3.0);
      REQUIRE(result[2] == 3.0);
    }

    SECTION("int - int")
    {
      cartesian_vector v1{4, 5, 6};
      cartesian_vector v2{1, 2, 3};
      cartesian_vector result = v1 - v2;
      REQUIRE(result[0] == 3);
      REQUIRE(result[1] == 3);
      REQUIRE(result[2] == 3);
    }
  }

  SECTION("cartesian_vector scalar multiplication")
  {
    SECTION("double * double")
    {
      cartesian_vector v{1.0, 2.0, 3.0};
      cartesian_vector result = v * 2.0;
      REQUIRE(result[0] == 2.0);
      REQUIRE(result[1] == 4.0);
      REQUIRE(result[2] == 6.0);
    }

    SECTION("double * int")
    {
      cartesian_vector v{1.0, 2.0, 3.0};
      cartesian_vector result = v * 2;
      REQUIRE(result[0] == 2.0);
      REQUIRE(result[1] == 4.0);
      REQUIRE(result[2] == 6.0);
    }

    SECTION("int * double")
    {
      cartesian_vector v{1, 2, 3};
      cartesian_vector result = v * 2.0;
      REQUIRE(result[0] == 2.0);
      REQUIRE(result[1] == 4.0);
      REQUIRE(result[2] == 6.0);
    }

    SECTION("int * int")
    {
      cartesian_vector v{1, 2, 3};
      cartesian_vector result = v * 2;
      REQUIRE(result[0] == 2);
      REQUIRE(result[1] == 4);
      REQUIRE(result[2] == 6);
    }
  }

  SECTION("cartesian_vector scalar division")
  {
    SECTION("double / double")
    {
      cartesian_vector v{2.0, 4.0, 6.0};
      cartesian_vector result = v / 2.0;
      REQUIRE(result[0] == 1.0);
      REQUIRE(result[1] == 2.0);
      REQUIRE(result[2] == 3.0);
    }

    SECTION("double / int")
    {
      cartesian_vector v{2.0, 4.0, 6.0};
      cartesian_vector result = v / 2;
      REQUIRE(result[0] == 1.0);
      REQUIRE(result[1] == 2.0);
      REQUIRE(result[2] == 3.0);
    }

    SECTION("int / double")
    {
      cartesian_vector v{2, 4, 6};
      cartesian_vector result = v / 2.0;
      REQUIRE(result[0] == 1.0);
      REQUIRE(result[1] == 2.0);
      REQUIRE(result[2] == 3.0);
    }

    SECTION("int / int")
    {
      cartesian_vector v{2, 4, 6};
      cartesian_vector result = v / 2;
      REQUIRE(result[0] == 1);
      REQUIRE(result[1] == 2);
      REQUIRE(result[2] == 3);
    }
  }

  SECTION("cartesian_vector norm")
  {
    cartesian_vector v1{3.0, 4.0, 0.0};
    cartesian_vector v2{2.0, 3.0, 6.0};
    REQUIRE(v1.norm() == 5.0);
    REQUIRE(v2.norm() == 7.0);
    REQUIRE(norm(v1) == 5.0);
    REQUIRE(norm(v2) == 7.0);
  }

  SECTION("cartesian_vector magnitude")
  {
    cartesian_vector v1{3.0, 4.0, 0.0};
    cartesian_vector v2{2.0, 3.0, 6.0};
    REQUIRE(v1.magnitude() == 5.0);
    REQUIRE(v2.magnitude() == 7.0);
    REQUIRE(magnitude(v1) == 5.0);
    REQUIRE(magnitude(v2) == 7.0);
  }

  SECTION("cartesian_vector unit vector")
  {
    cartesian_vector v{3.0, 4.0, 0.0};
    cartesian_vector unit_v = v.unit();
    REQUIRE_THAT(unit_v.norm(), WithinULP(1.0, 1));
    REQUIRE_THAT(unit_v.magnitude(), WithinULP(1.0, 1));
    REQUIRE_THAT(unit_v[0], WithinULP(3.0 / 5.0, 1));
    REQUIRE_THAT(unit_v[1], WithinULP(4.0 / 5.0, 1));
    REQUIRE_THAT(unit_v[2], WithinULP(0.0, 1));

    REQUIRE(unit_vector(v) == unit_v);
  }

  SECTION("cartesian_vector equality")
  {
    cartesian_vector v1{1.0, 2.0, 3.0};
    cartesian_vector v2{1, 2, 3};
    cartesian_vector v3{1.1, 2.0, 3.0};
    cartesian_vector v4{1.0, 2.1, 3.0};
    cartesian_vector v5{1.0, 2.0, 3.1};
    REQUIRE(v1 == v2);
    REQUIRE(v1 != v3);
    REQUIRE(v1 != v4);
    REQUIRE(v1 != v5);
  }

  SECTION("cartesian_vector scalar product")
  {
    SECTION("double * double")
    {
      cartesian_vector v1{1.0, 2.0, 3.0};
      cartesian_vector v2{4.0, 5.0, 6.0};
      REQUIRE(scalar_product(v1, v2) == 32.0);
    }

    SECTION("double * int")
    {
      cartesian_vector v1{1.0, 2.0, 3.0};
      cartesian_vector v2{4, 5, 6};
      REQUIRE(scalar_product(v1, v2) == 32.0);
    }

    SECTION("int * double")
    {
      cartesian_vector v1{1, 2, 3};
      cartesian_vector v2{4.0, 5.0, 6.0};
      REQUIRE(scalar_product(v1, v2) == 32.0);
    }

    SECTION("int * int")
    {
      cartesian_vector v1{1, 2, 3};
      cartesian_vector v2{4, 5, 6};
      REQUIRE(scalar_product(v1, v2) == 32);
    }
  }

  SECTION("cartesian_vector vector product")
  {
    SECTION("double * double")
    {
      cartesian_vector v1{1.0, 2.0, 3.0};
      cartesian_vector v2{4.0, 5.0, 6.0};
      cartesian_vector result = vector_product(v1, v2);
      REQUIRE(result[0] == -3.0);
      REQUIRE(result[1] == 6.0);
      REQUIRE(result[2] == -3.0);
    }

    SECTION("double * int")
    {
      cartesian_vector v1{1.0, 2.0, 3.0};
      cartesian_vector v2{4, 5, 6};
      cartesian_vector result = vector_product(v1, v2);
      REQUIRE(result[0] == -3.0);
      REQUIRE(result[1] == 6.0);
      REQUIRE(result[2] == -3.0);
    }

    SECTION("int * double")
    {
      cartesian_vector v1{1, 2, 3};
      cartesian_vector v2{4.0, 5.0, 6.0};
      cartesian_vector result = vector_product(v1, v2);
      REQUIRE(result[0] == -3.0);
      REQUIRE(result[1] == 6.0);
      REQUIRE(result[2] == -3.0);
    }

    SECTION("int * int")
    {
      cartesian_vector v1{1, 2, 3};
      cartesian_vector v2{4, 5, 6};
      cartesian_vector result = vector_product(v1, v2);
      REQUIRE(result[0] == -3);
      REQUIRE(result[1] == 6);
      REQUIRE(result[2] == -3);
    }

    SECTION("two dimensions yields the perp-dot scalar")
    {
      cartesian_vector v1{1.0, 2.0};
      cartesian_vector v2{4.0, 5.0};
      auto result = vector_product(v1, v2);  // perp-dot: v1[0]*v2[1] - v1[1]*v2[0]
      static_assert(std::is_same_v<decltype(result), double>);
      REQUIRE(result == -3.0);
    }
  }
}

TEST_CASE("cartesian_vector text output", "[vector][fmt][ostream]")
{
  std::ostringstream os;

  SECTION("integral representation")
  {
    cartesian_vector v{1, 2, 3};
    os << v;

    SECTION("iostream") { CHECK(os.str() == "[1, 2, 3]"); }
    SECTION("fmt with default format {}") { CHECK(MP_UNITS_STD_FMT::format("{}", v) == os.str()); }
  }

  SECTION("floating-point representation")
  {
    cartesian_vector v{1.2, 2.3, 3.4};
    os << v;

    SECTION("iostream") { CHECK(os.str() == "[1.2, 2.3, 3.4]"); }
    SECTION("fmt with default format {}") { CHECK(MP_UNITS_STD_FMT::format("{}", v) == os.str()); }
  }
}

TEST_CASE("cartesian_vector with a complex representation", "[vector][complex]")
{
  using c = std::complex<double>;

  SECTION("Hermitian magnitude is a real scalar")
  {
    // |(3+4i, 0, 0)| = |3+4i| = 5
    cartesian_vector v{3. + 4.i, c{}, c{}};
    STATIC_CHECK(std::is_same_v<decltype(v.magnitude()), double>);
    REQUIRE_THAT(v.magnitude(), WithinRel(5.0, 1e-12));
    // |(1+i, 1+i, 1+i)| = sqrt(3 * 2) = sqrt(6)
    cartesian_vector w{1. + 1.i, 1. + 1.i, 1. + 1.i};
    REQUIRE_THAT(w.magnitude(), WithinRel(std::sqrt(6.0), 1e-12));
  }

  SECTION("scalar_product is sesquilinear (conjugates the first argument)")
  {
    // <a, a> = |a|^2 is real and non-negative
    cartesian_vector a{1. + 1.i, c{}, c{}};
    REQUIRE(scalar_product(a, a) == c{2.0, 0.0});

    // <(1,0,0), (i,0,0)> = conj(1) * i = i
    cartesian_vector e1{c{1.0}, c{}, c{}};
    cartesian_vector ei{1.i, c{}, c{}};
    REQUIRE(scalar_product(e1, ei) == 1.i);
    // anti-Hermitian symmetry: <b, a> = conj(<a, b>)
    REQUIRE(scalar_product(ei, e1) == std::conj(scalar_product(e1, ei)));
  }

  SECTION("addition and scaling work componentwise")
  {
    cartesian_vector a{1. + 1.i, 2. + 0.i, 0. + 3.i};
    cartesian_vector b{1. - 1.i, 0. + 2.i, 3. + 0.i};
    auto s = a + b;
    REQUIRE(s[0] == c{2.0, 0.0});
    auto t = a * c{2.0, 0.0};
    REQUIRE(t[0] == c{2.0, 2.0});
  }
}

namespace {
template<typename V>
concept embeddable = requires(V v) { embed(v); };
template<typename V>
concept projectable = requires(V v) { project(v); };
}  // namespace
// embed only lifts 2D->3D and project only lowers 3D->2D (each defined for one source dimension)
static_assert(embeddable<cartesian_vector<double, 2>> && !embeddable<cartesian_vector<double, 3>>);
static_assert(projectable<cartesian_vector<double, 3>> && !projectable<cartesian_vector<double, 2>>);

TEST_CASE("cartesian_vector embed/project between 2D and 3D", "[vector]")
{
  SECTION("embed zero-fills the new coordinate")
  {
    cartesian_vector v3 = embed(cartesian_vector{1.0, 2.0});
    static_assert(std::is_same_v<decltype(v3), cartesian_vector<double, 3>>);
    REQUIRE(v3[0] == 1.0);
    REQUIRE(v3[1] == 2.0);
    REQUIRE(v3[2] == 0.0);
  }

  SECTION("project drops the last coordinate")
  {
    cartesian_vector v2 = project(cartesian_vector{1.0, 2.0, 3.0});
    static_assert(std::is_same_v<decltype(v2), cartesian_vector<double, 2>>);
    REQUIRE(v2[0] == 1.0);
    REQUIRE(v2[1] == 2.0);
  }

  SECTION("project . embed is the identity on 2D")
  {
    REQUIRE(project(embed(cartesian_vector{1.0, 2.0})) == cartesian_vector{1.0, 2.0});
  }
}
