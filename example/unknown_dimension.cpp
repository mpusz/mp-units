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
#include <iostream>

namespace {

template<units::physical::in_length D, units::physical::in_time T>
constexpr units::physical::in_speed AUTO avg_speed(D d, T t)
{
  return d / t;
}

void example()
{
  using namespace units::physical;
  using namespace units::physical::si::literals;

  in_length AUTO d1 = 123q_m;
  in_time AUTO t1 = 10q_s;
  in_speed AUTO v1 = avg_speed(d1, t1);

  auto temp1 = v1 * 50q_m;  // produces intermediate unknown dimension with 'unknown_coherent_unit' as its 'coherent_unit'
  in_speed AUTO v2 = temp1 / 100q_m; // back to known dimensions again
  in_length AUTO d2 = v2 * 60q_s;

  std::cout << "d1 = " << d1 << '\n';
  std::cout << "t1 = " << t1 << '\n';
  std::cout << "v1 = " << v1 << '\n';
  std::cout << "temp1 = " << temp1 << '\n';
  std::cout << "v2 = " << v2 << '\n';
  std::cout << "d2 = " << d2 << '\n';
}

} // namespace

int main()
{
  try {
    example();
  }
  catch (const std::exception& ex) {
    std::cerr << "Unhandled std exception caught: " << ex.what() << '\n';
  }
  catch (...) {
    std::cerr << "Unhandled unknown exception caught\n";
  }
}
