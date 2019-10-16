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

#include <units/quantity.h>

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

    template<typename Ratio>
    inline constexpr std::string_view ratio_txt = "";

    template<> inline constexpr std::string_view ratio_txt<ratio<1, std::atto::den>> = "a";
    template<> inline constexpr std::string_view ratio_txt<ratio<1, std::femto::den>> = "f";
    template<> inline constexpr std::string_view ratio_txt<ratio<1, std::pico::den>> = "p";
    template<> inline constexpr std::string_view ratio_txt<ratio<1, std::nano::den>> = "n";
    template<> inline constexpr std::string_view ratio_txt<ratio<1, std::micro::den>> = "\u00b5\u0073";
    template<> inline constexpr std::string_view ratio_txt<ratio<1, std::milli::den>> = "m";
    template<> inline constexpr std::string_view ratio_txt<ratio<1, std::centi::den>> = "c";
    template<> inline constexpr std::string_view ratio_txt<ratio<1, std::deci::den>> = "d";
    template<> inline constexpr std::string_view ratio_txt<ratio<std::deca::num>> = "da";
    template<> inline constexpr std::string_view ratio_txt<ratio<std::hecto::num>> = "h";
    template<> inline constexpr std::string_view ratio_txt<ratio<std::kilo::num>> = "k";
    template<> inline constexpr std::string_view ratio_txt<ratio<std::mega::num>> = "M";
    template<> inline constexpr std::string_view ratio_txt<ratio<std::giga::num>> = "G";
    template<> inline constexpr std::string_view ratio_txt<ratio<std::tera::num>> = "T";
    template<> inline constexpr std::string_view ratio_txt<ratio<std::peta::num>> = "P";
    template<> inline constexpr std::string_view ratio_txt<ratio<std::exa::num>> = "E";

    template<typename Ratio, typename CharT, typename Traits>
    void print_prefix_or_ratio(std::basic_ostream<CharT, Traits>& os)
    {
      if constexpr(Ratio::num != 1 || Ratio::den != 1) {
        constexpr auto prefix = ratio_txt<Ratio>;

        if constexpr(prefix != "") {
          // print as a prefixed unit
          os << prefix;
        }
        else {
          // print as a ratio of the coherent unit
          print_ratio<Ratio>(os);
        }
      }
    }

    template<typename CharT, typename Traits, typename... Es>
    void print_dimensions(std::basic_ostream<CharT, Traits>& os, dimension<Es...>)
    {
      bool first = true;
      auto ingr_printer = [&]<typename E>(E) {
        if constexpr(E::num < 0) {
          os << (first ? "1/" : "/");
        }
        else {
          os << (first ? "" : "*");
        }
        os << E::dimension::symbol;
        if constexpr(E::den != 1) {
          os << "^(" << abs(E::num) << "/" << E::den << ")";
        }
        else if constexpr(abs(E::num) != 1) {
          os << "^" << abs(E::num);
        }
        first = false;
      };
      (ingr_printer(Es{}), ...);
    }

  }

}  // namespace units
