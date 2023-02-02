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
#include <mp_units/format.h>
#include <mp_units/math.h>
#include <mp_units/systems/isq/space_and_time.h>
#include <mp_units/systems/si/unit_symbols.h>
#include <array>
#include <iostream>

// Based on: https://www.kalmanfilter.net/kalman1d.html#ex5

using namespace mp_units;

template<Quantity Q>
void print_header(kalman::estimation<Q> initial)
{
  std::cout << STD_FMT::format("Initial: {}\n", initial);
  std::cout << STD_FMT::format("{:>2} | {:>5} | {:>8} | {:>16} | {:>16}\n", "N", "Gain", "Measured", "Curr. Estimate",
                               "Next Estimate");
}

template<Quantity Q, QuantityOf<dimensionless> K>
void print(auto iteration, K gain, Q measured, kalman::estimation<Q> current, kalman::estimation<Q> next)
{
  std::cout << STD_FMT::format("{:2} | {:5%.2Q} | {:8} | {:>16.2} | {:>16.2}\n", iteration, gain, measured, current,
                               next);
}

int main()
{
  using namespace kalman;
  using namespace mp_units::si::unit_symbols;

  const estimation initial = {state{60. * isq::height[m]}, pow<2>(15. * isq::height[m])};
  const std::array measurements = {48.54 * isq::height[m], 47.11 * isq::height[m], 55.01 * isq::height[m],
                                   55.15 * isq::height[m], 49.89 * isq::height[m], 40.85 * isq::height[m],
                                   46.72 * isq::height[m], 50.05 * isq::height[m], 51.27 * isq::height[m],
                                   49.95 * isq::height[m]};
  const auto measurement_uncertainty = pow<2>(5. * isq::height[m]);

  auto update = [=]<Quantity Q>(const estimation<Q>& previous, const Q& measurement,
                                QuantityOf<dimensionless> auto gain) {
    return estimation{state_update(previous.state, measurement, gain), covariance_update(previous.uncertainty, gain)};
  };

  auto predict = []<Quantity Q>(const estimation<Q>& current) { return current; };

  print_header(initial);
  estimation next = predict(initial);
  for (int index = 1; const auto& measured : measurements) {
    const auto& previous = next;
    const auto gain = kalman_gain(previous.uncertainty, measurement_uncertainty);
    const estimation current = update(previous, measured, gain);
    next = predict(current);
    print(index++, gain, measured, current, next);
  }
}
