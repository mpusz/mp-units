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
#include <mp-units/bits/config.h>
#include <mp-units/bits/hacks.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/compat_macros.h>
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/quantity.h>
#include <mp-units/framework/quantity_point_concepts.h>
#include <mp-units/framework/rounding.h>
#include <mp-units/overflow_policies.h>
#if MP_UNITS_HOSTED
#include <mp-units/bits/ostream.h>
#endif

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <bit>
#include <compare>  // IWYU pragma: export
#include <cstdint>
#include <limits>
#include <type_traits>
#if MP_UNITS_HAS_EXPECTED
#include <expected>
#endif
#endif
#endif

namespace mp_units {

namespace detail {

template<PointOrigin PO>
constexpr bool is_specialization_of_natural_point_origin = false;

template<PointOrigin PO>
[[nodiscard]] consteval bool is_natural_point_origin(PO)
{
  return is_specialization_of_natural_point_origin<PO>;
}

template<PointOrigin PO>
[[deprecated("2.6.0: Use `is_natural_point_origin` instead")]] [[nodiscard]] consteval bool is_zeroth_point_origin(
  PO po)
{
  return is_natural_point_origin(po);
}

template<typename FwdQ, PointOrigin PO, Quantity Q = std::remove_cvref_t<FwdQ>>
  requires PointOriginFor<PO, Q::quantity_spec>
[[nodiscard]] constexpr QuantityPoint auto make_quantity_point(FwdQ&& q, PO po)
{
  if constexpr (detail::is_natural_point_origin(PO{}))
    return quantity_point{std::forward<FwdQ>(q)};
  else
    return quantity_point{std::forward<FwdQ>(q), po};
}

// Re-expresses a compile-time origin displacement in its "origin displacement unit": the coarsest
// sub-multiple of the coherent (reference) unit in which the displacement is still represented
// exactly. Feeding this into the common-unit machinery instead of the unit an origin happens to be
// stored in produces the most efficient common unit for point differences (e.g. the offset of the
// Celsius origin stored as `273150 mK` is re-expressed as `5463 (1/20 K)`, so that a temperature
// point difference is no longer forced down to millikelvins). Displacements that are already an
// integer number of the coherent unit are left untouched (e.g. a `42 m` origin offset stays `42 m`).
// The displacement between two origins is a compile-time constant, so it is passed as an NTTP.
template<Quantity auto Q>
[[nodiscard]] consteval Quantity auto coarsen_origin_displacement()
{
  constexpr auto v = Q.numerical_value_in(Q.unit);
  // A zero displacement has no magnitude to derive a unit from, and a non-integral value cannot seed
  // a magnitude; in both cases keep the displacement as stored.
  if constexpr (!std::integral<decltype(v)> || v == 0)
    return Q;
  else {
    constexpr auto canonical = get_canonical_unit(Q.unit);
    constexpr auto value_in_coherent_unit = mag<detail::abs(v)> * canonical.mag;
    return Q.in(pow<-1>(denominator(value_in_coherent_unit)) * canonical.reference_unit, truncated);
  }
}

struct point_origin_interface {
  template<PointOrigin PO>
  [[nodiscard]] friend consteval QuantitySpec auto get_quantity_spec(PO po)
  {
    return po._quantity_spec_;
  }

  template<PointOrigin PO, typename FwdQ, Quantity Q = std::remove_cvref_t<FwdQ>>
    requires PointOriginFor<PO, Q::quantity_spec>
  [[nodiscard]] friend constexpr QuantityPoint auto operator+(PO po, FwdQ&& q)
  {
    return quantity_point{std::forward<FwdQ>(q), po};
  }

  template<typename FwdQ, PointOrigin PO, Quantity Q = std::remove_cvref_t<FwdQ>>
    requires PointOriginFor<PO, Q::quantity_spec>
  [[nodiscard]] friend constexpr QuantityPoint auto operator+(FwdQ&& q, PO po)
  {
    return po + std::forward<FwdQ>(q);
  }

  template<PointOrigin PO, Quantity Q>
    requires PointOriginFor<PO, Q::quantity_spec>
  [[nodiscard]] friend constexpr QuantityPoint auto operator-(PO po, const Q& q)
    requires requires { -q; }
  {
    return po + (-q);
  }

  template<PointOrigin PO1, detail::SameAbsolutePointOriginAs<PO1{}> PO2>
    requires(PointOriginFor<PO2, get_quantity_spec(PO1{})> || PointOriginFor<PO1, get_quantity_spec(PO2{})>) &&
            (is_derived_from_specialization_of_v<PO1, relative_point_origin> ||
             is_derived_from_specialization_of_v<PO2, relative_point_origin>)
  [[nodiscard]] friend constexpr Quantity auto operator-(PO1, PO2)
  {
    constexpr auto raw = [] {
      if constexpr (is_derived_from_specialization_of_v<PO1, absolute_point_origin>)
        return PO1{} - PO2::_quantity_point_;
      else if constexpr (is_derived_from_specialization_of_v<PO2, absolute_point_origin>)
        return PO1::_quantity_point_ - PO2{};
      else
        return PO1::_quantity_point_ - PO2::_quantity_point_;
    }();
    return detail::coarsen_origin_displacement<raw>();
  }

