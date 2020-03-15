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

#include "units/physical/cgs/length.h"
#include "units/physical/cgs/mass.h"
#include "units/physical/si/area.h"
#include "units/physical/si/frequency.h"
#include "units/physical/si/power.h"
#include "units/physical/si/pressure.h"
#include "units/physical/si/velocity.h"
#include "units/physical/si/volume.h"
#include "units/physical/si/surface_tension.h"
#include "units/physical/si/resistance.h"
#include "units/format.h"
#include "units/math.h"
#include <catch2/catch.hpp>
#include <sstream>

using namespace units;
using namespace units::si;
using namespace Catch::Matchers;

TEST_CASE("operator<< on a quantity", "[text][ostream][fmt]")
{
  std::stringstream stream;

  SECTION("quantity with a predefined unit")
  {
    SECTION("integral representation")
    {
      const auto q = 60q_W;
      stream << q;

      SECTION("iostream")
      {
        CHECK(stream.str() == "60 W");
      }

      SECTION("fmt with default format {} on a quantity")
      {
        CHECK(fmt::format("{}", q) == stream.str());
      }

      SECTION("fmt with format {:%Q %q} on a quantity")
      {
        CHECK(fmt::format("{:%Q %q}", q) == stream.str());
      }
    }

    SECTION("floating-point representation")
    {
      const auto q = 1023.5q_Pa;
      stream << q;

      SECTION("iostream")
      {
        CHECK(stream.str() == "1023.5 Pa");
      }

      SECTION("fmt with default format {} on a quantity")
      {
        CHECK(fmt::format("{}", q) == stream.str());
      }

      SECTION("fmt with format {:%Q %q} on a quantity")
      {
        CHECK(fmt::format("{:%Q %q}", q) == stream.str());
      }
    }
  }

  SECTION("quantity with a predefined prefixed unit")
  {
    const auto q = 125q_us;
    stream << q;

    SECTION("iostream")
    {
      CHECK(stream.str() == "125 µs");
    }

    SECTION("fmt with default format {} on a quantity")
    {
      CHECK(fmt::format("{}", q) == stream.str());
    }

    SECTION("fmt with format {:%Q %q} on a quantity")
    {
      CHECK(fmt::format("{:%Q %q}", q) == stream.str());
    }
  }

  SECTION("quantity with a predefined unit + prefix")
  {
    SECTION("in terms of base units")
    {
      const length<scaled_unit<ratio<1, 1, 6>, metre>> q(123);
      stream << q;

      SECTION("iostream")
      {
        CHECK(stream.str() == "123 Mm");
      }

      SECTION("fmt with default format {} on a quantity")
      {
        CHECK(fmt::format("{}", q) == stream.str());
      }

      SECTION("fmt with format {:%Q %q} on a quantity")
      {
        CHECK(fmt::format("{:%Q %q}", q) == stream.str());
      }
    }

    SECTION("in terms of derived units")
    {
      const energy<scaled_unit<ratio<1, 1, -2>, joule>> q(60);
      stream << q;

      SECTION("iostream")
      {
        CHECK(stream.str() == "60 cJ");
      }

      SECTION("fmt with default format {} on a quantity")
      {
        CHECK(fmt::format("{}", q) == stream.str());
      }

      SECTION("fmt with format {:%Q %q} on a quantity")
      {
        CHECK(fmt::format("{:%Q %q}", q) == stream.str());
      }
    }
  }

  SECTION("quantity with a deduced unit")
  {
    SECTION("coherent derived unit")
    {
      SECTION("acceleration")
      {
        const auto q = 20q_m / 2q_s / 1q_s;
        stream << q;

        SECTION("iostream")
        {
          CHECK(stream.str() == "10 m/s²");
        }

        SECTION("fmt with default format {} on a quantity")
        {
          CHECK(fmt::format("{}", q) == stream.str());
        }

        SECTION("fmt with format {:%Q %q} on a quantity")
        {
          CHECK(fmt::format("{:%Q %q}", q) == stream.str());
        }
      }

      SECTION("volume")
      {
        const auto q = 2q_m * 1q_m * 1q_m;
        stream << q;

        SECTION("iostream")
        {
          CHECK(stream.str() == "2 m³");
        }

        SECTION("fmt with default format {} on a quantity")
        {
          CHECK(fmt::format("{}", q) == stream.str());
        }

        SECTION("fmt with format {:%Q %q} on a quantity")
        {
          CHECK(fmt::format("{:%Q %q}", q) == stream.str());
        }
      }

      SECTION("surface tension")
      {
        const auto q = 20q_N / 2q_m;
        stream << q;

        SECTION("iostream")
        {
          CHECK(stream.str() == "10 N/m");
        }

        SECTION("fmt with default format {} on a quantity")
        {
          CHECK(fmt::format("{}", q) == stream.str());
        }

        SECTION("fmt with format {:%Q %q} on a quantity")
        {
          CHECK(fmt::format("{:%Q %q}", q) == stream.str());
        }
      }
    }

    SECTION("deduced derived unit")
    {
      SECTION("velocity")
      {
        const auto q = 20q_km / 2q_h;
        stream << q;

        SECTION("iostream")
        {
          CHECK(stream.str() == "10 km/h");
        }

        SECTION("fmt with default format {} on a quantity")
        {
          CHECK(fmt::format("{}", q) == stream.str());
        }

        SECTION("fmt with format {:%Q %q} on a quantity")
        {
          CHECK(fmt::format("{:%Q %q}", q) == stream.str());
        }
      }

      SECTION("surface tension")
      {
        struct newton_per_centimetre : deduced_unit<newton_per_centimetre, dim_surface_tension, newton, centimetre> {};
        const surface_tension<newton_per_centimetre> q(123);
        stream << q;

        SECTION("iostream")
        {
          CHECK(stream.str() == "123 N/cm");
        }

        SECTION("fmt with default format {} on a quantity")
        {
          CHECK(fmt::format("{}", q) == stream.str());
        }

        SECTION("fmt with format {:%Q %q} on a quantity")
        {
          CHECK(fmt::format("{:%Q %q}", q) == stream.str());
        }
      }
    }
  }

  SECTION("quantity with a predefined dimension but unknown unit")
  {
    SECTION("unit::ratio as an SI prefix for a dimension with a special symbol")
    {
      const auto q = 4q_N * 2q_cm;
      stream << q;

      SECTION("iostream")
      {
        CHECK(stream.str() == "8 cJ");
      }

      SECTION("fmt with default format {} on a quantity")
      {
        CHECK(fmt::format("{}", q) == stream.str());
      }

      SECTION("fmt with format {:%Q %q} on a quantity")
      {
        CHECK(fmt::format("{:%Q %q}", q) == stream.str());
      }
    }

    SECTION("unit::ratio for a dimension without a special symbol")
    {
      const auto q = 2q_cm * 2q_m * 2q_m;
      stream << q;

      SECTION("iostream")
      {
        CHECK(stream.str() == "8 × 10⁻² m³");
      }

      SECTION("fmt with default format {} on a quantity")
      {
        CHECK(fmt::format("{}", q) == stream.str());
      }

      SECTION("fmt with format {:%Q %q} on a quantity")
      {
        CHECK(fmt::format("{:%Q %q}", q) == stream.str());
      }
    }

    SECTION("unit::ratio::num != 1 && unit::ratio::den == 1")
    {
      const auto q = 4 * 2q_min / (2q_s * 2q_s);
      stream << q;

      SECTION("iostream")
      {
        CHECK(stream.str() == "2 [6 × 10¹] Hz");
      }

      SECTION("fmt with default format {} on a quantity")
      {
        CHECK(fmt::format("{}", q) == stream.str());
      }

      SECTION("fmt with format {:%Q %q} on a quantity")
      {
        CHECK(fmt::format("{:%Q %q}", q) == stream.str());
      }
    }

    SECTION("unit::ratio::num == 1 && unit::ratio::den != 1")
    {
      const auto q = 20q_J / 2q_min;
      stream << q;

      SECTION("iostream")
      {
        CHECK(stream.str() == "10 [1/6 × 10⁻¹] W");
      }

      SECTION("fmt with default format {} on a quantity")
      {
        CHECK(fmt::format("{}", q) == stream.str());
      }

      SECTION("fmt with format {:%Q %q} on a quantity")
      {
        CHECK(fmt::format("{:%Q %q}", q) == stream.str());
      }
    }

    SECTION("unit::ratio::num != 1 && unit::ratio::den != 1")
    {
      const auto q = 60q_kJ / 2q_min;
      stream << q;

      SECTION("iostream")
      {
        CHECK(stream.str() == "30 [1/6 × 10²] W");
      }

      SECTION("fmt with default format {} on a quantity")
      {
        CHECK(fmt::format("{}", q) == stream.str());
      }

      SECTION("fmt with format {:%Q %q} on a quantity")
      {
        CHECK(fmt::format("{:%Q %q}", q) == stream.str());
      }
    }
  }

  SECTION("quantity with an unkown dimension")
  {
    SECTION("unit::ratio::num == 1 && unit::ratio::den == 1")
    {
      SECTION("SI base units")
      {
        const auto q = 2q_s * 2q_m * 2q_kg;
        stream << q;

        SECTION("iostream")
        {
          CHECK(stream.str() == "8 m⋅kg⋅s");
        }

        SECTION("fmt with default format {} on a quantity")
        {
          CHECK(fmt::format("{}", q) == stream.str());
        }

        SECTION("fmt with format {:%Q %q} on a quantity")
        {
          CHECK(fmt::format("{:%Q %q}", q) == stream.str());
        }
      }

      SECTION("CGS base units")
      {
        const auto q = 2q_s * cgs::length<cgs::centimetre>(2) * cgs::mass<cgs::gram>(2);
        stream << q;

        SECTION("iostream")
        {
          CHECK(stream.str() == "8 cm⋅g⋅s");
        }

        SECTION("fmt with default format {} on a quantity")
        {
          CHECK(fmt::format("{}", q) == stream.str());
        }

        SECTION("fmt with format {:%Q %q} on a quantity")
        {
          CHECK(fmt::format("{:%Q %q}", q) == stream.str());
        }
      }
    }

    SECTION("unit::ratio as an SI prefix")
    {
      const auto q = 4q_km * 2q_s;
      stream << q;

      SECTION("iostream")
      {
        CHECK(stream.str() == "8 × 10³ m⋅s");
      }

      SECTION("fmt with default format {} on a quantity")
      {
        CHECK(fmt::format("{}", q) == stream.str());
      }

      SECTION("fmt with format {:%Q %q} on a quantity")
      {
        CHECK(fmt::format("{:%Q %q}", q) == stream.str());
      }
    }

    SECTION("unit::ratio::num != 1 && unit::ratio::den == 1")
    {
      const auto q = 4q_kg * 2q_min / (2q_s * 2q_s);
      stream << q;

      SECTION("iostream")
      {
        CHECK(stream.str() == "2 [6 × 10¹] kg/s");
      }

      SECTION("fmt with default format {} on a quantity")
      {
        CHECK(fmt::format("{}", q) == stream.str());
      }

      SECTION("fmt with format {:%Q %q} on a quantity")
      {
        CHECK(fmt::format("{:%Q %q}", q) == stream.str());
      }
    }

    SECTION("unit::ratio::num == 1 && unit::ratio::den != 1")
    {
      const auto q = 20q_kg / 2q_min;
      stream << q;

      SECTION("iostream")
      {
        CHECK(stream.str() == "10 [1/6 × 10⁻¹] kg/s");
      }

      SECTION("fmt with default format {} on a quantity")
      {
        CHECK(fmt::format("{}", q) == stream.str());
      }

      SECTION("fmt with format {:%Q %q} on a quantity")
      {
        CHECK(fmt::format("{:%Q %q}", q) == stream.str());
      }
    }

      SECTION("CGS base units")
      {
        const auto q = 2q_s * cgs::length<si::metre>(2) * cgs::mass<si::kilogram>(2);
        stream << q;

        SECTION("iostream")
        {
          CHECK(stream.str() == "8 × 10⁵ cm⋅g⋅s");
        }

        SECTION("fmt with default format {} on a quantity")
        {
          CHECK(fmt::format("{}", q) == stream.str());
        }

        SECTION("fmt with format {:%Q %q} on a quantity")
        {
          CHECK(fmt::format("{:%Q %q}", q) == stream.str());
        }
      }

    SECTION("unit::ratio::num != 1 && unit::ratio::den != 1")
    {
      const auto q = 60q_min / 2q_km;
      stream << q;

      SECTION("iostream")
      {
        CHECK(stream.str() == "30 [6 × 10⁻²] 1/m⋅s");
      }

      SECTION("fmt with default format {} on a quantity")
      {
        CHECK(fmt::format("{}", q) == stream.str());
      }

      SECTION("fmt with format {:%Q %q} on a quantity")
      {
        CHECK(fmt::format("{:%Q %q}", q) == stream.str());
      }
    }

    SECTION("exp::num == 1 && exp::den == 1")
    {
      const auto q = 4q_m * 2q_s;
      stream << q;

      SECTION("iostream")
      {
        CHECK(stream.str() == "8 m⋅s");
      }

      SECTION("fmt with default format {} on a quantity")
      {
        CHECK(fmt::format("{}", q) == stream.str());
      }

      SECTION("fmt with format {:%Q %q} on a quantity")
      {
        CHECK(fmt::format("{:%Q %q}", q) == stream.str());
      }
    }

    SECTION("exp::num == 2 && exp::den == 1 for positive exponent")
    {
      const auto q = 4q_m * 2q_s * 2q_s;
      stream << q;

      SECTION("iostream")
      {
        CHECK(stream.str() == "16 m⋅s²");
      }

      SECTION("fmt with default format {} on a quantity")
      {
        CHECK(fmt::format("{}", q) == stream.str());
      }

      SECTION("fmt with format {:%Q %q} on a quantity")
      {
        CHECK(fmt::format("{:%Q %q}", q) == stream.str());
      }
    }

    SECTION("exp::num == 2 && exp::den == 1 for negative exponent (first dimension)")
    {
      const auto q = 8q_s / 2q_m / 2q_m;
      stream << q;

      SECTION("iostream")
      {
        CHECK(stream.str() == "2 1/m²⋅s");
      }

      SECTION("fmt with default format {} on a quantity")
      {
        CHECK(fmt::format("{}", q) == stream.str());
      }

      SECTION("fmt with format {:%Q %q} on a quantity")
      {
        CHECK(fmt::format("{:%Q %q}", q) == stream.str());
      }
    }

    SECTION("exp::num == 2 && exp::den == 1 for negative exponent (not first dimension)")
    {
      const auto q = 8q_m / 2q_kg / 2q_kg;
      stream << q;

      SECTION("iostream")
      {
        CHECK(stream.str() == "2 m/kg²");
      }

      SECTION("fmt with default format {} on a quantity")
      {
        CHECK(fmt::format("{}", q) == stream.str());
      }

      SECTION("fmt with format {:%Q %q} on a quantity")
      {
        CHECK(fmt::format("{:%Q %q}", q) == stream.str());
      }
    }

    SECTION("fractional positive exponent")
    {
      const auto q = sqrt(9q_m);
      stream << q;

      SECTION("iostream")
      {
        CHECK(stream.str() == "3 m^(1/2)");
      }

      SECTION("fmt with default format {} on a quantity")
      {
        CHECK(fmt::format("{}", q) == stream.str());
      }

      SECTION("fmt with format {:%Q %q} on a quantity")
      {
        CHECK(fmt::format("{:%Q %q}", q) == stream.str());
      }
    }

    SECTION("fractional negative exponent")
    {
      const auto q = sqrt(9 / 1q_m);
      stream << q;

      SECTION("iostream")
      {
        CHECK(stream.str() == "3 1/m^(1/2)");
      }

      SECTION("fmt with default format {} on a quantity")
      {
        CHECK(fmt::format("{}", q) == stream.str());
      }

      SECTION("fmt with format {:%Q %q} on a quantity")
      {
        CHECK(fmt::format("{:%Q %q}", q) == stream.str());
      }
    }
  }
}

