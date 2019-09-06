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

#include "units/dimension.h"
#include <utility>

using namespace std::experimental::units;

namespace {

  inline constexpr base_dimension d0{"d0"};
  inline constexpr base_dimension d1{"d1"};
  inline constexpr base_dimension d2{"d2"};
  inline constexpr base_dimension d3{"d3"};

  // exp_invert

  static_assert(std::is_same_v<exp_invert_t<exp<d0, 1>>, exp<d0, -1>>);
  static_assert(std::is_same_v<exp_invert_t<exp<d1, -1>>, exp<d1, 1>>);

  // make_dimension

  static_assert(std::is_same_v<make_dimension_t<exp<d0, 1>>, dimension<exp<d0, 1>>>);
  static_assert(std::is_same_v<make_dimension_t<exp<d0, 1>, exp<d1, 1>>, dimension<exp<d0, 1>, exp<d1, 1>>>);
  static_assert(std::is_same_v<make_dimension_t<exp<d1, 1>, exp<d0, 1>>, dimension<exp<d0, 1>, exp<d1, 1>>>);
  static_assert(std::is_same_v<make_dimension_t<exp<d1, 1>, exp<d1, 1>>, dimension<exp<d1, 2>>>);
  static_assert(std::is_same_v<make_dimension_t<exp<d1, 1>, exp<d1, -1>>, dimension<>>);

  static_assert(std::is_same_v<make_dimension_t<exp<d0, 1>, exp<d1, 1>, exp<d0, 1>, exp<d1, 1>>, dimension<exp<d0, 2>, exp<d1, 2>>>);
  static_assert(
      std::is_same_v<make_dimension_t<exp<d0, -1>, exp<d1, -1>, exp<d0, -1>, exp<d1, -1>>, dimension<exp<d0, -2>, exp<d1, -2>>>);

  static_assert(std::is_same_v<make_dimension_t<exp<d0, 1>, exp<d1, 1>, exp<d1, -1>>, dimension<exp<d0, 1>>>);
  static_assert(std::is_same_v<make_dimension_t<exp<d0, 1>, exp<d0, -1>, exp<d1, 1>>, dimension<exp<d1, 1>>>);
  static_assert(std::is_same_v<make_dimension_t<exp<d0, 1>, exp<d1, 1>, exp<d0, -1>>, dimension<exp<d1, 1>>>);
  static_assert(std::is_same_v<make_dimension_t<exp<d0, 1>, exp<d1, 1>, exp<d0, -1>, exp<d1, -1>>, dimension<>>);

  // dimension_multiply

  static_assert(
      std::is_same_v<dimension_multiply_t<dimension<exp<d0, 1>>, dimension<exp<d1, 1>>>, dimension<exp<d0, 1>, exp<d1, 1>>>);
  static_assert(std::is_same_v<dimension_multiply_t<dimension<exp<d0, 1>, exp<d1, 1>, exp<d2, 1>>, dimension<exp<d3, 1>>>,
                               dimension<exp<d0, 1>, exp<d1, 1>, exp<d2, 1>, exp<d3, 1>>>);
  static_assert(std::is_same_v<dimension_multiply_t<dimension<exp<d0, 1>, exp<d1, 1>, exp<d2, 1>>, dimension<exp<d1, 1>>>,
                               dimension<exp<d0, 1>, exp<d1, 2>, exp<d2, 1>>>);
  static_assert(std::is_same_v<dimension_multiply_t<dimension<exp<d0, 1>, exp<d1, 1>, exp<d2, 1>>, dimension<exp<d1, -1>>>,
                               dimension<exp<d0, 1>, exp<d2, 1>>>);

  // dimension_divide

  static_assert(
      std::is_same_v<dimension_divide_t<dimension<exp<d0, 1>>, dimension<exp<d1, 1>>>, dimension<exp<d0, 1>, exp<d1, -1>>>);
  static_assert(std::is_same_v<dimension_divide_t<dimension<exp<d0, 1>>, dimension<exp<d0, 1>>>, dimension<>>);

}  // namespace
