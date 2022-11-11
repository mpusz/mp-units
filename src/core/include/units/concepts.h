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

// IWYU pragma: begin_exports
// #include <units/bits/basic_concepts.h>
// #include <units/bits/quantity_of.h>
// IWYU pragma: end_exports
#include <units/bits/external/type_traits.h>
#include <units/dimension.h>
#include <units/unit.h>

namespace units {

template<Dimension auto D, Unit auto U>
struct reference;

namespace detail {

template<typename T>
inline constexpr bool is_specialization_of_reference = false;

template<Dimension auto D, Unit auto U>
inline constexpr bool is_specialization_of_reference<reference<D, U>> = true;

}  // namespace detail

/**
 * @brief A concept matching all references in the library.
 *
 * Satisfied by all specializations of @c reference.
 */
template<typename T>
concept Reference = detail::is_specialization_of_reference<T>;

namespace detail {

template<typename T>
inline constexpr bool is_quantity = false;

}

template<typename T>
concept Quantity = detail::is_quantity<T>;

template<typename T, typename U>
concept common_type_with_ =  // exposition only
  (std::same_as<std::common_type_t<T, U>, std::common_type_t<U, T>>) &&
  (std::constructible_from<std::common_type_t<T, U>, T>) && (std::constructible_from<std::common_type_t<T, U>, U>);

template<typename T, typename U = T>
concept scalable_number_ =  // exposition only
  (std::regular_invocable<std::multiplies<>, T, U>) && (std::regular_invocable<std::divides<>, T, U>);

template<typename T>
concept castable_number_ =  // exposition only
  common_type_with_<T, std::intmax_t> && scalable_number_<std::common_type_t<T, std::intmax_t>>;

template<typename T>
concept scalable_ =  // exposition only
  castable_number_<T> || (requires { typename T::value_type; } && castable_number_<typename T::value_type> &&
                          scalable_number_<T, std::common_type_t<typename T::value_type, std::intmax_t>>);

template<typename T, typename U>
concept scalable_with_ =  // exposition only
  common_type_with_<T, U> && scalable_<std::common_type_t<T, U>>;

template<typename T>
concept Representation = (!Quantity<T>) &&
                         // (!QuantityLike<T>) && (!wrapped_quantity_<T>) &&
                         std::regular<T> && scalable_<T>;

template<Reference auto R, Representation Rep>
class quantity;

namespace detail {

template<auto R, typename Rep>
inline constexpr bool is_quantity<quantity<R, Rep>> = true;

// template<typename T>
//   requires units::is_derived_from_specialization_of<T, units::quantity>
// inline constexpr bool is_quantity<T> = true;

}  // namespace detail

/**
 * @brief A concept matching all quantities with provided dimension
 *
 * Satisfied by all quantities with a dimension being the instantiation derived from
 * the provided dimension type.
 */
template<typename Q, Dimension auto D>
concept quantity_of = Quantity<Q> && Dimension<std::remove_const_t<decltype(D)>> && (Q::dimension == D);

/**
 * @brief A concept matching all quantities with provided dimension or reference
 *
 * Satisfied by all quantities with a dimension being the instantiation derived from
 * the provided dimension type.
 */
template<typename Q, Dimension auto D>
concept weak_quantity_of = Quantity<Q> && Dimension<std::remove_const_t<decltype(D)>> &&
                           (interconvertible(Q::dimension, D));

}  // namespace units
