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
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <mp-units/compat_macros.h>
#include <mp-units/ext/format.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cstdint>
#include <iomanip>
#include <limits>
#include <locale>
#include <sstream>
#include <string>
#include <string_view>
#endif
#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/cartesian_vector.h>
#include <mp-units/format.h>
#include <mp-units/ostream.h>  // IWYU pragma: keep
#include <mp-units/systems/cgs.h>
#include <mp-units/systems/isq/electromagnetism.h>
#include <mp-units/systems/isq/mechanics.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si.h>
#endif

using namespace mp_units;
using namespace mp_units::si::unit_symbols;
using v = cartesian_vector<double>;

TEST_CASE("dimension_symbol", "[dimension][symbol]")
{
  using enum text_encoding;

  std::ostringstream os;

  SECTION("default formatting")
  {
    os << dimension_symbol(isq::power.dimension);
    CHECK(os.str() == "L²MT⁻³");
  }

  SECTION("Portable mode")
  {
    os << dimension_symbol<dimension_symbol_formatting{.encoding = portable}>(isq::power.dimension);
    CHECK(os.str() == "L^2MT^-3");
  }
}

TEST_CASE("unit_symbol", "[unit][symbol]")
{
  using enum text_encoding;
  using enum unit_symbol_solidus;
  using enum unit_symbol_separator;

  std::ostringstream os;

  SECTION("default formatting")
  {
    os << unit_symbol(m / s2);
    CHECK(os.str() == "m/s²");
  }

  SECTION("Portable mode")
  {
    os << unit_symbol<unit_symbol_formatting{.encoding = portable}>(m / s2);
    CHECK(os.str() == "m/s^2");
  }

  SECTION("solidus")
  {
    os << unit_symbol<unit_symbol_formatting{.solidus = never}>(m / s2);
    CHECK(os.str() == "m s⁻²");
  }

  SECTION("separator")
  {
    os << unit_symbol<unit_symbol_formatting{.solidus = never, .separator = half_high_dot}>(m / s2);
    CHECK(os.str() == "m⋅s⁻²");
  }
}

// TODO add dimension formatting tests

TEST_CASE("unit formatting", "[unit][fmt]")
{
  SECTION("Unit formatting should use proper text encoding")
  {
    SECTION("Unicode text output")
    {
      CHECK(MP_UNITS_STD_FMT::format("{:U}", km / h) == "km/h");
      CHECK(MP_UNITS_STD_FMT::format("{:U}", si::kilo<si::ohm>) == "kΩ");
      CHECK(MP_UNITS_STD_FMT::format("{:U}", us) == "µs");
      CHECK(MP_UNITS_STD_FMT::format("{:U}", m / s2) == "m/s²");
    }

    SECTION("Unicode text output is used by default")
    {
      CHECK(MP_UNITS_STD_FMT::format("{}", km / h) == "km/h");
      CHECK(MP_UNITS_STD_FMT::format("{}", si::kilo<si::ohm>) == "kΩ");
      CHECK(MP_UNITS_STD_FMT::format("{}", us) == "µs");
      CHECK(MP_UNITS_STD_FMT::format("{}", m / s2) == "m/s²");
    }

    SECTION("Portable text output")
    {
      CHECK(MP_UNITS_STD_FMT::format("{:P}", km / h) == "km/h");
      CHECK(MP_UNITS_STD_FMT::format("{:P}", si::kilo<si::ohm>) == "kohm");
      CHECK(MP_UNITS_STD_FMT::format("{:P}", us) == "us");
      CHECK(MP_UNITS_STD_FMT::format("{:P}", m / s2) == "m/s^2");
    }
  }

  SECTION("unit formatting should print solidus according to specs")
  {
    SECTION("Solidus for only one element in denominator")
    {
      CHECK(MP_UNITS_STD_FMT::format("{:1}", km / h) == "km/h");
      CHECK(MP_UNITS_STD_FMT::format("{:1}", m / s2) == "m/s²");
      CHECK(MP_UNITS_STD_FMT::format("{:1}", kg / m / s2) == "kg m⁻¹ s⁻²");
    }

    SECTION("Solidus for only one element in denominator is used by default")
    {
      CHECK(MP_UNITS_STD_FMT::format("{}", km / h) == "km/h");
      CHECK(MP_UNITS_STD_FMT::format("{}", m / s2) == "m/s²");
      CHECK(MP_UNITS_STD_FMT::format("{}", kg / m / s2) == "kg m⁻¹ s⁻²");
    }

    SECTION("Always use solidus")
    {
      CHECK(MP_UNITS_STD_FMT::format("{:a}", km / h) == "km/h");
      CHECK(MP_UNITS_STD_FMT::format("{:a}", m / s2) == "m/s²");
      CHECK(MP_UNITS_STD_FMT::format("{:a}", kg / m / s2) == "kg/(m s²)");
    }

    SECTION("Never use solidus")
    {
      CHECK(MP_UNITS_STD_FMT::format("{:n}", km / h) == "km h⁻¹");
      CHECK(MP_UNITS_STD_FMT::format("{:n}", m / s2) == "m s⁻²");
      CHECK(MP_UNITS_STD_FMT::format("{:n}", kg / m / s2) == "kg m⁻¹ s⁻²");
    }
  }

  SECTION("Unit formatting should user proper separator")
  {
    SECTION("Space")
    {
      CHECK(MP_UNITS_STD_FMT::format("{:s}", kg * m / s2) == "kg m/s²");
      CHECK(MP_UNITS_STD_FMT::format("{:s}", kg / m / s2) == "kg m⁻¹ s⁻²");
      CHECK(MP_UNITS_STD_FMT::format("{:sa}", kg / m / s2) == "kg/(m s²)");
    }

    SECTION("Space is used by default")
    {
      CHECK(MP_UNITS_STD_FMT::format("{}", kg * m / s2) == "kg m/s²");
      CHECK(MP_UNITS_STD_FMT::format("{}", kg / m / s2) == "kg m⁻¹ s⁻²");
      CHECK(MP_UNITS_STD_FMT::format("{:a}", kg / m / s2) == "kg/(m s²)");
    }

    SECTION("Dot")
    {
      CHECK(MP_UNITS_STD_FMT::format("{:d}", kg * m / s2) == "kg⋅m/s²");
      CHECK(MP_UNITS_STD_FMT::format("{:d}", kg / m / s2) == "kg⋅m⁻¹⋅s⁻²");
      CHECK(MP_UNITS_STD_FMT::format("{:ad}", kg / m / s2) == "kg/(m⋅s²)");
    }
  }
}

