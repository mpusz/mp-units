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

#include <units/base_dimension.h>
#include <units/bits/absolute_magnitude.h>
#include <units/bits/derived_dimension_base.h>
#include <units/bits/dim_consolidate.h>
#include <units/bits/dim_unpack.h>
#include <units/bits/external/downcasting.h>
#include <units/bits/external/type_list.h>

// IWYU pragma: begin_exports
#include <units/exponent.h>
// IWYU pragma: end_exports

namespace units {

namespace detail {

/**
 * @brief Converts user provided derived dimension specification into a valid units::derived_dimension_base definition
 *
 * User provided definition of a derived dimension may contain the same base dimension repeated more than once on the
 * list possibly hidden in other derived units provided by the user. The process here should:
 * 1. Extract derived dimensions into exponents of base dimensions.
 * 2. Sort the exponents so the same dimensions are placed next to each other.
 * 3. Consolidate contiguous range of exponents of the same base dimensions to a one (or possibly zero) exponent for
 *    this base dimension.
 */
template<Exponent... Es>
using make_dimension = TYPENAME to_derived_dimension_base<
  typename dim_consolidate<type_list_sort<typename dim_unpack<Es...>::type, exponent_less>>::type>::type;

}  // namespace detail

/**
 * @brief The list of exponents of dimensions (both base and derived) provided by the user
 *
 * This is the user's interface to create derived dimensions. Exponents list can contain powers of factors of both
 * base and derived dimensions. This is called a "recipe" of the dimension and among others is used to print
 * unnamed coherent units of this dimension.
 *
 * Coherent unit is a unit that, for a given system of quantities and for a chosen set of base units, is a product
 * of powers of base units with no other proportionality factor than one.
 *
 * The implementation is responsible for unpacking all of the dimensions into a list containing only base dimensions
 * and their factors and putting them to derived_dimension_base class template.
 *
 * Sometimes units of equivalent quantities in different systems of units do not share the same reference so they
 * cannot be easily converted to each other. An example can be a pressure for which a coherent unit in SI is pascal
 * and in CGS barye. Those two units are not directly related with each other with some ratio. As they both are
 * coherent units of their dimensions, the ratio between them is directly determined by the ratios of base units
 * defined in base dimensions end their exponents in the derived dimension recipe. To provide interoperability of
 * such quantities of different systems mag is being used. The result of the division of two
 * mag of two quantities of equivalent dimensions in two different systems gives a ratio between their
 * coherent units. Alternatively, the user would always have to directly define a barye in terms of pascal or vice
 * versa.
 *
 * @tparam Child inherited class type used by the downcasting facility (CRTP Idiom)
 * @tparam U a coherent unit of a derived dimension
 * @tparam Es the list of exponents of ingredient dimensions
 */
template<typename Child, Unit U, Exponent... Es>
struct derived_dimension : downcast_dispatch<Child, typename detail::make_dimension<Es...>> {
  using recipe = exponent_list<Es...>;
  using coherent_unit = U;
  static constexpr UNITS_CONSTRAINED_AUTO_WORKAROUND(Magnitude) auto mag =
    detail::absolute_magnitude(typename derived_dimension::exponents()) / U::mag;
};

}  // namespace units
