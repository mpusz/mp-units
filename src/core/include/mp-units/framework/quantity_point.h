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
#include <mp-units/bits/hacks.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/compat_macros.h>
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/quantity.h>
#include <mp-units/framework/quantity_point_concepts.h>
#include <mp-units/overflow_policies.h>
#if MP_UNITS_HOSTED
#include <mp-units/bits/format.h>
#include <mp-units/bits/ostream.h>
#endif

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <compare>  // IWYU pragma: export
#include <limits>
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

template<typename FwdQ, PointOrigin PO, QuantityOf<PO::_quantity_spec_> Q = std::remove_cvref_t<FwdQ>>
[[nodiscard]] constexpr QuantityPoint auto make_quantity_point(FwdQ&& q, PO po)
{
  if constexpr (detail::is_natural_point_origin(PO{}))
    return quantity_point{std::forward<FwdQ>(q)};
  else
    return quantity_point{std::forward<FwdQ>(q), po};
}

struct point_origin_interface {
  template<PointOrigin PO, typename FwdQ, QuantityOf<PO::_quantity_spec_> Q = std::remove_cvref_t<FwdQ>>
  [[nodiscard]] friend constexpr QuantityPoint auto operator+(PO po, FwdQ&& q)
  {
    return quantity_point{std::forward<FwdQ>(q), po};
  }

  template<typename FwdQ, PointOrigin PO, QuantityOf<PO::_quantity_spec_> Q = std::remove_cvref_t<FwdQ>>
  [[nodiscard]] friend constexpr QuantityPoint auto operator+(FwdQ&& q, PO po)
  {
    return po + std::forward<FwdQ>(q);
  }

  template<PointOrigin PO, Quantity Q>
    requires ReferenceOf<MP_UNITS_NONCONST_TYPE(Q::reference), PO::_quantity_spec_>
  [[nodiscard]] friend constexpr QuantityPoint auto operator-(PO po, const Q& q)
    requires requires { -q; }
  {
    return po + (-q);
  }

  template<PointOrigin PO1, detail::SameAbsolutePointOriginAs<PO1{}> PO2>
    requires(QuantitySpecOf<MP_UNITS_NONCONST_TYPE(PO1::_quantity_spec_), PO2::_quantity_spec_> ||
             QuantitySpecOf<MP_UNITS_NONCONST_TYPE(PO2::_quantity_spec_), PO1::_quantity_spec_>) &&
            (is_derived_from_specialization_of_v<PO1, relative_point_origin> ||
             is_derived_from_specialization_of_v<PO2, relative_point_origin>)
  [[nodiscard]] friend constexpr Quantity auto operator-(PO1 po1, PO2 po2)
  {
    if constexpr (is_derived_from_specialization_of_v<PO1, absolute_point_origin>) {
      return po1 - po2._quantity_point_;
    } else if constexpr (is_derived_from_specialization_of_v<PO2, absolute_point_origin>) {
      return po1._quantity_point_ - po2;
    } else {
      return po1._quantity_point_ - po2._quantity_point_;
    }
  }

