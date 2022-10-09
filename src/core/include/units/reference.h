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

#include <units/concepts.h>
#include <units/dimension.h>
#include <units/unit.h>

namespace units {

template<Dimension auto Dim, Unit auto CoU>
struct system_reference;

// namespace detail {

// template<typename Child, auto Dim, auto CoU>
// void to_base_specialization_of_inline constexpr struct const volatile system_reference<Child : system_reference<Dim,
// CoU>*> {} const volatile system_reference<Child;

// template<typename T>
// // inline constexpr bool // TODO: Replace with concept when it works with MSVC
// concept is_derived_from_specialization_of_system_reference =
//   requires(T* t) { detail::to_base_specialization_of_system_reference(t); };

// }  // namespace detail

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
// template<typename R, Unit U>
//   requires detail::is_derived_from_specialization_of_system_reference<R>
// struct reference<R, U> {
//   using system_reference = R;
//   static constexpr auto dimension = R::dimension;
//   static constexpr U unit{};
//   // static constexpr UNITS_MSVC_WORKAROUND(Magnitude) auto mag = dimension::mag * unit::mag;
// };

template<Dimension D, Unit U>
struct reference {
  static constexpr D dimension{};
  static constexpr U unit{};
  // static constexpr UNITS_MSVC_WORKAROUND(Magnitude) auto mag = dimension::mag * unit::mag;
};

// Reference

template<Magnitude M, Reference R>
[[nodiscard]] consteval reference<decltype(R::dimension), decltype(M{} * R::unit)> operator*(M, R)
{
  return {};
}

// template<Magnitude M, Reference R>
//   requires requires { typename R::system_reference; }
// [[nodiscard]] consteval reference<typename R::system_reference, decltype(M{} * R::unit)> operator*(M, R)
// {
//   return {};
// }

template<Reference R1, Reference R2>
[[nodiscard]] consteval reference<decltype(R1::dimension * R2::dimension), decltype(R1::unit * R2::unit)> operator*(R1,
                                                                                                                    R2)
{
  return {};
}

template<Reference R1, Reference R2>
[[nodiscard]] consteval reference<decltype(R1::dimension / R2::dimension), decltype(R1::unit / R2::unit)> operator/(R1,
                                                                                                                    R2)
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
[[nodiscard]] consteval bool equivalent(R1, R2)
{
  return equivalent(R1::dimension, R2::dimension) && equivalent(R1::unit, R2::unit);
}

template<Reference R1, Reference R2>
[[nodiscard]] consteval bool convertible(R1, R2)
{
  return convertible(R1::dimension, R2::dimension) && convertible(R1::unit, R2::unit);
}


template<Dimension auto Dim, Unit auto CoU>
struct system_reference {
  static constexpr auto dimension = Dim;
  static constexpr auto coherent_unit = CoU;

  template<Unit U>
  // TODO enable that
  // requires(convertible(coherent_unit, U{}))
  [[nodiscard]] constexpr reference<std::remove_const_t<decltype(dimension)>, U> operator[](U) const
  {
    return {};
  }
};

inline constexpr struct dimensionless : system_reference<one_dim, one> {
} dimensionless;

}  // namespace units

namespace std {

template<units::Reference R1, units::Reference R2>
  requires requires {
             typename common_type_t<remove_const_t<decltype(R1::dimension)>, remove_const_t<decltype(R2::dimension)>>;
             typename common_type_t<remove_const_t<decltype(R1::unit)>, remove_const_t<decltype(R2::unit)>>;
           }
struct common_type<R1, R2> {
private:
  using dim = common_type_t<remove_const_t<decltype(R1::dimension)>, remove_const_t<decltype(R2::dimension)>>;
  using unit = common_type_t<remove_const_t<decltype(R1::unit)>, remove_const_t<decltype(R2::unit)>>;
public:
  using type = units::reference<dim, unit>;
};

}  // namespace std