  template<PointOrigin PO1, PointOrigin PO2>
  [[nodiscard]] friend consteval bool operator==(PO1 po1, PO2 po2)
  {
    if constexpr (is_derived_from_specialization_of_v<PO1, absolute_point_origin> &&
                  is_derived_from_specialization_of_v<PO2, absolute_point_origin>)
      return std::is_same_v<PO1, PO2> ||
             (detail::is_natural_point_origin(po1) && detail::is_natural_point_origin(po2) &&
              mp_units::interconvertible(get_quantity_spec(po1), get_quantity_spec(po2)));
    else if constexpr (is_derived_from_specialization_of_v<PO1, relative_point_origin> &&
                       is_derived_from_specialization_of_v<PO2, relative_point_origin>)
      return PO1::_quantity_point_ == PO2::_quantity_point_;
    else if constexpr (is_derived_from_specialization_of_v<PO1, relative_point_origin>)
      return detail::same_absolute_point_origins(po1, po2) &&
             PO1::_quantity_point_.quantity_from(PO1::_quantity_point_.absolute_point_origin) == 0;
    else if constexpr (is_derived_from_specialization_of_v<PO2, relative_point_origin>)
      return detail::same_absolute_point_origins(po1, po2) &&
             PO2::_quantity_point_.quantity_from(PO2::_quantity_point_.absolute_point_origin) == 0;
  }
};

}  // namespace detail

namespace detail {

template<auto Arg>
constexpr bool is_bounds_arg = requires { Arg.min; } || requires { Arg.max; };

template<auto... Args>
consteval int bounds_arg_count()
{
  return (0 + ... + (is_bounds_arg<Args> ? 1 : 0));
}

// Selects the unique bounds policy argument from a variadic NTTP pack,
// regardless of order. Returns `undefined` when no bounds argument is present.
template<auto... Args>
  requires(sizeof...(Args) == 0)
consteval auto bounds_arg()
{
  return utility::unspecified;
}

template<auto First, auto... Rest>
consteval auto bounds_arg()
{
  if constexpr (is_bounds_arg<First>)
    return First;
  else
    return bounds_arg<Rest...>();
}

}  // namespace detail

MP_UNITS_EXPORT template<QuantitySpec auto QS, auto... Args>
struct absolute_point_origin : detail::point_origin_interface {
  static_assert(detail::bounds_arg_count<Args...>() <= 1, "at most one bounds policy may be provided");
  static constexpr QuantitySpec auto _quantity_spec_ = QS;
  static constexpr auto _bounds_ = detail::bounds_arg<Args...>();
};

MP_UNITS_EXPORT template<QuantityPoint auto QP, auto... Args>
struct relative_point_origin : detail::point_origin_interface {
  static_assert(detail::bounds_arg_count<Args...>() <= 1, "at most one bounds policy may be provided");
  static constexpr QuantityPoint auto _quantity_point_ = QP;
  static constexpr QuantitySpec auto _quantity_spec_ = []() {
    // select the strongest of specs
    if constexpr (detail::QuantityKindSpec<decltype(QP.quantity_spec)>)
      return get_quantity_spec(QP.point_origin);
    else
      return QP.quantity_spec;
  }();
  static constexpr PointOrigin auto _absolute_point_origin_ = QP.absolute_point_origin;
  static constexpr auto _bounds_ = detail::bounds_arg<Args...>();
};

namespace detail {

template<auto... Args>
constexpr bool is_specialization_of_absolute_point_origin<absolute_point_origin<Args...>> = true;

template<auto... Args>
constexpr bool is_specialization_of_relative_point_origin<relative_point_origin<Args...>> = true;

// Conditional base class for natural_point_origin_: applies check_non_negative bounds
// when QS is tagged non_negative with real_scalar character, otherwise no bounds.
template<QuantitySpec auto QS, bool = QS._is_non_negative_>
struct natural_origin_base {
  using type = absolute_point_origin<QS>;
};

template<QuantitySpec auto QS>
struct natural_origin_base<QS, true> {
  using type = absolute_point_origin<QS, check_non_negative{}>;
};

template<QuantitySpec auto QS>
using natural_origin_base_t = natural_origin_base<QS>::type;

}  // namespace detail

template<QuantitySpec auto QS>
struct natural_point_origin_ final : detail::natural_origin_base_t<QS> {};

MP_UNITS_EXPORT template<QuantitySpec auto QS>
constexpr natural_point_origin_<QS> natural_point_origin;

template<QuantitySpec auto QS>
using zeroth_point_origin_ [[deprecated("2.6.0: Use `natural_point_origin_` instead")]] = natural_point_origin_<QS>;

MP_UNITS_EXPORT template<QuantitySpec auto QS>
[[deprecated("2.6.0: Use `natural_point_origin` instead")]]
constexpr natural_point_origin_<QS>
  zeroth_point_origin = natural_point_origin<QS>;

namespace detail {

template<auto QS>
constexpr bool is_specialization_of_natural_point_origin<natural_point_origin_<QS>> = true;

}  // namespace detail


MP_UNITS_EXPORT template<Reference R>
[[nodiscard]] consteval PointOriginFor<get_quantity_spec(R{})> auto default_point_origin(R)
{
  if constexpr (requires { get_unit(R{})._point_origin_; })
    return get_unit(R{})._point_origin_;
  else
    return natural_point_origin<get_quantity_spec(R{})>;
}

namespace detail {

template<PointOrigin PO>
[[nodiscard]] consteval PointOrigin auto get_absolute_point_origin(PO po)
{
  if constexpr (is_derived_from_specialization_of_v<PO, absolute_point_origin>)
    return po;
  else
    return po._quantity_point_.absolute_point_origin;
}

}  // namespace detail

namespace detail {

// Compiles to the quantity offset from PO's own _quantity_point_ reference origin,
// i.e., the value stored in the relative_point_origin<QP> template argument.
// Only valid when PO is a relative_point_origin.
template<PointOrigin PO>
  requires requires { PO::_quantity_point_; }
[[nodiscard]] consteval auto offset_from_parent(PO)
{
  constexpr auto parent_po = PO::_quantity_point_.point_origin;
  return PO::_quantity_point_.quantity_from(parent_po);
}

// True iff PO carries a bounds policy of its own (a _bounds_ member that is not unspecified).
// Unlike has_quantity_bounds_v, it runs no nesting check.
template<PointOrigin PO>
[[nodiscard]] consteval bool has_own_bounds(PO)
{
  // Clang 16 rejects (and Clang 17 crashes on) the nested consteval calls inside the `if constexpr`
  // condition. A `constexpr` local avoids both.
  constexpr detail::character_mask mask = get_quantity_spec(PO{})._character_;
  if constexpr (detail::is_real_scalar(mask)) {
    if constexpr (requires { PO::_bounds_; })
      return utility::specified<decltype(PO::_bounds_)>;
    else
      return false;
  } else {
    return false;
  }
}

// True iff an origin above PO (PO itself excluded) carries bounds of its own.
template<PointOrigin PO>
[[nodiscard]] consteval bool has_bounded_ancestor(PO)
{
  if constexpr (is_derived_from_specialization_of_v<PO, relative_point_origin>) {
    constexpr auto parent_po = PO::_quantity_point_.point_origin;
    return has_own_bounds(parent_po) || has_bounded_ancestor(parent_po);
  } else {
    return false;
  }
}

// The nearest origin above PO (PO itself excluded) that carries bounds of its own.
// Only valid when has_bounded_ancestor(PO) is true.
template<PointOrigin PO>
[[nodiscard]] consteval auto nearest_bounded_ancestor(PO)
{
  constexpr auto parent_po = PO::_quantity_point_.point_origin;
  if constexpr (has_own_bounds(parent_po))
    return parent_po;
  else
    return nearest_bounded_ancestor(parent_po);
}

// The cumulative offset from PO's frame to the frame of nearest_bounded_ancestor(PO).
// Only valid when has_bounded_ancestor(PO) is true.
template<PointOrigin PO>
[[nodiscard]] consteval auto offset_to_bounded_ancestor(PO)
{
  constexpr auto parent_po = PO::_quantity_point_.point_origin;
  constexpr auto off = offset_from_parent(PO{});
  if constexpr (has_own_bounds(parent_po))
    return off;
  else
    return off + offset_to_bounded_ancestor(parent_po);
}

// True iff the bounds policy excludes its min (max) from the range, i.e. stores it as an
// exclusive_bound.
template<typename Policy>
[[nodiscard]] consteval bool excludes_min(const Policy&)
{
  if constexpr (requires { Policy::min; })
    return is_exclusive_bound<decltype(Policy::min)>;
  else
    return false;
}

template<typename Policy>
[[nodiscard]] consteval bool excludes_max(const Policy&)
{
  if constexpr (requires { Policy::max; })
    return is_exclusive_bound<decltype(Policy::max)>;
  else
    return false;
}

// Cached boolean with the value of has_own_bounds(PO{}), whose instantiation also runs the nesting
// check. When PO is a relative_point_origin with a bounded ancestor, the lambda validates at compile
// time that PO's range nests within the range of its nearest bounded ancestor. A value inside PO's
// bounds then lies between the min and max of every origin above it, up to rounding of
// floating-point offsets. A side bounded in the
// ancestor must be bounded in PO as well. Bounds are compared with their exclusivity: an excluded
// ancestor bound needs an included bound of PO strictly inside it. The variable is instantiated when
// PO's bounds are first used, and the compiler caches it, so the static_asserts fire once per origin
// rather than at each enforce_bounds call.
template<PointOrigin PO>
constexpr bool has_quantity_bounds_v = [] {
  if constexpr (has_own_bounds(PO{})) {
    static_assert(
      requires { PO::_bounds_.min; } || requires { PO::_bounds_.max; },
      "bounds policy must have at least a 'min' or 'max' member");
    if constexpr (has_bounded_ancestor(PO{})) {
      constexpr auto ancestor_po = nearest_bounded_ancestor(PO{});
      using ancestor_po_t = MP_UNITS_NONCONST_TYPE(ancestor_po);
      static_assert(has_quantity_bounds_v<ancestor_po_t>);  // validates the rest of the chain
      constexpr auto offset = offset_to_bounded_ancestor(PO{});
      // Each side is compared by its bounds. Where the ancestor excludes its bound and PO includes
      // its own, PO's bound has to lie strictly inside.
      if constexpr (requires { ancestor_po_t::_bounds_.min; }) {
        if constexpr (!requires { PO::_bounds_.min; })
          static_assert(
            requires { PO::_bounds_.min; },
            "relative origin must have a lower bound because an origin above it has one");
        else if constexpr (excludes_min(ancestor_po_t::_bounds_) && !excludes_min(PO::_bounds_))
          static_assert(bound_value(PO::_bounds_.min) + offset > bound_value(ancestor_po_t::_bounds_.min),
                        "relative origin lower bound violates the bounds of an origin above it");
        else
          static_assert(bound_value(PO::_bounds_.min) + offset >= bound_value(ancestor_po_t::_bounds_.min),
                        "relative origin lower bound violates the bounds of an origin above it");
      }
      if constexpr (requires { ancestor_po_t::_bounds_.max; }) {
        if constexpr (!requires { PO::_bounds_.max; })
          static_assert(
            requires { PO::_bounds_.max; },
            "relative origin must have an upper bound because an origin above it has one");
        else if constexpr (excludes_max(ancestor_po_t::_bounds_) && !excludes_max(PO::_bounds_))
          static_assert(bound_value(PO::_bounds_.max) + offset < bound_value(ancestor_po_t::_bounds_.max),
                        "relative origin upper bound violates the bounds of an origin above it");
        else
          static_assert(bound_value(PO::_bounds_.max) + offset <= bound_value(ancestor_po_t::_bounds_.max),
                        "relative origin upper bound violates the bounds of an origin above it");
      }
    }
    return true;
  } else {
    return false;
  }
}();

template<typename PO>
concept HasQuantityBounds = PointOrigin<PO> && has_quantity_bounds_v<PO>;

// Returns true if PO or any ancestor has quantity_bounds defined.
template<PointOrigin PO>
consteval bool any_ancestor_has_bounds(PO)
{
  if constexpr (HasQuantityBounds<PO>)
    return true;
  else if constexpr (is_derived_from_specialization_of_v<PO, relative_point_origin>)
    return any_ancestor_has_bounds(PO::_quantity_point_.point_origin);
  else
    return false;
}

// Returns the nearest (self or ancestor) PointOrigin that has bounds defined.
// Only valid when any_ancestor_has_bounds<PO>() is true.
template<PointOrigin PO>
[[nodiscard]] consteval auto bounds_po_for(PO po)
{
  if constexpr (HasQuantityBounds<PO>)
    return po;
  else {
    constexpr auto parent_po = PO::_quantity_point_.point_origin;
    return bounds_po_for(parent_po);
  }
}

// Returns the compile-time cumulative quantity offset from PO's frame to the
// nearest bounds-owning ancestor's frame.
// Only valid when !HasQuantityBounds<PO> && any_ancestor_has_bounds<PO>() is true.
template<PointOrigin PO>
[[nodiscard]] consteval auto bounds_offset(PO)
{
  static_assert(is_derived_from_specialization_of_v<PO, relative_point_origin>,
                "bounds_offset requires a relative_point_origin with a bounds-owning ancestor");
  constexpr auto parent_po = PO::_quantity_point_.point_origin;
  constexpr auto off = offset_from_parent(PO{});
  if constexpr (HasQuantityBounds<std::remove_cvref_t<decltype(parent_po)>>)
    return off;
  else
    return off + bounds_offset(parent_po);
}

// True iff each bound of the policy is exact in V (see bound_exact_in).
template<Quantity V, typename Policy>
[[nodiscard]] consteval bool policy_bounds_exact_in(const Policy& policy)
{
  bool exact = true;
  if constexpr (requires { policy.min; }) exact = exact && bound_exact_in<V>(policy.min);
  if constexpr (requires { policy.max; }) exact = exact && bound_exact_in<V>(policy.max);
  return exact;
}

// The offset from PO's frame to the frame of its bounds owner, in the representation of Q. A
// floating-point representation keeps the offset's unit. Any other one is converted to Q's unit too, so
// the translated value stays in Q's quantity type, which bounds_exact_in requires to be exact.
// Only valid when !HasQuantityBounds<PO> && any_ancestor_has_bounds(PO) is true.
template<PointOrigin auto PO, Quantity Q>
[[nodiscard]] constexpr Quantity auto bounds_offset_in()
{
  constexpr auto off = bounds_offset(PO);
  if constexpr (treat_as_floating_point<typename Q::rep>)
    return value_cast<typename Q::rep>(off);
  else
    return value_cast<Q::unit, typename Q::rep>(off);
}

// True iff each bound of the bounds owner is exact in the type the check runs in. So must be the
// offset into the owner's frame, unless Q's representation is a floating-point one. A truncated
// constant would move the bound.
template<PointOrigin auto PO, Quantity Q>
[[nodiscard]] consteval bool bounds_exact_in()
{
  if constexpr (HasQuantityBounds<MP_UNITS_NONCONST_TYPE(PO)>)
    return policy_bounds_exact_in<Q>(PO._bounds_);
  else if constexpr (!any_ancestor_has_bounds(PO))
    return true;
  else if constexpr (!requires { value_cast<Q::unit, typename Q::rep>(bounds_offset(PO)); })
    return false;
  else {
    constexpr auto bpo = bounds_po_for(PO);
    using translated = decltype(std::declval<const Q&>() + bounds_offset_in<PO, Q>());
    if constexpr (!treat_as_floating_point<typename Q::rep>)
      if (bounds_offset_in<PO, Q>() != bounds_offset(PO)) return false;
    return policy_bounds_exact_in<translated>(bpo._bounds_);
  }
}

// Apply bounds to q (expressed relative to PO's origin frame).
// Uses compile-time lookup to find the bounds owner and its offset from PO — at most a
// single runtime translate/apply/un-translate with no recursive function calls.
template<PointOrigin auto PO, auto R, typename Rep>
constexpr quantity<R, Rep> enforce_bounds(quantity<R, Rep> q)
{
  static_assert(bounds_exact_in<PO, quantity<R, Rep>>(),
                "the point's quantity type cannot hold a bound or the offset to the origin whose bounds apply");
  if constexpr (HasQuantityBounds<MP_UNITS_NONCONST_TYPE(PO)>) {
    // Direct bounds on PO — apply without any translation.
    return apply_bounds_policy(PO._bounds_, q);
  } else if constexpr (any_ancestor_has_bounds(PO)) {
    // No direct bounds — locate the bounds-owning ancestor and its cumulative offset,
    // both resolved entirely at compile time.
    constexpr auto bpo = bounds_po_for(PO);
    // The offset in the same rep as q, so that the arithmetic preserves Rep (e.g.,
    // constrained<double, throw_policy>) and check_non_negative uses the correct violation
    // handler instead of making the bounds a precondition.
    const auto typed_off = bounds_offset_in<PO, quantity<R, Rep>>();
    // Single flat: translate to owner's frame, apply policy, translate back. When the policy let
    // the value through unchanged, q itself is returned: translating back could round it to a
    // neighboring value in a floating-point representation.
    const auto in_owner_frame = q + typed_off;
    const auto result = apply_bounds_policy(bpo._bounds_, in_owner_frame);
    if (result == in_owner_frame) return q;
    return result - typed_off;
  } else {
    // No bounds anywhere in the chain — return q unchanged.  The explicit `else` is
    // important: MSVC Debug doesn't dead-code-eliminate after the always-returning
    // `if constexpr` branches above, so a bare fallthrough `return q;` here would trip
    // C4702 "unreachable code" on instantiations where one of the branches above is
    // taken.
    return q;
  }
}

// Tells whether `enforce_bounds<PO>(q)` would accept q without reporting a violation. It mirrors
// enforce_bounds: the same bounds owner, the same offset arithmetic, and the same call of the
// checking policy's predicate. Only valid when `enforced_bounds_check<PO, Q>()` is true.
template<PointOrigin auto PO, Quantity Q>
[[nodiscard]] constexpr bool bounds_contain(const Q& q)
{
  static_assert(bounds_exact_in<PO, Q>(),
                "the point's quantity type cannot hold a bound or the offset to the origin whose bounds apply");
  if constexpr (HasQuantityBounds<MP_UNITS_NONCONST_TYPE(PO)>) {
    return PO._bounds_(q);
  } else if constexpr (any_ancestor_has_bounds(PO)) {
    constexpr auto bpo = bounds_po_for(PO);
    return bpo._bounds_(q + bounds_offset_in<PO, Q>());
  } else {
    return true;
  }
}

// True iff the bounds policy that enforce_bounds applies to a value of type Q at PO is a checking
// one, i.e. its operator() returns `bool` for the value it is handed (vacuously true when there are
// no bounds at all). Construction applies only that policy, and the nesting check in
// has_quantity_bounds_v keeps its range within those of the origins above.
template<PointOrigin auto PO, Quantity Q>
[[nodiscard]] consteval bool enforced_bounds_check()
{
  if constexpr (HasQuantityBounds<MP_UNITS_NONCONST_TYPE(PO)>) {
    return CheckingPolicyFor<decltype(PO._bounds_), Q>;
  } else if constexpr (any_ancestor_has_bounds(PO)) {
    constexpr auto bpo = bounds_po_for(PO);
    return CheckingPolicyFor<decltype(bpo._bounds_), decltype(std::declval<const Q&>() + bounds_offset_in<PO, Q>())>;
  } else {
    return true;
  }
}

// The bounds precondition of storing v (expressed relative to PO's origin frame) at PO. It holds
// unless the enforced policy is a checking one, the representation has no
// constraint_violation_handler to report a rejection to, and the policy rejects v. Where it can
// fail, it is the precondition of the quantity_point constructors, increments, decrements, and
// compound assignments that store v.
template<PointOrigin auto PO, Quantity Q>
[[nodiscard]] constexpr bool bounds_precondition(const Q& v)
{
  if constexpr (any_ancestor_has_bounds(PO) && enforced_bounds_check<PO, Q>() &&
                !HasConstraintViolationHandler<typename Q::rep>)
    return bounds_contain<PO>(v);
  else
    return true;
}

// The value a compound assignment or an increment computes. Its precondition tests that value, and
// the operation stores it only after the bounds policy has seen it, so a handler that throws leaves
// the point unchanged.
template<Quantity Q, Quantity Q2>
[[nodiscard]] constexpr Q added(Q lhs, const Q2& rhs)
{
  lhs += rhs;
  return lhs;
}

template<Quantity Q, Quantity Q2>
[[nodiscard]] constexpr Q subtracted(Q lhs, const Q2& rhs)
{
  lhs -= rhs;
  return lhs;
}

template<Quantity Q>
[[nodiscard]] constexpr Q incremented(Q q)
{
  ++q;
  return q;
}

template<Quantity Q>
[[nodiscard]] constexpr Q decremented(Q q)
{
  --q;
  return q;
}

// Hidden-friend interface for `quantity_point`. Provides heterogeneous binary and comparison
// operators between any two `quantity_point<R1, PO1, Rep1>` and `quantity_point<R2, PO2, Rep2>`
// specializations (and between a `quantity_point` and a `quantity` or a `PointOrigin`). Made
// findable only via ADL.
struct quantity_point_iface {
  // operator+ (qp + q)
  template<auto R1, auto PO1, typename Rep1, auto R2, typename Rep2>
    requires ReferenceOf<MP_UNITS_NTTP_HACK(decltype(R2)), get_quantity_spec(PO1)>
  [[nodiscard]] friend constexpr QuantityPoint auto operator+(const quantity_point<R1, PO1, Rep1>& qp,
                                                              const quantity<R2, Rep2>& q)
    requires requires { qp.quantity_ref_from(PO1) + q; }
  {
    return detail::make_quantity_point(qp.quantity_ref_from(PO1) + q, PO1);
  }