TEST_CASE("format string with only %Q should print quantity value only", "[text][fmt]")
{
  SECTION("integral representation")
  {
    SECTION("positive value")
    {
      CHECK(fmt::format("{:%Q}", 123q_km_per_h) == "123");
    }

    SECTION("negative value")
    {
      CHECK(fmt::format("{:%Q}", 5q_m - 10q_m) == "-5");
    }
  }

  SECTION("floating-point representation")
  {
    SECTION("positive value")
    {
      CHECK(fmt::format("{:%Q}", 221.q_km / 2q_h) == "110.5");
    }

    SECTION("negative value")
    {
      CHECK(fmt::format("{:%Q}", 3.14q_m - 10q_m) == "-6.86");
    }

    SECTION("nan")
    {
      CHECK(fmt::format("{:%Q}", length<metre>(std::numeric_limits<double>::quiet_NaN())) == "nan");
    }

    SECTION("inf")
    {
      CHECK(fmt::format("{:%Q}", length<metre>(std::numeric_limits<double>::infinity())) == "inf");
    }

    SECTION("-inf")
    {
      CHECK(fmt::format("{:%Q}", length<metre>(-std::numeric_limits<double>::infinity())) == "-inf");
    }
  }
}

TEST_CASE("format string with only %q should print quantity unit symbol only", "[text][fmt]")
{
  CHECK(fmt::format("{:%q}", 123q_km_per_h) == "km/h");
}

