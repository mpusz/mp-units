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

#include <units/bits/deduced_symbol_text.h>
#include <units/bits/external/text_tools.h>
#include <units/prefix.h>
#include <units/derived_dimension.h>
#include <sstream>

namespace units::detail {

inline constexpr basic_symbol_text base_multiplier("\u00D7 10", "x 10");

template<ratio R>
constexpr auto ratio_text()
{
  if constexpr(R.num == 1 && R.den == 1 && R.exp != 0) {
    return base_multiplier + superscript<R.exp>() + basic_fixed_string(" ");
  }
  else if constexpr(R.num != 1 || R.den != 1 || R.exp != 0) {
    auto txt = basic_fixed_string("[") + regular<R.num>();
    if constexpr(R.den == 1) {
      if constexpr(R.exp == 0) {
        return txt + basic_fixed_string("] ");
      }
      else {
        return txt + " " + base_multiplier + superscript<R.exp>() +
            basic_fixed_string("] ");
      }
    }
    else {
      if constexpr(R.exp == 0) {
        return txt + basic_fixed_string("/") + regular<R.den>() +
            basic_fixed_string("] ");
      }
      else {
        return txt + basic_fixed_string("/") + regular<R.den>() +
            " " + base_multiplier + superscript<R.exp>() +
            basic_fixed_string("] ");
      }
    }
  }
  else {
    return basic_fixed_string("");
  }
}

template<ratio R, typename PrefixFamily>
constexpr auto prefix_or_ratio_text()
{
  if constexpr(R.num == 1 && R.den == 1 && R.exp == 0) {
    // no ratio/prefix
    return basic_fixed_string("");
  }
  else {
    if constexpr (!std::is_same_v<PrefixFamily, no_prefix>) {
      // try to form a prefix
      using prefix = downcast<detail::prefix_base<PrefixFamily, R>>;

      if constexpr(!std::is_same_v<prefix, prefix_base<PrefixFamily, R>>) {
        // print as a prefixed unit
        return prefix::symbol;
      }
      else {
        // print as a ratio of the coherent unit
        return ratio_text<R>();
      }
    }
    else {
      // print as a ratio of the coherent unit
      return ratio_text<R>();
    }
  }
}

template<typename... Es, std::size_t... Idxs>
constexpr auto derived_dimension_unit_text(exp_list<Es...>, std::index_sequence<Idxs...>)
{
  constexpr auto neg_exp = negative_exp_count<Es...>;
  return (exp_text<Es, dimension_unit<typename Es::dimension>::symbol, neg_exp, Idxs>() + ...);
}

template<typename... Es>
constexpr auto derived_dimension_unit_text(exp_list<Es...> list)
{
  return derived_dimension_unit_text(list, std::index_sequence_for<Es...>());
}

template<Exponent... Es>
constexpr auto exp_list_with_named_units(exp_list<Es...>);

template<Exponent Exp>
constexpr auto exp_list_with_named_units(Exp)
{
  using dim = Exp::dimension;
  if constexpr(dimension_unit<dim>::is_named) {
    return exp_list<Exp>();
  }
  else {
    using recipe = dim::recipe;
    return exp_list_with_named_units(recipe());
  }
}

template<Exponent... Es>
constexpr auto exp_list_with_named_units(exp_list<Es...>)
{
  return type_list_join<decltype(exp_list_with_named_units(Es()))...>();
}

template<Dimension Dim>
constexpr auto derived_dimension_unit_text()
{
  using recipe = Dim::recipe;
  return derived_dimension_unit_text(exp_list_with_named_units(recipe()));
}

// TODO Inline below concept when switched to gcc-10
template<typename T>
concept has_symbol = requires{ T::symbol; };

template<Dimension Dim, Unit U>
constexpr auto unit_text()
{
  if constexpr(has_symbol<U>) {
    // already has a symbol so print it
    return U::symbol;
  }
  else {
    // print as a prefix or ratio of a coherent unit
    using coherent_unit = dimension_unit<Dim>;
    auto prefix_txt = prefix_or_ratio_text<U::ratio / coherent_unit::ratio, typename U::reference::prefix_family>();

    if constexpr(has_symbol<coherent_unit>) {
      // use predefined coherent unit symbol
      return prefix_txt + coherent_unit::symbol;
    }
    else {
      // use derived dimension ingredients to create a unit symbol
      return prefix_txt + derived_dimension_unit_text<Dim>();
    }
  }
}

template<typename CharT, class Traits, Quantity Q>
std::basic_string<CharT> to_string(const Q& q)
{
  std::basic_ostringstream<CharT, Traits> s;
  s << q.count();
  constexpr auto symbol = unit_text<typename Q::dimension, typename Q::unit>();
  if constexpr (symbol.standard().size()) {
    s << " " << symbol.standard();
  }
  return s.str();
}

}  // namespace units::detail
