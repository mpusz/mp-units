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
#include <units/bits/external/downcasting.h>
#include <units/exponent.h>

namespace units::detail {

/**
 * @brief A dimension of a derived quantity
 *
 * Expression of the dependence of a quantity on the base quantities (and their base dimensions) of a system of
 * quantities as a product of powers of factors corresponding to the base quantities, omitting any numerical factors.
 * A power of a factor is the factor raised to an exponent.
 *
 * A derived dimension can be formed from multiple exponents (i.e. speed is represented as "exponent<L, 1>, exponent<T,
 * -1>"). It is also possible to form a derived dimension with only one exponent (i.e. frequency is represented as just
 * "exponent<T, -1>").
 *
 * @note This class template is used by the library engine and should not be directly instantiated by the user.
 *
 * @tparam Es zero or more exponents of a derived dimension
 */
template<Exponent... Es>
  requires(BaseDimension<typename Es::dimension> && ...)
struct derived_dimension_base : downcast_base<derived_dimension_base<Es...>> {
  using exponents = exponent_list<Es...>;
};

template<typename T>
struct to_derived_dimension_base;

template<Exponent... Es>
struct to_derived_dimension_base<exponent_list<Es...>> {
  using type = derived_dimension_base<Es...>;
};

}  // namespace units::detail
