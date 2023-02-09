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

#include <mp_units/format.h>
#include <mp_units/iostream.h>
#include <mp_units/systems/international/international.h>
#include <mp_units/systems/isq/space_and_time.h>
#include <mp_units/systems/si/unit_symbols.h>
#include <iostream>

using namespace mp_units;

constexpr QuantityOf<isq::speed> auto avg_speed(QuantityOf<isq::distance> auto d, QuantityOf<isq::duration> auto t)
{
  return quantity_cast<isq::speed>(d / t);
}

int main()
{
  using namespace mp_units::si::unit_symbols;
  using namespace mp_units::international::unit_symbols;

  constexpr auto v1 = 110 * (km / h);
  constexpr auto v2 = 70. * mph;
  constexpr auto v3 = avg_speed(220 * km, 2 * h);
  constexpr auto v4 = avg_speed(140 * mi, 2 * h);
  constexpr auto v5 = value_cast<m / s>(v3);
  constexpr auto v6 = value_cast<m / s>(v4);
  constexpr auto v7 = value_cast<int>(v6);

  std::cout << v1 << '\n';                                       // 110 km/h
  std::cout << v2 << '\n';                                       // 70 mi/h
  std::cout << STD_FMT::format("{}", v3) << '\n';                // 110 km/h
  std::cout << STD_FMT::format("{:*^14}", v4) << '\n';           // ***70 mi/h****
  std::cout << STD_FMT::format("{:%Q in %q}", v5) << '\n';       // 30.5556 in m/s
  std::cout << STD_FMT::format("{0:%Q} in {0:%q}", v6) << '\n';  // 31.2928 in m/s
  std::cout << STD_FMT::format("{:%Q}", v7) << '\n';             // 31
}
