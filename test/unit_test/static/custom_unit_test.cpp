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

#include <units/dimensions/voltage.h>
#include <units/dimensions/frequency.h>
#include <units/math.h>

/* ************** DERIVED DIMENSIONS THAT INCLUDE UNITS WITH SPECIAL NAMES **************** */

namespace {

  struct base_dim_digital_information : units::base_dimension<"digital information", "b"> {};

  struct digital_information : units::derived_dimension<digital_information, units::exp<base_dim_digital_information, 1>> {};

  template<typename T>
  concept DigitalInformation = units::QuantityOf<T, digital_information>;

  struct data_prefix {};

  struct kibi : units::prefix<kibi, data_prefix, units::ratio<    1'024>, "Ki"> {};

  struct bit : units::named_coherent_derived_unit<bit, "b", digital_information, data_prefix> {};
  struct kilobit : units::prefixed_derived_unit<kilobit, kibi, bit> {};

  struct byte : units::named_derived_unit<byte, "B", digital_information, units::ratio<8>> {};
  struct kilobyte : units::prefixed_derived_unit<kilobyte, kibi, byte> {};

  inline namespace literals {

    constexpr auto operator""_b(unsigned long long l) { return units::quantity<bit, std::int64_t>(l); }
    constexpr auto operator""_Kib(unsigned long long l) { return units::quantity<kilobit, std::int64_t>(l); }
    constexpr auto operator""_B(unsigned long long l) { return units::quantity<byte, std::int64_t>(l); }
    constexpr auto operator""_KiB(unsigned long long l) { return units::quantity<kilobyte, std::int64_t>(l); }

  }
}

namespace {

  static_assert(1_B == 8_b);
  static_assert(1024_b == 1_Kib);
  static_assert(1024_B == 1_KiB);
  static_assert(8 * 1024_b == 1_KiB);
  static_assert(8 * 1_Kib == 1_KiB);

}

namespace {

  using namespace units;

  // power spectral density
  struct power_spectral_density : derived_dimension<power_spectral_density, units::exp<voltage, 2>, units::exp<frequency, -1>> {};
  struct sq_volt_per_hertz : coherent_derived_unit<sq_volt_per_hertz, power_spectral_density> {};

  // amplitude spectral density
  struct amplitude_spectral_density : derived_dimension<amplitude_spectral_density, units::exp<voltage, 1>, units::exp<frequency, -1, 2>> {};
  struct volt_per_sqrt_hertz : coherent_derived_unit<volt_per_sqrt_hertz, amplitude_spectral_density> {};
}

namespace {

  static_assert(std::is_same_v<dimension_sqrt<power_spectral_density>, amplitude_spectral_density>);
  static_assert(std::is_same_v<dimension_pow<amplitude_spectral_density, 2>, power_spectral_density>);

  static_assert(std::is_same_v<decltype(pow<2>(quantity<volt_per_sqrt_hertz>(4))), decltype(quantity<sq_volt_per_hertz>(16))>);
  static_assert(std::is_same_v<decltype(sqrt(quantity<sq_volt_per_hertz>(16))), decltype(quantity<volt_per_sqrt_hertz>(4))>);

}
