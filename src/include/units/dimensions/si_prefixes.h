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

#include <units/unit.h>
#include <units/format.h>

namespace units {

  // prefix tags
  struct si_prefix;

  // SI prefixes

  using atto = prefix<si_prefix, ratio<1, std::atto::den>, "a">;
  using femto = prefix<si_prefix, ratio<1, std::femto::den>, "f">;
  using pico = prefix<si_prefix, ratio<1, std::pico::den>, "p">;
  using nano = prefix<si_prefix, ratio<1, std::nano::den>, "n">;
  using micro = prefix<si_prefix, ratio<1, std::micro::den>, "µ">;
  using milli = prefix<si_prefix, ratio<1, std::milli::den>, "m">;
  using centi = prefix<si_prefix, ratio<1, std::centi::den>, "c">;
  using deci = prefix<si_prefix, ratio<1, std::deci::den>, "d">;
  using deca = prefix<si_prefix, ratio<std::deca::num>, "da">;
  using hecto = prefix<si_prefix, ratio<std::hecto::num>, "h">;
  using kilo = prefix<si_prefix, ratio<std::kilo::num>, "k">;
  using mega = prefix<si_prefix, ratio<std::mega::num>, "M">;
  using giga = prefix<si_prefix, ratio<std::giga::num>, "G">;
  using tera = prefix<si_prefix, ratio<std::tera::num>, "T">;
  using peta = prefix<si_prefix, ratio<std::peta::num>, "P">;
  using exa = prefix<si_prefix, ratio<std::exa::num>, "E">;

  template<> inline constexpr std::string_view prefix_symbol<si_prefix, ratio<1, std::atto::den>> = "a";
  template<> inline constexpr std::string_view prefix_symbol<si_prefix, ratio<1, std::femto::den>> = "f";
  template<> inline constexpr std::string_view prefix_symbol<si_prefix, ratio<1, std::pico::den>> = "p";
  template<> inline constexpr std::string_view prefix_symbol<si_prefix, ratio<1, std::nano::den>> = "n";
  template<> inline constexpr std::string_view prefix_symbol<si_prefix, ratio<1, std::micro::den>> = "µ";
  template<> inline constexpr std::string_view prefix_symbol<si_prefix, ratio<1, std::milli::den>> = "m";
  template<> inline constexpr std::string_view prefix_symbol<si_prefix, ratio<1, std::centi::den>> = "c";
  template<> inline constexpr std::string_view prefix_symbol<si_prefix, ratio<1, std::deci::den>> = "d";
  template<> inline constexpr std::string_view prefix_symbol<si_prefix, ratio<std::deca::num>> = "da";
  template<> inline constexpr std::string_view prefix_symbol<si_prefix, ratio<std::hecto::num>> = "h";
  template<> inline constexpr std::string_view prefix_symbol<si_prefix, ratio<std::kilo::num>> = "k";
  template<> inline constexpr std::string_view prefix_symbol<si_prefix, ratio<std::mega::num>> = "M";
  template<> inline constexpr std::string_view prefix_symbol<si_prefix, ratio<std::giga::num>> = "G";
  template<> inline constexpr std::string_view prefix_symbol<si_prefix, ratio<std::tera::num>> = "T";
  template<> inline constexpr std::string_view prefix_symbol<si_prefix, ratio<std::peta::num>> = "P";
  template<> inline constexpr std::string_view prefix_symbol<si_prefix, ratio<std::exa::num>> = "E";

}
