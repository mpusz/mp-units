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

#include <units/generic/angle.h>
#include <units/isq/si/force.h>
#include <units/isq/si/length.h>
#include <units/isq/si/torque.h>
#include <units/math.h>
#include <units/quantity_io.h>
#include <iostream>

int main()
{
  using namespace units;
  using namespace units::isq;
  using namespace units::aliases;
  using namespace units::aliases::isq::si;

  const Length auto lever = cm<>(20);
  const Force auto force = N<>(500);
  const Angle auto angle = deg<>(90);
  const Torque auto torque = lever * force * sin(angle) / cotes_angle<>;

  std::cout << "Applying a perpendicular force of " << force << " to a " << lever << " long lever results in "
            << quantity_cast<si::newton_metre_per_radian>(torque) << " of torque.\n";
}