TEST_CASE("format string with only %q should print Unicode quantity unit symbol only", "[text][fmt]")
{
  CHECK(fmt::format("{:%Q%q}", 123q_kR) == "123kΩ");
}

TEST_CASE("format string with only %a should print ASCII quantity unit symbol only", "[text][fmt]")
{
  CHECK(fmt::format("{:%Q%a}", 123q_kR) == "123kohm");
}

TEST_CASE("%q an %Q can be put anywhere in a format string", "[text][fmt]")
{
  SECTION("no space")
  {
    CHECK(fmt::format("{:%Q%q}", 123q_km_per_h) == "123km/h");
  }

  SECTION("separator")
  {
    CHECK(fmt::format("{:%Q###%q}", 123q_km_per_h) == "123###km/h");
  }

  SECTION("opposite order")
  {
    CHECK(fmt::format("{:%q %Q}", 123q_km_per_h) == "km/h 123");
  }

  SECTION("tabulator")
  {
    CHECK(fmt::format("{:%Q%t%q}", 123q_km_per_h) == "123\tkm/h");
  }

  SECTION("new line")
  {
    CHECK(fmt::format("{:%Q%n%q}", 123q_km_per_h) == "123\nkm/h");
  }

  SECTION("% sign")
  {
    CHECK(fmt::format("{:%Q%% %q}", 123q_km_per_h) == "123% km/h");
  }
}

