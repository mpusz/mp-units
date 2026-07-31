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
#include <mp-units/bits/fixed_point.h>
#include <mp-units/bits/int_power.h>
#include <mp-units/framework/representation_concepts.h>
#include <mp-units/framework/rounding.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/ext/contracts.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <concepts>
#include <cstdint>
#include <limits>
#include <type_traits>
#endif
#endif


namespace mp_units {

namespace detail {

/**
 * @brief Intentionally silent narrowing cast with compiler float-conversion diagnostics suppressed.
 *
 * @tparam To    target type
 * @tparam From  source type (deduced)
 */
MP_UNITS_EXPORT template<typename To, typename From>
[[nodiscard]] constexpr To silent_cast(From value) noexcept
{
  MP_UNITS_DIAGNOSTIC_PUSH
  MP_UNITS_DIAGNOSTIC_IGNORE_CONVERSION
  MP_UNITS_DIAGNOSTIC_IGNORE_FLOAT_CONVERSION
  MP_UNITS_DIAGNOSTIC_IGNORE_INT_CONVERSION
  MP_UNITS_DIAGNOSTIC_IGNORE_SIGN_CONVERSION
  MP_UNITS_DIAGNOSTIC_IGNORE_LOSS_OF_DATA
  return static_cast<To>(value);
  MP_UNITS_DIAGNOSTIC_POP
}

template<typename T>
inline constexpr bool treat_as_integral = !treat_as_floating_point<T>;

// A zero and a one of the same type as the value passed in, derived from that value instead of from
// the `representation_values` customization point. The types the scaling arithmetic runs in are
// widened or otherwise derived from the operands, so they are never the representation type a caller
// named and no constraint could speak about them. Subtracting a value from itself is exactly zero and
// dividing it by itself is exactly one, so these ask nothing of a type beyond the arithmetic
// `RealScalar` already mandates.
template<typename T>
[[nodiscard]] constexpr T get_zero(const T& value)
{
  return value - value;
}

template<typename T>
[[nodiscard]] constexpr T get_one(const T& value)
{
  MP_UNITS_PRECONDITION(value != get_zero(value));
  return value / value;
}

/**
 * @brief Integer division with the quotient rounded according to @c Mode
 */
template<rounding_mode Mode, typename T, typename D>
[[nodiscard]] constexpr auto div_round(const T& dividend, const D& divisor)
{
  MP_UNITS_PRECONDITION(divisor > get_zero(divisor));  // unit magnitude denominators are always positive
  using quot_type = std::remove_const_t<decltype(dividend / divisor)>;
  const quot_type quot = dividend / divisor;
  if constexpr (Mode == rounding_mode::truncated)
    return quot;
  else {
    // named `step` rather than `one`, which would shadow the `mp_units::one` unit (MSVC C4459)
    const quot_type step = get_one(divisor);
    const quot_type two_steps = step + step;
    // `%` lets the compiler share work with the division it has already emitted, which measures as
    // a couple of instructions fewer on some compilers (GCC produces the same code either way).
    // `%` is optional for a `RealScalar` though, so the subtraction stays as the general form: it
    // needs only the arithmetic the concept mandates, which is what the emulated 128-bit integer
    // and any other representation type without `%` rely on.
    const auto rem = [&] {
      if constexpr (requires { dividend % divisor; })
        return dividend % divisor;
      else
        return dividend - quot * divisor;
    }();
    using rem_type = std::remove_const_t<decltype(rem)>;
    const rem_type no_rem = get_zero(rem);
    if constexpr (Mode == rounding_mode::rounded_down)
      return rem < no_rem ? quot - step : quot;
    else if constexpr (Mode == rounding_mode::rounded_up)
      return rem > no_rem ? quot + step : quot;
    else {  // rounding_mode::rounded (to nearest, ties to even)
      const auto abs_rem = rem < no_rem ? rem_type{-rem} : rem;
      // `abs_rem > divisor / 2` rephrased as `abs_rem > divisor - abs_rem` to stay exact for
      // odd divisors, and with subtraction instead of `2 * abs_rem` to avoid overflow
      const auto complement = divisor - abs_rem;
      const bool is_odd = quot - (quot / two_steps) * two_steps != get_zero(quot);
      if (abs_rem > complement || (abs_rem == complement && is_odd)) return rem < no_rem ? quot - step : quot + step;
      return quot;
    }
  }
}

/**
 * @brief Rounds a floating-point value to an integral value according to @c Mode
 *
 * Hand-rolled rather than delegating to `std::trunc`/`std::floor`/`std::ceil`, which lower to
 * a single hardware instruction but are not usable in constant expressions before C++23 and are not
 * guaranteed to exist at all in a freestanding build. `std::nearbyint` would not do for the
 * round-half-to-even mode either: it honors the dynamic rounding mode, while the policy is specified
 * as ties-to-even.
 */
template<rounding_mode Mode, std::floating_point T>
[[nodiscard]] constexpr T round_fp(T value)
{
  // all values with a magnitude at or above this threshold are integral (their ULP is >= 1),
  // and every value below it fits into `std::int64_t` (the threshold never exceeds 2^63)
  constexpr T integral_threshold = int_power<T>(2, std::numeric_limits<T>::digits - 1);
  if (!(value < integral_threshold && value > -integral_threshold)) return value;
  // `whole` and `lower` rather than `trunc` and `floor`, which would shadow the `mp_units::floor`
  // and `std::trunc`/`std::floor` function templates (MSVC C4459)
  const T whole = static_cast<T>(static_cast<std::int64_t>(value));
  if constexpr (Mode == rounding_mode::truncated)
    return whole;
  else if constexpr (Mode == rounding_mode::rounded_down)
    return whole > value ? whole - T{1} : whole;
  else if constexpr (Mode == rounding_mode::rounded_up)
    return whole < value ? whole + T{1} : whole;
  else {  // rounding_mode::rounded (to nearest, ties to even)
    const T lower = whole > value ? whole - T{1} : whole;
    const T frac = value - lower;
    if (frac > T{0.5}) return lower + T{1};
    if (frac < T{0.5}) return lower;
    const bool is_odd = static_cast<T>(static_cast<std::int64_t>(lower / T{2})) * T{2} != lower;
    return is_odd ? lower + T{1} : lower;
  }
}

template<auto M, typename T>
[[nodiscard]] constexpr auto scale_fp(const T& v)
{
  using element_t = value_type_t<T>;
  static_assert(treat_as_floating_point<element_t>);
  if constexpr (is_integral(pow<-1>(M)) && !is_integral(M) && std::floating_point<element_t>) {
    // M has an integral inverse (pure divisor).  For built-in floating-point element
    // types prefer division over multiplication to avoid the rounding errors introduced
    // by 1/x in floating-point.
    //
    // This optimization is restricted to std::floating_point because user-defined
    // floating-point-like representation types (treat_as_floating_point but not
    // std::floating_point, e.g. cnl::scaled_integer used as a fixed-point real) may
    // have a representable range much smaller than the divisor itself (e.g. converting
    // 100 into scaled_integer<int, power<-25>> would shift it left by 25 bits and
    // overflow int, breaking constexpr evaluation).  For such types the multiplier
    // path below is safer: the reciprocal is a small fraction that fits comfortably
    // in the wrapper's representable range.
    constexpr element_t div = detail::silent_cast<element_t>(get_value<long double>(pow<-1>(M)));
    return v / div;
  } else {
    constexpr element_t ratio = detail::silent_cast<element_t>(get_value<long double>(M));
    return v * ratio;
  }
}

template<typename T>
constexpr decltype(auto) as_element(const T& value)
{
  if constexpr (std::convertible_to<T, value_type_t<T>>)
    return static_cast<value_type_t<T>>(value);
  else
    return value;
}

template<auto M, rounding_mode Mode, typename T>
[[nodiscard]] constexpr auto scale_int(const T& v)
{
  using element_t = value_type_t<T>;
  using wider_t = wider_int_for<element_t>;
  static_assert(treat_as_integral<element_t>);
  if constexpr (is_integral(M)) {
    constexpr wider_t mul = get_value<wider_t>(M);
    return v * mul;
  } else if constexpr (is_integral(pow<-1>(M))) {
    constexpr wider_t div = get_value<wider_t>(pow<-1>(M));
    return div_round<Mode>(v, div);
  } else if constexpr (is_integral(M * (denominator(M) / numerator(M)))) {
    // M is a pure rational p/q (no irrational factors such as π).
    // Use wider_t for the numerator to prevent intermediate overflow in v * num.
    // The wider type propagates through the type's own operators:
    // plain int widens via C++ promotion rules, safe_int widens via its
    // checked operator* template, cartesian_vector widens element-wise, etc.
    constexpr wider_t num = get_value<wider_t>(numerator(M));
    constexpr element_t den = get_value<element_t>(denominator(M));
    return div_round<Mode>(v * num, den);
  } else {
    // M has irrational factors (e.g. π): use long double fixed-point approximation.
    // fixed_point::scale operates on plain integers, so extract the scalar element.
    static_assert(std::convertible_to<T, element_t>,
                  "Scaling an integral-element wrapping type by an irrational magnitude factor "
                  "is not supported; use a floating-point element type instead");
    constexpr auto ratio = fixed_point<element_t>(get_value<long double>(M));
    return ratio.template scale<Mode>(as_element(v));
  }
}

template<typename To, rounding_mode Mode, UnitMagnitude M, typename From>
  requires UnitMagnitudeScalable<From>
[[nodiscard]] constexpr auto scale_impl(M m, const From& value)
{
  if constexpr (requires { value * m; }) {
    // Type provides magnitude-aware scaling via operator*(T, UnitMagnitude).
    // The customization point has no way to communicate a rounding mode, so only the
    // status-quo truncating semantics may reach it (guaranteed by the public API constraints).
    static_assert(Mode == rounding_mode::truncated,
                  "Rounding policies are not supported for representation types providing "
                  "a custom operator*(T, UnitMagnitude) scaling");
    return value * m;
  } else if constexpr (UsesFloatingPointScaling<From> || UsesFloatingPointScaling<To>) {
    // Floating-point path — handles both plain arithmetic types and wrappers.
    // Uses the type's own operator* / operator/ (element-wise for wrappers).
    // Convert to To first when:
    //  - From is integral (e.g. int → double): scaling must happen in FP space; or
    //  - the FP element type is not std::floating_point (e.g. a fixed-point wrapper
    //    like cnl::scaled_integer used as a real number): the wrapper's own
    //    operator* may produce a different, more-precise type whose conversion to
    //    To could overflow the wrapper's narrow rep (e.g. scaled_integer<int,P<-25>>
    //    times itself yields P<-50>, which cannot be downshifted to P<-15> within
    //    a 32-bit int).  Converting to To first keeps all arithmetic in To's type.
    const auto scaled = [&] {
      if constexpr (treat_as_integral<value_type_t<From>> || !std::floating_point<value_type_t<From>>)
        return scale_fp<M{}>(static_cast<To>(value));
      else
        return scale_fp<M{}>(value);
    }();
    // A floating-point destination represents the scaled value with at most half ULP error
    // (`rounded` semantics, the only policy valid for it), so rounding applies only when the
    // destination is integral and a mode other than the `silent_cast` truncation is requested.
    if constexpr (Mode == rounding_mode::truncated || !treat_as_integral<value_type_t<To>>)
      return silent_cast<To>(scaled);
    else
      return silent_cast<To>(round_fp<Mode>(scaled));
  } else {
    // Integer arithmetic path (UsesIntegerScaling) — handles plain arithmetic types,
    // wrappers (safe_int), and containers (cartesian_vector<int>) alike.
    // For integral/pure-divisor magnitudes, uses the type's own operator* / operator/
    // (safe_int's overflow checks, cartesian_vector's element-wise operations).
    // For rational magnitudes, widens the factor to prevent intermediate overflow;
    // the wider factor propagates through the type's own operators.
    // static_cast<To> triggers the wrapper's checked narrowing constructor when needed.
    return static_cast<To>(scale_int<M{}, Mode>(value));
  }
}

}  // namespace detail

/**
 * @brief Scale @p value by the unit magnitude passed as @p m, converting to type @c To.
 *
 * When @p From provides a magnitude-aware @c operator*(From,M) customization point, it is
 * used first.  The return type may differ from @c To (e.g. a representation with scaled
 * bounds).  Otherwise, the built-in floating-point or integer path is
 * used and the result type is @c To.
 *
 * Use this in custom `operator*(T, UnitMagnitude)` implementations to reuse the
 * library's built-in scaling logic instead of duplicating it.
 */
MP_UNITS_EXPORT template<typename To, UnitMagnitude M, typename From>
  requires detail::UnitMagnitudeScalable<From>
[[nodiscard]] constexpr auto scale(M m, const From& value)
{
  return detail::scale_impl<To, detail::rounding_mode::truncated>(m, value);
}

/**
 * @brief Scale @p value by the unit magnitude passed as @p m, converting to type @c To,
 *        with the result rounded according to @p policy.
 */
MP_UNITS_EXPORT template<typename To, UnitMagnitude M, typename From, RoundingPolicy Policy>
  requires detail::UnitMagnitudeScalable<From>
[[nodiscard]] constexpr auto scale(M m, const From& value, Policy)
{
  return detail::scale_impl<To, detail::rounding_mode_of<Policy>>(m, value);
}

}  // namespace mp_units
