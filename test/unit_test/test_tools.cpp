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

#include "units/ratio.h"

namespace {

  using namespace units;

  template<Ratio R1, Ratio R2>
  inline constexpr bool same = R1::num == R2::num && R1::den == R2::den;

  static_assert(same<ratio<2, 4>, ratio<1, 2>>);

  static_assert(std::is_same_v<ratio_multiply<ratio<4>, ratio<1, 8>>, ratio<1, 2>>);
  static_assert(std::is_same_v<ratio_multiply<ratio<4>, ratio<1, 2>>, ratio<2>>);
  static_assert(std::is_same_v<ratio_multiply<ratio<1, 8>, ratio<2>>, ratio<1, 4>>);
  static_assert(std::is_same_v<ratio_multiply<ratio<1, 2>, ratio<8>>, ratio<4>>);

  static_assert(std::is_same_v<ratio_divide<ratio<4>, ratio<2>>, ratio<2>>);
  static_assert(std::is_same_v<ratio_divide<ratio<2>, ratio<8>>, ratio<1, 4>>);
  static_assert(std::is_same_v<ratio_divide<ratio<1, 8>, ratio<2>>, ratio<1, 16>>);
  static_assert(std::is_same_v<ratio_divide<ratio<6>, ratio<3>>, ratio<2>>);

  // common_ratio

  static_assert(std::is_same_v<common_ratio<ratio<1>, ratio<1000>>, ratio<1>>);
  static_assert(std::is_same_v<common_ratio<ratio<1000>, ratio<1>>, ratio<1>>);
  static_assert(std::is_same_v<common_ratio<ratio<1>, ratio<1, 1000>>, ratio<1, 1000>>);
  static_assert(std::is_same_v<common_ratio<ratio<1, 1000>, ratio<1>>, ratio<1, 1000>>);

}  // namespace