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

#include <mp-units/bits/requires_hosted.h>
//
#include <mp-units/bits/module_macros.h>
#include <mp-units/ext/contracts.h>
#include <mp-units/systems/hep/quantities.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework/quantity.h>
#include <mp-units/framework/unit.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cmath>
#endif
#endif

MP_UNITS_EXPORT
namespace mp_units::hep {

/**
 * @brief Computes the relativistic β from the Lorentz factor, β = √(1 − 1/γ²)
 *
 * This relation is not a product of powers, so no quantity equation can state it. The function
 * takes and returns typed quantities and computes on the numbers in between.
 *
 * @pre `gamma` is not less than 1
 */
template<ReferenceOf<lorentz_factor> auto R, typename Rep>
  requires treat_as_floating_point<Rep> && (requires(Rep v) { sqrt(v); } || requires(Rep v) { std::sqrt(v); })
[[nodiscard]] quantity<relativistic_beta[one], Rep> beta_from_gamma(const quantity<R, Rep>& gamma)
  MP_UNITS_PRE(!(gamma.numerical_value_in(one) < Rep{1}))
{
  MP_UNITS_EXPECTS(!(gamma.numerical_value_in(one) < Rep{1}));
  using std::sqrt;
  const Rep value = gamma.numerical_value_in(one);
  // √((γ − 1)(γ + 1))/γ rather than √(1 − 1/γ²), which loses the digits of a γ close to 1
  return relativistic_beta(sqrt((value - Rep{1}) * (value + Rep{1})) / value * one);
}

/**
 * @brief Computes the Lorentz factor from the relativistic β, γ = 1/√(1 − β²)
 *
 * @pre `beta` is not less than 0 and less than 1
 */
template<ReferenceOf<relativistic_beta> auto R, typename Rep>
  requires treat_as_floating_point<Rep> && (requires(Rep v) { sqrt(v); } || requires(Rep v) { std::sqrt(v); })
[[nodiscard]] quantity<lorentz_factor[one], Rep> gamma_from_beta(const quantity<R, Rep>& beta)
  MP_UNITS_PRE(!(beta.numerical_value_in(one) < Rep{0} || beta.numerical_value_in(one) >= Rep{1}))
{
  MP_UNITS_EXPECTS(!(beta.numerical_value_in(one) < Rep{0} || beta.numerical_value_in(one) >= Rep{1}));
  using std::sqrt;
  const Rep value = beta.numerical_value_in(one);
  // (1 − β)(1 + β) rather than 1 − β², which loses the digits of a β close to 1
  return lorentz_factor(Rep{1} / sqrt((Rep{1} - value) * (Rep{1} + value)) * one);
}

}  // namespace mp_units::hep
