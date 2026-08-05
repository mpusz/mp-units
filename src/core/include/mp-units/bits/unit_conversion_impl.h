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

#include <mp-units/bits/constexpr_math.h>
#include <mp-units/bits/type_list.h>
#include <mp-units/bits/unit_magnitude.h>
#include <mp-units/ext/inplace_vector.h>
#include <mp-units/ext/type_name.h>
#include <mp-units/framework/symbol_text.h>
#include <mp-units/framework/symbolic_expression.h>
#include <mp-units/framework/unit_concepts.h>
#include <mp-units/framework/unit_definitions.h>
#include <mp-units/framework/unit_magnitude.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <type_traits>
#endif
#endif

namespace mp_units::detail {

template<Unit T, auto M, typename U>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const scaled_unit_impl<M, U>&)
{
  auto base = mp_units::get_canonical_unit(U{});
  return canonical_unit{M * base.mag, base.reference_unit};
}

template<Unit T, typename... Us>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const common_unit<Us...>& u)
{
  return mp_units::get_canonical_unit(u._common_unit_);
}

template<Unit T, symbol_text Symbol, auto... Args>
[[nodiscard]] consteval auto get_canonical_unit_impl(T t, const named_unit<Symbol, Args...>&)
{
  return canonical_unit{mag<1>, t};
}

template<Unit T, symbol_text Symbol, Unit auto U, auto... Args>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const named_unit<Symbol, U, Args...>&)
{
  return mp_units::get_canonical_unit(U);
}

template<Unit T, symbol_text Symbol, Unit auto U, auto... Args>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const named_constant<Symbol, U, Args...>&)
{
  return mp_units::get_canonical_unit(U);
}

template<typename F, int Num, int... Den, typename... Us>
[[nodiscard]] consteval auto get_canonical_unit_impl(const power<F, Num, Den...>&, const type_list<Us...>&)
{
  auto mag = (mp_units::mag<1> * ... * pow<Num, Den...>(get_canonical_unit_impl(Us{}, Us{}).mag));
  auto ref_unit = (one * ... * pow<Num, Den...>(get_canonical_unit_impl(Us{}, Us{}).reference_unit));
  return canonical_unit{mag, ref_unit};
}

template<typename T, typename F, int Num, int... Den>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const power<F, Num, Den...>&)
{
  auto base = mp_units::get_canonical_unit(F{});
  if constexpr (requires { typename decltype(base.reference_unit)::_num_; }) {
    auto num = get_canonical_unit_impl(power<F, Num, Den...>{}, typename decltype(base.reference_unit)::_num_{});
    auto den = get_canonical_unit_impl(power<F, Num, Den...>{}, typename decltype(base.reference_unit)::_den_{});
    return canonical_unit{pow<Num, Den...>(base.mag) * num.mag / den.mag, num.reference_unit / den.reference_unit};
  } else {
    return canonical_unit{pow<Num, Den...>(base.mag),
                          derived_unit<power<decltype(base.reference_unit), Num, Den...>>{}};
  }
}

template<typename... Us>
[[nodiscard]] consteval auto get_canonical_unit_impl(const type_list<Us...>&)
{
  auto unit_magnitude = (mp_units::mag<1> * ... * get_canonical_unit_impl(Us{}, Us{}).mag);
  auto ref_unit = (one * ... * get_canonical_unit_impl(Us{}, Us{}).reference_unit);
  return canonical_unit{unit_magnitude, ref_unit};
}

template<Unit T, typename... Expr>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const derived_unit_impl<Expr...>&)
{
  auto num = get_canonical_unit_impl(typename derived_unit<Expr...>::_num_{});
  auto den = get_canonical_unit_impl(typename derived_unit<Expr...>::_den_{});
  return canonical_unit{num.mag / den.mag, num.reference_unit / den.reference_unit};
}

// A negative canonical magnitude can only originate from a `negative_tag` in a `scaled_unit` magnitude or
// in a `named_constant` definition, and such a tag always sorts first in a magnitude pack.  This allows
// determining the sign structurally in O(expression tree) without forming any magnitude products.
template<symbol_text Symbol, auto... Args>
[[nodiscard]] consteval bool unit_mag_is_positive(const named_unit<Symbol, Args...>&)
{
  return true;
}

