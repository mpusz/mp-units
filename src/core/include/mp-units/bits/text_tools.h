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

#include <mp-units/bits/ratio.h>
#include <mp-units/compat_macros.h>
#include <mp-units/ext/fixed_string.h>
#include <mp-units/ext/type_traits.h>
#include <mp-units/framework/symbol_text.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cstdint>
#endif
#endif

namespace mp_units::detail {

template<std::intmax_t Value>
  requires(0 <= Value) && (Value < 10)
constexpr basic_fixed_string superscript_number = u8"";

template<>
MP_UNITS_INLINE constexpr basic_fixed_string superscript_number<0> = u8"\u2070";
template<>
MP_UNITS_INLINE constexpr basic_fixed_string superscript_number<1> = u8"\u00b9";
template<>
MP_UNITS_INLINE constexpr basic_fixed_string superscript_number<2> = u8"\u00b2";
template<>
MP_UNITS_INLINE constexpr basic_fixed_string superscript_number<3> = u8"\u00b3";
template<>
MP_UNITS_INLINE constexpr basic_fixed_string superscript_number<4> = u8"\u2074";
template<>
MP_UNITS_INLINE constexpr basic_fixed_string superscript_number<5> = u8"\u2075";
template<>
MP_UNITS_INLINE constexpr basic_fixed_string superscript_number<6> = u8"\u2076";
template<>
MP_UNITS_INLINE constexpr basic_fixed_string superscript_number<7> = u8"\u2077";
template<>
MP_UNITS_INLINE constexpr basic_fixed_string superscript_number<8> = u8"\u2078";
template<>
MP_UNITS_INLINE constexpr basic_fixed_string superscript_number<9> = u8"\u2079";

inline constexpr symbol_text superscript_minus(u8"\u207b", "-");

inline constexpr symbol_text superscript_prefix(u8"", "^");

template<std::intmax_t Value>
[[nodiscard]] consteval auto superscript_helper()
{
  if constexpr (Value < 0)
    return superscript_minus + superscript_helper<-Value>();
  else if constexpr (Value < 10)
    return symbol_text(superscript_number<Value>, basic_fixed_string(static_cast<char>('0' + Value)));
  else
    return superscript_helper<Value / 10>() + superscript_helper<Value % 10>();
}

template<std::intmax_t Value>
[[nodiscard]] consteval auto superscript()
{
  return superscript_prefix + superscript_helper<Value>();
}

template<std::intmax_t Value>
[[nodiscard]] consteval auto regular()
{
  if constexpr (Value < 0)
    return symbol_text("-") + superscript_helper<-Value>();
  else if constexpr (Value < 10)
    return symbol_text(static_cast<char>('0' + Value));
  else
    return regular<Value / 10>() + regular<Value % 10>();
}

template<typename CharT, std::size_t N, std::size_t M, std::output_iterator<CharT> Out>
constexpr Out copy(const symbol_text<N, M>& txt, text_encoding encoding, Out out)
{
  if (encoding == text_encoding::unicode) {
    if constexpr (is_same_v<CharT, char8_t>)
      return ::mp_units::detail::copy(txt.unicode().begin(), txt.unicode().end(), out);
    else if constexpr (is_same_v<CharT, char>) {
      for (const char8_t ch : txt.unicode()) *out++ = static_cast<char>(ch);
      return out;
    } else
      MP_UNITS_THROW(std::invalid_argument("Unicode text can't be copied to CharT output"));
  } else {
    if constexpr (is_same_v<CharT, char>)
      return ::mp_units::detail::copy(txt.ascii().begin(), txt.ascii().end(), out);
    else
      MP_UNITS_THROW(std::invalid_argument("ASCII text can't be copied to CharT output"));
  }
}

template<typename CharT, std::size_t N, std::size_t M, std::output_iterator<CharT> Out>
constexpr Out copy_symbol(const symbol_text<N, M>& txt, text_encoding encoding, bool negative_power, Out out)
{
  out = copy<CharT>(txt, encoding, out);
  if (negative_power) {
    constexpr auto exp = superscript<-1>();
    out = copy<CharT>(exp, encoding, out);
  }
  return out;
}

template<typename CharT, int Num, int... Den, std::output_iterator<CharT> Out>
constexpr Out copy_symbol_exponent(text_encoding encoding, bool negative_power, Out out)
{
  constexpr ratio r{Num, Den...};
  if constexpr (r.den != 1) {
    // add root part
    if (negative_power) {
      constexpr auto txt =
        symbol_text("^-(") + regular<r.num>() + symbol_text("/") + regular<r.den>() + symbol_text(")");
      return copy<CharT>(txt, encoding, out);
    }
    constexpr auto txt = symbol_text("^(") + regular<r.num>() + symbol_text("/") + regular<r.den>() + symbol_text(")");
    return copy<CharT>(txt, encoding, out);
  } else if constexpr (r.num != 1) {
    // add exponent part
    if (negative_power) {
      constexpr auto txt = superscript<-r.num>();
      return copy<CharT>(txt, encoding, out);
    }
    constexpr auto txt = superscript<r.num>();
    return copy<CharT>(txt, encoding, out);
  }
}

}  // namespace mp_units::detail
