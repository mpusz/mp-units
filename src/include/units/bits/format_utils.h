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

#include <units/dimension.h>
#include <units/prefix.h>
#include <ostream>
#include <string_view>

namespace units {

  namespace detail {

    template<typename Ratio, typename CharT, typename Traits>
    void print_ratio(std::basic_ostream<CharT, Traits>& os)
    {
      if constexpr(Ratio::num != 1 || Ratio::den != 1) {
        if constexpr(Ratio::den == 1) {
          os << "[" << Ratio::num << "]";
        }
        else {
          os << "[" << Ratio::num << "/" << Ratio::den << "]";
        }
      }
    }

    template<typename Ratio, typename PrefixType, typename CharT, typename Traits>
    void print_prefix_or_ratio(std::basic_ostream<CharT, Traits>& os)
    {
      if constexpr(Ratio::num != 1 || Ratio::den != 1) {
        if(!std::same_as<PrefixType, no_prefix>) {
          using prefix = downcast<detail::prefix_base<PrefixType, Ratio>>;

          if constexpr(!std::same_as<prefix, prefix_base<PrefixType, Ratio>>) {
            // print as a prefixed unit
            os << prefix::symbol;
            return;
          }
        }
        // print as a ratio of the coherent unit
        print_ratio<Ratio>(os);
      }
    }


    template<int Value>
      requires (0 <= Value) && (Value < 10)
    inline constexpr basic_fixed_string superscript_number = "\u2070";

//    template<> inline constexpr basic_fixed_string superscript_number<0> = "\u2070";
    template<> inline constexpr basic_fixed_string superscript_number<1> = "\u00b9";
    template<> inline constexpr basic_fixed_string superscript_number<2> = "\u00b2";
    template<> inline constexpr basic_fixed_string superscript_number<3> = "\u00b3";
    template<> inline constexpr basic_fixed_string superscript_number<4> = "\u2074";
    template<> inline constexpr basic_fixed_string superscript_number<5> = "\u2075";
    template<> inline constexpr basic_fixed_string superscript_number<6> = "\u2076";
    template<> inline constexpr basic_fixed_string superscript_number<7> = "\u2077";
    template<> inline constexpr basic_fixed_string superscript_number<8> = "\u2078";
    template<> inline constexpr basic_fixed_string superscript_number<9> = "\u2079";

    template<int Value>
      requires (Value >= 0)
    constexpr auto superscript()
    {
      if constexpr(Value < 10)
        return superscript_number<Value>;
      else
        return superscript<Value / 10>() + superscript<Value % 10>();
    }

    template<int Value>
      requires (Value >= 0)
    constexpr auto regular()
    {
      if constexpr(Value < 10)
        return basic_fixed_string(static_cast<char>('0' + Value));
      else
        return regular<Value / 10>() + regular<Value % 10>();
    }


    template<bool Divide, std::size_t Idx>
    constexpr auto operator_txt()
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

    template<typename E, std::size_t Idx>
    constexpr auto exp_txt()
    {
      // get calculation operator + symbol
      const auto txt = operator_txt<E::num < 0, Idx>() + E::dimension::symbol;
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

    template<typename... Es, std::size_t... Idxs>
    constexpr auto symbol_text_impl(dimension<Es...>, std::index_sequence<Idxs...>)
    {
      return (exp_txt<Es, Idxs>() + ...);
    }

    template<typename... Es>
    constexpr auto symbol_text(dimension<Es...> d)
    {
      return symbol_text_impl<>(d, std::index_sequence_for<Es...>());
    }

  }

}  // namespace units
