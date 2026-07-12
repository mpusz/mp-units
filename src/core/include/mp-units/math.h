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

#include <mp-units/bits/module_macros.h>
#include <mp-units/compat_macros.h>
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/quantity.h>
#include <mp-units/framework/quantity_point.h>
#include <mp-units/framework/unit.h>
#include <mp-units/framework/value_cast.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <version>
#if MP_UNITS_HOSTED
#include <cmath>
#endif  // MP_UNITS_HOSTED
#if MP_UNITS_HOSTED || __cpp_lib_freestanding_numeric >= 202502L
#include <numeric>
#endif
#endif  // MP_UNITS_IMPORT_STD
#endif  // MP_UNITS_IN_MODULE_INTERFACE

MP_UNITS_EXPORT
namespace mp_units {

// The math functions below customize on their standard-library counterparts through ADL. Only the
// `std::`-qualified fallback (and its use in the constraints) is guarded on availability, so a user
// providing an ADL overload for a custom representation type can still use these in freestanding
// mode. `std::abs` (in `<cstdlib>`/`<cmath>`, freestanding since C++26 - P2338) and `std::midpoint`
// (in `<numeric>`, freestanding since C++26 - P2976) enable their fallback whenever the matching
// freestanding feature-test macro is present; the remaining functions rely on the rest of `<cmath>`,
// which is hosted-only.

/**
 * @brief Computes the absolute value of a quantity
 *
 * @param q Quantity being the base of the operation
 * @return Quantity The absolute value of a provided quantity
 */
template<auto R, typename Rep>
  requires requires(Rep v) { abs(v); }
#if MP_UNITS_HOSTED || __cpp_lib_freestanding_cstdlib >= 202306L
           || requires(Rep v) { std::abs(v); }
#endif
[[nodiscard]] constexpr quantity<R, Rep> abs(const quantity<R, Rep>& q) noexcept
{
#if MP_UNITS_HOSTED || __cpp_lib_freestanding_cstdlib >= 202306L
  using std::abs;
#endif
  return {static_cast<Rep>(abs(q.numerical_value_ref_in(q.unit))), R};
}

/**
 * @brief Computes the value of a quantity raised to the `Num/Den` power
 *
 * Both the quantity value and its quantity specification are the base of the operation.
 *
 * @tparam Num Exponent numerator
 * @tparam Den Exponent denominator
 * @param q Quantity being the base of the operation
 * @return Quantity The result of computation
 */
template<std::intmax_t Num, std::intmax_t Den = 1, auto R, typename Rep>
  requires(Den != 0) && requires(Rep v) {
    representation_values<Rep>::one();
    requires requires { pow(v, 1.0); }
#if MP_UNITS_HOSTED
               || requires { std::pow(v, 1.0); }
#endif
               ;
  }
[[nodiscard]] constexpr quantity<pow<Num, Den>(R), Rep> pow(const quantity<R, Rep>& q) noexcept

{
  if constexpr (Num == 0) {
    return {representation_values<Rep>::one(), pow<0>(R)};
  } else if constexpr (Num == Den) {
    return q;
  } else {
#if MP_UNITS_HOSTED
    using std::pow;
#endif
    return {
      static_cast<Rep>(pow(q.numerical_value_ref_in(q.unit), static_cast<double>(Num) / static_cast<double>(Den))),
      pow<Num, Den>(R)};
  }
}

/**
 * @brief Computes the square root of a quantity
 *
 * Both the quantity value and its quantity specification are the base of the operation.
 *
 * @param q Quantity being the base of the operation
 * @return Quantity The result of computation
 */
template<auto R, typename Rep>
  requires requires(Rep v) { sqrt(v); }
#if MP_UNITS_HOSTED
           || requires(Rep v) { std::sqrt(v); }
#endif
[[nodiscard]] constexpr quantity<sqrt(R), Rep> sqrt(const quantity<R, Rep>& q) noexcept
{
#if MP_UNITS_HOSTED
  using std::sqrt;
#endif
  return {static_cast<Rep>(sqrt(q.numerical_value_ref_in(q.unit))), sqrt(R)};
}

/**
 * @brief Computes the cubic root of a quantity
 *
 * Both the quantity value and its quantity specification are the base of the operation.
 *
 * @param q Quantity being the base of the operation
 * @return Quantity The result of computation
 */
template<auto R, typename Rep>
  requires requires(Rep v) { cbrt(v); }
#if MP_UNITS_HOSTED
           || requires(Rep v) { std::cbrt(v); }
#endif
[[nodiscard]] constexpr quantity<cbrt(R), Rep> cbrt(const quantity<R, Rep>& q) noexcept
{
#if MP_UNITS_HOSTED
  using std::cbrt;
#endif
  return {static_cast<Rep>(cbrt(q.numerical_value_ref_in(q.unit))), cbrt(R)};
}

/**
 * @brief Computes Euler's raised to the given power
 *
 * @note Such an operation has sense only for a dimensionless quantity.
 *
 * @param q Quantity being the base of the operation
 * @return Quantity The value of the same quantity type
 */
template<ReferenceOf<dimensionless> auto R, typename Rep>
  requires requires(Rep v) { exp(v); }
#if MP_UNITS_HOSTED
           || requires(Rep v) { std::exp(v); }
#endif
[[nodiscard]] constexpr quantity<R, Rep> exp(const quantity<R, Rep>& q)
{
#if MP_UNITS_HOSTED
  using std::exp;
#endif
  return value_cast<get_unit(R)>(
    quantity{static_cast<Rep>(exp(q.force_numerical_value_in(q.unit))), detail::clone_reference_with<one>(R)});
}

/**
 * @brief Determines if a quantity is finite.
 *
 * @param a: Quantity to analyze.
 * @return bool: Whether the quantity is finite or not.
 */
template<auto R, typename Rep>
  requires requires(Rep v) { isfinite(v); }
#if MP_UNITS_HOSTED
           || requires(Rep v) { std::isfinite(v); }
#endif
[[nodiscard]] constexpr bool isfinite(const quantity<R, Rep>& a) noexcept
{
#if MP_UNITS_HOSTED
  using std::isfinite;
#endif
  return isfinite(a.numerical_value_ref_in(a.unit));
}

/**
 * @brief Determines if a quantity point is finite.
 *
 * @param a: Quantity point to analyze.
 * @return bool: Whether the quantity point is finite or not.
 */
template<auto R, auto PO, typename Rep>
  requires requires(quantity<R, Rep> q) { isfinite(q); }
[[nodiscard]] constexpr bool isfinite(const quantity_point<R, PO, Rep>& a) noexcept
{
  return isfinite(a.quantity_ref_from(a.point_origin));
}

/**
 * @brief Determines if a quantity is infinite.
 *
 * @param a: Quantity to analyze.
 * @return bool: Whether the quantity is infinite or not.
 */
template<auto R, typename Rep>
  requires requires(Rep v) { isinf(v); }
#if MP_UNITS_HOSTED
           || requires(Rep v) { std::isinf(v); }
#endif
[[nodiscard]] constexpr bool isinf(const quantity<R, Rep>& a) noexcept
{
#if MP_UNITS_HOSTED
  using std::isinf;
#endif
  return isinf(a.numerical_value_ref_in(a.unit));
}

/**
 * @brief Determines if a quantity point is infinite.
 *
 * @param a: Quantity point to analyze.
 * @return bool: Whether the quantity point is infinite or not.
 */
template<auto R, auto PO, typename Rep>
  requires requires(quantity<R, Rep> q) { isinf(q); }
[[nodiscard]] constexpr bool isinf(const quantity_point<R, PO, Rep>& a) noexcept
{
  return isinf(a.quantity_ref_from(a.point_origin));
}


/**
 * @brief Determines if a quantity is a nan.
 *
 * @param a: Quantity to analyze.
 * @return bool: Whether the quantity is a NaN or not.
 */
template<auto R, typename Rep>
  requires requires(Rep v) { isnan(v); }
#if MP_UNITS_HOSTED
           || requires(Rep v) { std::isnan(v); }
#endif
[[nodiscard]] constexpr bool isnan(const quantity<R, Rep>& a) noexcept
{
#if MP_UNITS_HOSTED
  using std::isnan;
#endif
  return isnan(a.numerical_value_ref_in(a.unit));
}


/**
 * @brief Determines if a quantity point is a nan.
 *
 * @param a: Quantity point to analyze.
 * @return bool: Whether the quantity point is a NaN or not.
 */
template<auto R, auto PO, typename Rep>
  requires requires(quantity<R, Rep> q) { isnan(q); }
[[nodiscard]] constexpr bool isnan(const quantity_point<R, PO, Rep>& a) noexcept
{
  return isnan(a.quantity_ref_from(a.point_origin));
}

/**
 * @brief Computes the fma of 3 quantities
 *
 * @param a: Multiplicand
 * @param x: Multiplicand
 * @param b: Addend
 * @return Quantity: The nearest floating point representable to ax+b
 */
template<auto R, auto S, auto T, typename Rep1, typename Rep2, typename Rep3>
  requires requires { get_common_quantity_spec(get_quantity_spec(R) * get_quantity_spec(S), get_quantity_spec(T)); } &&
           (equivalent(get_unit(R) * get_unit(S), get_unit(T))) && requires(Rep1 v1, Rep2 v2, Rep3 v3) {
             requires requires { fma(v1, v2, v3); }
#if MP_UNITS_HOSTED
                        || requires { std::fma(v1, v2, v3); }
#endif
                        ;
           }
[[nodiscard]] constexpr QuantityOf<get_common_quantity_spec(get_quantity_spec(R) * get_quantity_spec(S),
                                                            get_quantity_spec(T))> auto
fma(const quantity<R, Rep1>& a, const quantity<S, Rep2>& x, const quantity<T, Rep3>& b) noexcept
{
#if MP_UNITS_HOSTED
  using std::fma;
#endif
  return quantity{
    fma(a.numerical_value_ref_in(a.unit), x.numerical_value_ref_in(x.unit), b.numerical_value_ref_in(b.unit)),
    get_common_reference(R * S, T)};
}

/**
 * @brief Computes the fma of 2 quantities and a quantity point
 *
 * @param a: Multiplicand
 * @param x: Multiplicand
 * @param b: Addend
 * @return QuantityPoint: The nearest floating point representable to ax+b
 */
template<auto R, auto S, auto T, auto Origin, typename Rep1, typename Rep2, typename Rep3>
  requires requires { get_common_quantity_spec(get_quantity_spec(R) * get_quantity_spec(S), get_quantity_spec(T)); } &&
           (equivalent(get_unit(R) * get_unit(S), get_unit(T))) && requires(Rep1 v1, Rep2 v2, Rep3 v3) {
             requires requires { fma(v1, v2, v3); }
#if MP_UNITS_HOSTED
                        || requires { std::fma(v1, v2, v3); }
#endif
                        ;
           }
[[nodiscard]] constexpr QuantityPointOf<get_common_quantity_spec(get_quantity_spec(R) * get_quantity_spec(S),
                                                                 get_quantity_spec(T))> auto
fma(const quantity<R, Rep1>& a, const quantity<S, Rep2>& x, const quantity_point<T, Origin, Rep3>& b) noexcept
{
#if MP_UNITS_HOSTED
  using std::fma;
#endif
  return Origin + quantity{fma(a.numerical_value_ref_in(a.unit), x.numerical_value_ref_in(x.unit),
                               b.quantity_ref_from(b.point_origin).numerical_value_ref_in(b.unit)),
                           get_common_reference(R * S, T)};
}

/**
 * @brief Computes the floating-point remainder of the division operation x / y.
 */
template<auto R1, typename Rep1, auto R2, typename Rep2>
  requires requires(Rep1 v1, Rep2 v2) {
    get_common_reference(R1, R2);
    requires requires { fmod(v1, v2); }
#if MP_UNITS_HOSTED
               || requires { std::fmod(v1, v2); }
#endif
               ;
  }
[[nodiscard]] constexpr QuantityOf<get_quantity_spec(R1)> auto fmod(const quantity<R1, Rep1>& x,
                                                                    const quantity<R2, Rep2>& y) noexcept
{
  constexpr auto ref = get_common_reference(R1, R2);
  constexpr auto unit = get_unit(ref);
#if MP_UNITS_HOSTED
  using std::fmod;
#endif
  return quantity{fmod(x.numerical_value_in(unit), y.numerical_value_in(unit)), ref};
}

/**
 * @brief Computes the IEEE remainder of the floating point division operation x / y.
 */
template<auto R1, typename Rep1, auto R2, typename Rep2>
  requires requires(Rep1 v1, Rep2 v2) {
    get_common_reference(R1, R2);
    requires requires { remainder(v1, v2); }
#if MP_UNITS_HOSTED
               || requires { std::remainder(v1, v2); }
#endif
               ;
  }
[[nodiscard]] constexpr QuantityOf<get_quantity_spec(R1)> auto remainder(const quantity<R1, Rep1>& x,
                                                                         const quantity<R2, Rep2>& y) noexcept
{
  constexpr auto ref = get_common_reference(R1, R2);
  constexpr auto unit = get_unit(ref);
#if MP_UNITS_HOSTED
  using std::remainder;
#endif
  return quantity{remainder(x.numerical_value_in(unit), y.numerical_value_in(unit)), ref};
}


/**
 * @brief Returns the epsilon of the quantity
 *
 * The returned value is defined by a <tt>std::numeric_limits<typename Q::rep>::epsilon()</tt>.
 *
 * @tparam Q Quantity type being the base of the operation
 * @return Quantity The epsilon value for quantity's representation type
 */
template<typename Rep, Reference R>
  requires RepresentationOf<Rep, get_quantity_spec(R{})> && requires { std::numeric_limits<Rep>::epsilon(); }
[[deprecated(
  "2.5.0: Use `std::numeric_limits<Quantity>::epsilon()` instead")]] [[nodiscard]] constexpr quantity<R{}, Rep>
  epsilon(R r) noexcept
{
  return {static_cast<Rep>(std::numeric_limits<Rep>::epsilon()), r};
}

/**
 * @brief Computes the largest quantity with integer representation and unit type To with its number not greater than q
 *
 * @tparam q Quantity being the base of the operation
 * @return Quantity The rounded quantity with unit type To
 */
template<Unit auto To, auto R, typename Rep>
[[nodiscard]] constexpr quantity<detail::clone_reference_with<To>(R), Rep> floor(const quantity<R, Rep>& q) noexcept
  requires requires { q.force_in(To); } &&
           ((treat_as_floating_point<Rep> && (requires(Rep v) { floor(v); }
#if MP_UNITS_HOSTED
                                              || requires(Rep v) { std::floor(v); }
#endif
                                              )) ||
            (!treat_as_floating_point<Rep> && requires { representation_values<Rep>::one(); }))
{
  const quantity res = q.force_in(To);
  if constexpr (treat_as_floating_point<Rep>) {
#if MP_UNITS_HOSTED
    using std::floor;
#endif
    return {static_cast<Rep>(floor(res.numerical_value_ref_in(res.unit))), res.reference};
  } else {
    if (res > q) return res - quantity{representation_values<Rep>::one(), res.reference};
    return res;
  }
}

/**
 * @brief Computes the smallest quantity with integer representation and unit type To with its number not less than q
 *
 * @tparam q Quantity being the base of the operation
 * @return Quantity The rounded quantity with unit type To
 */
template<Unit auto To, auto R, typename Rep>
[[nodiscard]] constexpr quantity<detail::clone_reference_with<To>(R), Rep> ceil(const quantity<R, Rep>& q) noexcept
  requires requires { q.force_in(To); } &&
           ((treat_as_floating_point<Rep> && (requires(Rep v) { ceil(v); }
#if MP_UNITS_HOSTED
                                              || requires(Rep v) { std::ceil(v); }
#endif
                                              )) ||
            (!treat_as_floating_point<Rep> && requires { representation_values<Rep>::one(); }))
{
  const quantity res = q.force_in(To);
  if constexpr (treat_as_floating_point<Rep>) {
#if MP_UNITS_HOSTED
    using std::ceil;
#endif
    return {static_cast<Rep>(ceil(res.numerical_value_ref_in(res.unit))), res.reference};
  } else {
    if (res < q) return res + quantity{representation_values<Rep>::one(), res.reference};
    return res;
  }
}

/**
 * @brief Computes the nearest quantity with integer representation and unit type `To` to `q`
 *
 * Returns the value `res` representable in `To` unit that is the closest to `q`. If there are two
 * such values, returns the even value (that is, the value `res` such that `res % 2 == 0`).
 *
 * @tparam q Quantity being the base of the operation
 * @return Quantity The quantity rounded to the nearest unit `To`, rounding to even in halfway
 *                  cases.
 */
template<Unit auto To, auto R, typename Rep>
[[nodiscard]] constexpr quantity<detail::clone_reference_with<To>(R), Rep> round(const quantity<R, Rep>& q) noexcept
  requires requires {
    mp_units::floor<To>(q);
    representation_values<Rep>::one();
  } && std::constructible_from<std::int64_t, Rep>
{
  const auto res_low = mp_units::floor<To>(q);
  const auto res_high = res_low + quantity{representation_values<Rep>::one(), res_low.reference};
  const auto diff0 = q - res_low;
  const auto diff1 = res_high - q;
  if (diff0 == diff1) {
    // TODO How to extend this to custom representation types?
    if (static_cast<std::int64_t>(res_low.numerical_value_ref_in(To)) & 1) return res_high;
    return res_low;
  } else if (diff0 < diff1)
    return res_low;
  return res_high;
}

/**
 * @brief Computes the inverse of a quantity in a provided unit
 */
template<Unit auto To, auto R, typename Rep>
[[nodiscard]] constexpr Quantity auto inverse(const quantity<R, Rep>& q)
  requires requires {
    representation_values<Rep>::one();
    value_cast<To>(representation_values<Rep>::one() / q);
  }
{
  constexpr QuantitySpec auto spec = get_quantity_spec(To) * quantity<R, Rep>::quantity_spec;
  constexpr Unit auto unit = To * quantity<R, Rep>::unit;
  return spec(representation_values<Rep>::one() * one).force_in(unit) / q;
}

/**
 * @brief Computes the square root of the sum of the squares of x and y,
 *        without undue overflow or underflow at intermediate stages of the computation
 */
template<auto R1, typename Rep1, auto R2, typename Rep2>
  requires requires(Rep1 v1, Rep2 v2) {
    get_common_reference(R1, R2);
    requires requires { hypot(v1, v2); }
#if MP_UNITS_HOSTED
               || requires { std::hypot(v1, v2); }
#endif
               ;
  }
[[nodiscard]] constexpr QuantityOf<get_quantity_spec(get_common_reference(R1, R2))> auto hypot(
  const quantity<R1, Rep1>& x, const quantity<R2, Rep2>& y) noexcept
{
  constexpr auto ref = get_common_reference(R1, R2);
  constexpr auto unit = get_unit(ref);
#if MP_UNITS_HOSTED
  using std::hypot;
#endif
  return quantity{hypot(x.numerical_value_in(unit), y.numerical_value_in(unit)), ref};
}

/**
 * @brief Computes the square root of the sum of the squares of x, y, and z,
 *        without undue overflow or underflow at intermediate stages of the computation
 */
template<auto R1, typename Rep1, auto R2, typename Rep2, auto R3, typename Rep3>
  requires requires(Rep1 v1, Rep2 v2, Rep3 v3) {
    get_common_reference(R1, R2, R3);
    requires requires { hypot(v1, v2, v3); }
#if MP_UNITS_HOSTED
               || requires { std::hypot(v1, v2, v3); }
#endif
               ;
  }
[[nodiscard]] constexpr QuantityOf<get_quantity_spec(get_common_reference(R1, R2, R3))> auto hypot(
  const quantity<R1, Rep1>& x, const quantity<R2, Rep2>& y, const quantity<R3, Rep3>& z) noexcept
{
  constexpr auto ref = get_common_reference(R1, R2);
  constexpr auto unit = get_unit(ref);
#if MP_UNITS_HOSTED
  using std::hypot;
#endif
  return quantity{hypot(x.numerical_value_in(unit), y.numerical_value_in(unit), z.numerical_value_in(unit)), ref};
}

/**
 * @brief Linear interpolation or extrapolation
 *
 * Computes the linear interpolation between `a` and `b`, if the parameter `t` is inside `[​0​, 1)`
 * (the linear extrapolation otherwise), i.e. the result of `a + t(b − a)` with accounting for
 * floating-point calculation imprecision.
 */
template<auto R1, auto Origin, typename Rep1, auto R2, typename Rep2, typename Factor>
  requires requires(Rep1 a, Rep2 b, Factor t) {
    get_common_reference(R1, R2);
    requires requires { lerp(a, b, t); }
#if MP_UNITS_HOSTED
               || requires { std::lerp(a, b, t); }
#endif
               ;
  }
[[nodiscard]] constexpr QuantityPointOf<get_quantity_spec(get_common_reference(R1, R2))> auto lerp(
  const quantity_point<R1, Origin, Rep1>& a, const quantity_point<R2, Origin, Rep2>& b, const Factor& t) noexcept
{
  constexpr auto ref = get_common_reference(R1, R2);
  constexpr auto unit = get_unit(ref);
#if MP_UNITS_HOSTED
  using std::lerp;
#endif
  return Origin + quantity{lerp(a.quantity_ref_from(Origin).numerical_value_in(unit),
                                b.quantity_ref_from(Origin).numerical_value_in(unit), t),
                           ref};
}

/**
 * @brief Computes the midpoint of two points
 */
template<auto R1, auto Origin, typename Rep1, auto R2, typename Rep2>
  requires requires(Rep1 a, Rep2 b) {
    get_common_reference(R1, R2);
    requires requires { midpoint(a, b); }
#if MP_UNITS_HOSTED || __cpp_lib_freestanding_numeric >= 202502L
               || requires { std::midpoint(a, b); }
#endif
               ;
  }
[[nodiscard]] constexpr QuantityPointOf<get_quantity_spec(get_common_reference(R1, R2))> auto midpoint(
  const quantity_point<R1, Origin, Rep1>& a, const quantity_point<R2, Origin, Rep2>& b) noexcept
{
  constexpr auto ref = get_common_reference(R1, R2);
  constexpr auto unit = get_unit(ref);
#if MP_UNITS_HOSTED || __cpp_lib_freestanding_numeric >= 202502L
  using std::midpoint;
#endif
  return Origin + quantity{midpoint(a.quantity_ref_from(Origin).numerical_value_in(unit),
                                    b.quantity_ref_from(Origin).numerical_value_in(unit)),
                           ref};
}

}  // namespace mp_units
