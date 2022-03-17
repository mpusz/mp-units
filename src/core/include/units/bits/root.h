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

#include <gsl/gsl-lite.hpp>
#include <units/bits/constexpr_math.h>
#include <units/bits/math_concepts.h>
#include <units/bits/pow.h>
#include <units/bits/ratio_maths.h>
#include <cmath>

namespace units::detail {

template<std::intmax_t N, typename F>
  requires gt_zero<N>
[[nodiscard]] constexpr std::intmax_t iroot_impl(std::intmax_t v, const F& pow_function) noexcept
{
  if constexpr (N == 1) {
    return v;
  } else {
    gsl_Expects(v >= 0);
    if (v == 0) {
      return 0;
    }

    constexpr double exponent = 1.0 / static_cast<double>(N);
    const auto root = static_cast<std::intmax_t>(pow_function(static_cast<double>(v), exponent));

    // integer roots may be truncated down by 1 or in even rarer cases up by 1 due to finite precision of pow and
    // exponent, check both cases
    if (v == pow_impl<N>(root + 1)) {
      return root + 1;
    }
    if (v < pow_impl<N>(root)) {
      return root - 1;
    }
    return root;
  }
}

// maximum v is std::numeric_limits<std::intmax_t>::max() which is the worst case for exp convergence
// ExpOrder = 12 and Factor = 64 give a precision of about O(1e-15) for a wide range of 1 / N exponents
// Factor = 32 needs quite a few more terms to converge
// https://godbolt.org/z/odWq1o
template<std::intmax_t N, std::size_t ExpOrder = 12, std::intmax_t Factor = 64>
  requires gt_zero<N>
[[nodiscard]] constexpr std::intmax_t iroot_compile(std::intmax_t v) noexcept
{
  return iroot_impl<N>(v, [](double x, double exponent) { return constexpr_pow<ExpOrder, Factor>(x, exponent); });
}

template<std::intmax_t N>
  requires gt_zero<N>
[[nodiscard]] std::intmax_t iroot_runtime(std::intmax_t v) noexcept
{
  return iroot_impl<N>(v, [](double x, [[maybe_unused]] double exponent) {
    if constexpr (N == 2) {
      return std::sqrt(x);
    } else if constexpr (N == 3) {
      return std::cbrt(x);
    } else {
      return std::pow(x, exponent);
    }
  });
}

template<std::intmax_t N>
  requires gt_zero<N>
[[nodiscard]] constexpr std::intmax_t iroot(std::intmax_t v) noexcept
{
  // compile time version is much slower, use faster version at runtime
  if (std::is_constant_evaluated()) {
    return iroot_compile<N>(v);
  }

  return iroot_runtime<N>(v);
}

}  // namespace units::detail
