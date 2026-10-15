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

#include <mp-units/bits/quantity_spec_equation_conversion.h>
#include <mp-units/bits/quantity_spec_hierarchy.h>
#include <mp-units/bits/quantity_spec_hierarchy_conversion.h>
#include <mp-units/compat_macros.h>
#include <mp-units/ext/type_name.h>
#include <mp-units/framework/quantity_spec_concepts.h>
#include <mp-units/framework/quantity_spec_definitions.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <type_traits>
#endif
#endif

// The entry points of the conversion engine: `convertible` dispatches to the tree rule, the kind
// rules, or the equation rule, and the common quantity specification is derived from it.
namespace mp_units::detail {

template<QuantitySpec From, QuantitySpec To>
[[nodiscard]] consteval conversion_rank convertible(From, To)
{
  using enum conversion_rank;
  if constexpr (std::is_same_v<From, To>)
    return implicit;
  else if constexpr (!std::is_same_v<MP_UNITS_NONCONST_TYPE(From::_dimension_),
                                     MP_UNITS_NONCONST_TYPE(To::_dimension_)>)
    return none;
  else if constexpr (QuantityKindSpec<From> || QuantityKindSpec<To>) {
    // The kind rules with a `kind_of<>` on at least one side. A kind root `K` is a named quantity or a
    // derived specification of kind roots. When either side is derived the equation rule runs on `K`
    // itself, told that the source is a kind, so that each downcast step from it ranks `implicit` (see
    // the kind rules in `quantity_spec_hierarchy_conversion.h`). They are inlined here rather than
    // a function of their own, which would be one more instantiation per conversion.
    using from_root = decltype(detail::remove_kind(From{}));
    using to_root = decltype(detail::remove_kind(To{}));
    if constexpr (NamedQuantitySpec<from_root> && NamedQuantitySpec<to_root>) {
      if constexpr (QuantityKindSpec<From>)
        return detail::with_kind_boundary<from_root, to_root, detail::convertible_from_kind<from_root, to_root>(),
                                          true>();
      else
        return detail::with_kind_boundary<from_root, to_root, detail::convertible_to_kind<from_root, to_root>()>();
    } else
      return detail::convertible_by_equations<from_root, to_root, QuantityKindSpec<From>, QuantityKindSpec<To>>();
  } else if constexpr (NamedQuantitySpec<From> && NamedQuantitySpec<To>)
    return detail::with_kind_boundary<From, To, detail::convertible_named<From, To>()>();
  else
    return detail::convertible_by_equations<From, To>();
}

[[nodiscard]] consteval bool explicitly_convertible_to_dimensionless(QuantitySpec auto qs)
{
  return detail::convertible(qs, dimensionless) >= conversion_rank::explicit_only;
}

// common quantity specification

struct no_common_quantity_spec {};

template<QuantitySpec Q1, QuantitySpec Q2>
[[nodiscard]] consteval auto common_quantity_spec_of_unrelated();

template<QuantitySpec Q1, QuantitySpec Q2>
[[nodiscard]] consteval auto common_quantity_spec()
{
  using enum conversion_rank;
  if constexpr (std::is_same_v<Q1, Q2>)
    return Q1{};
  else {
    constexpr bool to_second = detail::convertible(Q1{}, Q2{}) == implicit;
    constexpr bool to_first = detail::convertible(Q2{}, Q1{}) == implicit;
    if constexpr (to_second && to_first) {
      // interconvertible: prefer a non-kind over a kind, then a named over a derived
      if constexpr (QuantityKindSpec<Q1> && !QuantityKindSpec<Q2>)
        return Q2{};
      else if constexpr (QuantityKindSpec<Q2> && !QuantityKindSpec<Q1>)
        return Q1{};
      else if constexpr (NamedQuantitySpec<Q1> && !NamedQuantitySpec<Q2>)
        return Q1{};
      else if constexpr (NamedQuantitySpec<Q2> && !NamedQuantitySpec<Q1>)
        return Q2{};
      else
        return detail::better_type_name(Q1{}, Q2{});
    } else if constexpr (to_second)
      return Q2{};
    else if constexpr (to_first)
      return Q1{};
    else
      return detail::common_quantity_spec_of_unrelated<Q1, Q2>();
  }
}

// Neither converts implicitly to the other: the nearest common ancestor of two named
// specifications of one tree when both reach it implicitly, else the kind root of one when the
// kind root of the other converts to it implicitly.
template<QuantitySpec Q1, QuantitySpec Q2>
[[nodiscard]] consteval auto common_quantity_spec_of_unrelated()
{
  using enum conversion_rank;
  constexpr auto by_kind_roots = [] {
    constexpr QuantitySpec auto kind_root1 = detail::get_kind_tree_root(Q1{});
    constexpr QuantitySpec auto kind_root2 = detail::get_kind_tree_root(Q2{});
    if constexpr (detail::convertible(kind_root2, kind_root1) == implicit)
      return kind_root1;
    else if constexpr (detail::convertible(kind_root1, kind_root2) == implicit)
      return kind_root2;
    else
      return no_common_quantity_spec{};
  };
  if constexpr (NamedQuantitySpec<Q1> && NamedQuantitySpec<Q2> &&
                std::is_same_v<hierarchy_root<Q1>, hierarchy_root<Q2>>) {
    constexpr QuantitySpec auto ancestor = detail::nearest_common_ancestor<Q1, Q2>();
    if constexpr (detail::convertible(Q1{}, ancestor) == implicit && detail::convertible(Q2{}, ancestor) == implicit)
      return ancestor;
    else
      return by_kind_roots();
  } else
    return by_kind_roots();
}

template<QuantitySpec Q1, QuantitySpec Q2>
constexpr auto get_common_quantity_spec_result = detail::common_quantity_spec<Q1, Q2>();

template<QuantitySpec Q1, QuantitySpec Q2>
[[nodiscard]] consteval bool have_common_quantity_spec(Q1, Q2)
{
  return !std::is_same_v<decltype(detail::common_quantity_spec<Q1, Q2>()), no_common_quantity_spec>;
}

}  // namespace mp_units::detail
