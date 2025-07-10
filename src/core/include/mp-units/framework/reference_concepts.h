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
#include <mp-units/compat_macros.h>
#include <mp-units/framework/quantity_spec_concepts.h>
#include <mp-units/framework/unit_concepts.h>

namespace mp_units {

MP_UNITS_EXPORT template<QuantitySpec Q, Unit U>
struct reference;

MP_UNITS_EXPORT_BEGIN

template<MP_UNITS_ASSOCIATED_UNIT U>
[[nodiscard]] consteval QuantitySpec auto get_quantity_spec(U);

template<typename Q, typename U>
[[nodiscard]] consteval QuantitySpec auto get_quantity_spec(reference<Q, U>)
{
  return Q{};
}

[[nodiscard]] consteval Unit auto get_unit(MP_UNITS_ASSOCIATED_UNIT auto u) { return u; }

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
concept Reference = MP_UNITS_ASSOCIATED_UNIT_T(T) || is_specialization_of<T, reference>;

/**
 * @brief A concept matching all references of the provided quantity spec
 *
 * Satisfied by all references for which @c QuantitySpecOf<QS> is true.
 */
template<typename T, auto QS>
concept ReferenceOf = Reference<T> && QuantitySpec<MP_UNITS_REMOVE_CONST(decltype(QS))> &&
                      QuantitySpecOf<decltype(get_quantity_spec(T{})), QS>;

MP_UNITS_EXPORT_END

}  // namespace mp_units
