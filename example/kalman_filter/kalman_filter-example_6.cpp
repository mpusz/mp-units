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

#include "kalman.h"
#include <units/format.h>
#include <units/isq/si/thermodynamic_temperature.h>
#include <units/math.h>
#include <units/quantity_point.h>
#include <units/unit.h>
#include <array>
#include <iostream>

// TODO Fix when Celsius is properly supported (#232)
namespace units::isq::si {

struct degree_celsius : alias_unit<kelvin, basic_symbol_text{"°C", "deg_C"}> {};

namespace thermodynamic_temperature_references {

inline constexpr auto deg_C = reference<dim_thermodynamic_temperature, degree_celsius>{};

}  // namespace thermodynamic_temperature_references

namespace references {

using namespace thermodynamic_temperature_references;

}  // namespace references

}  // namespace units::isq::si

// Based on: https://www.kalmanfilter.net/kalman1d.html#ex6

using namespace units;

template<QuantityPoint QP>
void print_header(kalman::estimation<QP> initial)
{
  std::cout << STD_FMT::format("Initial: {}\n", initial);
  std::cout << STD_FMT::format("{:>2} | {:>7} | {:>10} | {:>18} | {:>18}\n", "N", "Gain", "Measured", "Curr. Estimate",
                               "Next Estimate");
}

template<QuantityPoint QP, Dimensionless K>
void print(auto iteration, K gain, QP measured, kalman::estimation<QP> current, kalman::estimation<QP> next)
{
  std::cout << STD_FMT::format("{:2} | {:7%.4Q} | {:10%.3Q %q} | {:>18.3} | {:>18.3}\n", iteration, gain,
                               measured.relative(), current, next);
}

int main()
{
  using namespace kalman;
  using namespace units::isq;
  using namespace units::isq::si::references;

  const auto process_noise_variance = 0.0001 * (deg_C * deg_C);
  const estimation initial = {state{quantity_point(10. * deg_C)}, pow<2>(100. * deg_C)};
  const std::array measurements = {quantity_point(49.95 * deg_C),  quantity_point(49.967 * deg_C),
                                   quantity_point(50.1 * deg_C),   quantity_point(50.106 * deg_C),
                                   quantity_point(49.992 * deg_C), quantity_point(49.819 * deg_C),
                                   quantity_point(49.933 * deg_C), quantity_point(50.007 * deg_C),
                                   quantity_point(50.023 * deg_C), quantity_point(49.99 * deg_C)};
  const auto measurement_uncertainty = pow<2>(0.1 * deg_C);

  auto update = [=]<QuantityPoint QP>(const estimation<QP>& previous, const QP& meassurement, Dimensionless auto gain) {
    return estimation{state_update(previous.state, meassurement, gain), covariance_update(previous.uncertainty, gain)};
  };

  auto predict = [=]<QuantityPoint QP>(const estimation<QP>& current) {
    return estimation{current.state, covariance_extrapolation(current.uncertainty, process_noise_variance)};
  };

  print_header(initial);
  estimation next = predict(initial);
  for (int index = 1; const auto& m : measurements) {
    const auto& previous = next;
    const auto gain = kalman_gain(previous.uncertainty, measurement_uncertainty);
    const estimation current = update(previous, m, gain);
    next = predict(current);
    print(index++, gain, m, current, next);
  }
}
