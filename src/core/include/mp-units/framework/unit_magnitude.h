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
#include <mp-units/bits/unit_magnitude.h>
#include <mp-units/framework/symbol_text.h>
#include <mp-units/framework/unit_magnitude_concepts.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
// TODO remove when deprecated known_first_factor is removed
#include <optional>
#endif
#endif

namespace mp_units {

MP_UNITS_EXPORT_BEGIN

#if !MP_UNITS_HAS_FLOATING_POINT_NTTP

template<symbol_text Symbol>
struct mag_constant {
  static constexpr auto _symbol_ = Symbol;
};

#else

template<symbol_text Symbol, long double Value>
  requires(Value > 0)
struct mag_constant {
  static constexpr auto _symbol_ = Symbol;
  static constexpr long double _value_ = Value;
};

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// `mag()` implementation.

/**
 * No longer used, as the library factors magnitudes without a hint
 */
template<std::intmax_t N>
[[deprecated("2.5.0: `known_first_factor` is no longer necessary and can simply be removed")]]
constexpr std::optional<std::intmax_t>
  known_first_factor = std::nullopt;

/**
 * The unit magnitude of a number
 *
 * `V` is an integer or a constant defined with @ref mag_constant. A magnitude is kept as a product
 * of powers of primes and named constants, so multiplying magnitudes is exact. Multiplied by a
 * unit, it defines a unit by an exact factor:
 *
 * @code{.cpp}
 * inline constexpr struct nautical_mile final :
 *   named_unit<"nmi", mag<1852> * si::metre> {} nautical_mile;
 * @endcode
 *
 * @tparam V a nonzero integer or a @ref mag_constant
 *
 * @see [Systems of Units](docs:users_guide/framework_basics/systems_of_units#scaled-units) in the User's Guide
 */
template<detail::MagArg auto V>
  requires detail::is_nonzero_mag_arg<V>
constexpr UnitMagnitude auto mag = detail::make_magnitude<V>();

#if MP_UNITS_MAG_RATIO_HACK
/**
 * The unit magnitude of the ratio `N/D`
 *
 * It defines a unit as an exact fraction of another:
 *
 * @code{.cpp}
 * // snippet-template: si_excerpt
 * inline constexpr struct arcminute final :
 *   named_unit<symbol_text{u8"′", "'"}, mag_ratio<1, 60> * degree> {} arcminute;
 * @endcode
 *
 * @tparam N the numerator, nonzero
 * @tparam D the denominator
 *
 * @see [Systems of Units](docs:users_guide/framework_basics/systems_of_units#scaled-units) in the User's Guide
 */
template<std::intmax_t N, std::intmax_t D>
  requires(N != 0)
constexpr UnitMagnitude auto mag_ratio = []() consteval {
  constexpr auto abs_n = N < 0 ? -N : N;
  constexpr auto abs_d = D < 0 ? -D : D;
  // prime_factorization is only defined for positive N, so normalize both halves to be
  // positive and reconstruct the overall sign from N and D independently.
  constexpr auto abs_mag = detail::prime_factorization_v<abs_n> / detail::prime_factorization_v<abs_d>;
  if constexpr ((N < 0) != (D < 0))
    return detail::unit_magnitude<detail::negative_tag{}>{} * abs_mag;
  else
    return abs_mag;
}();
#else
/**
 * The unit magnitude of the ratio `N/D`
 *
 * It defines a unit as an exact fraction of another:
 *
 * @code{.cpp}
 * // snippet-template: si_excerpt
 * inline constexpr struct arcminute final :
 *   named_unit<symbol_text{u8"′", "'"}, mag_ratio<1, 60> * degree> {} arcminute;
 * @endcode
 *
 * @tparam N the numerator, nonzero
 * @tparam D the denominator
 *
 * @see [Systems of Units](docs:users_guide/framework_basics/systems_of_units#scaled-units) in the User's Guide
 */
template<std::intmax_t N, std::intmax_t D>
  requires(N != 0)
constexpr UnitMagnitude auto mag_ratio = detail::make_magnitude<detail::ratio{N, D}>();
#endif

/**
 * The unit magnitude of `Base` raised to the power `Num/Den`
 *
 * A prefix is defined with it, and so is a unit whose factor reads better as a power of ten:
 *
 * @code{.cpp}
 * template<PrefixableUnit U>
 * struct kilo_ final : prefixed_unit<"k", mag_power<10, 3>, U{}> {};
 *
 * inline constexpr struct nominal_solar_radius final :
 *   named_constant<symbol_text{u8"R_☉ᴺ", "R_SUN_N"},
 *                  mag_ratio<6'957, 1'000> * mag_power<10, 8> * si::metre> {} nominal_solar_radius;
 * @endcode
 *
 * @tparam Base the base, a positive integer or a @ref mag_constant
 * @tparam Num the numerator of the exponent
 * @tparam Den the denominator of the exponent
 *
 * @see [Systems of Units](docs:users_guide/framework_basics/systems_of_units#prefixed-units) in the User's Guide
 */
template<detail::MagArg auto Base, int Num, int Den = 1>
  requires detail::is_positive_mag_arg<Base>
constexpr UnitMagnitude auto mag_power = pow<Num, Den>(mag<Base>);

namespace detail {

// The magnitude constant behind the `pi` unit. It has no public name because `pi` is the only
// spelling users need, both on its own (`2 * pi`) and as a factor of a unit
// (`mag_ratio<1, 180> * π * rad`).
#if !MP_UNITS_HAS_FLOATING_POINT_NTTP
inline constexpr struct pi_c final : mag_constant<symbol_text{u8"π" /* U+03C0 GREEK SMALL LETTER PI */, "pi"}> {
  static constexpr auto _value_ = std::numbers::pi_v<long double>;
#else
inline constexpr struct pi_c final :
    mag_constant<symbol_text{u8"π" /* U+03C0 GREEK SMALL LETTER PI */, "pi"}, std::numbers::pi_v<long double> > {
#endif
} pi_c;

}  // namespace detail

/**
 * The former magnitude of π. Multiply by the unit @ref pi instead.
 */
[[deprecated("2.3.0: Use `mag<pi>` instead")]] inline constexpr UnitMagnitude auto mag_pi = mag<detail::pi_c>;

MP_UNITS_EXPORT_END

namespace detail {

// This is introduced to break the dependency cycle between `unit_magnitude::_magnitude_text` and `prime_factorization`
template<MagArg auto Base, int Num, int Den>
  requires is_positive_mag_arg<Base>
[[nodiscard]] consteval UnitMagnitude auto mag_power_lazy()
{
  return pow<Num, Den>(mag<Base>);
}

}  // namespace detail

}  // namespace mp_units
