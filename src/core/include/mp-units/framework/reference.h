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
#include <mp-units/bits/get_associated_quantity.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/compat_macros.h>
#include <mp-units/framework/quantity_concepts.h>
#include <mp-units/framework/quantity_point_concepts.h>
#include <mp-units/framework/reference_concepts.h>
#include <mp-units/framework/representation_concepts.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cstdint>
#endif
#endif

namespace mp_units {

namespace detail {

template<QuantitySpec auto Q, Unit auto U>
using reference_t = reference<MP_UNITS_REMOVE_CONST(decltype(Q)), MP_UNITS_REMOVE_CONST(decltype(U))>;

}  // namespace detail

MP_UNITS_EXPORT_BEGIN

template<MP_UNITS_ASSOCIATED_UNIT U>
[[nodiscard]] consteval QuantitySpec auto get_quantity_spec(U)
{
  return kind_of<detail::get_associated_quantity(U{})>;
}

/**
 * @brief Quantity reference type
 *
 * Quantity reference describes all the properties of a quantity besides its
 * representation type.
 *
 * In most cases this class template is not explicitly instantiated by the user.
 * It is implicitly instantiated by the library's framework while binding a quantity
 * specification with a compatible unit.
 *
 * @code{.cpp}
 * Reference auto kmph = isq::speed[km / h];
 * QuantityOf<isq::speed[km / h]> auto speed = 90 * kmph;
 * @endcode
 *
 * The following syntaxes are not allowed:
 * `2 / kmph`, `kmph * 3`, `kmph / 4`, `70 * isq::length[km] / isq:time[h]`.
 */
template<QuantitySpec Q, Unit U>
struct reference {
  template<typename Q2, typename U2>
  [[nodiscard]] friend consteval bool operator==(reference, reference<Q2, U2>)
  {
    return Q{} == Q2{} && U{} == U2{};
  }

  template<MP_UNITS_ASSOCIATED_UNIT U2>
  [[nodiscard]] friend consteval bool operator==(reference, U2 u2)
  {
    return Q{} == get_quantity_spec(u2) && U{} == u2;
  }

  template<typename Q2, typename U2>
  [[nodiscard]] friend consteval detail::reference_t<MP_UNITS_EXPRESSION_WORKAROUND(Q{} * Q2{}),
                                                     MP_UNITS_EXPRESSION_WORKAROUND(U{} * U2{})>
  operator*(reference, reference<Q2, U2>)
  {
    return {};
  }

  template<MP_UNITS_ASSOCIATED_UNIT U2>
  [[nodiscard]] friend consteval detail::reference_t<(MP_UNITS_EXPRESSION_WORKAROUND(Q{} * get_quantity_spec(U2{}))),
                                                     MP_UNITS_EXPRESSION_WORKAROUND(U{} * U2{})>
  operator*(reference, U2)
  {
    return {};
  }

  template<MP_UNITS_ASSOCIATED_UNIT U1>
  [[nodiscard]] friend consteval detail::reference_t<MP_UNITS_EXPRESSION_WORKAROUND(get_quantity_spec(U1{}) * Q{}),
                                                     MP_UNITS_EXPRESSION_WORKAROUND(U1{} * U{})>
  operator*(U1, reference)
  {
    return {};
  }

  template<typename Q2, typename U2>
  [[nodiscard]] friend consteval detail::reference_t<MP_UNITS_EXPRESSION_WORKAROUND(Q{} / Q2{}),
                                                     MP_UNITS_EXPRESSION_WORKAROUND(U{} / U2{})>
  operator/(reference, reference<Q2, U2>)
  {
    return {};
  }

  template<MP_UNITS_ASSOCIATED_UNIT U2>
  [[nodiscard]] friend consteval detail::reference_t<MP_UNITS_EXPRESSION_WORKAROUND(Q{} / get_quantity_spec(U2{})),
                                                     MP_UNITS_EXPRESSION_WORKAROUND(U{} / U2{})>
  operator/(reference, U2)
  {
    return {};
  }

