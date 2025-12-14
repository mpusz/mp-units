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

#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cmath>
#include <compare>  // IWYU pragma: export
#include <ostream>
#include <utility>
#endif

/**
 * @brief A representation type for physical measurements with uncertainties.
 *
 * This class represents a measured value with its associated uncertainty (standard deviation),
 * providing automatic uncertainty propagation through mathematical operations.
 *
 * @tparam T The underlying numeric type (e.g., double, float)
 *
 * **Uncertainty Propagation:**
 *
 * The class implements first-order uncertainty propagation (linear approximation) using
 * standard formulas from error analysis:
 *
 * - Addition/Subtraction: σ² = σ_x² + σ_y² (quadrature sum)
 * - Multiplication/Division: (σ/f)² = (σ_x/x)² + (σ_y/y)² (relative uncertainties in quadrature)
 * - Functions: σ_f = |df/dx| × σ_x (derivative times uncertainty)
 *
 * **Important Assumptions and Limitations:**
 *
 * 1. **Independent Variables:** All measurements are assumed to be statistically independent.
 *    Operations like `x - x` will give non-zero uncertainty (should be zero for perfectly
 *    correlated variables). For correlated measurements, a more sophisticated approach with
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
 * 4. **No Correlation Tracking:** The class does not track which measurements are derived
 *    from common sources, so it cannot detect or handle correlations automatically.
 *
 * **When to Use:**
 * - Combining independent measurements from different instruments
 * - Propagating random uncertainties through calculations
 * - Educational purposes and demonstrations
 *
 * **When NOT to Use:**
 * - When measurements are correlated (e.g., `f(x,x)` where x appears multiple times)
 * - When systematic uncertainties dominate
 * - When uncertainties are large relative to values (>10%)
 * - For Monte Carlo simulations (use direct sampling instead)
 *
 * @note This implementation is suitable for typical experimental physics and engineering
 *       calculations where independent measurements with small relative uncertainties are
 *       combined. For IAU astronomical constants and similar systems defined in terms of
 *       uncertainties, this provides appropriate uncertainty propagation.
 *
 * **Example:**
 * @code
 * auto length = measurement{10.0, 0.1} * m;  // 10.0 ± 0.1 m
 * auto width = measurement{5.0, 0.05} * m;   // 5.0 ± 0.05 m
 * auto area = length * width;                // 50.0 ± 0.71 m²
 * @endcode
 */
template<typename T>
class measurement {
public:
  using value_type = T;

  measurement() = default;

  /**
   * @brief Constructs a measurement with a value and uncertainty.
   *
   * @param val The measured value
   * @param err The uncertainty (absolute, will be converted to positive value)
   *
   * @note The uncertainty is stored as absolute value |err|, so negative uncertainties
   *       are automatically corrected.
   */
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  constexpr explicit measurement(value_type val, const value_type& err = {}) :
      value_(std::move(val)), uncertainty_([&] {
        using namespace std;
        return abs(err);
      }())
  {
  }

  /// @brief Returns the central measured value
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
  [[nodiscard]] constexpr measurement operator-() const { return measurement(-value(), uncertainty()); }

  /**
   * @brief Addition of two measurements.
   * @note Assumes independent measurements. Formula: σ² = σ_x² + σ_y²
   */
  [[nodiscard]] friend constexpr measurement operator+(const measurement& lhs, const measurement& rhs)
  {
    using namespace std;
    return measurement(lhs.value() + rhs.value(), hypot(lhs.uncertainty(), rhs.uncertainty()));
  }

  /**
   * @brief Subtraction of two measurements.
   * @note Assumes independent measurements. Formula: σ² = σ_x² + σ_y²
   * @warning For correlated measurements (e.g., x - x), this will incorrectly give non-zero uncertainty.
   */
  [[nodiscard]] friend constexpr measurement operator-(const measurement& lhs, const measurement& rhs)
  {
    using namespace std;
    return measurement(lhs.value() - rhs.value(), hypot(lhs.uncertainty(), rhs.uncertainty()));
  }

  /**
   * @brief Multiplication of two measurements.
   * @note Assumes independent measurements. Formula: (σ_f/f)² = (σ_x/x)² + (σ_y/y)²
   */
  [[nodiscard]] friend constexpr measurement operator*(const measurement& lhs, const measurement& rhs)
  {
    const auto val = lhs.value() * rhs.value();
    using namespace std;
    return measurement(val, val * hypot(lhs.relative_uncertainty(), rhs.relative_uncertainty()));
  }

  /**
   * @brief Multiplication by an exact scalar.
   * @note The scalar is treated as exact (no uncertainty). Formula: σ_f = |k| × σ_x
   */
  [[nodiscard]] friend constexpr measurement operator*(const measurement& lhs,
                                                       const std::convertible_to<value_type> auto& value)
  {
    using namespace std;
    return measurement(static_cast<value_type>(lhs.value() * value),
                       static_cast<value_type>(abs(value) * lhs.uncertainty()));
  }