TEST_CASE("unit formatting error handling", "[unit][fmt][exception]")
{
  SECTION("unknown unit modifiers should throw")
  {
    SECTION("only the invalid modifier")
    {
      REQUIRE_THROWS_MATCHES(MP_UNITS_STD_FMT::vformat("{:x}", MP_UNITS_STD_FMT::make_format_args(m)),
                             MP_UNITS_STD_FMT::format_error,
                             Catch::Matchers::Message("invalid unit modifier specified"));
    }

    SECTION("invalid modifier in the front")
    {
      REQUIRE_THROWS_MATCHES(MP_UNITS_STD_FMT::vformat("{:xUda}", MP_UNITS_STD_FMT::make_format_args(m)),
                             MP_UNITS_STD_FMT::format_error,
                             Catch::Matchers::Message("invalid unit modifier specified"));
    }

    SECTION("invalid modifier in the end")
    {
      REQUIRE_THROWS_MATCHES(MP_UNITS_STD_FMT::vformat("{:Udax}", MP_UNITS_STD_FMT::make_format_args(m)),
                             MP_UNITS_STD_FMT::format_error,
                             Catch::Matchers::Message("invalid unit modifier specified"));
    }

    SECTION("invalid modifier in the middle")
    {
      REQUIRE_THROWS_MATCHES(MP_UNITS_STD_FMT::vformat("{:Udxa}", MP_UNITS_STD_FMT::make_format_args(m)),
                             MP_UNITS_STD_FMT::format_error,
                             Catch::Matchers::Message("invalid unit modifier specified"));
    }
  }

  SECTION("repeated unit modifiers should throw")
  {
    SECTION("text encoding")
    {
      REQUIRE_THROWS_MATCHES(
        MP_UNITS_STD_FMT::vformat("{:UdaU}", MP_UNITS_STD_FMT::make_format_args(m)), MP_UNITS_STD_FMT::format_error,
        Catch::Matchers::Message("only one of 'UAP' unit modifiers may be used in the format spec"));
      REQUIRE_THROWS_MATCHES(
        MP_UNITS_STD_FMT::vformat("{:dUaU}", MP_UNITS_STD_FMT::make_format_args(m)), MP_UNITS_STD_FMT::format_error,
        Catch::Matchers::Message("only one of 'UAP' unit modifiers may be used in the format spec"));
      REQUIRE_THROWS_MATCHES(
        MP_UNITS_STD_FMT::vformat("{:dUUa}", MP_UNITS_STD_FMT::make_format_args(m)), MP_UNITS_STD_FMT::format_error,
        Catch::Matchers::Message("only one of 'UAP' unit modifiers may be used in the format spec"));
    }

    SECTION("solidus")
    {
      REQUIRE_THROWS_MATCHES(
        MP_UNITS_STD_FMT::vformat("{:aUda}", MP_UNITS_STD_FMT::make_format_args(m)), MP_UNITS_STD_FMT::format_error,
        Catch::Matchers::Message("only one of '1an' unit modifiers may be used in the format spec"));
      REQUIRE_THROWS_MATCHES(
        MP_UNITS_STD_FMT::vformat("{:daUa}", MP_UNITS_STD_FMT::make_format_args(m)), MP_UNITS_STD_FMT::format_error,
        Catch::Matchers::Message("only one of '1an' unit modifiers may be used in the format spec"));
      REQUIRE_THROWS_MATCHES(
        MP_UNITS_STD_FMT::vformat("{:daaU}", MP_UNITS_STD_FMT::make_format_args(m)), MP_UNITS_STD_FMT::format_error,
        Catch::Matchers::Message("only one of '1an' unit modifiers may be used in the format spec"));
    }

    SECTION("separator")
    {
      REQUIRE_THROWS_MATCHES(
        MP_UNITS_STD_FMT::vformat("{:dUad}", MP_UNITS_STD_FMT::make_format_args(m)), MP_UNITS_STD_FMT::format_error,
        Catch::Matchers::Message("only one of 'sd' unit modifiers may be used in the format spec"));
      REQUIRE_THROWS_MATCHES(
        MP_UNITS_STD_FMT::vformat("{:dadU}", MP_UNITS_STD_FMT::make_format_args(m)), MP_UNITS_STD_FMT::format_error,
        Catch::Matchers::Message("only one of 'sd' unit modifiers may be used in the format spec"));
      REQUIRE_THROWS_MATCHES(
        MP_UNITS_STD_FMT::vformat("{:addU}", MP_UNITS_STD_FMT::make_format_args(m)), MP_UNITS_STD_FMT::format_error,
        Catch::Matchers::Message("only one of 'sd' unit modifiers may be used in the format spec"));
    }
  }

  SECTION("more then one modifier of the same kind should throw")
  {
    SECTION("text encoding")
    {
      REQUIRE_THROWS_MATCHES(
        MP_UNITS_STD_FMT::vformat("{:UdaP}", MP_UNITS_STD_FMT::make_format_args(m)), MP_UNITS_STD_FMT::format_error,
        Catch::Matchers::Message("only one of 'UAP' unit modifiers may be used in the format spec"));
      REQUIRE_THROWS_MATCHES(
        MP_UNITS_STD_FMT::vformat("{:dPaU}", MP_UNITS_STD_FMT::make_format_args(m)), MP_UNITS_STD_FMT::format_error,
        Catch::Matchers::Message("only one of 'UAP' unit modifiers may be used in the format spec"));
      REQUIRE_THROWS_MATCHES(
        MP_UNITS_STD_FMT::vformat("{:dPUa}", MP_UNITS_STD_FMT::make_format_args(m)), MP_UNITS_STD_FMT::format_error,
        Catch::Matchers::Message("only one of 'UAP' unit modifiers may be used in the format spec"));
    }

    SECTION("solidus")
    {
      REQUIRE_THROWS_MATCHES(
        MP_UNITS_STD_FMT::vformat("{:aUdn}", MP_UNITS_STD_FMT::make_format_args(m)), MP_UNITS_STD_FMT::format_error,
        Catch::Matchers::Message("only one of '1an' unit modifiers may be used in the format spec"));
      REQUIRE_THROWS_MATCHES(
        MP_UNITS_STD_FMT::vformat("{:dnUa}", MP_UNITS_STD_FMT::make_format_args(m)), MP_UNITS_STD_FMT::format_error,
        Catch::Matchers::Message("only one of '1an' unit modifiers may be used in the format spec"));
      REQUIRE_THROWS_MATCHES(
        MP_UNITS_STD_FMT::vformat("{:da1U}", MP_UNITS_STD_FMT::make_format_args(m)), MP_UNITS_STD_FMT::format_error,
        Catch::Matchers::Message("only one of '1an' unit modifiers may be used in the format spec"));
    }

    SECTION("separator")
    {
      REQUIRE_THROWS_MATCHES(
        MP_UNITS_STD_FMT::vformat("{:dUas}", MP_UNITS_STD_FMT::make_format_args(m)), MP_UNITS_STD_FMT::format_error,
        Catch::Matchers::Message("only one of 'sd' unit modifiers may be used in the format spec"));
      REQUIRE_THROWS_MATCHES(
        MP_UNITS_STD_FMT::vformat("{:sadU}", MP_UNITS_STD_FMT::make_format_args(m)), MP_UNITS_STD_FMT::format_error,
        Catch::Matchers::Message("only one of 'sd' unit modifiers may be used in the format spec"));
      REQUIRE_THROWS_MATCHES(
        MP_UNITS_STD_FMT::vformat("{:adsU}", MP_UNITS_STD_FMT::make_format_args(m)), MP_UNITS_STD_FMT::format_error,
        Catch::Matchers::Message("only one of 'sd' unit modifiers may be used in the format spec"));
    }
  }

  SECTION("half_high_dot separator requested for portable encoding should throw")
  {
    REQUIRE_THROWS_MATCHES(MP_UNITS_STD_FMT::vformat("{:dPa}", MP_UNITS_STD_FMT::make_format_args(m)),
                           MP_UNITS_STD_FMT::format_error,
                           Catch::Matchers::Message("half_high_dot unit separator allowed only for UTF-8 encoding"));
  }
}

