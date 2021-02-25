// The MIT License (MIT)
//
// Copyright (c) 2020 Yves Delley
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

#include <units/one_rep.h>
#include <units/bits/basic_concepts.h>
#include <units/customization_points.h>
#include <type_traits>

namespace units {

struct invalid_zero_rep {};

/**
 * @brief A representation type to be used for unit constants
 *
 * This representation type is intended to be used in the unit constants definition:
 *
 * @code{.cpp}
 * namespace unit_constants {
 * 
 * inline constexpr auto zero_celsius_point = thermodynamic_temperature_point<degree_celsius, zero_rep, celsius_temperature_origin>{};
 *
 * }
 * @endcode
 * 
 * Unit constants simplify quantity point creation:
 * 
 * @code{.cpp}
 * using namespace units::physical::si::unit_constants;
 *
 * auto d = 25 * deg_C + zero_celsius_point;
 * @endcode
 *
 */
struct zero_rep {

  template<QuantityValue Rep>
  [[nodiscard]] friend constexpr Rep operator+(const Rep& lhs, zero_rep)
  {
    return lhs;
  }
  template<QuantityValue Rep>
  [[nodiscard]] friend constexpr Rep operator+(zero_rep, const Rep& rhs)
  {
    return rhs;
  }

  template<QuantityValue Rep>
  [[nodiscard]] friend constexpr Rep operator-(const Rep& lhs, zero_rep)
  {
    return lhs;
  }
  template<QuantityValue Rep>
  [[nodiscard]] friend constexpr Rep operator-(zero_rep, const Rep& rhs)
  {
    return -rhs;
  }

  [[nodiscard]] constexpr zero_rep operator-() const { return {}; }

  [[nodiscard]] friend constexpr zero_rep operator*(zero_rep, zero_rep) { return {}; }
  [[nodiscard]] friend constexpr invalid_zero_rep operator/(zero_rep, zero_rep) { return {}; }

  [[nodiscard]] friend constexpr zero_rep operator*(zero_rep, one_rep) { return {}; }
  [[nodiscard]] friend constexpr zero_rep operator*(one_rep, zero_rep) { return {}; }
  [[nodiscard]] friend constexpr zero_rep operator/(zero_rep, one_rep) { return {}; }
  [[nodiscard]] friend constexpr invalid_zero_rep operator/(one_rep, zero_rep) { return {}; }

  template<QuantityValue Rep>
  [[nodiscard]] friend constexpr zero_rep operator*(const Rep&, zero_rep)
  {
    return {};
  }
  template<QuantityValue Rep>
  [[nodiscard]] friend constexpr invalid_zero_rep operator/(const Rep&, zero_rep)
  {
    return {};
  }

  template<QuantityValue Rep>
  [[nodiscard]] friend constexpr zero_rep operator*(zero_rep, const Rep&)
  {
    return {};
  }
  template<QuantityValue Rep>
  [[nodiscard]] friend constexpr zero_rep operator/(zero_rep, const Rep&)
  {
    return {};
  }

  template<QuantityValue Rep>
  [[nodiscard]] constexpr operator Rep() const noexcept
  {
    return quantity_values<Rep>::zero();
  }

  [[nodiscard]] bool operator==(const zero_rep&) const = default;
  [[nodiscard]] auto operator<=>(const zero_rep&) const = default;

  [[nodiscard]] constexpr bool operator==(const invalid_zero_rep&) const { return false; }
};

template<>
struct quantity_values<zero_rep> {
  static constexpr zero_rep zero() noexcept { return zero_rep{}; }
  static constexpr invalid_zero_rep one() noexcept { return invalid_zero_rep{}; }
  static constexpr zero_rep min() noexcept { return zero(); }
  static constexpr zero_rep max() noexcept { return zero(); }
};

}  // namespace units

namespace std {

template<>
struct common_type<units::zero_rep, units::zero_rep> {
  using type = units::zero_rep;
};
template<units::QuantityValue Rep>
struct common_type<units::zero_rep, Rep> {
  using type = Rep;
};
template<units::QuantityValue Rep>
struct common_type<Rep, units::zero_rep> {
  using type = Rep;
};

}  // namespace std
