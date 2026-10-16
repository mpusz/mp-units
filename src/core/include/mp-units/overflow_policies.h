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

#include <mp-units/bits/config.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/bits/unsatisfied.h>
#include <mp-units/ext/contracts.h>
#include <mp-units/ext/type_traits.h>
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/quantity_concepts.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <concepts>
#include <type_traits>
#include <utility>
#endif
#endif

namespace mp_units {

/**
 * Marks a range bound as excluded from the range.
 *
 * A plain quantity passed as `min` or `max` of a bounds policy is included in the range.
 * `check_in_range{0 * deg, exclusive_bound{360 * deg}}` checks `[0°, 360°)`. Only the policies
 * that can honor an excluded end accept it: @ref check_in_range at either end, and
 * @ref wrap_to_range at its `max`, which that policy excludes anyway.
 */
MP_UNITS_EXPORT template<Quantity Q>
struct exclusive_bound {
  Q value;
};

#if !MP_UNITS_HAS_AGGREGATE_CTAD

template<Quantity Q>
exclusive_bound(Q) -> exclusive_bound<Q>;

#endif

namespace detail {

template<typename T>
constexpr bool is_exclusive_bound = is_specialization_of<T, exclusive_bound>;

// The quantity a range end holds.
template<typename B>
struct bound_value_type {
  using type = B;
};

template<typename Q>
struct bound_value_type<exclusive_bound<Q>> {
  using type = Q;
};

template<typename B>
using bound_value_t = bound_value_type<B>::type;

// A range end: a quantity (included) or an exclusive_bound (excluded).
template<typename T>
concept RangeBound = Quantity<T> || is_exclusive_bound<T>;

template<typename B>
[[nodiscard]] constexpr const auto& bound_value(const B& bound)
{
  if constexpr (is_exclusive_bound<B>)
    return bound.value;
  else
    return bound;
}

struct zero_quantity_t;

// Re-expresses a policy bound in the type of the checked value V.
template<Quantity V, typename B>
[[nodiscard]] constexpr V bound_in(const B& bound)
{
  if constexpr (std::same_as<B, zero_quantity_t>)
    return V::zero();
  else
    return V{bound_value(bound)};
}

template<Quantity V, typename B>
[[nodiscard]] constexpr bool above_lower_bound(const V& v, const B& bound)
{
  if constexpr (is_exclusive_bound<B>)
    return v > bound_in<V>(bound);
  else
    return v >= bound_in<V>(bound);
}

template<Quantity V, typename B>
[[nodiscard]] constexpr bool below_upper_bound(const V& v, const B& bound)
{
  if constexpr (is_exclusive_bound<B>)
    return v < bound_in<V>(bound);
  else
    return v <= bound_in<V>(bound);
}

// The test of `check_in_range` and `check_non_negative`: v lies between min and max, each end
// included unless it is an exclusive_bound, and a missing bound leaving that side unbounded. It is
// written as a positive test so that an unordered value (NaN) is outside.
template<typename Policy, Quantity V>
[[nodiscard]] constexpr bool within_bounds(const Policy& policy, const V& v)
{
  if constexpr (requires { policy.min; } && requires { policy.max; })
    return above_lower_bound(v, policy.min) && below_upper_bound(v, policy.max);
  else if constexpr (requires { policy.min; })
    return above_lower_bound(v, policy.min);
  else
    return below_upper_bound(v, policy.max);
}

// True iff Policy is a checking policy for V, i.e. its operator() returns exactly `bool`.
// apply_bounds_policy treats any other policy as an adjusting one and requires it to return a quantity.
template<typename Policy, typename V>
concept CheckingPolicyFor = requires(const Policy& policy, const V& v) {
  { policy(v) } -> std::same_as<bool>;
};

// Applies a bounds policy to v. A checking policy's rejection is reported through the
// representation's constraint_violation_handler, and v is returned unchanged. Without a handler,
// a value the policy rejects violates the precondition of the public operation storing it, which
// that operation states and checks itself. An adjusting policy's result is returned.
template<typename Policy, Quantity V>
constexpr V apply_bounds_policy(const Policy& policy, V v)
{
  if constexpr (CheckingPolicyFor<Policy, V>) {
    if constexpr (HasConstraintViolationHandler<typename V::rep>)
      if (!policy(v)) constraint_violation_handler<typename V::rep>::on_violation("value out of bounds");
    return v;
  } else {
    // Validated like the nesting of an origin's bounds: a misconfigured origin is reported when a
    // value is first checked against it.
    static_assert(
      requires {
        { policy(v) } -> Quantity;
      }, "a bounds policy's operator() must return bool (checking) or a quantity (adjusting)");
    return policy(v);
  }
}

// Reduces a non-negative diff modulo a positive step in O(log) subtractions of step * 2^k, each of
// which is exact for an IEEE floating-point representation and cannot overflow an integral one.
// Subtracting step itself would never terminate once diff is so large that diff - step == diff.
template<Quantity V>
[[nodiscard]] constexpr V reduce_modulo(V diff, const V& step)
{
  while (diff >= step) {
    V chunk = step;
    while (chunk <= diff - chunk) chunk += chunk;
    diff -= chunk;
  }
  return diff;
}

// (x - origin) modulo a positive modulus, in [0, modulus), for a representation with an exact
// remainder (an integral one), without forming x - origin, which can overflow near the
// representation's limits. Each remainder lies in (-modulus, modulus) and their difference in
// (-modulus, modulus), so no intermediate value exceeds the modulus.
template<Quantity V>
[[nodiscard]] constexpr V offset_modulo(const V& x, const V& origin, const V& modulus)
{
  const auto non_negative_remainder = [&](const V& value) {
    const V remainder{value % modulus};
    return remainder < V::zero() ? V{remainder + modulus} : remainder;
  };
  const V diff{non_negative_remainder(x) - non_negative_remainder(origin)};
  return diff < V::zero() ? V{diff + modulus} : diff;
}

// True for an infinite or NaN floating-point value: v - v is NaN exactly then, and always zero for
// an integral representation.
template<Quantity V>
[[nodiscard]] constexpr bool is_non_finite(const V& v)
{
  return !(v - v == v - v);
}

}  // namespace detail

// ============================================================================
// Bounds-checking and value-transformation policies
//
// Each policy stores its bounds as `min` and/or `max`. A checking policy's operator() returns `bool`
// and the library reports a value it rejects (see apply_bounds_policy above). An adjusting policy's
// operator() returns the value to store. "Available Overflow Policies" in
// docs/users_guide/framework_basics/the_affine_space.md describes the full interface.
//
// Available policies:
//   1. check_in_range       - Rejection goes to constraint_violation_handler or violates a precondition
//   2. clamp_to_range       - Saturate to boundaries (error correction)
//   3. wrap_to_range        - Modulo wrapping to [min, max)
//   4. reflect_in_range     - Bounce/fold at boundaries (physics)
//   5. check_non_negative   - Error reporting for values < 0 (for inherently non-negative quantities)
//   6. clamp_non_negative   - Saturate to 0 for values < 0 (tolerates floating-point noise)
//
// When to use:
//   - Use check_in_range for bounds-checked points (error behavior depends on the rep type)
//   - Use clamp_to_range when you want to "correct" out-of-range values
//   - Use wrap_to_range for periodic/cyclic values (angles, hours)
//   - Use reflect_in_range for physical boundaries (latitude, bouncing particles)
//   - Use check_non_negative for quantities that are tagged non_negative in the ISQ
//     (automatically applied to natural_point_origin<QS> when QS is non_negative)
//   - Use clamp_non_negative when negative values can legitimately arise from floating-point
//     rounding in a non-negative domain (e.g., tiny negative energy from numerical noise)
//   Note: reflect_non_negative and wrap_non_negative are not provided — [0, ∞) has no
//   upper bound, so neither reflection nor wrapping is physically well-defined.
//
// `is_within_bounds()` and `quantity_point::try_make()` are offered for points whose enforced
// policy is a checking one. They test a value with the predicate that construction applies.
// ============================================================================

/**
 * @brief Policy that tests whether the value lies between `min` and `max`.
 *
 * Each end is included unless it is given as @ref exclusive_bound.
 *
 * A checking policy, so the value is stored unchanged. When the representation has a
 * `constraint_violation_handler` specialization, the library calls its `on_violation()` for a
 * rejected value in every build mode. Otherwise, a rejected value violates the precondition of the
 * operation that stores it.
 *
 * Example:
 * @code{cpp}
 * // With constrained<double, throw_policy> rep → throws std::domain_error on violation
 * // With plain double rep → a value outside [-90°, 90°] violates a precondition
 * inline constexpr struct equator :
 *     absolute_point_origin<geo_latitude, check_in_range{-90 * deg, 90 * deg}> {} equator;
 * @endcode
 */
MP_UNITS_EXPORT template<detail::RangeBound Lo, detail::RangeBound Hi>
struct check_in_range {
  Lo min;
  Hi max;