  // operator+ (q + qp)
  template<auto R1, typename Rep1, auto R2, auto PO2, typename Rep2>
    requires ReferenceOf<MP_UNITS_NTTP_HACK(decltype(R1)), get_quantity_spec(PO2)>
  [[nodiscard]] friend constexpr QuantityPoint auto operator+(const quantity<R1, Rep1>& q,
                                                              const quantity_point<R2, PO2, Rep2>& qp)
    requires requires { q + qp.quantity_ref_from(PO2); }
  {
    return qp + q;
  }

  // operator- (qp - q)
  template<auto R1, auto PO1, typename Rep1, auto R2, typename Rep2>
    requires ReferenceOf<MP_UNITS_NTTP_HACK(decltype(R2)), get_quantity_spec(PO1)>
  [[nodiscard]] friend constexpr QuantityPoint auto operator-(const quantity_point<R1, PO1, Rep1>& qp,
                                                              const quantity<R2, Rep2>& q)
    requires requires { qp.quantity_ref_from(PO1) - q; }
  {
    return detail::make_quantity_point(qp.quantity_ref_from(PO1) - q, PO1);
  }

  // operator- (qp - qp)
  template<auto R1, auto PO1, typename Rep1, QuantityPointOf<mp_units::detail::get_absolute_point_origin(PO1)> QP2>
  [[nodiscard]] friend constexpr Quantity auto operator-(const quantity_point<R1, PO1, Rep1>& lhs, const QP2& rhs)
    // TODO consider constraining it for both branches
    requires requires { lhs.quantity_ref_from(PO1) - rhs.quantity_ref_from(QP2::point_origin); }
  {
    if constexpr (PO1 == QP2::point_origin)
      return lhs.quantity_ref_from(PO1) - rhs.quantity_ref_from(QP2::point_origin);
    else
      return lhs.quantity_ref_from(PO1) - rhs.quantity_ref_from(QP2::point_origin) + (PO1 - QP2::point_origin);
  }

