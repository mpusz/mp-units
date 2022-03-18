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

#include <catch2/catch.hpp>
#include <units/customization_points.h>
#include <units/format.h>
#include <units/generic/dimensionless.h>
#include <units/isq/si/cgs/cgs.h>
#include <units/isq/si/si.h>
#include <units/math.h>  // IWYU pragma: keep
#include <units/quantity_io.h>
#include <iomanip>
#include <limits>
#include <locale>

using namespace units;
using namespace units::isq;
using namespace units::isq::si;
using namespace Catch::Matchers;

TEST_CASE("operator<< on a quantity", "[text][ostream][fmt]")
{
  std::ostringstream os;

  SECTION("quantity with a predefined unit")
  {
    SECTION("integral representation")
    {
      const auto q = 60_q_W;
      os << q;

      SECTION("iostream") { CHECK(os.str() == "60 W"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
    }

    SECTION("floating-point representation")
    {
      const auto q = 1023.5_q_Pa;
      os << q;

      SECTION("iostream") { CHECK(os.str() == "1023.5 Pa"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
    }
  }

  SECTION("quantity with a predefined prefixed unit")
  {
    const auto q = 125_q_us;
    os << q;

    SECTION("iostream") { CHECK(os.str() == "125 µs"); }

    SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

    SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
  }

  SECTION("quantity with a predefined unit + prefix")
  {
    SECTION("in terms of base units")
    {
      const length<scaled_unit<ratio(1, 1, 6), metre>> q(123);
      os << q;

      SECTION("iostream") { CHECK(os.str() == "123 Mm"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
    }

    SECTION("in terms of derived units")
    {
      const energy<scaled_unit<ratio(1, 1, -2), joule>> q(60);
      os << q;

      SECTION("iostream") { CHECK(os.str() == "60 cJ"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
    }
  }

  SECTION("quantity with an alias unit")
  {
    const auto q = 2_q_l;
    os << q;

    SECTION("iostream") { CHECK(os.str() == "2 l"); }

    SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

    SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
  }

  SECTION("quantity with a prefixed alias unit")
  {
    const auto q = 2_q_ml;
    os << q;

    SECTION("iostream") { CHECK(os.str() == "2 ml"); }

    SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

    SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
  }

  SECTION("quantity with a derived unit")
  {
    SECTION("coherent derived unit")
    {
      SECTION("acceleration")
      {
        const auto q = 20_q_m / 2_q_s / 1_q_s;
        os << q;

        SECTION("iostream") { CHECK(os.str() == "10 m/s²"); }

        SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

        SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
      }

      SECTION("volume")
      {
        const auto q = 2_q_m * 1_q_m * 1_q_m;
        os << q;

        SECTION("iostream") { CHECK(os.str() == "2 m³"); }

        SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

        SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
      }

      SECTION("surface tension")
      {
        const auto q = 20_q_N / 2_q_m;
        os << q;

        SECTION("iostream") { CHECK(os.str() == "10 N/m"); }

        SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

        SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
      }
    }

    SECTION("deduced derived unit")
    {
      SECTION("speed")
      {
        const auto q = 20_q_km / 2_q_h;
        os << q;

        SECTION("iostream") { CHECK(os.str() == "10 km/h"); }

        SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

        SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
      }

      SECTION("surface tension")
      {
        struct newton_per_centimetre :
            derived_unit<newton_per_centimetre, si::dim_surface_tension, newton, centimetre> {};
        const surface_tension<newton_per_centimetre> q(123);
        os << q;

        SECTION("iostream") { CHECK(os.str() == "123 N/cm"); }

        SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

        SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
      }
    }
  }

  SECTION("quantity with a predefined dimension but unknown unit")
  {
    SECTION("unit::ratio as an SI prefix for a dimension with a special symbol")
    {
      const auto q = 4_q_N * 2_q_cm;
      os << q;

      SECTION("iostream") { CHECK(os.str() == "8 cJ"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
    }

    SECTION("unit::ratio for a dimension without a special symbol")
    {
      const auto q = 2_q_um * 2_q_cm * 2_q_cm;
      os << q;

      SECTION("iostream") { CHECK(os.str() == "8 × 10⁻¹⁰ m³"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
    }

    SECTION("unit::ratio::num != 1 && unit::ratio::den == 1")
    {
      const auto q = 4 * 2_q_min / (2_q_s * 2_q_s);
      os << q;

      SECTION("iostream") { CHECK(os.str() == "2 [6 × 10¹] Hz"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
    }

    SECTION("unit::ratio::num == 1 && unit::ratio::den != 1")
    {
      const auto q = 20_q_J / 2_q_min;
      os << q;

      SECTION("iostream") { CHECK(os.str() == "10 [1/6 × 10⁻¹] W"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
    }

    SECTION("unit::ratio::num != 1 && unit::ratio::den != 1")
    {
      const auto q = 60_q_kJ / 2_q_min;
      os << q;

      SECTION("iostream") { CHECK(os.str() == "30 [1/6 × 10²] W"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
    }
  }

  SECTION("dimensionless quantity")
  {
    SECTION("one with ratio == 1")
    {
      const auto q = 4_q_m / 2_q_m;
      os << q;

      SECTION("iostream") { CHECK(os.str() == "2"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == "2 "); }
    }

    SECTION("one with ratio.exp != 0")
    {
      const auto q = 4_q_km / 2_q_m;
      os << q;

      SECTION("iostream") { CHECK(os.str() == "2 × 10³"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == "2 × 10³"); }
    }

    SECTION("percents")
    {
#if UNITS_DOWNCAST_MODE == 0
      const auto q = quantity_cast<dim_one, percent>(15._q_m / 100._q_m);
#else
      const auto q = quantity_cast<percent>(15._q_m / 100._q_m);
#endif
      os << q;

      SECTION("iostream") { CHECK(os.str() == "15 %"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
    }
  }

  SECTION("quantity with an unkown dimension")
  {
    SECTION("unit::ratio::num == 1 && unit::ratio::den == 1")
    {
      SECTION("SI base units")
      {
        const auto q = 2_q_s * 2_q_m * 2_q_kg;
        os << q;

        SECTION("iostream") { CHECK(os.str() == "8 m ⋅ kg ⋅ s"); }

        SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

        SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
      }

      SECTION("CGS base units")
      {
        const auto q = 2._q_s * si::cgs::length<si::cgs::centimetre>(2) * si::cgs::mass<si::cgs::gram>(2);
        os << q;

        SECTION("iostream") { CHECK(os.str() == "8 cm ⋅ g ⋅ s"); }

        SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

        SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
      }
    }

    SECTION("unit::ratio as an SI prefix")
    {
      const auto q = 4_q_km * 2_q_s;
      os << q;

      SECTION("iostream") { CHECK(os.str() == "8 × 10³ m ⋅ s"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
    }

    SECTION("unit::ratio::num != 1 && unit::ratio::den == 1")
    {
      const auto q = 4_q_kg * 2_q_min / (2_q_s * 2_q_s);
      os << q;

      SECTION("iostream") { CHECK(os.str() == "2 [6 × 10¹] kg/s"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
    }

    SECTION("unit::ratio::num == 1 && unit::ratio::den != 1")
    {
      const auto q = 20_q_kg / 2_q_min;
      os << q;

      SECTION("iostream") { CHECK(os.str() == "10 [1/6 × 10⁻¹] kg/s"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
    }

    SECTION("CGS base units")
    {
      const auto q = 2._q_s * si::cgs::length<si::metre>(2) * si::cgs::mass<si::kilogram>(2);
      os << q;

      SECTION("iostream") { CHECK(os.str() == "8 × 10⁵ cm ⋅ g ⋅ s"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
    }

    SECTION("unit::ratio::num != 1 && unit::ratio::den != 1")
    {
      const auto q = 60_q_min / 2_q_km;
      os << q;

      SECTION("iostream") { CHECK(os.str() == "30 [6 × 10⁻²] 1/m ⋅ s"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
    }

    SECTION("exponent::num == 1 && exponent::den == 1")
    {
      const auto q = 4_q_m * 2_q_s;
      os << q;

      SECTION("iostream") { CHECK(os.str() == "8 m ⋅ s"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
    }

    SECTION("exponent::num == 2 && exponent::den == 1 for positive exponent")
    {
      const auto q = 4_q_m * 2_q_s * 2_q_s;
      os << q;

      SECTION("iostream") { CHECK(os.str() == "16 m ⋅ s²"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
    }

    SECTION("exponent::num == 2 && exponent::den == 1 for negative exponent (first dimension)")
    {
      const auto q = 8_q_s / 2_q_m / 2_q_m;
      os << q;

      SECTION("iostream") { CHECK(os.str() == "2 1/m² ⋅ s"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
    }

    SECTION("exponent::num == 2 && exponent::den == 1 for negative exponent (not first dimension)")
    {
      const auto q = 8_q_m / 2_q_kg / 2_q_kg;
      os << q;

      SECTION("iostream") { CHECK(os.str() == "2 m/kg²"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
    }

    SECTION("fractional positive exponent")
    {
      const auto q = sqrt(9_q_m);
      os << q;

      SECTION("iostream") { CHECK(os.str() == "3 m^(1/2)"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
    }

    SECTION("fractional negative exponent")
    {
      const auto q = sqrt(9 / 1_q_m);
      os << q;

      SECTION("iostream") { CHECK(os.str() == "3 1/m^(1/2)"); }

      SECTION("fmt with default format {} on a quantity") { CHECK(STD_FMT::format("{}", q) == os.str()); }

      SECTION("fmt with format {:%Q %q} on a quantity") { CHECK(STD_FMT::format("{:%Q %q}", q) == os.str()); }
    }
  }
}

TEST_CASE("format string with only %Q should print quantity value only", "[text][fmt]")
{
  SECTION("integral representation")
  {
    SECTION("positive value") { CHECK(STD_FMT::format("{:%Q}", 123_q_km_per_h) == "123"); }

    SECTION("negative value") { CHECK(STD_FMT::format("{:%Q}", 5_q_m - 10_q_m) == "-5"); }
  }

  SECTION("floating-point representation")
  {
    SECTION("positive value") { CHECK(STD_FMT::format("{:%Q}", 221._q_km / 2_q_h) == "110.5"); }

    SECTION("negative value") { CHECK(STD_FMT::format("{:%Q}", 3.14_q_m - 10_q_m) == "-6.86"); }

    SECTION("nan")
    {
      CHECK(STD_FMT::format("{:%Q}", length<metre>(std::numeric_limits<double>::quiet_NaN())) == "nan");
    }

    SECTION("inf") { CHECK(STD_FMT::format("{:%Q}", length<metre>(std::numeric_limits<double>::infinity())) == "inf"); }

    SECTION("-inf")
    {
      CHECK(STD_FMT::format("{:%Q}", length<metre>(-std::numeric_limits<double>::infinity())) == "-inf");
    }
  }
}

TEST_CASE("format string with only %q should print quantity unit symbol only", "[text][fmt]")
{
  SECTION("standard format for a unit without Unicode symbols")
  {
    CHECK(STD_FMT::format("{:%q}", 123_q_km_per_h) == "km/h");
  }

  SECTION("ASCII format for a unit without Unicode symbols")
  {
    CHECK(STD_FMT::format("{:%Aq}", 123_q_km_per_h) == "km/h");
  }

  SECTION("standard format for a unit with Unicode symbols")
  {
    SECTION("Unicode signs in a unit symbol") { CHECK(STD_FMT::format("{:%q}", 123_q_kR) == "kΩ"); }

    SECTION("Unicode signs in a unit symbol prefix") { CHECK(STD_FMT::format("{:%q}", 123_q_uV) == "µV"); }
  }

  SECTION("ASCII format for a unit with Unicode symbols")
  {
    SECTION("Unicode signs in a unit symbol") { CHECK(STD_FMT::format("{:%Aq}", 123_q_kR) == "kohm"); }

    SECTION("Unicode signs in a unit symbol prefix") { CHECK(STD_FMT::format("{:%Aq}", 123_q_uV) == "uV"); }
  }
}

TEST_CASE("%q and %Q can be put anywhere in a format string", "[text][fmt]")
{
  SECTION("no space") { CHECK(STD_FMT::format("{:%Q%q}", 123_q_km_per_h) == "123km/h"); }

  SECTION("separator") { CHECK(STD_FMT::format("{:%Q###%q}", 123_q_km_per_h) == "123###km/h"); }

  SECTION("opposite order") { CHECK(STD_FMT::format("{:%q %Q}", 123_q_km_per_h) == "km/h 123"); }

  SECTION("tabulator") { CHECK(STD_FMT::format("{:%Q%t%q}", 123_q_km_per_h) == "123\tkm/h"); }

  SECTION("new line") { CHECK(STD_FMT::format("{:%Q%n%q}", 123_q_km_per_h) == "123\nkm/h"); }

  SECTION("% sign") { CHECK(STD_FMT::format("{:%Q%% %q}", 123_q_km_per_h) == "123% km/h"); }
}

TEST_CASE("fill and align specification", "[text][fmt][ostream]")
{
  SECTION("ostream")
  {
    std::ostringstream os;

    SECTION("width = 10")
    {
      os << "|" << std::setw(10) << 123_q_m << "|";
      CHECK(os.str() == "|     123 m|");
    }

    SECTION("width = 10, align = right")
    {
      os << "|" << std::setw(10) << std::right << 123_q_m << "|";
      CHECK(os.str() == "|     123 m|");
    }

    SECTION("width = 10, align = left")
    {
      os << "|" << std::setw(10) << std::left << 123_q_m << "|";
      CHECK(os.str() == "|123 m     |");
    }

    SECTION("width = 10, fill = *")
    {
      os << "|" << std::setw(10) << std::setfill('*') << 123_q_m << "|";
      CHECK(os.str() == "|*****123 m|");
    }

    SECTION("width = 10, fill = *, align = right")
    {
      os << "|" << std::setw(10) << std::setfill('*') << std::right << 123_q_m << "|";
      CHECK(os.str() == "|*****123 m|");
    }

    SECTION("width = 10, fill = *, align = left")
    {
      os << "|" << std::setw(10) << std::setfill('*') << std::left << 123_q_m << "|";
      CHECK(os.str() == "|123 m*****|");
    }
  }

  SECTION("default format {} on a quantity")
  {
    CHECK(STD_FMT::format("|{:0}|", 123_q_m) == "|123 m|");
    CHECK(STD_FMT::format("|{:10}|", 123_q_m) == "|     123 m|");
    CHECK(STD_FMT::format("|{:<10}|", 123_q_m) == "|123 m     |");
    CHECK(STD_FMT::format("|{:>10}|", 123_q_m) == "|     123 m|");
    CHECK(STD_FMT::format("|{:^10}|", 123_q_m) == "|  123 m   |");
    CHECK(STD_FMT::format("|{:*<10}|", 123_q_m) == "|123 m*****|");
    CHECK(STD_FMT::format("|{:*>10}|", 123_q_m) == "|*****123 m|");
    CHECK(STD_FMT::format("|{:*^10}|", 123_q_m) == "|**123 m***|");
  }

  SECTION("full format {:%Q %q} on a quantity")
  {
    CHECK(STD_FMT::format("|{:0%Q%q}|", 123_q_m) == "|123m|");
    CHECK(STD_FMT::format("|{:10%Q%q}|", 123_q_m) == "|      123m|");
    CHECK(STD_FMT::format("|{:<10%Q%q}|", 123_q_m) == "|123m      |");
    CHECK(STD_FMT::format("|{:>10%Q%q}|", 123_q_m) == "|      123m|");
    CHECK(STD_FMT::format("|{:^10%Q%q}|", 123_q_m) == "|   123m   |");
    CHECK(STD_FMT::format("|{:*<10%Q%q}|", 123_q_m) == "|123m******|");
    CHECK(STD_FMT::format("|{:*>10%Q%q}|", 123_q_m) == "|******123m|");
    CHECK(STD_FMT::format("|{:*^10%Q%q}|", 123_q_m) == "|***123m***|");
  }

  SECTION("value only format {:%Q} on a quantity")
  {
    CHECK(STD_FMT::format("|{:0%Q}|", 123_q_m) == "|123|");
    CHECK(STD_FMT::format("|{:10%Q}|", 123_q_m) == "|       123|");
    CHECK(STD_FMT::format("|{:<10%Q}|", 123_q_m) == "|123       |");
    CHECK(STD_FMT::format("|{:>10%Q}|", 123_q_m) == "|       123|");
    CHECK(STD_FMT::format("|{:^10%Q}|", 123_q_m) == "|   123    |");
    CHECK(STD_FMT::format("|{:*<10%Q}|", 123_q_m) == "|123*******|");
    CHECK(STD_FMT::format("|{:*>10%Q}|", 123_q_m) == "|*******123|");
    CHECK(STD_FMT::format("|{:*^10%Q}|", 123_q_m) == "|***123****|");
  }

  SECTION("symbol only format {:%q} on a quantity")
  {
    CHECK(STD_FMT::format("|{:0%q}|", 123_q_m) == "|m|");
    CHECK(STD_FMT::format("|{:10%q}|", 123_q_m) == "|m         |");
    CHECK(STD_FMT::format("|{:<10%q}|", 123_q_m) == "|m         |");
    CHECK(STD_FMT::format("|{:>10%q}|", 123_q_m) == "|         m|");
    CHECK(STD_FMT::format("|{:^10%q}|", 123_q_m) == "|    m     |");
    CHECK(STD_FMT::format("|{:*<10%q}|", 123_q_m) == "|m*********|");
    CHECK(STD_FMT::format("|{:*>10%q}|", 123_q_m) == "|*********m|");
    CHECK(STD_FMT::format("|{:*^10%q}|", 123_q_m) == "|****m*****|");
  }
}

TEST_CASE("sign specification", "[text][fmt]")
{
  length<metre> inf(std::numeric_limits<double>::infinity());
  length<metre> nan(std::numeric_limits<double>::quiet_NaN());

  SECTION("full format {:%Q %q} on a quantity")
  {
    CHECK(STD_FMT::format("{0:%Q%q},{0:%+Q%q},{0:%-Q%q},{0:% Q%q}", 1_q_m) == "1m,+1m,1m, 1m");
    CHECK(STD_FMT::format("{0:%Q%q},{0:%+Q%q},{0:%-Q%q},{0:% Q%q}", -1_q_m) == "-1m,-1m,-1m,-1m");
    CHECK(STD_FMT::format("{0:%Q%q},{0:%+Q%q},{0:%-Q%q},{0:% Q%q}", inf) == "infm,+infm,infm, infm");
    CHECK(STD_FMT::format("{0:%Q%q},{0:%+Q%q},{0:%-Q%q},{0:% Q%q}", nan) == "nanm,+nanm,nanm, nanm");
  }

  SECTION("value only format {:%Q} on a quantity")
  {
    CHECK(STD_FMT::format("{0:%Q},{0:%+Q},{0:%-Q},{0:% Q}", 1_q_m) == "1,+1,1, 1");
    CHECK(STD_FMT::format("{0:%Q},{0:%+Q},{0:%-Q},{0:% Q}", -1_q_m) == "-1,-1,-1,-1");
    CHECK(STD_FMT::format("{0:%Q},{0:%+Q},{0:%-Q},{0:% Q}", inf) == "inf,+inf,inf, inf");
    CHECK(STD_FMT::format("{0:%Q},{0:%+Q},{0:%-Q},{0:% Q}", nan) == "nan,+nan,nan, nan");
  }
}

TEST_CASE("precision specification", "[text][fmt]")
{
  SECTION("full format {:%Q %q} on a quantity")
  {
    CHECK(STD_FMT::format("{:%.0Q %q}", 1.2345_q_m) == "1 m");
    CHECK(STD_FMT::format("{:%.1Q %q}", 1.2345_q_m) == "1.2 m");
    CHECK(STD_FMT::format("{:%.2Q %q}", 1.2345_q_m) == "1.23 m");
#ifdef UNITS_COMP_MSVC
    CHECK(STD_FMT::format("{:%.3Q %q}", 1.2345_q_m) == "1.234 m");
#else
    CHECK(STD_FMT::format("{:%.3Q %q}", 1.2345_q_m) == "1.235 m");
#endif
    CHECK(STD_FMT::format("{:%.4Q %q}", 1.2345_q_m) == "1.2345 m");
    CHECK(STD_FMT::format("{:%.5Q %q}", 1.2345_q_m) == "1.23450 m");
    CHECK(STD_FMT::format("{:%.10Q %q}", 1.2345_q_m) == "1.2345000000 m");
  }

  SECTION("value only format {:%Q} on a quantity")
  {
    CHECK(STD_FMT::format("{:%.0Q}", 1.2345_q_m) == "1");
    CHECK(STD_FMT::format("{:%.1Q}", 1.2345_q_m) == "1.2");
    CHECK(STD_FMT::format("{:%.2Q}", 1.2345_q_m) == "1.23");
#ifdef UNITS_COMP_MSVC
    CHECK(STD_FMT::format("{:%.3Q}", 1.2345_q_m) == "1.234");
#else
    CHECK(STD_FMT::format("{:%.3Q}", 1.2345_q_m) == "1.235");
#endif
    CHECK(STD_FMT::format("{:%.4Q}", 1.2345_q_m) == "1.2345");
    CHECK(STD_FMT::format("{:%.5Q}", 1.2345_q_m) == "1.23450");
    CHECK(STD_FMT::format("{:%.10Q}", 1.2345_q_m) == "1.2345000000");
  }
}

TEST_CASE("precision specification for integral representation should throw", "[text][fmt][exception]")
{
  SECTION("full format {:%Q %q} on a quantity")
  {
    REQUIRE_THROWS_MATCHES(STD_FMT::vformat("{:%.1Q %q}", STD_FMT::make_format_args(1_q_m)), STD_FMT::format_error,
                           Message("precision not allowed for integral quantity representation"));
  }

  SECTION("value only format {:%Q} on a quantity")
  {
    REQUIRE_THROWS_MATCHES(STD_FMT::vformat("{:%.1Q}", STD_FMT::make_format_args(1_q_m)), STD_FMT::format_error,
                           Message("precision not allowed for integral quantity representation"));
  }
}

TEST_CASE("type specification", "[text][fmt]")
{
  SECTION("full format {:%Q %q} on a quantity")
  {
    CHECK(STD_FMT::format("{:%bQ %q}", 42_q_m) == "101010 m");
    CHECK(STD_FMT::format("{:%BQ %q}", 42_q_m) == "101010 m");
    CHECK(STD_FMT::format("{:%dQ %q}", 42_q_m) == "42 m");
    CHECK(STD_FMT::format("{:%oQ %q}", 42_q_m) == "52 m");
    CHECK(STD_FMT::format("{:%xQ %q}", 42_q_m) == "2a m");
    CHECK(STD_FMT::format("{:%XQ %q}", 42_q_m) == "2A m");

#ifdef UNITS_COMP_MSVC
#if UNITS_USE_LIBFMT
    CHECK(STD_FMT::format("{:%aQ %q}", 1.2345678_q_m) == "0x1.3c0ca2a5b1d5dp+0 m");
    CHECK(STD_FMT::format("{:%.3aQ %q}", 1.2345678_q_m) == "0x1.3c1p+0 m");
    CHECK(STD_FMT::format("{:%AQ %q}", 1.2345678_q_m) == "0X1.3C0CA2A5B1D5DP+0 m");
    CHECK(STD_FMT::format("{:%.3AQ %q}", 1.2345678_q_m) == "0X1.3C1P+0 m");
#else
    CHECK(STD_FMT::format("{:%aQ %q}", 1.2345678_q_m) == "1.3c0ca2a5b1d5dp+0 m");
    CHECK(STD_FMT::format("{:%.3aQ %q}", 1.2345678_q_m) == "1.3c1p+0 m");
    CHECK(STD_FMT::format("{:%AQ %q}", 1.2345678_q_m) == "1.3C0CA2A5B1D5DP+0 m");
    CHECK(STD_FMT::format("{:%.3AQ %q}", 1.2345678_q_m) == "1.3C1P+0 m");
#endif  // UNITS_USE_LIBFMT
#else
    CHECK(STD_FMT::format("{:%aQ %q}", 1.2345678_q_m) == "0x9.e065152d8eae841p-3 m");
    CHECK(STD_FMT::format("{:%.3aQ %q}", 1.2345678_q_m) == "0x9.e06p-3 m");
    CHECK(STD_FMT::format("{:%AQ %q}", 1.2345678_q_m) == "0X9.E065152D8EAE841P-3 m");
    CHECK(STD_FMT::format("{:%.3AQ %q}", 1.2345678_q_m) == "0X9.E06P-3 m");
#endif
    CHECK(STD_FMT::format("{:%eQ %q}", 1.2345678_q_m) == "1.234568e+00 m");
    CHECK(STD_FMT::format("{:%.3eQ %q}", 1.2345678_q_m) == "1.235e+00 m");
    CHECK(STD_FMT::format("{:%EQ %q}", 1.2345678_q_m) == "1.234568E+00 m");
    CHECK(STD_FMT::format("{:%.3EQ %q}", 1.2345678_q_m) == "1.235E+00 m");
    CHECK(STD_FMT::format("{:%gQ %q}", 1.2345678_q_m) == "1.23457 m");
    CHECK(STD_FMT::format("{:%gQ %q}", 1.2345678e8_q_m) == "1.23457e+08 m");
    CHECK(STD_FMT::format("{:%.3gQ %q}", 1.2345678_q_m) == "1.23 m");
    CHECK(STD_FMT::format("{:%.3gQ %q}", 1.2345678e8_q_m) == "1.23e+08 m");
    CHECK(STD_FMT::format("{:%GQ %q}", 1.2345678_q_m) == "1.23457 m");
    CHECK(STD_FMT::format("{:%GQ %q}", 1.2345678e8_q_m) == "1.23457E+08 m");
    CHECK(STD_FMT::format("{:%.3GQ %q}", 1.2345678_q_m) == "1.23 m");
    CHECK(STD_FMT::format("{:%.3GQ %q}", 1.2345678e8_q_m) == "1.23E+08 m");
  }

  SECTION("value only format {:%Q} on a quantity")
  {
    CHECK(STD_FMT::format("{:%bQ}", 42_q_m) == "101010");
    CHECK(STD_FMT::format("{:%BQ}", 42_q_m) == "101010");
    CHECK(STD_FMT::format("{:%dQ}", 42_q_m) == "42");
    CHECK(STD_FMT::format("{:%oQ}", 42_q_m) == "52");
    CHECK(STD_FMT::format("{:%xQ}", 42_q_m) == "2a");
    CHECK(STD_FMT::format("{:%XQ}", 42_q_m) == "2A");

#ifdef UNITS_COMP_MSVC
#if UNITS_USE_LIBFMT
    CHECK(STD_FMT::format("{:%aQ}", 1.2345678_q_m) == "0x1.3c0ca2a5b1d5dp+0");
    CHECK(STD_FMT::format("{:%.3aQ}", 1.2345678_q_m) == "0x1.3c1p+0");
    CHECK(STD_FMT::format("{:%AQ}", 1.2345678_q_m) == "0X1.3C0CA2A5B1D5DP+0");
    CHECK(STD_FMT::format("{:%.3AQ}", 1.2345678_q_m) == "0X1.3C1P+0");
#else
    CHECK(STD_FMT::format("{:%aQ}", 1.2345678_q_m) == "1.3c0ca2a5b1d5dp+0");
    CHECK(STD_FMT::format("{:%.3aQ}", 1.2345678_q_m) == "1.3c1p+0");
    CHECK(STD_FMT::format("{:%AQ}", 1.2345678_q_m) == "1.3C0CA2A5B1D5DP+0");
    CHECK(STD_FMT::format("{:%.3AQ}", 1.2345678_q_m) == "1.3C1P+0");
#endif  // UNITS_USE_LIBFMT
#else
    CHECK(STD_FMT::format("{:%aQ}", 1.2345678_q_m) == "0x9.e065152d8eae841p-3");
    CHECK(STD_FMT::format("{:%.3aQ}", 1.2345678_q_m) == "0x9.e06p-3");
    CHECK(STD_FMT::format("{:%AQ}", 1.2345678_q_m) == "0X9.E065152D8EAE841P-3");
    CHECK(STD_FMT::format("{:%.3AQ}", 1.2345678_q_m) == "0X9.E06P-3");
#endif
    CHECK(STD_FMT::format("{:%eQ}", 1.2345678_q_m) == "1.234568e+00");
    CHECK(STD_FMT::format("{:%.3eQ}", 1.2345678_q_m) == "1.235e+00");
    CHECK(STD_FMT::format("{:%EQ}", 1.2345678_q_m) == "1.234568E+00");
    CHECK(STD_FMT::format("{:%.3EQ}", 1.2345678_q_m) == "1.235E+00");
    CHECK(STD_FMT::format("{:%gQ}", 1.2345678_q_m) == "1.23457");
    CHECK(STD_FMT::format("{:%gQ}", 1.2345678e8_q_m) == "1.23457e+08");
    CHECK(STD_FMT::format("{:%.3gQ}", 1.2345678_q_m) == "1.23");
    CHECK(STD_FMT::format("{:%.3gQ}", 1.2345678e8_q_m) == "1.23e+08");
    CHECK(STD_FMT::format("{:%GQ}", 1.2345678_q_m) == "1.23457");
    CHECK(STD_FMT::format("{:%GQ}", 1.2345678e8_q_m) == "1.23457E+08");
    CHECK(STD_FMT::format("{:%.3GQ}", 1.2345678_q_m) == "1.23");
    CHECK(STD_FMT::format("{:%.3GQ}", 1.2345678e8_q_m) == "1.23E+08");
  }
}

TEST_CASE("different base types with the # specifier", "[text][fmt]")
{
  SECTION("full format {:%Q %q} on a quantity")
  {
    CHECK(STD_FMT::format("{:%#bQ %q}", 42_q_m) == "0b101010 m");
    CHECK(STD_FMT::format("{:%#BQ %q}", 42_q_m) == "0B101010 m");
    CHECK(STD_FMT::format("{:%#oQ %q}", 42_q_m) == "052 m");
    CHECK(STD_FMT::format("{:%#xQ %q}", 42_q_m) == "0x2a m");
    CHECK(STD_FMT::format("{:%#XQ %q}", 42_q_m) == "0X2A m");
  }

  SECTION("value only format {:%Q} on a quantity")
  {
    CHECK(STD_FMT::format("{:%#bQ}", 42_q_m) == "0b101010");
    CHECK(STD_FMT::format("{:%#BQ}", 42_q_m) == "0B101010");
    CHECK(STD_FMT::format("{:%#oQ}", 42_q_m) == "052");
    CHECK(STD_FMT::format("{:%#xQ}", 42_q_m) == "0x2a");
    CHECK(STD_FMT::format("{:%#XQ}", 42_q_m) == "0X2A");
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
    CHECK(STD_FMT::format(grp2, "{:%LQ %q}", 299792458_q_m_per_s) == "2_99_79_24_58 m/s");
    CHECK(STD_FMT::format(grp3, "{:%LQ %q}", 299792458_q_m_per_s) == "299'792'458 m/s");
  }
}

TEST_CASE("quantity_cast", "[text][ostream]")
{
  std::ostringstream os;

  SECTION("int to double representation")
  {
    const auto q = 121_q_km / 2_q_h;

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
    const auto q = 121._q_km / 2_q_h;

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
