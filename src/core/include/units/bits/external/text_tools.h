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
#include <units/symbol_text.h>

namespace units::detail {

template<std::intmax_t Value>
  requires(0 <= Value) && (Value < 10)
inline constexpr basic_fixed_string superscript_number = "";

template<>
inline constexpr basic_fixed_string superscript_number<0> = "\u2070";
template<>
inline constexpr basic_fixed_string superscript_number<1> = "\u00b9";
template<>
inline constexpr basic_fixed_string superscript_number<2> = "\u00b2";
template<>
inline constexpr basic_fixed_string superscript_number<3> = "\u00b3";
template<>
inline constexpr basic_fixed_string superscript_number<4> = "\u2074";
template<>
inline constexpr basic_fixed_string superscript_number<5> = "\u2075";
template<>
inline constexpr basic_fixed_string superscript_number<6> = "\u2076";
template<>
inline constexpr basic_fixed_string superscript_number<7> = "\u2077";
template<>
inline constexpr basic_fixed_string superscript_number<8> = "\u2078";
template<>
inline constexpr basic_fixed_string superscript_number<9> = "\u2079";

inline constexpr basic_symbol_text superscript_minus("\u207b", "-");

inline constexpr basic_symbol_text superscript_prefix("", "^");

template<std::intmax_t Value>
constexpr auto superscript_helper()
{
  if constexpr (Value < 0)
    return superscript_minus + superscript_helper<-Value>();
  else if constexpr (Value < 10)
    return basic_symbol_text(superscript_number<Value>, basic_fixed_string(static_cast<char>('0' + Value)));
  else
    return superscript_helper<Value / 10>() + superscript_helper<Value % 10>();
}

template<std::intmax_t Value>
constexpr auto superscript()
{
  return superscript_prefix + superscript_helper<Value>();
}

template<std::intmax_t Value>
constexpr auto regular()
{
  if constexpr (Value < 0)
    return basic_fixed_string("-") + superscript_helper<-Value>();
  else if constexpr (Value < 10)
    return basic_symbol_text(static_cast<char>('0' + Value));
  else
    return regular<Value / 10>() + regular<Value % 10>();
}

}  // namespace units::detail
