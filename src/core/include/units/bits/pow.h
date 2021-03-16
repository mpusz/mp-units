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

#include <cstdint>
#include <cassert>

namespace units::detail {

constexpr std::intmax_t ipow10(std::intmax_t exp)
{
  assert(exp >= 0);
  if (exp == 0) return 1;
  std::intmax_t result = 1;
  while (exp > 0) {
    result *= 10;
    --exp;
  }
  return result;
}

template<typename Rep>
constexpr Rep fpow10(std::intmax_t exp)
{
  if (exp == 0) return Rep(1.0);
  Rep result = Rep(1.0);
  if (exp < 0) {
    while (exp < 0) {
      result = result / Rep(10.0);
      ++exp;
    }
  } else {
    while (exp > 0) {
      result = result * Rep(10.0);
      --exp;
    }
  }
  return result;
}

template<std::intmax_t N, typename T>
constexpr T pow_impl(const T& v) noexcept
{
  if constexpr (N == 0) {
    return T(1);
  } else if constexpr (N == 1) {
    return v;
  } else if constexpr (N < 0) {
    return 1 / pow_impl<-N>(v);
  } else if constexpr (N % 2 == 0) {  // even
    return pow_impl<N / 2>(v * v);
  } else {  // odd
    return v * pow_impl<(N - 1) / 2>(v * v);
  }
}

}  // namespace units::detail
