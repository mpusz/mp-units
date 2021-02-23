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
#include <units/customization_points.h>
#include <type_traits>

namespace units {

struct invalid_one_rep {};

/**
 * @brief A representation type to be used for unit constants
 *
 * This representation type is intended to be used in the unit constants definition:
 *
 * @code{.cpp}
 * namespace unit_constants {
 * 
 * inline constexpr auto m = length<metre, one_rep>{};
 * inline constexpr auto km = length<kilometre, one_rep>{};
 * 
 * }
 * @endcode
 * 
 * Unit constants simplify quantity creation:
 * 
 * @code{.cpp}
 * using namespace units::physical::si::unit_constants;
 *
 * auto d = 123 * m;
 * auto v = 70 * km / h;
 * @endcode
 *
 * Also, it is allowed to define custom unit constants from existing ones:
 * 
 * @code{.cpp}
 * constexpr auto Nm = N * m;
 * constexpr auto mph = mi / h;
 * @endcode
 * 
 * `km * 3` or `s / 4` syntax is not allowed for quantity creation.
 */
struct one_rep {
  [[nodiscard]] friend constexpr one_rep operator*(one_rep, one_rep) { return {}; }
  [[nodiscard]] friend constexpr one_rep operator/(one_rep, one_rep) { return {}; }

  template<QuantityValue Rep>
  [[nodiscard]] friend constexpr Rep operator*(const Rep& lhs, one_rep)
  {
    return lhs;
  }
  template<QuantityValue Rep>
  [[nodiscard]] friend constexpr Rep operator/(const Rep& lhs, one_rep)
  {
    return lhs;
  }

  template<QuantityValue Rep>
  [[nodiscard]] friend constexpr Rep operator*(one_rep, const Rep&) = delete;
  template<QuantityValue Rep>
  [[nodiscard]] friend constexpr Rep operator/(one_rep, const Rep&) = delete;

  template<typename Rep>
    requires requires { quantity_values<Rep>::one(); } &&
          (!Quantity<Rep> && !QuantityLike<Rep> && !wrapped_quantity_<Rep>)  // TODO: Replace with `QuantityValue`
  [[nodiscard]] constexpr operator Rep() const noexcept                      // when Clang catches up.
  {
    return quantity_values<Rep>::one();
  }

  [[nodiscard]] bool operator==(const one_rep&) const = default;
  [[nodiscard]] auto operator<=>(const one_rep&) const = default;

  [[nodiscard]] constexpr bool operator==(const invalid_one_rep&) const { return false; }
};

template<>
struct quantity_values<one_rep> {
  static constexpr invalid_one_rep zero() noexcept { return invalid_one_rep{}; }
  static constexpr one_rep one() noexcept { return one_rep{}; }
  static constexpr one_rep min() noexcept { return one(); }
  static constexpr one_rep max() noexcept { return one(); }
};

}  // namespace units

namespace std {

template<>
struct common_type<units::one_rep, units::one_rep> {
  using type = units::one_rep;
};
template<units::QuantityValue Rep>
struct common_type<units::one_rep, Rep> {
  using type = Rep;
};
template<units::QuantityValue Rep>
struct common_type<Rep, units::one_rep> {
  using type = Rep;
};

}  // namespace std
