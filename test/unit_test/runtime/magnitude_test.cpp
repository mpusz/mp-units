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

#include <catch2/catch.hpp>
#include <units/magnitude.h>
#include <units/ratio.h>
#include <type_traits>

using namespace units;
using namespace units::detail;

template<>
inline constexpr std::optional<std::intmax_t> units::known_first_factor<9223372036854775783> = 9223372036854775783;

namespace {

// A set of non-standard bases for testing purposes.
struct noninteger_base {
  static constexpr long double value = 1.234L;
};
struct noncanonical_two_base {
  static constexpr long double value = 2.0L;
};
struct other_noncanonical_two_base {
  static constexpr long double value = 2.0L;
};
struct invalid_zero_base {
  static constexpr long double value = 0.0L;
};
struct invalid_negative_base {
  static constexpr long double value = -1.234L;
};

template<ratio Power>
constexpr auto pi_to_the()
{
  return magnitude<base_power<pi_base>{Power}>{};
}

template<typename T, typename U>
void check_same_type_and_value(T actual, U expected)
{
  CHECK(std::is_same_v<T, U>);
  CHECK(actual == expected);
}

TEST_CASE("base_power")
{
  SECTION ("base rep deducible for integral base") {
    CHECK(base_power{2} == base_power<std::intmax_t>{2, ratio{1}});
    CHECK(base_power{2, 3} == base_power<std::intmax_t>{2, ratio{3}});
    CHECK(base_power{2, ratio{3, 4}} == base_power<std::intmax_t>{2, ratio{3, 4}});
  }

  SECTION ("get_base retrieves base for integral base") {
    CHECK(base_power{2}.get_base() == 2);
    CHECK(base_power{3, 5}.get_base() == 3);
    CHECK(base_power{5, ratio{1, 3}}.get_base() == 5);
  }

  SECTION ("get_base retrieves member value for non-integer base") {
    CHECK(base_power<noninteger_base>{}.get_base() == 1.234L);
    CHECK(base_power<noninteger_base>{2}.get_base() == 1.234L);
    CHECK(base_power<noninteger_base>{ratio{5, 8}}.get_base() == 1.234L);
  }

  SECTION ("same-base values not equal if types are different") {
    const auto a = base_power<noncanonical_two_base>{};
    const auto b = base_power{2};
    const auto c = base_power<other_noncanonical_two_base>{};

    REQUIRE(a.get_base() == b.get_base());
    CHECK(a != b);

    REQUIRE(a.get_base() == c.get_base());
    CHECK(a != c);
  }

  SECTION ("same-type values not equal if bases are different") {
    CHECK(base_power{2} != base_power{3});
    CHECK(base_power{2, ratio{5, 4}} != base_power{3, ratio{5, 4}});
  }

  SECTION ("same-type, same-base values not equal if powers are different") {
    CHECK(base_power{2} != base_power{2, 2});
    CHECK(base_power<pi_base>{} != base_power<pi_base>{ratio{1, 3}});
  }

  SECTION ("product with inverse equals identity") {
    auto check_product_with_inverse_is_identity = [](auto x) { CHECK(x * pow<-1>(x) == as_magnitude<1>()); };

    check_product_with_inverse_is_identity(as_magnitude<3>());
    check_product_with_inverse_is_identity(as_magnitude<ratio{4, 17}>());
    check_product_with_inverse_is_identity(pi_to_the<ratio{-22, 7}>());
  }

  SECTION ("pow() multiplies exponent") {
    CHECK(pow(base_power{2}, 0) == base_power{2, 0});
    CHECK(pow(base_power{2, 3}, ratio{-1, 2}) == base_power{2, ratio{-3, 2}});
    CHECK(pow(base_power<pi_base>{ratio{3, 2}}, ratio{1, 3}) == base_power<pi_base>{ratio{1, 2}});
  }
}

TEST_CASE("make_ratio performs prime factorization correctly")
{
  SECTION ("Performs prime factorization when denominator is 1") {
    CHECK(as_magnitude<1>() == magnitude<>{});
    CHECK(as_magnitude<2>() == magnitude<base_power{2}>{});
    CHECK(as_magnitude<3>() == magnitude<base_power{3}>{});
    CHECK(as_magnitude<4>() == magnitude<base_power{2, 2}>{});

    CHECK(as_magnitude<792>() == magnitude<base_power{2, 3}, base_power{3, 2}, base_power{11}>{});
  }

  SECTION ("Supports fractions") {
    CHECK(as_magnitude<ratio{5, 8}>() == magnitude<base_power{2, -3}, base_power{5}>{});
  }

  SECTION ("Supports nonzero exp") {
    constexpr ratio r{3, 1, 2};
    REQUIRE(r.exp == 2);
    CHECK(as_magnitude<r>() == as_magnitude<300>());
  }

  SECTION ("Can handle prime factor which would be large enough to overflow int") {
    // This was taken from a case which failed when we used `int` for our base to store prime numbers.
    // The failure was due to a prime factor which is larger than 2^31.
    as_magnitude<ratio(16'605'390'666'050, 10'000'000'000'000)>();
  }

  SECTION ("Can handle prime number which would exceed GCC iteration limit") {
    // GCC 10 has a constexpr loop iteration limit of 262144.  A naive algorithm, which performs trial division on 2 and
    // all odd numbers up to sqrt(N), will exceed this limit for the following prime.  Thus, for this test to pass, we
    // need to be using a more efficient algorithm.  (We could increase the limit, but we don't want users to have to
    // mess with compiler flags just to compile the code.)
    as_magnitude<334'524'384'739>();
  }

  SECTION ("Can bypass computing primes by providing known_first_factor<N>") {
    // Sometimes, even wheel factorization isn't enough to handle the compilers' limits on constexpr steps and/or
    // iterations.  To work around these cases, we can explicitly provide the correct answer directly to the compiler.
    //
    // In this case, we test that we can represent the largest prime that fits in a signed 64-bit int.  The reason this
    // test can pass is that we have provided the answer, by specializing the `known_first_factor` variable template
    // above in this file.
    as_magnitude<9'223'372'036'854'775'783>();
  }
}

TEST_CASE("magnitude converts to numerical value")
{
  SECTION ("Positive integer powers of integer bases give integer values") {
    constexpr auto mag_412 = as_magnitude<412>();
    check_same_type_and_value(get_value<int>(mag_412), 412);
    check_same_type_and_value(get_value<std::size_t>(mag_412), std::size_t{412});
    check_same_type_and_value(get_value<float>(mag_412), 412.0f);
    check_same_type_and_value(get_value<double>(mag_412), 412.0);
  }

  SECTION ("Negative integer powers of integer bases compute correct values") {
    constexpr auto mag_0p125 = as_magnitude<ratio{1, 8}>();
    check_same_type_and_value(get_value<float>(mag_0p125), 0.125f);
    check_same_type_and_value(get_value<double>(mag_0p125), 0.125);
  }

  SECTION ("pi to the 1 supplies correct values") {
    constexpr auto pi = pi_to_the<1>();
    check_same_type_and_value(get_value<float>(pi), std::numbers::pi_v<float>);
    check_same_type_and_value(get_value<double>(pi), std::numbers::pi_v<double>);
    check_same_type_and_value(get_value<long double>(pi), std::numbers::pi_v<long double>);
  }

  SECTION ("pi to arbitrary power performs computations in most accurate type at compile time") {
    if constexpr (sizeof(float) < sizeof(long double)) {
      constexpr auto pi_cubed = pi_to_the<3>();

      auto cube = [](auto x) { return x * x * x; };
      constexpr auto via_float = cube(std::numbers::pi_v<float>);
      constexpr auto via_long_double = static_cast<float>(cube(std::numbers::pi_v<long double>));

      constexpr auto pi_cubed_value = get_value<float>(pi_cubed);
      REQUIRE(pi_cubed_value != via_float);
      CHECK(pi_cubed_value == via_long_double);
    }
  }

  SECTION ("Impossible requests are prevented at compile time") {
    // Naturally, we cannot actually write a test to verify a compiler error.  But any of these can
    // be uncommented if desired to verify that it breaks the build.

    // get_value<int8_t>(as_magnitude<412>());

    // Would work for pow<62>:
    // get_value<int64_t>(pow<63>(as_magnitude<2>()));

    // Would work for pow<63>:
    // get_value<uint64_t>(pow<64>(as_magnitude<2>()));

    get_value<double>(pow<308>(as_magnitude<10>()));  // Compiles, correctly.
    // get_value<double>(pow<309>(as_magnitude<10>()));
    // get_value<double>(pow<3099>(as_magnitude<10>()));
    // get_value<double>(pow<3099999>(as_magnitude<10>()));

    auto sqrt_2 = pow<ratio{1, 2}>(as_magnitude<2>());
    CHECK(!is_integral(sqrt_2));
    // get_value<int>(sqrt_2);
  }
}

TEST_CASE("Equality works for magnitudes")
{
  SECTION ("Equivalent ratios are equal") {
    CHECK(as_magnitude<1>() == as_magnitude<1>());
    CHECK(as_magnitude<3>() == as_magnitude<3>());
    CHECK(as_magnitude<ratio{3, 4}>() == as_magnitude<ratio{9, 12}>());
  }

  SECTION ("Different ratios are unequal") {
    CHECK(as_magnitude<3>() != as_magnitude<5>());
    CHECK(as_magnitude<3>() != as_magnitude<ratio{3, 2}>());
  }

  SECTION ("Supports constexpr") {
    constexpr auto eq = (as_magnitude<ratio{4, 5}>() == as_magnitude<ratio{4, 3}>());
    CHECK(!eq);
  }
}

TEST_CASE("Multiplication works for magnitudes")
{
  SECTION ("Reciprocals reduce to null magnitude") {
    CHECK(as_magnitude<ratio{3, 4}>() * as_magnitude<ratio{4, 3}>() == as_magnitude<1>());
  }

  SECTION ("Products work as expected") {
    CHECK(as_magnitude<ratio{4, 5}>() * as_magnitude<ratio{4, 3}>() == as_magnitude<ratio{16, 15}>());
  }

  SECTION ("Products handle pi correctly") {
    CHECK(pi_to_the<1>() * as_magnitude<ratio{2, 3}>() * pi_to_the<ratio{-1, 2}>() ==
          magnitude<base_power{2}, base_power{3, -1}, base_power<pi_base>{ratio{1, 2}}>{});
  }

  SECTION ("Supports constexpr") {
    constexpr auto p = as_magnitude<ratio{4, 5}>() * as_magnitude<ratio{4, 3}>();
    CHECK(p == as_magnitude<ratio{16, 15}>());
  }
}

TEST_CASE("Division works for magnitudes")
{
  SECTION ("Dividing anything by itself reduces to null magnitude") {
    CHECK(as_magnitude<ratio{3, 4}>() / as_magnitude<ratio{3, 4}>() == as_magnitude<1>());
    CHECK(as_magnitude<15>() / as_magnitude<15>() == as_magnitude<1>());
  }

  SECTION ("Quotients work as expected") {
    CHECK(as_magnitude<ratio{4, 5}>() / as_magnitude<ratio{4, 3}>() == as_magnitude<ratio{3, 5}>());
  }

  SECTION ("Supports constexpr") {
    constexpr auto q = as_magnitude<ratio{4, 5}>() / as_magnitude<ratio{4, 3}>();
    CHECK(q == as_magnitude<ratio{3, 5}>());
  }
}

TEST_CASE("Can raise Magnitudes to rational powers")
{
  SECTION ("Anything to the 0 is 1") {
    CHECK(pow<0>(as_magnitude<1>()) == as_magnitude<1>());
    CHECK(pow<0>(as_magnitude<123>()) == as_magnitude<1>());
    CHECK(pow<0>(as_magnitude<ratio{3, 4}>()) == as_magnitude<1>());
    CHECK(pow<0>(pi_to_the<ratio{-1, 2}>()) == as_magnitude<1>());
  }

  SECTION ("Anything to the 1 is itself") {
    CHECK(pow<1>(as_magnitude<1>()) == as_magnitude<1>());
    CHECK(pow<1>(as_magnitude<123>()) == as_magnitude<123>());
    CHECK(pow<1>(as_magnitude<ratio{3, 4}>()) == as_magnitude<ratio{3, 4}>());
    CHECK(pow<1>(pi_to_the<ratio{-1, 2}>()) == pi_to_the<ratio{-1, 2}>());
  }

  SECTION ("Can raise to arbitrary rational power") {
    CHECK(pow<ratio{-8, 3}>(pi_to_the<ratio{-1, 2}>()) == pi_to_the<ratio{4, 3}>());
  }
}

TEST_CASE("can distinguish integral, rational, and irrational magnitudes")
{
  SECTION ("Integer magnitudes are integral and rational") {
    auto check_rational_and_integral = [](Magnitude auto m) {
      CHECK(is_integral(m));
      CHECK(is_rational(m));
    };
    check_rational_and_integral(magnitude<>{});
    check_rational_and_integral(as_magnitude<1>());
    check_rational_and_integral(as_magnitude<3>());
    check_rational_and_integral(as_magnitude<8>());
    check_rational_and_integral(as_magnitude<412>());
    check_rational_and_integral(as_magnitude<ratio{1, 1}>());
  }

  SECTION ("Fractional magnitudes are rational, but not integral") {
    auto check_rational_but_not_integral = [](Magnitude auto m) {
      CHECK(!is_integral(m));
      CHECK(is_rational(m));
    };
    check_rational_but_not_integral(as_magnitude<ratio{1, 2}>());
    check_rational_but_not_integral(as_magnitude<ratio{5, 8}>());
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Detail function tests below.

TEST_CASE("int_power computes integer powers")
{
  SECTION ("handles floating point") {
    check_same_type_and_value(int_power(0.123L, 0), 1.0L);
    check_same_type_and_value(int_power(0.246f, 1), 0.246f);
    check_same_type_and_value(int_power(0.5f, 3), 0.125f);
    check_same_type_and_value(int_power(2.5, 4), 39.0625);

    CHECK(std::is_same_v<long double, decltype(compute_base_power<double>(base_power{10, 20}))>);
  }

  SECTION ("handles integral") {
    check_same_type_and_value(int_power(8, 0), 1);
    check_same_type_and_value(int_power(9L, 1), 9L);
    check_same_type_and_value(int_power(2, 10), 1024);
  }
}

TEST_CASE("Prime helper functions")
{
  SECTION ("multiplicity") {
    CHECK(multiplicity(2, 8) == 3);
    CHECK(multiplicity(2, 1024) == 10);
    CHECK(multiplicity(11, 6655) == 3);
  }

  SECTION ("remove_power()") {
    CHECK(remove_power(17, 0, 5) == 5);
    CHECK(remove_power(2, 3, 24) == 3);
    CHECK(remove_power(11, 3, 6655) == 5);
  }
}

TEST_CASE("Prime factorization")
{
  SECTION ("1 factors into the null magnitude") {
    CHECK(prime_factorization_v<1> == magnitude<>{});
  }

  SECTION ("Prime numbers factor into themselves") {
    CHECK(prime_factorization_v<2> == magnitude<base_power{2}>{});
    CHECK(prime_factorization_v<3> == magnitude<base_power{3}>{});
    CHECK(prime_factorization_v<5> == magnitude<base_power{5}>{});
    CHECK(prime_factorization_v<7> == magnitude<base_power{7}>{});
    CHECK(prime_factorization_v<11> == magnitude<base_power{11}>{});

    CHECK(prime_factorization_v<41> == magnitude<base_power{41}>{});
  }

  SECTION ("Prime factorization finds factors and multiplicities") {
    CHECK(prime_factorization_v<792> == magnitude<base_power{2, 3}, base_power{3, 2}, base_power{11}>{});
  }
}

TEST_CASE("is_prime detects primes")
{
  SECTION ("Non-positive numbers are not prime") {
    CHECK(!is_prime(-1328));
    CHECK(!is_prime(-1));
    CHECK(!is_prime(0));
  }

  SECTION ("1 is not prime") {
    CHECK(!is_prime(1));
  }

  SECTION ("Discriminates between primes and non-primes") {
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

TEST_CASE("is_valid_base_power")
{
  SECTION ("0 power is invalid") {
    REQUIRE(is_valid_base_power(base_power{2}));
    CHECK(!is_valid_base_power(base_power{2, 0}));

    REQUIRE(is_valid_base_power(base_power{41}));
    CHECK(!is_valid_base_power(base_power{41, 0}));

    REQUIRE(is_valid_base_power(base_power<pi_base>{}));
    CHECK(!is_valid_base_power(base_power<pi_base>{0}));
  }

  SECTION ("non-prime integers are invalid") {
    CHECK(!is_valid_base_power(base_power{-8}));
    CHECK(!is_valid_base_power(base_power{0}));
    CHECK(!is_valid_base_power(base_power{1}));

    CHECK(is_valid_base_power(base_power{2}));
    CHECK(is_valid_base_power(base_power{3}));

    CHECK(!is_valid_base_power(base_power{4}));
  }

  SECTION ("non-positive floating point bases are invalid") {
    CHECK(!is_valid_base_power(base_power<invalid_zero_base>{}));
    CHECK(!is_valid_base_power(base_power<invalid_negative_base>{}));
  }
}

TEST_CASE("pairwise_all evaluates all pairs")
{
  const auto all_pairs_return_true = pairwise_all{[](auto, auto) { return true; }};
  const auto all_pairs_return_false = pairwise_all{[](auto, auto) { return false; }};
  const auto all_increasing = pairwise_all{std::less{}};

  SECTION ("always true for empty tuples") {
    CHECK(all_pairs_return_true());
    CHECK(all_pairs_return_false());
  }

  SECTION ("always true for single-element tuples") {
    CHECK(all_pairs_return_true(1));
    CHECK(all_pairs_return_false(3.14));
    CHECK(all_pairs_return_true('x'));
  }

  SECTION ("true for longer tuples iff true for all neighbouring pairs") {
    CHECK(all_increasing(1, 1.5));
    CHECK(all_increasing(1, 1.5, 2));

    CHECK(!all_increasing(1, 2.0, 2));
    CHECK(!all_increasing(1, 2.5, 2));

    CHECK(all_pairs_return_true('c', 1, 8.9, 42u));
    CHECK(!all_pairs_return_false('c', 1, 8.9, 42u));
  }
}

TEST_CASE("strictly_increasing")
{
  SECTION ("Empty input is sorted") {
    CHECK(strictly_increasing());
  }

  SECTION ("Single-element input is sorted") {
    CHECK(strictly_increasing(3));
    CHECK(strictly_increasing(15.42));
    CHECK(strictly_increasing('c'));
  }

  SECTION ("Multi-value inputs compare correctly") {
    CHECK(strictly_increasing(3, 3.14));
    CHECK(!strictly_increasing(3, 3.0));
    CHECK(!strictly_increasing(4, 3.0));
  }
}

}  // namespace
