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

// To be replaced with:
// P3133: Fast first-factor finding function

#pragma once

#include <mp-units/ext/algorithm.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/ext/contracts.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <array>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <optional>
#include <tuple>
#endif
#endif

namespace mp_units::detail {

// (a + b) % n.
//
// Precondition: (a < n).
// Precondition: (b < n).
// Precondition: (n > 0).
[[nodiscard]] consteval std::uint64_t add_mod(std::uint64_t a, std::uint64_t b, std::uint64_t n)
{
  MP_UNITS_EXPECTS_DEBUG(a < n);
  MP_UNITS_EXPECTS_DEBUG(b < n);
  MP_UNITS_EXPECTS_DEBUG(n > 0u);

  if (a >= n - b) {
    return a - (n - b);
  } else {
    return a + b;
  }
}

// (a - b) % n.
//
// Precondition: (a < n).
// Precondition: (b < n).
// Precondition: (n > 0).
[[nodiscard]] consteval std::uint64_t sub_mod(std::uint64_t a, std::uint64_t b, std::uint64_t n)
{
  MP_UNITS_EXPECTS_DEBUG(a < n);
  MP_UNITS_EXPECTS_DEBUG(b < n);
  MP_UNITS_EXPECTS_DEBUG(n > 0u);

  if (a >= b) {
    return a - b;
  } else {
    return n - (b - a);
  }
}

// (a * b) % n.
//
// Precondition: (a < n).
// Precondition: (b < n).
// Precondition: (n > 0).
[[nodiscard]] consteval std::uint64_t mul_mod(std::uint64_t a, std::uint64_t b, std::uint64_t n)
{
  MP_UNITS_EXPECTS_DEBUG(a < n);
  MP_UNITS_EXPECTS_DEBUG(b < n);
  MP_UNITS_EXPECTS_DEBUG(n > 0u);

  if (b == 0u || a < std::numeric_limits<std::uint64_t>::max() / b) {
    return (a * b) % n;
  }

  const std::uint64_t batch_size = n / a;
  const std::uint64_t num_batches = b / batch_size;

  return add_mod(
    // Transform into "negative space" to make the first parameter as small as possible;
    // then, transform back.
    (n - mul_mod(n % a, num_batches, n)) % n,

    // Handle the leftover product (which is guaranteed to fit in the integer type).
    (a * (b % batch_size)) % n,

    n);
}

// (a / 2) % n.
//
// Precondition: (a < n).
// Precondition: (n % 2 == 1).
[[nodiscard]] consteval std::uint64_t half_mod_odd(std::uint64_t a, std::uint64_t n)
{
  MP_UNITS_EXPECTS_DEBUG(a < n);
  MP_UNITS_EXPECTS_DEBUG(n % 2 == 1);

  return (a / 2u) + ((a % 2u == 0u) ? 0u : (n / 2u + 1u));
}

// (base ^ exp) % n.
[[nodiscard]] consteval std::uint64_t pow_mod(std::uint64_t base, std::uint64_t exp, std::uint64_t n)
{
  std::uint64_t result = 1u;
  base %= n;

  while (exp > 0u) {
    if (exp % 2u == 1u) {
      result = mul_mod(result, base, n);
    }

    exp /= 2u;
    base = mul_mod(base, base, n);
  }

  return result;
}

// Decompose a positive integer by factoring out all powers of 2.
struct NumberDecomposition {
  std::uint64_t power_of_two;
  std::uint64_t odd_remainder;
};

// Express any positive `n` as `(2^s * d)`, where `d` is odd.
[[nodiscard]] consteval NumberDecomposition decompose(std::uint64_t n)
{
  NumberDecomposition result{0u, n};
  while (result.odd_remainder % 2u == 0u) {
    result.odd_remainder /= 2u;
    ++result.power_of_two;
  }
  return result;
}

// Perform a Miller-Rabin primality test on `n` using base `a`.
//
// Precondition: (a >= 2).
// Precondition: (n >= a + 2).
// Precondition: (n % 2 == 1).
[[nodiscard]] consteval bool miller_rabin_probable_prime(std::uint64_t a, std::uint64_t n)
{
  MP_UNITS_EXPECTS_DEBUG(a >= 2u);
  MP_UNITS_EXPECTS_DEBUG(n >= a + 2u);
  MP_UNITS_EXPECTS_DEBUG(n % 2u == 1u);

  const auto [s, d] = decompose(n - 1u);
  auto x = pow_mod(a, d, n);
  if (x == 1u) {
    return true;
  }

  const auto minus_one = n - 1u;
  for (auto r = 0u; r < s; ++r) {
    if (x == minus_one) {
      return true;
    }
    x = mul_mod(x, x, n);
  }

  return false;
}

// The Jacobi symbol, notated as `(a/n)`, is defined for odd positive `n` and any integer `a`, taking values
// in the set `{-1, 0, 1}`.  Besides being a completely multiplicative function (so that, for example, both
// (a*b/n) = (a/n) * (b/n), and (a/n*m) = (a/n) * (a/m)), it obeys the following symmetry rules, which enable
// its calculation:
//
//  1. (a/1) = 1, and (1/n) = 1, for all a and n.
//
//  2. (a/n) = 0 whenever a and n have a nontrivial common factor.
//
//  3. (a/n) = (b/n) whenever (a % n) = (b % n).
//
//  4. (2a/n) = (a/n) if n % 8 = 1 or 7, and -(a/n) if n % 8 = 3 or 5.
//
//  5. (a/n) = (n/a) * x if a and n are both odd, positive, and coprime.  Here, x is 1 if either (a % 4) = 1
//     or (n % 4) = 1, and -1 otherwise.
//
//  6. (-1/n) = 1 if n % 4 = 1, and -1 if n % 4 = 3.
[[nodiscard]] consteval int jacobi_symbol(std::int64_t raw_a, std::uint64_t n)
{
  // Rule 1: n=1 case.
  if (n == 1u) {
    return 1;
  }

  // Starting conditions: transform `a` to strictly non-negative values, setting `result` to the sign that we
  // pick up (if any) from following these rules (i.e., rules 3 and 6).
  int result = ((raw_a >= 0) || (n % 4u == 1u)) ? 1 : -1;
  auto a = static_cast<std::uint64_t>(raw_a < 0 ? -raw_a : raw_a) % n;

  while (a != 0u) {
    // Rule 4.
    const int sign_for_even = (n % 8u == 1u || n % 8u == 7u) ? 1 : -1;
    while (a % 2u == 0u) {
      a /= 2u;
      result *= sign_for_even;
    }

    // Rule 1: a=1 case.
    if (a == 1u) {
      return result;
    }

    // Rule 2.
    if (std::gcd(a, n) != 1u) {
      return 0;
    }

    // Note that at this point, we know that `a` and `n` are coprime, and are both odd and positive.
    // Therefore, we meet the preconditions for rule 5 (the "flip-and-reduce" rule).
    result *= (n % 4u == 1u || a % 4u == 1u) ? 1 : -1;
    const std::uint64_t new_a = n % a;
    n = a;
    a = new_a;
  }

  return 0;
}

[[nodiscard]] consteval bool is_perfect_square(std::uint64_t n)
{
  if (n < 2u) {
    return true;
  }

  std::uint64_t prev = n / 2u;
  while (true) {
    const std::uint64_t curr = (prev + n / prev) / 2u;
    if (curr * curr == n) {
      return true;
    }
    if (curr >= prev) {
      return false;
    }
    prev = curr;
  }
}

// The "D" parameter in the Strong Lucas Probable Prime test.
//
// Default construction produces the first value to try, according to Selfridge's parameter selection.
// Calling `successor()` on this repeatedly will produce the sequence of values to try.
struct LucasDParameter {
  std::uint64_t mag = 5u;
  bool pos = true;

