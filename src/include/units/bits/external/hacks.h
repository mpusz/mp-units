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

#if __clang__
#define COMP_CLANG __clang_major__
#elif __GNUC__
#define COMP_GCC __GNUC__
#define COMP_GCC_MINOR __GNUC_MINOR__
#elif _MSC_VER
#define COMP_MSVC _MSC_VER
#endif

#if defined(COMP_CLANG) && !defined(UNITS_LIBCXX)
#define UNITS_LIBCXX 0
#endif

#if COMP_CLANG && UNITS_LIBCXX

#include <concepts/compare.hpp>
#include <concepts/concepts.hpp>
#include <range/v3/functional/comparisons.hpp>

#endif

#include <concepts>
#include <compare>

#if COMP_MSVC || COMP_CLANG

#define TYPENAME typename

#else

#define TYPENAME

#endif


namespace std {

#if COMP_GCC

template<class T>
concept default_constructible = constructible_from<T>;

#elif COMP_CLANG && UNITS_LIBCXX

// concepts
using concepts::three_way_comparable;
using concepts::three_way_comparable_with;
// using concepts::common_reference_with;
using concepts::common_with;
using concepts::constructible_from;
using concepts::convertible_to;
// using concepts::default_constructible;
using concepts::derived_from;
// using concepts::equality_comparable;
using concepts::equality_comparable_with;
// // using concepts::floating_point;
// using concepts::integral;
using concepts::regular;
// using concepts::same_as;
// using concepts::totally_ordered;
// using concepts::totally_ordered_with;

using ranges::compare_three_way;

// namespace ranges {

// using ::ranges::forward_range;
// using ::ranges::range_value_t;

// }

// // missing in Range-v3
// template<class T>
// concept floating_point = std::is_floating_point_v<T>;

template<class F, class... Args>
concept invocable =
requires(F&& f, Args&&... args) {
  std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
};

template<class F, class... Args>
concept regular_invocable = invocable<F, Args...>;

template<class T, class U>
constexpr bool cmp_equal(T t, U u) noexcept
{
  using UT = std::make_unsigned_t<T>;
  using UU = std::make_unsigned_t<U>;
  if constexpr (std::is_signed_v<T> == std::is_signed_v<U>)
      return t == u;
  else if constexpr (std::is_signed_v<T>)
      return t < 0 ? false : UT(t) == u;
  else
      return u < 0 ? false : t == UU(u);
}
 
template<class T, class U>
constexpr bool cmp_not_equal(T t, U u) noexcept
{
  return !cmp_equal(t, u);
}
 
template<class T, class U>
constexpr bool cmp_less(T t, U u) noexcept
{
  using UT = std::make_unsigned_t<T>;
  using UU = std::make_unsigned_t<U>;
  if constexpr (std::is_signed_v<T> == std::is_signed_v<U>)
      return t < u;
  else if constexpr (std::is_signed_v<T>)
      return t < 0 ? true : UT(t) < u;
  else
      return u < 0 ? false : t < UU(u);
}
 
template<class T, class U>
constexpr bool cmp_greater(T t, U u) noexcept
{
  return cmp_less(u, t);
}
 
template<class T, class U>
constexpr bool cmp_less_equal(T t, U u) noexcept
{
  return !cmp_greater(t, u);
}
 
template<class T, class U>
constexpr bool cmp_greater_equal(T t, U u) noexcept
{
  return !cmp_less(t, u);
}

template<class R, class T>
constexpr bool in_range(T t) noexcept
{
  return std::cmp_greater_equal(t, std::numeric_limits<R>::min()) &&
        std::cmp_less_equal(t, std::numeric_limits<R>::max());
}

#endif

} // namespace std
