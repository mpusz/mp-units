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

#include <units/math.h>
#include <units/isq/si/area.h>
#include <units/isq/si/length.h>
#include <units/isq/si/volume.h>
#include <catch2/catch.hpp>
#include <limits>

using namespace units;
using namespace units::isq::si;

// classical

TEST_CASE("'pow<N>()' on quantity changes the value and the dimension accordingly", "[math][pow]")
{
  SECTION ("'pow<0>(q)' returns '1'") {
    CHECK(pow<0>(2_q_m) == 1);
  }

  SECTION ("'pow<1>(q)' returns 'q'") {
    CHECK(pow<1>(2_q_m) == 2_q_m);
  }

  SECTION ("'pow<2>(q)' squares both the value and a dimension") {
    CHECK(pow<2>(2_q_m) == 4_q_m2);
  }

  SECTION ("'pow<3>(q)' cubes both the value and a dimension") {
    CHECK(pow<3>(2_q_m) == 8_q_m3);
  }
}

TEST_CASE("'sqrt()' on quantity changes the value and the dimension accordingly", "[math][sqrt]")
{
  REQUIRE(sqrt(4_q_m2) == 2_q_m);
}

TEST_CASE("'cbrt()' on quantity changes the value and the dimension accordingly", "[math][cbrt]")
{
  REQUIRE(cbrt(8_q_m3) == 2_q_m);
}

TEST_CASE("'pow<Num, Den>()' on quantity changes the value and the dimension accordingly", "[math][pow]")
{
  REQUIRE(pow<1, 4>(16_q_m2) == sqrt(4_q_m));
}

TEST_CASE("absolute functions on quantity returns the absolute value", "[math][abs][fabs]")
{
  SECTION ("'abs()' on a negative quantity returns the abs")
  {
    SECTION ("integral representation")
    {
      REQUIRE(abs(-1_q_m) == 1_q_m);
    }

    SECTION ("floating-point representation")
    {
      REQUIRE(abs(-1._q_m) == 1_q_m);
    }
  }

  SECTION ("'abs()' on a positive quantity returns the abs")
  {
    SECTION ("integral representation")
    {
      REQUIRE(abs(1_q_m) == 1_q_m);
    }

    SECTION ("floating-point representation")
    {
      REQUIRE(abs(1._q_m) == 1_q_m);
    }
  }
}

TEST_CASE("numeric_limits functions", "[limits]")
{
  SECTION ("'epsilon' works as expected using default floating type") {
    REQUIRE(epsilon<decltype(1._q_m)>().number() == std::numeric_limits<decltype(1._q_m)::rep>::epsilon());
  }
  SECTION ("'epsilon' works as expected using integers") {
    REQUIRE(epsilon<decltype(1_q_m)>().number() == std::numeric_limits<decltype(1_q_m)::rep>::epsilon());
  }
  SECTION ("'epsilon' works as expected using mixed Rep types") {
    REQUIRE(epsilon<decltype(1_q_m)>().number() != std::numeric_limits<decltype(1._q_m)::rep>::epsilon());
  }
}

TEMPLATE_TEST_CASE_SIG("pow<N>() implementation exponentiates values to power N", "[math][pow][exp]",
                       (std::intmax_t N, N), 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 20, 25)
{
  auto v = GENERATE(range(0.5, 20.0, 0.5));
  REQUIRE(detail::pow_impl<N>(v) == Approx(std::pow(v, N)).epsilon(1e-15).margin(0));
}

template<std::intmax_t N>
struct Pow {
  constexpr static std::intmax_t exponent = N;
  template<typename T>
  [[nodiscard]] constexpr static auto pow(T const& v) noexcept
  {
    return detail::pow_impl<N>(v);
  }
};

template<std::intmax_t N>
struct CompileRoot : Pow<N> {
  [[nodiscard]] constexpr static std::intmax_t root(std::intmax_t v) noexcept { return detail::iroot_compile<N>(v); }
};

template<std::intmax_t N>
struct RuntimeRoot : Pow<N> {
  [[nodiscard]] static std::intmax_t root(std::intmax_t v) noexcept { return detail::iroot_runtime<N>(v); }
};

// test to make sure precision is not lost when rounding what should be integer roots
template<typename TestType>
static void root_test()
{
  SECTION ("Roots are truncated down") {
    auto base = GENERATE(range(1.0, 10.0, 1.0)); // doubles to guard against overflow

    if (TestType::pow(base) < static_cast<double>(std::numeric_limits<std::intmax_t>::max())) {
      const std::intmax_t x = TestType::pow(static_cast<std::intmax_t>(base));
      const auto expect = static_cast<std::intmax_t>(base);

      REQUIRE(TestType::root(x - 1) == expect - 1);
      REQUIRE(TestType::root(x) == expect);
    }
  }

  SECTION ("Roots are truncated correctly for very large inputs") {
    auto exponent = GENERATE(range(10, std::numeric_limits<std::intmax_t>::digits10, 1));
    const auto large_val = static_cast<std::intmax_t>(std::pow(10, exponent));
    const auto expected = static_cast<std::intmax_t>(std::pow(10, exponent / static_cast<double>(TestType::exponent)));

    REQUIRE(TestType::root(large_val) == expected);
  }
}

/* Catch2 uses int for indexing in TEMPLATE_PRODUCT_TEST_CASE_SIG so it does not compile with -Werror=sign-conversion
 * https://github.com/catchorg/Catch2/pull/2074

TEMPLATE_PRODUCT_TEST_CASE_SIG("detail::iroot<N>()", "[math][pow][iroot]", (std::intmax_t N, N),
                               (CompileRoot, RuntimeRoot), (1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 20, 25))
{
  root_test<TestType>();
}
 */
#define ROOT_TEST_CASE(Type)                                                                                          \
  TEMPLATE_TEST_CASE_SIG("detail::iroot<N>() - " #Type, "[math][pow][iroot]", (std::intmax_t N, N), 1, 2, 3, 4, 5, 6, \
                         7, 8, 9, 10, 15, 20, 25)                                                                     \
  {                                                                                                                   \
    root_test<Type<N>>();                                                                                             \
  }

ROOT_TEST_CASE(CompileRoot)
ROOT_TEST_CASE(RuntimeRoot)
