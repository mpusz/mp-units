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

// IWYU pragma: private, include <mp-units/framework.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/quantity_spec_concepts.h>
#include <mp-units/framework/reference_concepts.h>
#include <mp-units/framework/representation_concepts.h>

namespace mp_units {

MP_UNITS_EXPORT template<Reference auto R, RepresentationOf<get_quantity_spec(R).character> Rep>
class quantity;

namespace detail {

template<auto R, typename Rep>
void to_base_specialization_of_quantity(const volatile quantity<R, Rep>*);

template<typename T>
constexpr bool is_derived_from_specialization_of_quantity =
  requires(T* type) { to_base_specialization_of_quantity(type); };

}  // namespace detail

/**
 * @brief A concept matching all quantities in the library
 *
 * Satisfied by all types being a either specialization or derived from `quantity`
 */
MP_UNITS_EXPORT template<typename T>
concept Quantity = detail::is_derived_from_specialization_of_quantity<T>;

namespace detail {

template<typename T, template<typename> typename Traits>
concept QuantityLikeImpl = requires(const T& qty, const Traits<T>::rep& num) {
  { Traits<T>::to_numerical_value(qty) } -> std::same_as<typename Traits<T>::rep>;
  { Traits<T>::from_numerical_value(num) } -> std::same_as<T>;
  requires std::same_as<decltype(Traits<T>::explicit_import), const bool>;
  requires std::same_as<decltype(Traits<T>::explicit_export), const bool>;
  typename std::bool_constant<Traits<T>::explicit_import>;
  typename std::bool_constant<Traits<T>::explicit_export>;
};

}  // namespace detail

/**
 * @brief A concept matching all quantities with provided quantity spec
 *
 * Satisfied by all quantities with a quantity_spec being the instantiation derived from
 * the provided quantity_spec type.
 */
MP_UNITS_EXPORT template<typename Q, auto QS>
concept QuantityOf = Quantity<Q> && QuantitySpecOf<std::remove_const_t<decltype(Q::quantity_spec)>, QS>;

/**
 * @brief A concept matching all external quantities like types
 *
 * Satisfied by all external types (not-defined in mp-units) that via a `quantity_like_traits` provide
 * all quantity-specific information.
 */
MP_UNITS_EXPORT template<typename T>
concept QuantityLike = !Quantity<T> && detail::QuantityLikeImpl<T, quantity_like_traits> && requires {
  typename quantity<quantity_like_traits<T>::reference, typename quantity_like_traits<T>::rep>;
};

}  // namespace mp_units
