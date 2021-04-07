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

#include <units/isq/si/cgs/length.h>
#include <units/isq/si/cgs/speed.h> // IWYU pragma: keep
#include <units/isq/si/international/length.h>
#include <units/isq/si/international/speed.h> // IWYU pragma: keep
#include <units/isq/si/length.h> // IWYU pragma: keep
#include <units/isq/si/time.h>
#include <units/isq/si/speed.h>
#include <units/quantity_io.h>
#include <exception>
#include <iostream>

namespace {

using namespace units::isq;

constexpr si::speed<si::metre_per_second, int>
fixed_int_si_avg_speed(si::length<si::metre, int> d,
                       si::time<si::second, int> t)
{
  return d / t;
}

constexpr si::speed<si::metre_per_second>
fixed_double_si_avg_speed(si::length<si::metre> d,
                          si::time<si::second> t)
{
  return d / t;
}

template<typename U1, typename R1, typename U2, typename R2>
constexpr Speed auto si_avg_speed(si::length<U1, R1> d,
                                  si::time<U2, R2> t)
{
  return d / t;
}

constexpr Speed auto avg_speed(Length auto d, Time auto t)
{
  return d / t;
}

template<Length D, Time T, Speed V>
void print_result(D distance, T duration, V speed)
{
  const auto result_in_kmph = units::quantity_cast<si::speed<si::kilometre_per_hour>>(speed);
  std::cout << "Average speed of a car that makes " << distance << " in "
            << duration << " is " << result_in_kmph << ".\n";
}

void example()
{
  // SI (int)
  {
    using namespace units::isq::si::literals;
    constexpr auto distance = 220_q_km;
    constexpr auto duration = 2_q_h;

    std::cout << "SI units with 'int' as representation\n";

    print_result(distance, duration, fixed_int_si_avg_speed(distance, duration));
    print_result(distance, duration, fixed_double_si_avg_speed(distance, duration));
    print_result(distance, duration, si_avg_speed(distance, duration));
    print_result(distance, duration, avg_speed(distance, duration));
  }

  // SI (double)
  {
    using namespace units::isq::si::literals;
    constexpr auto distance = 220._q_km;
    constexpr auto duration = 2._q_h;

    std::cout << "\nSI units with 'double' as representation\n";

    // conversion from a floating-point to an integral type is a truncating one so an explicit cast is needed
    print_result(distance, duration, fixed_int_si_avg_speed(quantity_cast<int>(distance), quantity_cast<int>(duration)));

    print_result(distance, duration, fixed_double_si_avg_speed(distance, duration));
    print_result(distance, duration, si_avg_speed(distance, duration));
    print_result(distance, duration, avg_speed(distance, duration));
  }

  // Customary Units (int)
  {
    using namespace units::isq::si::international::literals;
    using namespace units::isq::si::literals;
    constexpr auto distance = 140_q_mi;
    constexpr auto duration = 2_q_h;

    std::cout << "\nUS Customary Units with 'int' as representation\n";

    // it is not possible to make a lossless conversion of miles to meters on an integral type
    // (explicit cast needed)
    print_result(distance, duration, fixed_int_si_avg_speed(quantity_cast<si::metre>(distance), duration));
    print_result(distance, duration, fixed_double_si_avg_speed(distance, duration));
    print_result(distance, duration, si_avg_speed(distance, duration));
    print_result(distance, duration, avg_speed(distance, duration));
  }

  // Customary Units (double)
  {
    using namespace units::isq::si::international::literals;
    using namespace units::isq::si::literals;
    constexpr auto distance = 140._q_mi;
    constexpr auto duration = 2._q_h;

    std::cout << "\nUS Customary Units with 'double' as representation\n";

    // conversion from a floating-point to an integral type is a truncating one so an explicit cast is needed
    // also it is not possible to make a lossless conversion of miles to meters on an integral type
    // (explicit cast needed)
    print_result(distance, duration, fixed_int_si_avg_speed(quantity_cast<si::length<si::metre, int>>(distance), quantity_cast<int>(duration)));

    print_result(distance, duration, fixed_double_si_avg_speed(distance, duration));
    print_result(distance, duration, si_avg_speed(distance, duration));
    print_result(distance, duration, avg_speed(distance, duration));
  }

  // CGS (int)
  {
    using namespace units::isq::si::cgs::literals;
    constexpr auto distance = 22'000'000_q_cm;
    constexpr si::cgs::time<si::hour, int> duration(2);   // cannot use an SI literal here

    std::cout << "\nCGS units with 'int' as representation\n";

    // it is not possible to make a lossless conversion of centimeters to meters on an integral type
    // (explicit cast needed)
    print_result(distance, duration, fixed_int_si_avg_speed(quantity_cast<si::metre>(distance), duration));
    print_result(distance, duration, fixed_double_si_avg_speed(distance, duration));

    // not possible to convert both a dimension and a unit with implicit cast
    print_result(distance, duration, si_avg_speed(quantity_cast<si::dim_length>(distance), duration));

    print_result(distance, duration, avg_speed(distance, duration));
  }

  // CGS (double)
  {
    using namespace units::isq::si::cgs::literals;
    constexpr auto distance = 22'000'000._q_cm;
    constexpr si::cgs::time<si::hour, double> duration(2);   // cannot use an SI literal here

    std::cout << "\nCGS units with 'double' as representation\n";

    // conversion from a floating-point to an integral type is a truncating one so an explicit cast is needed
    // it is not possible to make a lossless conversion of centimeters to meters on an integral type
    // (explicit cast needed)
    print_result(distance, duration, fixed_int_si_avg_speed(quantity_cast<si::length<si::metre, int>>(distance), quantity_cast<int>(duration)));

    print_result(distance, duration, fixed_double_si_avg_speed(distance, duration));

    // not possible to convert both a dimension and a unit with implicit cast
    print_result(distance, duration, si_avg_speed(quantity_cast<si::dim_length>(distance), duration));

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