template<symbol_text Symbol, Unit auto U, auto... Args>
[[nodiscard]] consteval bool unit_mag_is_positive(const named_unit<Symbol, U, Args...>&)
{
  return mp_units::detail::unit_mag_is_positive(U);
}

template<symbol_text Symbol, Unit auto U, auto... Args>
[[nodiscard]] consteval bool unit_mag_is_positive(const named_constant<Symbol, U, Args...>&)
{
  return mp_units::detail::unit_mag_is_positive(U);
}

template<typename F, int Num, int... Den>
[[nodiscard]] consteval bool unit_mag_is_positive(const power<F, Num, Den...>&)
{
  // an even exponent numerator makes the result positive regardless of the base sign
  return mp_units::detail::unit_mag_is_positive(F{}) || Num % 2 == 0;
}

template<typename... Us>
[[nodiscard]] consteval bool unit_mag_is_positive(const type_list<Us...>&)
{
  // the result is positive if and only if the number of negative factors is even
  return (0U + ... + static_cast<unsigned>(!mp_units::detail::unit_mag_is_positive(Us{}))) % 2 == 0;
}

template<typename... Expr>
[[nodiscard]] consteval bool unit_mag_is_positive(const derived_unit_impl<Expr...>&)
{
  // the sign of a factor is the same no matter if it ends up in the numerator or the denominator
  return mp_units::detail::unit_mag_is_positive(typename derived_unit<Expr...>::_num_{}) ==
         mp_units::detail::unit_mag_is_positive(typename derived_unit<Expr...>::_den_{});
}

template<auto M, typename U>
[[nodiscard]] consteval bool unit_mag_is_positive(const scaled_unit_impl<M, U>&)
{
  return check_magnitude_is_positive(M) == mp_units::detail::unit_mag_is_positive(U{});
}

