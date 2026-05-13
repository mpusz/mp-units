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
#include <mp-units/bits/hacks.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/framework/symbolic_expression.h>

namespace mp_units {

#if MP_UNITS_API_NO_CRTP
MP_UNITS_EXPORT template<auto...>
#else
MP_UNITS_EXPORT template<typename, auto...>
#endif
struct quantity_spec;

namespace detail {

struct quantity_spec_interface_base;

#if MP_UNITS_API_NO_CRTP

template<typename T>
constexpr bool is_specialization_of_quantity_spec = is_specialization_of_v<T, quantity_spec>;

#else

template<typename T>
constexpr bool is_specialization_of_quantity_spec = false;

template<typename Derived, auto... Params>
constexpr bool is_specialization_of_quantity_spec<quantity_spec<Derived, Params...> > = true;

#endif

}  // namespace detail

MP_UNITS_EXPORT template<typename T>
concept QuantitySpec = std::derived_from<T, detail::quantity_spec_interface_base> && detail::SymbolicConstant<T> &&
                       (!detail::is_specialization_of_quantity_spec<T>);

template<typename Q>
struct kind_of_;

namespace detail {

template<typename T>
concept QuantityKindSpec = QuantitySpec<T> && is_specialization_of<T, kind_of_>;

}  // namespace detail

MP_UNITS_EXPORT template<QuantitySpec From, QuantitySpec To>
[[nodiscard]] consteval bool implicitly_convertible(From from, To to);

MP_UNITS_EXPORT template<QuantitySpec From, QuantitySpec To>
[[nodiscard]] consteval bool explicitly_convertible(From from, To to);

/**
 * @brief A concept matching all quantity specifications of a provided quantity spec value
 *
 * Satisfied by all quantity specifications that are implicitly convertible to the provided @c QS
 * value.
 */
MP_UNITS_EXPORT template<typename T, auto QS>
concept QuantitySpecOf =
  QuantitySpec<T> && QuantitySpec<MP_UNITS_REMOVE_CONST(decltype(QS))> && (mp_units::implicitly_convertible(T{}, QS));

}  // namespace mp_units
