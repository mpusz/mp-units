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

#include <mp-units/bits/requires_hosted.h>
//
#include <mp-units/bits/module_macros.h>
#include <mp-units/ext/contracts.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/bits/fmt.h>
#include <mp-units/bits/format.h>
#include <mp-units/framework/quantity.h>
#include <mp-units/framework/unit.h>
#include <mp-units/utility/representation.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cmath>
#include <concepts>
#include <ostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#endif
#endif

namespace mp_units::utility {

/**
 * @brief A representation type for values with uncertainties
 *
 * This class represents a value with its associated uncertainty (standard deviation),
 * providing automatic uncertainty propagation through mathematical operations.
 *
 * @tparam T The underlying real scalar type (e.g., double, float)
 *
 * **Uncertainty Propagation:**
 *
 * The class implements first-order uncertainty propagation (linear approximation) using
 * standard formulas from error analysis:
 *
 * - Addition/Subtraction: σ² = σ_x² + σ_y² (quadrature sum)
 * - Multiplication: σ_f² = (y·σ_x)² + (x·σ_y)² (partial derivatives in quadrature)
 * - Division: σ_f² = (σ_x/y)² + (x·σ_y/y²)²
 * - Functions: σ_f = |df/dx| × σ_x (derivative times uncertainty)
 *
 * **Important Assumptions and Limitations:**
 *
 * 1. **Independent Variables:** All values are assumed to be statistically independent.
 *    Operations like `x - x` will give non-zero uncertainty (should be zero for perfectly
 *    correlated variables). For correlated values, a more sophisticated approach with
 *    covariance matrices would be needed.
 *
 * 2. **First-Order Approximation:** Uses linear approximation (first derivative only).
 *    This is accurate when uncertainties are small relative to the values. For large
 *    relative uncertainties or highly non-linear functions, higher-order terms may be needed.
 *
 * 3. **Gaussian Distribution:** Assumes uncertainties represent one standard deviation (σ)
 *    of normally distributed errors. Not suitable for systematic errors or non-Gaussian
 *    distributions without additional consideration.
 *
 * 4. **No Correlation Tracking:** The class does not track which values are derived
 *    from common sources, so it cannot detect or handle correlations automatically.
 *
 * **When to Use:**
 * - Combining independent measurements from different instruments
 * - Propagating random uncertainties through calculations
 * - Working with measured physical constants (see `measurement_of`)
 *
 * **When NOT to Use:**
 * - When values are correlated (e.g., `f(x,x)` where x appears multiple times)
 * - When systematic uncertainties dominate
 * - When uncertainties are large relative to values (>10%)
 * - For Monte Carlo simulations (use direct sampling instead)
 *
 * **Example:**
 * @code
 * quantity length = uncertain{10.0, 0.1} * m;  // 10.0 ± 0.1 m
 * quantity width = uncertain{5.0, 0.05} * m;   // 5.0 ± 0.05 m
 * quantity area = length * width;              // 50.0 ± 0.71 m²
 * @endcode
 */
// The element type is constrained (rather than left a bare `typename`) so that an unsupported one
// is rejected where `uncertain<T>` is named instead of deep inside a propagation formula:
// - `RealScalar` is what the propagation relies on - a real field (`hypot` and the derivative
//   formulas are real-valued) that is totally ordered (the uncertainty precondition compares
//   against zero),
// - `disable_representation` additionally excludes the types the library itself refuses to store
//   in a `quantity` (notably `bool`), so `uncertain<T>` is never a valid representation built on
//   an invalid one. It is checked directly rather than through `RepresentationOf` because the
//   latter needs the quantity-character machinery, which is not complete at this include point,
// - `representation_values<T>::zero()` is what the uncertainty precondition compares against.
//   `RealScalar` does not imply it: it requires `std::copyable` and `std::equality_comparable`
//   rather than `std::regular`, so neither default-construction nor construction from `int` is
//   guaranteed. Stating it here also keeps the requirement independent of whether the
//   precondition macro expands to anything in a given configuration.
MP_UNITS_EXPORT template<typename T>
  requires RealScalar<T> && (!disable_representation<T>) && requires { representation_values<T>::zero(); }
class uncertain {
public:
  using value_type = T;

  [[nodiscard]] uncertain() = default;