  // operator- (qp - po)
  template<auto R1, auto PO1, typename Rep1, PointOrigin PO2>
    requires QuantityPointOf<quantity_point<R1, PO1, Rep1>, PO2{}> &&
             ReferenceOf<MP_UNITS_NTTP_HACK(decltype(R1)), get_quantity_spec(PO2{})>
  [[nodiscard]] friend constexpr Quantity auto operator-(const quantity_point<R1, PO1, Rep1>& qp, PO2 po)
  {
    if constexpr (PO1 == po)
      return qp.quantity_ref_from(PO1);
    else if constexpr (is_derived_from_specialization_of_v<PO2, ::mp_units::absolute_point_origin>) {
      if constexpr (PO1 == quantity_point<R1, PO1, Rep1>::absolute_point_origin)
        return qp.quantity_ref_from(PO1);
      else
        return qp.quantity_ref_from(PO1) + (PO1 - quantity_point<R1, PO1, Rep1>::absolute_point_origin);
    } else {
      if constexpr (PO1 == po._quantity_point_.point_origin)
        return qp.quantity_ref_from(PO1) - po._quantity_point_.quantity_ref_from(po._quantity_point_.point_origin);
      else
        return qp.quantity_ref_from(PO1) - po._quantity_point_.quantity_ref_from(po._quantity_point_.point_origin) +
               (PO1 - po._quantity_point_.point_origin);
    }
  }

  // operator- (po - qp)
  template<PointOrigin PO1, auto R2, auto PO2, typename Rep2>
    requires QuantityPointOf<quantity_point<R2, PO2, Rep2>, PO1{}> &&
             ReferenceOf<MP_UNITS_NTTP_HACK(decltype(R2)), get_quantity_spec(PO1{})>
  [[nodiscard]] friend constexpr Quantity auto operator-(PO1 po, const quantity_point<R2, PO2, Rep2>& qp)
  {
    return -(qp - po);
  }

  // operator==
  // Across origins, both sides are first expressed as offsets from the absolute origin. That needs
  // arithmetic with the origin offset, which a representation such as `constrained` may not support,
  // so that expression is part of the constraint.
  template<auto R1, auto PO1, typename Rep1, QuantityPointOf<mp_units::detail::get_absolute_point_origin(PO1)> QP2>
    requires std::equality_comparable_with<quantity<R1, Rep1>, typename QP2::quantity_type>
  [[nodiscard]] friend constexpr bool operator==(const quantity_point<R1, PO1, Rep1>& lhs, const QP2& rhs)
    requires(PO1 == QP2::point_origin) ||
            requires { lhs - lhs.absolute_point_origin == rhs - rhs.absolute_point_origin; }
  {
    if constexpr (PO1 == QP2::point_origin)
      return lhs.quantity_ref_from(PO1) == rhs.quantity_ref_from(QP2::point_origin);
    else
      return lhs - lhs.absolute_point_origin == rhs - rhs.absolute_point_origin;
  }

  // operator<=>
  template<auto R1, auto PO1, typename Rep1, QuantityPointOf<mp_units::detail::get_absolute_point_origin(PO1)> QP2>
    requires std::three_way_comparable_with<quantity<R1, Rep1>, typename QP2::quantity_type>
  [[nodiscard]] friend constexpr auto operator<=>(const quantity_point<R1, PO1, Rep1>& lhs, const QP2& rhs)
    requires(PO1 == QP2::point_origin) ||
            requires { lhs - lhs.absolute_point_origin <=> rhs - rhs.absolute_point_origin; }
  {
    if constexpr (PO1 == QP2::point_origin)
      return lhs.quantity_ref_from(PO1) <=> rhs.quantity_ref_from(QP2::point_origin);
    else
      return lhs - lhs.absolute_point_origin <=> rhs - rhs.absolute_point_origin;
  }

#if MP_UNITS_HOSTED

  template<typename CharT, typename Traits, auto R, auto PO, typename Rep>
    requires(PO == default_point_origin(R))
  friend std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os,
                                                       const quantity_point<R, PO, Rep>& qp)
    requires requires { os << qp.quantity_from_zero(); }
  {
    return os << qp.quantity_from_zero();
  }

#endif  // MP_UNITS_HOSTED
};

// Re-expresses a range bound as the point's own quantity type. That conversion is value-preserving
// for every bounded point type the shipped policies can instantiate (they construct the point's
// quantity type from the bound, which the framework only permits when nothing is lost), so the bound
// is reported exactly. A user-defined bounds policy that does not need such a construction may
// declare a bound that only converts by truncating; that one is rounded *inwards*, because a `min`
// rounded towards negative infinity or a `max` rounded towards positive infinity would report
// a range wider than the declared one and admit values outside it.
template<RoundingPolicy Policy, Quantity ToQ, Quantity Q>
[[nodiscard]] constexpr ToQ bound_as(const Q& bound)
{
  if constexpr (std::convertible_to<Q, ToQ>)
    return ToQ{bound};
  else
    return value_cast<ToQ>(bound, Policy{});
}

// The floating-point type a representation is stepped through by one value at a time: the
// representation itself, or the one it wraps (constrained<double>). void when there is none.
template<typename Rep>
[[nodiscard]] consteval auto stepping_type()
{
  if constexpr (std::floating_point<Rep>)
    return std::type_identity<Rep>{};
  else if constexpr (requires { typename representation_underlying_type<Rep>::type; }) {
    using underlying = representation_underlying_type<Rep>::type;
    if constexpr (std::floating_point<underlying>)
      return std::type_identity<underlying>{};
    else
      return std::type_identity<void>{};
  } else
    return std::type_identity<void>{};
}

template<typename Rep>
using stepping_type_t = decltype(stepping_type<Rep>())::type;

// True iff next_after can step T at compile time: an IEEE 754 binary32 or binary64 type, whose
// neighbouring values have neighbouring bit patterns.
template<typename T>
[[nodiscard]] consteval bool is_steppable()
{
  if constexpr (std::floating_point<T>)
    return std::numeric_limits<T>::is_iec559 && (sizeof(T) == 4 || sizeof(T) == 8);
  else
    return false;
}

template<typename T>
constexpr bool steppable = is_steppable<T>();

// The neighbour of a finite x towards positive (up) or negative infinity.
template<typename T>
  requires steppable<T>
[[nodiscard]] constexpr T next_after(T x, bool up)
{
  using bits_type = std::conditional_t<sizeof(T) == 4, std::uint32_t, std::uint64_t>;
  if (x == T{0}) return up ? std::numeric_limits<T>::denorm_min() : -std::numeric_limits<T>::denorm_min();
  auto bits = std::bit_cast<bits_type>(x);
  if ((x > T{0}) == up)
    ++bits;
  else
    --bits;
  return std::bit_cast<T>(bits);
}

// Where min() (Lower) or max() of a point of Q at PO comes from.
enum class point_limit_source : std::uint8_t { none, own_bound, inherited_bound, representation };

template<PointOrigin auto PO, Quantity Q, bool Lower>
[[nodiscard]] consteval point_limit_source limit_source()
{
  constexpr bool rep_limit = Lower ? requires { Q::min(); } : requires { Q::max(); };
  constexpr auto rep_source = rep_limit ? point_limit_source::representation : point_limit_source::none;
  if constexpr (!bounds_exact_in<PO, Q>())
    return point_limit_source::none;
  else if constexpr (HasQuantityBounds<MP_UNITS_NONCONST_TYPE(PO)>) {
    if constexpr (Lower ? requires { PO._bounds_.min; } : requires { PO._bounds_.max; })
      return (Lower ? excludes_min(PO._bounds_) : excludes_max(PO._bounds_)) ? point_limit_source::none
                                                                             : point_limit_source::own_bound;
    else
      return rep_source;
  } else if constexpr (any_ancestor_has_bounds(PO)) {
    constexpr auto bpo = bounds_po_for(PO);
    if constexpr (Lower ? requires { bpo._bounds_.min; } : requires { bpo._bounds_.max; }) {
      if constexpr (Lower ? excludes_min(bpo._bounds_) : excludes_max(bpo._bounds_))
        return point_limit_source::none;
      else if constexpr (treat_as_floating_point<typename Q::rep> && !steppable<stepping_type_t<typename Q::rep>>)
        return point_limit_source::none;
      else
        return point_limit_source::inherited_bound;
    } else
      return rep_source;
  } else
    return rep_source;
}

// The lowest (Lower) or highest value of Q at PO that the bound of PO's nearest bounded ancestor
// accepts. It is translated from that bound in one step, which is exact for a representation that
// is not floating-point. A floating-point one rounds, so the result is then moved one value at a
// time to the last one that the policy's comparison still accepts after the translation that
// enforcement applies.
template<PointOrigin auto PO, Quantity Q, bool Lower>
[[nodiscard]] consteval Q inherited_limit()
{
  constexpr auto bpo = bounds_po_for(PO);
  const auto offset = bounds_offset_in<PO, Q>();
  using translated = decltype(std::declval<const Q&>() + offset);
  const auto accepts = [&](const Q& candidate) {
    if constexpr (Lower)
      return above_lower_bound(translated{candidate + offset}, bpo._bounds_.min);
    else
      return below_upper_bound(translated{candidate + offset}, bpo._bounds_.max);
  };
  Q limit = [&] {
    if constexpr (Lower)
      return Q{bound_in<translated>(bpo._bounds_.min) - offset};
    else
      return Q{bound_in<translated>(bpo._bounds_.max) - offset};
  }();
  if constexpr (treat_as_floating_point<typename Q::rep>) {
    using step_type = stepping_type_t<typename Q::rep>;
    const auto step = [](const Q& value, bool up) {
      const auto next = next_after(static_cast<step_type>(value.numerical_value_in(Q::unit)), up);
      return Q{static_cast<typename Q::rep>(next), Q::reference};
    };
    // Inwards (up for a lower bound) until accepted, then outwards while still accepted.
    while (!accepts(limit)) limit = step(limit, Lower);
    while (accepts(step(limit, !Lower))) limit = step(limit, !Lower);
  }
  return limit;
}

}  // namespace detail

