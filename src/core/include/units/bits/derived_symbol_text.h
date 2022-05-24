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
#include <units/symbol_text.h>

namespace units::detail {

template<bool Divide, std::size_t NegativeExpCount, std::size_t Idx>
constexpr auto operator_text()
{
  if constexpr (Idx == 0) {
    if constexpr (Divide && NegativeExpCount == 1) {
      return basic_fixed_string("1/");
    } else {
      return basic_fixed_string("");
    }
  } else {
    if constexpr (Divide && NegativeExpCount == 1) {
      return basic_fixed_string("/");
    } else {
      return basic_symbol_text("⋅", " ");
    }
  }
}

template<typename E, basic_symbol_text Symbol, std::size_t NegativeExpCount, std::size_t Idx>
constexpr auto exp_text()
{
  // get calculation operator + symbol
  const auto txt = operator_text<(E::num < 0), NegativeExpCount, Idx>() + Symbol;
  if constexpr (E::den != 1) {
    // add root part
    return txt + basic_fixed_string("^(") + regular<abs(E::num)>() + basic_fixed_string("/") + regular<E::den>() +
           basic_fixed_string(")");
  } else if constexpr (E::num != 1) {
    // add exponent part
    if constexpr (NegativeExpCount > 1) {  // no '/' sign here (only negative exponents)
      return txt + superscript<E::num>();
    } else if constexpr (E::num != -1) {  // -1 is replaced with '/' sign here
      return txt + superscript<abs(E::num)>();
    } else {
      return txt;
    }
  } else {
    return txt;
  }
}

template<typename... Es>
inline constexpr int negative_exp_count = ((Es::num < 0 ? 1 : 0) + ... + 0);

template<typename... Us, typename... Es, std::size_t... Idxs>
constexpr auto derived_symbol_text(exponent_list<Es...>, std::index_sequence<Idxs...>)
{
  constexpr auto neg_exp = negative_exp_count<Es...>;
  return (exp_text<Es, Us::symbol, neg_exp, Idxs>() + ...);
}

template<DerivedDimension Dim, Unit... Us>
constexpr auto derived_symbol_text()
{
  return derived_symbol_text<Us...>(typename Dim::recipe(), std::index_sequence_for<Us...>());
}

}  // namespace units::detail
