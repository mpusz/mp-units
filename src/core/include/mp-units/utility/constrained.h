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

#include <mp-units/bits/hacks.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/framework/customization_points.h>
#if MP_UNITS_HOSTED
#include <mp-units/bits/fmt.h>
#endif

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <compare>
#include <concepts>
#include <cstdlib>
#include <limits>
#include <string_view>
#include <type_traits>
#if MP_UNITS_HOSTED
#include <stdexcept>
#include <string>
#endif
#endif
#endif

namespace mp_units::utility {

// ============================================================================
// Error policy concept
// ============================================================================

MP_UNITS_EXPORT template<typename EP>
concept ConstraintPolicy = requires(std::string_view msg) { EP::on_constraint_violation(msg); };

// ============================================================================
// Base error policies
//
// These policies define how domain constraint violations are reported.
// safe_int.h extends them via inheritance to add on_overflow().
// ============================================================================

/**
 * @brief Error policy that terminates the program on constraint violation (freestanding-safe).
 */
MP_UNITS_EXPORT struct terminate_policy {
  [[noreturn]] static void on_constraint_violation(std::string_view) noexcept { std::abort(); }
};

#if MP_UNITS_HOSTED

/**
 * @brief Error policy that throws std::domain_error on constraint violation (hosted only).
 */
MP_UNITS_EXPORT struct throw_policy {
  [[noreturn]] static void on_constraint_violation(std::string_view msg) { throw std::domain_error(std::string(msg)); }
};

#endif  // MP_UNITS_HOSTED

MP_UNITS_EXPORT template<typename T, ConstraintPolicy ErrorPolicy>
class constrained;

// ============================================================================
// detail::constrained_ops — base class carrying all heterogeneous binary ops
//
// Placing the operators here (rather than in the class body) implements the
// "Hidden Friend Injection Idiom": the friend functions are function templates
// parameterized by two different constrained<> instantiations, so they
//   • are found only via ADL (hidden friends — not in the general candidate set),
//   • disable implicit conversions from plain scalars to constrained<>, and
//   • enable arithmetic between different instantiations, e.g.
//       constrained<int,EP> + constrained<long,EP>.
// ============================================================================

namespace detail {

struct constrained_binary_ops {
  // -- Binary arithmetic (+, -, *, /, %) --
  template<typename T, typename EP, typename U>
  [[nodiscard]] friend constexpr auto operator+(const constrained<T, EP>& lhs, const constrained<U, EP>& rhs)
    -> constrained<decltype(lhs.value_ + rhs.value_), EP>
  {
    return lhs.value_ + rhs.value_;
  }

  template<typename T, typename EP, typename U>
  [[nodiscard]] friend constexpr auto operator-(const constrained<T, EP>& lhs, const constrained<U, EP>& rhs)
    -> constrained<decltype(lhs.value_ - rhs.value_), EP>
  {
    return lhs.value_ - rhs.value_;
  }

  template<typename T, typename EP, typename U>
  [[nodiscard]] friend constexpr auto operator*(const constrained<T, EP>& lhs, const constrained<U, EP>& rhs)
    -> constrained<decltype(lhs.value_ * rhs.value_), EP>
  {
    return lhs.value_ * rhs.value_;
  }

  // Scalar multiplication: constrained<T,EP> * U and U * constrained<T,EP>
  template<typename T, typename EP, typename U>
  [[nodiscard]] friend constexpr auto operator*(const constrained<T, EP>& lhs, const U& rhs)
    -> constrained<decltype(lhs.value_ * rhs), EP>
  {
    return lhs.value_ * rhs;
  }

  template<typename T, typename EP, typename U>
  [[nodiscard]] friend constexpr auto operator*(const U& lhs, const constrained<T, EP>& rhs)
    -> constrained<decltype(lhs * rhs.value_), EP>
  {
    return lhs * rhs.value_;
  }

  template<typename T, typename EP, typename U>
  [[nodiscard]] friend constexpr auto operator/(const constrained<T, EP>& lhs, const constrained<U, EP>& rhs)
    -> constrained<decltype(lhs.value_ / rhs.value_), EP>
  {
    return lhs.value_ / rhs.value_;
  }