MP_UNITS_EXPORT_BEGIN

/**
 * Tells whether constructing `QP{q, po}` would pass the bounds of the point's origin.
 *
 * `q` is converted to `QP::quantity_type` and passed to the `bool operator()` of the bounds policy
 * that construction enforces. That is the policy of `QP::point_origin` or, for a relative origin
 * without bounds of its own, the policy of its nearest bounded ancestor, with `q` translated into
 * that ancestor's frame. Origins further up are not tested, and construction does not apply their
 * policies either. The compile-time nesting check places the enforced bounds within theirs, up to
 * rounding of floating-point offsets. No point is constructed, so a rejected value neither calls the
 * representation's violation handler nor violates a precondition.
 *
 * Offered when the enforced bounds policy is a checking policy, such as @ref check_in_range,
 * @ref check_non_negative, or a user-defined policy whose `operator()` returns `bool`. Also offered
 * when the point has no bounds, where it always returns `true`.
 *
 * @see @ref quantity_point::try_make for the same test returning the constructed point
 */
template<QuantityPoint QP, typename FwdQ, QuantityOf<QP::quantity_spec> Q = std::remove_cvref_t<FwdQ>>
  requires(detail::bounds_exact_in<QP::point_origin, typename QP::quantity_type>()) &&
          (detail::enforced_bounds_check<QP::point_origin, typename QP::quantity_type>()) &&
          std::constructible_from<typename QP::quantity_type, FwdQ>
[[nodiscard]] constexpr bool is_within_bounds(FwdQ&& q, decltype(QP::point_origin))
{
  return detail::bounds_contain<QP::point_origin>(typename QP::quantity_type{std::forward<FwdQ>(q)});
}

template<QuantityPoint QP, typename FwdQ, QuantityOf<QP::quantity_spec> Q = std::remove_cvref_t<FwdQ>>
  requires(detail::bounds_exact_in<QP::point_origin, typename QP::quantity_type>()) &&
          (detail::enforced_bounds_check<QP::point_origin, typename QP::quantity_type>()) &&
          std::constructible_from<typename QP::quantity_type, FwdQ> &&
          (QP::point_origin == default_point_origin(QP::reference))
[[nodiscard]] constexpr bool is_within_bounds(FwdQ&& q)
{
  return detail::bounds_contain<QP::point_origin>(typename QP::quantity_type{std::forward<FwdQ>(q)});
}

#if MP_UNITS_HAS_EXPECTED

/**
 * The error returned by @ref quantity_point::try_make for a value that @ref is_within_bounds rejects.
 */
struct bounds_violation {
  // By reference: MSVC 14.51 rejects copying this empty class out of a std::expected in a
  // constant expression as a read of an uninitialized member.
  [[nodiscard]] friend constexpr bool operator==(const bounds_violation&, const bounds_violation&) = default;
};

#endif  // MP_UNITS_HAS_EXPECTED

/**
 * @brief A quantity point
 *
 * An absolute quantity measured from an origin.
 *
 * @tparam R a reference of the quantity point providing all information about quantity properties
 * @tparam PO a type that represents the origin point from which the quantity point is measured from
 * @tparam Rep a type to be used to represent values of a quantity point
 */
template<Reference auto R, PointOriginFor<get_quantity_spec(R)> auto PO = default_point_origin(R),
         RepresentationOf<get_quantity_spec(R)> Rep = double>
class quantity_point : public detail::quantity_point_iface {
public:
  // member types and values
  static constexpr Reference auto reference = R;
  static constexpr QuantitySpec auto quantity_spec = get_quantity_spec(reference);
  // The member is read directly rather than through `get_dimension(quantity_spec)`: the same value, without one
  // accessor instantiation per quantity point type.
  static constexpr Dimension auto dimension = quantity_spec._dimension_;
#if !MP_UNITS_EAGER_STATIC_MEMBER_INIT_HACK
  // Deprecated in 2.6.0. Ill-formed for a point of a quantity kind, which admits every character. Omitted on
  // MSVC up to 19.51 (see `MP_UNITS_EAGER_STATIC_MEMBER_INIT_HACK`).
  [[deprecated(
    "2.6.0: a quantity's character is no longer a value to inspect; ask "
    "`QuantityPointOf<QP, quantity_tensor_order::vector>`, `QuantityPointOf<QP, quantity_field::complex>`, or "
    "another character instead")]] static constexpr quantity_character character =
    detail::definite_character<quantity_spec._character_>();
#endif
  static constexpr Unit auto unit = get_unit(reference);
  static constexpr PointOrigin auto absolute_point_origin = detail::get_absolute_point_origin(PO);
  static constexpr PointOrigin auto point_origin = PO;
  using rep = Rep;
  using quantity_type = quantity<reference, Rep>;

  quantity_type quantity_from_origin_is_an_implementation_detail_;  // needs to be public for a structural type

  // static member functions
  // min() reports the lowest value the bounds accept: those of the origin, or those of its nearest
  // bounded ancestor translated into the point's frame. max() reports the highest. Neither is offered
  // when those bounds exclude that end, and without a bound on that side each reports the
  // representation's limit.
  [[nodiscard]] static constexpr quantity_point min() noexcept
    requires(detail::limit_source<PO, quantity_type, true>() != detail::point_limit_source::none)
  {
    constexpr auto source = detail::limit_source<PO, quantity_type, true>();
    if constexpr (source == detail::point_limit_source::own_bound) {
      // zero_quantity_t signals a [0, ∞) policy: return the natural zero for this
      // quantity type directly — correct quantity spec, no unit scaling required.
      if constexpr (std::same_as<std::remove_cvref_t<decltype(PO._bounds_.min)>, detail::zero_quantity_t>)
        return {quantity_type::zero(), PO};
      else
        return {detail::bound_as<rounded_up_t, quantity_type>(detail::bound_value(PO._bounds_.min)), PO};
    } else if constexpr (source == detail::point_limit_source::inherited_bound) {
      constexpr quantity_type limit = detail::inherited_limit<PO, quantity_type, true>();
      return {limit, PO};
    } else {
      return {quantity_type::min(), PO};
    }
  }

  [[nodiscard]] static constexpr quantity_point max() noexcept
    requires(detail::limit_source<PO, quantity_type, false>() != detail::point_limit_source::none)
  {
    constexpr auto source = detail::limit_source<PO, quantity_type, false>();
    if constexpr (source == detail::point_limit_source::own_bound)
      return {detail::bound_as<rounded_down_t, quantity_type>(detail::bound_value(PO._bounds_.max)), PO};
    else if constexpr (source == detail::point_limit_source::inherited_bound) {
      constexpr quantity_type limit = detail::inherited_limit<PO, quantity_type, false>();
      return {limit, PO};
    } else
      return {quantity_type::max(), PO};
  }

  [[nodiscard]] static constexpr quantity_point zero() noexcept
    requires(PO == default_point_origin(R)) && requires { quantity_type::zero(); }
  {
    return {quantity_type::zero(), PO};
  }

#if MP_UNITS_HAS_EXPECTED
  /**
   * Constructs `quantity_point{q, po}`, or returns @ref bounds_violation for a value that
   * @ref is_within_bounds rejects.
   *
   * Written only in terms of @ref is_within_bounds and the constructor, so a rejected value is not
   * passed to the constructor. This makes it the wide-contract alternative to the constructor, whose
   * precondition such a value violates when the representation has no `constraint_violation_handler`.
   * The same function can be written over `std::optional` or another error type. Available where
   * `<expected>` is.
   *
   * @see @ref is_within_bounds for the test alone
   */
  template<typename FwdQ>
    requires requires(const FwdQ& q) { mp_units::is_within_bounds<quantity_point>(q, PO); } &&
             std::constructible_from<quantity_point, FwdQ, decltype(PO)>
  [[nodiscard]] static constexpr std::expected<quantity_point, bounds_violation> try_make(FwdQ&& q, decltype(PO) po)
  {
    if (!mp_units::is_within_bounds<quantity_point>(std::as_const(q), po)) return std::unexpected(bounds_violation{});
    return quantity_point{std::forward<FwdQ>(q), po};
  }

