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

#include <mp-units/math.h>
#include <mp-units/ostream.h>
#include <mp-units/systems/isq_angle/isq_angle.h>
#include <mp-units/systems/si/unit_symbols.h>
#include <iostream>

template<class T>
  requires mp_units::is_scalar<T>
inline constexpr bool mp_units::is_vector<T> = true;

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;
  using namespace mp_units::angular::unit_symbols;
  using mp_units::angular::unit_symbols::deg;
  using mp_units::angular::unit_symbols::rad;

  const auto lever = isq_angle::position_vector(20 * cm);
  const auto force = isq_angle::force(500 * N);
  const auto angle = isq_angle::angular_measure(90. * deg);
  const auto torque = isq_angle::torque(lever * force * angular::sin(angle) / (1 * isq_angle::cotes_angle));

  std::cout << "Applying a perpendicular force of " << force << " to a " << lever << " long lever results in "
            << torque.in(N * m / rad) << " of torque.\n";
}
