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

#include "units/dimensions/area.h"
#include "units/dimensions/frequency.h"
#include "units/dimensions/power.h"
#include "units/dimensions/velocity.h"
#include "units/dimensions/volume.h"
#include "units/format.h"
#include "units/math.h"
#include <catch2/catch.hpp>
#include <sstream>

using namespace units;

TEST_CASE("operator<< on a quantity", "[text][ostream]")
{
  std::stringstream stream;

  SECTION("quantity with a predefined unit")
  {
    SECTION("integral representation")
    {
      stream << 60W;
      REQUIRE(stream.str() == "60 W");
    }

    SECTION("floating-point representation")
    {
      stream << 72.5kJ;
      REQUIRE(stream.str() == "72.5 kJ");
    }

    SECTION("unit with a prefix")
    {
      stream << 125us;
      REQUIRE(stream.str() == "125 µs");
    }
  }

  SECTION("quantity with a predefined dimension but unknown unit")
  {
    SECTION("unit::ratio as an SI prefix for a dimension with a special symbol")
    {
      stream << 4.N * 2cm;
      REQUIRE(stream.str() == "8 cJ");
    }

    SECTION("unit::ratio for a dimension without a special symbol")
    {
      stream << 2.cm * 2m * 2m;
      REQUIRE(stream.str() == "8 [1/100]m^3");
    }

    SECTION("unit::ratio::num != 1 && unit::ratio::den == 1")
    {
      stream << 4 * 2min / (2s * 2s);
      REQUIRE(stream.str() == "2 [60]Hz");
    }

    SECTION("unit::ratio::num == 1 && unit::ratio::den != 1")
    {
      stream << 20._J / 2min;
      REQUIRE(stream.str() == "10 [1/60]W");
    }

    SECTION("unit::ratio::num != 1 && unit::ratio::den != 1")
    {
      stream << 60.kJ / 2min;
      REQUIRE(stream.str() == "30 [50/3]W");
    }
  }

  SECTION("quantity with an unkown dimension")
  {
    SECTION("unit::ratio::num == 1 && unit::ratio::den == 1")
    {
      stream << 2s * 2m * 2kg;
      REQUIRE(stream.str() == "8 m⋅kg⋅s");
    }

    SECTION("unit::ratio as an SI prefix")
    {
      stream << 4km * 2s;
      REQUIRE(stream.str() == "8 [1000]m⋅s");
    }

    SECTION("unit::ratio::num != 1 && unit::ratio::den == 1")
    {
      stream << 4kg * 2min / (2s * 2s);
      REQUIRE(stream.str() == "2 [60]kg/s");
    }

    SECTION("unit::ratio::num == 1 && unit::ratio::den != 1")
    {
      stream << 20.kg / 2min;
      REQUIRE(stream.str() == "10 [1/60]kg/s");
    }

    SECTION("unit::ratio::num != 1 && unit::ratio::den != 1")
    {
      stream << 60.min / 2km;
      REQUIRE(stream.str() == "30 [3/50]1/m⋅s");
    }

    SECTION("exp::num == 1 && exp::den == 1")
    {
      stream << 4m * 2s;
      REQUIRE(stream.str() == "8 m⋅s");
    }

    SECTION("exp::num == 2 && exp::den == 1 for positive exponent")
    {
      stream << 4m * 2s * 2s;
      REQUIRE(stream.str() == "16 m⋅s^2");
    }

    SECTION("exp::num == 2 && exp::den == 1 for negative exponent (first dimension)")
    {
      stream << 8.s / 2m / 2m;
      REQUIRE(stream.str() == "2 1/m^2⋅s");
    }

    SECTION("exp::num == 2 && exp::den == 1 for negative exponent (not first dimension)")
    {
      stream << 8.m / 2kg / 2kg;
      REQUIRE(stream.str() == "2 m/kg^2");
    }

    SECTION("fractional positive exponent")
    {
      stream << sqrt(9.m);
      REQUIRE(stream.str() == "3 m^(1/2)");
    }

    SECTION("fractional negative exponent")
    {
      stream << sqrt(9 / 1.m);
      REQUIRE(stream.str() == "3 1/m^(1/2)");
    }
  }
}

