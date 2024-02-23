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

#include <mp-units/bits/external/algorithm.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <array>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <optional>
#include <tuple>
#endif

namespace mp_units::detail {

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
  std::array<std::uintmax_t, N> primes;
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
consteval void call_for_coprimes_up_to(std::uintmax_t n, const std::array<std::uintmax_t, N>& basis, Callable&& call)
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
  std::array<std::uintmax_t, ResultSize> coprimes;
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
