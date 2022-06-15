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

struct kibi : prefix<kibi, "Ki", ratio(1'024)> {};
struct mebi : prefix<mebi, "Mi", ratio(1'048'576)> {};
struct gibi : prefix<gibi, "Gi", ratio(1'073'741'824)> {};
struct tebi : prefix<tebi, "Ti", ratio(1'099'511'627'776)> {};
struct pebi : prefix<pebi, "Pi", ratio(1'125'899'906'842'624)> {};
struct exbi : prefix<exbi, "Ei", ratio(1'152'921'504'606'846'976)> {};

}  // namespace units::isq::iec80000
