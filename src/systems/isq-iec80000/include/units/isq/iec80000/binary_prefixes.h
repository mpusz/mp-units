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

#pragma once

#include <units/prefix.h>

namespace units::isq::iec80000 {

struct kibi : prefix<kibi, "Ki", pow<10>(mag<2>())> {};
struct mebi : prefix<mebi, "Mi", pow<20>(mag<2>())> {};
struct gibi : prefix<gibi, "Gi", pow<30>(mag<2>())> {};
struct tebi : prefix<tebi, "Ti", pow<40>(mag<2>())> {};
struct pebi : prefix<pebi, "Pi", pow<50>(mag<2>())> {};
struct exbi : prefix<exbi, "Ei", pow<60>(mag<2>())> {};
struct zebi : prefix<zebi, "Zi", pow<70>(mag<2>())> {};
struct yobi : prefix<yobi, "Yi", pow<80>(mag<2>())> {};

}  // namespace units::isq::iec80000
