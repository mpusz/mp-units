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
#include <mp-units/ext/contracts.h>
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/quantity_concepts.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <concepts>
#endif
#endif

namespace mp_units {

// ============================================================================
// Bounds-checking and value-transformation policies
//
// Each policy is a class template parameterised on a quantity type Q.
// It stores the [min, max] bounds and provides operator()(V v) that
// enforces those bounds on a quantity of compatible type.
//
// Available policies:
//   1. check_in_range       - Error reporting via constraint_violation_handler or MP_UNITS_EXPECTS
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
// ============================================================================

/**
 * @brief Policy that checks the value is within [min, max] and reports violations.
 *
 * If the quantity's representation type has a `constraint_violation_handler` specialization,
 * the handler's `on_violation()` is called on out-of-bounds values (providing guaranteed
 * enforcement regardless of build mode). Otherwise, falls back to `MP_UNITS_EXPECTS`,
 * which may be disabled in release builds.
 *
 * Example:
 * @code{cpp}
 * // With constrained<double, throw_policy> rep → throws std::domain_error on violation
 * // With plain double rep → asserts via MP_UNITS_EXPECTS (may be no-op in release)
 * inline constexpr struct equator :
 *     absolute_point_origin<geo_latitude, check_in_range{-90 * deg, 90 * deg}> {} equator;
 * @endcode
 */
MP_UNITS_EXPORT template<Quantity Q>
struct check_in_range {
  Q min;
  Q max;

  template<Quantity V>
  constexpr V operator()(V v) const
  {
    const V vmin{min};
    const V vmax{max};
    if constexpr (detail::HasConstraintViolationHandler<typename V::rep>) {
      if (v < vmin || v > vmax) constraint_violation_handler<typename V::rep>::on_violation("value out of bounds");
    } else {
      MP_UNITS_EXPECTS(v >= vmin && v <= vmax);
    }
    return v;
  }
};

#if MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG < 17

template<Quantity Q>
check_in_range(Q, Q) -> check_in_range<Q>;

#endif

/**
 * @brief Policy that clamps the value to [min, max].
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

#if MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG < 17

template<Quantity Q>
clamp_to_range(Q, Q) -> clamp_to_range<Q>;

#endif

/**
 * @brief Policy that wraps the value into the half-open range [min, max).
 *
 * Uses modulo arithmetic to wrap values into the range.
 * Use for periodic/cyclic quantities (angles, time-of-day, etc.).
 * For example, with [0°, 360°): 370° -> 10°, -10° -> 350°.
 */
MP_UNITS_EXPORT template<Quantity Q>
struct wrap_to_range {
  Q min;
  Q max;

  template<Quantity V>
  constexpr V operator()(V v) const
  {
    const V vmin{min};
    const V vmax{max};
    const quantity range = vmax - vmin;
    while (v >= vmax) v -= range;
    while (v < vmin) v += range;
    return v;
  }
};

#if MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG < 17

template<Quantity Q>
wrap_to_range(Q, Q) -> wrap_to_range<Q>;

#endif

/**
 * @brief Policy that reflects (folds) the value at both boundaries.
 *
 * Values that exceed [min, max] are "bounced back" from the boundary.
 * For example, with [-90, 90] (latitude): 91 -> 89, 180 -> 0, 270 -> -90.
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
    const quantity range = vmax - vmin;
    const quantity period = V{2 * range};
    while (v >= V{vmin + period}) v -= period;
    while (v < vmin) v += period;
    if (v > vmax) v = V{2 * vmax} - v;
    return v;
  }
};

#if MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG < 17

template<Quantity Q>
reflect_in_range(Q, Q) -> reflect_in_range<Q>;

#endif

namespace detail {

/**
 * @brief Sentinel for the lower domain bound of a half-line [0, ∞) policy.
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
 * @brief Policy that checks the value is ≥ 0 and reports violations.
 *
 * Intended for quantities that are inherently non-negative (e.g., _length_, _mass_,
 * _duration_). Automatically applied to `natural_point_origin<QS>` when `QS` is tagged
 * `non_negative` in the ISQ.
 *
 * If the quantity's representation type has a `constraint_violation_handler` specialization,
 * the handler's `on_violation()` is called on negative values (providing guaranteed
 * enforcement regardless of build mode). Otherwise, falls back to `MP_UNITS_EXPECTS`,
 * which may be disabled in release builds.
 */
MP_UNITS_EXPORT struct check_non_negative {
  // Lower domain bound: zero in any unit.  Consumed by quantity_point::min() and
  // std::numeric_limits<quantity_point>::lowest().  The operator() uses V::zero() directly
  // (no unit scaling needed); this member exists solely for the numeric_limits interface.
  detail::zero_quantity_t min;

  template<Quantity V>
  constexpr V operator()(V v) const
  {
    const V vzero{V::zero()};
    if constexpr (detail::HasConstraintViolationHandler<typename V::rep>) {
      if (v < vzero) constraint_violation_handler<typename V::rep>::on_violation("value must be non-negative");
    } else {
      MP_UNITS_EXPECTS(v >= vzero);
    }
    return v;
  }
};

/**
 * @brief Policy that clamps the value to [0, ∞).
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