  /**
   * @brief Converts an exact value to an uncertain one (value ± 0)
   *
   * Intentionally implicit - an exact value is losslessly representable as an uncertain value
   * with a zero uncertainty, which lets exact quantities join uncertainty propagation in
   * mixed-representation arithmetic.
   */
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  [[nodiscard]] constexpr explicit(false) uncertain(value_type val) : value_(std::move(val)) {}

  /**
   * @brief Constructs an uncertain value
   *
   * @param val The central value
   * @param err The absolute standard uncertainty (must be non-negative)
   */
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  [[nodiscard]] constexpr explicit uncertain(value_type val, value_type err) :
      value_(std::move(val)), uncertainty_(std::move(err))
  {
    MP_UNITS_PRECONDITION(uncertainty_ >= representation_values<value_type>::zero());
  }

  /// @brief Returns the central value
  [[nodiscard]] constexpr const value_type& value() const { return value_; }

  /// @brief Returns the absolute uncertainty (standard deviation)
  [[nodiscard]] constexpr const value_type& uncertainty() const { return uncertainty_; }

  /// @brief Returns the relative uncertainty (σ/x)
  [[nodiscard]] constexpr value_type relative_uncertainty() const { return uncertainty() / value(); }

  /// @brief Returns the lower bound of the uncertainty interval (value - σ)
  [[nodiscard]] constexpr value_type lower_bound() const { return value() - uncertainty(); }

  /// @brief Returns the upper bound of the uncertainty interval (value + σ)
  [[nodiscard]] constexpr value_type upper_bound() const { return value() + uncertainty(); }

  /// @brief Unary negation (uncertainty unchanged)
  [[nodiscard]] constexpr uncertain operator-() const { return uncertain(-value(), uncertainty()); }

  /**
   * @brief Addition of two uncertain values
   * @note Assumes independent values. Formula: σ² = σ_x² + σ_y²
   */
  [[nodiscard]] friend constexpr uncertain operator+(const uncertain& lhs, const uncertain& rhs)
  {
    using std::hypot;
    return uncertain(lhs.value() + rhs.value(), hypot(lhs.uncertainty(), rhs.uncertainty()));
  }

  /**
   * @brief Subtraction of two uncertain values
   * @note Assumes independent values. Formula: σ² = σ_x² + σ_y²
   * @warning For correlated values (e.g., x - x), this will incorrectly give non-zero uncertainty.
   */
  [[nodiscard]] friend constexpr uncertain operator-(const uncertain& lhs, const uncertain& rhs)
  {
    using std::hypot;
    return uncertain(lhs.value() - rhs.value(), hypot(lhs.uncertainty(), rhs.uncertainty()));
  }

  /**
   * @brief Multiplication of two uncertain values
   *
   * @note Assumes independent values. Formula: σ_f² = (y·σ_x)² + (x·σ_y)²
   * @note This is the absolute form of `(σ_f/f)² = (σ_x/x)² + (σ_y/y)²`. The two agree for
   *       non-zero operands, but the relative form divides by the value, so it yields NaN when
   *       an operand is zero (`0 ± 0.1` is a perfectly ordinary measurement).
   */
  [[nodiscard]] friend constexpr uncertain operator*(const uncertain& lhs, const uncertain& rhs)
  {
    using std::hypot;
    return uncertain(lhs.value() * rhs.value(),
                     hypot(rhs.value() * lhs.uncertainty(), lhs.value() * rhs.uncertainty()));
  }

  /**
   * @brief Multiplication by an exact scalar
   * @note The scalar is treated as exact (no uncertainty). Formula: σ_f = |k| × σ_x
   */
  [[nodiscard]] friend constexpr uncertain operator*(const uncertain& lhs,
                                                     const std::convertible_to<value_type> auto& value)
  {
    using std::abs;
    return uncertain(static_cast<value_type>(lhs.value() * value),
                     static_cast<value_type>(abs(value) * lhs.uncertainty()));
  }

  /// @brief Multiplication by an exact scalar (commutative)
  [[nodiscard]] friend constexpr uncertain operator*(const std::convertible_to<value_type> auto& value,
                                                     const uncertain& rhs)
  {
    using std::abs;
    return uncertain(value * rhs.value(), abs(value) * rhs.uncertainty());
  }

