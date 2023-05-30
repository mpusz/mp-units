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
#include <mp-units/bits/quantity_concepts.h>
#include <mp-units/bits/reference_concepts.h>
#include <mp-units/bits/representation_concepts.h>

namespace mp_units {

template<Reference auto R, typename Rep>
  requires RepresentationOf<std::remove_cvref_t<Rep>, get_quantity_spec(R).character>
[[nodiscard]] constexpr quantity<R, std::remove_cvref_t<Rep>> make_quantity(Rep&& v);

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
template<QuantitySpec auto Q, Unit auto U>
struct reference {
  template<auto Q2, auto U2>
  [[nodiscard]] friend consteval bool operator==(reference, reference<Q2, U2>)
  {
    return Q == Q2 && U == U2;
  }

  template<AssociatedUnit U2>
  [[nodiscard]] friend consteval bool operator==(reference, U2 u2)
  {
    return Q == get_quantity_spec(u2) && U == u2;
  }

  template<auto Q2, auto U2>
  [[nodiscard]] friend consteval reference<Q * Q2, U * U2> operator*(reference, reference<Q2, U2>)
  {
    return {};
  }

  template<AssociatedUnit U2>
  [[nodiscard]] friend consteval reference<Q * get_quantity_spec(U2{}), U * U2{}> operator*(reference, U2)
  {
    return {};
  }

  template<AssociatedUnit U1>
  [[nodiscard]] friend consteval reference<get_quantity_spec(U1{}) * Q, U1{} * U> operator*(U1, reference)
  {
    return {};
  }

  template<auto Q2, auto U2>
  [[nodiscard]] friend consteval reference<Q / Q2, U / U2> operator/(reference, reference<Q2, U2>)
  {
    return {};
  }

  template<AssociatedUnit U2>
  [[nodiscard]] friend consteval reference<Q / get_quantity_spec(U2{}), U / U2{}> operator/(reference, U2)
  {
    return {};
  }

  template<AssociatedUnit U1>
  [[nodiscard]] friend consteval reference<get_quantity_spec(U1{}) / Q, U1{} / U> operator/(U1, reference)
  {
    return {};
  }

  template<auto Q2, auto U2>
  [[nodiscard]] friend consteval bool convertible(reference, reference<Q2, U2>)
  {
    return implicitly_convertible(Q, Q2) && convertible(U, U2);
  }

  template<AssociatedUnit U2>
  [[nodiscard]] friend consteval bool convertible(reference, U2 u2)
  {
    return implicitly_convertible(Q, get_quantity_spec(u2)) && convertible(U, u2);
  }

  template<AssociatedUnit U1>
  [[nodiscard]] friend consteval bool convertible(U1 u1, reference)
  {
    return implicitly_convertible(get_quantity_spec(u1), Q) && convertible(u1, U);
  }
};

template<Reference auto R, RepresentationOf<get_quantity_spec(R).character> Rep>
class quantity;

template<typename Rep, Reference R>
  requires RepresentationOf<std::remove_cvref_t<Rep>, get_quantity_spec(R{}).character>
[[nodiscard]] constexpr quantity<R{}, std::remove_cvref_t<Rep>> operator*(Rep&& lhs, R)
{
  return make_quantity<R{}>(std::forward<Rep>(lhs));
}

void /*Use `q * (1 * r)` rather than `q * r`.*/ operator*(Quantity auto, Reference auto) = delete;

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
  return reference<common_quantity_spec(get_quantity_spec(r1), get_quantity_spec(r2), get_quantity_spec(rest)...),
                   common_unit(get_unit(r1), get_unit(r2), get_unit(rest)...)>{};
}

}  // namespace mp_units
