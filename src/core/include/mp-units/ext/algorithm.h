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

// Copy-pasted C++ standard libraries
// TODO To be replaced with `import std;` when available
//      `#include <algorithm.h>` is too heavy to do in every translation unit

#pragma once

#include <mp-units/bits/hacks.h>  // IWYU pragma: keep

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <compare>
#include <initializer_list>
#include <iterator>
#endif
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

template<class ForwardIt1, class ForwardIt2>
constexpr void iter_swap(ForwardIt1 a, ForwardIt2 b)
{
  using std::swap;
  swap(*a, *b);
}

template<class ForwardIt1, class ForwardIt2>
constexpr ForwardIt2 swap_ranges(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2)
{
  for (; first1 != last1; ++first1, ++first2) iter_swap(first1, first2);
  return first2;
}

}  // namespace mp_units::detail