  template<typename FwdQ>
    requires requires(const FwdQ& q) { mp_units::is_within_bounds<quantity_point>(q); } &&
             std::constructible_from<quantity_point, FwdQ>
  [[nodiscard]] static constexpr std::expected<quantity_point, bounds_violation> try_make(FwdQ&& q)
  {
    if (!mp_units::is_within_bounds<quantity_point>(std::as_const(q))) return std::unexpected(bounds_violation{});
    return quantity_point{std::forward<FwdQ>(q)};
  }
#endif

  // construction and assignment
  [[nodiscard]] quantity_point() = default;

  template<typename FwdQ, QuantityOf<quantity_spec> Q = std::remove_cvref_t<FwdQ>>
    requires std::constructible_from<quantity_type, FwdQ> && (point_origin == default_point_origin(R)) &&
             (detail::bounds_exact_in<PO, quantity_type>())
  [[nodiscard]] constexpr explicit quantity_point(FwdQ&& q)
    MP_UNITS_PRE_CONSTEVAL_HACK(detail::bounds_precondition<point_origin>(quantity_type{q})) :
      quantity_from_origin_is_an_implementation_detail_(
        detail::enforce_bounds<point_origin>(quantity_type{std::forward<FwdQ>(q)}))
  {
    MP_UNITS_EXPECTS_CONSTEVAL_HACK(
      detail::bounds_precondition<point_origin>(quantity_from_origin_is_an_implementation_detail_));
  }

  template<typename FwdQ, QuantityOf<quantity_spec> Q = std::remove_cvref_t<FwdQ>>
    requires std::constructible_from<quantity_type, FwdQ> && (detail::bounds_exact_in<PO, quantity_type>())
  [[nodiscard]] constexpr quantity_point(FwdQ&& q, decltype(PO))
    MP_UNITS_PRE_CONSTEVAL_HACK(detail::bounds_precondition<point_origin>(quantity_type{q})) :
      quantity_from_origin_is_an_implementation_detail_(
        detail::enforce_bounds<point_origin>(quantity_type{std::forward<FwdQ>(q)}))
  {
    MP_UNITS_EXPECTS_CONSTEVAL_HACK(
      detail::bounds_precondition<point_origin>(quantity_from_origin_is_an_implementation_detail_));
  }

  template<typename FwdQ, PointOrigin PO2, Quantity Q = std::remove_cvref_t<FwdQ>>
    requires PointOriginFor<PO2, Q::quantity_spec> && std::constructible_from<quantity_type, FwdQ> &&
             detail::SameAbsolutePointOriginAs<PO2, PO> &&
             (detail::bounds_exact_in<PO2{}, quantity<Q::reference, typename Q::rep>>()) &&
             (detail::bounds_exact_in<PO, quantity_type>())
  [[nodiscard]] constexpr quantity_point(FwdQ&& q, PO2) :
      quantity_point(
        quantity_point<std::remove_reference_t<Q>::reference, PO2{}, typename std::remove_reference_t<Q>::rep>{
          std::forward<FwdQ>(q), PO2{}})
  {
  }

  template<QuantityPointOf<absolute_point_origin> QP>
    requires std::constructible_from<quantity_type, typename QP::quantity_type>
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  [[nodiscard]] constexpr explicit(!std::convertible_to<typename QP::quantity_type, quantity_type>)
    quantity_point(const QP& qp)
      MP_UNITS_PRE_CONSTEVAL_HACK(detail::bounds_precondition<point_origin>(quantity_from_point(qp))) :
      quantity_from_origin_is_an_implementation_detail_(detail::enforce_bounds<point_origin>(quantity_from_point(qp)))
  {
    MP_UNITS_EXPECTS_CONSTEVAL_HACK(
      detail::bounds_precondition<point_origin>(quantity_from_origin_is_an_implementation_detail_));
  }

  template<QuantityPoint QP>
    requires(!QuantityPointOf<QP, absolute_point_origin>) && requires(const QP& src) {
      { src.point_for(point_origin) } -> QuantityPointOf<point_origin>;
      quantity_type{src.point_for(point_origin) - point_origin};
    }
  [[nodiscard]] constexpr explicit quantity_point(const QP& qp)
    MP_UNITS_PRE_CONSTEVAL_HACK(detail::bounds_precondition<point_origin>(quantity_from_projected_point(qp))) :
      quantity_from_origin_is_an_implementation_detail_(
        detail::enforce_bounds<point_origin>(quantity_from_projected_point(qp)))
  {
    MP_UNITS_EXPECTS_CONSTEVAL_HACK(
      detail::bounds_precondition<point_origin>(quantity_from_origin_is_an_implementation_detail_));
  }

  template<QuantityPointLike QP>
    requires(quantity_point_like_traits<QP>::point_origin == point_origin) &&
            std::constructible_from<quantity_type, quantity<quantity_point_like_traits<QP>::reference,
                                                            typename quantity_point_like_traits<QP>::rep>>
  [[nodiscard]] constexpr explicit(
    quantity_point_like_traits<QP>::explicit_import ||
    !std::convertible_to<
      quantity<quantity_point_like_traits<QP>::reference, typename quantity_point_like_traits<QP>::rep>, quantity_type>)
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    quantity_point(const QP& qp)
      MP_UNITS_PRE_CONSTEVAL_HACK(detail::bounds_precondition<point_origin>(quantity_from_point_like(qp))) :
      quantity_from_origin_is_an_implementation_detail_(
        detail::enforce_bounds<point_origin>(quantity_from_point_like(qp)))
  {
    MP_UNITS_EXPECTS_CONSTEVAL_HACK(
      detail::bounds_precondition<point_origin>(quantity_from_origin_is_an_implementation_detail_));
  }

  template<detail::SameAbsolutePointOriginAs<absolute_point_origin> NewPO>
  [[nodiscard]] constexpr QuantityPointOf<(NewPO{})> auto point_for(NewPO new_origin) const
  {
    if constexpr (std::is_same_v<NewPO, MP_UNITS_NONCONST_TYPE(point_origin)>)
      return *this;
    else
      return ::mp_units::quantity_point{*this - new_origin, new_origin};
  }

  template<PointOrigin NewPO, typename... Args>
    requires detail::HasFrameProjection<absolute_point_origin, detail::get_absolute_point_origin(NewPO{})> &&
             requires(quantity_point<reference, absolute_point_origin, Rep> at_src, Args... args) {
               {
                 frame_projection<absolute_point_origin, detail::get_absolute_point_origin(NewPO{})>(at_src, args...)
               } -> QuantityPointOf<detail::get_absolute_point_origin(NewPO{})>;
             }
  [[nodiscard]] constexpr QuantityPointOf<(NewPO{})> auto point_for(NewPO new_origin, Args&&... args) const
  {
    auto at_src = point_for(absolute_point_origin);
    constexpr PointOrigin auto abs_tgt = detail::get_absolute_point_origin(NewPO{});
    auto at_tgt = frame_projection<absolute_point_origin, abs_tgt>(at_src, std::forward<Args>(args)...);
    if constexpr (std::is_same_v<NewPO, MP_UNITS_NONCONST_TYPE(abs_tgt)>)
      return at_tgt;
    else
      return at_tgt.point_for(new_origin);
  }

  // data access
  template<PointOrigin PO2>
    requires(PO2{} == point_origin)
  [[nodiscard]] constexpr quantity_type& quantity_ref_from(PO2) & noexcept
  {
    return quantity_from_origin_is_an_implementation_detail_;
  }

  template<PointOrigin PO2>
    requires(PO2{} == point_origin)
  [[nodiscard]] constexpr const quantity_type& quantity_ref_from(PO2) const& noexcept
  {
    return quantity_from_origin_is_an_implementation_detail_;
  }

  template<PointOrigin PO2>
    requires(PO2{} == point_origin)
  constexpr const quantity_type&& quantity_ref_from(PO2) const&& noexcept
#if MP_UNITS_HAS_DELETED_FUNCTION_MESSAGE
    = delete ("Can't form a reference to a temporary");
#else
    = delete;
#endif

  template<PointOrigin PO2>
    requires requires(const quantity_point qp) { qp - PO2{}; }
  [[nodiscard]] constexpr Quantity auto quantity_from(PO2) const
  {
    return *this - PO2{};
  }

  template<QuantityPointOf<absolute_point_origin> QP>
  [[nodiscard]] constexpr Quantity auto quantity_from(const QP& qp) const
  {
    return *this - qp;
  }

  [[nodiscard]] constexpr const quantity_type& quantity_from_zero() const noexcept
    requires(PO == default_point_origin(R))
  {
    return quantity_ref_from(PO);
  }

  // Extracts the numerical value of the point on the scale of `U` (measured from the default
  // point origin of that unit). This is the exact inverse of `point<U>(value)` and is provided
  // only for points that use `default_point_origin(R)` - the same condition under which text
  // output and `quantity_from_zero()` are available. Points with a custom origin have no unique
  // numerical representation, so their value must be requested from an explicit origin via
  // `quantity_from(origin).numerical_value_in(U)`.
  template<UnitOf<quantity_spec> U>
    requires(PO == default_point_origin(R)) && detail::ImplicitScaling<unit, U{}, rep>
  [[nodiscard]] constexpr RepresentationOf<quantity_spec> auto numerical_value_in(U) const noexcept
  {
    return in(U{}).quantity_from_zero().numerical_value_in(U{});
  }

