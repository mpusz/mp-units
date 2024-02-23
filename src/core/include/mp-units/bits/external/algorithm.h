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

#include <mp-units/bits/external/hacks.h>  // IWYU pragma: keep

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <compare>
#include <initializer_list>
#include <iterator>
#include <ranges>
#endif

namespace mp_units::detail {

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
  MP_UNITS_DIAGNOSTIC_PUSH
  MP_UNITS_DIAGNOSTIC_IGNORE_ZERO_AS_NULLPOINTER_CONSTANT
  for (; !exhaust1 && !exhaust2; exhaust1 = (++f1 == l1), exhaust2 = (++f2 == l2))
    if (auto c = comp(*f1, *f2); c != 0) return c;
  MP_UNITS_DIAGNOSTIC_POP

  return !exhaust1   ? std::strong_ordering::greater
         : !exhaust2 ? std::strong_ordering::less
                     : std::strong_ordering::equal;
}

template<class I1, class I2>
constexpr auto lexicographical_compare_three_way(I1 f1, I1 l1, I2 f2, I2 l2)
{
  return ::mp_units::detail::lexicographical_compare_three_way(f1, l1, f2, l2, std::compare_three_way());
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

template<class I, class O>
struct in_out_result {
  [[no_unique_address]] I in;
  [[no_unique_address]] O out;

  template<class I2, class O2>
    requires std::convertible_to<const I&, I2> && std::convertible_to<const O&, O2>
  constexpr operator in_out_result<I2, O2>() const&
  {
    return {in, out};
  }

  template<class I2, class O2>
    requires std::convertible_to<I, I2> && std::convertible_to<O, O2>
  constexpr operator in_out_result<I2, O2>() &&
  {
    return {std::move(in), std::move(out)};
  }
};

template<class I, class O>
using copy_result = in_out_result<I, O>;

template<std::input_iterator I, std::sentinel_for<I> S, std::weakly_incrementable O>
  requires std::indirectly_copyable<I, O>
constexpr copy_result<I, O> copy(I first, S last, O result)
{
  for (; first != last; ++first, (void)++result) {
    *result = *first;
  }
  return {std::move(first), std::move(result)};
}

template<std::ranges::input_range R, std::weakly_incrementable O>
  requires std::indirectly_copyable<std::ranges::iterator_t<R>, O>
constexpr copy_result<std::ranges::borrowed_iterator_t<R>, O> copy(R&& r, O result)
{
  return ::mp_units::detail::copy(std::ranges::begin(r), std::ranges::end(r), std::move(result));
}


}  // namespace mp_units::detail
