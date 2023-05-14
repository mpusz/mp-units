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

#include <mp_units/bits/quantity_concepts.h>
#include <mp_units/bits/reference_concepts.h>
#include <mp_units/bits/representation_concepts.h>
#include <mp_units/bits/sudo_cast.h>
#include <mp_units/bits/unit_concepts.h>

namespace mp_units {

template<Reference auto R, RepresentationOf<get_quantity_spec(R).character> Rep>
class quantity;

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
template<Unit auto ToU, auto R, typename Rep>
  requires(convertible(get_unit(R), ToU))
[[nodiscard]] constexpr Quantity auto value_cast(const quantity<R, Rep>& q)
{
  if constexpr (detail::is_specialization_of_reference<std::remove_const_t<decltype(R)>> ||
                !AssociatedUnit<std::remove_const_t<decltype(ToU)>>) {
    constexpr reference<quantity<R, Rep>::quantity_spec, ToU> r;
    return detail::sudo_cast<quantity<r, Rep>>(q);
  } else {
    return detail::sudo_cast<quantity<ToU, Rep>>(q);
  }
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
template<Representation ToRep, auto R, typename Rep>
  requires RepresentationOf<ToRep, get_quantity_spec(R).character> && std::constructible_from<ToRep, Rep>
[[nodiscard]] constexpr Quantity auto value_cast(const quantity<R, Rep>& q)
{
  return detail::sudo_cast<quantity<R, ToRep>>(q);
}

}  // namespace mp_units
