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
#include <mp-units/bits/unsatisfied.h>
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/quantity_character.h>
#include <mp-units/framework/quantity_spec.h>
#include <mp-units/framework/quantity_spec_concepts.h>
#include <mp-units/framework/reference.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cstddef>
#include <tuple>
#endif
#endif

namespace mp_units {

namespace detail {

// Element access for a vector representation. Prefers `get<I>(v)` (compile-time index, allows
// bounds-checking on tuple-like reps), and falls back to `v[I]`. A rep providing neither is not
// decomposable.
template<std::size_t Idx, typename V>
[[nodiscard]] constexpr decltype(auto) component_access(const V& v)
  requires requires { get<Idx>(v); } || requires { v[Idx]; }
{
  if constexpr (requires { get<Idx>(v); })
    return get<Idx>(v);
  else
    return v[Idx];
}

// Compile-time number of elements of a representation, read from `std::tuple_size` when the rep
// models the tuple protocol, otherwise `dynamic_extent` (unknown, so the count guard is skipped).
inline constexpr std::size_t dynamic_extent = static_cast<std::size_t>(-1);

template<typename V>
[[nodiscard]] consteval std::size_t extent()
{
  if constexpr (requires { std::tuple_size<V>::value; })
    return std::tuple_size<V>::value;
  else
    return dynamic_extent;
}

// Number of values in `Others` equal to `Root` (used for the kind/root validity checks).
template<auto Root, auto... Others>
[[nodiscard]] consteval std::size_t count_equal()
{
  return (std::size_t{0} + ... + (Root == Others ? std::size_t{1} : std::size_t{0}));
}

#if !MP_UNITS_HAS_PACK_INDEXING

// Portable fallback for `Axes...[I]` on implementations without C++26 pack indexing.
template<std::size_t Idx, auto V0, auto... Vs>
[[nodiscard]] consteval auto nth_value()
{
  if constexpr (Idx == 0)
    return V0;
  else
    return nth_value<Idx - 1, Vs...>();
}

#endif

// Axis-intrinsic validity of a component axis list (independent of the whole and the
// representation): there are at least two axes, each is a concrete vector quantity (not a `kind_of<>`
// quantity kind), the axes share a common hierarchy root (are the same kind of quantity), and they
// are pairwise-distinct kinds.
// TODO Once point and delta quantity specs are introduced, exclude those as component axes too.
template<auto... Axes>
concept ValidVectorAxes =
  ((sizeof...(Axes) >= 2) || unsatisfied<"a vector decomposition needs at least two component axes">()) &&
  ((... && admits_only(Axes._character_.order, quantity_tensor_order::vector)) ||
   unsatisfied<"every component axis must be a vector quantity">()) &&
  ((... && !QuantityKindSpec<MP_UNITS_NTTP_HACK(decltype(Axes))>) ||
   unsatisfied<"a component axis cannot be a kind_of<> quantity kind">()) &&
  ((... && (count_equal<get_hierarchy_root(Axes), get_hierarchy_root(Axes)...>() == sizeof...(Axes))) ||
   unsatisfied<"all component axes must share a common hierarchy root (be the same kind of quantity)">()) &&
  ((... && (count_equal<get_kind_tree_root(Axes), get_kind_tree_root(Axes)...>() == 1)) ||
   unsatisfied<"component axes must be distinct kinds (declare each with is_kind)">());

}  // namespace detail

/**
 * @brief Ordered list of the 1D-vector component axes a vector quantity decomposes into
 *
 * A user opts a vector whole into decomposition by specializing `vector_components` (the
 * customization point in `customization_points.h`) and inheriting from this template, listing the
 * per-axis quantity specs in coordinate order:
 *
 * @code
 * template<>
 * struct mp_units::vector_components<flight_velocity> :
 *     mp_units::vector_axes<forward_velocity, wind_drift, sink_rate> {};
 * @endcode
 *
 * The axis-intrinsic preconditions are enforced here: every axis must be a vector quantity, the
 * axes must share a common hierarchy root (be the same kind of quantity), and they must be
 * pairwise-distinct kinds (declare each with `is_kind`). A specialization that violates them is
 * ill-formed at the point of definition. The relational preconditions (axes share the whole's
 * hierarchy root, differ from the whole's kind, and fit the representation) need the whole and the
 * representation, so they are enforced when a `quantity` is actually decomposed.
 *
 * Specializing this template is optional. A vector quantity without one still decomposes, with each
 * component carrying the whole's kind instead of an axis of its own, and access by axis spec
 * unavailable. The declaration buys that per-axis type safety.
 */
MP_UNITS_EXPORT template<QuantitySpec auto... Axes>
  requires detail::ValidVectorAxes<Axes...>
struct vector_axes {
  static constexpr std::size_t size = sizeof...(Axes);

