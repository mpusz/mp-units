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

#include <mp-units/bits/get_associated_quantity.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/bits/quantity_concepts.h>
#include <mp-units/bits/reference_concepts.h>
#include <mp-units/bits/representation_concepts.h>

namespace mp_units {

namespace detail {

template<QuantitySpec auto Q, Unit auto U>
using reference_t = reference<std::remove_const_t<decltype(Q)>, std::remove_const_t<decltype(U)>>;

}

MP_UNITS_EXPORT_BEGIN

[[nodiscard]] consteval QuantitySpec auto get_quantity_spec(AssociatedUnit auto u)
{
  return detail::get_associated_quantity(u);
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

  template<AssociatedUnit U2>
  [[nodiscard]] friend consteval bool operator==(reference, U2 u2)
  {
    return Q{} == get_quantity_spec(u2) && U{} == u2;
  }

  template<typename Q2, typename U2>
  [[nodiscard]] friend consteval detail::reference_t<Q{} * Q2{}, U{} * U2{}> operator*(reference, reference<Q2, U2>)
  {
    return {};
  }

  template<AssociatedUnit U2>
  [[nodiscard]] friend consteval detail::reference_t<Q{} * get_quantity_spec(U2{}), U{} * U2{}> operator*(reference, U2)
  {
    return {};
  }

  template<AssociatedUnit U1>
  [[nodiscard]] friend consteval detail::reference_t<get_quantity_spec(U1{}) * Q{}, U1{} * U{}> operator*(U1, reference)
  {
    return {};
  }

  template<typename Q2, typename U2>
  [[nodiscard]] friend consteval detail::reference_t<Q{} / Q2{}, U{} / U2{}> operator/(reference, reference<Q2, U2>)
  {
    return {};
  }

  template<AssociatedUnit U2>
  [[nodiscard]] friend consteval detail::reference_t<Q{} / get_quantity_spec(U2{}), U{} / U2{}> operator/(reference, U2)
  {
    return {};
  }

  template<AssociatedUnit U1>
  [[nodiscard]] friend consteval detail::reference_t<get_quantity_spec(U1{}) / Q{}, U1{} / U{}> operator/(U1, reference)
  {
    return {};
  }

  [[nodiscard]] friend consteval detail::reference_t<inverse(Q{}), inverse(U{})> inverse(reference) { return {}; }

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
    requires detail::non_zero<Den>
  [[nodiscard]] friend consteval detail::reference_t<pow<Num, Den>(Q{}), pow<Num, Den>(U{})> pow(reference)
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
  [[nodiscard]] friend consteval detail::reference_t<sqrt(Q{}), sqrt(U{})> sqrt(reference) { return {}; }

  /**
   * @brief Computes the cubic root of a reference
   *
   * @param r Reference being the base of the operation
   *
   * @return The result of computation
   */
  [[nodiscard]] friend consteval detail::reference_t<cbrt(Q{}), cbrt(U{})> cbrt(reference) { return {}; }

  template<typename Q2, typename U2>
  [[nodiscard]] friend consteval bool convertible(reference, reference<Q2, U2>)
  {
    return implicitly_convertible(Q{}, Q2{}) && convertible(U{}, U2{});
  }

  template<AssociatedUnit U2>
  [[nodiscard]] friend consteval bool convertible(reference, U2 u2)
  {
    return implicitly_convertible(Q{}, get_quantity_spec(u2)) && convertible(U{}, u2);
  }

  template<AssociatedUnit U1>
  [[nodiscard]] friend consteval bool convertible(U1 u1, reference)
  {
    return implicitly_convertible(get_quantity_spec(u1), Q{}) && convertible(u1, U{});
  }
};


template<typename Rep, Reference R>
  requires RepresentationOf<std::remove_cvref_t<Rep>, get_quantity_spec(R{}).character>
[[nodiscard]] constexpr quantity<R{}, std::remove_cvref_t<Rep>> operator*(Rep&& lhs, R)
{
  return quantity{std::forward<Rep>(lhs), R{}};
}

template<typename Rep, Reference R>
  requires RepresentationOf<std::remove_cvref_t<Rep>, get_quantity_spec(R{}).character>
[[nodiscard]] constexpr quantity<inverse(R{}), std::remove_cvref_t<Rep>> operator/(Rep&& lhs, R)
{
  return quantity{std::forward<Rep>(lhs), inverse(R{})};
}

template<Reference R, typename Rep>
  requires RepresentationOf<std::remove_cvref_t<Rep>, get_quantity_spec(R{}).character>
constexpr auto operator*(R, Rep&&) = delete;

template<Reference R, typename Rep>
  requires RepresentationOf<std::remove_cvref_t<Rep>, get_quantity_spec(R{}).character>
constexpr auto operator/(R, Rep&&) = delete;

template<typename Q, Reference R>
  requires Quantity<std::remove_cvref_t<Q>>
[[nodiscard]] constexpr Quantity auto operator*(Q&& q, R)
{
  return quantity{std::forward<Q>(q).numerical_value_is_an_implementation_detail_,
                  std::remove_cvref_t<Q>::reference * R{}};
}

template<typename Q, Reference R>
  requires Quantity<std::remove_cvref_t<Q>>
[[nodiscard]] constexpr Quantity auto operator/(Q&& q, R)
{
  return quantity{std::forward<Q>(q).numerical_value_is_an_implementation_detail_,
                  std::remove_cvref_t<Q>::reference / R{}};
}

template<Reference R, typename Q>
  requires Quantity<std::remove_cvref_t<Q>>
constexpr auto operator*(R, Q&& q) = delete;

template<Reference R, typename Q>
  requires Quantity<std::remove_cvref_t<Q>>
constexpr auto operator/(R, Q&& q) = delete;

[[nodiscard]] consteval AssociatedUnit auto common_reference(AssociatedUnit auto u1, AssociatedUnit auto u2,
                                                             AssociatedUnit auto... rest)
  requires requires {
    {
      common_quantity_spec(get_quantity_spec(u1), get_quantity_spec(u2), get_quantity_spec(rest)...)
    } -> QuantitySpec;
    {
      common_unit(u1, u2, rest...)
    } -> AssociatedUnit;
  }
{
  return common_unit(u1, u2, rest...);
}

template<Reference R1, Reference R2, Reference... Rest>
[[nodiscard]] consteval Reference auto common_reference(R1 r1, R2 r2, Rest... rest)
  requires(!(AssociatedUnit<R1> && AssociatedUnit<R2> && (... && AssociatedUnit<Rest>))) && requires {
    {
      common_quantity_spec(get_quantity_spec(r1), get_quantity_spec(r2), get_quantity_spec(rest)...)
    } -> QuantitySpec;
    {
      common_unit(get_unit(r1), get_unit(r2), get_unit(rest)...)
    } -> Unit;
  }
{
  return detail::reference_t<common_quantity_spec(get_quantity_spec(r1), get_quantity_spec(r2),
                                                  get_quantity_spec(rest)...),
                             common_unit(get_unit(r1), get_unit(r2), get_unit(rest)...)>{};
}

MP_UNITS_EXPORT_END

namespace detail {

template<AssociatedUnit auto To, AssociatedUnit From>
[[nodiscard]] consteval std::remove_const_t<decltype(To)> clone_reference_with(From)
{
  return {};
}

template<Unit auto To, QuantitySpec QS, Unit U>
[[nodiscard]] consteval reference<QS, std::remove_const_t<decltype(To)>> clone_reference_with(reference<QS, U>)
{
  return {};
}

}  // namespace detail

}  // namespace mp_units
