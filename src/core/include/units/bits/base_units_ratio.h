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
#include <units/exponent.h>
#include <units/ratio.h>

namespace units::detail {

template<Exponent E>
  requires(E::den == 1 || E::den == 2)  // TODO provide support for any den
constexpr ratio exp_ratio()
{
  const ratio base_ratio = E::dimension::base_unit::ratio;
  const ratio positive_ratio =
    E::num * E::den < 0 ? ratio(base_ratio.den, base_ratio.num, -base_ratio.exp) : base_ratio;
  const std::intmax_t N = E::num * E::den < 0 ? -E::num : E::num;
  const ratio ratio_pow = pow<N>(positive_ratio);
  return E::den == 2 ? sqrt(ratio_pow) : ratio_pow;
}

/**
 * @brief Calculates the common ratio of all the references of base units in the derived dimension
 */
template<typename... Es>
constexpr ratio base_units_ratio(exponent_list<Es...>)
{
  return (exp_ratio<Es>() * ... * ratio(1));
}

}  // namespace units::detail
