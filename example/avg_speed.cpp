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

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!! Before you commit any changes to this file please make sure to check if it !!!
// !!! renders correctly in the documentation "Examples" section.                 !!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#include <mp-units/ostream.h>
#include <mp-units/systems/cgs/cgs.h>
#include <mp-units/systems/international/international.h>
#include <mp-units/systems/isq/isq.h>
#include <mp-units/systems/si/si.h>
#include <exception>
#include <iostream>

namespace {

using namespace mp_units;

constexpr quantity<si::metre / si::second, int> fixed_int_si_avg_speed(quantity<si::metre, int> d,
                                                                       quantity<si::second, int> t)
{
  return d / t;
}

constexpr quantity<si::metre / si::second> fixed_double_si_avg_speed(quantity<si::metre> d, quantity<si::second> t)
{
  return d / t;
}

constexpr QuantityOf<isq::speed> auto avg_speed(QuantityOf<isq::length> auto d, QuantityOf<isq::time> auto t)
{
  return d / t;
}

template<QuantityOf<isq::length> D, QuantityOf<isq::time> T, QuantityOf<isq::speed> V>
void print_result(D distance, T duration, V speed)
{
  const auto result_in_kmph = speed.force_in(si::kilo<si::metre> / non_si::hour);
  std::cout << "Average speed of a car that makes " << distance << " in " << duration << " is " << result_in_kmph
            << ".\n";
}

void example()
{
  using namespace mp_units::si::unit_symbols;

  // SI (int)
  {
    constexpr auto distance = 220 * km;
    constexpr auto duration = 2 * h;

    std::cout << "SI units with 'int' as representation\n";

    print_result(distance, duration, fixed_int_si_avg_speed(distance, duration));
    print_result(distance, duration, fixed_double_si_avg_speed(distance, duration));
    print_result(distance, duration, avg_speed(distance, duration));
  }

  // SI (double)
  {
    constexpr auto distance = 220. * km;
    constexpr auto duration = 2. * h;

    std::cout << "\nSI units with 'double' as representation\n";

    // conversion from a floating-point to an integral type is a truncating one so an explicit cast is needed
    print_result(distance, duration, fixed_int_si_avg_speed(value_cast<int>(distance), value_cast<int>(duration)));
    print_result(distance, duration, fixed_double_si_avg_speed(distance, duration));
    print_result(distance, duration, avg_speed(distance, duration));
  }

  // Customary Units (int)
  {
    using namespace mp_units::international::unit_symbols;

    constexpr auto distance = 140 * mi;
    constexpr auto duration = 2 * h;

    std::cout << "\nUS Customary Units with 'int' as representation\n";

    // it is not possible to make a lossless conversion of miles to meters on an integral type
    // (explicit cast needed)
    print_result(distance, duration, fixed_int_si_avg_speed(distance.force_in(m), duration));
    print_result(distance, duration, fixed_double_si_avg_speed(distance, duration));
    print_result(distance, duration, avg_speed(distance, duration));
  }

  // Customary Units (double)
  {
    using namespace mp_units::international::unit_symbols;

    constexpr auto distance = 140. * mi;
    constexpr auto duration = 2. * h;

    std::cout << "\nUS Customary Units with 'double' as representation\n";

    // conversion from a floating-point to an integral type is a truncating one so an explicit cast is needed
    // also it is not possible to make a lossless conversion of miles to meters on an integral type
    // (explicit cast needed)
    print_result(distance, duration,
                 fixed_int_si_avg_speed(value_cast<int>(distance.force_in(m)), value_cast<int>(duration)));
    print_result(distance, duration, fixed_double_si_avg_speed(distance, duration));
    print_result(distance, duration, avg_speed(distance, duration));
  }

  // CGS (int)
  {
    constexpr auto distance = 22'000'000 * cgs::centimetre;
    constexpr auto duration = 7200 * cgs::second;

    std::cout << "\nCGS units with 'int' as representation\n";

    // it is not possible to make a lossless conversion of centimeters to meters on an integral type
    // (explicit cast needed)
    print_result(distance, duration, fixed_int_si_avg_speed(distance.force_in(m), duration));
    print_result(distance, duration, fixed_double_si_avg_speed(distance, duration));
    print_result(distance, duration, avg_speed(distance, duration));
  }

  // CGS (double)
  {
    constexpr auto distance = 22'000'000. * cgs::centimetre;
    constexpr auto duration = 7200. * cgs::second;

    std::cout << "\nCGS units with 'double' as representation\n";

    // conversion from a floating-point to an integral type is a truncating one so an explicit cast is needed
    // it is not possible to make a lossless conversion of centimeters to meters on an integral type
    // (explicit cast needed)
    print_result(distance, duration,
                 fixed_int_si_avg_speed(value_cast<int>(distance.force_in(m)), value_cast<int>(duration)));

    print_result(distance, duration, fixed_double_si_avg_speed(distance, duration));
    print_result(distance, duration, avg_speed(distance, duration));
  }
}

}  // namespace

int main()
{
  try {
    example();
  } catch (const std::exception& ex) {
    std::cerr << "Unhandled std exception caught: " << ex.what() << '\n';
  } catch (...) {
    std::cerr << "Unhandled unknown exception caught\n";
  }
}
