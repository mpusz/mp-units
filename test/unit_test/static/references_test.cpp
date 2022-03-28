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

#include "test_tools.h"
#include <units/bits/external/hacks.h>
#include <units/bits/external/type_traits.h>
#include <units/isq/si/area.h>
#include <units/isq/si/length.h>
#include <units/isq/si/speed.h>
#include <units/isq/si/time.h>

using namespace units;
using namespace units::isq;
using namespace units::isq::si;
using namespace units::isq::si::references;

static_assert(2 * m == 2_q_m);
static_assert(2 * s == 2_q_s);
template<auto& s>
concept invalid_operations = requires {
  requires !requires { 2 / s; };
  requires !requires { s / 2; };
  requires !requires { s * 2; };
  requires !requires { s + 2; };
  requires !requires { 2 + s; };
  requires !requires { s + s; };
  requires !requires { s - 2; };
  requires !requires { 2 - s; };
  requires !requires { s - s; };
  requires !requires { s + 1_q_s; };
  requires !requires { s - 1_q_s; };
  requires !requires { s * 1_q_s; };
  requires !requires { s / 1_q_s; };
  requires !requires { 1_q_s + s; };
  requires !requires { 1_q_s - s; };
  requires !requires { 1_q_s * s; };
  requires !requires { 1_q_s / s; };
};
static_assert(invalid_operations<s>);

static_assert(2_q_m / (1 * s) == 2_q_m_per_s);
static_assert(2 * (m / s) == 2_q_m_per_s);

#if !(UNITS_COMP_GCC == 10 && UNITS_COMP_GCC_MINOR == 1)  // GCC 10.1.0 ICEs
constexpr auto m_per_s = m / s;
static_assert(2 * ::m_per_s == 2_q_m_per_s);
#endif

static_assert(120 * km / (2 * h) == 60_q_km_per_h);
static_assert([] {
  const auto length{120};
  const auto duration{2};
  return length * km / (duration * h);
}() == 60_q_km_per_h);
static_assert(compare<decltype(std::int64_t{120} * km / (2 * h)), decltype(60_q_km_per_h)>);
static_assert(compare<decltype(120.L * km / (2 * h)), decltype(60._q_km_per_h)>);

static_assert(1. / 4 * m2 == 1._q_m2 / 4);

UNITS_DIAGNOSTIC_PUSH
UNITS_DIAGNOSTIC_IGNORE_SHADOW
constexpr bool test_hiding() {
  Speed auto v0 = 10 * (m / s);
  signed s = 2;  // hides   ^
  Length auto v = 20 * m / s;
  /*      */ v0 = 10 * (m / ::s);
  return !is_same_v<decltype(v0), decltype(v)>;
}

static_assert(test_hiding());
UNITS_DIAGNOSTIC_POP

int main() {}
