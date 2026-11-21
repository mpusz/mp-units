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

#include <mp-units/bits/hacks.h>
#include <mp-units/framework/quantity.h>
#include <mp-units/framework/quantity_point.h>
#include <mp-units/framework/reference_concepts.h>
#include <mp-units/framework/representation_concepts.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <type_traits>
#endif
#endif

namespace mp_units {

template<Reference R>
  requires detail::SymbolicArg<R>
struct delta_ {
  template<typename FwdRep, RepresentationOf<get_quantity_spec(R{})> Rep = std::remove_cvref_t<FwdRep>>
  [[nodiscard]] constexpr quantity<R{}, Rep> operator()(FwdRep && lhs) const
  {
    return quantity{std::forward<FwdRep>(lhs), R{}};
  }
};

template<Reference R>
  requires detail::SymbolicArg<R>
struct point_ {
  template<typename FwdRep, RepresentationOf<get_quantity_spec(R{})> Rep = std::remove_cvref_t<FwdRep>>
  [[nodiscard]] constexpr quantity_point<R{}, default_point_origin(R{}), Rep> operator()(FwdRep && lhs) const
  {
    return quantity_point{quantity{std::forward<FwdRep>(lhs), R{}}};
  }
};

MP_UNITS_EXPORT_BEGIN

/**
 * Creates a delta of reference `R` from a number
 *
 * `delta<R>(v)` returns a @ref quantity of reference `R` that holds `v`, with the
 * representation type deduced from `v`. It gives the same result as `v * R`, which is
 * deprecated for a unit with a point origin, such as `si::kelvin` or `si::degree_Celsius`.
 * `42 * deg_C` could mean a temperature or a temperature difference, and `delta` and
 * @ref point state which one is meant.
 *
 * @code{.cpp}
 * // a thermostat switches the heating off half a degree above its setpoint
 * quantity_point setpoint = point<deg_C>(21.);
 * quantity hysteresis = delta<deg_C>(0.5);
 * quantity_point switch_off = setpoint + hysteresis;  // 21.5 °C
 * @endcode
 *
 * @tparam R the reference or unit of the delta
 *
 * @see @ref point for creating a point from a number
 * @see [The Affine Space](docs:users_guide/framework_basics/the_affine_space) for when a value is a delta and when it
 * is a point
 */
template<Reference auto R>
constexpr delta_<MP_UNITS_NTTP_HACK(decltype(R))> delta{};

/**
 * Creates a point of reference `R` from a number
 *
 * `point<R>(v)` returns a @ref quantity_point of reference `R` that holds `v`, measured from
 * the default origin of `R`. That is the origin of its unit when the unit has one, such as
 * `si::ice_point` for `si::degree_Celsius`, and the @ref natural_point_origin of its quantity
 * otherwise.
 *
 * @code{.cpp}
 * quantity_point morning = point<deg_C>(14.);
 * quantity_point noon = point<deg_C>(21.);
 * quantity warming = noon - morning;  // 7 °C, a delta
 * @endcode
 *
 * @tparam R the reference or unit of the point
 *
 * @see @ref delta for creating a delta from a number
 * @see @ref default_point_origin for the origin a reference implies
 * @see [The Affine Space](docs:users_guide/framework_basics/the_affine_space) for when a value is a delta and when it
 * is a point
 */
template<Reference auto R>
constexpr point_<MP_UNITS_NTTP_HACK(decltype(R))> point{};

/**
 * The former name of @ref point
 */
template<Reference auto R>
[[deprecated("2.5.0: Use `point` instead")]] constexpr point_<MP_UNITS_NTTP_HACK(decltype(R))> absolute{};

MP_UNITS_EXPORT_END

}  // namespace mp_units