  template<MP_UNITS_ASSOCIATED_UNIT U1>
  [[nodiscard]] friend consteval detail::reference_t<MP_UNITS_EXPRESSION_WORKAROUND(get_quantity_spec(U1{}) / Q{}),
                                                     MP_UNITS_EXPRESSION_WORKAROUND(U1{} / U{})>
  operator/(U1, reference)
  {
    return {};
  }

  [[nodiscard]] friend consteval detail::reference_t<MP_UNITS_EXPRESSION_WORKAROUND(inverse(Q{})),
                                                     MP_UNITS_EXPRESSION_WORKAROUND(inverse(U{}))>
  inverse(reference)
  {
    return {};
  }

  /**
   * @brief Computes the value of a reference raised to the `Num/Den` power
   *
   * @tparam Num Exponent numerator
   * @tparam Den Exponent denominator
   * @param r Reference being the base of the operation
   *
   * @return The result of computation
   */
  template<std::intmax_t Num, std::intmax_t Den = 1>
    requires(Den != 0)
  [[nodiscard]] friend consteval detail::reference_t<MP_UNITS_EXPRESSION_WORKAROUND((pow<Num, Den>(Q{}))),
                                                     MP_UNITS_EXPRESSION_WORKAROUND((pow<Num, Den>(U{})))>
  pow(reference)
  {
    return {};
  }

  /**
   * @brief Computes the square root of a reference
   *
   * @param r Reference being the base of the operation
   *
   * @return The result of computation
   */
  [[nodiscard]] friend consteval detail::reference_t<MP_UNITS_EXPRESSION_WORKAROUND(sqrt(Q{})),
                                                     MP_UNITS_EXPRESSION_WORKAROUND(sqrt(U{}))>
  sqrt(reference)
  {
    return {};
  }

