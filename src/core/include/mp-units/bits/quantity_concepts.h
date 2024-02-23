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

#include <mp-units/bits/module_macros.h>
#include <mp-units/bits/quantity_spec_concepts.h>
#include <mp-units/bits/reference_concepts.h>
#include <mp-units/bits/representation_concepts.h>
#include <mp-units/customization_points.h>

namespace mp_units {

MP_UNITS_EXPORT template<Reference auto R, RepresentationOf<get_quantity_spec(R).character> Rep>
class quantity;

namespace detail {

template<auto R, typename Rep>
void to_base_specialization_of_quantity(const volatile quantity<R, Rep>*);

template<typename T>
inline constexpr bool is_derived_from_specialization_of_quantity =
  requires(std::remove_reference_t<T>* t) { to_base_specialization_of_quantity(t); };

}  // namespace detail

MP_UNITS_EXPORT_BEGIN

/**
 * @brief A concept matching all quantities in the library
 *
 * Satisfied by all types being a either specialization or derived from `quantity`
 */
template<typename T>
concept Quantity = detail::is_derived_from_specialization_of_quantity<T>;

/**
 * @brief A concept matching all quantities with provided quantity spec
 *
 * Satisfied by all quantities with a quantity_spec being the instantiation derived from
 * the provided quantity_spec type.
 */
template<typename Q, auto QS>
concept QuantityOf = Quantity<Q> && QuantitySpecOf<std::remove_const_t<decltype(Q::quantity_spec)>, QS>;

/**
 * @brief A concept matching all external quantities like types
 *
 * Satisfied by all external types (not-defined in mp-units) that via a `quantity_like_traits` provide
 * all quantity-specific information.
 */
template<typename T>
concept QuantityLike = requires {
  quantity_like_traits<T>::reference;
  requires Reference<std::remove_const_t<decltype(quantity_like_traits<T>::reference)>>;
  typename quantity_like_traits<T>::rep;
  requires RepresentationOf<typename quantity_like_traits<T>::rep,
                            get_quantity_spec(quantity_like_traits<T>::reference).character>;
} && requires(T q, typename quantity_like_traits<T>::rep v) {
  {
    quantity_like_traits<T>::to_numerical_value(q)
  } -> detail::ConversionSpecOf<typename quantity_like_traits<T>::rep>;
  {
    quantity_like_traits<T>::from_numerical_value(v)
  } -> detail::ConversionSpecOf<T>;
};

MP_UNITS_EXPORT_END

}  // namespace mp_units