TEST_CASE("fill and align specification", "[text][fmt]")
{
  SECTION("default format {} on a quantity")
  {
    CHECK(fmt::format("|{:0}|", 123q_m) == "|123 m|");
    CHECK(fmt::format("|{:10}|", 123q_m) == "|     123 m|");
    CHECK(fmt::format("|{:<10}|", 123q_m) == "|123 m     |");
    CHECK(fmt::format("|{:>10}|", 123q_m) == "|     123 m|");
    CHECK(fmt::format("|{:^10}|", 123q_m) == "|  123 m   |");
    CHECK(fmt::format("|{:*<10}|", 123q_m) == "|123 m*****|");
    CHECK(fmt::format("|{:*>10}|", 123q_m) == "|*****123 m|");
    CHECK(fmt::format("|{:*^10}|", 123q_m) == "|**123 m***|");
  }

  SECTION("full format {:%Q %q} on a quantity")
  {
    CHECK(fmt::format("|{:0%Q%q}|", 123q_m) == "|123m|");
    CHECK(fmt::format("|{:10%Q%q}|", 123q_m) == "|      123m|");
    CHECK(fmt::format("|{:<10%Q%q}|", 123q_m) == "|123m      |");
    CHECK(fmt::format("|{:>10%Q%q}|", 123q_m) == "|      123m|");
    CHECK(fmt::format("|{:^10%Q%q}|", 123q_m) == "|   123m   |");
    CHECK(fmt::format("|{:*<10%Q%q}|", 123q_m) == "|123m******|");
    CHECK(fmt::format("|{:*>10%Q%q}|", 123q_m) == "|******123m|");
    CHECK(fmt::format("|{:*^10%Q%q}|", 123q_m) == "|***123m***|");
  }

  SECTION("value only format {:%Q} on a quantity")
  {
    CHECK(fmt::format("|{:0%Q}|", 123q_m) == "|123|");
    CHECK(fmt::format("|{:10%Q}|", 123q_m) == "|       123|");
    CHECK(fmt::format("|{:<10%Q}|", 123q_m) == "|123       |");
    CHECK(fmt::format("|{:>10%Q}|", 123q_m) == "|       123|");
    CHECK(fmt::format("|{:^10%Q}|", 123q_m) == "|   123    |");
    CHECK(fmt::format("|{:*<10%Q}|", 123q_m) == "|123*******|");
    CHECK(fmt::format("|{:*>10%Q}|", 123q_m) == "|*******123|");
    CHECK(fmt::format("|{:*^10%Q}|", 123q_m) == "|***123****|");
  }

  SECTION("symbol only format {:%q} on a quantity")
  {
    CHECK(fmt::format("|{:0%q}|", 123q_m) == "|m|");
    CHECK(fmt::format("|{:10%q}|", 123q_m) == "|m         |");
    CHECK(fmt::format("|{:<10%q}|", 123q_m) == "|m         |");
    CHECK(fmt::format("|{:>10%q}|", 123q_m) == "|         m|");
    CHECK(fmt::format("|{:^10%q}|", 123q_m) == "|    m     |");
    CHECK(fmt::format("|{:*<10%q}|", 123q_m) == "|m*********|");
    CHECK(fmt::format("|{:*>10%q}|", 123q_m) == "|*********m|");
    CHECK(fmt::format("|{:*^10%q}|", 123q_m) == "|****m*****|");
  }
}

