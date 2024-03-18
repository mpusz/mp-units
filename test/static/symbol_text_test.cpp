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

#include <mp-units/bits/symbol_text.h>

using namespace mp_units;

namespace {

constexpr symbol_text sym1('b');
static_assert(sym1 == 'b');
static_assert(sym1 != 'a');
static_assert(sym1 != symbol_text("ab"));
static_assert(sym1 < 'c');
static_assert(sym1 > 'a');
static_assert(sym1 <= 'b');
static_assert(sym1 <= 'c');
static_assert(sym1 >= 'b');
static_assert(sym1 >= 'a');
static_assert(sym1.unicode() == u8"b");
static_assert(sym1.ascii() == "b");

constexpr symbol_text sym3("ab");
static_assert(sym3.unicode() == u8"ab");
static_assert(sym3.ascii() == "ab");

constexpr basic_fixed_string txt1("bc");
constexpr symbol_text sym4(txt1);
static_assert(sym4.unicode() == u8"bc");
static_assert(sym4.ascii() == "bc");

constexpr symbol_text sym5(u8"bc", "de");
static_assert(sym5.unicode() == u8"bc");
static_assert(sym5.ascii() == "de");

constexpr basic_fixed_string txt2("de");
constexpr symbol_text sym6(sym4.unicode(), txt2);
static_assert(sym6.unicode() == u8"bc");
static_assert(sym6.ascii() == "de");

static_assert(sym6 == symbol_text(u8"bc", "de"));
static_assert(sym6 != symbol_text(u8"fg", "hi"));
static_assert(sym6 != symbol_text(u8"bcd", "ef"));

static_assert(sym6 < symbol_text("c"));
static_assert(sym6 > symbol_text("a"));
static_assert(sym6 <= symbol_text("c"));
static_assert(sym6 <= symbol_text("bcd"));
static_assert(sym6 >= symbol_text("a"));
static_assert(sym6 >= symbol_text("bc"));

static_assert(symbol_text("a") + sym4 == symbol_text("abc"));
static_assert(sym4 + symbol_text("f") == symbol_text("bcf"));

static_assert(symbol_text(u8"a", "f") + sym6 == symbol_text(u8"abc", "fde"));
static_assert(sym6 + symbol_text(u8"a", "f") == symbol_text(u8"bca", "def"));

static_assert(symbol_text('a') + sym6 == symbol_text(u8"abc", "ade"));
static_assert(sym6 + symbol_text('f') == symbol_text(u8"bcf", "def"));

static_assert(symbol_text("a") + sym6 == symbol_text(u8"abc", "ade"));
static_assert(sym6 + symbol_text("f") == symbol_text(u8"bcf", "def"));

}  // namespace