TEST_CASE("fmt with default format {} on a quantity", "[text][fmt]")
{
  std::stringstream stream;

  SECTION("quantity with a predefined unit")
  {
    SECTION("integral representation")
    {
      constexpr auto q = 60W;
      stream << q;
      REQUIRE(fmt::format("{}", q) == stream.str());
    }

    SECTION("floating-point representation")
    {
      constexpr auto q = 72.5kJ;
      stream << q;
      REQUIRE(fmt::format("{}", q) == stream.str());
    }

    SECTION("unit with a prefix")
    {
      constexpr auto q = 125us;
      stream << q;
      REQUIRE(fmt::format("{}", q) == stream.str());
    }
  }

  SECTION("quantity with a predefined dimension but unknown unit")
  {
    SECTION("unit::ratio as an SI prefix for a dimension with a special symbol")
    {
      constexpr auto q = 4.N * 2cm;
      stream << q;
      REQUIRE(fmt::format("{}", q) == stream.str());
    }

    SECTION("unit::ratio for a dimension without a special symbol")
    {
      constexpr auto q = 2.cm * 2m * 2m;
      stream << q;
      REQUIRE(fmt::format("{}", q) == stream.str());
    }

    SECTION("unit::ratio::num != 1 && unit::ratio::den == 1")
    {
      constexpr auto q = 4 * 2min / (2s * 2s);
      stream << q;
      REQUIRE(fmt::format("{}", q) == stream.str());
    }

    SECTION("unit::ratio::num == 1 && unit::ratio::den != 1")
    {
      constexpr auto q = 20._J / 2min;
      stream << q;
      REQUIRE(fmt::format("{}", q) == stream.str());
    }

    SECTION("unit::ratio::num != 1 && unit::ratio::den != 1")
    {
      constexpr auto q = 60.kJ / 2min;
      stream << q;
      REQUIRE(fmt::format("{}", q) == stream.str());
    }
  }

  SECTION("quantity with an unkown dimension")
  {
    SECTION("unit::ratio::num == 1 && unit::ratio::den == 1")
    {
      constexpr auto q = 2s * 2m * 2kg;
      stream << q;
      REQUIRE(fmt::format("{}", q) == stream.str());
    }

    SECTION("unit::ratio as an SI prefix")
    {
      constexpr auto q = 4km * 2s;
      stream << q;
      REQUIRE(fmt::format("{}", q) == stream.str());
    }

    SECTION("unit::ratio::num != 1 && unit::ratio::den == 1")
    {
      constexpr auto q = 4kg * 2min / (2s * 2s);
      stream << q;
      REQUIRE(fmt::format("{}", q) == stream.str());
    }

    SECTION("unit::ratio::num == 1 && unit::ratio::den != 1")
    {
      constexpr auto q = 20.kg / 2min;
      stream << q;
      REQUIRE(fmt::format("{}", q) == stream.str());
    }

    SECTION("unit::ratio::num != 1 && unit::ratio::den != 1")
    {
      constexpr auto q = 60.min / 2km;
      stream << q;
      REQUIRE(fmt::format("{}", q) == stream.str());
    }

    SECTION("exp::num == 1 && exp::den == 1")
    {
      constexpr auto q = 4m * 2s;
      stream << q;
      REQUIRE(fmt::format("{}", q) == stream.str());
    }

    SECTION("exp::num == 2 && exp::den == 1 for positive exponent")
    {
      constexpr auto q = 4m * 2s * 2s;
      stream << q;
      REQUIRE(fmt::format("{}", q) == stream.str());
    }

    SECTION("exp::num == 2 && exp::den == 1 for negative exponent (first dimension)")
    {
      constexpr auto q = 8.s / 2m / 2m;
      stream << q;
      REQUIRE(fmt::format("{}", q) == stream.str());
    }

    SECTION("exp::num == 2 && exp::den == 1 for negative exponent (not first dimension)")
    {
      constexpr auto q = 8.m / 2kg / 2kg;
      stream << q;
      REQUIRE(fmt::format("{}", q) == stream.str());
    }

    SECTION("fractional positive exponent")
    {
      auto q = sqrt(9.m);
      stream << q;
      REQUIRE(fmt::format("{}", q) == stream.str());
    }

    SECTION("fractional negative exponent")
    {
      auto q = sqrt(9 / 1.m);
      stream << q;
      REQUIRE(fmt::format("{}", q) == stream.str());
    }
  }
}

// Restate operator<< definitions in terms of std::format to make I/O manipulators apply to whole objects
// rather than their parts


// Giving a precision specification
// in the chrono-format-spec is valid only for std::chrono::duration types where the representation type Rep
// is a floating-point type. For all other Rep types, a format_error shall be thrown if the chrono-format-spec
// contains a precision specification.

 // string s = format("{:=>8}", 42ms); // value of s is "====42ms"

