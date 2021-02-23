// The MIT License (MIT)
//
// Copyright (c) 2020 Yves Delley
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

#include <units/origin.h>
#include <units/physical/si/base/thermodynamic_temperature.h>
#include <units/physical/si/imperial/base/thermodynamic_temperature.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity_io.h>
#include <units/quantity_point.h>
#include <iostream>

using namespace units;

namespace si = units::physical::si;
namespace imperial = units::physical::si::imperial;

using namespace si::literals;
using namespace si::unit_constants;

int main()
{
  si::celsius_temperature_point<> freezing = 0_qp_deg_C;
  si::celsius_temperature_point<> boiling = 100_qp_deg_C;

  auto f_C = quantity_point_cast<si::celsius_temperature_point<>>(freezing);
  auto b_C = quantity_point_cast<si::celsius_temperature_point<>>(boiling);

  auto f_F = quantity_point_cast<imperial::fahrenheit_temperature_point<>>(freezing);
  auto b_F = quantity_point_cast<imperial::fahrenheit_temperature_point<>>(boiling);

  auto f_K = quantity_point_cast<si::kelvin_temperature_point<>>(freezing);
  auto b_K = quantity_point_cast<si::kelvin_temperature_point<>>(boiling);

  std::cout << "Freezing point: " << f_C.relative() << "; Boiling point: " << b_C.relative() << std::endl;
  std::cout << "Freezing point: " << f_F.relative() << "; Boiling point: " << b_F.relative() << std::endl;
  std::cout << "Freezing point: " << f_K.relative() << "; Boiling point: " << b_K.relative() << std::endl;
}