TEST_CASE("sign specification", "[text][fmt]")
{
  length<metre> inf(std::numeric_limits<double>::infinity());
  length<metre> nan(std::numeric_limits<double>::quiet_NaN());

  SECTION("default format {} on a quantity")
  {
    CHECK(fmt::format("{0:},{0:+},{0:-},{0: }", 1q_m) == "1 m,+1 m,1 m, 1 m");
    CHECK(fmt::format("{0:},{0:+},{0:-},{0: }", -1q_m) == "-1 m,-1 m,-1 m,-1 m");
    CHECK(fmt::format("{0:},{0:+},{0:-},{0: }", inf) == "inf m,+inf m,inf m, inf m");
    CHECK(fmt::format("{0:},{0:+},{0:-},{0: }", nan) == "nan m,+nan m,nan m, nan m");
  }

  SECTION("full format {:%Q %q} on a quantity")
  {
    CHECK(fmt::format("{0:%Q%q},{0:+%Q%q},{0:-%Q%q},{0: %Q%q}", 1q_m) == "1m,+1m,1m, 1m");
    CHECK(fmt::format("{0:%Q%q},{0:+%Q%q},{0:-%Q%q},{0: %Q%q}", -1q_m) == "-1m,-1m,-1m,-1m");
    CHECK(fmt::format("{0:%Q%q},{0:+%Q%q},{0:-%Q%q},{0: %Q%q}", inf) == "infm,+infm,infm, infm");
    CHECK(fmt::format("{0:%Q%q},{0:+%Q%q},{0:-%Q%q},{0: %Q%q}", nan) == "nanm,+nanm,nanm, nanm");
  }

  SECTION("value only format {:%Q} on a quantity")
  {
    CHECK(fmt::format("{0:%Q},{0:+%Q},{0:-%Q},{0: %Q}", 1q_m) == "1,+1,1, 1");
    CHECK(fmt::format("{0:%Q},{0:+%Q},{0:-%Q},{0: %Q}", -1q_m) == "-1,-1,-1,-1");
    CHECK(fmt::format("{0:%Q},{0:+%Q},{0:-%Q},{0: %Q}", inf) == "inf,+inf,inf, inf");
    CHECK(fmt::format("{0:%Q},{0:+%Q},{0:-%Q},{0: %Q}", nan) == "nan,+nan,nan, nan");
  }
}

