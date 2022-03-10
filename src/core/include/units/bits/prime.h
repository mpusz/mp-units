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

#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <optional>

namespace units::detail
{

constexpr bool is_prime_by_trial_division(std::size_t n) {
  for (std::size_t f = 2; f * f <= n; f += 1 + (f % 2)) {
    if (n % f == 0) { return false; }
  }
  return true;
}

constexpr std::size_t num_primes_between(std::size_t start, std::size_t end) {
  std::size_t n = 0;

  for (auto k = start; k < end; ++k) {
    if (is_prime_by_trial_division(k)) { ++n; }
  }

  return n;
}

template<std::size_t Start, std::size_t End>
constexpr auto primes_between() {
  std::array<std::size_t, num_primes_between(Start, End)> results;
  auto iter = std::begin(results);

  for (auto k = Start; k < End; ++k) {
    if (is_prime_by_trial_division(k)) {
      *iter = k;
      ++iter;
    }
  }

  assert(iter == std::end(results));
  return results;
}

// Return the first factor of n, as long as it is either k or n.
//
// Precondition: no integer smaller than k evenly divides n.
constexpr std::optional<std::size_t> first_factor_maybe(std::size_t n, std::size_t k) {
  if (n % k == 0) { return k; }
  if (k * k > n) { return n; }
  return std::nullopt;
}

template<std::size_t N>
constexpr std::array<std::size_t, N> first_n_primes() {
  std::array<std::size_t, N> primes;
  primes[0] = 2;
  for (std::size_t i = 1; i < N; ++i) {
    primes[i] = primes[i - 1] + 1;
    while (!is_prime_by_trial_division(primes[i])) { ++primes[i]; }
  }
  return primes;
}

template<std::size_t N>
constexpr std::size_t product(const std::array<std::size_t, N> &values) {
  std::size_t product = 1;
  for (const auto &v : values) { product *= v; }
  return product;
}

// A configurable instantiation of the "wheel factorization" algorithm [1] for prime numbers.
//
// Instantiate with N to use a "basis" of the first N prime numbers.  Higher values of N use fewer trial divisions, at
// the cost of additional space.  The amount of space consumed is roughly the total number of prime numbers that are
// less than the _product_ of the first N prime numbers.  This means it grows rapidly with N.  Consider this approximate
// chart:
//
//   N | Num primes stored | Trial divisions needed ~= (stored + 1 - N) / product(basis)
//   --+-------------------+-----------------------
//   1 |                 1 |                 50.0 %
//   2 |                 3 |                 33.3 %
//   3 |                10 |                 26.7 %
//   4 |                46 |                 20.5 %
//   5 |               343 |                 14.7 %
//
// Consider this behaviour when choosing the value of N most appropriate for your needs, and watch out for diminishing
// returns.
//
// [1] https://en.wikipedia.org/wiki/Wheel_factorization
template<std::size_t BasisSize>
struct WheelFactorizer {
  static constexpr auto basis = first_n_primes<BasisSize>();
  static constexpr std::size_t wheel_size = product(basis);
  static constexpr auto primes_in_first_wheel = primes_between<basis.back() + 1, wheel_size>();

  static constexpr std::size_t find_first_factor(std::size_t n) {
    for (const auto& p : basis) {
      if (const auto k = first_factor_maybe(n, p)) { return *k; }
    }

    for (const auto& p : primes_in_first_wheel) {
      if (const auto k = first_factor_maybe(n, p)) { return *k; }
    }

    for (std::size_t wheel = wheel_size; wheel < n; wheel += wheel_size) {
      if (const auto k = first_factor_maybe(n, wheel + 1)) { return *k; }

      for (const auto &p : primes_in_first_wheel) {
        if (const auto k = first_factor_maybe(n, wheel + p)) { return *k; }
      }
    }

    return n;
  }

  static constexpr bool is_prime(std::size_t n) {
    return (n > 1) && find_first_factor(n) == n;
  }
};

} // namespace units::detail
