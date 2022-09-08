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

#include <units/dimension.h>
#include <units/unit.h>

namespace units {

// TODO Concept for system reference
template<auto R, typename Rep = double>
class quantity {
public:
  using reference = decltype(R);
  static constexpr auto dimension = reference::dimension;
  static constexpr auto unit = reference::unit;

  quantity(Rep) {}
};

template<typename Child, Dimension auto Dim, Unit auto CoU>
struct system_reference;

namespace detail {

template<typename Child, auto Dim, auto CoU>
void to_base_specialization_of_system_reference(const volatile system_reference<Child, Dim, CoU>*);

template<typename T>
// inline constexpr bool // TODO: Replace with concept when it works with MSVC
concept is_derived_from_specialization_of_system_reference =
  requires(T* t) { detail::to_base_specialization_of_system_reference(t); };

}  // namespace detail

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
template<typename T, Unit U>
struct reference;

template<typename R, Unit U>
  requires detail::is_derived_from_specialization_of_system_reference<R>
struct reference<R, U> {
  using system_reference = R;
  static constexpr auto dimension = R::dimension;
  static constexpr U unit{};
  // static constexpr UNITS_MSVC_WORKAROUND(Magnitude) auto mag = dimension::mag * unit::mag;
};

template<DerivedDimension D, Unit U>
struct reference<D, U> {
  static constexpr D dimension{};
  static constexpr U unit{};
  // static constexpr UNITS_MSVC_WORKAROUND(Magnitude) auto mag = dimension::mag * unit::mag;
};

// Reference
/**
 * @brief A concept matching all references in the library.
 *
 * Satisfied by all specializations of @c reference.
 */
template<typename T>
concept Reference = is_specialization_of<T, reference>;

template<Reference R1, Reference R2>
[[nodiscard]] constexpr reference<decltype(R1::dimension * R2::dimension), decltype(R1::unit * R2::unit)> operator*(R1,
                                                                                                                    R2)
{
  return {};
}

template<Reference R1, Reference R2>
[[nodiscard]] constexpr reference<decltype(R1::dimension / R2::dimension), decltype(R1::unit / R2::unit)> operator/(R1,
                                                                                                                    R2)
{
  return {};
}

// TODO Update when quantity is done
// template<Representation Rep>
// [[nodiscard]] friend constexpr Quantity auto operator*(const Rep& lhs, reference)
template<typename Rep, Reference R>
[[nodiscard]] constexpr quantity<R{}, Rep> operator*(const Rep& lhs, R)
{
  return quantity<R{}, Rep>(lhs);
}

// friend void /*Use `q * (1 * r)` rather than `q * r`.*/ operator*(Quantity auto, reference) = delete;

// TODO will use deducing this
template<typename Child, Dimension auto Dim, Unit auto CoU>
struct system_reference {
  static constexpr auto dimension = Dim;
  static constexpr auto coherent_unit = CoU;

  template<Unit U>
  // requires same_unit_reference<CoU, U>
  [[nodiscard]] constexpr reference<Child, U> operator[](U) const
  {
    return {};
  }
};

}  // namespace units
