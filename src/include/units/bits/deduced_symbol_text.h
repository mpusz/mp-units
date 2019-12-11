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

#include <units/bits/external/fixed_string.h>
#include <units/bits/external/text_tools.h>
#include <units/derived_dimension.h>

namespace units::detail {

template<bool Divide, std::size_t Idx>
constexpr auto operator_text()
{
  if constexpr(Idx == 0) {
    if constexpr(Divide) {
      return basic_fixed_string("1/");
    }
    else {
      return basic_fixed_string("");
    }
  }
  else {
    if constexpr(Divide) {
      return basic_fixed_string("/");
    }
    else {
      return basic_fixed_string("⋅");
    }
  }
}

template<typename E, basic_fixed_string Symbol, std::size_t Idx>
constexpr auto exp_text()
{
  // get calculation operator + symbol
  const auto txt = operator_text<E::num < 0, Idx>() + Symbol;
  if constexpr(E::den != 1) {
    // add root part
    return txt + basic_fixed_string("^(") + regular<abs(E::num)>() + basic_fixed_string("/") + regular<E::den>() + basic_fixed_string(")");
  }
  else if constexpr(abs(E::num) != 1) {
    // add exponent part
    return txt + superscript<abs(E::num)>();
  }
  else {
    return txt;
  }
}

template<typename... Us, typename... Es, std::size_t... Idxs>
constexpr auto deduced_symbol_text(derived_dimension<Es...>, std::index_sequence<Idxs...>)
{
  return (exp_text<Es, Us::symbol, Idxs>() + ...);
}

template<DerivedDimension Dim, Unit... Us>
constexpr auto deduced_symbol_text()
{
  return deduced_symbol_text<Us...>(Dim(), std::index_sequence_for<Us...>());
}

}  // namespace units::detail