  /**
   * @brief Division of two uncertain values
   *
   * @note Assumes independent values. Formula: σ_f² = (σ_x/y)² + (x·σ_y/y²)²
   * @note The absolute form is used for the same reason as in `operator*`: it stays well-defined
   *       for a zero numerator.
   */
  [[nodiscard]] friend constexpr uncertain operator/(const uncertain& lhs, const uncertain& rhs)
  {
    const auto val = lhs.value() / rhs.value();
    using std::hypot;
    return uncertain(val, hypot(lhs.uncertainty() / rhs.value(), val * rhs.relative_uncertainty()));
  }

  /**
   * @brief Division by an exact scalar
   * @note The scalar is treated as exact (no uncertainty). Formula: σ_f = σ_x / |k|
   */
  [[nodiscard]] friend constexpr uncertain operator/(const uncertain& lhs,
                                                     const std::convertible_to<value_type> auto& value)
  {
    using std::abs;
    return uncertain(static_cast<value_type>(lhs.value()) / value,
                     static_cast<value_type>(lhs.uncertainty() / abs(value)));
  }

  /**
   * @brief Division of an exact scalar by an uncertain value
   * @note Formula: σ_f = |f| × (σ_x / x)
   */
  [[nodiscard]] friend constexpr uncertain operator/(const std::convertible_to<value_type> auto& value,
                                                     const uncertain& rhs)
  {
    const auto val = static_cast<value_type>(value / rhs.value());
    using std::abs;
    return uncertain(val, abs(val) * rhs.relative_uncertainty());
  }

  /**
   * @brief Adds the uncertainty of a unit conversion factor to the value
   *
   * Implements the `fold_conversion_uncertainty` customization point. The two are taken as
   * independent, so they combine in quadrature: σ' = √(σ² + (x·u_r)²).
   */
  [[nodiscard]] friend constexpr uncertain fold_conversion_uncertainty(const uncertain& val,
                                                                       long double relative_uncertainty)
  {
    using std::hypot;
    return uncertain(val.value(),
                     static_cast<value_type>(hypot(static_cast<long double>(val.uncertainty()),
                                                   static_cast<long double>(val.value()) * relative_uncertainty)));
  }

  /**
   * @brief Comparison of two uncertain values
   *
   * `RealScalar` requires `std::totally_ordered`, so a real scalar representation cannot opt out
   * of ordering. The order is primarily by the central value, with the uncertainty as a
   * tie-breaker, which keeps it total and consistent with equality. It is structural, not
   * statistical: values with overlapping uncertainty intervals still compare as unequal. For
   * interval-aware comparisons use the `lower_bound()`/`upper_bound()` observers.
   */
  [[nodiscard]] constexpr auto operator<=>(const uncertain&) const = default;

  friend std::ostream& operator<<(std::ostream& os, const uncertain& arg)
  {
    return os << arg.value() << " ± " << arg.uncertainty();
  }

  /**
   * @brief Absolute value of an uncertain value
   * @note Uncertainty is preserved unchanged.
   */
  [[nodiscard]] friend constexpr uncertain abs(const uncertain& arg)
    requires requires { abs(arg.value()); } || requires { std::abs(arg.value()); }
  {
    using std::abs;
    return uncertain(abs(arg.value()), arg.uncertainty());
  }

  /**
   * @brief Power function with runtime exponent
   *
   * @param base The uncertain value to raise to a power
   * @param exponent The exponent (exact value, no uncertainty)
   * @return uncertain Result with propagated uncertainty
   *
   * @note Formula: if f = x^n, then σ_f = |n × x^(n-1) × σ_x| = |n × f/x × σ_x|
   * @note This is a first-order approximation valid for small relative uncertainties.
   */
  [[nodiscard]] friend constexpr uncertain pow(const uncertain& base, const value_type& exponent)
    requires requires { pow(base.value(), exponent); } || requires { std::pow(base.value(), exponent); }
  {
    using std::abs;
    using std::pow;
    const auto val = pow(base.value(), exponent);
    return uncertain(val, abs(exponent * val / base.value() * base.uncertainty()));
  }

  /**
   * @brief Square root of an uncertain value
   *
   * @note Formula: σ_f = σ_x / (2√x)
   * @note This is equivalent to pow(arg, 0.5) but more efficient.
   */
  [[nodiscard]] friend constexpr uncertain sqrt(const uncertain& arg)
    requires requires { sqrt(arg.value()); } || requires { std::sqrt(arg.value()); }
  {
    using std::sqrt;
    const auto val = sqrt(arg.value());
    return uncertain(val, arg.uncertainty() / (value_type{2} * val));
  }

