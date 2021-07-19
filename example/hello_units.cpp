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

#include <units/format.h>
#include <units/isq/si/international/length.h>
#include <units/isq/si/international/speed.h> // IWYU pragma: keep
#include <units/isq/si/length.h>
#include <units/isq/si/speed.h> // IWYU pragma: keep
#include <units/isq/si/time.h>
#include <units/quantity_io.h>
#include <iostream>

using namespace units::isq;

constexpr Speed auto avg_speed(Length auto d, Time auto t)
{
  return d / t;
}

int main()
{
  using namespace units::isq::si::literals;
  using namespace units::isq::si::references;
  using namespace units::aliases::isq::si::international;

  constexpr Speed auto v1 = 110 * (km / h);
  constexpr Speed auto v2 = mi_per_h<>(70.);
  constexpr Speed auto v3 = avg_speed(220_q_km, 2_q_h);
  constexpr Speed auto v4 = avg_speed(si::length<si::international::mile>(140), si::time<si::hour>(2));
#if UNITS_DOWNCAST_MODE == 0
  constexpr Speed auto v5 = quantity_cast<si::speed<si::metre_per_second>>(v3);
  constexpr Speed auto v6 = quantity_cast<si::dim_speed, si::metre_per_second>(v4);
#else
  constexpr Speed auto v5 = quantity_cast<si::speed<si::metre_per_second>>(v3);
  constexpr Speed auto v6 = quantity_cast<si::metre_per_second>(v4);
#endif
  constexpr Speed auto v7 = quantity_cast<int>(v6);

  std::cout << v1 << '\n';                                  // 110 km/h
  std::cout << v2 << '\n';                                  // 70 mi/h
  std::cout << fmt::format("{}", v3) << '\n';               // 110 km/h
  std::cout << fmt::format("{:*^14}", v4) << '\n';          // ***70 mi/h****
  std::cout << fmt::format("{:%Q in %q}", v5) << '\n';      // 30.5556 in m/s
  std::cout << fmt::format("{0:%Q} in {0:%q}", v6) << '\n'; // 31.2928 in m/s
  std::cout << fmt::format("{:%Q}", v7) << '\n';            // 31
}