TEST_CASE("default quantity formatting", "[quantity][ostream][fmt]")
{
  std::ostringstream os;

  SECTION("quantity with a predefined unit")
  {
    SECTION("integral representation")
    {
      const auto q = 60 * isq::power[W];
      os << q;

      SECTION("iostream") { CHECK(os.str() == "60 W"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(MP_UNITS_STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%N%?%U} on a quantity")
      {
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U}", q) == os.str());
      }
    }

    SECTION("floating-point representation")
    {
      const auto q = 1023.5 * isq::pressure[Pa];
      os << q;

      SECTION("iostream") { CHECK(os.str() == "1023.5 Pa"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(MP_UNITS_STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%N%?%U} on a quantity")
      {
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U}", q) == os.str());
      }
    }
  }

  SECTION("quantity with a predefined prefixed unit")
  {
    const auto q = 125 * isq::time[us];
    os << q;

    SECTION("iostream") { CHECK(os.str() == "125 µs"); }

    SECTION("fmt with default format {} on a quantity") { CHECK(MP_UNITS_STD_FMT::format("{}", q) == os.str()); }

    SECTION("fmt with format {:%N%?%U} on a quantity") { CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U}", q) == os.str()); }
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

        SECTION("fmt with default format {} on a quantity") { CHECK(MP_UNITS_STD_FMT::format("{}", q) == os.str()); }

        SECTION("fmt with format {:%N%?%U} on a quantity")
        {
          CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U}", q) == os.str());
        }
      }

      SECTION("volume")
      {
        const auto q = 2 * isq::length[m] * (1 * isq::length[m]) * (1 * isq::length[m]);
        os << q;

        SECTION("iostream") { CHECK(os.str() == "2 m³"); }

        SECTION("fmt with default format {} on a quantity") { CHECK(MP_UNITS_STD_FMT::format("{}", q) == os.str()); }

        SECTION("fmt with format {:%N%?%U} on a quantity")
        {
          CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U}", q) == os.str());
        }
      }

      SECTION("entropy")
      {
        const auto q = 20 * isq::kinetic_energy[J] / (delta<isq::thermodynamic_temperature[K]>(2));
        os << q;

        SECTION("iostream") { CHECK(os.str() == "10 J/K"); }

        SECTION("fmt with default format {} on a quantity") { CHECK(MP_UNITS_STD_FMT::format("{}", q) == os.str()); }

        SECTION("fmt with format {:%N%?%U} on a quantity")
        {
          CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U}", q) == os.str());
        }
      }
    }

    SECTION("scaled derived unit")
    {
      SECTION("speed")
      {
        const auto q = 20 * isq::length[km] / (2 * isq::time[h]);
        os << q;

        SECTION("iostream") { CHECK(os.str() == "10 km/h"); }

        SECTION("fmt with default format {} on a quantity") { CHECK(MP_UNITS_STD_FMT::format("{}", q) == os.str()); }

        SECTION("fmt with format {:%N%?%U} on a quantity")
        {
          CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U}", q) == os.str());
        }
      }

      SECTION("angular impulse")
      {
        const auto q = v{1, 2, 3} * isq::angular_impulse[N * m * s];
        os << q;

        SECTION("iostream") { CHECK(os.str() == "[1, 2, 3] m N s"); }

        SECTION("fmt with default format {} on a quantity") { CHECK(MP_UNITS_STD_FMT::format("{}", q) == os.str()); }

        SECTION("fmt with format {:%N%?%U} on a quantity")
        {
          CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U}", q) == os.str());
        }
      }

      SECTION("compressibility")
      {
        const auto q = 123 * isq::compressibility[one / Pa];
        os << q;

        SECTION("iostream") { CHECK(os.str() == "123 1/Pa"); }

        SECTION("fmt with default format {} on a quantity") { CHECK(MP_UNITS_STD_FMT::format("{}", q) == os.str()); }

        SECTION("fmt with format {:%N%?%U} on a quantity")
        {
          CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U}", q) == os.str());
        }
      }

      SECTION("angular acceleration")
      {
        const auto q = v{1, 2, 3} * isq::angular_acceleration[rad / s2];
        os << q;

        SECTION("iostream") { CHECK(os.str() == "[1, 2, 3] rad/s²"); }

        SECTION("fmt with default format {} on a quantity") { CHECK(MP_UNITS_STD_FMT::format("{}", q) == os.str()); }

        SECTION("fmt with format {:%N%?%U} on a quantity")
        {
          CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U}", q) == os.str());
        }
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

      SECTION("fmt with default format {} on a quantity") { CHECK(MP_UNITS_STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%N%?%U} on a quantity") { CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U}", q) == "2"); }
    }

    SECTION("one with ratio.exp != 0")
    {
      const auto q = 4 * isq::length[km] / (2 * isq::length[m]);
      os << q;

      SECTION("iostream") { CHECK(os.str() == "2 km/m"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(MP_UNITS_STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%N%?%U} on a quantity")
      {
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U}", q) == "2 km/m");
      }
    }

    SECTION("percents")
    {
      constexpr auto q = value_cast<percent>(15. * isq::length[m] / (100. * isq::length[m]));
      os << q;

      SECTION("iostream") { CHECK(os.str() == "15 %"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(MP_UNITS_STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%N%?%U} on a quantity")
      {
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U}", q) == os.str());
      }
    }

    SECTION("radians")
    {
      const auto q = 42 * rad;
      os << q;

      SECTION("iostream") { CHECK(os.str() == "42 rad"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(MP_UNITS_STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%N%?%U} on a quantity")
      {
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U}", q) == os.str());
      }
    }
  }

  SECTION("no space before unit symbol")
  {
    SECTION("degree")
    {
      const auto q = 42 * deg;
      os << q;

      SECTION("iostream") { CHECK(os.str() == "42°"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(MP_UNITS_STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%N%?%U} on a quantity")
      {
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U}", q) == os.str());
      }
    }

    SECTION("arcminute")
    {
      const auto q = 42 * arcmin;
      os << q;

      SECTION("iostream") { CHECK(os.str() == "42′"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(MP_UNITS_STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%N%?%U} on a quantity")
      {
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U}", q) == os.str());
      }
    }

    SECTION("arcsecond")
    {
      const auto q = 42 * arcsec;
      os << q;

      SECTION("iostream") { CHECK(os.str() == "42″"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(MP_UNITS_STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%N%?%U} on a quantity")
      {
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U}", q) == os.str());
      }
    }
  }

  SECTION("8-bit integers")
  {
    SECTION("signed positive")
    {
      const auto q = std::int8_t{42} * si::second;
      os << q;

      SECTION("iostream") { CHECK(os.str() == "42 s"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(MP_UNITS_STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%N%?%U} on a quantity")
      {
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U}", q) == os.str());
      }
    }

    SECTION("signed negative")
    {
      const auto q = std::int8_t{-42} * si::second;
      os << q;

      SECTION("iostream") { CHECK(os.str() == "-42 s"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(MP_UNITS_STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%N%?%U} on a quantity")
      {
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U}", q) == os.str());
      }
    }

    SECTION("unsigned")
    {
      const auto q = std::uint8_t{42} * si::second;
      os << q;

      SECTION("iostream") { CHECK(os.str() == "42 s"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(MP_UNITS_STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%N%?%U} on a quantity")
      {
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U}", q) == os.str());
      }
    }
  }
}

TEST_CASE("quantity fill and align specification", "[quantity][ostream][fmt]")
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
    CHECK(MP_UNITS_STD_FMT::format("|{:0}|", 123 * isq::length[m]) == "|123 m|");
    CHECK(MP_UNITS_STD_FMT::format("|{:10}|", 123 * isq::length[m]) == "|     123 m|");
    CHECK(MP_UNITS_STD_FMT::format("|{:<10}|", 123 * isq::length[m]) == "|123 m     |");
    CHECK(MP_UNITS_STD_FMT::format("|{:>10}|", 123 * isq::length[m]) == "|     123 m|");
    CHECK(MP_UNITS_STD_FMT::format("|{:^10}|", 123 * isq::length[m]) == "|  123 m   |");
    CHECK(MP_UNITS_STD_FMT::format("|{:*<10}|", 123 * isq::length[m]) == "|123 m*****|");
    CHECK(MP_UNITS_STD_FMT::format("|{:*>10}|", 123 * isq::length[m]) == "|*****123 m|");
    CHECK(MP_UNITS_STD_FMT::format("|{:*^10}|", 123 * isq::length[m]) == "|**123 m***|");
  }

  SECTION("full format {:%N%?%U} on a quantity")
  {
    CHECK(MP_UNITS_STD_FMT::format("|{:0%N%?%U}|", 123 * isq::length[m]) == "|123 m|");
    CHECK(MP_UNITS_STD_FMT::format("|{:10%N%?%U}|", 123 * isq::length[m]) == "|     123 m|");
    CHECK(MP_UNITS_STD_FMT::format("|{:<10%N%?%U}|", 123 * isq::length[m]) == "|123 m     |");
    CHECK(MP_UNITS_STD_FMT::format("|{:>10%N%?%U}|", 123 * isq::length[m]) == "|     123 m|");
    CHECK(MP_UNITS_STD_FMT::format("|{:^10%N%?%U}|", 123 * isq::length[m]) == "|  123 m   |");
    CHECK(MP_UNITS_STD_FMT::format("|{:*<10%N%?%U}|", 123 * isq::length[m]) == "|123 m*****|");
    CHECK(MP_UNITS_STD_FMT::format("|{:*>10%N%?%U}|", 123 * isq::length[m]) == "|*****123 m|");
    CHECK(MP_UNITS_STD_FMT::format("|{:*^10%N%?%U}|", 123 * isq::length[m]) == "|**123 m***|");
  }

  SECTION("value only format {:%N} on a quantity")
  {
    CHECK(MP_UNITS_STD_FMT::format("|{:0%N}|", 123 * isq::length[m]) == "|123|");
    CHECK(MP_UNITS_STD_FMT::format("|{:10%N}|", 123 * isq::length[m]) == "|       123|");
    CHECK(MP_UNITS_STD_FMT::format("|{:<10%N}|", 123 * isq::length[m]) == "|123       |");
    CHECK(MP_UNITS_STD_FMT::format("|{:>10%N}|", 123 * isq::length[m]) == "|       123|");
    CHECK(MP_UNITS_STD_FMT::format("|{:^10%N}|", 123 * isq::length[m]) == "|   123    |");
    CHECK(MP_UNITS_STD_FMT::format("|{:*<10%N}|", 123 * isq::length[m]) == "|123*******|");
    CHECK(MP_UNITS_STD_FMT::format("|{:*>10%N}|", 123 * isq::length[m]) == "|*******123|");
    CHECK(MP_UNITS_STD_FMT::format("|{:*^10%N}|", 123 * isq::length[m]) == "|***123****|");
  }

  SECTION("symbol only format {:%U} on a quantity")
  {
    CHECK(MP_UNITS_STD_FMT::format("|{:0%U}|", 123 * isq::length[m]) == "|m|");
    CHECK(MP_UNITS_STD_FMT::format("|{:10%U}|", 123 * isq::length[m]) == "|         m|");
    CHECK(MP_UNITS_STD_FMT::format("|{:<10%U}|", 123 * isq::length[m]) == "|m         |");
    CHECK(MP_UNITS_STD_FMT::format("|{:>10%U}|", 123 * isq::length[m]) == "|         m|");
    CHECK(MP_UNITS_STD_FMT::format("|{:^10%U}|", 123 * isq::length[m]) == "|    m     |");
    CHECK(MP_UNITS_STD_FMT::format("|{:*<10%U}|", 123 * isq::length[m]) == "|m*********|");
    CHECK(MP_UNITS_STD_FMT::format("|{:*>10%U}|", 123 * isq::length[m]) == "|*********m|");
    CHECK(MP_UNITS_STD_FMT::format("|{:*^10%U}|", 123 * isq::length[m]) == "|****m*****|");
  }
}

TEST_CASE("quantity subentities selection", "[quantity][fmt]")
{
  SECTION("quantity format string with only %N should print quantity value only")
  {
    SECTION("integral representation")
    {
      SECTION("positive value") { CHECK(MP_UNITS_STD_FMT::format("{:%N}", 123 * isq::speed[km / h]) == "123"); }

      SECTION("negative value")
      {
        CHECK(MP_UNITS_STD_FMT::format("{:%N}", 5 * isq::length[m] - 10 * isq::length[m]) == "-5");
      }
    }

    SECTION("floating-point representation")
    {
      SECTION("positive value")
      {
        CHECK(MP_UNITS_STD_FMT::format("{:%N}", 221. * isq::length[km] / (2 * isq::time[h])) == "110.5");
      }

      SECTION("negative value")
      {
        CHECK(MP_UNITS_STD_FMT::format("{:%N}", 3.14 * isq::length[m] - 10 * isq::length[m]) == "-6.859999999999999");
      }

      SECTION("nan")
      {
        CHECK(MP_UNITS_STD_FMT::format("{:%N}", std::numeric_limits<double>::quiet_NaN() * isq::length[m]) == "nan");
      }

      SECTION("inf")
      {
        CHECK(MP_UNITS_STD_FMT::format("{:%N}", std::numeric_limits<double>::infinity() * isq::length[m]) == "inf");
      }

      SECTION("-inf")
      {
        CHECK(MP_UNITS_STD_FMT::format("{:%N}", -std::numeric_limits<double>::infinity() * isq::length[m]) == "-inf");
      }
    }
  }

  SECTION("quantity format string with only %U should print quantity unit symbol only")
  {
    CHECK(MP_UNITS_STD_FMT::format("{:%U}", 123 * isq::speed[km / h]) == "km/h");
    CHECK(MP_UNITS_STD_FMT::format("{:%U}", 123 * isq::resistance[si::kilo<si::ohm>]) == "kΩ");
    CHECK(MP_UNITS_STD_FMT::format("{:%U}", 123 * isq::time[us]) == "µs");
    CHECK(MP_UNITS_STD_FMT::format("{:%U}", v{1, 2, 3} * isq::acceleration[m / s2]) == "m/s²");
    CHECK(MP_UNITS_STD_FMT::format("{:%U}", 123 * percent) == "%");
  }

  SECTION("%U and %N can be put anywhere in a format string")
  {
    SECTION("no space") { CHECK(MP_UNITS_STD_FMT::format("{:%N%U}", 123 * isq::speed[km / h]) == "123km/h"); }

    SECTION("separator") { CHECK(MP_UNITS_STD_FMT::format("{:%N###%U}", 123 * isq::speed[km / h]) == "123###km/h"); }

    SECTION("opposite order") { CHECK(MP_UNITS_STD_FMT::format("{:%U %N}", 123 * isq::speed[km / h]) == "km/h 123"); }
  }
}

// TODO provide basic tests if format string when provided in a quantity formatter are passed to respective dimensions
// and units formatters (detail formatting tests for dimensions and units are done separately)

TEST_CASE("quantity numerical value formatting for `std` arithmetic types", "[quantity][fmt]")
{
  SECTION("sign specification")
  {
    auto inf = std::numeric_limits<double>::infinity() * si::metre;
    auto nan = std::numeric_limits<double>::quiet_NaN() * si::metre;

    SECTION("full format {:%N%?%U} on a quantity")
    {
      CHECK(MP_UNITS_STD_FMT::format("{0:%N%U},{0:%N%U:N[+]},{0:%N%U:N[-]},{0:%N%U:N[ ]}", 1 * isq::length[m]) ==
            "1m,+1m,1m, 1m");
      CHECK(MP_UNITS_STD_FMT::format("{0:%N%U},{0:%N%U:N[+]},{0:%N%U:N[-]},{0:%N%U:N[ ]}", -1 * isq::length[m]) ==
            "-1m,-1m,-1m,-1m");
      CHECK(MP_UNITS_STD_FMT::format("{0:%N%U},{0:%N%U:N[+]},{0:%N%U:N[-]},{0:%N%U:N[ ]}", inf) ==
            "infm,+infm,infm, infm");
      CHECK(MP_UNITS_STD_FMT::format("{0:%N%U},{0:%N%U:N[+]},{0:%N%U:N[-]},{0:%N%U:N[ ]}", nan) ==
            "nanm,+nanm,nanm, nanm");
    }

    SECTION("value only format {:%N} on a quantity")
    {
      CHECK(MP_UNITS_STD_FMT::format("{0:%N},{0:%N:N[+]},{0:%N:N[-]},{0:%N:N[ ]}", 1 * isq::length[m]) == "1,+1,1, 1");
      CHECK(MP_UNITS_STD_FMT::format("{0:%N},{0:%N:N[+]},{0:%N:N[-]},{0:%N:N[ ]}", -1 * isq::length[m]) ==
            "-1,-1,-1,-1");
      CHECK(MP_UNITS_STD_FMT::format("{0:%N},{0:%N:N[+]},{0:%N:N[-]},{0:%N:N[ ]}", inf) == "inf,+inf,inf, inf");
      CHECK(MP_UNITS_STD_FMT::format("{0:%N},{0:%N:N[+]},{0:%N:N[-]},{0:%N:N[ ]}", nan) == "nan,+nan,nan, nan");
    }
  }

  SECTION("precision specification")
  {
    SECTION("full format on a quantity")
    {
      SECTION("default spec")
      {
        CHECK(MP_UNITS_STD_FMT::format("{::N[.0f]}", 1.2345 * isq::length[m]) == "1 m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[.1f]}", 1.2345 * isq::length[m]) == "1.2 m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[.2f]}", 1.2345 * isq::length[m]) == "1.23 m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[.3f]}", 1.2345 * isq::length[m]) == "1.234 m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[.4f]}", 1.2345 * isq::length[m]) == "1.2345 m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[.5f]}", 1.2345 * isq::length[m]) == "1.23450 m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[.10f]}", 1.2345 * isq::length[m]) == "1.2345000000 m");
      }

      SECTION("explicit spec")
      {
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[.0f]}", 1.2345 * isq::length[m]) == "1 m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[.1f]}", 1.2345 * isq::length[m]) == "1.2 m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[.2f]}", 1.2345 * isq::length[m]) == "1.23 m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[.3f]}", 1.2345 * isq::length[m]) == "1.234 m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[.4f]}", 1.2345 * isq::length[m]) == "1.2345 m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[.5f]}", 1.2345 * isq::length[m]) == "1.23450 m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[.10f]}", 1.2345 * isq::length[m]) == "1.2345000000 m");
      }

      SECTION("modified spec")
      {
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[.0f]}", 1.2345 * isq::length[m]) == "1m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[.1f]}", 1.2345 * isq::length[m]) == "1.2m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[.2f]}", 1.2345 * isq::length[m]) == "1.23m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[.3f]}", 1.2345 * isq::length[m]) == "1.234m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[.4f]}", 1.2345 * isq::length[m]) == "1.2345m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[.5f]}", 1.2345 * isq::length[m]) == "1.23450m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[.10f]}", 1.2345 * isq::length[m]) == "1.2345000000m");
      }
    }

    SECTION("value only format {:%N} on a quantity")
    {
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[.0f]}", 1.2345 * isq::length[m]) == "1");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[.1f]}", 1.2345 * isq::length[m]) == "1.2");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[.2f]}", 1.2345 * isq::length[m]) == "1.23");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[.3f]}", 1.2345 * isq::length[m]) == "1.234");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[.4f]}", 1.2345 * isq::length[m]) == "1.2345");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[.5f]}", 1.2345 * isq::length[m]) == "1.23450");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[.10f]}", 1.2345 * isq::length[m]) == "1.2345000000");
    }
  }

  SECTION("type specification")
  {
    SECTION("full format {:%N%?%U} on a quantity")
    {
      SECTION("default spec")
      {
        CHECK(MP_UNITS_STD_FMT::format("{::N[b]}", 42 * isq::length[m]) == "101010 m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[B]}", 42 * isq::length[m]) == "101010 m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[d]}", 42 * isq::length[m]) == "42 m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[o]}", 42 * isq::length[m]) == "52 m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[x]}", 42 * isq::length[m]) == "2a m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[X]}", 42 * isq::length[m]) == "2A m");

