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

#include <units/time.h>
#include <units/length.h>
#include <units/mass.h>
#include <units/current.h>
#include <units/current.h>
#include <units/temperature.h>
#include <units/substance.h>
#include <units/luminous_intensity.h>

#include <units/frequency.h>
#include <units/velocity.h>
#include <units/area.h>

#include <utility>

namespace {

  using namespace units;

  /* ************** BASE DIMENSIONS **************** */

  // time

  static_assert(1_h == 3600_s);

  // length

  static_assert(1_km == 1000_m);
  static_assert(1_m == 100_cm);
  static_assert(1_m == 1000_mm);
  static_assert(1_km + 1_m == 1001_m);
  static_assert(10_km / 5_km == 2);
  static_assert(10_km / 2 == 5_km);

  static_assert(1_yd == 0.9144_m);
  static_assert(1_yd == 3_ft);
  static_assert(1_ft == 12_in);
  static_assert(1_mi == 1760_yd);

//  static_assert(5_in + 8_cm == 207_mm);



  /* ************** DERIVED DIMENSIONS **************** */

  // frequency

  static_assert(2 / 1_s == 2_Hz);
  static_assert(1000 / 1_s == 1_kHz);
  static_assert(1 / 1_ms == 1_kHz);
  static_assert(3.2_GHz == 3'200'000'000_Hz);

  // velocity

  static_assert(std::is_same_v<decltype(1_km / 1_s), velocity<unit<dimension_velocity, std::ratio<1000, 1>>, std::int64_t>>);

  static_assert(10_m / 5_s == 2_mps);
  static_assert(10 / 5_s * 1_m == 2_mps);
  static_assert(1_km / 1_s == 1000_mps);
  // static_assert(1_km / 1_h == 1_kmph);  // should not compile
  static_assert(1.0_km / 1_h == 1_kmph);
  static_assert(1000.0_m / 3600.0_s == 1_kmph);

  static_assert(10.0_mi / 2_h == 5_mph);

  static_assert(2_kmph * 2_h == 4_km);
  // static_assert(2_kmph * 15_min == 500_m); // should not compile
  static_assert(2_kmph * 15.0_min == 500_m);
  static_assert(2.0_kmph * 15_min == 500_m);

  static_assert(2_km / 2_kmph == 1_h);
  // static_assert(2000_m / 2_kmph == 1_h); // should not compile
  static_assert(quantity_cast<length<kilometer, int>>(2000_m) / 2_kmph == 1_h);

  // area

  static_assert(1_m * 1_m == 1_sq_m);
  static_assert(10_km * 10_km == 100_sq_km);
  static_assert(1_sq_m == 10'000_sq_cm);

}  // namespace