  template<typename T, typename EP, typename U>
  [[nodiscard]] friend constexpr auto operator/(const constrained<T, EP>& lhs, const U& rhs)
    -> constrained<decltype(lhs.value_ / rhs), EP>
  {
    return lhs.value_ / rhs;
  }

  template<typename T, typename EP, typename U>
  [[nodiscard]] friend constexpr auto operator/(const U& lhs, const constrained<T, EP>& rhs)
    -> constrained<decltype(lhs / rhs.value_), EP>
  {
    return lhs / rhs.value_;
  }

  template<typename T, typename EP, typename U>
  [[nodiscard]] friend constexpr auto operator%(const constrained<T, EP>& lhs, const constrained<U, EP>& rhs)
    -> constrained<decltype(lhs.value_ % rhs.value_), EP>
  {
    return lhs.value_ % rhs.value_;
  }

  // -- Comparison --
  template<typename T, typename EP, typename U>
    requires std::equality_comparable_with<T, U>
  [[nodiscard]] friend constexpr bool operator==(const constrained<T, EP>& lhs, const constrained<U, EP>& rhs)
  {
    return lhs.value_ == rhs.value_;
  }

  template<typename T, typename EP, typename U>
    requires std::three_way_comparable_with<T, U>
  [[nodiscard]] friend constexpr auto operator<=>(const constrained<T, EP>& lhs, const constrained<U, EP>& rhs)
  {
    return lhs.value_ <=> rhs.value_;
  }
};

}  // namespace detail

// ============================================================================
// constrained<T, ErrorPolicy>
//
// A transparent wrapper that carries an error policy for domain constraint
// enforcement. The wrapper itself does not perform any checks — it serves
// as a tag that the library detects via constraint_violation_handler to
// dispatch constraint violations to the policy.
//
// All arithmetic is forwarded to the underlying type T.
// ============================================================================

/**
 * @brief A transparent wrapper that carries an error policy for domain constraint enforcement.
 *
 * This wrapper does not check anything itself. It serves as a signal to the mp-units
 * library that domain constraint violations should be reported via the ErrorPolicy
 * rather than through precondition checks.
 *
 * All arithmetic operations are forwarded to the underlying type T.
 *
 * @tparam T            the underlying representation type
 * @tparam ErrorPolicy  policy type satisfying ConstraintPolicy (provides static
 * on_constraint_violation(std::string_view))
 */
MP_UNITS_EXPORT template<typename T,
#if MP_UNITS_HOSTED
                         ConstraintPolicy ErrorPolicy = throw_policy>
#else
                         ConstraintPolicy ErrorPolicy = terminate_policy>
#endif
class constrained : detail::constrained_binary_ops {
public:
  // public members required to satisfy structural type requirements :-(
  T value_{};
  using value_type = T;
  using error_policy = ErrorPolicy;

  constrained() = default;
  constexpr explicit(false) constrained(T v) noexcept : value_(v) {}

  [[nodiscard]] constexpr explicit(false) operator T() const noexcept { return value_; }

  [[nodiscard]] constexpr T value() const noexcept { return value_; }

  // -- Unary arithmetic --
  [[nodiscard]] friend constexpr auto operator+(const constrained& x)
    -> constrained<decltype(+std::declval<T>()), ErrorPolicy>
  {
    return +x.value_;
  }

  MP_UNITS_DIAGNOSTIC_PUSH
  // Generic operator- is intentionally instantiated with unsigned T as well (where unary minus
  // returns the same unsigned type by C++ rules; MSVC C4146 flags this even though it is the
  // documented behavior).
  MP_UNITS_DIAGNOSTIC_IGNORE_UNARY_MINUS_UNSIGNED
  [[nodiscard]] friend constexpr auto operator-(const constrained& x)
    -> constrained<decltype(-std::declval<T>()), ErrorPolicy>
  {
    return -x.value_;
  }
  MP_UNITS_DIAGNOSTIC_POP

  // -- Increment / decrement --
  constexpr constrained& operator++()
    requires requires(T& v) {
      { ++v } -> std::same_as<T&>;
    }
  {
    ++value_;
    return *this;
  }

  constexpr auto operator++(int) -> constrained<decltype(std::declval<T&>()++), ErrorPolicy> { return value_++; }

