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

// Copy-pasted C++ standard libraries to be replaced with `import std;` when available
// `#include <algorithm.h>` is too heavy to do in every translation unit

#pragma once

#include <mp-units/bits/hacks.h>  // IWYU pragma: keep

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <compare>
#include <initializer_list>
#include <iterator>
#endif

namespace mp_units::detail {

template<class InputIt, class UnaryPred>
constexpr InputIt find_if(InputIt first, InputIt last, UnaryPred p)
{
  for (; first != last; ++first)
    if (p(*first)) return first;

  return last;
}

template<class InputIt, class UnaryPred>
constexpr InputIt find_if_not(InputIt first, InputIt last, UnaryPred q)
{
  for (; first != last; ++first)
    if (!q(*first)) return first;

  return last;
}

template<class InputIt, class ForwardIt>
constexpr InputIt find_first_of(InputIt first, InputIt last, ForwardIt s_first, ForwardIt s_last)
{
  for (; first != last; ++first) {
    for (ForwardIt it = s_first; it != s_last; ++it) {
      if (*first == *it) {
        return first;
      }
    }
  }
  return last;
}

template<class InputIt, class UnaryPred>
constexpr bool all_of(InputIt first, InputIt last, UnaryPred p)
{
  return find_if_not(first, last, p) == last;
}

template<class InputIt1, class InputIt2>
constexpr bool equal(InputIt1 first1, InputIt1 last1, InputIt2 first2)
{
  for (; first1 != last1; ++first1, ++first2) {
    if (!(*first1 == *first2)) {
      return false;
    }
  }
  return true;
}

template<class I1, class I2, class Cmp>
constexpr auto lexicographical_compare_three_way(I1 first1, I1 last1, I2 first2, I2 last2, Cmp comp)
  -> decltype(comp(*first1, *first2))
{
  using ret_t = decltype(comp(*first1, *first2));
  static_assert(std::disjunction_v<std::is_same<ret_t, std::strong_ordering>, std::is_same<ret_t, std::weak_ordering>,
                                   std::is_same<ret_t, std::partial_ordering>>,
                "The return type must be a comparison category type.");

  bool exhaust1 = (first1 == last1);
  bool exhaust2 = (first2 == last2);
  MP_UNITS_DIAGNOSTIC_PUSH
  MP_UNITS_DIAGNOSTIC_IGNORE_ZERO_AS_NULLPOINTER_CONSTANT
  for (; !exhaust1 && !exhaust2; exhaust1 = (++first1 == last1), exhaust2 = (++first2 == last2))
    if (auto c = comp(*first1, *first2); c != 0) return c;
  MP_UNITS_DIAGNOSTIC_POP

  if (!exhaust1) return std::strong_ordering::greater;
  if (!exhaust2) return std::strong_ordering::less;
  return std::strong_ordering::equal;
}

template<class I1, class I2>
constexpr auto lexicographical_compare_three_way(I1 first1, I1 last1, I2 first2, I2 last2)
{
  return ::mp_units::detail::lexicographical_compare_three_way(first1, last1, first2, last2, std::compare_three_way());
}

template<class ForwardIt>
constexpr ForwardIt max_element(ForwardIt first, ForwardIt last)
{
  if (first == last) return last;

  ForwardIt largest = first;
  ++first;

  for (; first != last; ++first)
    if (*largest < *first) largest = first;

  return largest;
}

template<class T>
constexpr T max(std::initializer_list<T> ilist)
{
  return *max_element(ilist.begin(), ilist.end());
}

template<class T>
constexpr const T& max(const T& a, const T& b)
{
  return (a < b) ? b : a;
}

template<class ForwardIt>
constexpr ForwardIt min_element(ForwardIt first, ForwardIt last)
{
  if (first == last) return last;

  ForwardIt smallest = first;
  ++first;

  for (; first != last; ++first)
    if (*first < *smallest) smallest = first;

  return smallest;
}

template<class T>
constexpr T min(std::initializer_list<T> ilist)
{
  return *min_element(ilist.begin(), ilist.end());
}

template<class T>
constexpr const T& min(const T& a, const T& b)
{
  return (b < a) ? b : a;
}

template<class InputIt, class OutputIt>
constexpr OutputIt copy(InputIt first, InputIt last, OutputIt d_first)
{
  for (; first != last; (void)++first, (void)++d_first) *d_first = *first;
  return d_first;
}

}  // namespace mp_units::detail
