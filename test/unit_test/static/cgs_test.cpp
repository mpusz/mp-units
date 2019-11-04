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

#include <units/dimensions/acceleration.h>
#include <units/dimensions/power.h>
#include <units/dimensions/pressure.h>
#include <units/math.h>

namespace cgs {

  using units::centimetre;
  using units::gram;
  using units::second;
  struct centimetre_per_second : units::deduced_derived_unit<centimetre_per_second, units::velocity, centimetre, second> {};
  struct gal : units::named_deduced_derived_unit<gal, "Gal", units::acceleration, centimetre, second> {};
  struct dyne : units::named_deduced_derived_unit<dyne, "dyn", units::force, centimetre, gram, second> {};
  struct erg : units::named_deduced_derived_unit<erg, "erg", units::energy, centimetre, gram, second> {};
  struct ergps : units::named_deduced_derived_unit<ergps, "erg/s", units::power, centimetre, gram, second> {};   // TODO make it work for erg and non-named
  struct barye : units::named_deduced_derived_unit<barye, "Ba", units::pressure, centimetre, gram, second> {};


  inline namespace literals {

    using namespace units::literals;

    constexpr auto operator""cmps(unsigned long long l) { return units::quantity<centimetre_per_second, std::int64_t>(l); }
    constexpr auto operator""cmps(long double l) { return units::quantity<centimetre_per_second, long double>(l); }
    constexpr auto operator""Gal(unsigned long long l) { return units::quantity<gal, std::int64_t>(l); }
    constexpr auto operator""Gal(long double l) { return units::quantity<gal, long double>(l); }
    constexpr auto operator""dyn(unsigned long long l) { return units::quantity<dyne, std::int64_t>(l); }
    constexpr auto operator""dyn(long double l) { return units::quantity<dyne, long double>(l); }
    constexpr auto operator""_erg(unsigned long long l) { return units::quantity<erg, std::int64_t>(l); }
    constexpr auto operator""_erg(long double l) { return units::quantity<erg, long double>(l); }
    constexpr auto operator""_ergps(unsigned long long l) { return units::quantity<ergps, std::int64_t>(l); }
    constexpr auto operator""_ergps(long double l) { return units::quantity<ergps, long double>(l); }
    constexpr auto operator""Ba(unsigned long long l) { return units::quantity<barye, std::int64_t>(l); }
    constexpr auto operator""Ba(long double l) { return units::quantity<barye, long double>(l); }

  }  // namespace literals

}

namespace {

  using namespace cgs::literals;

  static_assert(100cm == 1m);
  static_assert(1'000g == 1kg);
  static_assert(100cmps == 1mps);
  static_assert(100Gal == 1mps_sq);
  static_assert(100'000dyn == 1N);
  static_assert(10'000'000_erg == 1_J);
  static_assert(10'000'000_ergps == 1W);
  static_assert(10Ba == 1Pa);

}