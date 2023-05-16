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

#include <mp_units/bits/external/type_traits.h>
#include <mp_units/bits/magnitude.h>
#include <mp_units/bits/quantity_concepts.h>
#include <mp_units/bits/reference_concepts.h>
#include <mp_units/unit.h>

namespace mp_units {

template<Reference auto R, RepresentationOf<get_quantity_spec(R).character> Rep>
class quantity;

namespace detail {

// The default implementation for the number scaling customization point
template<typename Rep, Unit From, Unit To>
[[nodiscard]] constexpr auto scale_quantity_number(Rep v, From from, To to)
  requires(have_same_canonical_reference_unit(from, to))
{
  using multiplier_type = decltype([] {
    // widen the type to prevent overflows
    using wider_type = decltype(Rep{} * std::intmax_t{});
    // check if `wider_type` supports scaling operations
    if constexpr (requires(wider_type v) { v* v / v; })
      // if the `wider_type` can handle scaling operations then use it to improve accuracy
      return wider_type{};
    else
      // needed for example for linear algebra where `op/` on matrix types is not available
      return std::intmax_t{};
  }());

  constexpr Magnitude auto c_mag = detail::get_canonical_unit(from).mag / detail::get_canonical_unit(to).mag;
  constexpr Magnitude auto num = numerator(c_mag);
  constexpr Magnitude auto den = denominator(c_mag);
  constexpr Magnitude auto irr = c_mag * (den / num);

  constexpr auto val = [](Magnitude auto m) { return get_value<multiplier_type>(m); };
  return v * val(num) / val(den) * val(irr);
}

/**
 * @brief Explicit cast of entire quantity
 *
 * @note This is too powerful to be used by users.
 *
 * @tparam To a target quantity type to cast to
 */
template<Quantity To, auto R, typename Rep>
  requires(castable(get_quantity_spec(R), To::quantity_spec)) &&
          ((get_unit(R) == To::unit && std::constructible_from<typename To::rep, Rep>) ||
           (get_unit(R) != To::unit && convertible(get_unit(R), To::unit)))  // && scalable_with_<typename To::rep>))
// TODO how to constrain the second part here?
[[nodiscard]] constexpr Quantity auto sudo_cast(const quantity<R, Rep>& q)
{
  if constexpr (get_unit(R) == To::unit) {
    // no scaling of the number needed
    return static_cast<TYPENAME To::rep>(q.number()) * To::reference;  // this is the only (and recommended) way to do
                                                                       // a truncating conversion on a number, so we are
                                                                       // using static_cast to suppress all the compiler
                                                                       // warnings on conversions
  } else {
    // scale the number
    using rep_type = decltype([] {
      // determines the best representation type
      if constexpr (requires { typename std::common_type_t<Rep, typename To::rep>; })
        // returns a common type of two representation types if available
        // i.e. `double` and `int` will end up with `double` precision
        return std::common_type_t<Rep, typename To::rep>{};
      else
        return Rep{};
    }());

    // `scale_quantity_number` is a customization point
    // Will be found only via ADL (if provided) for user-defined units
    return static_cast<TYPENAME To::rep>(
             scale_quantity_number(static_cast<rep_type>(q.number()), get_unit(R), To::unit)) *
           To::reference;
  }
}

}  // namespace detail
}  // namespace mp_units
