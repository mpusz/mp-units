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

#include <units/point_origin.h>
#include <units/isq/si/thermodynamic_temperature.h>
#include <units/isq/si/imperial/thermodynamic_temperature.h>
#include <units/isq/si/prefixes.h>
#include <units/quantity_io.h>
#include <units/quantity_point.h>
#include <iostream>

using namespace units;

namespace si = units::isq::si;
namespace imperial = units::isq::si::imperial;

using namespace units::isq::si::literals;
using namespace units::isq::si::references;
using namespace units::isq::si::imperial::references;

template <typename QP>
void print_typical_temperatures() {
  // Highlighting different methods to construct a thermodynamic_temperature_point:
  static constexpr auto absolute = units::absolute<si::kelvin_temperature_origin>(0*K);
  static constexpr auto really_cold = units::absolute<imperial::fahrenheit_temperature_origin>(0*deg_F);
  static constexpr auto freezing = 0_qp_deg_C;
  static constexpr auto human_body = interpret_as_temperature_point(98*deg_F);
  static constexpr auto boiling =  units::absolute<si::celsius_temperature_origin>(100_q_deg_C);

  std::cout << "Absolute zero: " << quantity_point_cast<QP>(absolute).relative();
  std::cout << ", really cold: " << quantity_point_cast<QP>(really_cold).relative();
  std::cout << ", water freezing: " << quantity_point_cast<QP>(freezing).relative();
  std::cout << ", typical human body: " << quantity_point_cast<QP>(human_body).relative();
  std::cout << ", water boiling: " << quantity_point_cast<QP>(boiling).relative();
  std::cout << std::endl;
}

int main()
{
  print_typical_temperatures<si::celsius_temperature_point<>>();
  print_typical_temperatures<imperial::fahrenheit_temperature_point<>>();
  print_typical_temperatures<si::kelvin_temperature_point<>>();
}