  /**
   * @brief Exponential function of an uncertain value
   *
   * @note Formula: if f = exp(x), then σ_f = |exp(x) × σ_x| = |f × σ_x|
   * @warning Uncertainty grows exponentially with the value. For large x, this can lead
   *          to very large relative uncertainties where first-order approximation breaks down.
   */
  [[nodiscard]] friend constexpr uncertain exp(const uncertain& arg)
    requires requires { exp(arg.value()); } || requires { std::exp(arg.value()); }
  {
    using std::abs;
    using std::exp;
    const auto val = exp(arg.value());
    return uncertain(val, abs(val * arg.uncertainty()));
  }

  /**
   * @brief Natural logarithm of an uncertain value
   *
   * @note Formula: if f = ln(x), then σ_f = |σ_x / x| = σ_x / x (for positive x)
   * @note Relative uncertainty in x becomes absolute uncertainty in ln(x).
   */
  [[nodiscard]] friend constexpr uncertain log(const uncertain& arg)
    requires requires { log(arg.value()); } || requires { std::log(arg.value()); }
  {
    using std::abs;
    using std::log;
    return uncertain(log(arg.value()), abs(arg.uncertainty() / arg.value()));
  }

  /**
   * @brief Common (base-10) logarithm of an uncertain value
   * @note Formula: σ_f = |σ_x / (x × ln(10))|
   */
  [[nodiscard]] friend constexpr uncertain log10(const uncertain& arg)
    requires requires { log10(arg.value()); } || requires { std::log10(arg.value()); }
  {
    using std::abs;
    using std::log;
    using std::log10;
    return uncertain(log10(arg.value()), abs(arg.uncertainty() / (arg.value() * log(value_type{10}))));
  }

  /**
   * @brief Binary (base-2) logarithm of an uncertain value
   * @note Formula: σ_f = |σ_x / (x × ln(2))|
   */
  [[nodiscard]] friend constexpr uncertain log2(const uncertain& arg)
    requires requires { log2(arg.value()); } || requires { std::log2(arg.value()); }
  {
    using std::abs;
    using std::log;
    using std::log2;
    return uncertain(log2(arg.value()), abs(arg.uncertainty() / (arg.value() * log(value_type{2}))));
  }

  /**
   * @brief Cube root of an uncertain value
   * @note Formula: if f = ∛x, then σ_f = σ_x / (3 × f²)
   */
  [[nodiscard]] friend constexpr uncertain cbrt(const uncertain& arg)
    requires requires { cbrt(arg.value()); } || requires { std::cbrt(arg.value()); }
  {
    using std::abs;
    using std::cbrt;
    const auto val = cbrt(arg.value());
    return uncertain(val, abs(arg.uncertainty() / (value_type{3} * val * val)));
  }

  /**
   * @brief Sine of an uncertain value (argument in radians)
   * @note Formula: σ_f = |cos(x)| × σ_x
   */
  [[nodiscard]] friend constexpr uncertain sin(const uncertain& arg)
    requires requires { sin(arg.value()); } || requires { std::sin(arg.value()); }
  {
    using std::abs;
    using std::cos;
    using std::sin;
    return uncertain(sin(arg.value()), abs(cos(arg.value())) * arg.uncertainty());
  }

  /**
   * @brief Cosine of an uncertain value (argument in radians)
   * @note Formula: σ_f = |sin(x)| × σ_x
   */
  [[nodiscard]] friend constexpr uncertain cos(const uncertain& arg)
    requires requires { cos(arg.value()); } || requires { std::cos(arg.value()); }
  {
    using std::abs;
    using std::cos;
    using std::sin;
    return uncertain(cos(arg.value()), abs(sin(arg.value())) * arg.uncertainty());
  }

  /**
   * @brief Tangent of an uncertain value (argument in radians)
   * @note Formula: σ_f = σ_x / cos²(x)
   */
  [[nodiscard]] friend constexpr uncertain tan(const uncertain& arg)
    requires requires { tan(arg.value()); } || requires { std::tan(arg.value()); }
  {
    using std::abs;
    using std::cos;
    using std::tan;
    const auto cos_val = cos(arg.value());
    return uncertain(tan(arg.value()), abs(arg.uncertainty() / (cos_val * cos_val)));
  }

