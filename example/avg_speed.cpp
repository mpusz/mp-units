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

#include <units/isq/space_and_time.h>
#include <units/quantity_io.h>
#include <units/si/cgs/cgs.h>
#include <units/si/international/length.h>
#include <units/si/unit_symbols.h>
#include <exception>
#include <iostream>

namespace {

using namespace units;
using namespace units::si::unit_symbols;

constexpr quantity<isq::speed[m / s], int> fixed_int_si_avg_speed(quantity<isq::length[m], int> d,
                                                                  quantity<isq::time[s], int> t)
{
  return d / t;
}

constexpr quantity<isq::speed[m / s]> fixed_double_si_avg_speed(quantity<isq::length[m]> d, quantity<isq::time[s]> t)
{
  return d / t;
}

constexpr quantity_of<isq::speed> auto avg_speed(quantity_of<isq::length> auto d, quantity_of<isq::time> auto t)
{
  return quantity_cast<isq::speed>(d / t);
}

template<quantity_of<isq::length> D, quantity_of<isq::time> T, quantity_of<isq::speed> V>
void print_result(D distance, T duration, V speed)
{
  const auto result_in_kmph = quantity_cast<km / h>(speed);
  std::cout << "Average speed of a car that makes " << distance << " in " << duration << " is " << result_in_kmph
            << ".\n";
}

void example()
{
  // SI (int)
  {
    constexpr auto distance = 220 * isq::length[km];
    constexpr auto duration = 2 * isq::time[h];

    std::cout << "SI units with 'int' as representation\n";

    print_result(distance, duration, fixed_int_si_avg_speed(distance, duration));
    print_result(distance, duration, fixed_double_si_avg_speed(distance, duration));
    print_result(distance, duration, avg_speed(distance, duration));
  }

  // SI (double)
  {
    constexpr auto distance = 220. * isq::length[km];
    constexpr auto duration = 2. * isq::time[h];

    std::cout << "\nSI units with 'double' as representation\n";

    // conversion from a floating-point to an integral type is a truncating one so an explicit cast is needed
    print_result(distance, duration,
                 fixed_int_si_avg_speed(quantity_cast<int>(distance), quantity_cast<int>(duration)));
    print_result(distance, duration, fixed_double_si_avg_speed(distance, duration));
    print_result(distance, duration, avg_speed(distance, duration));
  }

  // Customary Units (int)
  {
    using namespace units::si::international::unit_symbols;

    constexpr auto distance = 140 * isq::length[mi];
    constexpr auto duration = 2 * isq::time[h];

    std::cout << "\nUS Customary Units with 'int' as representation\n";

    // it is not possible to make a lossless conversion of miles to meters on an integral type
    // (explicit cast needed)
    print_result(distance, duration, fixed_int_si_avg_speed(quantity_cast<si::metre>(distance), duration));
    print_result(distance, duration, fixed_double_si_avg_speed(distance, duration));
    print_result(distance, duration, avg_speed(distance, duration));
  }

  // Customary Units (double)
  {
    using namespace units::si::international::unit_symbols;
    constexpr auto distance = 140. * isq::length[mi];
    constexpr auto duration = 2. * isq::time[h];

    std::cout << "\nUS Customary Units with 'double' as representation\n";

    // conversion from a floating-point to an integral type is a truncating one so an explicit cast is needed
    // also it is not possible to make a lossless conversion of miles to meters on an integral type
    // (explicit cast needed)
    print_result(
      distance, duration,
      fixed_int_si_avg_speed(quantity_cast<quantity<isq::length[m], int>>(distance), quantity_cast<int>(duration)));
    print_result(distance, duration, fixed_double_si_avg_speed(distance, duration));
    print_result(distance, duration, avg_speed(distance, duration));
  }

  // CGS (int)
  {
    constexpr auto distance = 22'000'000 * isq::length[si::cgs::centimetre];
    constexpr auto duration = 7200 * isq::time[si::cgs::second];

    std::cout << "\nCGS units with 'int' as representation\n";

    // it is not possible to make a lossless conversion of centimeters to meters on an integral type
    // (explicit cast needed)
    print_result(distance, duration, fixed_int_si_avg_speed(quantity_cast<si::metre>(distance), duration));
    print_result(distance, duration, fixed_double_si_avg_speed(distance, duration));
    print_result(distance, duration, avg_speed(distance, duration));
  }

  // CGS (double)
  {
    constexpr auto distance = 22'000'000. * isq::length[si::cgs::centimetre];
    constexpr auto duration = 7200. * isq::time[si::cgs::second];

    std::cout << "\nCGS units with 'double' as representation\n";

    // conversion from a floating-point to an integral type is a truncating one so an explicit cast is needed
    // it is not possible to make a lossless conversion of centimeters to meters on an integral type
    // (explicit cast needed)
    print_result(
      distance, duration,
      fixed_int_si_avg_speed(quantity_cast<quantity<isq::length[m], int>>(distance), quantity_cast<int>(duration)));

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