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

#include "stdconcepts.h"
#include <ratio>
#include <type_traits>

namespace std {

  template<typename T>
  struct type_identity { using type = T; };

  template<typename T>
  using type_identity_t = typename type_identity<T>::type;

}

namespace units {

  using namespace mp::std_concepts;  // todo Remove when std::concepts will arrive

  template<typename T>
  concept bool Number = requires(T a, T b) {
    { a + b } -> T;
    { a - b } -> T;
    { a * b } -> T;
    { a / b } -> T;
    { -a } -> T;
    { a += b } -> T&;
    { a -= b } -> T&;
    { a *= b } -> T&;
    { a /= b } -> T&;
    { T{0} };// can construct a T from a zero
    // …
  } ;


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

  // todo: simplified
  template<Ratio Ratio1, Ratio Ratio2>
  struct common_ratio {
    using gcd_num = static_gcd<Ratio1::num, Ratio2::num>;
    using gcd_den = static_gcd<Ratio1::den, Ratio2::den>;
    using type = std::ratio<gcd_num::value, (Ratio1::den / gcd_den::value) * Ratio2::den>;
  };

  template<Ratio Ratio1, Ratio Ratio2>
  using common_ratio_t = typename common_ratio<Ratio1, Ratio2>::type;

  // upcasting_traits

  template<typename T>
  struct upcasting_traits : std::type_identity<T> {};

  template<typename T>
  using upcasting_traits_t = typename upcasting_traits<T>::type;


}  // namespace units
