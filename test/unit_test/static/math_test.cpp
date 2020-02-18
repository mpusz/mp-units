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

#include "units/physical/si/area.h"
#include "units/physical/si/velocity.h"
#include "units/physical/international/area.h"
#include "units/math.h"

namespace {

  using namespace units::si::literals;
  using namespace units::international::literals;

  static_assert(std::is_same_v<decltype(pow<0>(2q_m)), std::int64_t>);
  static_assert(std::is_same_v<decltype(pow<1>(2q_m)), decltype(2q_m)>);
  static_assert(std::is_same_v<decltype(pow<2>(2q_m)), decltype(4q_m2)>);
  static_assert(std::is_same_v<decltype(pow<2>(2q_km)), decltype(4q_km2)>);
  static_assert(std::is_same_v<decltype(pow<2>(2q_ft)), decltype(4q_ft2)>);
  static_assert(std::is_same_v<decltype(sqrt(4q_m2)), decltype(2q_m)>);
  static_assert(std::is_same_v<decltype(sqrt(4q_km2)), decltype(2q_km)>);
  static_assert(std::is_same_v<decltype(sqrt(4q_ft2)), decltype(2q_ft)>);


}  // namespace
