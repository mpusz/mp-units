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
#include <units/magnitude.h>

namespace units::detail {

// compatible_units
template<typename ExpList, Unit... Us>
inline constexpr bool compatible_units = false;

template<typename... Es, Unit... Us>
inline constexpr bool compatible_units<exponent_list<Es...>, Us...> = (UnitOf<Us, typename Es::dimension> && ...);

// derived_scaled_unit

template<Unit... Us, typename... Es>
constexpr Magnitude auto derived_mag(exponent_list<Es...>)
{
  return (magnitude<>{} * ... * pow<ratio{Es::num, Es::den}>(Us::mag / dimension_unit<typename Es::dimension>::mag));
}

template<DerivedDimension D, Unit... Us>
using derived_scaled_unit = scaled_unit<derived_mag<Us...>(typename D::recipe()), typename D::coherent_unit::reference>;

}  // namespace units::detail