#if MP_UNITS_USE_FMTLIB
        CHECK(MP_UNITS_STD_FMT::format("{::N[a]}", 1.2345678 * isq::length[m]) == "0x1.3c0ca2a5b1d5dp+0 m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[.3a]}", 1.2345678 * isq::length[m]) == "0x1.3c1p+0 m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[A]}", 1.2345678 * isq::length[m]) == "0X1.3C0CA2A5B1D5DP+0 m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[.3A]}", 1.2345678 * isq::length[m]) == "0X1.3C1P+0 m");
#else
        CHECK(MP_UNITS_STD_FMT::format("{::N[a]}", 1.2345678 * isq::length[m]) == "1.3c0ca2a5b1d5dp+0 m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[.3a]}", 1.2345678 * isq::length[m]) == "1.3c1p+0 m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[A]}", 1.2345678 * isq::length[m]) == "1.3C0CA2A5B1D5DP+0 m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[.3A]}", 1.2345678 * isq::length[m]) == "1.3C1P+0 m");
#endif
        CHECK(MP_UNITS_STD_FMT::format("{::N[e]}", 1.2345678 * isq::length[m]) == "1.234568e+00 m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[.3e]}", 1.2345678 * isq::length[m]) == "1.235e+00 m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[E]}", 1.2345678 * isq::length[m]) == "1.234568E+00 m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[.3E]}", 1.2345678 * isq::length[m]) == "1.235E+00 m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[g]}", 1.2345678 * isq::length[m]) == "1.23457 m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[g]}", 1.2345678e8 * isq::length[m]) == "1.23457e+08 m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[.3g]}", 1.2345678 * isq::length[m]) == "1.23 m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[.3g]}", 1.2345678e8 * isq::length[m]) == "1.23e+08 m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[G]}", 1.2345678 * isq::length[m]) == "1.23457 m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[G]}", 1.2345678e8 * isq::length[m]) == "1.23457E+08 m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[.3G]}", 1.2345678 * isq::length[m]) == "1.23 m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[.3G]}", 1.2345678e8 * isq::length[m]) == "1.23E+08 m");
      }

      SECTION("explicit spec")
      {
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[b]}", 42 * isq::length[m]) == "101010 m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[B]}", 42 * isq::length[m]) == "101010 m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[d]}", 42 * isq::length[m]) == "42 m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[o]}", 42 * isq::length[m]) == "52 m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[x]}", 42 * isq::length[m]) == "2a m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[X]}", 42 * isq::length[m]) == "2A m");

