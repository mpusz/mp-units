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

#include <units/bits/external/type_list.h>
#include <units/exponent.h>
#include <ratio>

namespace units::detail {

/**
 * @brief Consolidates contiguous ranges of exponents of the same dimension
 *
 * If there is more than one exponent with the same dimension they are aggregated into one exponent by adding
 * their exponents. If this accumulation will result with 0, such a dimension is removed from the list.
 *
 * @tparam D derived dimension to consolidate
 */
template<typename ExpList>
struct dim_consolidate;

template<>
struct dim_consolidate<exponent_list<>> {
  using type = exponent_list<>;
};

template<typename E>
struct dim_consolidate<exponent_list<E>> {
  using type = exponent_list<E>;
};

template<typename E1, typename... ERest>
struct dim_consolidate<exponent_list<E1, ERest...>> {
  using type = type_list_push_front<typename dim_consolidate<exponent_list<ERest...>>::type, E1>;
};

template<BaseDimension Dim, std::intmax_t Num1, std::intmax_t Den1, std::intmax_t Num2, std::intmax_t Den2,
         typename... ERest>
struct dim_consolidate<exponent_list<exponent<Dim, Num1, Den1>, exponent<Dim, Num2, Den2>, ERest...>> {
  using r1 = std::ratio<Num1, Den1>;
  using r2 = std::ratio<Num2, Den2>;
  using r = std::ratio_add<r1, r2>;
  using type = conditional<r::num == 0, typename dim_consolidate<exponent_list<ERest...>>::type,
                           typename dim_consolidate<exponent_list<exponent<Dim, r::num, r::den>, ERest...>>::type>;
};

}  // namespace units::detail