  template<UnitOf<quantity_spec> U, RoundingPolicy Policy>
    requires(PO == default_point_origin(R)) && detail::ExplicitlyCastable<unit, U{}, rep> &&
            detail::ValidRoundingPolicyFor<Policy, rep, rep>
  [[nodiscard]] constexpr RepresentationOf<quantity_spec> auto numerical_value_in(U, Policy policy) const noexcept
  {
    return in(U{}, policy).quantity_from_zero().numerical_value_in(U{});
  }

  template<UnitOf<quantity_spec> U>
    requires(PO == default_point_origin(R)) && detail::ExplicitlyCastable<unit, U{}, rep>
  [[deprecated(
    "2.6.0: use `numerical_value_in(unit, policy)` with a rounding policy (e.g. `truncated`) "
    "instead")]] [[nodiscard]] constexpr RepresentationOf<quantity_spec> auto force_numerical_value_in(U) const noexcept
  {
    return numerical_value_in(U{}, truncated);
  }

private:
  template<UnitOf<quantity_spec> ToU, std::invocable<quantity_type> ConvertFn>
  [[nodiscard]] constexpr QuantityPointOf<quantity_spec> auto in_impl(ToU, ConvertFn convert) const
  {
    if constexpr (requires { ToU{}._point_origin_; } && (PO == default_point_origin(R))) {
      constexpr auto new_po = ToU{}._point_origin_;
      return ::mp_units::quantity_point{convert(quantity_from(new_po)), new_po};
    } else {
      return ::mp_units::quantity_point{convert(quantity_ref_from(point_origin)), point_origin};
    }
  }

public:
  // unit conversions
  template<UnitOf<quantity_spec> ToU>
    requires detail::ImplicitScaling<unit, ToU{}, rep>
  [[nodiscard]] constexpr QuantityPointOf<quantity_spec> auto in(ToU) const
  {
    return in_impl(ToU{}, [](const auto& q) { return q.in(ToU{}); });
  }

  template<RepresentationOf<quantity_spec> ToRep>
    requires detail::RepConstructibleFrom<ToRep, rep>
  [[nodiscard]] constexpr QuantityPointOf<quantity_spec> auto in() const
  {
    return ::mp_units::quantity_point{quantity_ref_from(point_origin).template in<ToRep>(), point_origin};
  }

  template<RepresentationOf<quantity_spec> ToRep, UnitOf<quantity_spec> ToU>
    requires detail::RepConstructibleFrom<ToRep, rep> && detail::ImplicitConversion<unit, rep, ToU{}, ToRep>
  [[nodiscard]] constexpr QuantityPointOf<quantity_spec> auto in(ToU) const
  {
    return in_impl(ToU{}, [](const auto& q) { return q.template in<ToRep>(ToU{}); });
  }

  template<UnitOf<quantity_spec> ToU, RoundingPolicy Policy>
    requires detail::ExplicitlyCastable<unit, ToU{}, rep> && detail::ValidRoundingPolicyFor<Policy, rep, rep>
  [[nodiscard]] constexpr QuantityPointOf<quantity_spec> auto in(ToU, Policy policy) const
  {
    return in_impl(ToU{}, [policy](const auto& q) { return q.in(ToU{}, policy); });
  }

  template<RepresentationOf<quantity_spec> ToRep, RoundingPolicy Policy>
    requires std::constructible_from<ToRep, rep> && detail::ValidRoundingPolicyFor<Policy, rep, ToRep>
  [[nodiscard]] constexpr QuantityPointOf<quantity_spec> auto in(Policy policy) const
  {
    return ::mp_units::quantity_point{quantity_ref_from(point_origin).template in<ToRep>(policy), point_origin};
  }

  template<RepresentationOf<quantity_spec> ToRep, UnitOf<quantity_spec> ToU, RoundingPolicy Policy>
    requires std::constructible_from<ToRep, rep> && detail::ExplicitlyCastable<unit, ToU{}, ToRep> &&
             detail::ValidRoundingPolicyFor<Policy, rep, ToRep>
  [[nodiscard]] constexpr QuantityPointOf<quantity_spec> auto in(ToU, Policy policy) const
  {
    return in_impl(ToU{}, [policy](const auto& q) { return q.template in<ToRep>(ToU{}, policy); });
  }

  template<UnitOf<quantity_spec> ToU>
    requires detail::ExplicitlyCastable<unit, ToU{}, rep>
  [[deprecated(
    "2.6.0: use `in(unit, policy)` with a rounding policy (e.g. `truncated`) "
    "instead")]] [[nodiscard]] constexpr QuantityPointOf<quantity_spec> auto force_in(ToU) const
  {
    return in(ToU{}, truncated);
  }

  template<RepresentationOf<quantity_spec> ToRep>
    requires std::constructible_from<ToRep, rep>
  [[deprecated("2.6.0: use `in<Rep>(policy)` with a rounding policy (e.g. `truncated`) instead")]] [[nodiscard]]
  constexpr QuantityPointOf<quantity_spec> auto force_in() const
  {
    return in<ToRep>(truncated);
  }

  template<RepresentationOf<quantity_spec> ToRep, UnitOf<quantity_spec> ToU>
    requires std::constructible_from<ToRep, rep> && detail::ExplicitlyCastable<unit, ToU{}, rep>
  [[deprecated(
    "2.6.0: use `in<Rep>(unit, policy)` with a rounding policy (e.g. `truncated`) "
    "instead")]] [[nodiscard]] constexpr QuantityPointOf<quantity_spec> auto force_in(ToU) const
  {
    return in<ToRep>(ToU{}, truncated);
  }

  // conversion operators
  template<typename QP_, QuantityPointLike QP = std::remove_cvref_t<QP_>>
    requires(point_origin == quantity_point_like_traits<QP>::point_origin) &&
            std::convertible_to<quantity_type, quantity<quantity_point_like_traits<QP>::reference,
                                                        typename quantity_point_like_traits<QP>::rep>>
  [[nodiscard]] explicit(
    quantity_point_like_traits<QP>::explicit_export ||
    !std::convertible_to<quantity_type, quantity<quantity_point_like_traits<QP>::reference,
                                                 typename quantity_point_like_traits<QP>::rep>>) constexpr
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  operator QP_() const& noexcept(
    noexcept(quantity_point_like_traits<QP>::from_numerical_value(
      quantity_from_origin_is_an_implementation_detail_.numerical_value_is_an_implementation_detail_)) &&
    std::is_nothrow_copy_constructible_v<rep>)
  {
    return quantity_point_like_traits<QP>::from_numerical_value(
      quantity_from_origin_is_an_implementation_detail_.numerical_value_is_an_implementation_detail_);
  }

  template<typename QP_, QuantityPointLike QP = std::remove_cvref_t<QP_>>
    requires(point_origin == quantity_point_like_traits<QP>::point_origin) &&
            std::convertible_to<quantity_type, quantity<quantity_point_like_traits<QP>::reference,
                                                        typename quantity_point_like_traits<QP>::rep>>
  [[nodiscard]] explicit(
    quantity_point_like_traits<QP>::explicit_export ||
    !std::convertible_to<quantity_type, quantity<quantity_point_like_traits<QP>::reference,
                                                 typename quantity_point_like_traits<QP>::rep>>) constexpr
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  operator QP_() && noexcept(
    noexcept(quantity_point_like_traits<QP>::from_numerical_value(
      quantity_from_origin_is_an_implementation_detail_.numerical_value_is_an_implementation_detail_)) &&
    std::is_nothrow_move_constructible_v<rep>)
  {
    return quantity_point_like_traits<QP>::from_numerical_value(
      std::move(quantity_from_origin_is_an_implementation_detail_).numerical_value_is_an_implementation_detail_);
  }

  // member unary operators
  constexpr quantity_point& operator++() &
    requires requires { ++quantity_from_origin_is_an_implementation_detail_; }
  MP_UNITS_PRE_CONSTEVAL_HACK(
    detail::bounds_precondition<point_origin>(detail::incremented(quantity_from_origin_is_an_implementation_detail_)))
  {
    MP_UNITS_EXPECTS_CONSTEVAL_HACK(detail::bounds_precondition<point_origin>(
      detail::incremented(quantity_from_origin_is_an_implementation_detail_)));
    quantity_from_origin_is_an_implementation_detail_ =
      detail::enforce_bounds<point_origin>(detail::incremented(quantity_from_origin_is_an_implementation_detail_));
    return *this;
  }

  [[nodiscard]] constexpr quantity_point operator++(int)
    requires requires { quantity_from_origin_is_an_implementation_detail_++; }
  MP_UNITS_PRE_CONSTEVAL_HACK(
    detail::bounds_precondition<point_origin>(detail::incremented(quantity_from_origin_is_an_implementation_detail_)))
  {
    MP_UNITS_EXPECTS_CONSTEVAL_HACK(detail::bounds_precondition<point_origin>(
      detail::incremented(quantity_from_origin_is_an_implementation_detail_)));
    auto old_quantity = quantity_from_origin_is_an_implementation_detail_;
    quantity_from_origin_is_an_implementation_detail_ =
      detail::enforce_bounds<point_origin>(detail::incremented(quantity_from_origin_is_an_implementation_detail_));
    return {old_quantity, PO};
  }