#if MP_UNITS_USE_FMTLIB
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[a]}", 1.2345678 * isq::length[m]) == "0x1.3c0ca2a5b1d5dp+0 m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[.3a]}", 1.2345678 * isq::length[m]) == "0x1.3c1p+0 m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[A]}", 1.2345678 * isq::length[m]) == "0X1.3C0CA2A5B1D5DP+0 m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[.3A]}", 1.2345678 * isq::length[m]) == "0X1.3C1P+0 m");
#else
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[a]}", 1.2345678 * isq::length[m]) == "1.3c0ca2a5b1d5dp+0 m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[.3a]}", 1.2345678 * isq::length[m]) == "1.3c1p+0 m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[A]}", 1.2345678 * isq::length[m]) == "1.3C0CA2A5B1D5DP+0 m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[.3A]}", 1.2345678 * isq::length[m]) == "1.3C1P+0 m");
#endif
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[e]}", 1.2345678 * isq::length[m]) == "1.234568e+00 m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[.3e]}", 1.2345678 * isq::length[m]) == "1.235e+00 m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[E]}", 1.2345678 * isq::length[m]) == "1.234568E+00 m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[.3E]}", 1.2345678 * isq::length[m]) == "1.235E+00 m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[g]}", 1.2345678 * isq::length[m]) == "1.23457 m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[g]}", 1.2345678e8 * isq::length[m]) == "1.23457e+08 m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[.3g]}", 1.2345678 * isq::length[m]) == "1.23 m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[.3g]}", 1.2345678e8 * isq::length[m]) == "1.23e+08 m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[G]}", 1.2345678 * isq::length[m]) == "1.23457 m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[G]}", 1.2345678e8 * isq::length[m]) == "1.23457E+08 m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[.3G]}", 1.2345678 * isq::length[m]) == "1.23 m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[.3G]}", 1.2345678e8 * isq::length[m]) == "1.23E+08 m");
      }

      SECTION("modified spec")
      {
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[b]}", 42 * isq::length[m]) == "101010m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[B]}", 42 * isq::length[m]) == "101010m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[d]}", 42 * isq::length[m]) == "42m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[o]}", 42 * isq::length[m]) == "52m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[x]}", 42 * isq::length[m]) == "2am");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[X]}", 42 * isq::length[m]) == "2Am");