  /**
   * Tells whether `v` lies between `min` and `max`, each included unless given as
   * @ref exclusive_bound. Returns `false` for NaN.
   *
   * @see @ref is_within_bounds for the test applied before constructing a point
   */
  template<Quantity V>
  [[nodiscard]] constexpr bool operator()(const V& v) const
  {
    return detail::within_bounds(*this, v);
  }
};

#if !MP_UNITS_HAS_AGGREGATE_CTAD

template<detail::RangeBound Lo, detail::RangeBound Hi>
check_in_range(Lo, Hi) -> check_in_range<Lo, Hi>;

#endif

/**
 * @brief Policy that clamps the value to `[min, max]`.
 *
 * Saturates out-of-range values to the nearest boundary.
 * Use when you want to "correct" invalid values rather than signal an error.
 */
MP_UNITS_EXPORT template<Quantity Q>
struct clamp_to_range {
  Q min;
  Q max;

  template<Quantity V>
  constexpr V operator()(V v) const
  {
    const V vmin{min};
    const V vmax{max};
    if (v < vmin) return vmin;
    if (v > vmax) return vmax;
    return v;
  }
};

// Declared on every compiler: the rejecting guide below would otherwise suppress aggregate CTAD.
template<Quantity Q>
clamp_to_range(Q, Q) -> clamp_to_range<Q>;

template<detail::RangeBound Lo, detail::RangeBound Hi>
  requires(detail::is_exclusive_bound<Lo> || detail::is_exclusive_bound<Hi>) &&
          (detail::unsatisfied<"clamp_to_range cannot clamp to an excluded bound">())
clamp_to_range(Lo, Hi) -> clamp_to_range<detail::bound_value_t<Lo>>;

/**
 * @brief Policy that wraps the value into the half-open range `[min, max)`.
 *
 * Uses modulo arithmetic to wrap values into the range.
 * Use for periodic/cyclic quantities (angles, time-of-day, etc.).
 * For example, with `[0°, 360°)`: 370° -> 10°, -10° -> 350°.
 *
 * `max` may also be given as @ref exclusive_bound. Since it is excluded either way,
 * `quantity_point::max()` is not offered for a point with this policy.
 */
MP_UNITS_EXPORT template<Quantity Q>
struct wrap_to_range {
  Q min;
  exclusive_bound<Q> max;  // a wrapped value never takes max, so it is always excluded

