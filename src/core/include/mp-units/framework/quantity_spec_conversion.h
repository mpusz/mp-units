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
#include <mp-units/bits/module_macros.h>
#include <mp-units/bits/quantity_spec_conversion_impl.h>  // IWYU pragma: keep
#include <mp-units/framework/quantity_spec_concepts.h>
#include <mp-units/framework/quantity_spec_definitions.h>

namespace mp_units {

MP_UNITS_EXPORT_BEGIN

template<QuantitySpec From, QuantitySpec To>
[[nodiscard]] consteval bool implicitly_convertible(From from, To to)
{
  return detail::convertible(from, to) == detail::specs_convertible_result::yes;
}

template<QuantitySpec From, QuantitySpec To>
[[nodiscard]] consteval bool explicitly_convertible(From from, To to)
{
  return detail::convertible(from, to) >= detail::specs_convertible_result::explicit_conversion_beyond_kind;
}

template<QuantitySpec From, QuantitySpec To>
[[nodiscard]] consteval bool castable(From from, To to)
{
  return detail::convertible(from, to) >= detail::specs_convertible_result::cast;
}

template<QuantitySpec QS1, QuantitySpec QS2>
[[nodiscard]] consteval bool interconvertible(QS1 qs1, QS2 qs2)
{
  return mp_units::implicitly_convertible(qs1, qs2) && mp_units::implicitly_convertible(qs2, qs1);
}

template<QuantitySpec Q>
[[nodiscard]] consteval detail::QuantityKindSpec auto get_kind(Q)
{
  return kind_of<detail::get_kind_tree_root(Q{})>;
}

template<QuantitySpec Q>
[[nodiscard]] consteval bool is_non_negative(Q)
{
  return Q::_is_non_negative_;
}

[[nodiscard]] consteval QuantitySpec auto get_common_quantity_spec(QuantitySpec auto q) { return q; }

template<QuantitySpec Q1, QuantitySpec Q2>
  requires(detail::have_common_quantity_spec(Q1{}, Q2{}))
[[nodiscard]] consteval QuantitySpec auto get_common_quantity_spec(Q1, Q2)
{
  return detail::get_common_quantity_spec_result<Q1, Q2>;
}

[[nodiscard]] consteval QuantitySpec auto get_common_quantity_spec(QuantitySpec auto q1, QuantitySpec auto q2,
                                                                   QuantitySpec auto q3, QuantitySpec auto... rest)
  requires requires { mp_units::get_common_quantity_spec(mp_units::get_common_quantity_spec(q1, q2), q3, rest...); }
{
  return mp_units::get_common_quantity_spec(mp_units::get_common_quantity_spec(q1, q2), q3, rest...);
}

MP_UNITS_EXPORT_END

}  // namespace mp_units
