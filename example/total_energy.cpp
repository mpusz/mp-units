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

#include <units/isq/mechanics.h>
#include <units/math.h>
#include <units/natural/natural.h>
#include <units/quantity_io.h>
#include <units/si/constants.h>
#include <units/si/unit_symbols.h>
#include <exception>
#include <iostream>

template<class T>
  requires units::is_scalar<T>
inline constexpr bool units::is_vector<T> = true;

namespace {

using namespace units;

quantity_of<isq::mechanical_energy> auto total_energy(weak_quantity_of<isq::momentum> auto p,
                                                      weak_quantity_of<isq::mass> auto m,
                                                      weak_quantity_of<isq::speed> auto c)
{
  return quantity_cast<isq::mechanical_energy>(sqrt(pow<2>(p * c) + pow<2>(m * pow<2>(c))));
}

void si_example()
{
  using namespace units::si::unit_symbols;
  constexpr auto GeV = si::giga<si::electronvolt>;

  constexpr quantity_of<isq::speed> auto c = 1. * si::si2019::speed_of_light_in_vacuum;
  const weak_quantity_of<isq::momentum> auto p1 = 4. * isq::mechanical_energy[GeV] / c;
  const weak_quantity_of<isq::mass> auto m1 = 3. * isq::mechanical_energy[GeV] / pow<2>(c);
  const auto E = total_energy(p1, m1, c);

  std::cout << "\n*** SI units (c = " << c << " = " << c[si::metre / s] << ") ***\n";

  std::cout << "\n[in `GeV` and `c`]\n"
            << "p = " << p1 << "\n"
            << "m = " << m1 << "\n"
            << "E = " << E << "\n";

  const auto p2 = p1[GeV / (si::metre / s)];
  const auto m2 = m1[si::giga<si::electronvolt> / pow<2>(si::metre / s)];
  const auto E2 = total_energy(p2, m2, c)[GeV];

  std::cout << "\n[in `GeV`]\n"
            << "p = " << p2 << "\n"
            << "m = " << m2 << "\n"
            << "E = " << E2 << "\n";

  const auto p3 = p1[kg * si::metre / s];
  const auto m3 = m1[kg];
  const auto E3 = total_energy(p3, m3, c)[J];

  std::cout << "\n[in SI units]\n"
            << "p = " << p3 << "\n"
            << "m = " << m3 << "\n"
            << "E = " << E3 << "\n";

  std::cout << "\n[converted from SI units back to GeV]\n"
            << "E = " << quantity_cast<GeV>(E3) << "\n";
}

void natural_example()
{
  using namespace units::natural;
  using namespace units::natural::unit_symbols;

  constexpr quantity_of<isq::speed> auto c = 1. * speed_of_light_in_vacuum;
  const quantity_of<isq::momentum> auto p = 4. * momentum[GeV];
  const quantity_of<isq::mass> auto m = 3. * mass[GeV];
  const auto E = total_energy(p, m, c);

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