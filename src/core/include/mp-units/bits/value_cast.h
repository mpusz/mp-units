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

#include <mp-units/bits/quantity_concepts.h>
#include <mp-units/bits/representation_concepts.h>
#include <mp-units/bits/sudo_cast.h>
#include <mp-units/bits/unit_concepts.h>
#include <mp-units/reference.h>

namespace mp_units {

/**
 * @brief Explicit cast of a quantity's unit
 *
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In truncating cases an explicit cast have to be used.
 *
 * auto d = value_cast<si::second>(1234 * ms);
 *
 * @tparam ToU a unit to use for a target quantity
 */
template<Unit auto ToU, typename Q>
[[nodiscard]] constexpr Quantity auto value_cast(Q&& q)
  requires Quantity<std::remove_cvref_t<Q>> && (convertible(std::remove_reference_t<Q>::reference, ToU))
{
  using q_type = std::remove_reference_t<Q>;
  constexpr auto r = [] {
    if constexpr (detail::is_specialization_of_reference<std::remove_const_t<decltype(q_type::reference)>>::value ||
                  !AssociatedUnit<std::remove_const_t<decltype(ToU)>>)
      return reference<q_type::quantity_spec, ToU>{};
    else
      return ToU;
  }();
  return detail::sudo_cast<quantity<r, typename q_type::rep>>(std::forward<Q>(q));
}

/**
 * @brief Explicit cast of a quantity's representation type
 *
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In truncating cases an explicit cast have to be used.
 *
 * auto q = value_cast<int>(1.23 * ms);
 *
 * @tparam ToRep a representation type to use for a target quantity
 */
template<Representation ToRep, typename Q>
  requires Quantity<std::remove_cvref_t<Q>> &&
           RepresentationOf<ToRep, std::remove_reference_t<Q>::quantity_spec.character> &&
           std::constructible_from<ToRep, typename std::remove_reference_t<Q>::rep>
[[nodiscard]] constexpr quantity<std::remove_reference_t<Q>::reference, ToRep> value_cast(Q&& q)
{
  return detail::sudo_cast<quantity<std::remove_reference_t<Q>::reference, ToRep>>(std::forward<Q>(q));
}

}  // namespace mp_units
