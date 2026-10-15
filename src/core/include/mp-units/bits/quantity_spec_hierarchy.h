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

#include <mp-units/bits/type_list.h>
#include <mp-units/compat_macros.h>
#include <mp-units/ext/algorithm.h>
#include <mp-units/framework/quantity_spec_concepts.h>
#include <mp-units/framework/symbolic_expression.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <type_traits>
#endif
#endif

// The facts about a named quantity specification that follow from its parent chain and its
// equation alone: root, ancestry, whether it defines its equation, and depth. Everything that
// needs the `is_kind` property (kind roots) lives in `quantity_spec_hierarchy_conversion.h`,
// because this header is included before `is_kind` is declared.
//
// The root and the depth are class templates, so the compiler computes each once per quantity
// specification. The ancestry and equation checks are consteval functions.
namespace mp_units::detail {

template<typename Q>
concept HasParent = requires { Q::_parent_; };

// Either its own equation or the one propagated from the parent (see `propagate_equation`). An
// open factor has the equation of the quantity it wraps, with every factor open in turn.
template<typename Q>
  struct has_equation : std::bool_constant < requires {
  Q::_equation_;
}>{};

template<typename N>
struct has_equation<open_factor<N>> : has_equation<N> {};

template<typename Q>
concept HasEquation = has_equation<Q>::value;

template<HasParent Q>
using parent_of = MP_UNITS_NONCONST_TYPE(Q::_parent_);

template<typename E>
struct open_element : std::type_identity<open_factor<E>> {};

template<typename T, int... Ints>
struct open_element<power<T, Ints...>> : std::type_identity<power<open_factor<T>, Ints...>> {};

template<typename List>
struct open_elements;

template<typename... Ts>
struct open_elements<type_list<Ts...>> : std::type_identity<type_list<typename open_element<Ts>::type...>> {};

template<typename Q>
struct equation_of_impl : std::type_identity<MP_UNITS_NONCONST_TYPE(Q::_equation_)> {};

template<typename N>
struct open_equation {
  using _num_ = open_elements<typename equation_of_impl<N>::type::_num_>::type;
  using _den_ = open_elements<typename equation_of_impl<N>::type::_den_>::type;
};

template<typename N>
struct equation_of_impl<open_factor<N>> : std::type_identity<open_equation<N>> {};

template<HasEquation Q>
using equation_of = equation_of_impl<Q>::type;

// root

template<typename Q>
struct hierarchy_root_impl : std::type_identity<Q> {};

template<HasParent Q>
struct hierarchy_root_impl<Q> : hierarchy_root_impl<parent_of<Q>> {};

template<typename N>
struct hierarchy_root_impl<open_factor<N>> : hierarchy_root_impl<N> {};

template<typename Q>
using hierarchy_root = hierarchy_root_impl<Q>::type;

template<QuantitySpec Q>
[[nodiscard]] consteval QuantitySpec auto get_hierarchy_root(Q)
{
  if constexpr (QuantityKindSpec<Q>)
    return hierarchy_root<MP_UNITS_NONCONST_TYPE(Q::_quantity_spec_)>{};
  else
    return hierarchy_root<Q>{};
}

// ancestry

template<typename Ancestor, typename Q>
[[nodiscard]] consteval bool is_ancestor_or_self()
{
  if constexpr (std::is_same_v<Ancestor, Q>)
    return true;
  else if constexpr (HasParent<Q>)
    return detail::is_ancestor_or_self<Ancestor, parent_of<Q>>();
  else
    return false;
}

// equation

// A quantity defines its equation when the equation is its own and differs from the parent's. A
// child that restates the parent's equation inherits it like a child without one.
// `propagate_equation` copies the parent's `_equation_` into every child that has none, so the
// equation reaches every level of a branch and comparing with the direct parent covers them all.
template<typename Q>
[[nodiscard]] consteval bool defines_equation()
{
  if constexpr (is_open_factor<Q>)
    return detail::defines_equation<unwrap_open_factor<Q>>();
  else if constexpr (!HasEquation<Q>)
    return false;
  else if constexpr (!HasParent<Q>)
    return true;
  else if constexpr (!HasEquation<parent_of<Q>>)
    return true;
  else
    return !std::is_same_v<equation_of<Q>, equation_of<parent_of<Q>>>;
}

// depth

template<typename Q>
struct quantity_spec_depth;

template<typename... Ts>
[[nodiscard]] consteval int max_factor_depth(type_list<Ts...>)
{
  int result = 0;
  ((result = detail::max(result, quantity_spec_depth<expr_type<Ts>>::value)), ...);
  return result;
}

// One more than the deepest factor of either list of the equation.
template<typename Eq>
[[nodiscard]] consteval int equation_depth()
{
  return 1 +
         detail::max(detail::max_factor_depth(typename Eq::_num_{}), detail::max_factor_depth(typename Eq::_den_{}));
}

// The length of the longest equation chain below a quantity: a quantity without an equation has
// depth 0.
template<typename Q>
struct quantity_spec_depth : std::integral_constant<int, 0> {};

template<HasEquation Q>
struct quantity_spec_depth<Q> : std::integral_constant<int, detail::equation_depth<equation_of<Q>>()> {};

template<typename N>
struct quantity_spec_depth<open_factor<N>> : quantity_spec_depth<N> {};

// nearest common ancestor

// Both named and in one tree.
template<typename Q1, typename Q2>
[[nodiscard]] consteval QuantitySpec auto nearest_common_ancestor()
{
  if constexpr (detail::is_ancestor_or_self<Q1, Q2>())
    return Q1{};
  else
    return detail::nearest_common_ancestor<parent_of<Q1>, Q2>();
}

}  // namespace mp_units::detail