TEST_CASE("sign specification for unit only", "[text][fmt][exception]")
{
  CHECK_THROWS_MATCHES(fmt::format("{:+%q}", 1q_m), fmt::format_error, Message("sign not allowed for a quantity unit"));
  CHECK_THROWS_MATCHES(fmt::format("{:-%q}", 1q_m), fmt::format_error, Message("sign not allowed for a quantity unit"));
}


TEST_CASE("precision specification", "[text][fmt]")
{
  SECTION("default format {} on a quantity")
  {
    CHECK(fmt::format("{:.1}", 1.2345q_m) == "1.2 m");
    CHECK(fmt::format("{:.0}", 1.2345q_m) == "1 m");
    CHECK(fmt::format("{:.2}", 1.2345q_m) == "1.23 m");
    CHECK(fmt::format("{:.3}", 1.2345q_m) == "1.235 m");
    CHECK(fmt::format("{:.4}", 1.2345q_m) == "1.2345 m");
    CHECK(fmt::format("{:.5}", 1.2345q_m) == "1.23450 m");
    CHECK(fmt::format("{:.10}", 1.2345q_m) == "1.2345000000 m");
  }

  SECTION("full format {:%Q %q} on a quantity")
  {
    CHECK(fmt::format("{:.0%Q %q}", 1.2345q_m) == "1 m");
    CHECK(fmt::format("{:.1%Q %q}", 1.2345q_m) == "1.2 m");
    CHECK(fmt::format("{:.2%Q %q}", 1.2345q_m) == "1.23 m");
    CHECK(fmt::format("{:.3%Q %q}", 1.2345q_m) == "1.235 m");
    CHECK(fmt::format("{:.4%Q %q}", 1.2345q_m) == "1.2345 m");
    CHECK(fmt::format("{:.5%Q %q}", 1.2345q_m) == "1.23450 m");
    CHECK(fmt::format("{:.10%Q %q}", 1.2345q_m) == "1.2345000000 m");
  }

  SECTION("value only format {:%Q} on a quantity")
  {
    CHECK(fmt::format("{:.0%Q}", 1.2345q_m) == "1");
    CHECK(fmt::format("{:.1%Q}", 1.2345q_m) == "1.2");
    CHECK(fmt::format("{:.2%Q}", 1.2345q_m) == "1.23");
    CHECK(fmt::format("{:.3%Q}", 1.2345q_m) == "1.235");
    CHECK(fmt::format("{:.4%Q}", 1.2345q_m) == "1.2345");
    CHECK(fmt::format("{:.5%Q}", 1.2345q_m) == "1.23450");
    CHECK(fmt::format("{:.10%Q}", 1.2345q_m) == "1.2345000000");
  }
}

