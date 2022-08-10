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

#include <units/bits/external/hacks.h>  // IWYU pragma: keep
#include <compare>
#include <iterator>

namespace units::detail {

// TODO refactor two below functions with std::ranges when moved to modules


/**
 * @brief Returns the first successful value obtained from applying the function object to the elements of a given
 * range, if any.
 *
 * @tparam InputIt must meet the requirements of LegacyInputIterator
 * @tparam UnaryFunction must meet the requirements of MoveConstructible
 * @param first the beginning of the range of elements to examine
 * @param last the end of the range of elements to examine
 * @param f function object, to be applied to the result of dereferencing every iterator in the range
 * @return std::invoke_result_t<UnaryPredicate, std::iter_value_t<InputIt>>
 */
template<class InputIt, class UnaryFunction>
constexpr std::invoke_result_t<UnaryFunction, std::iter_value_t<InputIt>> get_first_of(InputIt first, InputIt last,
                                                                                       UnaryFunction f)
{
  for (; first != last; ++first)
    if (auto opt = f(*first)) return *opt;
  return {};
}

template<class Rng, class UnaryFunction>
constexpr auto get_first_of(const Rng& rng, UnaryFunction f)
{
  using std::begin, std::end;
  return get_first_of(begin(rng), end(rng), f);
}

// TODO remove all the below and use std when moved to modules
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
constexpr auto lexicographical_compare_three_way(I1 f1, I1 l1, I2 f2, I2 l2, Cmp comp) -> decltype(comp(*f1, *f2))
{
  using ret_t = decltype(comp(*f1, *f2));
  static_assert(std::disjunction_v<std::is_same<ret_t, std::strong_ordering>, std::is_same<ret_t, std::weak_ordering>,
                                   std::is_same<ret_t, std::partial_ordering>>,
                "The return type must be a comparison category type.");

  bool exhaust1 = (f1 == l1);
  bool exhaust2 = (f2 == l2);
  UNITS_DIAGNOSTIC_PUSH
  UNITS_DIAGNOSTIC_IGNORE_ZERO_AS_NULLPOINTER_CONSTANT
  for (; !exhaust1 && !exhaust2; exhaust1 = (++f1 == l1), exhaust2 = (++f2 == l2))
    if (auto c = comp(*f1, *f2); c != 0) return c;
  UNITS_DIAGNOSTIC_POP

  return !exhaust1   ? std::strong_ordering::greater
         : !exhaust2 ? std::strong_ordering::less
                     : std::strong_ordering::equal;
}

template<class I1, class I2>
constexpr auto lexicographical_compare_three_way(I1 f1, I1 l1, I2 f2, I2 l2)
{
  return ::units::detail::lexicographical_compare_three_way(f1, l1, f2, l2, std::compare_three_way());
}

}  // namespace units::detail
