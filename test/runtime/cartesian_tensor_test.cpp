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
#include <complex>
#include <sstream>
#endif
#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/cartesian_tensor.h>
#endif

using namespace mp_units;
using Catch::Matchers::WithinRel;
using namespace std::complex_literals;

// A second-order Cartesian tensor as defined by ISO 80000-2:2019, 18.
TEST_CASE("cartesian_tensor operations", "[tensor]")
{
  SECTION("initialization and access")
  {
    SECTION("no arguments")
    {
      cartesian_tensor<double> t{};
      for (std::size_t r = 0; r < 3; ++r)
        for (std::size_t c = 0; c < 3; ++c) REQUIRE(t(r, c) == 0);
    }

    SECTION("all arguments")
    {
      cartesian_tensor t{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
      REQUIRE(t(0, 0) == 1.0);
      REQUIRE(t(0, 2) == 3.0);
      REQUIRE(t(1, 1) == 5.0);
      REQUIRE(t(2, 0) == 7.0);
      REQUIRE(t(2, 2) == 9.0);
    }

    SECTION("convertible arguments")
    {
      cartesian_tensor<double> t{1, 2, 3, 4, 5, 6, 7, 8, 9};
      REQUIRE(t(0, 0) == 1.0);
      REQUIRE(t(2, 2) == 9.0);
    }

    SECTION("conversion from another representation")
    {
      cartesian_tensor t1{1, 2, 3, 4, 5, 6, 7, 8, 9};
      cartesian_tensor<double> t2 = t1;
      REQUIRE(t2(1, 1) == 5.0);
    }
  }

  SECTION("addition and subtraction (2-18.2)")
  {
    cartesian_tensor a{1, 2, 3, 4, 5, 6, 7, 8, 9};
    cartesian_tensor b{9, 8, 7, 6, 5, 4, 3, 2, 1};

    SECTION("operator+")
    {
      auto r = a + b;
      for (std::size_t i = 0; i < 3; ++i)
        for (std::size_t j = 0; j < 3; ++j) REQUIRE(r(i, j) == 10);
    }

    SECTION("operator- and unary -")
    {
      auto r = a - a;
      for (std::size_t i = 0; i < 3; ++i)
        for (std::size_t j = 0; j < 3; ++j) REQUIRE(r(i, j) == 0);
      REQUIRE((-a)(0, 0) == -1);
      REQUIRE((-a)(2, 2) == -9);
    }

    SECTION("mixed representation (int + double)")
    {
      cartesian_tensor<double> c{0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
      auto r = a + c;
      REQUIRE(r(0, 0) == 1.5);
    }
  }

  SECTION("scalar multiply/divide (2-18.3)")
  {
    cartesian_tensor<double> t{1, 2, 3, 4, 5, 6, 7, 8, 9};

    SECTION("t * s")
    {
      auto r = t * 2.0;
      REQUIRE(r(0, 0) == 2.0);
      REQUIRE(r(2, 2) == 18.0);
    }

    SECTION("s * t")
    {
      auto r = 2.0 * t;
      REQUIRE(r(0, 0) == 2.0);
      REQUIRE(r(2, 2) == 18.0);
    }

    SECTION("t / s")
    {
      auto r = t / 2.0;
      REQUIRE(r(0, 0) == 0.5);
      REQUIRE(r(2, 2) == 4.5);
    }
  }

  SECTION("compound assignments")
  {
    cartesian_tensor t{2.0, 4.0, 6.0, 8.0, 10.0, 12.0, 14.0, 16.0, 18.0};

    SECTION("operator+=")
    {
      cartesian_tensor o{1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
      t += o;
      REQUIRE(t(0, 0) == 3.0);
    }

    SECTION("operator-=")
    {
      cartesian_tensor o{1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
      t -= o;
      REQUIRE(t(0, 0) == 1.0);
    }

    SECTION("operator*=")
    {
      t *= 0.5;
      REQUIRE(t(0, 0) == 1.0);
    }

    SECTION("operator/=")
    {
      t /= 2.0;
      REQUIRE(t(0, 0) == 1.0);
    }
  }

  SECTION("equality")
  {
    cartesian_tensor a{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
    cartesian_tensor b{1, 2, 3, 4, 5, 6, 7, 8, 9};
    cartesian_tensor c{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.1};
    REQUIRE(a == b);
    REQUIRE(a != c);
  }

  SECTION("tensor (dyadic) product of two vectors (2-18.21)")
  {
    cartesian_vector a{1, 2, 3};
    cartesian_vector b{4, 5, 6};
    auto t = tensor_product(a, b);  // (a (x) b)_ij = a_i b_j
    REQUIRE(t(0, 0) == 4);
    REQUIRE(t(0, 1) == 5);
    REQUIRE(t(0, 2) == 6);
    REQUIRE(t(1, 0) == 8);
    REQUIRE(t(1, 1) == 10);
    REQUIRE(t(1, 2) == 12);
    REQUIRE(t(2, 0) == 12);
    REQUIRE(t(2, 1) == 15);
    REQUIRE(t(2, 2) == 18);
  }

  SECTION("inner product of two tensors (2-18.23)")
  {
    cartesian_tensor a{1, 2, 3, 4, 5, 6, 7, 8, 9};
    cartesian_tensor id{1, 0, 0, 0, 1, 0, 0, 0, 1};

    SECTION("T . I == T") { REQUIRE(inner_product(a, id) == a); }

    SECTION("T . T")
    {
      auto r = inner_product(a, a);
      // hand-computed A . A
      REQUIRE(r(0, 0) == 30);
      REQUIRE(r(0, 1) == 36);
      REQUIRE(r(0, 2) == 42);
      REQUIRE(r(1, 0) == 66);
      REQUIRE(r(1, 1) == 81);
      REQUIRE(r(1, 2) == 96);
      REQUIRE(r(2, 0) == 102);
      REQUIRE(r(2, 1) == 126);
      REQUIRE(r(2, 2) == 150);
    }
  }

  SECTION("inner product of a tensor and a vector (2-18.24)")
  {
    cartesian_tensor a{1, 2, 3, 4, 5, 6, 7, 8, 9};
    cartesian_vector v{1, 2, 3};
    auto r = inner_product(a, v);  // (T . a)_i = sum_j T_ij a_j
    REQUIRE(r[0] == 14);
    REQUIRE(r[1] == 32);
    REQUIRE(r[2] == 50);
  }

  SECTION("scalar (double-dot) product of two tensors (2-18.25)")
  {
    cartesian_tensor a{1, 2, 3, 4, 5, 6, 7, 8, 9};
    REQUIRE(scalar_product(a, a) == 285);  // sum of squares 1..9
  }

  SECTION("Frobenius norm/magnitude")
  {
    cartesian_tensor<double> a{1, 2, 3, 4, 5, 6, 7, 8, 9};
    REQUIRE_THAT(a.magnitude(), WithinRel(std::sqrt(285.0), 1e-12));
    REQUIRE_THAT(a.norm(), WithinRel(std::sqrt(285.0), 1e-12));
    REQUIRE_THAT(magnitude(a), WithinRel(std::sqrt(285.0), 1e-12));
    REQUIRE_THAT(norm(a), WithinRel(std::sqrt(285.0), 1e-12));
  }
}

TEST_CASE("cartesian_tensor text output", "[tensor][fmt][ostream]")
{
  std::ostringstream os;

  SECTION("integral representation")
  {
    cartesian_tensor t{1, 2, 3, 4, 5, 6, 7, 8, 9};
    os << t;

    SECTION("iostream") { CHECK(os.str() == "[[1, 2, 3], [4, 5, 6], [7, 8, 9]]"); }

    SECTION("fmt with default format {}") { CHECK(MP_UNITS_STD_FMT::format("{}", t) == os.str()); }
  }

  SECTION("floating-point representation")
  {
    cartesian_tensor t{1.5, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
    os << t;

    SECTION("iostream") { CHECK(os.str() == "[[1.5, 2, 3], [4, 5, 6], [7, 8, 9]]"); }

    SECTION("fmt with default format {}") { CHECK(MP_UNITS_STD_FMT::format("{}", t) == os.str()); }
  }
}

TEST_CASE("cartesian_tensor with a complex representation", "[tensor][complex]")
{
  using c = std::complex<double>;

  SECTION("Hermitian Frobenius norm is a real scalar")
  {
    // only the (0,0) entry is non-zero: |T| = |3+4i| = 5
    cartesian_tensor t{3. + 4.i, c{}, c{}, c{}, c{}, c{}, c{}, c{}, c{}};
    STATIC_CHECK(std::is_same_v<decltype(t.magnitude()), double>);
    REQUIRE_THAT(t.magnitude(), WithinRel(5.0, 1e-12));
    // sqrt(|1+i|^2 * 9) = sqrt(2 * 9) = sqrt(18)
    cartesian_tensor u{1. + 1.i, 1. + 1.i, 1. + 1.i, 1. + 1.i, 1. + 1.i, 1. + 1.i, 1. + 1.i, 1. + 1.i, 1. + 1.i};
    REQUIRE_THAT(u.magnitude(), WithinRel(std::sqrt(18.0), 1e-12));
  }

  SECTION("scalar_product (double-dot) is sesquilinear, T : T is real and non-negative")
  {
    cartesian_tensor a{1. + 1.i, c{}, c{}, c{}, c{}, c{}, c{}, c{}, c{}};
    // a : a = sum |a_ij|^2 = |1+i|^2 = 2
    REQUIRE(scalar_product(a, a) == c{2.0, 0.0});

    // Hermitian symmetry of the double-dot: <b, a> = conj(<a, b>)
    cartesian_tensor b{1.i, c{}, c{}, c{}, c{}, c{}, c{}, c{}, c{}};
    REQUIRE(scalar_product(b, a) == std::conj(scalar_product(a, b)));
  }

  SECTION("inner product (matmul) uses plain complex arithmetic, no conjugation")
  {
    cartesian_tensor id{c{1}, c{}, c{}, c{}, c{1}, c{}, c{}, c{}, c{1}};
    cartesian_tensor a{1. + 1.i, 2. + 0.i, c{}, c{}, 3. - 1.i, c{}, c{}, c{}, 0. + 2.i};
    REQUIRE(inner_product(id, a) == a);  // I . A == A
  }

  SECTION("inner product with a complex vector -> complex vector")
  {
    cartesian_tensor id{c{1}, c{}, c{}, c{}, c{1}, c{}, c{}, c{}, c{1}};
    cartesian_vector v{1. + 1.i, 2. + 0.i, 0. + 3.i};
    REQUIRE(inner_product(id, v) == v);  // I . v == v
  }
}
