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
#include <mp-units/bits/module_macros.h>
#include <mp-units/ext/type_traits.h>
#include <mp-units/framework/symbol_text.h>
#include <mp-units/framework/symbolic_expression.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <concepts>
#endif
#endif

namespace mp_units {

#if !MP_UNITS_HAS_FLOATING_POINT_NTTP
MP_UNITS_EXPORT template<symbol_text Symbol>
#else
MP_UNITS_EXPORT template<symbol_text Symbol, long double Value>
  requires(Value > 0)
#endif
struct mag_constant;

namespace detail {

template<auto... Ms>
struct unit_magnitude;

template<typename T>
constexpr bool is_mag_constant = is_derived_from_specialization_of_v<T, mag_constant> && detail::SymbolicConstant<T>;

}  // namespace detail

/**
 * A concept matching every unit magnitude
 *
 * A unit magnitude is the exact factor between a unit and the unit it is defined from, created
 * by `mag`, `mag_ratio`, `mag_power`, and their products, as in `mag<1000> * si::metre`. Its type
 * is an implementation detail, so this concept is the only public way to name one. A
 * representation type needs that when it scales itself, through an `operator*(T, M)` that may
 * return a different type, as one tracking its own precision does.
 *
 * @code{.cpp}
 * // snippet-template: none (pseudo-code: the representation type elides its operators with `...`)
 * // the power of ten at or below a magnitude
 * template<UnitMagnitude auto M>
 * consteval int decimal_shift();
 *
 * // a decimal fixed-point number, `count` units of 10 to the power of `Exp`
 * template<int Exp>
 * struct decimal {
 *   std::int64_t count;
 *
 *   // ... the arithmetic and comparison operators of a representation type ...
 *
 *   // called by the library during a unit conversion, with the conversion factor: its power of
 *   // ten moves the decimal point into the type, and only the rest rescales the count
 *   template<UnitMagnitude M>
 *   friend constexpr auto operator*(decimal value, M)
 *   {
 *     constexpr int shift = decimal_shift<M{}>();
 *     return decimal<Exp + shift>{
 *       scale_by_magnitude<std::int64_t>(M{} / mag_power<10, shift>, value.count)};
 *   }
 * };
 *
 * // the type rescales its own precision, so a conversion may be implicit
 * template<Unit auto From, int Exp, Unit auto To>
 * constexpr bool mp_units::implicitly_scalable<From, decimal<Exp>, To, decimal<Exp>> = true;
 *
 * quantity<si::metre, decimal<0>> track_length{decimal<0>{1500}, si::metre};
 * quantity in_kilometres = track_length.in(km);          // decimal<-3>{1500}, exactly 1.5 km
 * quantity in_feet = track_length.in(yard_pound::foot);  // decimal<0>{4921}
 * @endcode
 *
 * @code{.cpp}
 * // values of types nobody spells, so naming what each one is helps where `auto` alone doesn't
 * constexpr UnitMagnitude auto milli_factor = mag_ratio<1, 1000>;
 * constexpr Unit auto thousandth_of_metre = milli_factor * si::metre;
 * @endcode
 *
 * @see [Representation Types](docs:users_guide/advanced_topics/representation_types#unit-magnitude-aware-scaling)
 * for a representation type that scales itself
 * @see [Concepts](docs:users_guide/advanced_topics/concepts#UnitMagnitude) in the User's Guide
 */
MP_UNITS_EXPORT template<typename T>
concept UnitMagnitude = is_specialization_of_v<std::remove_const_t<T>, detail::unit_magnitude>;

}  // namespace mp_units
