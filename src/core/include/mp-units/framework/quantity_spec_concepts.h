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
#include <mp-units/bits/unsatisfied.h>
#include <mp-units/ext/type_traits.h>
#include <mp-units/framework/quantity_character.h>
#include <mp-units/framework/symbolic_expression.h>

namespace mp_units {

namespace detail {

struct quantity_spec_interface_base;

// No two arguments share a type, so a tag, a tensor order or a field is given at most once. Only an
// argument that is not a quantity specification can repeat, so the pairwise check runs only when
// two of them are given; the fold reuses the `is_base_of_v` that `QuantitySpec` instantiates anyway.
template<auto... Args>
concept QuantitySpecArguments =
  (std::size_t{0} + ... +
   std::size_t{!std::is_base_of_v<quantity_spec_interface_base, MP_UNITS_NTTP_HACK(decltype(Args))>}) < 2 ||
  mp_units::has_unique_types<Args...>() ||
  unsatisfied<"A quantity specification takes at most one value of each property type">();

}  // namespace detail

#if MP_UNITS_API_NO_CRTP
MP_UNITS_EXPORT template<auto... Args>
  requires detail::QuantitySpecArguments<Args...>
#else
MP_UNITS_EXPORT template<typename Self, auto... Args>
  requires detail::QuantitySpecArguments<Args...>
#endif
struct quantity_spec;

namespace detail {

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
/**
 * A concept matching every quantity specification
 *
 * Satisfied by a named quantity defined with `quantity_spec`, such as `isq::height`, by a
 * quantity kind made with `kind_of`, and by the unnamed specification a quantity equation
 * produces, such as `isq::length / isq::time`.
 *
 * @code{.cpp}
 * // a symbolic expression that tracks the quantity it computes
 * template<QuantitySpec auto QS>
 * class symbolic_expression {
 *   std::string expr_;
 * public:
 *   explicit symbolic_expression(std::string expr) : expr_(std::move(expr)) {}
 *   [[nodiscard]] const std::string& expression() const { return expr_; }
 * };
 *
 * template<QuantitySpec auto QS1, QuantitySpec auto QS2>
 * symbolic_expression<QS1 / QS2> operator/(const symbolic_expression<QS1>& lhs,
 *                                          const symbolic_expression<QS2>& rhs)
 * {
 *   return symbolic_expression<QS1 / QS2>("(" + lhs.expression() + " / " + rhs.expression() + ")");
 * }
 *
 * inline const symbolic_expression<isq::length> distance("d");
 * inline const symbolic_expression<isq::duration> duration("t");
 * inline const auto speed = distance / duration;  // a symbolic expression of length / duration
 * // doesn't compile; unconstrained, it would accept a unit
 * // symbolic_expression<si::metre> bad("d");
 * @endcode
 *
 * @code{.cpp}
 * // values of types nobody spells, so naming what each one is helps where `auto` alone doesn't
 * constexpr QuantitySpec auto power_spec = isq::energy / isq::duration;
 * constexpr Dimension auto power_dimension = get_dimension(power_spec);
 * constexpr Unit auto power_unit = si::joule / si::second;
 * constexpr Reference auto power_reference = power_spec[power_unit];
 * @endcode
 *
 * @see [Concepts](docs:users_guide/advanced_topics/concepts#QuantitySpec) in the User's Guide
 */
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
 * A concept matching the quantity specifications that convert to a given one or admit a given character
 *
 * `V` may be another quantity specification, in which case this is satisfied by every specification
 * implicitly convertible to it, or a character (a bare `quantity_tensor_order`, a bare
 * `quantity_field`, or a full `quantity_character`), in which case it is satisfied by every
 * specification that *admits* that character. A quantity kind admits every character. As in
 * @ref QuantityOf, the type of `V` selects the reading.
 *
 * Ask this where no quantity exists yet to ask @ref QuantityOf about, such as constraining a
 * reference taken as a template parameter (@ref utility::polar_vector requires a radius whose
 * specification admits scalar).
 *
 * @code{.cpp}
 * // a gauge reports a magnitude, never a direction
 * template<QuantitySpecOf<quantity_tensor_order::scalar> auto QS>
 * struct gauge_reading {
 *   quantity<QS[mm], double> value;
 * };
 *
 * constexpr gauge_reading<isq::thickness> coating{isq::thickness(0.25 * mm)};
 * // doesn't compile; unconstrained, it would accept a vector quantity, as a scalar may store one
 * // gauge_reading<isq::displacement> bad;
 * @endcode
 *
 * @code{.cpp}
 * // values of types nobody spells, so naming what each one is and which quantity it belongs to
 * // helps where `auto` alone doesn't
 * constexpr QuantitySpecOf<isq::power> auto power_spec = isq::energy / isq::duration;
 * constexpr UnitOf<isq::power> auto power_unit = si::joule / si::second;
 * constexpr ReferenceOf<isq::power> auto power_reference = power_spec[power_unit];
 * @endcode
 *
 * @see [Concepts](docs:users_guide/advanced_topics/concepts#QuantitySpecOf) in the User's Guide
 */
MP_UNITS_EXPORT template<typename T, auto V>
concept QuantitySpecOf =
  QuantitySpec<T> &&
  ((QuantitySpec<MP_UNITS_NTTP_HACK(decltype(V))> && mp_units::utility::implicitly_convertible(T{}, V)) ||
   (detail::CharacterValue<V> && detail::overlaps(T::_character_.order, detail::axis_mask<V>().order) &&
    detail::overlaps(T::_character_.field, detail::axis_mask<V>().field)));

}  // namespace mp_units
