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

#include <units/bits/external/downcasting.h>
#include <units/bits/external/fixed_string.h>
#include <units/ratio.h>

namespace units {

/**
 * @brief The base for all prefix types
 *
 * Every prefix type should inherit from this type to satisfy PrefixType concept.
 */
struct prefix_type {};

/**
 * @brief No prefix possible for the unit
 *
 * This is a special prefix type tag specifying that the unit can not be scaled with any kind
 * of the prefix.
 */
struct no_prefix : prefix_type {};

namespace detail {

template<PrefixType PT, Ratio R>
struct prefix_base : downcast_base<prefix_base<PT, R>> {
  using prefix_type = PT;
  using ratio = R;
};

}  // namespace detail

/**
 * @brief A prefix used to scale units
 *
 * Data from a prefix class is used in two cases:
 * - when defining a prefixed_unit its ratio is used to scale the reference unit and its
 *   symbol is used to prepend to the symbol of referenced unit
 * - when printing the symbol of a scaled unit that was not predefined by the user but its
 *   factor matches ratio of a prefix from the specified prefix family, its symbol will be
 *   prepended to the symbol of the unit
 *
 * @tparam Child inherited class type used by the downcasting facility (CRTP Idiom)
 * @tparam PT a type of prefix family
 * @tparam Symbol a text representation of the prefix
 * @tparam R factor to be used to scale a unit
 */
template<typename Child, PrefixType PT, basic_fixed_string Symbol, Ratio R>
  requires (!std::same_as<PT, no_prefix>)
struct prefix : downcast_child<Child, detail::prefix_base<PT, ratio<R::num, R::den, R::exp>>> {
  static constexpr auto symbol = Symbol;
};

}  // namespace units
