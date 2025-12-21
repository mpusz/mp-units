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

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!! Before you commit any changes to this file please make sure to check if it !!!
// !!! renders correctly in the documentation "Examples" section.                 !!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#include "measurement.h"
#include <mp-units/bits/hacks.h>
#include <mp-units/compat_macros.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <iostream>
#endif
#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/framework.h>
#include <mp-units/math.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si.h>
#endif

namespace {

static_assert(mp_units::RepresentationOf<measurement<double>, mp_units::quantity_character::real_scalar>);
static_assert(mp_units::RepresentationOf<measurement<double>, mp_units::quantity_character::vector>);

void example()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  std::cout << "Mass of the Sun:        M_sun = " << measurement{19884, 2} * (mag_power<10, 26> * kg) << '\n';

  const auto acceleration = isq::acceleration(measurement{9.8, 0.1} * m / s2);
  const auto time = measurement{1.2, 0.1} * s;
  const QuantityOf<isq::velocity> auto velocity = acceleration * time;
  std::cout << "Velocity calculation:   V = " << acceleration << " * " << time << " = " << velocity << " = "
            << velocity.in(km / h) << '\n';

  const auto length = measurement{123., 1.} * m;
  std::cout << "Scalar multiplication:  d = 10 * " << length << " = " << 10 * length << '\n';

  const auto radius = measurement{5.0, 0.1} * m;
  const auto circumference = radius * (mag<2> * mag<π> * one);
  const auto area = pow<2>(radius) * (mag<π> * one);
  std::cout << "Radius:                 r = " << radius << '\n';
  std::cout << "Circular circumference: 2πr = " << circumference << " = " << circumference.in(m) << '\n';
  std::cout << "Circular area:          πr² = " << area << " = " << area.in(m2) << '\n';

  const auto area_measured = measurement{25.0, 1.0} * (mag<π> * m2);
  const auto radius_from_area = sqrt(area_measured / (mag<π> * one));
  std::cout << "Radius from area:       A = " << area_measured << " -> r = √(A/π) = " << radius_from_area << '\n';
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
