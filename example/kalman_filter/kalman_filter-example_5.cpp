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
#include <array>
#include <iostream>
#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/format.h>
#include <mp-units/math.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si/unit_symbols.h>
#endif

// Based on: https://www.kalmanfilter.net/kalman1d.html#ex5

using namespace mp_units;

template<QuantityPoint QP>
void print_header(kalman::system_state_estimate<QP> initial)
{
  std::cout << MP_UNITS_STD_FMT::format("Initial: {} {}\n", initial.state(), initial.variance());
  std::cout << MP_UNITS_STD_FMT::format("{:>2} | {:>8} | {:>5} | {:>16} | {:>16}\n", "N", "Measured", "Gain",
                                        "Curr. Estimate", "Next Estimate");
}

template<QuantityPoint QP, QuantityOf<dimensionless> K>
void print(auto iteration, QP measured, K gain, kalman::system_state_estimate<QP> current,
           kalman::system_state_estimate<QP> next)
{
  std::cout << MP_UNITS_STD_FMT::format(
    "{:2} | {:8} | {:5:N[.2f]} | {:6:0[:N[.2f]]} {:8:N[.2f]} | {:6:0[:N[.2f]]} {:8:N[.2f]}\n", iteration, measured,
    gain, current.state(), current.variance(), next.state(), next.variance());
}

int main()
{
  using namespace mp_units::si::unit_symbols;
  using qp = quantity_point<isq::height[m]>;
  using estimate = kalman::system_state_estimate<qp>;
  using state = estimate::state_type;

  const estimate initial{state{qp{60. * m}}, 15. * m};
  const std::array measurements = {qp{49.03 * m}, qp{48.44 * m}, qp{55.21 * m}, qp{49.98 * m}, qp{50.6 * m},
                                   qp{52.61 * m}, qp{45.87 * m}, qp{42.64 * m}, qp{48.26 * m}, qp{55.84 * m}};
  const quantity measurement_error = isq::height(5. * m);
  const quantity measurement_variance = pow<2>(measurement_error);

  auto predict = [](const estimate& current) { return current; };

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
