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

namespace units::detail {

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

template<class InputIt, class UnaryPredicate>
constexpr InputIt find_if(InputIt first, InputIt last, UnaryPredicate p)
{
  for (; first != last; ++first) {
    if (p(*first)) {
      return first;
    }
  }
  return last;
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
  for (; !exhaust1 && !exhaust2; exhaust1 = (++f1 == l1), exhaust2 = (++f2 == l2))
    if (auto c = comp(*f1, *f2); c != 0) return c;

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
