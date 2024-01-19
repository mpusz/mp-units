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

template<typename T, typename Other>
struct get_common_type : std::common_type<T, Other> {};

template<typename T, typename Other>
using maybe_common_type = MP_UNITS_TYPENAME std::conditional_t<requires { typename std::common_type_t<T, Other>; },
                                                               get_common_type<T, Other>, std::type_identity<T>>::type;

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
[[nodiscard]] constexpr To sudo_cast(From&& q)
{
  constexpr auto q_unit = std::remove_reference_t<From>::unit;
  if constexpr (q_unit == To::unit) {
    // no scaling of the number needed
    return {static_cast<MP_UNITS_TYPENAME To::rep>(std::forward<From>(q).numerical_value_is_an_implementation_detail_),
            To::reference};  // this is the only (and recommended) way to do
                             // a truncating conversion on a number, so we are
                             // using static_cast to suppress all the compiler
                             // warnings on conversions
  } else {
    // scale the number
    constexpr Magnitude auto c_mag = get_canonical_unit(q_unit).mag / get_canonical_unit(To::unit).mag;
    constexpr Magnitude auto num = numerator(c_mag);
    constexpr Magnitude auto den = denominator(c_mag);
    constexpr Magnitude auto irr = c_mag * (den / num);
    using c_rep_type = maybe_common_type<typename std::remove_reference_t<From>::rep, typename To::rep>;
    using c_mag_type = common_magnitude_type<c_mag>;
    using multiplier_type = conditional<treat_as_floating_point<c_rep_type>,
                                        // ensure that the multiplier is also floating-point
                                        conditional<std::is_arithmetic_v<underlying_type_t<c_rep_type>>,
                                                    // reuse user's type if possible
                                                    std::common_type_t<c_mag_type, underlying_type_t<c_rep_type>>,
                                                    std::common_type_t<c_mag_type, double>>,
                                        c_mag_type>;
    using c_type = maybe_common_type<c_rep_type, multiplier_type>;
    constexpr auto val = [](Magnitude auto m) { return get_value<multiplier_type>(m); };
    if constexpr (std::is_floating_point_v<multiplier_type>) {
      // this results in great assembly
      constexpr auto ratio = val(num) / val(den) * val(irr);
      auto res = static_cast<MP_UNITS_TYPENAME To::rep>(
        static_cast<c_type>(q.numerical_value_is_an_implementation_detail_) * ratio);
      return {res, To::reference};
    } else {
      // this is slower but allows conversions like 2000 m -> 2 km without loosing data
      auto res = static_cast<MP_UNITS_TYPENAME To::rep>(
        static_cast<c_type>(q.numerical_value_is_an_implementation_detail_) * val(num) / val(den) * val(irr));
      return {res, To::reference};
    }
  }
}

}  // namespace mp_units::detail
