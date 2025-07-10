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
#include <mp-units/ext/format.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <array>
#include <iostream>
#endif
#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/framework/quantity_point.h>
#include <mp-units/math.h>
#include <mp-units/systems/isq/thermodynamics.h>
#include <mp-units/systems/si.h>
#endif

// Based on: https://www.kalmanfilter.net/kalman1d_pn.html#ex8

using namespace mp_units;

template<QuantityPoint QP>
void print_header(kalman::system_state_estimate<QP> initial)
{
  std::cout << MP_UNITS_STD_FMT::format("Initial: {}\n", initial.state(), initial.variance());
  std::cout << MP_UNITS_STD_FMT::format("{:>2} | {:>10} | {:>7} | {:>22} | {:>22}\n", "N", "Measured", "Gain",
                                        "Curr. Estimate", "Next Estimate");
}

template<QuantityPoint QP, QuantityOf<dimensionless> K>
void print(auto iteration, QP measured, K gain, kalman::system_state_estimate<QP> current,
           kalman::system_state_estimate<QP> next)
{
  std::cout << MP_UNITS_STD_FMT::format(
    "{:2} | {:10} | {:7:N[.4f]} | {:10:0[:N[.3f]]} {:11:N[.4f]} | {:10:0[:N[.3f]]} {:11:N[.4f]}\n", iteration, measured,
    gain, current.state(), current.variance(), next.state(), next.variance());
}

int main()
{
  using namespace mp_units::si::unit_symbols;
  using qp = quantity_point<isq::Celsius_temperature[deg_C]>;
  using estimate = kalman::system_state_estimate<qp>;
  using state = estimate::state_type;

  const quantity process_noise_variance = delta<pow<2>(deg_C)>(0.15);
  const estimate initial{state{qp{delta<deg_C>(10.)}}, delta<deg_C>(100.)};
  const std::array measurements = {qp{delta<deg_C>(50.486)}, qp{delta<deg_C>(50.963)}, qp{delta<deg_C>(51.597)},
                                   qp{delta<deg_C>(52.001)}, qp{delta<deg_C>(52.518)}, qp{delta<deg_C>(53.05)},
                                   qp{delta<deg_C>(53.438)}, qp{delta<deg_C>(53.858)}, qp{delta<deg_C>(54.465)},
                                   qp{delta<deg_C>(55.114)}};
  const quantity measurement_error = delta<deg_C>(0.1);
  const quantity measurement_variance = pow<2>(measurement_error);

  auto predict = [=](const estimate& current) {
    return estimate{current.state(), kalman::covariance_extrapolation(current.variance(), process_noise_variance)};
  };

  print_header(initial);
  estimate next = predict(initial);
  for (int index = 1; const auto& measurement : measurements) {
    const estimate& previous = next;
    const quantity gain = kalman::kalman_gain(previous.variance(), measurement_variance);
    const estimate current = state_estimate_update(previous, measurement, gain);
    next = predict(current);
    print(index++, measurement, gain, current, next);
  }
}
