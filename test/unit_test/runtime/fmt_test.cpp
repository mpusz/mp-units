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

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <units/cgs/cgs.h>
#include <units/customization_points.h>
#include <units/format.h>
#include <units/isq/mechanics.h>
#include <units/isq/space_and_time.h>
#include <units/quantity_io.h>
#include <units/si/si.h>
#include <iomanip>
#include <limits>
#include <locale>

template<class T>
  requires units::is_scalar<T>
inline constexpr bool units::is_vector<T> = true;

using namespace units;
using namespace units::si::unit_symbols;

TEST_CASE("operator<< on a quantity", "[text][ostream][fmt]")
{
  std::ostringstream os;

  SECTION("quantity with a predefined unit")
  {
    SECTION("integral representation")
    {
      const auto q = 60 * isq::power[W];
      os << q;

      SECTION("iostream") { CHECK(os.str() == "60 W"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
    }

    SECTION("floating-point representation")
    {
      const auto q = 1023.5 * isq::pressure[Pa];
      os << q;

      SECTION("iostream") { CHECK(os.str() == "1023.5 Pa"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
    }
  }

  SECTION("quantity with a predefined prefixed unit")
  {
    const auto q = 125 * isq::time[us];
    os << q;

    SECTION("iostream") { CHECK(os.str() == "125 µs"); }

    SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

    SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
  }

  SECTION("quantity with a derived unit")
  {
    SECTION("coherent derived unit")
    {
      SECTION("acceleration")
      {
        const auto q = 20 * isq::length[m] / (2 * isq::time[s]) / (1 * isq::time[s]);
        os << q;

        SECTION("iostream") { CHECK(os.str() == "10 m/s²"); }

        SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

        SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
      }

      SECTION("volume")
      {
        const auto q = 2 * isq::length[m] * (1 * isq::length[m]) * (1 * isq::length[m]);
        os << q;

        SECTION("iostream") { CHECK(os.str() == "2 m³"); }

        SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

        SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
      }

      SECTION("surface tension")
      {
        const auto q = 20 * isq::force[N] / (2 * isq::length[m]);
        os << q;

        SECTION("iostream") { CHECK(os.str() == "10 N/m"); }

        SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

        SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
      }
    }

    SECTION("scaled derived unit")
    {
      SECTION("speed")
      {
        const auto q = 20 * isq::length[km] / (2 * isq::time[h]);
        os << q;

        SECTION("iostream") { CHECK(os.str() == "10 km/h"); }

        SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

        SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
      }

      SECTION("angular impulse")
      {
        const auto q = 123 * isq::angular_impulse[N * m * s];
        os << q;

        SECTION("iostream") { CHECK(os.str() == "123 m N s"); }

        SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

        SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
      }

      SECTION("compressibility")
      {
        const auto q = 123 * isq::compressibility[1 / Pa];
        os << q;

        SECTION("iostream") { CHECK(os.str() == "123 1/Pa"); }

        SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

        SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
      }

      SECTION("angular acceleration")
      {
        const auto q = 123 * isq::angular_acceleration[rad / s2];
        os << q;

        SECTION("iostream") { CHECK(os.str() == "123 1/s²"); }

        SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

        SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
      }
    }
  }


  SECTION("dimensionless quantity")
  {
    SECTION("one with ratio == 1")
    {
      const auto q = 4 * isq::length[m] / (2 * isq::length[m]);
      os << q;

      SECTION("iostream") { CHECK(os.str() == "2"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == "2 "); }
    }

    SECTION("one with ratio.exp != 0")
    {
      const auto q = 4 * isq::length[km] / (2 * isq::length[m]);
      os << q;

      SECTION("iostream") { CHECK(os.str() == "2 km/m"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == "2 km/m"); }
    }

    SECTION("percents")
    {
      const auto q = quantity_cast<percent>(15. * isq::length[m] / (100. * isq::length[m]));
      os << q;

      SECTION("iostream") { CHECK(os.str() == "15 %"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
    }
  }
}

TEST_CASE("format string with only %Q should print quantity value only", "[text][fmt]")
{
  SECTION("integral representation")
  {
    SECTION("positive value") { CHECK(STD_FMT::format("{:%Q}", 123 * isq::speed[km / h]) == "123"); }

    SECTION("negative value") { CHECK(STD_FMT::format("{:%Q}", 5 * isq::length[m] - 10 * isq::length[m]) == "-5"); }
  }

  SECTION("floating-point representation")
  {
    SECTION("positive value")
    {
      CHECK(STD_FMT::format("{:%Q}", 221. * isq::length[km] / (2 * isq::time[h])) == "110.5");
    }

    SECTION("negative value")
    {
      CHECK(STD_FMT::format("{:%Q}", 3.14 * isq::length[m] - 10 * isq::length[m]) == "-6.86");
    }

    SECTION("nan")
    {
      CHECK(STD_FMT::format("{:%Q}", std::numeric_limits<double>::quiet_NaN() * isq::length[m]) == "nan");
    }

    SECTION("inf")
    {
      CHECK(STD_FMT::format("{:%Q}", std::numeric_limits<double>::infinity() * isq::length[m]) == "inf");
    }

    SECTION("-inf")
    {
      CHECK(STD_FMT::format("{:%Q}", -std::numeric_limits<double>::infinity() * isq::length[m]) == "-inf");
    }
  }
}

TEST_CASE("format string with only %q should print quantity unit symbol only", "[text][fmt]")
{
  SECTION("Text encoding")
  {
    SECTION("Unicode text output")
    {
      CHECK(STD_FMT::format("{:%Uq}", 123 * isq::speed[km / h]) == "km/h");
      // TODO enable this when resistance is defined
      // CHECK(STD_FMT::format("{:%Uq}", 123 * isq::resistance[kilo<ohm>]) == "kΩ");
      CHECK(STD_FMT::format("{:%Uq}", 123 * isq::time[us]) == "µs");
      CHECK(STD_FMT::format("{:%Uq}", 123 * isq::acceleration[m / s2]) == "m/s²");
    }

    SECTION("Unicode text output is used by default")
    {
      CHECK(STD_FMT::format("{:%q}", 123 * isq::speed[km / h]) == "km/h");
      // CHECK(STD_FMT::format("{:%q}", 123 * isq::resistance[kilo<ohm>]) == "kΩ");
      CHECK(STD_FMT::format("{:%q}", 123 * isq::time[us]) == "µs");
      CHECK(STD_FMT::format("{:%q}", 123 * isq::acceleration[m / s2]) == "m/s²");
    }

    SECTION("ASCII text output")
    {
      CHECK(STD_FMT::format("{:%Aq}", 123 * isq::speed[km / h]) == "km/h");
      // CHECK(STD_FMT::format("{:%Aq}", 123 * isq::resistance[kilo<ohm>]) == "kohm");
      CHECK(STD_FMT::format("{:%Aq}", 123 * isq::time[us]) == "us");
      CHECK(STD_FMT::format("{:%Aq}", 123 * isq::acceleration[m / s2]) == "m/s^2");
    }
  }

  SECTION("Solidus")
  {
    SECTION("Solidus for only one element in denominator")
    {
      CHECK(STD_FMT::format("{:%oq}", 123 * isq::speed[km / h]) == "km/h");
      CHECK(STD_FMT::format("{:%oq}", 123 * isq::acceleration[m / s2]) == "m/s²");
      CHECK(STD_FMT::format("{:%oq}", 123 * isq::pressure[kg / m / s2]) == "kg m⁻¹ s⁻²");
    }

    SECTION("Solidus for only one element in denominator is used by default")
    {
      CHECK(STD_FMT::format("{:%q}", 123 * isq::speed[km / h]) == "km/h");
      CHECK(STD_FMT::format("{:%q}", 123 * isq::acceleration[m / s2]) == "m/s²");
      CHECK(STD_FMT::format("{:%q}", 123 * isq::pressure[kg / m / s2]) == "kg m⁻¹ s⁻²");
    }

    SECTION("Always use solidus")
    {
      CHECK(STD_FMT::format("{:%aq}", 123 * isq::speed[km / h]) == "km/h");
      CHECK(STD_FMT::format("{:%aq}", 123 * isq::acceleration[m / s2]) == "m/s²");
      CHECK(STD_FMT::format("{:%aq}", 123 * isq::pressure[kg / m / s2]) == "kg/(m s²)");
    }

    SECTION("Never use solidus")
    {
      CHECK(STD_FMT::format("{:%nq}", 123 * isq::speed[km / h]) == "km h⁻¹");
      CHECK(STD_FMT::format("{:%nq}", 123 * isq::acceleration[m / s2]) == "m s⁻²");
      CHECK(STD_FMT::format("{:%nq}", 123 * isq::pressure[kg / m / s2]) == "kg m⁻¹ s⁻²");
    }
  }

  SECTION("Separator")
  {
    SECTION("Space")
    {
      CHECK(STD_FMT::format("{:%sq}", 123 * isq::force[kg * m / s2]) == "kg m/s²");
      CHECK(STD_FMT::format("{:%sq}", 123 * isq::pressure[kg / m / s2]) == "kg m⁻¹ s⁻²");
      CHECK(STD_FMT::format("{:%asq}", 123 * isq::pressure[kg / m / s2]) == "kg/(m s²)");
    }

    SECTION("Space is used by default")
    {
      CHECK(STD_FMT::format("{:%q}", 123 * isq::force[kg * m / s2]) == "kg m/s²");
      CHECK(STD_FMT::format("{:%q}", 123 * isq::pressure[kg / m / s2]) == "kg m⁻¹ s⁻²");
      CHECK(STD_FMT::format("{:%aq}", 123 * isq::pressure[kg / m / s2]) == "kg/(m s²)");
    }

    SECTION("Dot")
    {
      CHECK(STD_FMT::format("{:%dq}", 123 * isq::force[kg * m / s2]) == "kg⋅m/s²");
      CHECK(STD_FMT::format("{:%dq}", 123 * isq::pressure[kg / m / s2]) == "kg⋅m⁻¹⋅s⁻²");
      CHECK(STD_FMT::format("{:%adq}", 123 * isq::pressure[kg / m / s2]) == "kg/(m⋅s²)");
    }
  }
}

TEST_CASE("unknown unit modifiers should throw", "[text][fmt][exception]")
{
  SECTION("only the invalid modifier")
  {
    REQUIRE_THROWS_MATCHES(STD_FMT::vformat("{:%xq}", STD_FMT::make_format_args(1 * isq::length[m])),
                           STD_FMT::format_error, Catch::Matchers::Message("invalid unit modifier specified"));
  }

  SECTION("invalid modifier in the front")
  {
    REQUIRE_THROWS_MATCHES(STD_FMT::vformat("{:%xUdaq}", STD_FMT::make_format_args(1 * isq::length[m])),
                           STD_FMT::format_error, Catch::Matchers::Message("invalid unit modifier specified"));
  }

  SECTION("invalid modifier in the end")
  {
    REQUIRE_THROWS_MATCHES(STD_FMT::vformat("{:%Udaxq}", STD_FMT::make_format_args(1 * isq::length[m])),
                           STD_FMT::format_error, Catch::Matchers::Message("invalid unit modifier specified"));
  }

  SECTION("invalid modifier in the middle")
  {
    REQUIRE_THROWS_MATCHES(STD_FMT::vformat("{:%Udxaq}", STD_FMT::make_format_args(1 * isq::length[m])),
                           STD_FMT::format_error, Catch::Matchers::Message("invalid unit modifier specified"));
  }
}

TEST_CASE("repeated unit modifiers should throw", "[text][fmt][exception]")
{
  SECTION("text encoding")
  {
    REQUIRE_THROWS_MATCHES(STD_FMT::vformat("{:%UdaUq}", STD_FMT::make_format_args(1 * isq::length[m])),
                           STD_FMT::format_error,
                           Catch::Matchers::Message("only one of 'UA' unit modifiers may be used in the format spec"));
    REQUIRE_THROWS_MATCHES(STD_FMT::vformat("{:%dUaUq}", STD_FMT::make_format_args(1 * isq::length[m])),
                           STD_FMT::format_error,
                           Catch::Matchers::Message("only one of 'UA' unit modifiers may be used in the format spec"));
    REQUIRE_THROWS_MATCHES(STD_FMT::vformat("{:%dUUaq}", STD_FMT::make_format_args(1 * isq::length[m])),
                           STD_FMT::format_error,
                           Catch::Matchers::Message("only one of 'UA' unit modifiers may be used in the format spec"));
  }

  SECTION("solidus")
  {
    REQUIRE_THROWS_MATCHES(STD_FMT::vformat("{:%aUdaq}", STD_FMT::make_format_args(1 * isq::length[m])),
                           STD_FMT::format_error,
                           Catch::Matchers::Message("only one of 'oan' unit modifiers may be used in the format spec"));
    REQUIRE_THROWS_MATCHES(STD_FMT::vformat("{:%daUaq}", STD_FMT::make_format_args(1 * isq::length[m])),
                           STD_FMT::format_error,
                           Catch::Matchers::Message("only one of 'oan' unit modifiers may be used in the format spec"));
    REQUIRE_THROWS_MATCHES(STD_FMT::vformat("{:%daaUq}", STD_FMT::make_format_args(1 * isq::length[m])),
                           STD_FMT::format_error,
                           Catch::Matchers::Message("only one of 'oan' unit modifiers may be used in the format spec"));
  }

  SECTION("separator")
  {
    REQUIRE_THROWS_MATCHES(STD_FMT::vformat("{:%dUadq}", STD_FMT::make_format_args(1 * isq::length[m])),
                           STD_FMT::format_error,
                           Catch::Matchers::Message("only one of 'sd' unit modifiers may be used in the format spec"));
    REQUIRE_THROWS_MATCHES(STD_FMT::vformat("{:%dadUq}", STD_FMT::make_format_args(1 * isq::length[m])),
                           STD_FMT::format_error,
                           Catch::Matchers::Message("only one of 'sd' unit modifiers may be used in the format spec"));
    REQUIRE_THROWS_MATCHES(STD_FMT::vformat("{:%addUq}", STD_FMT::make_format_args(1 * isq::length[m])),
                           STD_FMT::format_error,
                           Catch::Matchers::Message("only one of 'sd' unit modifiers may be used in the format spec"));
  }
}

TEST_CASE("more then one modifier of the same kind should throw", "[text][fmt][exception]")
{
  SECTION("text encoding")
  {
    REQUIRE_THROWS_MATCHES(STD_FMT::vformat("{:%UdaAq}", STD_FMT::make_format_args(1 * isq::length[m])),
                           STD_FMT::format_error,
                           Catch::Matchers::Message("only one of 'UA' unit modifiers may be used in the format spec"));
    REQUIRE_THROWS_MATCHES(STD_FMT::vformat("{:%dAaUq}", STD_FMT::make_format_args(1 * isq::length[m])),
                           STD_FMT::format_error,
                           Catch::Matchers::Message("only one of 'UA' unit modifiers may be used in the format spec"));
    REQUIRE_THROWS_MATCHES(STD_FMT::vformat("{:%dAUaq}", STD_FMT::make_format_args(1 * isq::length[m])),
                           STD_FMT::format_error,
                           Catch::Matchers::Message("only one of 'UA' unit modifiers may be used in the format spec"));
  }

  SECTION("solidus")
  {
    REQUIRE_THROWS_MATCHES(STD_FMT::vformat("{:%aUdnq}", STD_FMT::make_format_args(1 * isq::length[m])),
                           STD_FMT::format_error,
                           Catch::Matchers::Message("only one of 'oan' unit modifiers may be used in the format spec"));
    REQUIRE_THROWS_MATCHES(STD_FMT::vformat("{:%dnUaq}", STD_FMT::make_format_args(1 * isq::length[m])),
                           STD_FMT::format_error,
                           Catch::Matchers::Message("only one of 'oan' unit modifiers may be used in the format spec"));
    REQUIRE_THROWS_MATCHES(STD_FMT::vformat("{:%daoUq}", STD_FMT::make_format_args(1 * isq::length[m])),
                           STD_FMT::format_error,
                           Catch::Matchers::Message("only one of 'oan' unit modifiers may be used in the format spec"));
  }

  SECTION("separator")
  {
    REQUIRE_THROWS_MATCHES(STD_FMT::vformat("{:%dUasq}", STD_FMT::make_format_args(1 * isq::length[m])),
                           STD_FMT::format_error,
                           Catch::Matchers::Message("only one of 'sd' unit modifiers may be used in the format spec"));
    REQUIRE_THROWS_MATCHES(STD_FMT::vformat("{:%sadUq}", STD_FMT::make_format_args(1 * isq::length[m])),
                           STD_FMT::format_error,
                           Catch::Matchers::Message("only one of 'sd' unit modifiers may be used in the format spec"));
    REQUIRE_THROWS_MATCHES(STD_FMT::vformat("{:%adsUq}", STD_FMT::make_format_args(1 * isq::length[m])),
                           STD_FMT::format_error,
                           Catch::Matchers::Message("only one of 'sd' unit modifiers may be used in the format spec"));
  }
}

TEST_CASE("dot separator requested for ASCII encoding should throw", "[text][fmt][exception]")
{
  REQUIRE_THROWS_MATCHES(STD_FMT::vformat("{:%dAaq}", STD_FMT::make_format_args(1 * isq::length[m])),
                         STD_FMT::format_error,
                         Catch::Matchers::Message("dot unit separator allowed only for Unicode encoding"));
}

TEST_CASE("%q and %Q can be put anywhere in a format string", "[text][fmt]")
{
  SECTION("no space") { CHECK(STD_FMT::format("{:%Q%q}", 123 * isq::speed[km / h]) == "123km/h"); }

  SECTION("separator") { CHECK(STD_FMT::format("{:%Q###%q}", 123 * isq::speed[km / h]) == "123###km/h"); }

  SECTION("opposite order") { CHECK(STD_FMT::format("{:%q %Q}", 123 * isq::speed[km / h]) == "km/h 123"); }
}

TEST_CASE("fill and align specification", "[text][fmt][ostream]")
{
  SECTION("ostream")
  {
    std::ostringstream os;

    SECTION("width = 10")
    {
      os << "|" << std::setw(10) << 123 * isq::length[m] << "|";
      CHECK(os.str() == "|     123 m|");
    }

    SECTION("width = 10, align = right")
    {
      os << "|" << std::setw(10) << std::right << 123 * isq::length[m] << "|";
      CHECK(os.str() == "|     123 m|");
    }

    SECTION("width = 10, align = left")
    {
      os << "|" << std::setw(10) << std::left << 123 * isq::length[m] << "|";
      CHECK(os.str() == "|123 m     |");
    }

    SECTION("width = 10, fill = *")
    {
      os << "|" << std::setw(10) << std::setfill('*') << 123 * isq::length[m] << "|";
      CHECK(os.str() == "|*****123 m|");
    }

    SECTION("width = 10, fill = *, align = right")
    {
      os << "|" << std::setw(10) << std::setfill('*') << std::right << 123 * isq::length[m] << "|";
      CHECK(os.str() == "|*****123 m|");
    }

    SECTION("width = 10, fill = *, align = left")
    {
      os << "|" << std::setw(10) << std::setfill('*') << std::left << 123 * isq::length[m] << "|";
      CHECK(os.str() == "|123 m*****|");
    }
  }

  SECTION("default format {} on a quantity")
  {
    CHECK(STD_FMT::format("|{:0}|", 123 * isq::length[m]) == "|123 m|");
    CHECK(STD_FMT::format("|{:10}|", 123 * isq::length[m]) == "|     123 m|");
    CHECK(STD_FMT::format("|{:<10}|", 123 * isq::length[m]) == "|123 m     |");
    CHECK(STD_FMT::format("|{:>10}|", 123 * isq::length[m]) == "|     123 m|");
    CHECK(STD_FMT::format("|{:^10}|", 123 * isq::length[m]) == "|  123 m   |");
    CHECK(STD_FMT::format("|{:*<10}|", 123 * isq::length[m]) == "|123 m*****|");
    CHECK(STD_FMT::format("|{:*>10}|", 123 * isq::length[m]) == "|*****123 m|");
    CHECK(STD_FMT::format("|{:*^10}|", 123 * isq::length[m]) == "|**123 m***|");
  }

  SECTION("full format {:%Q %q} on a quantity")
  {
    CHECK(STD_FMT::format("|{:0%Q%q}|", 123 * isq::length[m]) == "|123m|");
    CHECK(STD_FMT::format("|{:10%Q%q}|", 123 * isq::length[m]) == "|      123m|");
    CHECK(STD_FMT::format("|{:<10%Q%q}|", 123 * isq::length[m]) == "|123m      |");
    CHECK(STD_FMT::format("|{:>10%Q%q}|", 123 * isq::length[m]) == "|      123m|");
    CHECK(STD_FMT::format("|{:^10%Q%q}|", 123 * isq::length[m]) == "|   123m   |");
    CHECK(STD_FMT::format("|{:*<10%Q%q}|", 123 * isq::length[m]) == "|123m******|");
    CHECK(STD_FMT::format("|{:*>10%Q%q}|", 123 * isq::length[m]) == "|******123m|");
    CHECK(STD_FMT::format("|{:*^10%Q%q}|", 123 * isq::length[m]) == "|***123m***|");
  }

  SECTION("value only format {:%Q} on a quantity")
  {
    CHECK(STD_FMT::format("|{:0%Q}|", 123 * isq::length[m]) == "|123|");
    CHECK(STD_FMT::format("|{:10%Q}|", 123 * isq::length[m]) == "|       123|");
    CHECK(STD_FMT::format("|{:<10%Q}|", 123 * isq::length[m]) == "|123       |");
    CHECK(STD_FMT::format("|{:>10%Q}|", 123 * isq::length[m]) == "|       123|");
    CHECK(STD_FMT::format("|{:^10%Q}|", 123 * isq::length[m]) == "|   123    |");
    CHECK(STD_FMT::format("|{:*<10%Q}|", 123 * isq::length[m]) == "|123*******|");
    CHECK(STD_FMT::format("|{:*>10%Q}|", 123 * isq::length[m]) == "|*******123|");
    CHECK(STD_FMT::format("|{:*^10%Q}|", 123 * isq::length[m]) == "|***123****|");
  }

  SECTION("symbol only format {:%q} on a quantity")
  {
    CHECK(STD_FMT::format("|{:0%q}|", 123 * isq::length[m]) == "|m|");
    CHECK(STD_FMT::format("|{:10%q}|", 123 * isq::length[m]) == "|         m|");
    CHECK(STD_FMT::format("|{:<10%q}|", 123 * isq::length[m]) == "|m         |");
    CHECK(STD_FMT::format("|{:>10%q}|", 123 * isq::length[m]) == "|         m|");
    CHECK(STD_FMT::format("|{:^10%q}|", 123 * isq::length[m]) == "|    m     |");
    CHECK(STD_FMT::format("|{:*<10%q}|", 123 * isq::length[m]) == "|m*********|");
    CHECK(STD_FMT::format("|{:*>10%q}|", 123 * isq::length[m]) == "|*********m|");
    CHECK(STD_FMT::format("|{:*^10%q}|", 123 * isq::length[m]) == "|****m*****|");
  }
}

TEST_CASE("sign specification", "[text][fmt]")
{
  quantity<isq::length[m]> inf(std::numeric_limits<double>::infinity());
  quantity<isq::length[m]> nan(std::numeric_limits<double>::quiet_NaN());

  SECTION("full format {:%Q %q} on a quantity")
  {
    CHECK(STD_FMT::format("{0:%Q%q},{0:%+Q%q},{0:%-Q%q},{0:% Q%q}", 1 * isq::length[m]) == "1m,+1m,1m, 1m");
    CHECK(STD_FMT::format("{0:%Q%q},{0:%+Q%q},{0:%-Q%q},{0:% Q%q}", -1 * isq::length[m]) == "-1m,-1m,-1m,-1m");
    CHECK(STD_FMT::format("{0:%Q%q},{0:%+Q%q},{0:%-Q%q},{0:% Q%q}", inf) == "infm,+infm,infm, infm");
    CHECK(STD_FMT::format("{0:%Q%q},{0:%+Q%q},{0:%-Q%q},{0:% Q%q}", nan) == "nanm,+nanm,nanm, nanm");
  }

  SECTION("value only format {:%Q} on a quantity")
  {
    CHECK(STD_FMT::format("{0:%Q},{0:%+Q},{0:%-Q},{0:% Q}", 1 * isq::length[m]) == "1,+1,1, 1");
    CHECK(STD_FMT::format("{0:%Q},{0:%+Q},{0:%-Q},{0:% Q}", -1 * isq::length[m]) == "-1,-1,-1,-1");
    CHECK(STD_FMT::format("{0:%Q},{0:%+Q},{0:%-Q},{0:% Q}", inf) == "inf,+inf,inf, inf");
    CHECK(STD_FMT::format("{0:%Q},{0:%+Q},{0:%-Q},{0:% Q}", nan) == "nan,+nan,nan, nan");
  }
}

TEST_CASE("precision specification", "[text][fmt]")
{
  SECTION("full format {:%Q %q} on a quantity")
  {
    CHECK(STD_FMT::format("{:%.0Q %q}", 1.2345 * isq::length[m]) == "1 m");
    CHECK(STD_FMT::format("{:%.1Q %q}", 1.2345 * isq::length[m]) == "1.2 m");
    CHECK(STD_FMT::format("{:%.2Q %q}", 1.2345 * isq::length[m]) == "1.23 m");
    CHECK(STD_FMT::format("{:%.3Q %q}", 1.2345 * isq::length[m]) == "1.234 m");
    CHECK(STD_FMT::format("{:%.4Q %q}", 1.2345 * isq::length[m]) == "1.2345 m");
    CHECK(STD_FMT::format("{:%.5Q %q}", 1.2345 * isq::length[m]) == "1.23450 m");
    CHECK(STD_FMT::format("{:%.10Q %q}", 1.2345 * isq::length[m]) == "1.2345000000 m");
  }

  SECTION("value only format {:%Q} on a quantity")
  {
    CHECK(STD_FMT::format("{:%.0Q}", 1.2345 * isq::length[m]) == "1");
    CHECK(STD_FMT::format("{:%.1Q}", 1.2345 * isq::length[m]) == "1.2");
    CHECK(STD_FMT::format("{:%.2Q}", 1.2345 * isq::length[m]) == "1.23");
    CHECK(STD_FMT::format("{:%.3Q}", 1.2345 * isq::length[m]) == "1.234");
    CHECK(STD_FMT::format("{:%.4Q}", 1.2345 * isq::length[m]) == "1.2345");
    CHECK(STD_FMT::format("{:%.5Q}", 1.2345 * isq::length[m]) == "1.23450");
    CHECK(STD_FMT::format("{:%.10Q}", 1.2345 * isq::length[m]) == "1.2345000000");
  }
}

TEST_CASE("precision specification for integral representation should throw", "[text][fmt][exception]")
{
  SECTION("full format {:%Q %q} on a quantity")
  {
    REQUIRE_THROWS_MATCHES(STD_FMT::vformat("{:%.1Q %q}", STD_FMT::make_format_args(1 * isq::length[m])),
                           STD_FMT::format_error,
                           Catch::Matchers::Message("precision not allowed for integral quantity representation"));
  }

  SECTION("value only format {:%Q} on a quantity")
  {
    REQUIRE_THROWS_MATCHES(STD_FMT::vformat("{:%.1Q}", STD_FMT::make_format_args(1 * isq::length[m])),
                           STD_FMT::format_error,
                           Catch::Matchers::Message("precision not allowed for integral quantity representation"));
  }
}

TEST_CASE("type specification", "[text][fmt]")
{
  SECTION("full format {:%Q %q} on a quantity")
  {
    CHECK(STD_FMT::format("{:%bQ %q}", 42 * isq::length[m]) == "101010 m");
    CHECK(STD_FMT::format("{:%BQ %q}", 42 * isq::length[m]) == "101010 m");
    CHECK(STD_FMT::format("{:%dQ %q}", 42 * isq::length[m]) == "42 m");
    CHECK(STD_FMT::format("{:%oQ %q}", 42 * isq::length[m]) == "52 m");
    CHECK(STD_FMT::format("{:%xQ %q}", 42 * isq::length[m]) == "2a m");
    CHECK(STD_FMT::format("{:%XQ %q}", 42 * isq::length[m]) == "2A m");

    CHECK(STD_FMT::format("{:%aQ %q}", 1.2345678 * isq::length[m]) == "0x1.3c0ca2a5b1d5dp+0 m");
    CHECK(STD_FMT::format("{:%.3aQ %q}", 1.2345678 * isq::length[m]) == "0x1.3c1p+0 m");
    CHECK(STD_FMT::format("{:%AQ %q}", 1.2345678 * isq::length[m]) == "0X1.3C0CA2A5B1D5DP+0 m");
    CHECK(STD_FMT::format("{:%.3AQ %q}", 1.2345678 * isq::length[m]) == "0X1.3C1P+0 m");
    CHECK(STD_FMT::format("{:%eQ %q}", 1.2345678 * isq::length[m]) == "1.234568e+00 m");
    CHECK(STD_FMT::format("{:%.3eQ %q}", 1.2345678 * isq::length[m]) == "1.235e+00 m");
    CHECK(STD_FMT::format("{:%EQ %q}", 1.2345678 * isq::length[m]) == "1.234568E+00 m");
    CHECK(STD_FMT::format("{:%.3EQ %q}", 1.2345678 * isq::length[m]) == "1.235E+00 m");
    CHECK(STD_FMT::format("{:%gQ %q}", 1.2345678 * isq::length[m]) == "1.23457 m");
    CHECK(STD_FMT::format("{:%gQ %q}", 1.2345678e8 * isq::length[m]) == "1.23457e+08 m");
    CHECK(STD_FMT::format("{:%.3gQ %q}", 1.2345678 * isq::length[m]) == "1.23 m");
    CHECK(STD_FMT::format("{:%.3gQ %q}", 1.2345678e8 * isq::length[m]) == "1.23e+08 m");
    CHECK(STD_FMT::format("{:%GQ %q}", 1.2345678 * isq::length[m]) == "1.23457 m");
    CHECK(STD_FMT::format("{:%GQ %q}", 1.2345678e8 * isq::length[m]) == "1.23457E+08 m");
    CHECK(STD_FMT::format("{:%.3GQ %q}", 1.2345678 * isq::length[m]) == "1.23 m");
    CHECK(STD_FMT::format("{:%.3GQ %q}", 1.2345678e8 * isq::length[m]) == "1.23E+08 m");
  }

  SECTION("value only format {:%Q} on a quantity")
  {
    CHECK(STD_FMT::format("{:%bQ}", 42 * isq::length[m]) == "101010");
    CHECK(STD_FMT::format("{:%BQ}", 42 * isq::length[m]) == "101010");
    CHECK(STD_FMT::format("{:%dQ}", 42 * isq::length[m]) == "42");
    CHECK(STD_FMT::format("{:%oQ}", 42 * isq::length[m]) == "52");
    CHECK(STD_FMT::format("{:%xQ}", 42 * isq::length[m]) == "2a");
    CHECK(STD_FMT::format("{:%XQ}", 42 * isq::length[m]) == "2A");

    CHECK(STD_FMT::format("{:%aQ}", 1.2345678 * isq::length[m]) == "0x1.3c0ca2a5b1d5dp+0");
    CHECK(STD_FMT::format("{:%.3aQ}", 1.2345678 * isq::length[m]) == "0x1.3c1p+0");
    CHECK(STD_FMT::format("{:%AQ}", 1.2345678 * isq::length[m]) == "0X1.3C0CA2A5B1D5DP+0");
    CHECK(STD_FMT::format("{:%.3AQ}", 1.2345678 * isq::length[m]) == "0X1.3C1P+0");
    CHECK(STD_FMT::format("{:%eQ}", 1.2345678 * isq::length[m]) == "1.234568e+00");
    CHECK(STD_FMT::format("{:%.3eQ}", 1.2345678 * isq::length[m]) == "1.235e+00");
    CHECK(STD_FMT::format("{:%EQ}", 1.2345678 * isq::length[m]) == "1.234568E+00");
    CHECK(STD_FMT::format("{:%.3EQ}", 1.2345678 * isq::length[m]) == "1.235E+00");
    CHECK(STD_FMT::format("{:%gQ}", 1.2345678 * isq::length[m]) == "1.23457");
    CHECK(STD_FMT::format("{:%gQ}", 1.2345678e8 * isq::length[m]) == "1.23457e+08");
    CHECK(STD_FMT::format("{:%.3gQ}", 1.2345678 * isq::length[m]) == "1.23");
    CHECK(STD_FMT::format("{:%.3gQ}", 1.2345678e8 * isq::length[m]) == "1.23e+08");
    CHECK(STD_FMT::format("{:%GQ}", 1.2345678 * isq::length[m]) == "1.23457");
    CHECK(STD_FMT::format("{:%GQ}", 1.2345678e8 * isq::length[m]) == "1.23457E+08");
    CHECK(STD_FMT::format("{:%.3GQ}", 1.2345678 * isq::length[m]) == "1.23");
    CHECK(STD_FMT::format("{:%.3GQ}", 1.2345678e8 * isq::length[m]) == "1.23E+08");
  }
}

TEST_CASE("different base types with the # specifier", "[text][fmt]")
{
  SECTION("full format {:%Q %q} on a quantity")
  {
    CHECK(STD_FMT::format("{:%#bQ %q}", 42 * isq::length[m]) == "0b101010 m");
    CHECK(STD_FMT::format("{:%#BQ %q}", 42 * isq::length[m]) == "0B101010 m");
    CHECK(STD_FMT::format("{:%#oQ %q}", 42 * isq::length[m]) == "052 m");
    CHECK(STD_FMT::format("{:%#xQ %q}", 42 * isq::length[m]) == "0x2a m");
    CHECK(STD_FMT::format("{:%#XQ %q}", 42 * isq::length[m]) == "0X2A m");
  }

  SECTION("value only format {:%Q} on a quantity")
  {
    CHECK(STD_FMT::format("{:%#bQ}", 42 * isq::length[m]) == "0b101010");
    CHECK(STD_FMT::format("{:%#BQ}", 42 * isq::length[m]) == "0B101010");
    CHECK(STD_FMT::format("{:%#oQ}", 42 * isq::length[m]) == "052");
    CHECK(STD_FMT::format("{:%#xQ}", 42 * isq::length[m]) == "0x2a");
    CHECK(STD_FMT::format("{:%#XQ}", 42 * isq::length[m]) == "0X2A");
  }
}

TEST_CASE("localization with the 'L' specifier", "[text][fmt][localization]")
{
  struct group2 : std::numpunct<char> {
    char do_thousands_sep() const override { return '_'; }
    std::string do_grouping() const override { return "\2"; }
  };

  struct group3 : std::numpunct<char> {
    char do_thousands_sep() const override { return '\''; }
    std::string do_grouping() const override { return "\3"; }
  };

  std::locale grp2{std::locale::classic(), new group2};
  std::locale grp3{std::locale::classic(), new group3};

  SECTION("full format {:%LQ %q} on a quantity")
  {
    CHECK(STD_FMT::format(grp2, "{:%LQ %q}", 299792458 * isq::speed[m / s]) == "2_99_79_24_58 m/s");
    CHECK(STD_FMT::format(grp3, "{:%LQ %q}", 299792458 * isq::speed[m / s]) == "299'792'458 m/s");
  }
}

TEST_CASE("quantity_cast", "[text][ostream]")
{
  std::ostringstream os;

  SECTION("int to double representation")
  {
    const auto q = 121 * isq::length[km] / (2 * isq::time[h]);

    SECTION("original")
    {
      os << q;
      CHECK(os.str() == "60 km/h");
    }

    SECTION("int")
    {
      os << quantity_cast<int>(q);
      CHECK(os.str() == "60 km/h");
    }

    SECTION("double")
    {
      os << quantity_cast<double>(q);
      CHECK(os.str() == "60 km/h");
    }
  }

  SECTION("double to int representation")
  {
    const auto q = 121. * isq::length[km] / (2 * isq::time[h]);

    SECTION("original")
    {
      os << q;
      CHECK(os.str() == "60.5 km/h");
    }

    SECTION("int")
    {
      os << quantity_cast<int>(q);
      CHECK(os.str() == "60 km/h");
    }

    SECTION("double")
    {
      os << quantity_cast<double>(q);
      CHECK(os.str() == "60.5 km/h");
    }
  }
}
