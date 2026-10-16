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
#include <mp-units/bits/config.h>
#include <mp-units/bits/hacks.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/framework/quantity_character.h>
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
constexpr bool is_specialization_of_quantity_spec<quantity_spec<Derived, Params...>> = true;

#endif

}  // namespace detail

// `is_base_of_v` rather than `derived_from`: one constraint atom instead of two, evaluated per type; the base is
// always public and unambiguous, so the convertibility atom `derived_from` adds can never fail here.
MP_UNITS_EXPORT template<typename T>
concept QuantitySpec = std::is_base_of_v<detail::quantity_spec_interface_base, T> && detail::SymbolicConstant<T> &&
                       !detail::is_specialization_of_quantity_spec<std::remove_const_t<T>>;

template<typename Q>
struct kind_of_;

namespace detail {

template<typename T>
concept QuantityKindSpec = QuantitySpec<T> && is_specialization_of<std::remove_const_t<T>, kind_of_>;

// What a quantity specification admits, as a flag set per axis. Not public, as a quantity kind admits
// every character and no public value expresses that. Public code asks `QuantitySpecOf` with a
// character, or `QuantityOf` / `QuantityPointOf`, which also weigh the representation.
template<QuantitySpec QS>
[[nodiscard]] consteval character_mask admitted_character(QS qs)
{
  return qs._character_;
}

}  // namespace detail

namespace utility {

MP_UNITS_EXPORT template<QuantitySpec From, QuantitySpec To>
[[nodiscard]] consteval bool implicitly_convertible(From from, To to);

MP_UNITS_EXPORT template<QuantitySpec From, QuantitySpec To>
[[nodiscard]] consteval bool explicitly_convertible(From from, To to);

}  // namespace utility

/**
 * @brief A concept matching all quantity specifications of a provided property
 *
 * `V` may be another quantity specification, in which case this is satisfied by every specification
 * implicitly convertible to it, or a character (a bare `quantity_tensor_order`, a bare
 * `quantity_field`, or a full `quantity_character`), in which case it is satisfied by every
 * specification that *admits* that character. A quantity kind admits every character. As in
 * `QuantityOf`, the type of `V` selects the reading.
 *
 * Ask this where no quantity exists yet to ask `QuantityOf` about, such as constraining a reference
 * taken as a template parameter (`utility::polar_vector` requires a radius whose specification admits
 * scalar).
 */
MP_UNITS_EXPORT template<typename T, auto V>
concept QuantitySpecOf =
  QuantitySpec<T> &&
  ((QuantitySpec<MP_UNITS_NTTP_HACK(decltype(V))> && mp_units::utility::implicitly_convertible(T{}, V)) ||
   (detail::CharacterValue<V> && detail::overlaps(T::_character_.order, detail::axis_mask<V>().order) &&
    detail::overlaps(T::_character_.field, detail::axis_mask<V>().field)));

}  // namespace mp_units