  constexpr quantity_point& operator--() &
    requires requires { --quantity_from_origin_is_an_implementation_detail_; }
  MP_UNITS_PRE_CONSTEVAL_HACK(
    detail::bounds_precondition<point_origin>(detail::decremented(quantity_from_origin_is_an_implementation_detail_)))
  {
    MP_UNITS_EXPECTS_CONSTEVAL_HACK(detail::bounds_precondition<point_origin>(
      detail::decremented(quantity_from_origin_is_an_implementation_detail_)));
    quantity_from_origin_is_an_implementation_detail_ =
      detail::enforce_bounds<point_origin>(detail::decremented(quantity_from_origin_is_an_implementation_detail_));
    return *this;
  }

  [[nodiscard]] constexpr quantity_point operator--(int)
    requires requires { quantity_from_origin_is_an_implementation_detail_--; }
  MP_UNITS_PRE_CONSTEVAL_HACK(
    detail::bounds_precondition<point_origin>(detail::decremented(quantity_from_origin_is_an_implementation_detail_)))
  {
    MP_UNITS_EXPECTS_CONSTEVAL_HACK(detail::bounds_precondition<point_origin>(
      detail::decremented(quantity_from_origin_is_an_implementation_detail_)));
    auto old_quantity = quantity_from_origin_is_an_implementation_detail_;
    quantity_from_origin_is_an_implementation_detail_ =
      detail::enforce_bounds<point_origin>(detail::decremented(quantity_from_origin_is_an_implementation_detail_));
    return {old_quantity, PO};
  }

  // compound assignment operators
  template<auto R2, typename Rep2>
    requires(mp_units::utility::implicitly_convertible(get_quantity_spec(R2), quantity_spec)) &&
            detail::ImplicitConversion<get_unit(R2), Rep2, unit, rep> &&
            requires(const quantity_type q) { quantity_from_origin_is_an_implementation_detail_ += q; }
            constexpr quantity_point& operator+=(const quantity<R2, Rep2>& q) &
            MP_UNITS_PRE_CONSTEVAL_HACK(detail::bounds_precondition<point_origin>(
              detail::added(quantity_from_origin_is_an_implementation_detail_, q)))
  {
    MP_UNITS_EXPECTS_CONSTEVAL_HACK(
      detail::bounds_precondition<point_origin>(detail::added(quantity_from_origin_is_an_implementation_detail_, q)));
    quantity_from_origin_is_an_implementation_detail_ =
      detail::enforce_bounds<point_origin>(detail::added(quantity_from_origin_is_an_implementation_detail_, q));
    return *this;
  }

  template<auto R2, typename Rep2>
    requires(mp_units::utility::implicitly_convertible(get_quantity_spec(R2), quantity_spec)) &&
            detail::ImplicitConversion<get_unit(R2), Rep2, unit, rep> &&
            requires(const quantity_type q) { quantity_from_origin_is_an_implementation_detail_ -= q; }
            constexpr quantity_point& operator-=(const quantity<R2, Rep2>& q) &
            MP_UNITS_PRE_CONSTEVAL_HACK(detail::bounds_precondition<point_origin>(
              detail::subtracted(quantity_from_origin_is_an_implementation_detail_, q)))
  {
    MP_UNITS_EXPECTS_CONSTEVAL_HACK(detail::bounds_precondition<point_origin>(
      detail::subtracted(quantity_from_origin_is_an_implementation_detail_, q)));
    quantity_from_origin_is_an_implementation_detail_ =
      detail::enforce_bounds<point_origin>(detail::subtracted(quantity_from_origin_is_an_implementation_detail_, q));
    return *this;
  }

private:
  // The value each converting constructor stores, shared by its precondition and its initializer.
  template<QuantityPointOf<absolute_point_origin> QP>
  [[nodiscard]] static constexpr quantity_type quantity_from_point(const QP& qp)
  {
    if constexpr (point_origin == QP::point_origin)
      return quantity_type{qp.quantity_ref_from(point_origin)};
    else
      return quantity_type{qp - point_origin};
  }

  template<QuantityPoint QP>
  [[nodiscard]] static constexpr quantity_type quantity_from_projected_point(const QP& qp)
  {
    auto at_dest = qp.point_for(point_origin);
    return quantity_type{at_dest - point_origin};
  }

  template<QuantityPointLike QP>
  [[nodiscard]] static constexpr quantity_type quantity_from_point_like(const QP& qp)
  {
    return quantity_type{quantity_point_like_traits<QP>::to_numerical_value(qp),
                         get_unit(quantity_point_like_traits<QP>::reference)};
  }
};

// CTAD
#if MP_UNITS_QUANTITY_POINT_CTAD_HACK
template<QuantityPoint QP>
quantity_point(QP qp) -> quantity_point<QP::reference, QP::point_origin, typename QP::rep>;
#endif

template<Quantity Q>
quantity_point(Q q) -> quantity_point<Q::reference, default_point_origin(Q::reference), typename Q::rep>;

template<Quantity Q, PointOriginFor<Q::quantity_spec> PO>
quantity_point(Q q, PO) -> quantity_point<Q::reference, PO{}, typename Q::rep>;

template<QuantityPointLike QP>
quantity_point(QP)
  -> quantity_point<quantity_point_like_traits<QP>::reference, quantity_point_like_traits<QP>::point_origin,
                    typename quantity_point_like_traits<QP>::rep>;


MP_UNITS_EXPORT_END

}  // namespace mp_units

template<auto R, auto PO, typename Rep>
  requires std::numeric_limits<Rep>::is_specialized
class std::numeric_limits<mp_units::quantity_point<R, PO, Rep>> : public std::numeric_limits<Rep> {
public:
  static constexpr mp_units::quantity_point<R, PO, Rep> min() noexcept
    requires requires { mp_units::quantity_point<R, PO, Rep>::min(); }
  {
    return mp_units::quantity_point<R, PO, Rep>::min();
  }

  static constexpr mp_units::quantity_point<R, PO, Rep> max() noexcept
    requires requires { mp_units::quantity_point<R, PO, Rep>::max(); }
  {
    return mp_units::quantity_point<R, PO, Rep>::max();
  }

  static constexpr mp_units::quantity_point<R, PO, Rep> lowest() noexcept
    requires(!mp_units::detail::excludes_min(PO._bounds_)) &&
            (requires { PO._bounds_.min; } || requires { std::numeric_limits<mp_units::quantity<R, Rep>>::lowest(); })
  {
    if constexpr (requires { PO._bounds_.min; })
      return min();
    else
      return {std::numeric_limits<mp_units::quantity<R, Rep>>::lowest(), PO};
  }

  static constexpr mp_units::quantity_point<R, PO, Rep> epsilon() noexcept
    requires requires { std::numeric_limits<mp_units::quantity<R, Rep>>::epsilon(); }
  {
    return {std::numeric_limits<mp_units::quantity<R, Rep>>::epsilon(), PO};
  }

  static constexpr mp_units::quantity_point<R, PO, Rep> round_error() noexcept
    requires requires { std::numeric_limits<mp_units::quantity<R, Rep>>::round_error(); }
  {
    return {std::numeric_limits<mp_units::quantity<R, Rep>>::round_error(), PO};
  }

  static constexpr mp_units::quantity_point<R, PO, Rep> infinity() noexcept
    requires requires { std::numeric_limits<mp_units::quantity<R, Rep>>::infinity(); }
  {
    return {std::numeric_limits<mp_units::quantity<R, Rep>>::infinity(), PO};
  }

  static constexpr mp_units::quantity_point<R, PO, Rep> quiet_NaN() noexcept
    requires requires { std::numeric_limits<mp_units::quantity<R, Rep>>::quiet_NaN(); }
  {
    return {std::numeric_limits<mp_units::quantity<R, Rep>>::quiet_NaN(), PO};
  }

  static constexpr mp_units::quantity_point<R, PO, Rep> signaling_NaN() noexcept
    requires requires { std::numeric_limits<mp_units::quantity<R, Rep>>::signaling_NaN(); }
  {
    return {std::numeric_limits<mp_units::quantity<R, Rep>>::signaling_NaN(), PO};
  }

  static constexpr mp_units::quantity_point<R, PO, Rep> denorm_min() noexcept
    requires requires { std::numeric_limits<mp_units::quantity<R, Rep>>::denorm_min(); }
  {
    return {std::numeric_limits<mp_units::quantity<R, Rep>>::denorm_min(), PO};
  }
};

#if MP_UNITS_HOSTED

template<auto R, auto PO, typename Rep, typename Char>
  requires(PO == default_point_origin(R))
struct MP_UNITS_STD_FMT::formatter<mp_units::quantity_point<R, PO, Rep>, Char> :
    MP_UNITS_STD_FMT::formatter<typename mp_units::quantity_point<R, PO, Rep>::quantity_type> {
  template<typename FormatContext>
  auto format(const mp_units::quantity_point<R, PO, Rep>& qp, FormatContext& ctx) const -> decltype(ctx.out())
  {
    return MP_UNITS_STD_FMT::formatter<typename mp_units::quantity_point<R, PO, Rep>::quantity_type>::format(
      qp.quantity_from_zero(), ctx);
  }
};

#endif  // MP_UNITS_HOSTED
