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

#include <type_traits>
#include <cmath>

namespace units {

  // treat_as_floating_point

  template<typename Rep>  // TODO Conceptify that
  inline constexpr bool treat_as_floating_point = std::is_floating_point_v<Rep>;

  // // isnan
  // namespace isnan_impl {

  //   // non-ADL lookup block
  //   void isnan(); // undefined

  //   template<typename>
  //   inline constexpr bool has_customization = false;

  //   template<typename T>
  //     requires requires(const T& t) {
  //       { isnan(t) } -> bool;
  //     }
  //   inline constexpr bool has_customization<T> = true;

  //   struct fn {
  //     template<typename T>
  //     constexpr bool operator()(const T&) const
  //     {
  //       return false;
  //     }

  //     template<typename T>
  //       requires treat_as_floating_point<T>
  //     constexpr bool operator()(const T& value) const
  //     {
  //       return std::isnan(value);
  //     }

  //     template<typename T>
  //       requires treat_as_floating_point<T> && has_customization<T>
  //     constexpr bool operator()(const T& value) const
  //     {
  //       return isnan(value);  // uses ADL
  //     }
  //   };
  // }

  // inline constexpr isnan_impl::fn isnan{};

  // // isfinite
  // namespace isfinite_impl {

  //   // non-ADL lookup block
  //   void isfinite(); // undefined

  //   template<typename>
  //   inline constexpr bool has_customization = false;

  //   template<typename T>
  //     requires requires(const T& t) {
  //       { isfinite(t) } -> bool;
  //     }
  //   inline constexpr bool has_customization<T> = true;

  //   struct fn {
  //     template<typename T>
  //     constexpr bool operator()(const T&) const
  //     {
  //       return true;
  //     }

  //     template<typename T>
  //       requires treat_as_floating_point<T>
  //     constexpr bool operator()(const T& value) const
  //     {
  //       return std::isfinite(value);
  //     }

  //     template<typename T>
  //       requires treat_as_floating_point<T> && has_customization<T>
  //     constexpr bool operator()(const T& value) const
  //     {
  //       return isfinite(value);  // uses ADL
  //     }
  //   };
  // }

  // inline constexpr isfinite_impl::fn isfinite{};

}
