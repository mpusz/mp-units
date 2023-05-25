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

#include <mp-units/bits/quantity_spec_concepts.h>
#include <mp-units/bits/unit_concepts.h>

namespace mp_units {

template<QuantitySpec auto Q, Unit auto U>
struct reference;

namespace detail {

template<typename T>
inline constexpr bool is_specialization_of_reference = false;

template<auto Q, auto U>
inline constexpr bool is_specialization_of_reference<reference<Q, U>> = true;

}  // namespace detail

/**
 * @brief A concept matching all references in the library.
 *
 * Satisfied by all specializations of @c reference.
 */
template<typename T>
concept Reference = AssociatedUnit<T> || detail::is_specialization_of_reference<T>;

[[nodiscard]] consteval QuantitySpec auto get_quantity_spec(AssociatedUnit auto u);

template<auto Q, auto U>
[[nodiscard]] consteval QuantitySpec auto get_quantity_spec(reference<Q, U>)
{
  return Q;
}

[[nodiscard]] consteval Unit auto get_unit(AssociatedUnit auto u) { return u; }

template<auto Q, auto U>
[[nodiscard]] consteval Unit auto get_unit(reference<Q, U>)
{
  return U;
}

/**
 * @brief A concept matching all references with provided dimension or quantity spec
 *
 * Satisfied by all references with a dimension/quantity_spec being the instantiation derived from
 * the provided dimension/quantity_spec type.
 */
template<typename T, auto V>
concept ReferenceOf =
  Reference<T> &&
  ((Dimension<std::remove_const_t<decltype(V)>> && get_quantity_spec(T{}).dimension == V) ||
   (QuantitySpec<std::remove_const_t<decltype(V)>> && implicitly_convertible(get_quantity_spec(T{}), V)));

}  // namespace mp_units
