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

#include <mp-units/bits/external/fixed_string.h>

using namespace mp_units;

namespace {

constexpr basic_fixed_string txt1('a');
static_assert(txt1.size() == 1);
static_assert(txt1[0] == 'a');
static_assert(txt1 == basic_fixed_string("a"));
static_assert(txt1 < basic_fixed_string("b"));
static_assert(txt1 < basic_fixed_string("aa"));
static_assert(txt1 + basic_fixed_string('b') == basic_fixed_string("ab"));
static_assert(basic_fixed_string('b') + txt1 == basic_fixed_string("ba"));
static_assert(txt1 + basic_fixed_string("bc") == basic_fixed_string("abc"));
static_assert(basic_fixed_string("bc") + txt1 == basic_fixed_string("bca"));

constexpr basic_fixed_string txt2("abc");
static_assert(txt2.size() == 3);
static_assert(txt2[0] == 'a');
static_assert(txt2[1] == 'b');
static_assert(txt2[2] == 'c');
static_assert(txt2 == basic_fixed_string("abc"));
static_assert(txt2 < basic_fixed_string("b"));
static_assert(txt2 > basic_fixed_string("aa"));
static_assert(txt2 + basic_fixed_string('d') == basic_fixed_string("abcd"));
static_assert(basic_fixed_string('d') + txt2 == basic_fixed_string("dabc"));
static_assert(txt2 + basic_fixed_string("def") == basic_fixed_string("abcdef"));
static_assert(basic_fixed_string("def") + txt2 == basic_fixed_string("defabc"));

}  // namespace
