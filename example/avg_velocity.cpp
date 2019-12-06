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

#include <units/physical/si/velocity.h>
#include <iostream>

namespace {

constexpr units::si::velocity<units::si::metre_per_second, int>
fixed_int_si_avg_speed(units::si::length<units::si::metre, int> d,
                       units::si::time<units::si::second, int> t)
{
  return d / t;
}

constexpr units::si::velocity<units::si::metre_per_second>
fixed_double_si_avg_speed(units::si::length<units::si::metre> d,
                          units::si::time<units::si::second> t)
{
  return d / t;
}

template<typename U1, typename R1, typename U2, typename R2>
constexpr units::Velocity AUTO si_avg_speed(units::si::length<U1, R1> d,
                                            units::si::time<U2, R2> t)
{
  return d / t;
}

constexpr units::Velocity AUTO avg_speed(units::Length AUTO d, units::Time AUTO t)
{
  return d / t;
}

template<units::Length D, units::Time T, units::Velocity V>
void print_result(D distance, T duration, V velocity)
{
  const auto result_in_kmph = units::quantity_cast<units::si::kilometre_per_hour>(velocity);
  std::cout << "Average speed of a car that makes " << distance << " in "
            << duration << " is " << result_in_kmph << ".\n";
}

void example()
{
  using namespace units;
  using namespace units::si::literals;

  // SI (int)
  {
    constexpr Length AUTO distance = 220km;        // constructed from a UDL
    constexpr si::time<si::hour, int> duration(2); // constructed from a value

    std::cout << "SI units with 'int' as representation\n";

    print_result(distance, duration, fixed_int_si_avg_speed(distance, duration));
    print_result(distance, duration, fixed_double_si_avg_speed(distance, duration));

    // the framework will not allow a division (and multiplication) of different dimensions
    // with two integral representation (at least one of them have to ba floating-point one)
    print_result(distance, duration, si_avg_speed(quantity_cast<double>(distance), duration));
    print_result(distance, duration, avg_speed(quantity_cast<double>(distance), duration));
  }

  // SI (double)
  {
    constexpr Length AUTO distance = 220.km;  // constructed from a UDL
    constexpr si::time<si::hour> duration(2); // constructed from a value

    std::cout << "\nSI units with 'double' as representation\n";

    // conversion from a floating-point to an integral type is a truncating one so an explicit cast is needed
    print_result(distance, duration, fixed_int_si_avg_speed(quantity_cast<int>(distance), quantity_cast<int>(duration)));

    print_result(distance, duration, fixed_double_si_avg_speed(distance, duration));
    print_result(distance, duration, si_avg_speed(distance, duration));
    print_result(distance, duration, avg_speed(distance, duration));
  }

  // Customary Units (int)
  {
    constexpr Length AUTO distance = 140mi;        // constructed from a UDL
    constexpr si::time<si::hour, int> duration(2); // constructed from a value

    std::cout << "\nUS Customary Units with 'int' as representation\n";

    // it is not possible to make a lossless conversion of miles to meters on an integral type
    // (explicit cast needed)
    print_result(distance, duration, fixed_int_si_avg_speed(quantity_cast<si::metre>(distance), duration));
    print_result(distance, duration, fixed_double_si_avg_speed(distance, duration));

    // the framework will not allow a division (and multiplication) of different dimensions
    // with two integral representation (at least one of them have to ba floating-point one)
    print_result(distance, duration, si_avg_speed(quantity_cast<double>(distance), duration));
    print_result(distance, duration, avg_speed(quantity_cast<double>(distance), duration));
  }

  // Customary Units (double)
  {
    constexpr Length AUTO distance = 140.mi;  // constructed from a UDL
    constexpr si::time<si::hour> duration(2); // constructed from a value

    std::cout << "\nUS Customary Units with 'double' as representation\n";

    // conversion from a floating-point to an integral type is a truncating one so an explicit cast is needed
    // also it is not possible to make a lossless conversion of miles to meters on an integral type
    // (explicit cast needed)
    print_result(distance, duration, fixed_int_si_avg_speed(quantity_cast<si::metre, int>(distance), quantity_cast<int>(duration)));

    print_result(distance, duration, fixed_double_si_avg_speed(distance, duration));
    print_result(distance, duration, si_avg_speed(distance, duration));
    print_result(distance, duration, avg_speed(distance, duration));
  }
}

} // namespace

int main()
{
  try {
    example();
  }
  catch (const std::exception& ex) {
    std::cerr << "Unhandled std exception caught: " << ex.what() << '\n';
  }
  catch (...) {
    std::cerr << "Unhandled unknown exception caught\n";
  }
}