#if MP_UNITS_USE_FMTLIB
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[a]}", 1.2345678 * isq::length[m]) == "0x1.3c0ca2a5b1d5dp+0m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[.3a]}", 1.2345678 * isq::length[m]) == "0x1.3c1p+0m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[A]}", 1.2345678 * isq::length[m]) == "0X1.3C0CA2A5B1D5DP+0m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[.3A]}", 1.2345678 * isq::length[m]) == "0X1.3C1P+0m");
#else
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[a]}", 1.2345678 * isq::length[m]) == "1.3c0ca2a5b1d5dp+0m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[.3a]}", 1.2345678 * isq::length[m]) == "1.3c1p+0m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[A]}", 1.2345678 * isq::length[m]) == "1.3C0CA2A5B1D5DP+0m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[.3A]}", 1.2345678 * isq::length[m]) == "1.3C1P+0m");
#endif
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[e]}", 1.2345678 * isq::length[m]) == "1.234568e+00m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[.3e]}", 1.2345678 * isq::length[m]) == "1.235e+00m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[E]}", 1.2345678 * isq::length[m]) == "1.234568E+00m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[.3E]}", 1.2345678 * isq::length[m]) == "1.235E+00m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[g]}", 1.2345678 * isq::length[m]) == "1.23457m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[g]}", 1.2345678e8 * isq::length[m]) == "1.23457e+08m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[.3g]}", 1.2345678 * isq::length[m]) == "1.23m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[.3g]}", 1.2345678e8 * isq::length[m]) == "1.23e+08m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[G]}", 1.2345678 * isq::length[m]) == "1.23457m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[G]}", 1.2345678e8 * isq::length[m]) == "1.23457E+08m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[.3G]}", 1.2345678 * isq::length[m]) == "1.23m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[.3G]}", 1.2345678e8 * isq::length[m]) == "1.23E+08m");
      }
    }

    SECTION("value only format {:%N} on a quantity")
    {
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[b]}", 42 * isq::length[m]) == "101010");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[B]}", 42 * isq::length[m]) == "101010");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[d]}", 42 * isq::length[m]) == "42");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[o]}", 42 * isq::length[m]) == "52");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[x]}", 42 * isq::length[m]) == "2a");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[X]}", 42 * isq::length[m]) == "2A");

