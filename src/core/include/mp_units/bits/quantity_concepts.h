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

#include <mp_units/bits/quantity_spec_concepts.h>
#include <mp_units/bits/reference_concepts.h>
#include <mp_units/bits/representation_concepts.h>
#include <mp_units/customization_points.h>

namespace mp_units {

template<Reference auto R, RepresentationOf<get_quantity_spec(R).character> Rep>
class quantity;

namespace detail {

template<auto R, typename Rep>
void to_base_specialization_of_quantity(const volatile quantity<R, Rep>*);

}  // namespace detail

/**
 * @brief A concept matching all quantities in the library
 *
 * Satisfied by all types being a either specialization or derived from `quantity`
 */
template<typename T>
concept Quantity = requires(T* t) { detail::to_base_specialization_of_quantity(t); };

/**
 * @brief A concept matching all quantities with provided dimension or reference
 *
 * Satisfied by all quantities with a dimension/reference being the instantiation derived from
 * the provided dimension/reference type.
 */
template<typename Q, auto V>
concept QuantityOf =
  Quantity<Q> && ((Dimension<std::remove_const_t<decltype(V)>> && Q::dimension == V) ||
                  (QuantitySpec<std::remove_const_t<decltype(V)>> && implicitly_convertible(Q::quantity_spec, V)));

/**
 * @brief A concept matching all external quantities like types
 *
 * Satisfied by all external types (not-defined in mp-units) that via a `quantity_like_traits` provide
 * all quantity-specific information.
 */
template<typename T>
concept QuantityLike = requires(T q) {
  quantity_like_traits<T>::reference;
  typename quantity_like_traits<T>::rep;
  requires Reference<std::remove_const_t<decltype(quantity_like_traits<T>::reference)>>;
  requires RepresentationOf<typename quantity_like_traits<T>::rep,
                            get_quantity_spec(quantity_like_traits<T>::reference).character>;
  {
    quantity_like_traits<T>::number(q)
  } -> std::convertible_to<typename quantity_like_traits<T>::rep>;
};

}  // namespace mp_units