  // the I-th axis spec (C++26 pack indexing with a portable recursive fallback)
  template<std::size_t Idx>
  static constexpr QuantitySpec auto axis =
#if MP_UNITS_HAS_PACK_INDEXING
    Axes...[Idx];
#else
    detail::nth_value<Idx, Axes...>();
#endif
};

namespace detail {

// Relational validity and lookup helpers used by `Decomposable` and `get`. They take the
// `vector_axes` base by value so the axis pack is deduced, keeping them off the user-facing type.
template<QuantitySpec auto Whole, QuantitySpec auto... Axes>
[[nodiscard]] consteval bool axes_share_hierarchy_root(vector_axes<Axes...>)
{
  return (... && (get_hierarchy_root(Axes) == get_hierarchy_root(Whole)));
}

template<QuantitySpec auto Whole, QuantitySpec auto... Axes>
[[nodiscard]] consteval bool axes_distinct_from_whole_kind(vector_axes<Axes...>)
{
  return (... && (get_kind_tree_root(Axes) != get_kind_tree_root(Whole)));
}

template<QuantitySpec auto QS, QuantitySpec auto... Axes>
[[nodiscard]] consteval std::size_t axis_index_of(vector_axes<Axes...>)
{
  std::size_t i = 0, found = sizeof...(Axes);
  ((QS == Axes ? found = i : ++i), ...);
  return found;
}

// The user opted the whole into a *named* decomposition by specializing `vector_components`. This
// picks between the two decomposition modes below, and it is deliberately message-free: a quantity
// with no specialization qualifies for the unnamed mode instead.
template<auto QS>
concept HasVectorAxes = QuantitySpec<MP_UNITS_NTTP_HACK(decltype(QS))> && requires { vector_components<QS>::size; };

// The named mode of the decomposition `get`/tuple protocol: the quantity's spec has a
// `vector_components` specialization, the representation is element-accessible, and the axes are
// relationally valid against the whole. Only the quantity spec is needed (never the unit), so it is
// taken directly rather than the full reference. `QS` is an unconstrained `auto` because a
// constrained `auto` is not allowed on a concept's own template parameter.
template<auto QS, typename Rep>
concept NamedDecomposable =
  HasVectorAxes<QS> &&
  (is_derived_from_specialization_of_v<vector_components<QS>, vector_axes> ||
   unsatisfied<"vector_components must inherit from a vector_axes specialization">()) &&
  (requires(const Rep& r) { component_access<0>(r); } ||
   unsatisfied<"the representation type is not element-accessible (needs get<I> or operator[])">()) &&
  (axes_share_hierarchy_root<QS>(vector_components<QS>{}) ||
   unsatisfied<"every component axis must share the whole's hierarchy root">()) &&
  (axes_distinct_from_whole_kind<QS>(vector_components<QS>{}) ||
   unsatisfied<"every component axis must be a different kind than the whole (declare it with is_kind)">()) &&
  (extent<Rep>() == dynamic_extent || vector_components<QS>::size <= extent<Rep>() ||
   unsatisfied<"more component axes declared than the representation holds">());

// A representation that can supply the components of an undeclared decomposition: an order-1
// container that is element-accessible and states its length at compile time. The length has to come
// from here because there is no axis count to bound a compile-time index against, and
// `std::tuple_size` is the tuple protocol's own way to spell it. Written as nested requirements so
// that a type for which `tensor_order` is unspecified is simply not satisfied rather than a hard
// error.
template<typename Rep>
concept ComponentAccessible = requires {
  requires tensor_order<Rep> == 1;
  requires requires(const Rep& r) { component_access<0>(r); };
  requires extent<Rep>() != dynamic_extent;
};

// The unnamed mode: no axes are declared, so a component carries the whole's kind rather than an axis
// of its own, and eligibility is read off the representation alone. The whole's
// own character is not checked: it is the vector one whenever the spec is concrete
// (`RepresentationOf` already pins order there), and a quantity kind is deliberately
// character-agnostic (`cartesian_vector{1., 2., 3.} * m` is a vector quantity of the length kind).
//
// Unlike the named mode this is message-free, because it is also used as a *query*: `std::tuple_size`
// of a quantity is constrained on it, so an ordinary scalar quantity has to answer "no" rather than
// diagnose.
template<auto QS, typename Rep>
concept UnnamedDecomposable =
  QuantitySpec<MP_UNITS_NTTP_HACK(decltype(QS))> && !HasVectorAxes<QS> && ComponentAccessible<Rep>;

template<auto QS, typename Rep>
concept Decomposable = NamedDecomposable<QS, Rep> || UnnamedDecomposable<QS, Rep>;

// The number of components: the declared axis count in the named mode, the representation's own
// length in the unnamed one.
template<auto QS, typename Rep>
  requires Decomposable<QS, Rep>
[[nodiscard]] consteval std::size_t decomposition_size()
{
  if constexpr (NamedDecomposable<QS, Rep>)
    return vector_components<QS>::size;
  else
    return extent<Rep>();
}

// The quantity spec the `Idx`-th component carries: its declared axis in the named mode, the whole's
// *kind* in the unnamed one. The kind is the honest answer, because a component is not the whole: an
// undeclared decomposition knows the component belongs to the whole's kind, but nothing entitles it
// to claim the whole's own spec. Handing back `QS` would assert that a single component *is* an
// `isq::velocity`, letting it stand in wherever the complete vector quantity is expected.
//
// The component keeps the vector character even though that kind is rooted in a scalar quantity
// (`get_kind(isq::velocity)` is the `isq::speed` kind), because a kind admits every character and the
// representation decides. `get_kind` rather than `kind_of<QS>`: the latter requires `QS` to be its
// own tree root, which a vector spec never is.
template<auto QS, typename Rep, std::size_t Idx>
  requires Decomposable<QS, Rep>
[[nodiscard]] consteval QuantitySpec auto component_spec()
{
  if constexpr (NamedDecomposable<QS, Rep>)
    return vector_components<QS>::template axis<Idx>;
  else
    return get_kind(QS);
}

// `Decomposable` plus a bounds check. The bound is guarded behind `Decomposable` so it is only
// formed once that holds (it short-circuits for non-decomposable specs, avoiding a hard error).
template<auto QS, typename Rep, std::size_t Idx>
concept DecomposableIndex = Decomposable<QS, Rep> && (Idx < decomposition_size<QS, Rep>());

// Lookup by axis spec is a named-mode privilege: an unnamed decomposition has no axis to name.
template<auto QS, typename Rep, auto Axis>
concept DecomposableAxis = NamedDecomposable<QS, Rep> && QuantitySpec<MP_UNITS_NTTP_HACK(decltype(Axis))> &&
                           (axis_index_of<Axis>(vector_components<QS>{}) < vector_components<QS>::size);

}  // namespace detail

}  // namespace mp_units
