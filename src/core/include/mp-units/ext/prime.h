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
    n - mul_mod(n % a, num_batches, n),

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

[[nodiscard]] consteval bool is_prime_by_trial_division(std::uintmax_t n)
{
  for (std::uintmax_t f = 2; f * f <= n; f += 1 + (f % 2)) {
    if (n % f == 0) {
      return false;
    }
  }
  return true;
}

// Return the first factor of n, as long as it is either k or n.
//
// Precondition: no integer smaller than k evenly divides n.
[[nodiscard]] constexpr std::optional<std::uintmax_t> first_factor_maybe(std::uintmax_t n, std::uintmax_t k)
{
  if (n % k == 0) {
    return k;
  }
  if (k * k > n) {
    return n;
  }
  return std::nullopt;
}

template<std::size_t N>
[[nodiscard]] consteval std::array<std::uintmax_t, N> first_n_primes()
{
  std::array<std::uintmax_t, N> primes{};
  primes[0] = 2;
  for (std::size_t i = 1; i < N; ++i) {
    primes[i] = primes[i - 1] + 1;
    while (!is_prime_by_trial_division(primes[i])) {
      ++primes[i];
    }
  }
  return primes;
}

template<std::size_t N, typename Callable>
consteval void call_for_coprimes_up_to(std::uintmax_t n, const std::array<std::uintmax_t, N>& basis, Callable call)
{
  for (std::uintmax_t i = 0u; i < n; ++i) {
    if (std::apply([&i](auto... primes) { return ((i % primes != 0) && ...); }, basis)) {
      call(i);
    }
  }
}

template<std::size_t N>
[[nodiscard]] consteval std::size_t num_coprimes_up_to(std::uintmax_t n, const std::array<std::uintmax_t, N>& basis)
{
  std::size_t count = 0u;
  call_for_coprimes_up_to(n, basis, [&count](auto) { ++count; });
  return count;
}

template<std::size_t ResultSize, std::size_t N>
[[nodiscard]] consteval auto coprimes_up_to(std::size_t n, const std::array<std::uintmax_t, N>& basis)
{
  std::array<std::uintmax_t, ResultSize> coprimes{};
  std::size_t i = 0u;

  call_for_coprimes_up_to(n, basis, [&coprimes, &i](std::uintmax_t cp) { coprimes[i++] = cp; });

  return coprimes;
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

// A configurable instantiation of the "wheel factorization" algorithm [1] for prime numbers.
//
// Instantiate with N to use a "basis" of the first N prime numbers.  Higher values of N use fewer trial divisions, at
// the cost of additional space.  The amount of space consumed is roughly the total number of numbers that are a) less
// than the _product_ of the basis elements (first N primes), and b) coprime with every element of the basis.  This
// means it grows rapidly with N.  Consider this approximate chart:
//
//   N | Num coprimes | Trial divisions needed
//   --+--------------+-----------------------
//   1 |            1 |                 50.0 %
//   2 |            2 |                 33.3 %
//   3 |            8 |                 26.7 %
//   4 |           48 |                 22.9 %
//   5 |          480 |                 20.8 %
//
// Note the diminishing returns, and the rapidly escalating costs.  Consider this behaviour when choosing the value of N
// most appropriate for your needs.
//
// [1] https://en.wikipedia.org/wiki/Wheel_factorization
template<std::size_t BasisSize>
struct wheel_factorizer {
  static constexpr auto basis = first_n_primes<BasisSize>();
  static constexpr auto wheel_size = product(basis);
  static constexpr auto coprimes_in_first_wheel =
    coprimes_up_to<num_coprimes_up_to(wheel_size, basis)>(wheel_size, basis);

  [[nodiscard]] static consteval std::uintmax_t find_first_factor(std::uintmax_t n)
  {
    if (const auto k = detail::get_first_of(basis, [&](auto p) { return first_factor_maybe(n, p); })) return *k;

    if (const auto k = detail::get_first_of(std::next(begin(coprimes_in_first_wheel)), end(coprimes_in_first_wheel),
                                            [&](auto p) { return first_factor_maybe(n, p); }))
      return *k;

    for (std::size_t wheel = wheel_size; wheel < n; wheel += wheel_size)
      if (const auto k =
            detail::get_first_of(coprimes_in_first_wheel, [&](auto p) { return first_factor_maybe(n, wheel + p); }))
        return *k;
    return n;
  }

  [[nodiscard]] static consteval bool is_prime(std::size_t n) { return (n > 1) && find_first_factor(n) == n; }
};

}  // namespace mp_units::detail
