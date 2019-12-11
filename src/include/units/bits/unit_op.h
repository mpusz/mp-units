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

namespace units::detail {

template<Dimension Dim1, Unit U1, Dimension Dim2, Unit U2,
         typename Ratio1 = ratio_divide<typename U1::ratio, typename dimension_unit<Dim1>::ratio>,
         typename Ratio2 = ratio_divide<typename U2::ratio, typename dimension_unit<Dim2>::ratio>>
using unit_ratio_multiply = ratio_multiply<Ratio1, Ratio2>;

template<Dimension Dim1, Unit U1, Dimension Dim2, Unit U2,
         typename Ratio1 = ratio_divide<typename U1::ratio, typename dimension_unit<Dim1>::ratio>,
         typename Ratio2 = ratio_divide<typename U2::ratio, typename dimension_unit<Dim2>::ratio>>
using unit_ratio_divide = ratio_divide<Ratio1, Ratio2>;

}  // namespace units::detail
