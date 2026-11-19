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
#include <mp-units/compat_macros.h>
#include <mp-units/ext/type_traits.h>
#include <mp-units/framework/symbol_text.h>
#include <mp-units/framework/symbolic_expression.h>

namespace mp_units {

namespace detail {

struct dimension_interface;

}

MP_UNITS_EXPORT template<symbol_text Symbol>
struct base_dimension;

// `is_base_of_v` rather than `derived_from`: one constraint atom instead of two, evaluated per type; the base is
// always public and unambiguous, so the convertibility atom `derived_from` adds can never fail here.
/**
 * A concept matching every dimension
 *
 * Satisfied by a base dimension, which derives from `base_dimension`, and by a derived
 * dimension, which the library computes from the equation of a quantity specification.
 *
 * @code{.cpp}
 * // whether each state variable of a Kalman filter is the time derivative of the one before it
 * template<Dimension auto... Ds>
 * constexpr bool are_time_derivatives = true;
 *
 * template<Dimension auto D1, Dimension auto D2, Dimension auto... Ds>
 * constexpr bool are_time_derivatives<D1, D2, Ds...> =
 *   (D1 / D2 == isq::dim_time) && are_time_derivatives<D2, Ds...>;
 *
 * // the state of a Kalman filter: position, then its rate of change, then that rate's
 * template<QuantityPoint... QPs>
 *   requires are_time_derivatives<QPs::dimension...>
 * class system_state;
 * // doesn't compile: not dimensions
 * // auto bad = are_time_derivatives<isq::length, isq::speed>;
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
 * @see [Concepts](docs:users_guide/advanced_topics/concepts#Dimension) in the User's Guide
 */
MP_UNITS_EXPORT template<typename T>
concept Dimension = std::is_base_of_v<detail::dimension_interface, T> && detail::SymbolicConstant<T> &&
                    !is_specialization_of_v<std::remove_const_t<T>, base_dimension>;

namespace detail {

/**
 * A concept matching all named base dimensions in the library.
 *
 * Satisfied by all dimension types derived from a specialization of `base_dimension`.
 */
template<typename T>
concept BaseDimension = Dimension<T> && is_derived_from_specialization_of_v<T, base_dimension>;

}  // namespace detail

/**
 * A concept matching a dimension equal to a given one
 */
MP_UNITS_EXPORT template<typename T, auto D>
concept DimensionOf MP_UNITS_DEPRECATED_CONCEPT("2.6.0: Compare the dimensions with `==` instead") =
  Dimension<T> && Dimension<MP_UNITS_NTTP_HACK(decltype(D))> && (T{} == D);

}  // namespace mp_units