  /**
   * @brief Arc sine of an uncertain value (result in radians)
   * @note Formula: σ_f = σ_x / √(1 - x²)
   */
  [[nodiscard]] friend constexpr uncertain asin(const uncertain& arg)
    requires requires { asin(arg.value()); } || requires { std::asin(arg.value()); }
  {
    using std::abs;
    using std::asin;
    using std::sqrt;
    return uncertain(asin(arg.value()), abs(arg.uncertainty() / sqrt(value_type{1} - arg.value() * arg.value())));
  }

  /**
   * @brief Arc cosine of an uncertain value (result in radians)
   * @note Formula: σ_f = σ_x / √(1 - x²), the same magnitude as for `asin`
   */
  [[nodiscard]] friend constexpr uncertain acos(const uncertain& arg)
    requires requires { acos(arg.value()); } || requires { std::acos(arg.value()); }
  {
    using std::abs;
    using std::acos;
    using std::sqrt;
    return uncertain(acos(arg.value()), abs(arg.uncertainty() / sqrt(value_type{1} - arg.value() * arg.value())));
  }

  /**
   * @brief Arc tangent of an uncertain value (result in radians)
   * @note Formula: σ_f = σ_x / (1 + x²)
   */
  [[nodiscard]] friend constexpr uncertain atan(const uncertain& arg)
    requires requires { atan(arg.value()); } || requires { std::atan(arg.value()); }
  {
    using std::abs;
    using std::atan;
    return uncertain(atan(arg.value()), abs(arg.uncertainty() / (value_type{1} + arg.value() * arg.value())));
  }

