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
#include <mp-units/bits/quantity_spec_hierarchy.h>
#include <mp-units/ext/type_traits.h>
#include <mp-units/framework/quantity_spec_concepts.h>
#include <mp-units/framework/quantity_spec_definitions.h>
#include <mp-units/framework/symbolic_expression.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cstdint>
#include <type_traits>
#endif
#endif

// The half of the conversion engine that reads only the hierarchy: kind roots, the tree rule for
// two named quantity specifications, and the kind rules for a `kind_of<K>` whose `K` is named.
namespace mp_units::detail {

// The four outcomes a user sees. The public predicates are thresholds on this order.
enum class conversion_rank : std::int8_t { none, cast, explicit_only, implicit };

// The rank of a conversion is its weakest step.
[[nodiscard]] consteval conversion_rank weaker(conversion_rank lhs, conversion_rank rhs)
{
  return lhs < rhs ? lhs : rhs;
}

// declared kinds

template<typename T>
struct is_declared_kind_impl : std::false_type {};

#if MP_UNITS_API_NO_CRTP
template<auto... Args>
struct is_declared_kind_impl<quantity_spec<Args...>> :
    std::bool_constant<mp_units::contains<struct is_kind, Args...>()> {};
#else
template<typename Self, auto... Args>
struct is_declared_kind_impl<quantity_spec<Self, Args...>> :
    std::bool_constant<mp_units::contains<struct is_kind, Args...>()> {};
#endif

template<typename Q>
concept DeclaredKind = is_declared_kind_impl<typename Q::_base_type_>::value;

// kind roots

template<typename Q>
struct kind_root_impl : std::type_identity<Q> {};

template<HasParent Q>
  requires(!DeclaredKind<Q>)
struct kind_root_impl<Q> : kind_root_impl<parent_of<Q>> {};

// A kind factor wraps a kind root, and an open factor has the kind root of what it wraps.
template<typename N>
struct kind_root_impl<kind_of_<N>> : std::type_identity<N> {};

template<typename N>
struct kind_root_impl<open_factor<N>> : kind_root_impl<N> {};

// The nearest ancestor-or-self declared with `is_kind`, else the root of the tree.
template<typename Q>
using kind_root = kind_root_impl<Q>::type;

template<QuantitySpec Q>
using to_kind_root = kind_root<Q>;

template<QuantitySpec Q>
[[nodiscard]] consteval QuantitySpec auto get_kind_tree_root(Q q)
{
  if constexpr (QuantityKindSpec<Q>)
    // `kind_of_` requires a kind tree root, so the wrapped specification is one
    return Q::_quantity_spec_;
  else if constexpr (DerivedQuantitySpec<Q>)
    // builds a derived specification because `get_kind` needs one
    return detail::expr_map<to_kind_root, derived_quantity_spec, struct dimensionless, type_list_of_quantity_spec_less>(
      q);
  else
    return kind_root<Q>{};
}

// The tree rule: two named quantity specifications relate through their positions in the tree
// alone. Equations are never consulted here.

// A source `kind_of<K>` has not committed to a position below `K`, so a downcast from it is implicit.
// Leaving a declared kind on the way up stays explicit for any source.
template<typename From, typename To, bool SourceIsKind = false>
[[nodiscard]] consteval conversion_rank convertible_named()
{
  using enum conversion_rank;
  if constexpr (std::is_same_v<From, To>)
    return implicit;
  else if constexpr (!std::is_same_v<hierarchy_root<From>, hierarchy_root<To>>)
    return none;
  else if constexpr (detail::is_ancestor_or_self<To, From>())
    return std::is_same_v<kind_root<From>, kind_root<To>> ? implicit : explicit_only;
  else if constexpr (detail::is_ancestor_or_self<From, To>())
    return SourceIsKind ? implicit : explicit_only;
  else
    return std::is_same_v<kind_root<From>, kind_root<To>> ? cast : none;
}

// The kind rules for a named kind root `K`. `kind_of<K>` means "any quantity of the tree of `K`,
// which one is not known yet". It has no fixed position in the tree and so no sibling. A
// conversion between a kind of a named root and a named quantity never ranks `cast`. (A kind of a
// derived root goes through the equation rules instead, where a factor can meet a sibling:
// `kind_of<acceleration * time>` to `special_speed` meets `velocity` as a sibling.)

// `kind_of<K> -> To` (and `kind_of<K> -> kind_of<To>`): the kind may be specialized freely, through
// sub-kind roots as well. Above its root it leaves a declared kind.
template<typename K, typename To>
[[nodiscard]] consteval conversion_rank convertible_from_kind()
{
  using enum conversion_rank;
  if constexpr (detail::is_ancestor_or_self<K, To>())
    return implicit;
  else if constexpr (detail::is_ancestor_or_self<To, K>())
    return explicit_only;
  else
    return none;
}

// `From -> kind_of<K>` ranks as `From -> K` does.
template<typename From, typename K>
[[nodiscard]] consteval conversion_rank convertible_to_kind()
{
  using enum conversion_rank;
  if constexpr (detail::is_ancestor_or_self<K, From>())
    return std::is_same_v<kind_root<From>, K> ? implicit : explicit_only;
  else if constexpr (detail::is_ancestor_or_self<From, K>())
    return explicit_only;
  else
    return none;
}

}  // namespace mp_units::detail