  /**
   * @brief Computes the cubic root of a reference
   *
   * @param r Reference being the base of the operation
   *
   * @return The result of computation
   */
  [[nodiscard]] friend consteval detail::reference_t<MP_UNITS_EXPRESSION_WORKAROUND(cbrt(Q{})),
                                                     MP_UNITS_EXPRESSION_WORKAROUND(cbrt(U{}))>
  cbrt(reference)
  {
    return {};
  }
};


template<typename FwdRep, Reference R, RepresentationOf<get_quantity_spec(R{})> Rep = std::remove_cvref_t<FwdRep>>
  requires(!detail::OffsetUnit<decltype(get_unit(R{}))>)
[[nodiscard]] constexpr quantity<R{}, Rep> operator*(FwdRep&& lhs, R r)
{
  return quantity{std::forward<FwdRep>(lhs), r};
}

template<typename FwdRep, Reference R, RepresentationOf<get_quantity_spec(R{})> Rep = std::remove_cvref_t<FwdRep>>
  requires(!detail::OffsetUnit<decltype(get_unit(R{}))>)
[[nodiscard]] constexpr Quantity auto operator/(FwdRep&& lhs, R)
{
  return quantity{std::forward<FwdRep>(lhs), inverse(R{})};
}

template<typename FwdRep, Reference R, RepresentationOf<get_quantity_spec(R{})> Rep = std::remove_cvref_t<FwdRep>>
  requires detail::OffsetUnit<decltype(get_unit(R{}))>
[[deprecated(
  "2.3.0: References using offset units (e.g., temperatures) should be constructed with the `delta` or `point` "
  "helpers")]] constexpr auto
operator*(FwdRep&& lhs, R r)
{
  return quantity{std::forward<FwdRep>(lhs), r};
}

template<typename FwdRep, Reference R, RepresentationOf<get_quantity_spec(R{})> Rep = std::remove_cvref_t<FwdRep>>
  requires detail::OffsetUnit<decltype(get_unit(R{}))>
[[deprecated(
  "2.3.0: References using offset units (e.g., temperatures) should be constructed with the `delta` or `point` "
  "helpers")]] constexpr auto
operator/(FwdRep&& lhs, R)
{
  return quantity{std::forward<FwdRep>(lhs), inverse(R{})};
}

template<Reference R, typename Rep>
  requires RepresentationOf<std::remove_cvref_t<Rep>, get_quantity_spec(R{})>
// NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward)
constexpr auto operator*(R, Rep&&)
#if __cpp_deleted_function
  = delete("To create a `quantity` use `Rep * R`");
#else
  = delete;
#endif

template<Reference R, typename Rep>
  requires RepresentationOf<std::remove_cvref_t<Rep>, get_quantity_spec(R{})>
// NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward)
constexpr auto operator/(R, Rep&&)
#if __cpp_deleted_function
  = delete("To create a `quantity` use `Rep / R`");
#else
  = delete;
#endif

template<typename FwdQ, Reference R, Quantity Q = std::remove_cvref_t<FwdQ>>
[[nodiscard]] constexpr Quantity auto operator*(FwdQ&& q, R)
{
  return quantity{std::forward<FwdQ>(q).numerical_value_is_an_implementation_detail_, Q::reference * R{}};
}

template<typename FwdQ, Reference R, Quantity Q = std::remove_cvref_t<FwdQ>>
[[nodiscard]] constexpr Quantity auto operator/(FwdQ&& q, R)
{
  return quantity{std::forward<FwdQ>(q).numerical_value_is_an_implementation_detail_, Q::reference / R{}};
}

template<Reference R, typename Q>
  requires Quantity<std::remove_cvref_t<Q>>
// NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward)
constexpr auto operator*(R, Q&& q) = delete;

template<Reference R, typename Q>
  requires Quantity<std::remove_cvref_t<Q>>
// NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward)
constexpr auto operator/(R, Q&& q) = delete;

[[nodiscard]] consteval MP_UNITS_ASSOCIATED_UNIT auto get_common_reference(MP_UNITS_ASSOCIATED_UNIT auto u1,
                                                                           MP_UNITS_ASSOCIATED_UNIT auto u2,
                                                                           MP_UNITS_ASSOCIATED_UNIT auto... rest)
  requires requires {
    get_common_quantity_spec(get_quantity_spec(u1), get_quantity_spec(u2), get_quantity_spec(rest)...);
    get_common_unit(u1, u2, rest...);
  }
{
  return get_common_unit(u1, u2, rest...);
}

template<Reference R1, Reference R2, Reference... Rest>
[[nodiscard]] consteval Reference auto get_common_reference(R1 r1, R2 r2, Rest... rest)
  requires(!(MP_UNITS_ASSOCIATED_UNIT_T(R1) && MP_UNITS_ASSOCIATED_UNIT_T(R2) &&
             (... && MP_UNITS_ASSOCIATED_UNIT_T(Rest)))) &&
          requires {
            get_common_quantity_spec(get_quantity_spec(r1), get_quantity_spec(r2), get_quantity_spec(rest)...);
            get_common_unit(get_unit(r1), get_unit(r2), get_unit(rest)...);
          }
{
  return detail::reference_t<get_common_quantity_spec(get_quantity_spec(R1{}), get_quantity_spec(R2{}),
                                                      get_quantity_spec(rest)...),
                             get_common_unit(get_unit(R1{}), get_unit(R2{}), get_unit(rest)...)>{};
}

MP_UNITS_EXPORT_END

namespace detail {

template<MP_UNITS_ASSOCIATED_UNIT auto To, MP_UNITS_ASSOCIATED_UNIT From>
[[nodiscard]] consteval MP_UNITS_REMOVE_CONST(decltype(To)) clone_reference_with(From)
{
  return {};
}

template<Unit auto To, QuantitySpec QS, Unit U>
[[nodiscard]] consteval reference<QS, MP_UNITS_REMOVE_CONST(decltype(To))> clone_reference_with(reference<QS, U>)
{
  return {};
}

}  // namespace detail

}  // namespace mp_units