  template<PointOrigin PO1, PointOrigin PO2>
  [[nodiscard]] friend consteval bool operator==(PO1 po1, PO2 po2)
  {
    if constexpr (is_derived_from_specialization_of_v<PO1, absolute_point_origin> &&
                  is_derived_from_specialization_of_v<PO2, absolute_point_origin>)
      return is_same_v<PO1, PO2> || (detail::is_natural_point_origin(po1) && detail::is_natural_point_origin(po2) &&
                                     mp_units::interconvertible(po1._quantity_spec_, po2._quantity_spec_));
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
  return undefined;
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
    if constexpr (detail::QuantityKindSpec<MP_UNITS_NONCONST_TYPE(QP.quantity_spec)>)
      return QP.point_origin._quantity_spec_;
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

// Cached boolean: true iff PO has a _bounds_ member that is not undefined_t.
// When PO is a relative_point_origin whose direct parent also has bounds, the
// lambda also validates at compile time that PO's range nests within the parent's.
// The result is cached by the compiler as a variable template instantiation, so the
// static_assert fires exactly once per (PO) rather than at each enforce_bounds call.
template<PointOrigin PO>
constexpr bool has_quantity_bounds_v = [] {
  if constexpr (PO::_quantity_spec_.character == quantity_character::real_scalar) {
    if constexpr (requires { PO::_bounds_; } &&
                  !std::is_same_v<std::remove_cvref_t<decltype(PO::_bounds_)>, undefined_t>) {
      static_assert(
        requires { PO::_bounds_.min; } || requires { PO::_bounds_.max; },
        "bounds policy must have at least a 'min' or 'max' member");
      if constexpr (is_derived_from_specialization_of_v<PO, relative_point_origin>) {
        constexpr auto parent_po = PO::_quantity_point_.point_origin;
        using parent_po_t = std::remove_cvref_t<decltype(parent_po)>;
        if constexpr (has_quantity_bounds_v<parent_po_t>) {
          constexpr auto offset = offset_from_parent(PO{});
          if constexpr (requires { PO::_bounds_.min; } && requires { parent_po_t::_bounds_.min; })
            static_assert(PO::_bounds_.min + offset >= parent_po_t::_bounds_.min,
                          "relative origin lower bound violates the parent origin's bounds");
          if constexpr (requires { PO::_bounds_.max; } && requires { parent_po_t::_bounds_.max; })
            static_assert(PO::_bounds_.max + offset <= parent_po_t::_bounds_.max,
                          "relative origin upper bound violates the parent origin's bounds");
        }
      }
      return true;
    } else {
      return false;
    }
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

// Apply bounds to q (expressed relative to PO's origin frame).
// Uses compile-time lookup to find the bounds owner and its offset from PO — at most a
// single runtime translate/apply/un-translate with no recursive function calls.
template<PointOrigin auto PO, auto R, typename Rep>
constexpr quantity<R, Rep> enforce_bounds(quantity<R, Rep> q)
{
  if constexpr (HasQuantityBounds<std::remove_cvref_t<decltype(PO)>>) {
    // Direct bounds on PO — apply without any translation.
    return PO._bounds_(q);
  } else if constexpr (any_ancestor_has_bounds(PO)) {
    // No direct bounds — locate the bounds-owning ancestor and its cumulative offset,
    // both resolved entirely at compile time.
    constexpr auto bpo = bounds_po_for(PO);
    constexpr auto off = bounds_offset(PO);
    // Cast offset to the same rep as q so that the arithmetic preserves Rep
    // (e.g., constrained<double, throw_policy>) and check_non_negative uses the
    // correct violation handler instead of falling back to MP_UNITS_EXPECTS.
    const auto typed_off = value_cast<Rep>(off);
    // Single flat: translate to owner's frame, apply policy, translate back.
    return bpo._bounds_(q + typed_off) - typed_off;
  }
  return q;
}

// Hidden-friend interface for `quantity_point`. Provides heterogeneous binary and comparison
// operators between any two `quantity_point<R1, PO1, Rep1>` and `quantity_point<R2, PO2, Rep2>`
// specializations (and between a `quantity_point` and a `quantity` or a `PointOrigin`). Made
// findable only via ADL.
struct quantity_point_iface {
  // operator+ (qp + q)
  template<auto R1, auto PO1, typename Rep1, auto R2, typename Rep2>
    requires ReferenceOf<MP_UNITS_REMOVE_CONST(decltype(R2)), PO1._quantity_spec_>
  [[nodiscard]] friend constexpr QuantityPoint auto operator+(const quantity_point<R1, PO1, Rep1>& qp,
                                                              const quantity<R2, Rep2>& q)
    requires requires { qp.quantity_ref_from(PO1) + q; }
  {
    return detail::make_quantity_point(qp.quantity_ref_from(PO1) + q, PO1);
  }

  // operator+ (q + qp)
  template<auto R1, typename Rep1, auto R2, auto PO2, typename Rep2>
    requires ReferenceOf<MP_UNITS_REMOVE_CONST(decltype(R1)), PO2._quantity_spec_>
  [[nodiscard]] friend constexpr QuantityPoint auto operator+(const quantity<R1, Rep1>& q,
                                                              const quantity_point<R2, PO2, Rep2>& qp)
    requires requires { q + qp.quantity_ref_from(PO2); }
  {
    return qp + q;
  }

  // operator- (qp - q)
  template<auto R1, auto PO1, typename Rep1, auto R2, typename Rep2>
    requires ReferenceOf<MP_UNITS_REMOVE_CONST(decltype(R2)), PO1._quantity_spec_>
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
             ReferenceOf<MP_UNITS_REMOVE_CONST(decltype(R1)), PO2::_quantity_spec_>
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
             ReferenceOf<MP_UNITS_REMOVE_CONST(decltype(R2)), PO1::_quantity_spec_>
  [[nodiscard]] friend constexpr Quantity auto operator-(PO1 po, const quantity_point<R2, PO2, Rep2>& qp)
  {
    return -(qp - po);
  }

  // operator==
  template<auto R1, auto PO1, typename Rep1, QuantityPointOf<mp_units::detail::get_absolute_point_origin(PO1)> QP2>
    requires std::equality_comparable_with<quantity<R1, Rep1>, typename QP2::quantity_type>
  [[nodiscard]] friend constexpr bool operator==(const quantity_point<R1, PO1, Rep1>& lhs, const QP2& rhs)
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
    requires requires { os << qp.quantity_from_unit_zero(); }
  {
    return os << qp.quantity_from_unit_zero();
  }

#endif  // MP_UNITS_HOSTED
};

}  // namespace detail

MP_UNITS_EXPORT_BEGIN

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
  static constexpr Dimension auto dimension = quantity_spec.dimension;
  static constexpr Unit auto unit = get_unit(reference);
  static constexpr PointOrigin auto absolute_point_origin = detail::get_absolute_point_origin(PO);
  static constexpr PointOrigin auto point_origin = PO;
  using rep = Rep;
  using quantity_type = quantity<reference, Rep>;

  quantity_type quantity_from_origin_is_an_implementation_detail_;  // needs to be public for a structural type

  // static member functions
  [[nodiscard]] static constexpr quantity_point min() noexcept
    requires requires { PO._bounds_.min; } || requires { quantity_type::min(); }
  {
    if constexpr (requires { PO._bounds_.min; }) {
      // zero_quantity_t signals a [0, ∞) policy: return the natural zero for this
      // quantity type directly — correct quantity spec, no unit scaling required.
      if constexpr (std::same_as<std::remove_cvref_t<decltype(PO._bounds_.min)>, detail::zero_quantity_t>)
        return {quantity_type::zero(), PO};
      else
        return {PO._bounds_.min.force_in(unit), PO};
    } else {
      return {quantity_type::min(), PO};
    }
  }

  [[nodiscard]] static constexpr quantity_point max() noexcept
    requires requires { PO._bounds_.max; } || requires { quantity_type::max(); }
  {
    if constexpr (requires { PO._bounds_.max; })
      return {PO._bounds_.max.force_in(unit), PO};
    else
      return {quantity_type::max(), PO};
  }

  [[nodiscard]] static constexpr quantity_point zero() noexcept
    requires(PO == default_point_origin(R)) && requires { quantity_type::zero(); }
  {
    return {quantity_type::zero(), PO};
  }

  // construction and assignment
  quantity_point() = default;

  template<typename FwdQ, QuantityOf<quantity_spec> Q = std::remove_cvref_t<FwdQ>>
    requires std::constructible_from<quantity_type, FwdQ> && (point_origin == default_point_origin(R))
  constexpr explicit quantity_point(FwdQ&& q) :
      quantity_from_origin_is_an_implementation_detail_(
        detail::enforce_bounds<point_origin>(quantity_type{std::forward<FwdQ>(q)}))
  {
  }

  template<typename FwdQ, QuantityOf<quantity_spec> Q = std::remove_cvref_t<FwdQ>>
    requires std::constructible_from<quantity_type, FwdQ>
  constexpr quantity_point(FwdQ&& q, decltype(PO)) :
      quantity_from_origin_is_an_implementation_detail_(
        detail::enforce_bounds<point_origin>(quantity_type{std::forward<FwdQ>(q)}))
  {
  }

  template<typename FwdQ, PointOrigin PO2, QuantityOf<PO2::_quantity_spec_> Q = std::remove_cvref_t<FwdQ>>
    requires std::constructible_from<quantity_type, FwdQ> && detail::SameAbsolutePointOriginAs<PO2, PO>
  constexpr quantity_point(FwdQ&& q, PO2) :
      quantity_point(
        quantity_point<std::remove_reference_t<Q>::reference, PO2{}, typename std::remove_reference_t<Q>::rep>{
          std::forward<FwdQ>(q), PO2{}})
  {
  }

  template<QuantityPointOf<absolute_point_origin> QP>
    requires std::constructible_from<quantity_type, typename QP::quantity_type>
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  constexpr explicit(!std::convertible_to<typename QP::quantity_type, quantity_type>) quantity_point(const QP& qp) :
      quantity_from_origin_is_an_implementation_detail_(detail::enforce_bounds<point_origin>([&] {
        if constexpr (point_origin == QP::point_origin)
          return quantity_type{qp.quantity_ref_from(point_origin)};
        else
          return quantity_type{qp - point_origin};
      }()))
  {
  }

  template<QuantityPoint QP>
    requires(!QuantityPointOf<QP, absolute_point_origin>) && requires(const QP& src) {
      { src.point_for(point_origin) } -> QuantityPointOf<point_origin>;
      quantity_type{src.point_for(point_origin) - point_origin};
    }
  constexpr explicit quantity_point(const QP& qp) :
      quantity_from_origin_is_an_implementation_detail_(detail::enforce_bounds<point_origin>([&] {
        auto at_dest = qp.point_for(point_origin);
        return quantity_type{at_dest - point_origin};
      }()))
  {
  }

  template<QuantityPointLike QP>
    requires(quantity_point_like_traits<QP>::point_origin == point_origin) &&
            std::constructible_from<quantity_type, quantity<quantity_point_like_traits<QP>::reference,
                                                            typename quantity_point_like_traits<QP>::rep>>
  constexpr explicit(
    quantity_point_like_traits<QP>::explicit_import ||
    !std::convertible_to<
      quantity<quantity_point_like_traits<QP>::reference, typename quantity_point_like_traits<QP>::rep>, quantity_type>)
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    quantity_point(const QP& qp) :
      quantity_from_origin_is_an_implementation_detail_(detail::enforce_bounds<point_origin>(quantity_type{
        quantity_point_like_traits<QP>::to_numerical_value(qp), get_unit(quantity_point_like_traits<QP>::reference)}))
  {
  }

  template<detail::SameAbsolutePointOriginAs<absolute_point_origin> NewPO>
  [[nodiscard]] constexpr QuantityPointOf<(NewPO{})> auto point_for(NewPO new_origin) const
  {
    if constexpr (is_same_v<NewPO, MP_UNITS_NONCONST_TYPE(point_origin)>)
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
#if __cpp_deleted_function
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

  [[nodiscard]] constexpr Quantity auto quantity_from_unit_zero() const
    requires requires { unit._point_origin_; } || (PO == default_point_origin(R))
  {
    if constexpr (requires { unit._point_origin_; }) {
      // original quantity point unit can be lost in the below operation
      const auto q = quantity_from(unit._point_origin_);
      if constexpr (requires { q.in(unit); })
        // restore the unit if possible (non-truncating)
        return q.in(unit);
      else
        return q;
    } else
      return quantity_from(absolute_point_origin);
  }

  [[deprecated("2.6.0: Use `quantity_from_unit_zero` instead")]] [[nodiscard]] constexpr Quantity auto
  quantity_from_zero() const
  {
    return quantity_from_unit_zero();
  }

  // unit conversions
  template<UnitOf<quantity_spec> ToU>
    requires detail::ImplicitScaling<unit, ToU{}, rep>
  [[nodiscard]] constexpr QuantityPointOf<quantity_spec> auto in(ToU) const
  {
    return ::mp_units::quantity_point{quantity_ref_from(point_origin).in(ToU{}), point_origin};
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
    return ::mp_units::quantity_point{quantity_ref_from(point_origin).template in<ToRep>(ToU{}), point_origin};
  }

  template<UnitOf<quantity_spec> ToU>
    requires detail::ExplicitlyCastable<unit, ToU{}, rep>
  [[nodiscard]] constexpr QuantityPointOf<quantity_spec> auto force_in(ToU) const
  {
    return ::mp_units::quantity_point{quantity_ref_from(point_origin).force_in(ToU{}), point_origin};
  }

  template<RepresentationOf<quantity_spec> ToRep>
    requires std::constructible_from<ToRep, rep>
  [[nodiscard]] constexpr QuantityPointOf<quantity_spec> auto force_in() const
  {
    return ::mp_units::quantity_point{quantity_ref_from(point_origin).template force_in<ToRep>(), point_origin};
  }

  template<RepresentationOf<quantity_spec> ToRep, UnitOf<quantity_spec> ToU>
    requires std::constructible_from<ToRep, rep> && detail::ExplicitlyCastable<unit, ToU{}, rep>
  [[nodiscard]] constexpr QuantityPointOf<quantity_spec> auto force_in(ToU) const
  {
    return ::mp_units::quantity_point{quantity_ref_from(point_origin).template force_in<ToRep>(ToU{}), point_origin};
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
  {
    ++quantity_from_origin_is_an_implementation_detail_;
    quantity_from_origin_is_an_implementation_detail_ =
      detail::enforce_bounds<point_origin>(quantity_from_origin_is_an_implementation_detail_);
    return *this;
  }

  [[nodiscard]] constexpr quantity_point operator++(int)
    requires requires { quantity_from_origin_is_an_implementation_detail_++; }
  {
    auto old_quantity = quantity_from_origin_is_an_implementation_detail_;
    [[maybe_unused]] auto _ = quantity_from_origin_is_an_implementation_detail_++;
    quantity_from_origin_is_an_implementation_detail_ =
      detail::enforce_bounds<point_origin>(quantity_from_origin_is_an_implementation_detail_);
    return {old_quantity, PO};
  }

  constexpr quantity_point& operator--() &
    requires requires { --quantity_from_origin_is_an_implementation_detail_; }
  {
    --quantity_from_origin_is_an_implementation_detail_;
    quantity_from_origin_is_an_implementation_detail_ =
      detail::enforce_bounds<point_origin>(quantity_from_origin_is_an_implementation_detail_);
    return *this;
  }

  [[nodiscard]] constexpr quantity_point operator--(int)
    requires requires { quantity_from_origin_is_an_implementation_detail_--; }
  {
    auto old_quantity = quantity_from_origin_is_an_implementation_detail_;
    [[maybe_unused]] auto _ = quantity_from_origin_is_an_implementation_detail_--;
    quantity_from_origin_is_an_implementation_detail_ =
      detail::enforce_bounds<point_origin>(quantity_from_origin_is_an_implementation_detail_);
    return {old_quantity, PO};
  }

  // compound assignment operators
  template<auto R2, typename Rep2>
    requires(mp_units::implicitly_convertible(get_quantity_spec(R2), quantity_spec)) &&
            detail::ImplicitConversion<get_unit(R2), Rep2, unit, rep> &&
            requires(const quantity_type q) { quantity_from_origin_is_an_implementation_detail_ += q; }
  constexpr quantity_point& operator+=(const quantity<R2, Rep2>& q) &
  {
    quantity_from_origin_is_an_implementation_detail_ += q;
    quantity_from_origin_is_an_implementation_detail_ =
      detail::enforce_bounds<point_origin>(quantity_from_origin_is_an_implementation_detail_);
    return *this;
  }

  template<auto R2, typename Rep2>
    requires(mp_units::implicitly_convertible(get_quantity_spec(R2), quantity_spec)) &&
            detail::ImplicitConversion<get_unit(R2), Rep2, unit, rep> &&
            requires(const quantity_type q) { quantity_from_origin_is_an_implementation_detail_ -= q; }
  constexpr quantity_point& operator-=(const quantity<R2, Rep2>& q) &
  {
    quantity_from_origin_is_an_implementation_detail_ -= q;
    quantity_from_origin_is_an_implementation_detail_ =
      detail::enforce_bounds<point_origin>(quantity_from_origin_is_an_implementation_detail_);
    return *this;
  }
};

// CTAD
#ifdef MP_UNITS_COMP_MSVC
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
    requires requires { PO._bounds_.min; } || requires { std::numeric_limits<mp_units::quantity<R, Rep>>::lowest(); }
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
      qp.quantity_from_unit_zero(), ctx);
  }
};

#endif  // MP_UNITS_HOSTED
