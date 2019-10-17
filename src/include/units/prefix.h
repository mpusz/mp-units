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

  template<Unit U> using atto = unit<typename U::dimension, ratio_multiply<typename U::ratio, ratio<1, std::atto::den>>>;
  template<Unit U> using femto = unit<typename U::dimension, ratio_multiply<typename U::ratio, ratio<1, std::femto::den>>>;
  template<Unit U> using pico = unit<typename U::dimension, ratio_multiply<typename U::ratio, ratio<1, std::pico::den>>>;
  template<Unit U> using nano = unit<typename U::dimension, ratio_multiply<typename U::ratio, ratio<1, std::nano::den>>>;
  template<Unit U> using micro = unit<typename U::dimension, ratio_multiply<typename U::ratio, ratio<1, std::micro::den>>>;
  template<Unit U> using milli = unit<typename U::dimension, ratio_multiply<typename U::ratio, ratio<1, std::milli::den>>>;
  template<Unit U> using centi = unit<typename U::dimension, ratio_multiply<typename U::ratio, ratio<1, std::centi::den>>>;
  template<Unit U> using deca = unit<typename U::dimension, ratio_multiply<typename U::ratio, ratio<std::deca::num>>>;
  template<Unit U> using hecto = unit<typename U::dimension, ratio_multiply<typename U::ratio, ratio<std::hecto::num>>>;
  template<Unit U> using kilo = unit<typename U::dimension, ratio_multiply<typename U::ratio, ratio<std::kilo::num>>>;
  template<Unit U> using mega = unit<typename U::dimension, ratio_multiply<typename U::ratio, ratio<std::mega::num>>>;
  template<Unit U> using giga = unit<typename U::dimension, ratio_multiply<typename U::ratio, ratio<std::giga::num>>>;
  template<Unit U> using tera = unit<typename U::dimension, ratio_multiply<typename U::ratio, ratio<std::tera::num>>>;
  template<Unit U> using peta = unit<typename U::dimension, ratio_multiply<typename U::ratio, ratio<std::peta::num>>>;
  template<Unit U> using exa = unit<typename U::dimension, ratio_multiply<typename U::ratio, ratio<std::exa::num>>>;

  template<> inline constexpr std::string_view prefix_symbol<si_prefix, ratio<1, std::atto::den>> = "a";
  template<> inline constexpr std::string_view prefix_symbol<si_prefix, ratio<1, std::femto::den>> = "f";
  template<> inline constexpr std::string_view prefix_symbol<si_prefix, ratio<1, std::pico::den>> = "p";
  template<> inline constexpr std::string_view prefix_symbol<si_prefix, ratio<1, std::nano::den>> = "n";
  template<> inline constexpr std::string_view prefix_symbol<si_prefix, ratio<1, std::micro::den>> = "\u00b5\u0073";  // µ
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