  /**
   * @brief Two-argument arc tangent (result in radians)
   *
   * @note Assumes independent values. Formula: σ_f² = ((x·σ_y)² + (y·σ_x)²) / (x² + y²)²
   */
  [[nodiscard]] friend constexpr uncertain atan2(const uncertain& lhs, const uncertain& rhs)
    requires requires { atan2(lhs.value(), rhs.value()); } || requires { std::atan2(lhs.value(), rhs.value()); }
  {
    using std::atan2;
    using std::hypot;
    const auto sum_of_squares = rhs.value() * rhs.value() + lhs.value() * lhs.value();
    return uncertain(atan2(lhs.value(), rhs.value()),
                     hypot(rhs.value() * lhs.uncertainty(), lhs.value() * rhs.uncertainty()) / sum_of_squares);
  }

private:
  value_type value_{};
  value_type uncertainty_{};
};

/**
 * @brief Returns a measured constant as a quantity with an `uncertain` representation
 *
 * The uncertainty of a measured constant is relational: expressed in its own unit the constant
 * is exactly `1`, and the uncertainty belongs to the conversion factor between that unit and
 * others. This function therefore returns `1 ± 0` of the constant in an uncertainty-capable
 * representation, and the conversion engine materializes the relative standard uncertainty the
 * moment the quantity leaves units in which the constant cancels (see
 * `fold_conversion_uncertainty`). Seeding the value with `u_r` up front would double-count it on
 * such a conversion, because the value's uncertainty and the factor's are the same, fully
 * correlated one.
 *
 * @code
 * quantity G1 = measurement_of(iau::G);           // uncertain<double>{1} * G, exactly 1 G
 * quantity G2 = G1.in(m3 / kg / s2);              // 6.6743e-11 ± 1.46835e-15, u_r materialized
 * quantity G3 = measurement_of<float>(iau::G);    // uncertain<float>
 * @endcode
 *
 * @tparam Rep the underlying numeric type of the resulting `uncertain` representation
 */
MP_UNITS_EXPORT template<typename Rep = double, MeasuredConstant C>
[[nodiscard]] constexpr Quantity auto measurement_of(C)
{
  return uncertain<Rep>{Rep{1}} * C{};
}

namespace detail {

// Significand digits (without the sign and the radix separator) and the decimal exponent of
// a value rendered with a scientific presentation type.
struct scientific_parts {
  std::string digits;
  int exponent = 0;
};

[[nodiscard]] inline scientific_parts split_scientific(std::string_view text)
{
  const auto exp_pos = text.find_first_of("eE");
  scientific_parts parts;
  for (const char ch : text.substr(0, exp_pos))
    if ('0' <= ch && ch <= '9') parts.digits.push_back(ch);
  if (exp_pos == std::string_view::npos) return parts;

  auto exp_text = text.substr(exp_pos + 1);
  const bool negative = !exp_text.empty() && exp_text.front() == '-';
  if (!exp_text.empty() && (exp_text.front() == '-' || exp_text.front() == '+')) exp_text.remove_prefix(1);
  for (const char ch : exp_text) parts.exponent = parts.exponent * 10 + (ch - '0');
  if (negative) parts.exponent = -parts.exponent;
  return parts;
}

/**
 * @brief Renders `value(digits)` as specified in ISO 80000-1:2022, 7.2.4
 *
 * The digits in parentheses are the standard uncertainty expressed in terms of the least
 * significant digits of the value (`23.4782(32)` means `23.4782 ± 0.0032`). Every rounding
 * decision is delegated to the formatter of `T`, so a carry (`9.99` becoming `10.0`) is
 * resolved by the same rules that round any other number.
 *
 * @param sig_digits the number of significant digits of the uncertainty
 * @param type the presentation type: `e`/`f`/`g` and their uppercase forms, or `\0` for auto
 * @param sign the `sign` format option
 * @param format_value renders the value itself, given the number of decimals to quote it to and
 *        whether the scientific form was selected; supplied by the caller so that the `#` and
 *        `L` options reach the underlying formatter of `T`
 */
template<std::floating_point T, typename ValueFormatter>
[[nodiscard]] std::string format_concise(T value, T uncertainty, int sig_digits, char type, char sign,
                                         const ValueFormatter& format_value)
{
  using std::isfinite;
  const bool upper = (type == 'E' || type == 'F' || type == 'G');
  // Deriving how many digits to show never goes through the user's `#` and `L` options: the
  // count in parentheses is a plain integer, and a localized radix separator or an enforced
  // one would only get in the way of reading the exponent back out.
  const auto fmt_sci = [upper](T val, int decimals) {
    return upper ? MP_UNITS_STD_FMT::format("{:.{}E}", val, decimals)
                 : MP_UNITS_STD_FMT::format("{:.{}e}", val, decimals);
  };
  const auto with_sign = [sign](std::string text) {
    if (sign != '-' && !text.empty() && text.front() != '-') text.insert(text.begin(), sign);
    return text;
  };

  // Neither component can be quoted to a common decimal place, so fall back to the `±` form.
  if (!isfinite(value) || !isfinite(uncertainty))
    return with_sign(MP_UNITS_STD_FMT::format("{} ± {}", value, uncertainty));

  // Without an uncertainty there is no rounding rule; keep every digit the value shows.
  if (uncertainty == representation_values<T>::zero()) {
    std::string text = MP_UNITS_STD_FMT::format("{}", value);
    const auto exp_pos = text.find_first_of("eE");
    text.insert(exp_pos == std::string::npos ? text.size() : exp_pos, "(0)");
    return with_sign(std::move(text));
  }

  const scientific_parts unc = split_scientific(fmt_sci(uncertainty, sig_digits - 1));
  const int place = unc.exponent - (sig_digits - 1);
  const int value_exponent = split_scientific(fmt_sci(value, 6)).exponent;

  // Quoting a value whose last shown digit sits left of the decimal point would leave its
  // trailing zeros ambiguous, exactly the ambiguity ISO 80000-1:2022, 7.2.4 warns about, so
  // `place > 0` switches to the scientific form where the significant digits are explicit.
  const bool scientific =
    (type == 'e' || type == 'E') || (type != 'f' && type != 'F' && (value_exponent < -4 || place > 0));

  int decimals = 0;
  int shown_place = 0;
  if (scientific) {
    decimals = (value_exponent > place) ? value_exponent - place : 0;
    // Rounding may carry into the next power of ten (9.99e2 becoming 1.00e3), which moves the
    // exponent and therefore the number of mantissa digits needed to reach `place`.
    int shown_exponent = split_scientific(fmt_sci(value, decimals)).exponent;
    if (shown_exponent != value_exponent) {
      decimals = (shown_exponent > place) ? shown_exponent - place : 0;
      shown_exponent = split_scientific(fmt_sci(value, decimals)).exponent;
    }
    shown_place = shown_exponent - decimals;
  } else {
    decimals = (place < 0) ? -place : 0;
    shown_place = -decimals;
  }

  // The parenthesized number counts units of the last digit actually shown, which is coarser
  // than `place` whenever the uncertainty outruns the digits the value can carry.
  std::string digits = unc.digits;
  for (int i = shown_place; i < place; ++i) digits.push_back('0');

  std::string text = format_value(value, decimals, scientific);
  const auto exp_pos = text.find_first_of("eE");
  text.insert(exp_pos == std::string::npos ? text.size() : exp_pos, "(" + digits + ")");
  return with_sign(std::move(text));
}

}  // namespace detail

}  // namespace mp_units::utility