  /// @brief Multiplication by an exact scalar (commutative).
  [[nodiscard]] friend constexpr measurement operator*(const std::convertible_to<value_type> auto& value,
                                                       const measurement& rhs)
  {
    using namespace std;
    return measurement(value * rhs.value(), abs(value) * rhs.uncertainty());
  }

  /**
   * @brief Division of two measurements.
   * @note Assumes independent measurements. Formula: (σ_f/f)² = (σ_x/x)² + (σ_y/y)²
   */
  [[nodiscard]] friend constexpr measurement operator/(const measurement& lhs, const measurement& rhs)
  {
    const auto val = lhs.value() / rhs.value();
    using namespace std;
    return measurement(val, val * hypot(lhs.relative_uncertainty(), rhs.relative_uncertainty()));
  }

  /**
   * @brief Division by an exact scalar.
   * @note The scalar is treated as exact (no uncertainty). Formula: σ_f = σ_x / |k|
   */
  [[nodiscard]] friend constexpr measurement operator/(const measurement& lhs,
                                                       const std::convertible_to<value_type> auto& value)
  {
    using namespace std;
    return measurement(static_cast<value_type>(lhs.value()) / value,
                       static_cast<value_type>(lhs.uncertainty() / abs(value)));
  }

  /**
   * @brief Division of an exact scalar by a measurement.
   * @note Formula: σ_f = |f| × (σ_x / x)
   */
  [[nodiscard]] friend constexpr measurement operator/(const std::convertible_to<value_type> auto& value,
                                                       const measurement& rhs)
  {
    const auto val = static_cast<value_type>(value / rhs.value());
    using namespace std;
    return measurement(val, abs(val) * rhs.relative_uncertainty());
  }

  [[nodiscard]] constexpr auto operator<=>(const measurement&) const = default;

  friend std::ostream& operator<<(std::ostream& os, const measurement& v)
  {
    return os << v.value() << " ± " << v.uncertainty();
  }

  /**
   * @brief Absolute value of a measurement.
   * @note Uncertainty is preserved unchanged.
   */
  [[nodiscard]] friend constexpr measurement abs(const measurement& v)
    requires requires { abs(v.value()); } || requires { std::abs(v.value()); }
  {
    using std::abs;
    return measurement(abs(v.value()), v.uncertainty());
  }

  /**
   * @brief Power function with runtime exponent.
   *
   * @param base The measurement to raise to a power
   * @param exponent The exponent (exact value, no uncertainty)
   * @return measurement Result with propagated uncertainty
   *
   * @note Formula: if f = x^n, then σ_f = |n × x^(n-1) × σ_x| = |n × f/x × σ_x|
   * @note This is a first-order approximation valid for small relative uncertainties.
   */
  [[nodiscard]] friend constexpr measurement pow(const measurement& base, const value_type& exponent)
    requires requires { pow(base.value(), exponent); } || requires { std::pow(base.value(), exponent); }
  {
    using std::pow;
    using std::abs;
    const auto val = pow(base.value(), exponent);
    return measurement(val, abs(exponent * val / base.value() * base.uncertainty()));
  }

  /**
   * @brief Square root of a measurement.
   *
   * @note Formula: σ_f = σ_x / (2√x)
   * @note This is equivalent to pow(v, 0.5) but more efficient.
   */
  [[nodiscard]] friend constexpr measurement sqrt(const measurement& v)
    requires requires { sqrt(v.value()); } || requires { std::sqrt(v.value()); }
  {
    using std::sqrt;
    const auto val = sqrt(v.value());
    return measurement(val, v.uncertainty() / (value_type{2} * val));
  }

  /**
   * @brief Exponential function of a measurement.
   *
   * @note Formula: if f = exp(x), then σ_f = |exp(x) × σ_x| = |f × σ_x|
   * @warning Uncertainty grows exponentially with the value. For large x, this can lead
   *          to very large relative uncertainties where first-order approximation breaks down.
   */
  [[nodiscard]] friend constexpr measurement exp(const measurement& v)
    requires requires { exp(v.value()); } || requires { std::exp(v.value()); }
  {
    using std::exp;
    using std::abs;
    const auto val = exp(v.value());
    return measurement(val, abs(val * v.uncertainty()));
  }

  /**
   * @brief Natural logarithm of a measurement.
   *
   * @note Formula: if f = ln(x), then σ_f = |σ_x / x| = σ_x / x (for positive x)
   * @note Relative uncertainty in x becomes absolute uncertainty in ln(x).
   */
  [[nodiscard]] friend constexpr measurement log(const measurement& v)
    requires requires { log(v.value()); } || requires { std::log(v.value()); }
  {
    using std::log;
    using std::abs;
    return measurement(log(v.value()), abs(v.uncertainty() / v.value()));
  }

private:
  value_type value_{};
  value_type uncertainty_{};
};
