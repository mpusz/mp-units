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

#include <mp-units/bits/expression_template.h>
#include <mp-units/bits/unit_concepts.h>
#include <mp-units/quantity_spec.h>

namespace mp_units::detail {

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
  if constexpr (requires { U::quantity_spec; })
    return QuantityKindSpec<std::remove_const_t<decltype(U::quantity_spec)>>;
  else if constexpr (requires { U::reference_unit; })
    return all_are_kinds(U::reference_unit);
  else if constexpr (requires { typename U::_num_; }) {
    return all_are_kinds(typename U::_num_{}, typename U::_den_{});
  }
}

template<AssociatedUnit U>
[[nodiscard]] consteval auto get_associated_quantity_impl(U u);

template<AssociatedUnit U>
using to_quantity_spec = std::remove_const_t<decltype(get_associated_quantity_impl(U{}))>;

template<AssociatedUnit U>
[[nodiscard]] consteval auto get_associated_quantity_impl(U u)
{
  if constexpr (requires { U::quantity_spec; })
    return remove_kind(U::quantity_spec);
  else if constexpr (requires { U::reference_unit; })
    return get_associated_quantity_impl(U::reference_unit);
  else if constexpr (requires { typename U::_num_; }) {
    return expr_map<to_quantity_spec, derived_quantity_spec, struct dimensionless, type_list_of_quantity_spec_less>(u);
  }
}

template<AssociatedUnit U>
[[nodiscard]] consteval auto get_associated_quantity(U u)
{
  constexpr bool all_kinds = all_are_kinds(u);
  if constexpr (all_kinds)
    return kind_of<get_associated_quantity_impl(u)>;
  else
    return get_associated_quantity_impl(u);
}

}  // namespace mp_units::detail
