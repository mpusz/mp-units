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

#include <mp-units/ext/prime.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <array>
#include <cstddef>
#include <utility>
#endif

using namespace mp_units::detail;

namespace {

inline constexpr auto MAX_U64 = std::numeric_limits<std::uint64_t>::max();

template<std::size_t BasisSize, std::size_t... Is>
constexpr bool check_primes(std::index_sequence<Is...>)
{
  return ((Is < 2 || wheel_factorizer<BasisSize>::is_prime(Is) == is_prime_by_trial_division(Is)) && ...);
}

static_assert(check_primes<2>(std::make_index_sequence<122>{}));

// This is the smallest number that can catch the bug where we use only _prime_ numbers in the first wheel, rather than
// numbers which are _coprime to the basis_.
//
// The basis for N = 4 is {2, 3, 5, 7}, so the wheel size is 210.  11 * 11 = 121 is within the first wheel.  It is
// coprime with every element of the basis, but it is _not_ prime.  If we keep only prime numbers, then we will neglect
// using numbers of the form (210 * n + 121) as trial divisors, which is a problem if any are prime.  For n = 1, we have
// a divisor of (210 + 121 = 331), which happens to be prime but will not be used.  Thus, (331 * 331 = 109561) is a
// composite number which could wrongly appear prime if we skip over 331.
static_assert(wheel_factorizer<4>::is_prime(109'561) == is_prime_by_trial_division(109'561));

static_assert(wheel_factorizer<1>::coprimes_in_first_wheel.size() == 1);
static_assert(wheel_factorizer<2>::coprimes_in_first_wheel.size() == 2);
static_assert(wheel_factorizer<3>::coprimes_in_first_wheel.size() == 8);
static_assert(wheel_factorizer<4>::coprimes_in_first_wheel.size() == 48);
static_assert(wheel_factorizer<5>::coprimes_in_first_wheel.size() == 480);

static_assert(wheel_factorizer<3>::coprimes_in_first_wheel[0] == 1);
static_assert(wheel_factorizer<3>::coprimes_in_first_wheel[1] == 7);
static_assert(wheel_factorizer<3>::coprimes_in_first_wheel[2] == 11);
static_assert(wheel_factorizer<3>::coprimes_in_first_wheel[3] == 13);
static_assert(wheel_factorizer<3>::coprimes_in_first_wheel[4] == 17);
static_assert(wheel_factorizer<3>::coprimes_in_first_wheel[5] == 19);
static_assert(wheel_factorizer<3>::coprimes_in_first_wheel[6] == 23);
static_assert(wheel_factorizer<3>::coprimes_in_first_wheel[7] == 29);

static_assert(!wheel_factorizer<1>::is_prime(0));
static_assert(!wheel_factorizer<1>::is_prime(1));
static_assert(wheel_factorizer<1>::is_prime(2));

static_assert(!wheel_factorizer<2>::is_prime(0));
static_assert(!wheel_factorizer<2>::is_prime(1));
static_assert(wheel_factorizer<2>::is_prime(2));

static_assert(!wheel_factorizer<3>::is_prime(0));
static_assert(!wheel_factorizer<3>::is_prime(1));
static_assert(wheel_factorizer<3>::is_prime(2));

// Modular arithmetic.
static_assert(add_mod(1u, 2u, 5u) == 3u);
static_assert(add_mod(4u, 4u, 5u) == 3u);
static_assert(add_mod(MAX_U64 - 1u, MAX_U64 - 2u, MAX_U64) == MAX_U64 - 3u);

static_assert(sub_mod(2u, 1u, 5u) == 1u);
static_assert(sub_mod(1u, 2u, 5u) == 4u);
static_assert(sub_mod(MAX_U64 - 2u, MAX_U64 - 1u, MAX_U64) == MAX_U64 - 1u);
static_assert(sub_mod(1u, MAX_U64 - 1u, MAX_U64) == 2u);

static_assert(mul_mod(6u, 7u, 10u) == 2u);
static_assert(mul_mod(13u, 11u, 50u) == 43u);
static_assert(mul_mod(MAX_U64 / 2u, 10u, MAX_U64) == MAX_U64 - 5u);

static_assert(half_mod_odd(0u, 11u) == 0u);
static_assert(half_mod_odd(10u, 11u) == 5u);
static_assert(half_mod_odd(1u, 11u) == 6u);
static_assert(half_mod_odd(9u, 11u) == 10u);
static_assert(half_mod_odd(MAX_U64 - 1u, MAX_U64) == (MAX_U64 - 1u) / 2u);
static_assert(half_mod_odd(MAX_U64 - 2u, MAX_U64) == MAX_U64 - 1u);

static_assert(pow_mod(5u, 8u, 9u) == ((5u * 5u * 5u * 5u) * (5u * 5u * 5u * 5u)) % 9u);
static_assert(pow_mod(2u, 64u, MAX_U64) == 1u);

// Miller-Rabin primality testing.
static_assert(miller_rabin_probable_prime(2u, 5u));
static_assert(miller_rabin_probable_prime(2u, 7u));
static_assert(!miller_rabin_probable_prime(2u, 9u));
static_assert(miller_rabin_probable_prime(2u, 11u));

static_assert(miller_rabin_probable_prime(2u, 2047u), "Known base 2 pseudoprime");
static_assert(miller_rabin_probable_prime(2u, 3277u), "Known base 2 pseudoprime");

static_assert(miller_rabin_probable_prime(3u, 121u), "Known base 3 pseudoprime");
static_assert(miller_rabin_probable_prime(3u, 703u), "Known base 3 pseudoprime");

static_assert(miller_rabin_probable_prime(2u, 225'653'407'801u), "Large known prime");
static_assert(miller_rabin_probable_prime(2u, 334'524'384'739u), "Large known prime");
static_assert(miller_rabin_probable_prime(2u, 9'007'199'254'740'881u), "Large known prime");

static_assert(miller_rabin_probable_prime(2u, 18'446'744'073'709'551'557u), "Largest 64-bit prime");

// Jacobi symbols --- a building block for the Strong Lucas probable prime test, needed for Baillie-PSW.
static_assert(jacobi_symbol(1, 1u) == 1, "Jacobi symbol always 1 when 'numerator' is 1");
static_assert(jacobi_symbol(1, 3u) == 1, "Jacobi symbol always 1 when 'numerator' is 1");
static_assert(jacobi_symbol(1, 5u) == 1, "Jacobi symbol always 1 when 'numerator' is 1");
static_assert(jacobi_symbol(1, 987654321u) == 1, "Jacobi symbol always 1 when 'numerator' is 1");

static_assert(jacobi_symbol(3, 1u) == 1, "Jacobi symbol always 1 when 'denominator' is 1");
static_assert(jacobi_symbol(5, 1u) == 1, "Jacobi symbol always 1 when 'denominator' is 1");
static_assert(jacobi_symbol(-1234567890, 1u) == 1, "Jacobi symbol always 1 when 'denominator' is 1");

static_assert(jacobi_symbol(10, 5u) == 0, "Jacobi symbol always 0 when there's a common factor");
static_assert(jacobi_symbol(25, 15u) == 0, "Jacobi symbol always 0 when there's a common factor");
static_assert(jacobi_symbol(-24, 9u) == 0, "Jacobi symbol always 0 when there's a common factor");

static_assert(jacobi_symbol(14, 9u) == +jacobi_symbol(7, 9u),
              "Divide numerator by 2: positive when (denom % 8) in {1, 7}");
static_assert(jacobi_symbol(14, 15u) == +jacobi_symbol(7, 15u),
              "Divide numerator by 2: positive when (denom % 8) in {1, 7}");
static_assert(jacobi_symbol(14, 11u) == -jacobi_symbol(7, 11u),
              "Divide numerator by 2: negative when (denom % 8) in {3, 5}");
static_assert(jacobi_symbol(14, 13u) == -jacobi_symbol(7, 13u),
              "Divide numerator by 2: negative when (denom % 8) in {3, 5}");

static_assert(jacobi_symbol(19, 9u) == +jacobi_symbol(9, 19u), "Flip is identity when (n % 4) = 1");
static_assert(jacobi_symbol(17, 7u) == +jacobi_symbol(7, 17u), "Flip is identity when (a % 4) = 1");
static_assert(jacobi_symbol(19, 7u) == -jacobi_symbol(9, 7u), "Flip changes sign when (n % 4) = 3 and (a % 4) = 3");

static_assert(jacobi_symbol(1001, 9907u) == -1, "Example from Wikipedia page");
static_assert(jacobi_symbol(19, 45u) == 1, "Example from Wikipedia page");
static_assert(jacobi_symbol(8, 21u) == -1, "Example from Wikipedia page");
static_assert(jacobi_symbol(5, 21u) == 1, "Example from Wikipedia page");

// Tests for perfect square finder
static_assert(is_perfect_square(0u));
static_assert(is_perfect_square(1u));
static_assert(!is_perfect_square(2u));
static_assert(is_perfect_square(4u));

constexpr std::uint64_t BIG_SQUARE = [](auto x) { return x * x; }((std::uint64_t{1u} << 32) - 1u);
static_assert(!is_perfect_square(BIG_SQUARE - 1u));
static_assert(is_perfect_square(BIG_SQUARE));
static_assert(!is_perfect_square(BIG_SQUARE + 1u));

// Tests for the Strong Lucas Probable Prime test.
static_assert(as_int(LucasDParameter{.mag = 5, .pos = true}) == 5);
static_assert(as_int(LucasDParameter{.mag = 7, .pos = false}) == -7);

static_assert(as_int(LucasDParameter{}) == 5, "First D parameter in the sequence is 5");
static_assert(as_int(successor(LucasDParameter{})) == -7, "Incrementing adds 2 to the mag, and flips the sign");
static_assert(as_int(successor(successor(LucasDParameter{}))) == 9);
static_assert(as_int(successor(successor(successor(LucasDParameter{})))) == -11);

static_assert(strong_lucas_probable_prime(3u), "Known small prime");
static_assert(strong_lucas_probable_prime(5u), "Known small prime");
static_assert(strong_lucas_probable_prime(7u), "Known small prime");
static_assert(!strong_lucas_probable_prime(9u), "Known small composite");

// Test some Miller-Rabin pseudoprimes (https://oeis.org/A001262), which should NOT be marked prime.
static_assert(!strong_lucas_probable_prime(2047u), "Miller-Rabin pseudoprime");
static_assert(!strong_lucas_probable_prime(3277u), "Miller-Rabin pseudoprime");
static_assert(!strong_lucas_probable_prime(486737u), "Miller-Rabin pseudoprime");

// Test some Strong Lucas pseudoprimes (https://oeis.org/A217255).
static_assert(strong_lucas_probable_prime(5459u), "Strong Lucas pseudoprime");
static_assert(strong_lucas_probable_prime(5777u), "Strong Lucas pseudoprime");
static_assert(strong_lucas_probable_prime(10877u), "Strong Lucas pseudoprime");
static_assert(strong_lucas_probable_prime(324899u), "Strong Lucas pseudoprime");

// Test some actual primes
static_assert(strong_lucas_probable_prime(225'653'407'801u), "Large known prime");
static_assert(strong_lucas_probable_prime(334'524'384'739u), "Large known prime");
static_assert(strong_lucas_probable_prime(9'007'199'254'740'881u), "Large known prime");

static_assert(strong_lucas_probable_prime(18'446'744'073'709'551'557u), "Largest 64-bit prime");

// Tests for Baillie-PSW, which is known to be correct for all 64-bit integers.
static_assert(baillie_psw_probable_prime(3u), "Known small prime");
static_assert(baillie_psw_probable_prime(5u), "Known small prime");
static_assert(baillie_psw_probable_prime(7u), "Known small prime");
static_assert(!baillie_psw_probable_prime(9u), "Known small composite");

// Test some Miller-Rabin pseudoprimes (https://oeis.org/A001262), which should NOT be marked prime.
static_assert(!baillie_psw_probable_prime(2047u), "Miller-Rabin pseudoprime");
static_assert(!baillie_psw_probable_prime(3277u), "Miller-Rabin pseudoprime");
static_assert(!baillie_psw_probable_prime(486737u), "Miller-Rabin pseudoprime");

// Test some Strong Lucas pseudoprimes (https://oeis.org/A217255), which should NOT be marked prime.
static_assert(!baillie_psw_probable_prime(5459u), "Strong Lucas pseudoprime");
static_assert(!baillie_psw_probable_prime(5777u), "Strong Lucas pseudoprime");
static_assert(!baillie_psw_probable_prime(10877u), "Strong Lucas pseudoprime");
static_assert(!baillie_psw_probable_prime(324899u), "Strong Lucas pseudoprime");

// Test some actual primes
static_assert(baillie_psw_probable_prime(225'653'407'801u), "Large known prime");
static_assert(baillie_psw_probable_prime(334'524'384'739u), "Large known prime");
static_assert(baillie_psw_probable_prime(9'007'199'254'740'881u), "Large known prime");

static_assert(baillie_psw_probable_prime(18'446'744'073'709'551'557u), "Largest 64-bit prime");

}  // namespace
