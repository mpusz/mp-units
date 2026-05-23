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

#if defined MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG < 18

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

template<std::intmax_t N>
[[deprecated("2.5.0: `known_first_factor` is no longer necessary and can simply be removed")]]
constexpr std::optional<std::intmax_t>
  known_first_factor = std::nullopt;

template<detail::MagArg auto V>
  requires detail::is_nonzero_mag_arg<V>
constexpr UnitMagnitude auto mag = detail::make_magnitude<V>();

#if MP_UNITS_COMP_MSVC
// Workaround for MSVC ICE with ratio as NTTP in make_magnitude
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
template<std::intmax_t N, std::intmax_t D>
  requires(N != 0)
constexpr UnitMagnitude auto mag_ratio = detail::make_magnitude<detail::ratio{N, D}>();
#endif

/**
 * @brief  Create a Magnitude which is some rational number raised to a rational power.
 */
template<detail::MagArg auto Base, int Num, int Den = 1>
  requires detail::is_positive_mag_arg<Base>
constexpr UnitMagnitude auto mag_power = pow<Num, Den>(mag<Base>);

/**
 * @brief  A convenient Magnitude constant for pi, which we can manipulate like a regular number.
 */
#if defined MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG < 18
inline constexpr struct pi_c final : mag_constant<symbol_text{u8"π" /* U+03C0 GREEK SMALL LETTER PI */, "pi"}> {
  static constexpr auto _value_ = std::numbers::pi_v<long double>;
#else
inline constexpr struct pi_c final :
    mag_constant<symbol_text{u8"π" /* U+03C0 GREEK SMALL LETTER PI */, "pi"}, std::numbers::pi_v<long double> > {
#endif
} pi_c;

[[deprecated("2.3.0: Use `mag<pi>` instead")]] inline constexpr UnitMagnitude auto mag_pi = mag<pi_c>;

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
