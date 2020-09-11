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

#include <units/data/data.h>
#include <catch2/catch.hpp>
#include <sstream>

using namespace units::data;

TEST_CASE("operator<< on a data quantity", "[text][ostream]")
{
  std::stringstream stream;

  SECTION("quantity with a predefined unit and prefix")
  {
    SECTION("named unit")
    {
      stream << 64_q_B;
      REQUIRE(stream.str() == "64 B");
    }

    SECTION("prefixed coherent unit")
    {
      stream << 256_q_Kib;
      REQUIRE(stream.str() == "256 Kib");
    }

    SECTION("prefixed non-coherent unit")
    {
      stream << 1024_q_KiB;
      REQUIRE(stream.str() == "1024 KiB");
    }

    SECTION("other unit matching prefix")
    {
      stream << 8_q_Kib * 8_q_Kib / 2_q_b;
      REQUIRE(stream.str() == "32 Mib");
    }
  }
}