  // max may be given plain or as exclusive_bound; it is excluded either way.
  constexpr wrap_to_range(Q min_, Q max_) : min(min_), max{max_} {}
  constexpr wrap_to_range(Q min_, exclusive_bound<Q> max_) : min(min_), max(max_) {}

  template<Quantity V>
  constexpr V operator()(V v) const
  {
    const V vmin{min};
    const V vmax{max.value};
    if (detail::is_non_finite(v)) return v - v;  // no position on the circle: NaN
    if (v >= vmin && v < vmax) return v;
    const V range = vmax - vmin;
    if constexpr (requires { v % range; }) {
      // An integral representation takes the exact remainder, which never overflows.
      return V{vmin + detail::offset_modulo(v, vmin, range)};
    } else {
      if (v >= vmax)
        v = vmin + detail::reduce_modulo(V{v - vmin}, range);
      else {
        const V back = detail::reduce_modulo(V{vmin - v}, range);
        v = back == V::zero() ? vmin : V{vmax - back};
      }
      // A floating-point result that rounds onto the excluded max is the same point as min.
      return v < vmax ? v : vmin;
    }
  }
};

template<detail::RangeBound Lo, detail::RangeBound Hi>
  requires detail::is_exclusive_bound<Lo> &&
           (detail::unsatisfied<"wrap_to_range cannot exclude its min, which a wrapped value can take">())
wrap_to_range(Lo, Hi) -> wrap_to_range<detail::bound_value_t<Lo>>;

/**
 * @brief Policy that reflects (folds) the value at both boundaries.
 *
 * Values that exceed `[min, max]` are "bounced back" from the boundary.
 * For example, with `[-90, 90]` for a latitude: 91 -> 89, 180 -> 0, 270 -> -90.
 */
MP_UNITS_EXPORT template<Quantity Q>
struct reflect_in_range {
  Q min;
  Q max;