template<typename T, typename U>
  requires requires { typename std::common_type_t<T, U>; }
struct std::common_type<mp_units::utility::uncertain<T>, mp_units::utility::uncertain<U>> {
  using type = mp_units::utility::uncertain<std::common_type_t<T, U>>;
};

// An exact value joins uncertainty propagation as `value ± 0`, so mixed-representation quantity
// arithmetic (e.g., an `uncertain` quantity plus an exact one) converges on `uncertain`.
template<typename T, typename U>
  requires(std::is_arithmetic_v<U>) && requires { typename std::common_type_t<T, U>; }
struct std::common_type<mp_units::utility::uncertain<T>, U> {
  using type = mp_units::utility::uncertain<std::common_type_t<T, U>>;
};

template<typename T, typename U>
  requires(std::is_arithmetic_v<U>) && requires { typename std::common_type_t<T, U>; }
struct std::common_type<U, mp_units::utility::uncertain<T>> {
  using type = mp_units::utility::uncertain<std::common_type_t<T, U>>;
};

/**
 * @brief Formatting support for `uncertain`
 *
 * By default both components are printed as `value ± uncertainty`, each formatted with the
 * `format-spec` of the underlying representation type.
 *
 * A `~` appended to that spec selects instead the concise notation of ISO 80000-1:2022,
 * 7.2.4, in which the value is quoted to the last significant digit of the uncertainty and
 * the uncertainty follows in parentheses, counted in units of that digit. The tilde marks
 * the output as an approximation: the default form prints the value as it is, while this one
 * rounds it to the precision the uncertainty justifies:
 *
 * @code
 * uncertain val{6.67430e-11, 1.5e-15};
 * std::println("{}", val);        // 6.6743e-11 ± 1.5e-15
 * std::println("{:~}", val);      // 6.67430(15)e-11
 * std::println("{:.1~}", val);    // 6.6743(2)e-11
 * @endcode
 *
 * In the concise form `precision` gives the number of significant digits of the uncertainty
 * (two by default, as used by CODATA and recommended by GUM 7.2.6) rather than a digit count
 * for the value, and `type` selects between the scientific (`e`) and fixed (`f`) forms. The
 * default picks the one that leaves no trailing digit of the value ambiguous.
 */
template<typename T, typename Char>
struct MP_UNITS_STD_FMT::formatter<mp_units::utility::uncertain<T>, Char> : formatter<T, Char> {
  // The options of the concise notation. `unit_symbol_formatting` is a public type because the
  // `unit_symbol` API takes one; these have no consumer outside this formatter, so they stay here.
  struct concise_formatting {
    int significant_digits = 2;
    char sign = '-';
    char type = '\0';
    bool alternate = false;
    bool localized = false;
  };
  struct format_specs : mp_units::detail::fill_align_width_format_specs<Char>, concise_formatting {};
  format_specs specs_{};
  bool concise_ = false;

  // Finds the end of this replacement field, skipping over the nested `{...}` of a dynamic
  // width or precision so that their closing braces are not mistaken for the field's own.
  template<std::forward_iterator It>
  [[nodiscard]] static constexpr It find_spec_end(It begin, It end)
  {
    for (auto it = begin; it != end; ++it) {
      if (*it == '{') {
        while (it != end && *it != '}') ++it;
        if (it == end) break;
      } else if (*it == '}')
        return it;
    }
    return end;
  }

