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

#include <units/quantity.h>
#include <catch2/catch.hpp>
#include <sstream>

namespace data {

  struct base_dim_digital_information : units::base_dimension<"digital information", "b"> {};

  struct digital_information : units::derived_dimension<digital_information, units::exp<base_dim_digital_information, 1>> {};

  template<typename T>
  concept DigitalInformation = units::QuantityOf<T, digital_information>;

  struct data_prefix;

  struct bit : units::coherent_derived_unit<bit, "b", digital_information, data_prefix> {};
  struct kilobit : units::derived_unit<bit, "Kib", digital_information, units::ratio<1'024>> {};
  struct byte : units::derived_unit<byte, "B", digital_information, units::ratio<8>> {};

  inline namespace literals {

    constexpr auto operator""_b(unsigned long long l) { return units::quantity<bit, std::int64_t>(l); }
    constexpr auto operator""_Kib(unsigned long long l) { return units::quantity<kilobit, std::int64_t>(l); }
    constexpr auto operator""_B(unsigned long long l) { return units::quantity<byte, std::int64_t>(l); }

  }

}

template<> inline constexpr std::string_view units::prefix_symbol<data::data_prefix, units::ratio<    1'024>> = "Ki";
template<> inline constexpr std::string_view units::prefix_symbol<data::data_prefix, units::ratio<1'048'576>> = "Mi";

using namespace data;

TEST_CASE("operator<< on a custom quantity", "[text][ostream]")
{
  std::stringstream stream;

  SECTION("quantity with a predefined unit and prefix")
  {
    SECTION("named unit")
    {
      stream << 64_B;
      REQUIRE(stream.str() == "64 B");
    }

    SECTION("other unit matching prefix")
    {
      stream << 8_Kib * 8_Kib / 2_b;
      REQUIRE(stream.str() == "32 Mib");
    }
  }
}
