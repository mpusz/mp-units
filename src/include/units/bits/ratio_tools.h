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

#include <ratio>
#include <type_traits>

namespace units {

  // static_sign

  template<std::intmax_t Pn>
  struct static_sign : std::integral_constant<std::intmax_t, (Pn < 0) ? -1 : 1> {
  };

  // static_abs

  template<std::intmax_t Pn>
  struct static_abs : std::integral_constant<std::intmax_t, Pn * static_sign<Pn>::value> {
  };

  // static_gcd

  template<std::intmax_t Pn, std::intmax_t Qn>
  struct static_gcd : static_gcd<Qn, (Pn % Qn)> {
  };

  template<std::intmax_t Pn>
  struct static_gcd<Pn, 0> : std::integral_constant<std::intmax_t, static_abs<Pn>::value> {
  };

  template<std::intmax_t Qn>
  struct static_gcd<0, Qn> : std::integral_constant<std::intmax_t, static_abs<Qn>::value> {
  };

  // is_ratio

  namespace detail {

    template<typename T>
    inline constexpr bool is_ratio = false;

    template<intmax_t Num, intmax_t Den>
    inline constexpr bool is_ratio<std::ratio<Num, Den>> = true;

  }  // namespace detail

  template<typename T>
  concept bool Ratio = detail::is_ratio<T>;

  // common_ratio
  namespace detail {

    // todo: simplified
    template<typename R1, typename R2>
    struct common_ratio_impl {
      using gcd_num = static_gcd<R1::num, R2::num>;
      using gcd_den = static_gcd<R1::den, R2::den>;
      using type = std::ratio<gcd_num::value, (R1::den / gcd_den::value) * R2::den>;
    };

  }

  template<Ratio R1, Ratio R2>
  using common_ratio = detail::common_ratio_impl<R1, R2>::type;

}  // namespace units
