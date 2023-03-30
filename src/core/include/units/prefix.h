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

#include <units/bits/basic_concepts.h>
#include <units/bits/external/downcasting.h>
// IWYU pragma: begin_exports
#include <units/magnitude.h>
#include <units/ratio.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

namespace units {

namespace detail {

template<Magnitude auto M>
struct prefix_base : downcast_base<prefix_base<M>> {
  static constexpr UNITS_CONSTRAINED_AUTO_WORKAROUND(Magnitude) auto mag = M;
};

}  // namespace detail

/**
 * @brief A prefix used to scale units
 *
 * Data from a prefix class is used in two cases:
 * - when defining a prefixed_unit its ratio is used to scale the reference unit and its
 *   symbol is used to prepend to the symbol of referenced unit
 * - when printing the symbol of a scaled unit that was not predefined by the user but its
 *   factor matches ratio of a prefix, its symbol will be prepended to the symbol of the unit
 *
 * @tparam Child inherited class type used by the downcasting facility (CRTP Idiom)
 * @tparam Symbol a text representation of the prefix
 * @tparam R factor to be used to scale a unit
 */
template<typename Child, basic_symbol_text Symbol, Magnitude auto M>
struct prefix : downcast_dispatch<Child, detail::prefix_base<M>, downcast_mode::on> {
  static constexpr auto symbol = Symbol;
};

}  // namespace units