#if MP_UNITS_USE_FMTLIB
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[a]}", 1.2345678 * isq::length[m]) == "0x1.3c0ca2a5b1d5dp+0");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[.3a]}", 1.2345678 * isq::length[m]) == "0x1.3c1p+0");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[A]}", 1.2345678 * isq::length[m]) == "0X1.3C0CA2A5B1D5DP+0");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[.3A]}", 1.2345678 * isq::length[m]) == "0X1.3C1P+0");
#else
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[a]}", 1.2345678 * isq::length[m]) == "1.3c0ca2a5b1d5dp+0");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[.3a]}", 1.2345678 * isq::length[m]) == "1.3c1p+0");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[A]}", 1.2345678 * isq::length[m]) == "1.3C0CA2A5B1D5DP+0");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[.3A]}", 1.2345678 * isq::length[m]) == "1.3C1P+0");
#endif
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[e]}", 1.2345678 * isq::length[m]) == "1.234568e+00");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[.3e]}", 1.2345678 * isq::length[m]) == "1.235e+00");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[E]}", 1.2345678 * isq::length[m]) == "1.234568E+00");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[.3E]}", 1.2345678 * isq::length[m]) == "1.235E+00");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[g]}", 1.2345678 * isq::length[m]) == "1.23457");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[g]}", 1.2345678e8 * isq::length[m]) == "1.23457e+08");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[.3g]}", 1.2345678 * isq::length[m]) == "1.23");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[.3g]}", 1.2345678e8 * isq::length[m]) == "1.23e+08");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[G]}", 1.2345678 * isq::length[m]) == "1.23457");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[G]}", 1.2345678e8 * isq::length[m]) == "1.23457E+08");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[.3G]}", 1.2345678 * isq::length[m]) == "1.23");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[.3G]}", 1.2345678e8 * isq::length[m]) == "1.23E+08");
    }
  }

  SECTION("different base types with the # specifier")
  {
    SECTION("full format {:%N%?%U} on a quantity")
    {
      SECTION("default spec")
      {
        CHECK(MP_UNITS_STD_FMT::format("{::N[#b]}", 42 * isq::length[m]) == "0b101010 m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[#B]}", 42 * isq::length[m]) == "0B101010 m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[#o]}", 42 * isq::length[m]) == "052 m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[#x]}", 42 * isq::length[m]) == "0x2a m");
        CHECK(MP_UNITS_STD_FMT::format("{::N[#X]}", 42 * isq::length[m]) == "0X2A m");
      }

      SECTION("explicit spec")
      {
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[#b]}", 42 * isq::length[m]) == "0b101010 m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[#B]}", 42 * isq::length[m]) == "0B101010 m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[#o]}", 42 * isq::length[m]) == "052 m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[#x]}", 42 * isq::length[m]) == "0x2a m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%?%U:N[#X]}", 42 * isq::length[m]) == "0X2A m");
      }

      SECTION("modified spec")
      {
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[#b]}", 42 * isq::length[m]) == "0b101010m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[#B]}", 42 * isq::length[m]) == "0B101010m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[#o]}", 42 * isq::length[m]) == "052m");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[#x]}", 42 * isq::length[m]) == "0x2am");
        CHECK(MP_UNITS_STD_FMT::format("{:%N%U:N[#X]}", 42 * isq::length[m]) == "0X2Am");
      }
    }

    SECTION("value only format {:%N} on a quantity")
    {
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[#b]}", 42 * isq::length[m]) == "0b101010");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[#B]}", 42 * isq::length[m]) == "0B101010");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[#o]}", 42 * isq::length[m]) == "052");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[#x]}", 42 * isq::length[m]) == "0x2a");
      CHECK(MP_UNITS_STD_FMT::format("{:%N:N[#X]}", 42 * isq::length[m]) == "0X2A");
    }
  }

  SECTION("localization with the 'L' specifier")
  {
    struct group2 : std::numpunct<char> {
      [[nodiscard]] char do_thousands_sep() const override { return '_'; }
      [[nodiscard]] std::string do_grouping() const override { return "\2"; }
    };

    struct group3 : std::numpunct<char> {
      [[nodiscard]] char do_thousands_sep() const override { return '\''; }
      [[nodiscard]] std::string do_grouping() const override { return "\3"; }
    };

    const std::locale grp2{std::locale::classic(), new group2};
    const std::locale grp3{std::locale::classic(), new group3};

    SECTION("full format on a quantity")
    {
      SECTION("default spec")
      {
        CHECK(MP_UNITS_STD_FMT::format(grp2, "{::N[L]}", 299'792'458 * isq::speed[m / s]) == "2_99_79_24_58 m/s");
        CHECK(MP_UNITS_STD_FMT::format(grp3, "{::N[L]}", 299'792'458 * isq::speed[m / s]) == "299'792'458 m/s");
      }

      SECTION("explicit spec")
      {
        CHECK(MP_UNITS_STD_FMT::format(grp2, "{:%N%?%U:N[L]}", 299'792'458 * isq::speed[m / s]) == "2_99_79_24_58 m/s");
        CHECK(MP_UNITS_STD_FMT::format(grp3, "{:%N%?%U:N[L]}", 299'792'458 * isq::speed[m / s]) == "299'792'458 m/s");
      }

      SECTION("modified spec")
      {
        CHECK(MP_UNITS_STD_FMT::format(grp2, "{:%N%U:N[L]}", 299'792'458 * isq::speed[m / s]) == "2_99_79_24_58m/s");
        CHECK(MP_UNITS_STD_FMT::format(grp3, "{:%N%U:N[L]}", 299'792'458 * isq::speed[m / s]) == "299'792'458m/s");
      }
    }
  }
}

TEST_CASE("check if `value_cast` properly changes the numerical value of a quantity", "[value_cast][ostream]")
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
      os << value_cast<int>(q);
      CHECK(os.str() == "60 km/h");
    }

    SECTION("double")
    {
      os << value_cast<double>(q);
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
      os << value_cast<int>(q);
      CHECK(os.str() == "60 km/h");
    }

    SECTION("double")
    {
      os << value_cast<double>(q);
      CHECK(os.str() == "60.5 km/h");
    }
  }
}
