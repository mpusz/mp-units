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
#include <mp-units/bits/module_macros.h>
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/quantity_concepts.h>
#include <mp-units/framework/representation_concepts.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <compare>
#include <concepts>
#endif  // MP_UNITS_IMPORT_STD
#endif  // MP_UNITS_IN_MODULE_INTERFACE

namespace mp_units {

MP_UNITS_EXPORT_BEGIN

/**
 * The type of @ref zero
 *
 * All the comparisons of @ref zero are its hidden friends, so argument-dependent lookup finds them
 * through the tag, and they take no part in a comparison that doesn't involve it.
 */
struct zero_t {
  explicit zero_t() = default;

  [[nodiscard]] friend constexpr bool operator==(zero_t, zero_t) = default;
  [[nodiscard]] friend constexpr std::strong_ordering operator<=>(zero_t, zero_t) = default;

  // quantity (a quantity point has no overload: its value depends on the origin). A quantity of a
  // vector or a tensor has a zero only if its representation type specializes
  // `representation_values`, and it gets `==` but not `<=>`, because only a real scalar is ordered.
  template<auto R, typename Rep>
    requires std::equality_comparable<Rep> && requires { representation_values<Rep>::zero(); }
  [[nodiscard]] friend constexpr bool operator==(const quantity<R, Rep>& lhs, zero_t)
  {
    return lhs.numerical_value_ref_in(get_unit(R)) == representation_values<Rep>::zero();
  }

  template<auto R, typename Rep>
    requires detail::ScalarRepresentation<Rep> && std::three_way_comparable<Rep> &&
             requires { representation_values<Rep>::zero(); }
  [[nodiscard]] friend constexpr auto operator<=>(const quantity<R, Rep>& lhs, zero_t)
  {
    return lhs.numerical_value_ref_in(get_unit(R)) <=> representation_values<Rep>::zero();
  }

  // quantity-like types, such as `std::chrono::duration` once `mp-units/systems/si/chrono.h` is included
  // `T` is deduced as `zero_t` in a reversed candidate, which isn't quantity-like, and checking it
  // first keeps GCC from reporting the incomplete `quantity_like_traits<zero_t>` in a diagnostic.
  template<typename T>
    requires(!std::same_as<T, zero_t>) && detail::QuantityLike<T> &&
            std::equality_comparable<typename quantity_like_traits<T>::rep> &&
            requires { representation_values<typename quantity_like_traits<T>::rep>::zero(); }
  [[nodiscard]] friend constexpr bool operator==(const T& lhs, zero_t)
  {
    using rep = quantity_like_traits<T>::rep;
    return quantity_like_traits<T>::to_numerical_value(lhs) == representation_values<rep>::zero();
  }

  template<typename T>
    requires(!std::same_as<T, zero_t>) && detail::QuantityLike<T> &&
            detail::ScalarRepresentation<typename quantity_like_traits<T>::rep> &&
            std::three_way_comparable<typename quantity_like_traits<T>::rep> &&
            requires { representation_values<typename quantity_like_traits<T>::rep>::zero(); }
  [[nodiscard]] friend constexpr auto operator<=>(const T& lhs, zero_t)
  {
    using rep = quantity_like_traits<T>::rep;
    return quantity_like_traits<T>::to_numerical_value(lhs) <=> representation_values<rep>::zero();
  }

  // The conversions are defined in `zero_t`, not in `quantity`, to keep everything about zero in
  // one place, and because a quantity-like type can't be given a constructor. They make `zero_t`
  // have a common reference with a quantity and with a quantity-like type that a quantity
  // converts to implicitly, which `std::ranges::find` and `std::ranges::less` require. There is no conversion to a
  // representation type: `zero` would then reach a function that takes a plain number.
  template<auto R, typename Rep>
    requires requires { representation_values<Rep>::zero(); }
  [[nodiscard]] constexpr explicit(false) operator quantity<R, Rep>() const
  {
    return quantity{representation_values<Rep>::zero(), R};
  }

  template<detail::QuantityLike T>
    requires requires { representation_values<typename quantity_like_traits<T>::rep>::zero(); }
  [[nodiscard]] constexpr explicit(quantity_like_traits<T>::explicit_export) operator T() const
  {
    return quantity_like_traits<T>::from_numerical_value(
      representation_values<typename quantity_like_traits<T>::rep>::zero());
  }
};

/**
 * Zero of any quantity and of any quantity-like type
 *
 * For a quantity that isn't a point, zero is the only value whose meaning doesn't depend on the
 * unit. A comparison against it needs no unit, and writing `zero` instead of a bare `0` shows that
 * the unit is omitted on purpose:
 *
 * @code{.cpp}
 * quantity<si::metre> total = zero;
 * for (quantity leg : {3. * km, 0.5 * km}) total += leg;
 *
 * if (total > zero) std::println("travelled {}", total);
 * @endcode
 *
 * The same comparison works, once `mp-units/systems/si/chrono.h` is included, for a
 * `std::chrono::duration`, so generic code doesn't have to tell them apart:
 *
 * @code{.cpp}
 * template<typename T>
 * bool is_positive(const T& value) { return value > zero; }
 * @endcode
 *
 * The value of `zero` is part of its type, so it survives being passed through a function:
 * `std::less<>{}(q, zero)` and `std::ranges::find(v, zero)` work for quantities and for
 * `std::chrono::duration`. `zero` initializes both. It doesn't compare against or convert to a
 * plain number such as `double`, so it never stands in for one. A quantity point can't be
 * compared against `zero`, because its numerical value depends on the origin. Compare its delta
 * from a named origin instead.
 *
 * @see [Comparison against zero](docs:users_guide/framework_basics/quantity_arithmetics#comparison-against-zero)
 */
inline constexpr zero_t zero{};

MP_UNITS_EXPORT_END

}  // namespace mp_units
