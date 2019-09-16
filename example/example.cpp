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

#include <units/dimensions/velocity.h>
#include <iostream>

namespace {

namespace stde = std::experimental;
using namespace stde::units::literals;

template<stde::units::Length D, stde::units::Time T>
constexpr stde::units::Velocity avg_speed(D d, T t)
{
  return d / t;
}

template<stde::units::Velocity V, stde::units::Time T>
void example_1(V v, T t)
{
  const stde::units::Length distance = v * t;
  std::cout << "A car driving " << v.count() << " km/h in a time of " << t.count() << " minutes will pass "
            << stde::units::quantity_cast<stde::units::quantity<stde::units::metre, double>>(distance).count() << " metres.\n";
}

void example_2(double distance_v, double duration_v)
{
  stde::units::quantity<stde::units::kilometre> distance(distance_v);
  stde::units::quantity<stde::units::hour> duration(duration_v);
  const auto kmph = quantity_cast<stde::units::kilometre_per_hour>(avg_speed(distance, duration));
  std::cout << "Average speed of a car that makes " << distance.count() << " km in "
            << duration.count() << " hours is " << kmph.count() << " km/h.\n";
}

}

int main()
{
  try {
    example_1(60kmph, 10.0min);
    example_2(220, 2);
  }
  catch (const std::exception& ex) {
    std::cerr << "Unhandled std exception caught: " << ex.what() << '\n';
  }
  catch (...) {
    std::cerr << "Unhandled unknown exception caught\n";
  }
}
