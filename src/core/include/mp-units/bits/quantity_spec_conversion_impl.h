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

// IWYU pragma: private, include <mp-units/framework/quantity_spec.h>
// The conversion dispatcher: routes between the in-hierarchy engine
// (quantity_spec_hierarchy_conversion.h) and the equation engine
// (quantity_spec_equation_conversion.h), and computes common quantity specs.
#include <mp-units/bits/hacks.h>
#include <mp-units/bits/quantity_spec_equation_conversion.h>
#include <mp-units/bits/quantity_spec_hierarchy.h>
#include <mp-units/ext/algorithm.h>
#include <mp-units/ext/type_name.h>
#include <mp-units/ext/type_traits.h>
#include <mp-units/framework/quantity_spec_concepts.h>
#include <mp-units/framework/quantity_spec_definitions.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <type_traits>
#endif
#endif

namespace mp_units::detail {

// verifies convertibility when at least one of the specs is a kind
template<QuantitySpec From, QuantitySpec To>
[[nodiscard]] consteval specs_convertible_result convertible_kinds(From from, To)
{
  constexpr auto from_root = detail::get_kind_tree_root(From{});
  constexpr auto to_root = detail::get_kind_tree_root(To{});
  using enum specs_convertible_result;

  if constexpr (QuantityKindSpec<From>) {
    auto res = detail::convertible(from_root, to_root);
    // the below does not account for `explicit_conversion_beyond_kind`
    return (res == explicit_conversion) ? yes : res;
  } else {
    auto res = detail::convertible(from, to_root);
    // Promote explicit_conversion → yes when kind_of<> wraps a named quantity and from is not an ancestor of to_root.
    // Skip the promotion when to_root is derived (the explicit_conversion reflects a cross-kind distance that matters)
    // or when from is a direct ancestor of to_root (parent→child downcast remains explicit_conversion).
    if constexpr (NamedQuantitySpec<MP_UNITS_NONCONST_TYPE(to_root)> &&
                  !detail::is_child_of(MP_UNITS_NONCONST_TYPE(to_root){}, From{}))
      return (res == explicit_conversion) ? yes : res;
    return res;
  }
}

template<NamedQuantitySpec From, NamedQuantitySpec To>
[[nodiscard]] consteval specs_convertible_result convertible_named(From from, To to)
{
  using enum specs_convertible_result;

  if constexpr (detail::have_common_base(From{}, To{}))
    return detail::convertible_common_base(From{}, To{});
  else if constexpr (detail::get_kind_tree_root(From{}) != detail::get_kind_tree_root(To{}))
    return no;
  else {
    constexpr auto from_complexity = detail::get_complexity(From{});
    constexpr auto to_complexity = detail::get_complexity(To{});
    static_assert(from_complexity != to_complexity,
                  "Is it possible to have different quantities of the same complexity here?");
    if constexpr (from_complexity > to_complexity)
      return detail::convertible(explode<to_complexity>(from).quantity, to);
    else {
      auto res = detail::explode<from_complexity>(to);
      return detail::min(res.result, detail::convertible(from, res.quantity));
    }
  }
}


template<QuantitySpec From, QuantitySpec To>
[[nodiscard]] consteval specs_convertible_result convertible_impl(From from, To to)
{
  using enum specs_convertible_result;

  // NOLINTBEGIN(bugprone-branch-clone)
  if constexpr (From{} == To{})
    return yes;
  else if constexpr (get_dimension(From{}) != get_dimension(To{}))
    return no;
  else if constexpr (QuantityKindSpec<From> || QuantityKindSpec<To>)
    return detail::convertible_kinds(from, to);
  else if constexpr (NamedQuantitySpec<From> && NamedQuantitySpec<To>)
    return detail::convertible_named(from, to);
  else
    return detail::are_ingredients_convertible(from, to);
  // NOLINTEND(bugprone-branch-clone)
}

template<QuantitySpec From, QuantitySpec To>
constexpr specs_convertible_result convertible_result = detail::convertible_impl(From{}, To{});

template<QuantitySpec From, QuantitySpec To>
[[nodiscard]] consteval specs_convertible_result convertible(From, To)
{
  return convertible_result<From, To>;
}

[[nodiscard]] consteval bool explicitly_convertible_to_dimensionless(QuantitySpec auto qs)
{
  return mp_units::explicitly_convertible(qs, dimensionless);
}

struct no_common_quantity_spec {};

template<QuantitySpec Q1, QuantitySpec Q2>
[[nodiscard]] consteval auto get_common_quantity_spec_impl(Q1 q1, Q2 q2)
{
  if constexpr (std::is_same_v<Q1, Q2>)
    return q1;
  else {
    constexpr bool q1q2 = mp_units::implicitly_convertible(Q1{}, Q2{});
    constexpr bool q2q1 = mp_units::implicitly_convertible(Q2{}, Q1{});

    // in case of interconvertible quantities we try to figure out which one is a better choice
    if constexpr (q1q2 && q2q1) {
      // quantity kinds have lower priority
      if constexpr (detail::QuantityKindSpec<Q1> && !detail::QuantityKindSpec<Q2>)
        return q2;
      else if constexpr (!detail::QuantityKindSpec<Q1> && detail::QuantityKindSpec<Q2>)
        return q1;
      else {
        using q1_type = decltype(detail::remove_kind(q1));
        using q2_type = decltype(detail::remove_kind(q2));
        // derived quantities have a lower priority
        if constexpr (detail::NamedQuantitySpec<q1_type> && detail::DerivedQuantitySpec<q2_type>)
          return q1;
        else if constexpr (detail::DerivedQuantitySpec<q1_type> && detail::NamedQuantitySpec<q2_type>)
          return q2;
        else
          // TODO Check if there is a better choice here
          return detail::better_type_name(q1, q2);
      }
    }
    // we prefer a quantity to which we can convert the other quantity
    else if constexpr (q1q2)
      return q2;
    else if constexpr (q2q1)
      return q1;
    // if quantities can't be converted in any direction check if they have a common base in the tree
    else if constexpr (detail::have_common_base(Q1{}, Q2{})) {
      constexpr auto base = detail::get_common_base(Q1{}, Q2{});
      if constexpr (mp_units::implicitly_convertible(Q1{}, base) && mp_units::implicitly_convertible(Q2{}, base))
        return base;
      else
        return no_common_quantity_spec{};
    } else {
      // verify kind tree roots as the last resort check
      constexpr auto q1_root = detail::get_kind_tree_root(Q1{});
      constexpr auto q2_root = detail::get_kind_tree_root(Q2{});
      if constexpr (mp_units::implicitly_convertible(q1_root, q2_root))
        return q2_root;
      else if constexpr (mp_units::implicitly_convertible(q2_root, q1_root))
        return q1_root;
      else
        return no_common_quantity_spec{};
    }
  }
}

template<QuantitySpec Q1, QuantitySpec Q2>
constexpr auto get_common_quantity_spec_result = detail::get_common_quantity_spec_impl(Q1{}, Q2{});

template<QuantitySpec Q1, QuantitySpec Q2>
[[nodiscard]] consteval bool have_common_quantity_spec(Q1, Q2)
{
  return !std::is_same_v<decltype(detail::get_common_quantity_spec_result<Q1, Q2>), const no_common_quantity_spec>;
}

}  // namespace mp_units::detail
