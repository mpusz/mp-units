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
  requires detail::SymbolicArg<Q> && detail::SymbolicArg<U>
struct reference;

MP_UNITS_EXPORT_BEGIN

/**
 * A concept matching every reference
 *
 * Satisfied by every @ref Unit, which serves as a reference on its own, and by a @ref reference
 * pairing a quantity specification with a unit, such as `isq::height[si::metre]`.
 *
 * @code{.cpp}
 * // running statistics of a measured signal: the variance has the square of the mean's reference
 * template<Reference auto R, typename Rep = double>
 * struct signal_statistics {
 *   quantity<R, Rep> mean{};
 *   quantity<pow<2>(R), Rep> variance{};
 * };
 *
 * signal_statistics<isq::height[mm]> heights;
 * // doesn't compile: a quantity specification has no unit
 * // signal_statistics<isq::height> bad;
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
 * @see [Concepts](docs:users_guide/advanced_topics/concepts#Reference) in the User's Guide
 */
template<typename T>
concept Reference = Unit<T> || is_specialization_of<std::remove_const_t<T>, reference>;

/**
 * A concept matching the references of a given quantity
 *
 * Satisfied by a @ref Reference whose quantity specification satisfies @ref QuantitySpecOf with
 * `QS`, so `QS` may also be a character. `isq::height[si::metre]` and `si::metre` are references
 * of `isq::length`, while `isq::length[si::metre]` is not a reference of `isq::height`.
 *
 * @code{.cpp}
 * // a field bus delivers raw integers, and the protocol says what each one means
 * template<ReferenceOf<isq::length> auto R>
 * constexpr quantity<R, std::int32_t> decode_length(std::int32_t raw) { return raw * R; }
 *
 * constexpr quantity height = decode_length<isq::height[mm]>(1500);  // 1500 mm
 * // doesn't compile: not a length
 * // auto bad = decode_length<isq::duration[ms]>(1500);
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
 * @see [Concepts](docs:users_guide/advanced_topics/concepts#ReferenceOf) in the User's Guide
 */
template<typename T, auto QS>
concept ReferenceOf = Reference<T> && QuantitySpecOf<decltype(get_quantity_spec(T{})), QS>;

MP_UNITS_EXPORT_END

}  // namespace mp_units
