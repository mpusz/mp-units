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
#include <mp-units/compat_macros.h>
#include <sstream>
#include <string_view>
#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/ext/fixed_string.h>
#endif

using namespace mp_units;

TEST_CASE("fixed_string::at", "[fixed_string]")
{
  basic_fixed_string txt = "abc";
  SECTION("in range")
  {
    CHECK(txt.at(0) == 'a');
    CHECK(txt.at(1) == 'b');
    CHECK(txt.at(2) == 'c');
  }
  SECTION("out of range")
  {
    REQUIRE_THROWS_MATCHES(txt.at(3), std::out_of_range, Catch::Matchers::Message("basic_fixed_string::at"));
    REQUIRE_THROWS_MATCHES(txt.at(1024), std::out_of_range, Catch::Matchers::Message("basic_fixed_string::at"));
  }
}

TEST_CASE("fixed_string text output", "[fixed_string][ostream][fmt]")
{
  basic_fixed_string txt = "units";
  SECTION("iostream")
  {
    std::ostringstream os;
    os << txt;
    CHECK(os.str() == "units");
  }
  SECTION("fmt") { CHECK(MP_UNITS_STD_FMT::format("{}", txt) == "units"); }
}

TEST_CASE("fixed_string hash", "[fixed_string][hash]")
{
  basic_fixed_string txt = "units";
  CHECK(std::hash<fixed_string<5>>{}(txt) == std::hash<std::string_view>{}("units"));
}
