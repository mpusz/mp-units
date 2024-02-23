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

#include <mp-units/bits/external/hacks.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/quantity_spec.h>
#include <mp-units/reference.h>
#include <mp-units/unit.h>

MP_UNITS_EXPORT
namespace mp_units {

/**
 * @brief System-specific reference
 *
 * This type is used in rare cases where more than one base quantity in a specific
 * system of units uses the same unit. For example in a hypothetical system of natural units
 * where  constant for speed of light `c = 1`, length and time could be measured in seconds.
 * In such cases `system_reference` has to be used to explicitly express such a binding.
 *
 * For example:
 *
 * @code{.cpp}
 * // hypothetical natural system of units for c=1
 *
 * inline constexpr struct second : named_unit<"s"> {} second;
 * inline constexpr struct minute : named_unit<"min", mag<60> * second> {} minute;
 * inline constexpr struct gram : named_unit<"g"> {} gram;
 * inline constexpr struct kilogram : decltype(si::kilo<gram>) {} kilogram;
 *
 * inline constexpr struct time : system_reference<isq::time, second> {} time;
 * inline constexpr struct length : system_reference<isq::length, second> {} length;
 * inline constexpr struct speed : system_reference<isq::speed, second / second> {} speed;
 * inline constexpr struct force : system_reference<isq::force, kilogram / second> {} force;
 * @endcode
 *
 * @tparam Q quantity for which a unit is being assigned
 * @tparam CoU coherent unit for a quantity in this system
 */
template<QuantitySpec auto Q, Unit auto CoU>
  requires(!AssociatedUnit<std::remove_const_t<decltype(CoU)>>) || (CoU == one)
struct system_reference {
  static constexpr auto quantity_spec = Q;
  static constexpr auto coherent_unit = CoU;

  template<Unit U>
    requires(convertible(coherent_unit, U{}))
#if MP_UNITS_COMP_MSVC
  [[nodiscard]] constexpr decltype(reference<std::remove_const_t<decltype(quantity_spec)>, U>{}) operator[](U) const
#else
  [[nodiscard]] constexpr reference<std::remove_const_t<decltype(quantity_spec)>, U> operator[](U) const
#endif
  {
    return {};
  }
};

}  // namespace mp_units
