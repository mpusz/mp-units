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

using namespace units;

namespace {

  template<int Id, int Value>
  using e = exp<dim_id<Id>, Value>;

  // exp_invert

  static_assert(std::is_same_v<exp_invert_t<e<0, 1>>, e<0, -1>>);
  static_assert(std::is_same_v<exp_invert_t<e<1, -1>>, e<1, 1>>);

  // make_dimension

  static_assert(std::is_same_v<make_dimension_t<e<0, 1>>, dimension<e<0, 1>>>);
  static_assert(std::is_same_v<make_dimension_t<e<0, 1>, e<1, 1>>, dimension<e<0, 1>, e<1, 1>>>);
  static_assert(std::is_same_v<make_dimension_t<e<1, 1>, e<0, 1>>, dimension<e<0, 1>, e<1, 1>>>);
  static_assert(std::is_same_v<make_dimension_t<e<1, 1>, e<1, 1>>, dimension<e<1, 2>>>);
  static_assert(std::is_same_v<make_dimension_t<e<1, 1>, e<1, -1>>, dimension<>>);

  static_assert(std::is_same_v<make_dimension_t<e<0, 1>, e<1, 1>, e<0, 1>, e<1, 1>>, dimension<e<0, 2>, e<1, 2>>>);
  static_assert(
      std::is_same_v<make_dimension_t<e<0, -1>, e<1, -1>, e<0, -1>, e<1, -1>>, dimension<e<0, -2>, e<1, -2>>>);

  static_assert(std::is_same_v<make_dimension_t<e<0, 1>, e<1, 1>, e<1, -1>>, dimension<e<0, 1>>>);
  static_assert(std::is_same_v<make_dimension_t<e<0, 1>, e<0, -1>, e<1, 1>>, dimension<e<1, 1>>>);
  static_assert(std::is_same_v<make_dimension_t<e<0, 1>, e<1, 1>, e<0, -1>>, dimension<e<1, 1>>>);
  static_assert(std::is_same_v<make_dimension_t<e<0, 1>, e<1, 1>, e<0, -1>, e<1, -1>>, dimension<>>);

  // dimension_multiply

  static_assert(
      std::is_same_v<dimension_multiply_t<dimension<e<0, 1>>, dimension<e<1, 1>>>, dimension<e<0, 1>, e<1, 1>>>);
  static_assert(std::is_same_v<dimension_multiply_t<dimension<e<0, 1>, e<1, 1>, e<2, 1>>, dimension<e<3, 1>>>,
                               dimension<e<0, 1>, e<1, 1>, e<2, 1>, e<3, 1>>>);
  static_assert(std::is_same_v<dimension_multiply_t<dimension<e<0, 1>, e<1, 1>, e<2, 1>>, dimension<e<1, 1>>>,
                               dimension<e<0, 1>, e<1, 2>, e<2, 1>>>);
  static_assert(std::is_same_v<dimension_multiply_t<dimension<e<0, 1>, e<1, 1>, e<2, 1>>, dimension<e<1, -1>>>,
                               dimension<e<0, 1>, e<2, 1>>>);

  // dimension_divide

  static_assert(
      std::is_same_v<dimension_divide_t<dimension<e<0, 1>>, dimension<e<1, 1>>>, dimension<e<0, 1>, e<1, -1>>>);
  static_assert(std::is_same_v<dimension_divide_t<dimension<e<0, 1>>, dimension<e<0, 1>>>, dimension<>>);

}  // namespace
