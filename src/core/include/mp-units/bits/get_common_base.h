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

#include <mp-units/bits/quantity_spec_concepts.h>

namespace mp_units::detail {

[[nodiscard]] consteval std::size_t hierarchy_path_length(QuantitySpec auto q)
{
  if constexpr (requires { q._parent_; })
    return hierarchy_path_length(q._parent_) + 1;
  else
    return 1;
}

template<std::size_t Offset>
  requires(Offset >= 0)
[[nodiscard]] consteval QuantitySpec auto hierarchy_path_advance(QuantitySpec auto q)
{
  if constexpr (Offset == 0)
    return q;
  else if constexpr (requires { q._parent_; })
    return hierarchy_path_advance<Offset - 1>(q._parent_);
}

template<QuantitySpec A, QuantitySpec B>
[[nodiscard]] consteval bool have_common_base_in_hierarchy_of_equal_length(A a, B b)
{
  if constexpr (is_same_v<A, B>)
    return true;
  else if constexpr (requires { a._parent_; })
    return have_common_base_in_hierarchy_of_equal_length(a._parent_, b._parent_);
  else
    return false;
}

template<QuantitySpec A, QuantitySpec B>
[[nodiscard]] consteval auto have_common_base(A a, B b)
{
  constexpr std::size_t a_length = hierarchy_path_length(A{});
  constexpr std::size_t b_length = hierarchy_path_length(B{});
  if constexpr (a_length > b_length)
    return have_common_base_in_hierarchy_of_equal_length(hierarchy_path_advance<a_length - b_length>(a), b);
  else
    return have_common_base_in_hierarchy_of_equal_length(a, hierarchy_path_advance<b_length - a_length>(b));
}

template<QuantitySpec A, QuantitySpec B>
  requires(have_common_base_in_hierarchy_of_equal_length(A{}, B{}))
[[nodiscard]] consteval auto get_common_base_for_hierarchy_of_equal_length(A a, B b)
{
  if constexpr (is_same_v<A, B>)
    return a;
  else
    return get_common_base_for_hierarchy_of_equal_length(a._parent_, b._parent_);
}

template<QuantitySpec A, QuantitySpec B>
  requires(have_common_base(A{}, B{}))
[[nodiscard]] consteval auto get_common_base(A a, B b)
{
  constexpr int a_length = hierarchy_path_length(A{});
  constexpr int b_length = hierarchy_path_length(B{});
  if constexpr (a_length > b_length)
    return get_common_base_for_hierarchy_of_equal_length(hierarchy_path_advance<a_length - b_length>(a), b);
  else
    return get_common_base_for_hierarchy_of_equal_length(a, hierarchy_path_advance<b_length - a_length>(b));
}

}  // namespace mp_units::detail
