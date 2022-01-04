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

#include <units/magnitude.h>
#include <units/ratio.h>
#include <catch2/catch.hpp>
#include <type_traits>

namespace units::mag
{

// Convenience utility to create an integral base power.  For unit tests only.
template<std::intmax_t N, std::intmax_t Num, std::intmax_t Den>
struct IntBasePower
{
  // This setup is more complicated than might appear necessary in the hopes of appeasing a
  // (possibly spurious) MSVC 14 compiler error.
  static constexpr std::intmax_t num = Num;
  static constexpr std::intmax_t den = Den;
  static constexpr ratio power = ratio{num, den};
  using type = base_power<prime_base<N>, power>;
};
template<std::intmax_t N, std::intmax_t Num = 1, std::intmax_t Den = 1>
using int_base_power = typename IntBasePower<N, Num, Den>::type;

TEST_CASE("Magnitude is invertible")
{
  CHECK(std::is_same_v<inverse_t<magnitude<>>, magnitude<>>);
  CHECK(std::is_same_v<
      inverse_t<magnitude<int_base_power<2>>>, magnitude<int_base_power<2, -1>>>);
  CHECK(std::is_same_v<
      inverse_t<magnitude<int_base_power<3, 1, 2>, int_base_power<11, -5>>>,
      magnitude<int_base_power<3, -1, 2>, int_base_power<11, 5>>>);
}

TEST_CASE("Magnitude supports products")
{
  SECTION ("The nullary product gives the unit magnitude") {
    CHECK(std::is_same_v<product_t<>, magnitude<>>);
  }

  SECTION ("The unary product is the identity operation") {
    CHECK(std::is_same_v<
        product_t<magnitude<int_base_power<3, 4>>>,
        magnitude<int_base_power<3, 4>>>);

    CHECK(std::is_same_v<
        product_t<magnitude<int_base_power<2, -1, 3>, int_base_power<13, -2>>>,
        magnitude<int_base_power<2, -1, 3>, int_base_power<13, -2>>>);
  }

  SECTION ("Binary product with null magnitude is identity") {
    using arbitrary_mag = magnitude<int_base_power<11, 3, 2>>;
    CHECK(std::is_same_v<product_t<magnitude<>, magnitude<>>, magnitude<>>);
    CHECK(std::is_same_v<product_t<arbitrary_mag, magnitude<>>, arbitrary_mag>);
    CHECK(std::is_same_v<product_t<magnitude<>, arbitrary_mag>, arbitrary_mag>);
  }

  SECTION ("Binary product with distinct bases maintains sorted order") {
    CHECK(std::is_same_v<
        product_t<
          magnitude<int_base_power<2, 1, 3>, int_base_power<7, -2>>,
          magnitude<int_base_power<3>, int_base_power<5, 5>>>,
        magnitude<
          int_base_power<2, 1, 3>,
          int_base_power<3>,
          int_base_power<5, 5>,
          int_base_power<7, -2>>>);
  }

  SECTION ("Binary products add exponents for same bases") {
    CHECK(std::is_same_v<
        product_t<
          magnitude<int_base_power<2, 3>>,
          magnitude<int_base_power<2, -5>>>,
        magnitude<int_base_power<2, -2>>>);
    CHECK(std::is_same_v<
        product_t<
          magnitude<int_base_power<2, 3>, int_base_power<3, -1, 3>>,
          magnitude<int_base_power<2, -5>, int_base_power<5, 4>>>,
        magnitude<int_base_power<2, -2>, int_base_power<3, -1, 3>, int_base_power<5, 4>>>);
  }

  SECTION ("Binary products omit bases whose exponents cancel") {
    CHECK(std::is_same_v<
        product_t<
          magnitude<int_base_power<2, 1, 3>>, magnitude<int_base_power<2, -1, 3>>>,
        magnitude<>>);
    CHECK(std::is_same_v<
        product_t<
          magnitude<int_base_power<2, 1, 3>, int_base_power<7, -2>>,
          magnitude<int_base_power<2, -1, 3>, int_base_power<5, 5>>>,
        magnitude<int_base_power<5, 5>, int_base_power<7, -2>>>);
    CHECK(std::is_same_v<
        product_t<
          magnitude<int_base_power<2, 1, 3>, int_base_power<3, -2>, int_base_power<7, -2>>,
          magnitude<int_base_power<2, -1, 3>, int_base_power<5, 5>, int_base_power<7, 2>>>,
        magnitude<int_base_power<3, -2>, int_base_power<5, 5>>>);
  }

  SECTION ("N-ary products recurse") {
    CHECK(std::is_same_v<
        product_t<
          magnitude<int_base_power<2, 1, 3>>,
          magnitude<int_base_power<2, 2, 3>>,
          magnitude<int_base_power<3, -4>>,
          magnitude<int_base_power<5>>,
          magnitude<int_base_power<2, -1>>>,
        magnitude<int_base_power<3, -4>, int_base_power<5>>>);
  }
}

TEST_CASE("is_base_power detects well formed base powers")
{
  SECTION ("Arbitrary other types are not base powers")
  {
    CHECK(!is_base_power_v<void>);
    CHECK(!is_base_power_v<int>);
    CHECK(!is_base_power_v<magnitude<int_base_power<3, 1, 4>>>);
  }

  SECTION ("int_base_power forms base powers")
  {
    CHECK(is_base_power_v<int_base_power<2>>);
    CHECK(is_base_power_v<int_base_power<2, -1>>);
    CHECK(is_base_power_v<int_base_power<2, -1, 8>>);
  }

  SECTION ("base_power forms base powers with pi and ratio")
  {
    CHECK(is_base_power_v<base_power<pi>>);
    CHECK(is_base_power_v<base_power<pi, ratio{2}>>);
    CHECK(is_base_power_v<base_power<pi, ratio{-2, 3}>>);
  }

  SECTION ("base_power disqualified by base without value")
  {
    CHECK(!is_base_power_v<base_power<int>>);
    CHECK(!is_base_power_v<base_power<int, ratio{2}>>);
    CHECK(!is_base_power_v<base_power<int, ratio{-2, 3}>>);
  }
}

TEST_CASE("is_magnitude detects well formed magnitudes")
{
  SECTION ("Arbitrary other types are not magnitudes")
  {
    CHECK(!is_magnitude_v<void>);
    CHECK(!is_magnitude_v<int>);
    CHECK(!is_magnitude_v<int_base_power<3, 1, 4>>);
  }

  SECTION ("Null magnitude is magnitude")
  {
    CHECK(is_magnitude_v<magnitude<>>);
  }

  SECTION ("Single-base magnitude is magnitude")
  {
    CHECK(is_magnitude_v<magnitude<int_base_power<3, 1, 4>>>);
  }

  SECTION ("Out-of-order bases disqualify magnitudes")
  {
    CHECK(!is_magnitude_v<magnitude<int_base_power<3>, int_base_power<2>>>);
  }

  SECTION ("Repeated bases disqualify magnitudes")
  {
    CHECK(!is_magnitude_v<magnitude<int_base_power<2, 1>, int_base_power<2, 2>>>);
  }

  SECTION ("Mixed base types are magnitudes if sorted")
  {
    CHECK(is_magnitude_v<magnitude<int_base_power<2>, base_power<pi>>>);
    CHECK(is_magnitude_v<magnitude<int_base_power<3>, base_power<pi>>>);
    CHECK(!is_magnitude_v<magnitude<int_base_power<5>, base_power<pi>>>);
  }
}

TEST_CASE("strictly_increasing")
{
  SECTION ("Empty tuple is sorted")
  {
    CHECK(strictly_increasing(std::make_tuple()));
  }

  SECTION ("Single-element tuple is sorted")
  {
    CHECK(strictly_increasing(std::make_tuple(3)));
    CHECK(strictly_increasing(std::make_tuple(15.42)));
    CHECK(strictly_increasing(std::make_tuple('c')));
  }

  SECTION ("Multi-element tuples compare correctly")
  {
    CHECK(strictly_increasing(std::make_tuple(3, 3.14)));
    CHECK(!strictly_increasing(std::make_tuple(3, 3.0)));
    CHECK(!strictly_increasing(std::make_tuple(4, 3.0)));
  }
}

TEST_CASE("make_ratio performs prime factorization correctly")
{
  SECTION("Performs prime factorization when denominator is 1")
  {
    CHECK(std::is_same_v<decltype(make_ratio<1>()), magnitude<>>);
    CHECK(std::is_same_v<decltype(make_ratio<2>()), magnitude<int_base_power<2>>>);
    CHECK(std::is_same_v<decltype(make_ratio<3>()), magnitude<int_base_power<3>>>);
    CHECK(std::is_same_v<decltype(make_ratio<4>()), magnitude<int_base_power<2, 2>>>);

    CHECK(std::is_same_v<
        decltype(make_ratio<792>()),
        magnitude<int_base_power<2, 3>, int_base_power<3, 2>, int_base_power<11>>>);
  }

  SECTION("Reduces fractions to lowest terms")
  {
    CHECK(std::is_same_v<decltype(make_ratio<8, 8>()), magnitude<>>);
    CHECK(std::is_same_v<
        decltype(make_ratio<50, 80>()),
        magnitude<int_base_power<2, -3>, int_base_power<5>>>);
  }
}

TEST_CASE("make_magnitude handles arbitrary bases")
{
  SECTION("Equivalent to std::integral_constant for integer bases")
  {
    CHECK(make_base_power<prime_base<2>>() == make_ratio<2>());
    CHECK(make_base_power<prime_base<7>>() == make_ratio<7>());
  }

  SECTION("Handles non-integer bases")
  {
    CHECK(make_base_power<pi>() == magnitude<base_power<pi>>{});
    CHECK(make_base_power<pi, -3>() == magnitude<base_power<pi, ratio{-3}>>{});
    CHECK(make_base_power<pi, -3, 7>() == magnitude<base_power<pi, ratio{-3, 7}>>{});
  }
}

TEST_CASE("Equality works for magnitudes")
{
  SECTION("Equivalent ratios are equal")
  {
    CHECK(make_ratio<1>() == make_ratio<1>());
    CHECK(make_ratio<3>() == make_ratio<3>());
    CHECK(make_ratio<3, 4>() == make_ratio<9, 12>());
  }

  SECTION("Different ratios are unequal")
  {
    CHECK(make_ratio<3>() != make_ratio<5>());
    CHECK(make_ratio<3>() != make_ratio<3, 2>());
  }

  SECTION("Supports constexpr")
  {
    constexpr auto eq = make_ratio<4, 5>() == make_ratio<4, 3>();
    CHECK(!eq);
  }
}

TEST_CASE("Multiplication works for magnitudes")
{
  SECTION("Reciprocals reduce to null magnitude")
  {
    CHECK(make_ratio<3, 4>() * make_ratio<4, 3>() == make_ratio<1>());
  }

  SECTION("Products work as expected")
  {
    CHECK(make_ratio<4, 5>() * make_ratio<4, 3>() == make_ratio<16, 15>());
  }

  SECTION("Products handle pi correctly")
  {
    CHECK(
        make_base_power<pi>() * make_ratio<2, 3>() * make_base_power<pi, -1, 2>() ==
        magnitude<int_base_power<2>, int_base_power<3, -1>, base_power<pi, ratio{1, 2}>>{});

  }

  SECTION("Supports constexpr")
  {
    constexpr auto p = make_ratio<4, 5>() * make_ratio<4, 3>();
    CHECK(p == make_ratio<16, 15>());
  }
}

TEST_CASE("Division works for magnitudes")
{
  SECTION("Dividing anything by itself reduces to null magnitude")
  {
    CHECK(make_ratio<3, 4>() / make_ratio<3, 4>() == make_ratio<1>());
    CHECK(make_ratio<15>() / make_ratio<15>() == make_ratio<1>());
  }

  SECTION("Quotients work as expected")
  {
    CHECK(make_ratio<4, 5>() / make_ratio<4, 3>() == make_ratio<3, 5>());
  }

  SECTION("Supports constexpr")
  {
    constexpr auto q = make_ratio<4, 5>() / make_ratio<4, 3>();
    CHECK(q == make_ratio<3, 5>());
  }
}

namespace detail
{

TEST_CASE("Prime factorization")
{
  SECTION ("1 factors into the null magnitude")
  {
    CHECK(std::is_same_v<prime_factorization_t<1>, magnitude<>>);
  }

  SECTION ("Prime numbers factor into themselves")
  {
    CHECK(std::is_same_v<prime_factorization_t<2>, magnitude<int_base_power<2>>>);
    CHECK(std::is_same_v<prime_factorization_t<3>, magnitude<int_base_power<3>>>);
    CHECK(std::is_same_v<prime_factorization_t<5>, magnitude<int_base_power<5>>>);
    CHECK(std::is_same_v<prime_factorization_t<7>, magnitude<int_base_power<7>>>);
    CHECK(std::is_same_v<prime_factorization_t<11>, magnitude<int_base_power<11>>>);

    CHECK(std::is_same_v<prime_factorization_t<41>, magnitude<int_base_power<41>>>);
  }

  SECTION("Prime factorization finds factors and multiplicities")
  {
    CHECK(std::is_same_v<
        prime_factorization_t<792>,
        magnitude<int_base_power<2, 3>, int_base_power<3, 2>, int_base_power<11>>>);
  }
}

TEST_CASE("is_prime detects primes")
{
  SECTION("Non-positive numbers are not prime")
  {
    CHECK(!is_prime(-1328));
    CHECK(!is_prime(-1));
    CHECK(!is_prime(0));
  }

  SECTION("1 is not prime")
  {
    CHECK(!is_prime(1));
  }

  SECTION("Discriminates between primes and non-primes")
  {
    CHECK(is_prime(2));
    CHECK(is_prime(3));
    CHECK(!is_prime(4));
    CHECK(is_prime(5));
    CHECK(!is_prime(6));
    CHECK(is_prime(7));
    CHECK(!is_prime(8));
    CHECK(!is_prime(9));

    CHECK(is_prime(7919));
  }
}

TEST_CASE("pairwise_all evaluates all pairs")
{
  SECTION("always true for empty tuples")
  {
    CHECK(pairwise_all(std::make_tuple(), [](auto, auto){ return true; }));
    CHECK(pairwise_all(std::make_tuple(), [](auto, auto){ return false; }));
  }

  SECTION("always true for single-element tuples")
  {
    CHECK(pairwise_all(std::make_tuple(1), [](auto, auto){ return true; }));
    CHECK(pairwise_all(std::make_tuple(3.14), [](auto, auto){ return false; }));
    CHECK(pairwise_all(std::make_tuple('x'), [](auto, auto){ return true; }));
  }

  SECTION("true for longer tuples iff true for all neighbouring pairs")
  {
    CHECK(pairwise_all(std::make_tuple(1, 1.5), std::less{}));
    CHECK(pairwise_all(std::make_tuple(1, 1.5, 2), std::less{}));
    CHECK(!pairwise_all(std::make_tuple(1, 2.0, 2), std::less{}));
    CHECK(!pairwise_all(std::make_tuple(1, 2.5, 2), std::less{}));
  }
}

} // namespace detail

} // namespace units::mag
