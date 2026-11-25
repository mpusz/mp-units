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

// IWYU pragma: private, include <mp-units/framework/unit.h>
#include <mp-units/bits/hacks.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/bits/type_list.h>
#include <mp-units/bits/unit_conversion_impl.h>  // IWYU pragma: keep
#include <mp-units/framework/symbol_text.h>
#include <mp-units/framework/unit_concepts.h>
#include <mp-units/framework/unit_definitions.h>
#include <mp-units/framework/unit_magnitude.h>

namespace mp_units {

MP_UNITS_EXPORT_BEGIN

// common dimensionless units
// clang-format off
/**
 * Percent, a hundredth of @ref one
 *
 * @code{.cpp}
 * quantity efficiency = (1.7 * kW / (2. * kW)).in(percent);  // 85 %
 * @endcode
 *
 * @see @ref per_mille for thousandths
 * @see @ref parts_per_million for millionths
 */
inline constexpr struct percent final : named_unit<"%", mag_ratio<1, 100> * one> {} percent;

/**
 * Per mille, a thousandth of @ref one
 *
 * @code{.cpp}
 * quantity salinity = (35. * g / (1. * kg)).in(per_mille);  // 35 ‰, typical of seawater
 * @endcode
 *
 * @see @ref percent for hundredths
 * @see @ref parts_per_million for millionths
 */
inline constexpr struct per_mille final : named_unit<symbol_text{u8"‰" /* U+2030 PER MILLE SIGN */, "%o"}, mag_ratio<1, 1000> * one> {} per_mille;

/**
 * Parts per million, a millionth of @ref one
 *
 * @code{.cpp}
 * // the frequency error of a watch crystal
 * quantity nominal = 32'768. * Hz;
 * quantity measured = 32'768.5 * Hz;
 * quantity error = ((measured - nominal) / nominal).in(ppm);  // ≈ 15.26 ppm
 * @endcode
 *
 * @see @ref percent for hundredths
 * @see @ref per_mille for thousandths
 */
inline constexpr struct parts_per_million final : named_unit<"ppm", mag_ratio<1, 1'000'000> * one> {} parts_per_million;

/**
 * The short name of @ref parts_per_million
 */
inline constexpr auto ppm = parts_per_million;

// TODO make it a unit constant rather than a named unit when such support will be provided
/**
 * The number π, as a unit
 *
 * A quantity keeps the factor π in its unit, exactly, until it is converted to a unit without
 * it:
 *
 * @code{.cpp}
 * constexpr quantity radius = 2. * m;
 * constexpr quantity area = pow<2>(radius) * π;  // 4 π m²
 * constexpr quantity area_m2 = area.in(m2);      // ≈ 12.57 m²
 * @endcode
 */
inline constexpr struct pi final : named_constant<symbol_text{u8"π" /* U+03C0 GREEK SMALL LETTER PI */, "pi"}, mag<detail::pi_c> * one> {} pi;

/**
 * The Greek-letter name of @ref pi
 */
inline constexpr auto π /* U+03C0 GREEK SMALL LETTER PI */ = pi;
// clang-format on

MP_UNITS_EXPORT_END

namespace detail {

// The common unit of a set of units. Users reach it through `std::common_type_t` on two quantities.
// The name differs from the deprecated `mp_units::get_common_unit` because every unit
// derives from `detail::unit_interface`, so `detail` is an associated namespace. A same-named
// overload here would be a better match for an unqualified call, which would then never warn.
[[nodiscard]] consteval Unit auto find_common_unit(Unit auto u) { return u; }

template<Unit U1, UnitConvertibleTo<U1{}> U2>
[[nodiscard]] consteval Unit auto find_common_unit(U1 u1, U2 u2)
{
  return get_common_unit_impl(u1, u2);
}

template<Unit... Us, UnitConvertibleTo<common_unit<Us...>{}> NewUnit>
[[nodiscard]] consteval Unit auto find_common_unit(common_unit<Us...>, NewUnit)
{
  using type = collapse_common_unit<NewUnit, Us...>;
  if constexpr (type_list_size<type> == 1)
    return type_list_front<type>{};
  else
    return type_list_map<type, common_unit>{};
}

template<Unit... Us, UnitConvertibleTo<common_unit<Us...>{}> NewUnit>
[[nodiscard]] consteval Unit auto find_common_unit(NewUnit nu, common_unit<Us...> cu)
{
  return mp_units::detail::find_common_unit(cu, nu);
}

template<Unit Front, Unit... Rest, Unit... Us>
  requires(UnitConvertibleTo<common_unit<Front, Rest...>, common_unit<Us...>{}>)
[[nodiscard]] consteval Unit auto find_common_unit(common_unit<Front, Rest...>, common_unit<Us...>)
{
  if constexpr (sizeof...(Rest) == 1)
    return mp_units::detail::find_common_unit(Front{},
                                              mp_units::detail::find_common_unit(Rest{}..., common_unit<Us...>{}));
  else
    return mp_units::detail::find_common_unit(
      Front{}, mp_units::detail::find_common_unit(common_unit<Rest...>{}, common_unit<Us...>{}));
}

[[nodiscard]] consteval Unit auto find_common_unit(Unit auto u1, Unit auto u2, Unit auto u3, Unit auto... rest)
  requires requires { mp_units::detail::find_common_unit(mp_units::detail::find_common_unit(u1, u2), u3, rest...); }
{
  return mp_units::detail::find_common_unit(mp_units::detail::find_common_unit(u1, u2), u3, rest...);
}

}  // namespace detail

MP_UNITS_EXPORT_BEGIN

/**
 * The common unit of the arguments. Use `std::common_type_t` on two quantities instead.
 */
[[deprecated("2.6.0: Use `std::common_type_t` on two quantities instead")]] [[nodiscard]] consteval Unit auto
get_common_unit(Unit auto u, Unit auto... rest)
  requires requires { mp_units::detail::find_common_unit(u, rest...); }
{
  return mp_units::detail::find_common_unit(u, rest...);
}

MP_UNITS_EXPORT_END

}  // namespace mp_units