  template<Quantity V>
  constexpr V operator()(V v) const
  {
    const V vmin{min};
    const V vmax{max};
    if (detail::is_non_finite(v)) return v - v;  // no reflected position: NaN
    if (v >= vmin && v <= vmax) return v;
    const V range = vmax - vmin;
    const V period{2 * range};
    if constexpr (requires { v % period; }) {
      // An integral representation takes the exact remainder, which never overflows.
      V offset = detail::offset_modulo(v, vmin, period);
      if (offset > range) offset = period - offset;
      return V{vmin + offset};
    } else {
      // Position within one period [0, 2 range), then folded back onto [0, range].
      V pos = v >= vmin ? detail::reduce_modulo(V{v - vmin}, period) : detail::reduce_modulo(V{vmin - v}, period);
      if (v < vmin && pos != V::zero()) pos = period - pos;
      if (pos > range) pos = period - pos;
      v = vmin + pos;
      // Guard against a floating-point sum landing outside by rounding.
      if (v < vmin) return vmin;
      if (v > vmax) return vmax;
      return v;
    }
  }
};

// Declared on every compiler: the rejecting guide below would otherwise suppress aggregate CTAD.
template<Quantity Q>
reflect_in_range(Q, Q) -> reflect_in_range<Q>;

template<detail::RangeBound Lo, detail::RangeBound Hi>
  requires(detail::is_exclusive_bound<Lo> || detail::is_exclusive_bound<Hi>) &&
          (detail::unsatisfied<"reflect_in_range cannot reflect at an excluded bound">())
reflect_in_range(Lo, Hi) -> reflect_in_range<detail::bound_value_t<Lo>>;

namespace detail {

/**
 * @brief Sentinel for the lower domain bound of a half-line `[0, ∞)` policy.
 *
 * Used as the `.min` member of `check_non_negative` and `clamp_non_negative` so that
 * `quantity_point::min()` and `std::numeric_limits<quantity_point>::lowest()` return
 * the natural zero of that quantity type.  `quantity_point::min()` detects this type
 * and uses `quantity_type::zero()` directly — no unit scaling, correct quantity spec
 * automatically inferred from the `quantity_point` instantiation.
 *
 * Two auxiliary operators are provided for the compile-time bounds-nesting check in
 * `quantity_point.h` (assertions that a relative origin's bounds nest inside its parent's):
 *   - `zero_quantity_t + q = q` (zero is the additive identity)
 *   - `q >= zero_quantity_t` ≡ `q >= Q::zero()` (q is non-negative)
 */
struct zero_quantity_t {
  // zero + q = q  (additive identity, for nesting-check arithmetic)
  template<Quantity Q>
  [[nodiscard]] friend constexpr Q operator+(zero_quantity_t, Q q) noexcept
  {
    return q;
  }

  template<Quantity Q>
  [[nodiscard]] friend constexpr Q operator+(Q q, zero_quantity_t) noexcept
  {
    return q;
  }

  template<Quantity Q>
  [[nodiscard]] friend constexpr Q operator-(zero_quantity_t, Q q) noexcept
  {
    return -q;
  }

  template<Quantity Q>
  [[nodiscard]] friend constexpr Q operator-(Q q, zero_quantity_t) noexcept
  {
    return q;
  }

  template<Quantity Q>
  [[nodiscard]] friend constexpr auto operator==(Q q, zero_quantity_t) noexcept
  {
    return q == Q::zero();
  }

  template<Quantity Q>
  [[nodiscard]] friend constexpr auto operator<=>(Q q, zero_quantity_t) noexcept
  {
    return q <=> Q::zero();
  }
};

}  // namespace detail

/**
 * @brief Policy that tests whether the value is ≥ 0.
 *
 * Intended for quantities that are inherently non-negative (e.g., _length_, _mass_,
 * _duration_). Automatically applied to `natural_point_origin<QS>` when `QS` is tagged
 * `non_negative` in the ISQ.
 *
 * A checking policy like @ref check_in_range. It only tests the value. A rejected value goes to the
 * representation's `constraint_violation_handler` or, when there is none, violates the precondition
 * of the operation that stores it.
 */
MP_UNITS_EXPORT struct check_non_negative {
  // Lower domain bound: zero in any unit.  Consumed by quantity_point::min() and
  // std::numeric_limits<quantity_point>::lowest(), and by the test of operator() as the zero of
  // the checked quantity type.
  detail::zero_quantity_t min;

  /**
   * Tells whether `v` is zero or positive. Returns `false` for NaN.
   *
   * @see @ref is_within_bounds for the test applied before constructing a point
   */
  template<Quantity V>
  [[nodiscard]] constexpr bool operator()(const V& v) const
  {
    return detail::within_bounds(*this, v);
  }
};

/**
 * @brief Policy that clamps the value to `[0, ∞)`.
 *
 * Saturates negative values to zero. Use when small negative values can arise from
 * floating-point rounding in a naturally non-negative domain (e.g., a computed energy
 * that rounds to −1e−15 J should be treated as 0 J rather than signalling an error).
 */
MP_UNITS_EXPORT struct clamp_non_negative {
  // Lower domain bound: zero in any unit.  See check_non_negative::min for rationale.
  detail::zero_quantity_t min;

  template<Quantity V>
  constexpr V operator()(V v) const
  {
    const V vzero{V::zero()};
    return v < vzero ? vzero : v;
  }
};

}  // namespace mp_units
