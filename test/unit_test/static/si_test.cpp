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

#include <units/physical/si/acceleration.h>
#include <units/physical/si/area.h>
#include <units/physical/si/capacitance.h>
#include <units/physical/si/current.h>
#include <units/physical/si/electric_charge.h>
#include <units/physical/si/energy.h>
#include <units/physical/si/force.h>
#include <units/physical/si/frequency.h>
#include <units/physical/si/length.h>
#include <units/physical/si/luminous_intensity.h>
#include <units/physical/si/mass.h>
#include <units/physical/si/power.h>
#include <units/physical/si/pressure.h>
#include <units/physical/si/substance.h>
#include <units/physical/si/surface_tension.h>
#include <units/physical/si/temperature.h>
#include <units/physical/si/time.h>
#include <units/physical/si/velocity.h>
#include <units/physical/si/voltage.h>
#include <units/physical/si/volume.h>
#include <utility>

namespace {

  using namespace units;
  using namespace units::si;

  /* ************** BASE DIMENSIONS **************** */

  // length

  static_assert(1km == 1000m);
  static_assert(1m == 100cm);
  static_assert(1m == 1000mm);
  static_assert(1km + 1m == 1001m);
  static_assert(10km / 5km == 2);
  static_assert(100mm / 5cm == 2);
  static_assert(10km / 2 == 5km);

  static_assert(1yd == 0.9144m);
  static_assert(1yd == 3ft);
  static_assert(1ft == 12in);
  static_assert(1mi == 1760yd);

  static_assert(5in + 8cm == 207mm);

  static_assert(millimetre::symbol == "mm");
  static_assert(centimetre::symbol == "cm");
  static_assert(kilometre::symbol == "km");

  // mass

  static_assert(1kg == 1000g);

  static_assert(kilogram::symbol == "kg");

  // time

  static_assert(1h == 3600s);

  static_assert(nanosecond::symbol == "ns");
  static_assert(microsecond::symbol == "µs");
  static_assert(millisecond::symbol == "ms");

  // current

  // temperature

  // substance

  // luminous intensity


  /* ************** DERIVED DIMENSIONS WITH NAMED UNITS **************** */

  // frequency

  static_assert(2 / 1s == 2Hz);
  static_assert(120 / 1min == 2Hz);
  static_assert(1000 / 1s == 1kHz);
  static_assert(1 / 1ms == 1kHz);
  static_assert(3.2GHz == 3'200'000'000Hz);
  static_assert(10Hz * 1min == 600);
  static_assert(2 / 1Hz == 2s);

  static_assert(millihertz::symbol == "mHz");
  static_assert(kilohertz::symbol == "kHz");
  static_assert(megahertz::symbol == "MHz");
  static_assert(gigahertz::symbol == "GHz");
  static_assert(terahertz::symbol == "THz");

  // force
  static_assert(10kg * 10mps_sq == 100N);

  // pressure

  static_assert(10N / 10sq_m == 1Pa);

  // energy

  static_assert(10N * 10m == 100_J);
  static_assert(10Pa * 10cub_m == 100_J);

  // power

  static_assert(10_J / 10s == 1W);

  // electric charge

  static_assert(10A * 10s == 100C);

  // voltage

  static_assert(10W / 10A == 1V);
  static_assert(10_J / 10C == 1V);

  // capacitance

  static_assert(10C / 10V == 1F);

  /* ************** DERIVED DIMENSIONS IN TERMS OF BASE UNITS **************** */

  // velocity

  static_assert(std::is_same_v<decltype(1km / 1s), quantity<si::dim_velocity, scaled_unit<metre_per_second, ratio<1000>>, std::int64_t>>);

  static_assert(10m / 5s == 2mps);
  static_assert(10 / 5s * 1m == 2mps);
  static_assert(1km / 1s == 1000mps);
  // static_assert(1km / 1h == 1kmph);  // should not compile
  static_assert(1.0km / 1h == 1kmph);
  static_assert(1000.0m / 3600.0s == 1kmph);

  static_assert(10.0mi / 2h == 5mph);

  static_assert(2kmph * 2h == 4km);
  // static_assert(2kmph * 15min == 500m); // should not compile
  static_assert(2kmph * 15.0min == 500m);
  static_assert(2.0kmph * 15min == 500m);

  static_assert(2km / 2kmph == 1h);
  // static_assert(2000m / 2kmph == 1h); // should not compile
  static_assert(quantity_cast<quantity<si::dim_length, kilometre, int>>(2000m) / 2kmph == 1h);

//  static_assert(metre_per_second::symbol == basic_fixed_string("m/s"));
  // static_assert(kilometre_per_hour::symbol == basic_fixed_string("km/h"));


  // acceleration

  static_assert(10mps / 10s == 1mps_sq);

  // area

  static_assert(1m * 1m == 1sq_m);
  static_assert(10km * 10km == 100sq_km);
  static_assert(1sq_m == 10'000sq_cm);

  // volume

  static_assert(1m * 1m * 1m == 1cub_m);
  static_assert(10sq_m * 10m == 100cub_m);
  static_assert(10km * 10km * 10km == 1000cub_km);
  static_assert(1cub_m == 1'000'000cub_cm);


  /* ************** DERIVED DIMENSIONS IN TERMS OF OTHER UNITS **************** */

  static_assert(10N / 2m == 5Npm);

}  // namespace
