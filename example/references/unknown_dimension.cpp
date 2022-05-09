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

#include <units/isq/si/length.h>
#include <units/isq/si/speed.h>  // IWYU pragma: keep
#include <units/isq/si/time.h>
#include <units/quantity_io.h>
#include <exception>
#include <iostream>

namespace {

template<units::isq::Length D, units::isq::Time T>
constexpr units::isq::Speed auto avg_speed(D d, T t)
{
  return d / t;
}

void example()
{
  using namespace units::isq;
  using namespace units::isq::si::references;

  Length auto d1 = 123 * m;
  Time auto t1 = 10 * s;
  Speed auto v1 = avg_speed(d1, t1);

  auto temp1 =
    v1 * (50 * m);  // produces intermediate unknown dimension with 'unknown_coherent_unit' as its 'coherent_unit'
  Speed auto v2 = temp1 / (100 * m);  // back to known dimensions again
  Length auto d2 = v2 * (60 * s);

  std::cout << "d1 = " << d1 << '\n';
  std::cout << "t1 = " << t1 << '\n';
  std::cout << "v1 = " << v1 << '\n';
  std::cout << "temp1 = " << temp1 << '\n';
  std::cout << "v2 = " << v2 << '\n';
  std::cout << "d2 = " << d2 << '\n';
}

}  // namespace

int main()
{
  try {
    example();
  } catch (const std::exception& ex) {
    std::cerr << "Unhandled std exception caught: " << ex.what() << '\n';
  } catch (...) {
    std::cerr << "Unhandled unknown exception caught\n";
  }
}
