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
#include <mp-units/format.h>
#include <mp-units/math.h>
#include <mp-units/quantity_point.h>
#include <mp-units/systems/isq/thermodynamics.h>
#include <mp-units/systems/si/si.h>
#include <array>
#include <iostream>

// Based on: https://www.kalmanfilter.net/kalman1d.html#ex8

using namespace mp_units;

template<QuantityPoint QP>
void print_header(kalman::estimation<QP> initial)
{
  std::cout << MP_UNITS_STD_FMT::format("Initial: {}\n", initial);
  std::cout << MP_UNITS_STD_FMT::format("{:>2} | {:>7} | {:>10} | {:>16} | {:>16}\n", "N", "Gain", "Measured",
                                        "Curr. Estimate", "Next Estimate");
}

template<QuantityPoint QP, QuantityOf<dimensionless> K>
void print(auto iteration, K gain, QP measured, kalman::estimation<QP> current, kalman::estimation<QP> next)
{
  std::cout << MP_UNITS_STD_FMT::format("{:2} | {:7%.3Q} | {:10%.3Q %q} | {:>16.2} | {:>16.2}\n", iteration, gain,
                                        measured.quantity_ref_from(QP::point_origin), current, next);
}

int main()
{
  constexpr auto deg_C = isq::Celsius_temperature[si::degree_Celsius];

  using namespace kalman;

  const auto process_noise_variance = 0.15 * (deg_C * deg_C);
  const estimation initial = {state{si::ice_point + 10. * deg_C}, pow<2>(100. * deg_C)};
  const std::array measurements = {si::ice_point + 50.45 * deg_C,  si::ice_point + 50.967 * deg_C,
                                   si::ice_point + 51.6 * deg_C,   si::ice_point + 52.106 * deg_C,
                                   si::ice_point + 52.492 * deg_C, si::ice_point + 52.819 * deg_C,
                                   si::ice_point + 53.433 * deg_C, si::ice_point + 54.007 * deg_C,
                                   si::ice_point + 54.523 * deg_C, si::ice_point + 54.99 * deg_C};
  const auto measurement_uncertainty = pow<2>(0.1 * deg_C);

  auto update = [=]<QuantityPoint QP>(const estimation<QP>& previous, const QP& meassurement,
                                      QuantityOf<dimensionless> auto gain) {
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
