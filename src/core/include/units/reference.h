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

#include <units/bits/basic_concepts.h>
#include <units/bits/dimension_op.h>

namespace units {

template<Dimension D, UnitOf<D> U, Representation Rep>
class quantity;

template<Dimension D, UnitOf<D> U>
struct reference;

namespace detail {

template<Dimension D, Reference R1, Reference R2>
using reference_multiply_impl = reference<D, downcast_unit<D, R1::mag * R2::mag / D::mag>>;

template<typename D, Reference R1, Reference R2>
using reference_divide_impl = reference<D, downcast_unit<D, R1::mag / R2::mag / D::mag>>;

}  // namespace detail

template<Reference R1, Reference R2>
using reference_multiply =
  detail::reference_multiply_impl<dimension_multiply<typename R1::dimension, typename R2::dimension>, R1, R2>;

template<Reference R1, Reference R2>
using reference_divide =
  detail::reference_divide_impl<dimension_divide<typename R1::dimension, typename R2::dimension>, R1, R2>;

/**
 * @brief The type for quantity references
 *
 * This type is intended to be used in the quantity references definition:
 *
 * @code{.cpp}
 * namespace length_references {
 *
 * inline constexpr auto m = reference<dim_length, metre>{};
 * inline constexpr auto km = reference<dim_length, kilometre>{};
 *
 * }
 *
 * namespace references {
 *
 * using namespace length_references;
 *
 * }
 * @endcode
 *
 * Quantity references simplify quantity creation:
 *
 * @code{.cpp}
 * using namespace units::isq::si::references;
 *
 * auto d = 123 * m;
 * auto v = 70 * (km / h);
 * @endcode
 *
 * Also, it is allowed to define custom quantity references from existing ones:
 *
 * @code{.cpp}
 * constexpr auto Nm = N * m;
 * constexpr auto mph = mi / h;
 * @endcode
 *
 * The following syntaxes are not allowed:
 * `2 / s`, `km * 3`, `s / 4`, `70 * km / h`.
 */
template<Dimension D, UnitOf<D> U>
struct reference {
  using dimension = D;
  using unit = U;
  static constexpr UNITS_CONSTRAINED_AUTO_WORKAROUND(Magnitude) auto mag = dimension::mag * unit::mag;

  // Hidden Friends
  // Below friend functions are to be found via argument-dependent lookup only

  template<Reference R2>
  [[nodiscard]] friend constexpr reference_multiply<reference, R2> operator*(reference, R2)
  {
    return {};
  }

  template<Reference R2>
  [[nodiscard]] friend constexpr reference_divide<reference, R2> operator/(reference, R2)
  {
    return {};
  }

  template<Representation Rep>
  [[nodiscard]] friend constexpr Quantity auto operator*(const Rep& lhs, reference)
  {
    return quantity<D, U, Rep>(lhs);
  }

  friend void /*Use `q * (1 * r)` rather than `q * r`.*/ operator*(Quantity auto, reference) = delete;

  template<Reference R2>
  [[nodiscard]] friend constexpr bool operator==(reference, R2)
  {
    return false;
  }

  [[nodiscard]] friend constexpr bool operator==(reference, reference) { return true; }
};

// type traits
namespace detail {

template<typename D, typename U>
inline constexpr bool is_reference<reference<D, U>> = true;

}  // namespace detail

}  // namespace units
