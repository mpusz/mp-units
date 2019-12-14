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

#include <units/base_dimension.h>
#include <units/exp.h>
#include <units/ratio.h>

namespace units::detail {

template<Exponent E>
  requires (E::den == 1 || E::den == 2) // TODO provide support for any den
struct exp_ratio {
  using base_ratio = E::dimension::base_unit::ratio;
  using positive_ratio = conditional<E::num * E::den < 0, ratio<base_ratio::den, base_ratio::num>, base_ratio>;
  static constexpr std::int64_t N = E::num * E::den < 0 ? -E::num : E::num;
  using pow = ratio_pow<positive_ratio, N>;
  using type = conditional<E::den == 2, ratio_sqrt<pow>, pow>;
};

template<typename ExpList>
struct base_units_ratio_impl;

template<typename E, typename... Es>
struct base_units_ratio_impl<exp_list<E, Es...>> {
  using type = ratio_multiply<typename exp_ratio<E>::type, typename base_units_ratio_impl<exp_list<Es...>>::type>;
};

template<typename E>
struct base_units_ratio_impl<exp_list<E>> {
  using type = exp_ratio<E>::type;
};

/**
 * @brief Calculates the common ratio of all the references of base units in the derived dimension
 */
template<typename D>
using base_units_ratio = base_units_ratio_impl<typename D::exponents>::type;

}  // namespace units::detail
