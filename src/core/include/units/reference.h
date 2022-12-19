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

#include <units/bits/quantity_concepts.h>
#include <units/quantity_spec.h>
#include <units/unit.h>

namespace units {

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
 * quantity_of<isq::speed[km / h]> auto speed = 90 * kmph;
 * @endcode
 *
 * The following syntaxes are not allowed:
 * `2 / kmph`, `kmph * 3`, `kmph / 4`, `70 * isq::length[km] / isq:time[h]`.
 */
template<QuantitySpec auto Q, Unit auto U>
struct reference {
  static constexpr QuantitySpec auto quantity_spec = Q;
  static constexpr Dimension auto dimension = Q.dimension;
  static constexpr Unit auto unit = U;
};

// Reference

template<Magnitude M, Reference R>
[[nodiscard]] consteval reference<R::quantity_spec, M{} * R::unit> operator*(M, R)
{
  return {};
}

template<Reference R1, Reference R2>
[[nodiscard]] consteval reference<R1::quantity_spec * R2::quantity_spec, R1::unit * R2::unit> operator*(R1, R2)
{
  return {};
}

template<Reference R1, Reference R2>
[[nodiscard]] consteval reference<R1::quantity_spec / R2::quantity_spec, R1::unit / R2::unit> operator/(R1, R2)
{
  return {};
}

template<Representation Rep, Reference R>
[[nodiscard]] constexpr quantity<R{}, Rep> operator*(const Rep& lhs, R)
{
  return quantity<R{}, Rep>(lhs);
}

void /*Use `q * (1 * r)` rather than `q * r`.*/ operator*(Quantity auto, Reference auto) = delete;

template<Reference R1, Reference R2>
[[nodiscard]] consteval bool operator==(R1, R2)
{
  return R1::quantity_spec == R2::quantity_spec && R1::unit == R2::unit;
}

template<Reference R1, Reference R2>
[[nodiscard]] consteval bool interconvertible(R1, R2)
{
  return interconvertible(R1::quantity_spec, R2::quantity_spec) && interconvertible(R1::unit, R2::unit);
}

[[nodiscard]] consteval auto common_reference(Reference auto r1, Reference auto r2, Reference auto... rest)
  requires requires {
    {
      common_quantity_spec(r1.quantity_spec, r2.quantity_spec, rest.quantity_spec...)
    } -> QuantitySpec;
    {
      common_unit(r1.unit, r2.unit, rest.unit...)
    } -> Unit;
  }
{
  return reference<common_quantity_spec(r1.quantity_spec, r2.quantity_spec, rest.quantity_spec...),
                   common_unit(r1.unit, r2.unit, rest.unit...)>{};
}

}  // namespace units
