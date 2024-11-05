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

#include <mp-units/framework/expression_template.h>
#include <mp-units/framework/quantity_spec.h>
#include <mp-units/framework/unit_concepts.h>

namespace mp_units {

template<Unit U1, Unit U2, Unit... Rest>
struct common_unit;

namespace detail {

template<AssociatedUnit U>
[[nodiscard]] consteval auto all_are_kinds(U);

template<typename U, auto... Vs>
[[nodiscard]] consteval auto all_are_kinds(power<U, Vs...>)
{
  return all_are_kinds(U{});
}

template<typename... Nums, typename... Dens>
[[nodiscard]] consteval bool all_are_kinds(type_list<Nums...>, type_list<Dens...>)
{
  return (... && all_are_kinds(Nums{})) && (... && all_are_kinds(Dens{}));
}

template<AssociatedUnit U>
[[nodiscard]] consteval auto all_are_kinds(U)
{
  if constexpr (requires { U::_quantity_spec_; })
    return QuantityKindSpec<std::remove_const_t<decltype(U::_quantity_spec_)>>;
  else if constexpr (requires { U::_reference_unit_; })
    return all_are_kinds(U::_reference_unit_);
  else if constexpr (requires { typename U::_num_; }) {
    return all_are_kinds(typename U::_num_{}, typename U::_den_{});
  }
}

template<AssociatedUnit U>
[[nodiscard]] consteval auto get_associated_quantity_impl(U u);

template<AssociatedUnit U>
using to_quantity_spec = decltype(get_associated_quantity_impl(U{}));

template<typename... Us>
[[nodiscard]] consteval auto get_associated_quantity_impl(common_unit<Us...>)
{
  return get_common_quantity_spec(get_associated_quantity_impl(Us{})...);
}

template<AssociatedUnit U>
[[nodiscard]] consteval auto get_associated_quantity_impl(U u)
{
  if constexpr (requires { U::_quantity_spec_; })
    return remove_kind(U::_quantity_spec_);
  else if constexpr (requires { U::_reference_unit_; })
    return get_associated_quantity_impl(U::_reference_unit_);
  else if constexpr (requires { typename U::_num_; }) {
    return expr_map<to_quantity_spec, derived_quantity_spec, struct dimensionless, type_list_of_quantity_spec_less>(u);
  }
}

template<AssociatedUnit U>
[[nodiscard]] consteval auto get_associated_quantity(U u)
{
  constexpr bool all_kinds = all_are_kinds(U{});
  if constexpr (all_kinds)
    return kind_of<get_associated_quantity_impl(U{})>;
  else
    return get_associated_quantity_impl(u);
}

}  // namespace detail

}  // namespace mp_units
