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
#include <mp-units/framework/quantity_spec_concepts.h>
#include <mp-units/framework/unit_concepts.h>

namespace mp_units {

MP_UNITS_EXPORT template<QuantitySpec Q, Unit U>
struct reference;

MP_UNITS_EXPORT_BEGIN

[[nodiscard]] consteval QuantitySpec auto get_quantity_spec(AssociatedUnit auto u);

template<typename Q, typename U>
[[nodiscard]] consteval QuantitySpec auto get_quantity_spec(reference<Q, U>)
{
  return Q{};
}

[[nodiscard]] consteval Unit auto get_unit(AssociatedUnit auto u) { return u; }

template<typename Q, typename U>
[[nodiscard]] consteval Unit auto get_unit(reference<Q, U>)
{
  return U{};
}

/**
 * @brief A concept matching all references in the library.
 *
 * Satisfied by all specializations of @c reference.
 */
template<typename T>
concept Reference = AssociatedUnit<T> || is_specialization_of<T, reference>;

/**
 * @brief A concept matching all references with provided quantity spec
 *
 * Satisfied by all references with a quantity_spec being the instantiation derived from
 * the provided quantity_spec type.
 */
template<typename T, auto QS>
concept ReferenceOf = Reference<T> && QuantitySpecOf<decltype(get_quantity_spec(T{})), QS>;

MP_UNITS_EXPORT_END

namespace detail {

template<auto R1, auto R2>
concept SameReference =
  Reference<MP_UNITS_REMOVE_CONST(decltype(R1))> && Reference<MP_UNITS_REMOVE_CONST(decltype(R2))> && (R1 == R2);

}  // namespace detail

}  // namespace mp_units