TEST_CASE("precision specification for integral representation should throw", "[text][fmt][exception]")
{
  SECTION("default format {} on a quantity")
  {
    REQUIRE_THROWS_MATCHES(fmt::format("{:.1}", 1q_m), fmt::format_error, Message("precision not allowed for integral quantity representation"));
  }

  SECTION("full format {:%Q %q} on a quantity")
  {
    REQUIRE_THROWS_MATCHES(fmt::format("{:.1%Q %q}", 1q_m), fmt::format_error, Message("precision not allowed for integral quantity representation"));
  }

  SECTION("value only format {:%Q} on a quantity")
  {
    REQUIRE_THROWS_MATCHES(fmt::format("{:.1%Q}", 1q_m), fmt::format_error, Message("precision not allowed for integral quantity representation"));
  }
}

// Restate operator<< definitions in terms of std::format to make I/O manipulators apply to whole objects
// rather than their parts


TEST_CASE("quantity_cast", "[text][ostream]")
{
  std::stringstream stream;

  SECTION("int to double representation")
  {
    const auto q = 121q_km / 2q_h;

    SECTION("original")
    {
      stream << q;
      CHECK(stream.str() == "60 km/h");
    }

    SECTION("int")
    {
      stream << quantity_cast<int>(q);
      CHECK(stream.str() == "60 km/h");
    }

    SECTION("double")
    {
      stream << quantity_cast<double>(q);
      CHECK(stream.str() == "60 km/h");
    }
  }

  SECTION("double to int representation")
  {
    const auto q = 121.q_km / 2q_h;

    SECTION("original")
    {
      stream << q;
      CHECK(stream.str() == "60.5 km/h");
    }

    SECTION("int")
    {
      stream << quantity_cast<int>(q);
      CHECK(stream.str() == "60 km/h");
    }

    SECTION("double")
    {
      stream << quantity_cast<double>(q);
      CHECK(stream.str() == "60.5 km/h");
    }
  }
}