  // Parses a concise spec, which follows `std-format-spec` up to the terminating '~'. The
  // options are taken in the order the standard grammar defines them, so this cannot use
  // `parse_fill_align_width` (that one goes straight from the alignment to the width, while
  // `sign` and `#` sit in between).
  template<std::forward_iterator It>
  constexpr It parse_concise_specs(It begin, It end, MP_UNITS_STD_FMT::basic_format_parse_context<Char>& ctx)
  {
    auto it = begin;
    if (it == end) return end;

    it = mp_units::detail::parse_align(it, end, specs_);
    if (it != end && (*it == '+' || *it == '-' || *it == ' ')) specs_.sign = static_cast<char>(*it++);
    if (it != end && *it == '#') {
      specs_.alternate = true;
      ++it;
    }
    if (it != end) it = mp_units::detail::parse_dynamic_spec(it, end, specs_.width, specs_.width_ref, ctx);
    if (it != end && *it == '.') {
      ++it;
      if (it == end || *it < '0' || '9' < *it)
        throw MP_UNITS_STD_FMT::format_error("dynamic precision is not supported with the '~' format option");
      const int precision = mp_units::detail::parse_nonnegative_int(it, end, -1);
      if (precision <= 0)
        throw MP_UNITS_STD_FMT::format_error(
          "the precision of the '~' format option is a number of significant digits and must be positive");
      specs_.significant_digits = precision;
    }
    if (it != end && *it == 'L') {
      specs_.localized = true;
      ++it;
    }
    if (it != end) {
      constexpr auto valid_types = std::string_view{"eEfFgG"};
      if (valid_types.find(static_cast<char>(*it)) != std::string_view::npos) specs_.type = static_cast<char>(*it++);
    }
    if (it != end) throw MP_UNITS_STD_FMT::format_error("invalid format specifier for 'uncertain'");
    return end;
  }

public:
  constexpr auto parse(MP_UNITS_STD_FMT::basic_format_parse_context<Char>& ctx) -> decltype(ctx.begin())
  {
    const auto begin = ctx.begin();
    const auto spec_end = find_spec_end(begin, ctx.end());
    if (spec_end == begin || *(spec_end - 1) != '~') return formatter<T, Char>::parse(ctx);

    if constexpr (!std::floating_point<T>)
      throw MP_UNITS_STD_FMT::format_error(
        "the '~' (concise uncertainty) format option requires a floating-point representation type");

    concise_ = true;
    parse_concise_specs(begin, spec_end - 1, ctx);
    return spec_end;  // consume the terminating '~'
  }

  template<typename FormatContext>
  auto format(const mp_units::utility::uncertain<T>& arg, FormatContext& ctx) const
  {
    // `parse` rejects '~' for the other representation types, so the concise renderer is
    // never instantiated for them.
    if constexpr (std::floating_point<T>)
      if (concise_) {
        auto specs = specs_;
        mp_units::detail::handle_dynamic_spec<mp_units::detail::width_checker>(specs.width, specs.width_ref, ctx);
        // The number of decimals is derived from the uncertainty rather than taken from the
        // spec, so the value is rendered through a spec rebuilt around it. That keeps `#` and
        // `L` working, which the standard formatter of `T` applies for us.
        const auto format_value = [&specs, &ctx](const T& val, int decimals, bool scientific) {
          const bool upper = (specs.type == 'E' || specs.type == 'F' || specs.type == 'G');
          std::string spec = "{:";
          if (specs.alternate) spec += '#';
          spec += '.';
          spec += std::to_string(decimals);
          if (specs.localized) spec += 'L';
          spec += scientific ? (upper ? 'E' : 'e') : (upper ? 'F' : 'f');
          spec += '}';
          if (specs.localized)
            return MP_UNITS_STD_FMT::vformat(MP_UNITS_FMT_LOCALE(ctx.locale()), spec,
                                             MP_UNITS_STD_FMT::make_format_args(val));
          return MP_UNITS_STD_FMT::vformat(spec, MP_UNITS_STD_FMT::make_format_args(val));
        };
        const std::string text = mp_units::utility::detail::format_concise(
          arg.value(), arg.uncertainty(), specs.significant_digits, specs.type, specs.sign, format_value);
        return mp_units::detail::write_padded<Char>(ctx.out(), std::basic_string_view<Char>{text.data(), text.size()},
                                                    specs.width, specs.align, specs.fill);
      }

    auto out = formatter<T, Char>::format(arg.value(), ctx);
    out = MP_UNITS_STD_FMT::format_to(out, " ± ");
    ctx.advance_to(out);
    return formatter<T, Char>::format(arg.uncertainty(), ctx);
  }
};
