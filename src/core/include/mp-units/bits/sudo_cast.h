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

#include <mp-units/bits/external/type_traits.h>
#include <mp-units/bits/magnitude.h>
#include <mp-units/bits/quantity_concepts.h>
#include <mp-units/bits/reference_concepts.h>
#include <mp-units/unit.h>

namespace mp_units::detail {

// determines the best available representation type
template<Quantity From, Quantity To>
[[nodiscard]] consteval auto common_rep_type(From, To)
{
  if constexpr (requires { typename std::common_type_t<typename From::rep, typename To::rep>; })
    // returns a common type of two representation types if available
    // i.e. `double` and `int` will end up with `double` precision
    return std::common_type_t<typename From::rep, typename To::rep>{};
  else
    return typename From::rep{};
}

/**
 * @brief Explicit cast between different quantity types
 *
 * @note This is a low-level facility and is too powerful to be used by the users directly. They should either use
 * `value_cast` or `quantity_cast`.
 *
 * @tparam To a target quantity type to cast to
 */
template<Quantity To, typename From>
  requires Quantity<std::remove_cvref_t<From>> &&
           (castable(std::remove_reference_t<From>::quantity_spec, To::quantity_spec)) &&
           ((std::remove_reference_t<From>::unit == To::unit &&
             std::constructible_from<typename To::rep, typename std::remove_reference_t<From>::rep>) ||
            (std::remove_reference_t<From>::unit != To::unit))  // && scalable_with_<typename To::rep>))
// TODO how to constrain the second part here?
[[nodiscard]] constexpr Quantity auto sudo_cast(From&& q)
{
  if constexpr (std::remove_reference_t<From>::unit == To::unit) {
    // no scaling of the number needed
    return make_quantity<To::reference>(
      static_cast<TYPENAME To::rep>(std::forward<From>(q).number()));  // this is the only (and recommended) way to do
                                                                       // a truncating conversion on a number, so we are
                                                                       // using static_cast to suppress all the compiler
                                                                       // warnings on conversions
  } else {
    // scale the number
    constexpr Magnitude auto c_mag = get_canonical_unit(q.unit).mag / get_canonical_unit(To::unit).mag;
    constexpr Magnitude auto num = numerator(c_mag);
    constexpr Magnitude auto den = denominator(c_mag);
    constexpr Magnitude auto irr = c_mag * (den / num);
    using c_rep_type = decltype(common_rep_type(q, To{}));
    using c_mag_type = common_magnitude_type<c_mag>;
    using multiplier_type =
      conditional<treat_as_floating_point<c_rep_type>, std::common_type_t<c_mag_type, long double>, c_mag_type>;
    constexpr auto val = [](Magnitude auto m) { return get_value<multiplier_type>(m); };
    return static_cast<TYPENAME To::rep>(static_cast<c_rep_type>(std::forward<From>(q).number()) * val(num) / val(den) *
                                         val(irr)) *
           To::reference;
  }
}

}  // namespace mp_units::detail
