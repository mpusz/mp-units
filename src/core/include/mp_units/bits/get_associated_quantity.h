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

#include <mp_units/bits/expression_template.h>
#include <mp_units/bits/unit_concepts.h>
#include <mp_units/quantity_spec.h>

namespace mp_units::detail {

template<typename U, auto... Vs>
[[nodiscard]] consteval auto get_associated_quantity(power<U, Vs...>)
{
  return pow<Vs...>(get_associated_quantity(U{}));
}

template<typename... Us>
[[nodiscard]] consteval auto get_associated_quantity(type_list<Us...>)
{
  return (dimensionless * ... * get_associated_quantity(Us{}));
}

template<AssociatedUnit U>
[[nodiscard]] consteval auto get_associated_quantity(U)
{
  if constexpr (requires { U::reference_unit; })
    return get_associated_quantity(U::reference_unit);
  else if constexpr (requires { typename U::_num_; })
    return get_associated_quantity(typename U::_num_{}) / get_associated_quantity(typename U::_den_{});
  else if constexpr (requires { U::quantity_spec; })
    return U::quantity_spec;
}

}  // namespace mp_units::detail
