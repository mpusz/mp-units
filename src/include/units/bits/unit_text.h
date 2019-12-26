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

#include <units/derived_dimension.h>

namespace units::detail {

template<typename Ratio>
constexpr auto ratio_text()
{
  if constexpr(Ratio::num != 1 || Ratio::den != 1) {
    auto txt = basic_fixed_string("[") + regular<Ratio::num>();
    if constexpr(Ratio::den == 1) {
      return txt + basic_fixed_string("]");
    }
    else {
      return txt + basic_fixed_string("/") + regular<Ratio::den>() + basic_fixed_string("]");
    }
  }
  else {
    return basic_fixed_string("");
  }
}

template<typename Ratio, typename PrefixType>
constexpr auto prefix_or_ratio_text()
{
  if constexpr(Ratio::num == 1 && Ratio::den == 1) {
    // no ratio/prefix
    return basic_fixed_string("");
  }
  else {
    if constexpr (!std::same_as<PrefixType, no_prefix>) {
      // try to form a prefix
      using prefix = downcast<detail::prefix_base<PrefixType, Ratio>>;

      if constexpr(!std::same_as<prefix, prefix_base<PrefixType, Ratio>>) {
        // print as a prefixed unit
        return prefix::symbol;
      }
      else {
        // print as a ratio of the coherent unit
        return ratio_text<Ratio>();
      }
    }
    else {
      // print as a ratio of the coherent unit
      return ratio_text<Ratio>();
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

template<typename... Es>
constexpr bool all_named(exp_list<Es...>)
{
  return (dimension_unit<typename Es::dimension>::is_named && ...);
}

template<Dimension Dim>
constexpr auto derived_dimension_unit_text()
{
  using recipe = typename Dim::recipe;
  if constexpr(all_named(recipe()))
    return derived_dimension_unit_text(recipe());
  else
    return derived_dimension_unit_text(typename Dim::exponents());
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
    // print as a prefix or ratio of a reference unit
    auto prefix_txt = prefix_or_ratio_text<typename U::ratio, typename U::reference::prefix_type>();

    if constexpr(has_symbol<typename U::reference>) {
      // use predefined reference unit symbol
      return prefix_txt + U::reference::symbol;
    }
    else {
      // use derived dimension ingredients to create a unit symbol
      return prefix_txt + derived_dimension_unit_text<Dim>();
    }
  }
}

}  // namespace units::detail
