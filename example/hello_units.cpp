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

#include <units/physical/si/speed.h>
#include <units/physical/international/speed.h>
#include <units/format.h>
#include <iostream>

using namespace units::physical;

constexpr in_speed AUTO avg_speed(in_length AUTO d, in_time AUTO t)
{
  return d / t;
}

int main()
{
  using namespace units::physical::si::literals;
  in_speed AUTO v1 = avg_speed(220q_km, 2q_h);
  in_speed AUTO v2 = avg_speed(si::length<international::mile>(140), si::time<si::hour>(2));
  in_speed AUTO v3 = quantity_cast<si::metre_per_second>(v2);
  in_speed AUTO v4 = quantity_cast<int>(v3);

  std::cout << v1 << '\n';                             // 110 km/h
  std::cout << fmt::format("{}", v2) << '\n';          // 70 mi/h
  std::cout << fmt::format("{:%Q in %q}", v3) << '\n'; // 31.2928 in m/s
  std::cout << fmt::format("{:%Q}", v4) << '\n';       // 31
}
