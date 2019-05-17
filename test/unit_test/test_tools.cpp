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

#include "units/bits/ratio_tools.h"

namespace {

  using namespace units;
  
  // static_sign

  static_assert(static_sign<2>::value == 1);
  static_assert(static_sign<-3>::value == -1);
  static_assert(static_sign<0>::value == 1);

  // static_abs

  static_assert(static_abs<2>::value == 2);
  static_assert(static_abs<-3>::value == 3);
  static_assert(static_abs<0>::value == 0);

  // common_ratio

  static_assert(std::is_same_v<common_ratio_t<std::ratio<1>, std::kilo>, std::ratio<1>>);
  static_assert(std::is_same_v<common_ratio_t<std::kilo, std::ratio<1>>, std::ratio<1>>);
  static_assert(std::is_same_v<common_ratio_t<std::ratio<1>, std::milli>, std::milli>);
  static_assert(std::is_same_v<common_ratio_t<std::milli, std::ratio<1>>, std::milli>);

}  // namespace