  friend constexpr int as_int(LucasDParameter p)
  {
    int D = static_cast<int>(p.mag);
    return p.pos ? D : -D;
  }
  friend constexpr LucasDParameter successor(LucasDParameter p) { return {.mag = p.mag + 2u, .pos = !p.pos}; }
};

// The first `D` in the infinite sequence {5, -7, 9, -11, ...} whose Jacobi symbol is -1.  This is the D we
// want to use for the Strong Lucas Probable Prime test.
//
// Precondition: `n` is not a perfect square.
[[nodiscard]] consteval LucasDParameter find_first_D_with_jacobi_symbol_neg_one(std::uint64_t n)
{
  LucasDParameter D{};
  while (jacobi_symbol(as_int(D), n) != -1) {
    D = successor(D);
  }
  return D;
}

// An element of the Lucas sequence, with parameters tuned for the Strong Lucas test.
//
// The default values give the first element (i.e., k=1) of the sequence.
struct LucasSequenceElement {
  std::uint64_t u = 1u;
  std::uint64_t v = 1u;
};

// Produce the Lucas element whose index is twice the input element's index.
[[nodiscard]] consteval LucasSequenceElement double_strong_lucas_index(const LucasSequenceElement& element,
                                                                       std::uint64_t n, LucasDParameter D)
{
  const auto& [u, v] = element;

  std::uint64_t v_squared = mul_mod(v, v, n);
  std::uint64_t D_u_squared = mul_mod(D.mag % n, mul_mod(u, u, n), n);
  std::uint64_t new_v = D.pos ? add_mod(v_squared, D_u_squared, n) : sub_mod(v_squared, D_u_squared, n);
  new_v = half_mod_odd(new_v, n);

  return {.u = mul_mod(u, v, n), .v = new_v};
}

[[nodiscard]] consteval LucasSequenceElement increment_strong_lucas_index(const LucasSequenceElement& element,
                                                                          std::uint64_t n, LucasDParameter D)
{
  const auto& [u, v] = element;

  const auto new_u = half_mod_odd(add_mod(u, v, n), n);

  const auto D_u = mul_mod(D.mag % n, u, n);
  auto new_v = D.pos ? add_mod(v, D_u, n) : sub_mod(v, D_u, n);
  new_v = half_mod_odd(new_v, n);

  return {.u = new_u, .v = new_v};
}

[[nodiscard]] consteval LucasSequenceElement find_strong_lucas_element(std::uint64_t i, std::uint64_t n,
                                                                       LucasDParameter D)
{
  LucasSequenceElement element{};

  bool bits[64] = {};
  std::size_t n_bits = 0u;
  while (i > 1u) {
    bits[n_bits++] = (i & 1u);
    i >>= 1u;
  }

  for (auto j = n_bits; j > 0u; --j) {
    element = double_strong_lucas_index(element, n, D);
    if (bits[j - 1u]) {
      element = increment_strong_lucas_index(element, n, D);
    }
  }

  return element;
}

// Perform a Strong Lucas Probable Prime test on `n`.
//
// Precondition: (n >= 2).
// Precondition: (n is odd).
[[nodiscard]] consteval bool strong_lucas_probable_prime(std::uint64_t n)
{
  MP_UNITS_EXPECTS_DEBUG(n >= 2u);
  MP_UNITS_EXPECTS_DEBUG(n % 2u == 1u);

  if (is_perfect_square(n)) {
    return false;
  }

  const auto D = find_first_D_with_jacobi_symbol_neg_one(n);

  const auto [s, d] = decompose(n + 1u);

  auto element = find_strong_lucas_element(d, n, D);
  if (element.u == 0u) {
    return true;
  }

  for (auto i = 0u; i < s; ++i) {
    if (element.v == 0u) {
      return true;
    }
    element = double_strong_lucas_index(element, n, D);
  }

  return false;
}

// The Baillie-PSW test is technically a "probable prime" test.  However, it is known to be correct for all
// 64-bit integers, and no counterexample of any size has ever been found.  Thus, for this library's purposes,
// we can treat it as deterministic... probably.
[[nodiscard]] consteval bool baillie_psw_probable_prime(uint64_t n)
{
  if (n < 2u) {
    return false;
  }
  if (n < 4u) {
    return true;
  }
  if (n % 2u == 0u) {
    return false;
  }

  if (!miller_rabin_probable_prime(2u, n)) {
    return false;
  }

  return strong_lucas_probable_prime(n);
}

template<std::size_t N>
[[nodiscard]] consteval std::array<std::uintmax_t, N> first_n_primes()
{
  std::array<std::uintmax_t, N> primes{};
  primes[0] = 2;
  for (std::size_t i = 1; i < N; ++i) {
    primes[i] = primes[i - 1] + 1;
    while (!baillie_psw_probable_prime(primes[i])) {
      ++primes[i];
    }
  }
  return primes;
}

template<std::size_t N>
[[nodiscard]] consteval std::uintmax_t product(const std::array<std::uintmax_t, N>& values)
{
  std::uintmax_t product = 1;
  for (const auto& v : values) {
    product *= v;
  }
  return product;
}

template<class InputIt, class UnaryFunction>
constexpr std::invoke_result_t<UnaryFunction, std::iter_value_t<InputIt>> get_first_of(InputIt first, InputIt last,
                                                                                       UnaryFunction f)
{
  for (; first != last; ++first)
    if (auto opt = f(*first)) return opt;
  return {};
}

template<class Rng, class UnaryFunction>
constexpr auto get_first_of(const Rng& rng, UnaryFunction f)
{
  using std::begin, std::end;
  return get_first_of(begin(rng), end(rng), f);
}

[[nodiscard]] consteval std::uintmax_t find_first_factor(std::uintmax_t n)
{
  constexpr auto first_100_primes = first_n_primes<100>();

  for (const auto& p : first_100_primes) {
    if (n % p == 0u) {
      return p;
    }
    if (p * p > n) {
      return n;
    }
  }

  // If we got this far, and we haven't found a factor, and haven't terminated... maybe it's prime?  Do a fast check.
  if (baillie_psw_probable_prime(n)) {
    return n;
  }

  // If we're here, we know `n` is composite, so continue with trial division for all odd numbers.
  std::uintmax_t factor = first_100_primes.back() + 2u;
  while (factor * factor <= n) {
    if (n % factor == 0u) {
      return factor;
    }
    factor += 2u;
  }

  return n;  // Technically unreachable.
}

}  // namespace mp_units::detail
