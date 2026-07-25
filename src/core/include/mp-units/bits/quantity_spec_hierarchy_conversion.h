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
// The in-hierarchy part of the conversion engine: kind-tree roots and conversions between
// named quantities sharing a common base. In V3 this maps onto the C++ type system
// (inheritance + is_base_of/is_convertible), so this file is the part that gets replaced by
// language features.
#include <mp-units/bits/hacks.h>
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
#endif
#endif

namespace mp_units::detail {

enum class [[nodiscard]] specs_convertible_result : std::int8_t {
  no,
  cast,
  explicit_conversion_beyond_kind,
  explicit_conversion,
  yes
};

#if MP_UNITS_API_NO_CRTP
template<NamedQuantitySpec auto QS, auto... Args>
[[nodiscard]] consteval bool defined_as_kind_impl(quantity_spec<QS, Args...>)
#else
template<typename Self, NamedQuantitySpec auto QS, auto... Args>
[[nodiscard]] consteval bool defined_as_kind_impl(quantity_spec<Self, QS, Args...>)
#endif
{
  return mp_units::contains<struct is_kind, Args...>();
}

template<QuantitySpec Q>
  requires requires(Q q) { detail::get_kind_tree_root(q); }
using to_kind = decltype(detail::get_kind_tree_root(Q{}));

template<QuantitySpec Q>
[[nodiscard]] consteval QuantitySpec auto get_kind_tree_root_impl(Q q)
{
  auto defined_as_kind = []<typename QQ>(QQ qq) {
    if constexpr (requires { detail::defined_as_kind_impl(qq); })
      return detail::defined_as_kind_impl(QQ{});
    else
      return false;
  };

  // NOLINTBEGIN(bugprone-branch-clone)
  if constexpr (QuantityKindSpec<Q>) {
    return detail::remove_kind(q);
  } else if constexpr (defined_as_kind(Q{})) {
    return q;
  } else if constexpr (requires { Q::_parent_; }) {
    return detail::get_kind_tree_root(Q::_parent_);
  } else if constexpr (DerivedQuantitySpec<Q>) {
    return detail::expr_map<to_kind, derived_quantity_spec, struct dimensionless, type_list_of_quantity_spec_less>(q);
  } else {
    // root quantity
    return q;
  }
  // NOLINTEND(bugprone-branch-clone)
}

template<QuantitySpec Q>
constexpr QuantitySpec auto get_kind_tree_root_result = detail::get_kind_tree_root_impl(Q{});

template<QuantitySpec Q>
[[nodiscard]] consteval QuantitySpec auto get_kind_tree_root(Q)
{
  return get_kind_tree_root_result<Q>;
}

template<NamedQuantitySpec From, NamedQuantitySpec To>
[[nodiscard]] consteval specs_convertible_result convertible_common_base(From from, To to)
{
  using enum specs_convertible_result;
  if constexpr (is_same_v<From, To>) return yes;
  if constexpr (detail::is_child_of(From{}, To{}))
    return (detail::get_kind_tree_root(from) == detail::get_kind_tree_root(to)) ? yes : explicit_conversion_beyond_kind;
  if constexpr (detail::is_child_of(To{}, From{})) return explicit_conversion;
  if constexpr (detail::get_kind_tree_root(From{}) == detail::get_kind_tree_root(To{})) return cast;
  return no;
}

}  // namespace mp_units::detail
