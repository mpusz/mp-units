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
#include <units/exponent.h>
#include <units/magnitude.h>
#include <units/ratio.h>

namespace units::detail {

/**
 * @brief Calculates the "absolute" magnitude of the derived dimension defined by this list.
 *
 * "Absolute" magnitudes are not physically observable: only ratios of magnitudes are.  For example: if we multiplied
 * all magnitudes in the system by the same constant, no meaningful results would change.  However, in practice, we need
 * to make some global choice for the "absolute" values of magnitudes, so that we can compute their ratios.
 *
 * The point of this function is to compute the absolute magnitude of a derived dimension, in terms of the absolute
 * magnitudes of its constituent dimensions.
 */
template<typename... Es>
constexpr Magnitude auto absolute_magnitude(exponent_list<Es...>)
{
  return (magnitude<>{} * ... * pow<ratio{Es::num, Es::den}>(Es::dimension::base_unit::mag));
}

}  // namespace units::detail