template<typename... Us>
[[nodiscard]] consteval bool unit_mag_is_positive(const common_unit<Us...>& u)
{
  return mp_units::detail::unit_mag_is_positive(u._common_unit_);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// `conversion_relative_uncertainty` — the relative standard uncertainty of the conversion factor between two units,
// derived from the measured constants appearing in their definitions.
//
// Both unit expression trees are walked, accumulating the net exponent of every measured constant (identified by its
// type). A constant appearing on both sides cancels here exactly as its central value cancels in the magnitude ratio,
// so no spurious uncertainty is reported for e.g. a solar-mass -> Earth-mass conversion where `G` contributes to both
// units. The surviving contributions are combined in quadrature (first-order propagation) under a cross-constant
// independence assumption; the covariances CODATA publishes between its values are not modeled.
//
// The same two-argument dispatch as in `get_canonical_unit_impl` is used: the first argument preserves the actual
// derived type (needed to identify the constant and read its metadata), the second selects the overload by base.

struct measured_constant_contribution {
  std::string_view constant_id;  // unique per constant type
  std::intmax_t exp_num;         // net exponent of the constant in `from / to`
  std::intmax_t exp_den;
  long double relative_uncertainty;
};

// unit expressions reference only a handful of constants; the cap is a sanity bound, not a design limit
inline constexpr std::size_t max_measured_constants = 16;
using measured_constant_contributions = inplace_vector<measured_constant_contribution, max_measured_constants>;

consteval void add_measured_constant_contribution(measured_constant_contributions& out, std::string_view id,
                                                  std::intmax_t num, std::intmax_t den, long double rel_uncertainty)
{
  for (measured_constant_contribution& contrib : out)
    if (contrib.constant_id == id) {
      // sum of exponents: a/b + c/d = (a*d + c*b)/(b*d); the values are tiny, no normalization needed
      contrib.exp_num = contrib.exp_num * den + num * contrib.exp_den;
      contrib.exp_den = contrib.exp_den * den;
      return;
    }
  out.push_back(measured_constant_contribution{id, num, den, rel_uncertainty});
}

template<typename T, symbol_text Symbol, auto... Args>
consteval void collect_measured_constants(T, const named_unit<Symbol, Args...>&, measured_constant_contributions&,
                                          std::intmax_t, std::intmax_t);

template<typename T, symbol_text Symbol, Unit auto U, auto... Args>
consteval void collect_measured_constants(T, const named_unit<Symbol, U, Args...>&, measured_constant_contributions&,
                                          std::intmax_t, std::intmax_t);

template<typename T, symbol_text Symbol, Unit auto U, auto... Args>
consteval void collect_measured_constants(T, const named_constant<Symbol, U, Args...>&,
                                          measured_constant_contributions&, std::intmax_t, std::intmax_t);

template<typename T, typename F, int Num, int... Den>
consteval void collect_measured_constants(T, const power<F, Num, Den...>&, measured_constant_contributions&,
                                          std::intmax_t, std::intmax_t);

template<typename... Us>
consteval void collect_measured_constants(const type_list<Us...>&, measured_constant_contributions&, std::intmax_t,
                                          std::intmax_t);

template<typename T, typename... Expr>
consteval void collect_measured_constants(T, const derived_unit_impl<Expr...>&, measured_constant_contributions&,
                                          std::intmax_t, std::intmax_t);

template<typename T, auto M, typename U>
consteval void collect_measured_constants(T, const scaled_unit_impl<M, U>&, measured_constant_contributions&,
                                          std::intmax_t, std::intmax_t);

template<typename T, typename... Us>
consteval void collect_measured_constants(T, const common_unit<Us...>&, measured_constant_contributions&, std::intmax_t,
                                          std::intmax_t);

// base units are exact by construction
template<typename T, symbol_text Symbol, auto... Args>
consteval void collect_measured_constants(T, const named_unit<Symbol, Args...>&, measured_constant_contributions&,
                                          std::intmax_t, std::intmax_t)
{
}

template<typename T, symbol_text Symbol, Unit auto U, auto... Args>
consteval void collect_measured_constants(T, const named_unit<Symbol, U, Args...>&,
                                          measured_constant_contributions& out, std::intmax_t num, std::intmax_t den)
{
  mp_units::detail::collect_measured_constants(U, U, out, num, den);
}

template<typename T, symbol_text Symbol, Unit auto U, auto... Args>
consteval void collect_measured_constants(T, const named_constant<Symbol, U, Args...>&,
                                          measured_constant_contributions& out, std::intmax_t num, std::intmax_t den)
{
  if constexpr (MeasuredConstant<T>)
    // the declared uncertainty covers the constant's whole published value, so nested measured
    // constants (contributing only their central values to the magnitude) must not be added again;
    // the accessor also derives `u_r` for constants that declare the absolute form
    mp_units::detail::add_measured_constant_contribution(
      out, mp_units::detail::type_name<T>(), num, den,
      get_value<long double>(mp_units::get_relative_standard_uncertainty(T{})));
  else
    // an unannotated constant may still be defined in terms of measured ones
    mp_units::detail::collect_measured_constants(U, U, out, num, den);
}

template<typename T, typename F, int Num, int... Den>
consteval void collect_measured_constants(T, const power<F, Num, Den...>&, measured_constant_contributions& out,
                                          std::intmax_t num, std::intmax_t den)
{
  constexpr std::intmax_t denominator = (1 * ... * Den);
  mp_units::detail::collect_measured_constants(F{}, F{}, out, num * Num, den * denominator);
}

template<typename... Us>
consteval void collect_measured_constants(const type_list<Us...>&,
                                          [[maybe_unused]] measured_constant_contributions& out,
                                          [[maybe_unused]] std::intmax_t num, [[maybe_unused]] std::intmax_t den)
{
  (mp_units::detail::collect_measured_constants(Us{}, Us{}, out, num, den), ...);
}

template<typename T, typename... Expr>
consteval void collect_measured_constants(T, const derived_unit_impl<Expr...>&, measured_constant_contributions& out,
                                          std::intmax_t num, std::intmax_t den)
{
  mp_units::detail::collect_measured_constants(typename derived_unit<Expr...>::_num_{}, out, num, den);
  mp_units::detail::collect_measured_constants(typename derived_unit<Expr...>::_den_{}, out, -num, den);
}

template<typename T, auto M, typename U>
consteval void collect_measured_constants(T, const scaled_unit_impl<M, U>&, measured_constant_contributions& out,
                                          std::intmax_t num, std::intmax_t den)
{
  mp_units::detail::collect_measured_constants(U{}, U{}, out, num, den);
}

template<typename T, typename... Us>
consteval void collect_measured_constants(T, const common_unit<Us...>& u, measured_constant_contributions& out,
                                          std::intmax_t num, std::intmax_t den)
{
  mp_units::detail::collect_measured_constants(u._common_unit_, u._common_unit_, out, num, den);
}

template<Unit U1, Unit U2>
[[nodiscard]] consteval long double conversion_relative_uncertainty(U1 u1, U2 u2)
{
  measured_constant_contributions contributions;
  mp_units::detail::collect_measured_constants(u1, u1, contributions, 1, 1);
  mp_units::detail::collect_measured_constants(u2, u2, contributions, -1, 1);
  long double sum = 0.0L;
  for (const measured_constant_contribution& contrib : contributions)
    if (contrib.exp_num != 0) {
      const long double weighted = static_cast<long double>(contrib.exp_num) /
                                   static_cast<long double>(contrib.exp_den) * contrib.relative_uncertainty;
      sum += weighted * weighted;
    }
  // `std::sqrt` is not constexpr before C++26, so the library's own compile-time root is used
  return sum > 0.0L ? root(sum, 2).value_or(0.0L) : 0.0L;
}

// evaluated once per unit pair; the conversion engine consults it only for representation types
// that opted into uncertainty-aware scaling
template<Unit auto From, Unit auto To>
constexpr long double conversion_relative_uncertainty_result = conversion_relative_uncertainty(From, To);

// the engine behind the binary exported `get_common_unit` overload; also used by
// `collapse_common_unit` below, which the `common_unit`-flattening overloads rely on
template<Unit U1, UnitConvertibleTo<U1{}> U2>
[[nodiscard]] consteval Unit auto get_common_unit_impl(U1 u1, U2 u2)
{
  if constexpr (std::is_same_v<U1, U2>)
    return u1;
  else if constexpr (equivalent(U1{}, U2{})) {
    if constexpr (std::derived_from<U1, typename U2::_base_type_>)
      return u1;
    else if constexpr (std::derived_from<U2, typename U1::_base_type_>)
      return u2;
    else
      // TODO Check if there is a better choice here
      return detail::better_type_name(u1, u2);
  } else {
    constexpr auto canonical_lhs = mp_units::get_canonical_unit(U1{});
    constexpr auto canonical_rhs = mp_units::get_canonical_unit(U2{});

    if constexpr (is_positive_integral_power(canonical_lhs.mag / canonical_rhs.mag))
      return u2;
    else if constexpr (is_positive_integral_power(canonical_rhs.mag / canonical_lhs.mag))
      return u1;
    else if constexpr (detail::is_specialization_of_scaled_unit<U1> && detail::is_specialization_of_scaled_unit<U2>)
      // Both operands are anonymous scaled units over the same reference unit. A `common_unit` only
      // pays off when it preserves recognizable named units (e.g. `mi` and `km`); here there are no
      // names to keep, so a nested composite would be strictly uglier than the single scaled unit the
      // two already share. Collapse to it (e.g. the origin displacement units `1/20 K` and `1/9 K`
      // combine to `1/180 K` rather than `[(1/9 (1/20 K)), (1/20 (1/9 K))]`).
      return detail::get_common_scaled_unit(U1{}, U2{});
    else {
      if constexpr (detail::type_name_less<U1, U2>::value)
        return common_unit<U1, U2>{};
      else
        return common_unit<U2, U1>{};
    }
  }
}

template<TypeList List, Unit NewUnit, bool Included, Unit... Us>
struct collapse_common_unit_impl;

template<TypeList List, Unit NewUnit, bool Included, Unit Front, Unit... Rest>
struct collapse_common_unit_impl<List, NewUnit, Included, Front, Rest...> {
  using cu = decltype(mp_units::detail::get_common_unit_impl(NewUnit{}, Front{}));
  using type =
    conditional<is_specialization_of<cu, common_unit>,
                typename collapse_common_unit_impl<type_list_push_back<List, Front>, NewUnit, Included, Rest...>::type,
                typename collapse_common_unit_impl<type_list_push_back<List, cu>, NewUnit, true, Rest...>::type>;
};

template<TypeList List, Unit NewUnit>
struct collapse_common_unit_impl<List, NewUnit, false> {
  using type = type_list_push_back<List, NewUnit>;
};

template<TypeList List, Unit NewUnit>
struct collapse_common_unit_impl<List, NewUnit, true> {
  using type = List;
};

template<Unit NewUnit, Unit... Us>
using collapse_common_unit = type_list_unique<
  type_list_sort<typename collapse_common_unit_impl<type_list<>, NewUnit, false, Us...>::type, type_list_name_less>>;

}  // namespace mp_units::detail
