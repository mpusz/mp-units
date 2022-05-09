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
#include <units/generic/dimensionless.h>
#include <units/isq/si/acceleration.h>
#include <units/isq/si/length.h>
#include <units/isq/si/speed.h>
#include <units/isq/si/time.h>
#include <array>
#include <iostream>

// Based on: https://www.kalmanfilter.net/alphabeta.html#ex4

using namespace units;

void print_header(const kalman::State auto& initial)
{
  std::cout << STD_FMT::format("Initial: {}\n", initial);
  std::cout << STD_FMT::format("{:>2} | {:>8} | {:>35} | {:>35}\n", "N", "Measured", "Curr. Estimate", "Next Estimate");
}

void print(auto iteration, Quantity auto measured, const kalman::State auto& current, const kalman::State auto& next)
{
  std::cout << STD_FMT::format("{:2} | {:8} | {:>35.1} | {:>35.1}\n", iteration, measured, current, next);
}

int main()
{
  using namespace units::isq;
  using namespace units::isq::si::references;
  using state =
    kalman::state<si::length<si::metre>, si::speed<si::metre_per_second>, si::acceleration<si::metre_per_second_sq>>;
  constexpr auto mps = m / s;
  constexpr auto mps2 = mps / s;

  const auto interval = 5. * s;
  const state initial = {30 * km, 50 * mps, 0 * mps2};
  const std::array measurements = {30160 * m, 30365 * m, 30890 * m, 31050 * m, 31785 * m,
                                   32215 * m, 33130 * m, 34510 * m, 36010 * m, 37265 * m};
  std::array gain = {dimensionless<one>(0.5), dimensionless<one>(0.4), dimensionless<one>(0.1)};

  print_header(initial);
  state next = state_extrapolation(initial, interval);
  for (int index = 1; const auto& measured : measurements) {
    const auto& previous = next;
    const auto current = state_update(previous, measured, gain, interval);
    next = state_extrapolation(current, interval);
    print(index++, measured, current, next);
  }
}
