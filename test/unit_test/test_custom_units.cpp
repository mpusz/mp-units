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


/* ************** DERIVED DIMENSIONS THAT INCLUDE UNITS WITH SPECIAL NAMES **************** */

namespace {

  using namespace std::experimental;

  inline constexpr units::base_dimension base_dim_digital_information{"digital information"};

  struct digital_information : units::make_dimension_t<units::exp<base_dim_digital_information, 1>> {};

  template<typename T>
  concept bool DigitalInformation = units::QuantityOf<T, digital_information>;

  struct bit : units::unit<digital_information> {};
  struct byte : units::unit<digital_information, units::ratio<8>> {};

  inline namespace literals {

    constexpr auto operator""_b(unsigned long long l) { return units::quantity<bit, std::int64_t>(l); }
    constexpr auto operator""_b(long double l) { return units::quantity<bit, long double>(l); }

    constexpr auto operator""_B(unsigned long long l) { return units::quantity<byte, std::int64_t>(l); }
    constexpr auto operator""_B(long double l) { return units::quantity<byte, long double>(l); }

  }
}

namespace stde = std::experimental;

template<> struct stde::units::downcasting_traits<stde::units::downcast_from<digital_information>> : stde::units::downcast_to<digital_information> {};
template<> struct stde::units::downcasting_traits<stde::units::downcast_from<bit>> : stde::units::downcast_to<bit> {};
template<> struct stde::units::downcasting_traits<stde::units::downcast_from<::byte>> : stde::units::downcast_to<::byte> {};

namespace {

  static_assert(1_B == 8_b);

}

namespace {

  using namespace stde::units;

  // power spectral density
  // todo: add support for make_dimension_t of non base units
  //  struct power_spectral_density : make_dimension_t<exp<voltage, 2>, exp<frequency, -1>> {};
  struct power_spectral_density
      : make_dimension_t<exp<base_dim_mass, 2>, exp<base_dim_length, 4>, exp<base_dim_time, -6>, exp<base_dim_time, -1>> {
  };
  struct sq_volt_per_hertz : unit<power_spectral_density> {};

  // amplitude spectral density
  //  struct dimension_amplitude_spectral_density : make_dimension_t<exp<voltage, 1>, exp<frequency, -1, 2>> {};
  struct amplitude_spectral_density
      : make_dimension_t<exp<base_dim_mass, 1>, exp<base_dim_length, 2>, exp<base_dim_time, -3>, exp<base_dim_current, -1>, exp<base_dim_time, 1, 2>> {
  };
  // todo: add support for derived_unit
  //struct volt_per_sq_hertz : derived_unit<amplitude_spectral_density, kilogram, metre, second, ampere > {};
  struct volt_per_sqrt_hertz : unit<amplitude_spectral_density> {};
}

namespace std::experimental::units {

template<> struct downcasting_traits<downcast_from<power_spectral_density>> : downcast_to<power_spectral_density> {};
template<> struct downcasting_traits<downcast_from<sq_volt_per_hertz>> : downcast_to<sq_volt_per_hertz> {};

template<> struct downcasting_traits<downcast_from<amplitude_spectral_density>> : downcast_to<amplitude_spectral_density> {};
template<> struct downcasting_traits<downcast_from<volt_per_sqrt_hertz>> : downcast_to<volt_per_sqrt_hertz> {};

}

namespace {

  //static_assert(sqrt(quantity<sq_volt_per_hertz>(4)) = quantity<volt_per_sqrt_hertz>(2));

}
