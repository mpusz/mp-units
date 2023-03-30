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

// IWYU pragma: begin_exports
#include <units/bits/basic_concepts.h>
#include <units/bits/external/fixed_string.h>
// IWYU pragma: end_exports

#include <type_traits>

namespace units {

/**
 * @brief A dimension of a base quantity
 *
 * Base quantity is a quantity in a conventionally chosen subset of a given system of quantities, where no quantity
 * in the subset can be expressed in terms of the other quantities within that subset. They are referred to as
 * being mutually independent since a base quantity cannot be expressed as a product of powers of the other base
 * quantities.
 *
 * Base unit is a measurement unit that is adopted by convention for a base quantity in a specific system of units.
 *
 * Pair of Symbol and Unit template parameters form an unique identifier of the base dimension. The same identifiers can
 * be multiplied and divided which will result with an adjustment of its factor in an Exponent of a DerivedDimension
 * (in case of zero the dimension will be simplified and removed from further analysis of current expresion). In case
 * the Symbol is the same but the Unit differs (i.e. mixing SI and CGS length), there is no automatic simplification but
 * is possible to force it with a quantity_cast.
 *
 * @tparam Symbol an unique identifier of the base dimension used to provide dimensional analysis support
 * @tparam U a base unit to be used for this base dimension
 */
template<basic_fixed_string Symbol, NamedUnit U>
struct base_dimension {
  static constexpr auto symbol = Symbol;  ///< Unique base dimension identifier
  using base_unit = U;                    ///< Base unit adopted for this dimension
  static constexpr UNITS_CONSTRAINED_AUTO_WORKAROUND(Magnitude) auto mag = magnitude{};
};

// base_dimension_less
template<BaseDimension D1, BaseDimension D2>
struct base_dimension_less :
    std::bool_constant<(D1::symbol < D2::symbol) ||
                       (D1::symbol == D2::symbol && D1::base_unit::symbol < D1::base_unit::symbol)> {};

}  // namespace units