  constexpr constrained& operator--()
    requires requires(T& v) {
      { --v } -> std::same_as<T&>;
    }
  {
    --value_;
    return *this;
  }

  constexpr auto operator--(int) -> constrained<decltype(std::declval<T&>()--), ErrorPolicy> { return value_--; }

  // -- Compound assignment --
  constexpr constrained& operator+=(const constrained& rhs)
    requires requires(T& a, const T b) {
      { a += b } -> std::same_as<T&>;
    }
  {
    value_ += rhs.value_;
    return *this;
  }

  constexpr constrained& operator-=(const constrained& rhs)
    requires requires(T& a, const T b) {
      { a -= b } -> std::same_as<T&>;
    }
  {
    value_ -= rhs.value_;
    return *this;
  }

  constexpr constrained& operator*=(const constrained& rhs)
    requires requires(T& a, const T b) {
      { a *= b } -> std::same_as<T&>;
    }
  {
    value_ *= rhs.value_;
    return *this;
  }

  constexpr constrained& operator/=(const constrained& rhs)
    requires requires(T& a, const T b) {
      { a /= b } -> std::same_as<T&>;
    }
  {
    value_ /= rhs.value_;
    return *this;
  }

  constexpr constrained& operator%=(const constrained& rhs)
    requires requires(T& a, const T b) {
      { a %= b } -> std::same_as<T&>;
    }
  {
    value_ %= rhs.value_;
    return *this;
  }
};

}  // namespace mp_units::utility

namespace mp_units {

template<typename T, typename ErrorPolicy>
struct constraint_violation_handler<utility::constrained<T, ErrorPolicy>> {
  static constexpr void on_violation(std::string_view msg) { ErrorPolicy::on_constraint_violation(msg); }
};

}  // namespace mp_units

// std::numeric_limits specialization
namespace std {

template<typename T, typename ErrorPolicy>
class numeric_limits<mp_units::utility::constrained<T, ErrorPolicy>> : public numeric_limits<T> {
  using C = mp_units::utility::constrained<T, ErrorPolicy>;

public:
  // Always meaningful — override unconditionally.
  [[nodiscard]] static constexpr C lowest() noexcept { return numeric_limits<T>::lowest(); }
  [[nodiscard]] static constexpr C min() noexcept { return numeric_limits<T>::min(); }
  [[nodiscard]] static constexpr C max() noexcept { return numeric_limits<T>::max(); }

  // Only override when the underlying type actually provides meaningful values;
  // otherwise the base returns 0 / T{} and there is nothing useful to wrap.
  [[nodiscard]] static constexpr C epsilon() noexcept
    requires(!numeric_limits<T>::is_integer)
  {
    return numeric_limits<T>::epsilon();
  }
  [[nodiscard]] static constexpr C round_error() noexcept
    requires(!numeric_limits<T>::is_integer)
  {
    return numeric_limits<T>::round_error();
  }
  [[nodiscard]] static constexpr C infinity() noexcept
    requires(numeric_limits<T>::has_infinity)
  {
    return numeric_limits<T>::infinity();
  }
  [[nodiscard]] static constexpr C quiet_NaN() noexcept
    requires(numeric_limits<T>::has_quiet_NaN)
  {
    return numeric_limits<T>::quiet_NaN();
  }
  [[nodiscard]] static constexpr C signaling_NaN() noexcept
    requires(numeric_limits<T>::has_signaling_NaN)
  {
    return numeric_limits<T>::signaling_NaN();
  }
  [[nodiscard]] static constexpr C denorm_min() noexcept
    requires(numeric_limits<T>::has_denorm_loss)
  {
    return numeric_limits<T>::denorm_min();
  }
};

}  // namespace std

#if MP_UNITS_HOSTED
template<typename T, typename ErrorPolicy, typename Char>
struct MP_UNITS_STD_FMT::formatter<mp_units::utility::constrained<T, ErrorPolicy>, Char> : formatter<T, Char> {
  template<typename FormatContext>
  auto format(const mp_units::utility::constrained<T, ErrorPolicy>& v, FormatContext& ctx) const
  {
    return formatter<T, Char>::format(v.value(), ctx);
  }
};
#endif
