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

#include <mp-units/framework/quantity_spec_concepts.h>

namespace mp_units::detail {

[[nodiscard]] consteval auto hierarchy_path_length(QuantitySpec auto q)
{
  if constexpr (requires { q._parent_; })
    return std::integral_constant<std::size_t, decltype(hierarchy_path_length(q._parent_))::value + 1>{};
  else
    return std::integral_constant<std::size_t, 1>{};
}

template<std::size_t Offset>
  requires(Offset >= 0)
[[nodiscard]] consteval QuantitySpec auto hierarchy_path_advance(QuantitySpec auto q)
{
  if constexpr (Offset == 0)
    return q;
  else if constexpr (requires { q._parent_; })
    return decltype(hierarchy_path_advance<Offset - 1>(q._parent_)){};
}

template<QuantitySpec A, QuantitySpec B>
[[nodiscard]] consteval auto have_common_base_in_hierarchy_of_equal_length(A a, B b)
{
  if constexpr (is_same_v<A, B>)
    return std::true_type{};
  else if constexpr (requires { a._parent_; })
    return decltype(have_common_base_in_hierarchy_of_equal_length(a._parent_, b._parent_)){};
  else
    return std::false_type{};
}

template<QuantitySpec A, QuantitySpec B>
[[nodiscard]] consteval auto have_common_base(A a, B b)
{
  constexpr std::size_t a_length = decltype(hierarchy_path_length(A{}))::value;
  constexpr std::size_t b_length = decltype(hierarchy_path_length(B{}))::value;
  if constexpr (a_length > b_length)
    return decltype(have_common_base_in_hierarchy_of_equal_length(
      decltype(hierarchy_path_advance<a_length - b_length>(a)){}, b)){};
  else
    return decltype(have_common_base_in_hierarchy_of_equal_length(
      a, decltype(hierarchy_path_advance<b_length - a_length>(b)){})){};
}

template<QuantitySpec A, QuantitySpec B>
  requires(decltype(have_common_base_in_hierarchy_of_equal_length(A{}, B{}))::value)
[[nodiscard]] consteval QuantitySpec auto get_common_base_for_hierarchy_of_equal_length(A a, B b)
{
  if constexpr (is_same_v<A, B>)
    return a;
  else
    return decltype(get_common_base_for_hierarchy_of_equal_length(a._parent_, b._parent_)){};
}

template<QuantitySpec A, QuantitySpec B>
  requires(decltype(have_common_base(A{}, B{}))::value)
[[nodiscard]] consteval QuantitySpec auto get_common_base(A a, B b)
{
  constexpr std::size_t a_length = decltype(hierarchy_path_length(A{}))::value;
  constexpr std::size_t b_length = decltype(hierarchy_path_length(B{}))::value;
  if constexpr (a_length > b_length)
    return decltype(get_common_base_for_hierarchy_of_equal_length(hierarchy_path_advance<a_length - b_length>(a), b)){};
  else
    return decltype(get_common_base_for_hierarchy_of_equal_length(a, hierarchy_path_advance<b_length - a_length>(b))){};
}

template<QuantitySpec Child, QuantitySpec Parent>
[[nodiscard]] consteval auto is_child_of(Child ch, Parent)
{
  if constexpr (Child{} == Parent{})
    return std::true_type{};
  else {
    constexpr std::size_t child_length = decltype(hierarchy_path_length(Child{}))::value;
    constexpr std::size_t parent_length = decltype(hierarchy_path_length(Parent{}))::value;
    if constexpr (parent_length > child_length)
      return std::false_type{};
    else
      return std::bool_constant<hierarchy_path_advance<child_length - parent_length>(ch) == Parent{}>{};
  }
}

}  // namespace mp_units::detail
