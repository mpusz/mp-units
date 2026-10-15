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

MP_UNITS_EXPORT template<typename T>
concept QuantitySpec = std::derived_from<T, detail::quantity_spec_interface_base> && detail::SymbolicConstant<T> &&
                       !detail::is_specialization_of_quantity_spec<std::remove_const_t<T>>;

template<typename Q>
struct kind_of_;

namespace detail {

template<typename T>
concept QuantityKindSpec = QuantitySpec<T> && is_specialization_of<std::remove_const_t<T>, kind_of_>;

// What a quantity specification admits, as a flag set per axis. Deliberately not public, and named
// for the question it answers rather than for the old public `get_character`: a concrete
// specification admits one character, while a quantity kind admits every one of them, and only
// `admits()` / `admits_only()` can tell those apart. Public code asks `QuantitySpecOf` with a
// character, or `QuantityOf` / `QuantityPointOf`, which also weigh the representation.
template<QuantitySpec QS>
[[nodiscard]] consteval character_mask admitted_character(QS qs)
{
  return qs._character_;
}

// A factor of a derived quantity specification that has not committed to a quantity below `N`. It is
// the conversion engine's reading of a `kind_of_<K>` factor, and of what such a factor expands to,
// which is why `N` may be any named quantity and not only a kind root: an expansion keeps the names
// of the equation it came from.
template<typename N>
struct open_factor {
  static constexpr auto _dimension_ = N::_dimension_;
};

template<typename T>
constexpr bool is_open_factor = false;

template<typename N>
constexpr bool is_open_factor<open_factor<N>> = true;

template<typename T>
struct unwrap_open_factor_impl : std::type_identity<T> {};

template<typename N>
struct unwrap_open_factor_impl<open_factor<N>> : std::type_identity<N> {};

template<typename T>
using unwrap_open_factor = unwrap_open_factor_impl<T>::type;

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
 * specification that *admits* that character.
 *
 * The two readings are guarded by disjoint argument types and never widen each other. "Admits"
 * rather than "is": a concrete specification admits exactly one character, while a quantity kind
 * admits every one of them, because a quantity of that kind may hold any. That is why this is the
 * question to ask a specification, and why there is no character value to read back out of one.
 *
 * Ask this where no quantity exists yet to ask `QuantityOf` about, such as constraining a reference
 * taken as a template parameter (`utility::polar_vector` requires a scalar radius exactly this way).
 */
MP_UNITS_EXPORT template<typename T, auto V>
concept QuantitySpecOf =
  QuantitySpec<T> &&
  ((QuantitySpec<MP_UNITS_NTTP_HACK(decltype(V))> && mp_units::utility::implicitly_convertible(T{}, V)) ||
   (detail::CharacterValue<V> &&
    detail::overlaps(detail::admitted_character(T{}).order, detail::axis_mask<V>().order) &&
    detail::overlaps(detail::admitted_character(T{}).field, detail::axis_mask<V>().field)));

}  // namespace mp_units
