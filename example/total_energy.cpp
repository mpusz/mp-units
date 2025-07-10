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

#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <exception>
#include <iostream>
#endif
#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/math.h>
#include <mp-units/systems/isq/mechanics.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/natural.h>
#include <mp-units/systems/si.h>
#endif

namespace {

using namespace mp_units;

QuantityOf<isq::mechanical_energy> auto total_energy(QuantityOf<isq::momentum> auto p, QuantityOf<isq::mass> auto m,
                                                     QuantityOf<isq::speed> auto c)
{
  return isq::mechanical_energy(sqrt(pow<2>(p * c) + pow<2>(m * pow<2>(c))));
}

void si_example()
{
  using namespace mp_units::si::unit_symbols;
  constexpr Unit auto GeV = si::giga<si::electronvolt>;
  constexpr quantity c = 1. * si::si2019::speed_of_light_in_vacuum;
  const quantity c2 = pow<2>(c);

  const quantity p1 = isq::momentum(4. * GeV / c);
  const QuantityOf<isq::mass> auto m1 = 3. * GeV / c2;
  const quantity E = total_energy(p1, m1, c);

  std::cout << "\n*** SI units (c = " << c << " = " << c.in(si::metre / s) << ") ***\n";

  std::cout << "\n[in `GeV` and `c`]\n"
            << "p = " << p1 << "\n"
            << "m = " << m1 << "\n"
            << "E = " << E << "\n";

  const quantity p2 = p1.in(GeV / (m / s));
  const quantity m2 = m1.in(GeV / pow<2>(m / s));
  const quantity E2 = total_energy(p2, m2, c).in(GeV);

  std::cout << "\n[in `GeV`]\n"
            << "p = " << p2 << "\n"
            << "m = " << m2 << "\n"
            << "E = " << E2 << "\n";

  const quantity p3 = p1.in(kg * m / s);
  const quantity m3 = m1.in(kg);
  const quantity E3 = total_energy(p3, m3, c).in(J);

  std::cout << "\n[in SI base units]\n"
            << "p = " << p3 << "\n"
            << "m = " << m3 << "\n"
            << "E = " << E3 << "\n";

  std::cout << "\n[converted from SI units back to GeV]\n"
            << "E = " << E3.force_in(GeV) << "\n";
}

void natural_example()
{
  using namespace mp_units::natural;
  using namespace mp_units::natural::unit_symbols;

  constexpr quantity c = 1. * speed_of_light;
  const quantity p = 4. * momentum[GeV];
  const quantity m = 3. * mass[GeV];
  const quantity E = total_energy(p, m, c);

  std::cout << "\n*** Natural units (c = " << c << ") ***\n"
            << "p = " << p << "\n"
            << "m = " << m << "\n"
            << "E = " << E << "\n";
}

}  // namespace

int main()
{
  try {
    si_example();
    natural_example();
  } catch (const std::exception& ex) {
    std::cerr << "Unhandled std exception caught: " << ex.what() << '\n';
  } catch (...) {
    std::cerr << "Unhandled unknown exception caught\n";
  }
}
