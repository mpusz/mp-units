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

#include <units/physical/natural/constants.h>
#include <units/physical/si/energy.h>
#include <units/physical/si/momentum.h>
#include <units/physical/si/speed.h>
#include <units/physical/si/constants.h>
#include <units/math.h>
#include <iostream>

namespace {

using namespace units::physical;

in_energy AUTO total_energy(in_momentum AUTO p, in_mass AUTO m, in_speed AUTO c)
{
  return sqrt(pow<2>(p * c) + pow<2>(m * pow<2>(c)));
}

void si_example()
{
  using namespace units::physical::si;
  using GeV = gigaelectronvolt;

  constexpr in_speed AUTO c = si2019::speed_of_light<>;

  std::cout << "\n*** SI units (c = " << c << ") ***\n";

  const in_momentum AUTO p = 4.q_GeV / c;
  const in_mass AUTO m = 3.q_GeV / pow<2>(c);
  const in_energy AUTO E = total_energy(p, m, c);

  std::cout << "[in GeV]\n"
            << "p = " << p << "\n"
            << "m = " << m << "\n"
            << "E = " << E << "\n";

  const momentum<kilogram_metre_per_second> p_si = p;
  const mass<kilogram> m_si = m;
  const energy<joule> E_si = total_energy(p_si, m_si, c);

  std::cout << "\n[in SI units]\n"
            << "p = " << p_si << "\n"
            << "m = " << m_si << "\n"
            << "E = " << E_si << "\n";

  std::cout << "\n[converted from SI units back to GeV]\n"
            << "E = " << quantity_cast<GeV>(E_si) << "\n";
}

void natural_example()
{
  using namespace units::physical::natural;
  using GeV = gigaelectronvolt;

  constexpr in_speed AUTO c = speed_of_light<>;
  const momentum<GeV> p(4);
  const mass<GeV> m(3);
  const in_energy AUTO E = total_energy(p, m, c);

  std::cout << "\n*** Natural units (c = " << c << ") ***\n"
            << "p = " << p << "\n"
            << "m = " << m << "\n"
            << "E = " << E << "\n";
}

} // namespace

int main()
{
  try {
    si_example();
    natural_example();
  }
  catch (const std::exception& ex) {
    std::cerr << "Unhandled std exception caught: " << ex.what() << '\n';
  }
  catch (...) {
    std::cerr << "Unhandled unknown exception caught\n";
  }
}
