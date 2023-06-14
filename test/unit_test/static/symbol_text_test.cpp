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

constexpr basic_symbol_text sym1('b');
static_assert(sym1 == 'b');
static_assert(sym1 != 'a');
static_assert(sym1 != basic_symbol_text("ab"));
static_assert(sym1 < 'c');
static_assert(sym1 > 'a');
static_assert(sym1 <= 'b');
static_assert(sym1 <= 'c');
static_assert(sym1 >= 'b');
static_assert(sym1 >= 'a');
static_assert(sym1.unicode() == "b");
static_assert(sym1.ascii() == "b");

constexpr basic_symbol_text sym2('a', 'b');
static_assert(sym2 == basic_symbol_text('a', 'b'));
static_assert(sym2 != 'b');
static_assert(sym2.unicode() == 'a');
static_assert(sym2.ascii() == 'b');

constexpr basic_symbol_text sym3("ab");
static_assert(sym3.unicode() == "ab");
static_assert(sym3.ascii() == "ab");

constexpr basic_fixed_string txt1("bc");
constexpr basic_symbol_text sym4(txt1);
static_assert(sym4.unicode() == "bc");
static_assert(sym4.ascii() == "bc");

constexpr basic_symbol_text sym5("bc", "de");
static_assert(sym5.unicode() == "bc");
static_assert(sym5.ascii() == "de");

constexpr basic_fixed_string txt2("de");
constexpr basic_symbol_text sym6(sym4.unicode(), txt2);
static_assert(sym6.unicode() == "bc");
static_assert(sym6.ascii() == "de");

static_assert(sym6 == basic_symbol_text("bc", "de"));
static_assert(sym6 != basic_symbol_text("fg", "hi"));
static_assert(sym6 != basic_symbol_text("bcd", "ef"));

static_assert(sym6 < basic_symbol_text("c"));
static_assert(sym6 > basic_symbol_text("a"));
static_assert(sym6 <= basic_symbol_text("c"));
static_assert(sym6 <= basic_symbol_text("bcd"));
static_assert(sym6 >= basic_symbol_text("a"));
static_assert(sym6 >= basic_symbol_text("bc"));

static_assert(basic_symbol_text("a") + sym4 == basic_symbol_text("abc"));
static_assert(sym4 + basic_symbol_text("f") == basic_symbol_text("bcf"));

static_assert(basic_fixed_string("a") + sym4 == basic_symbol_text("abc"));
static_assert(sym4 + basic_fixed_string("f") == basic_symbol_text("bcf"));

static_assert(basic_symbol_text("a", "f") + sym6 == basic_symbol_text("abc", "fde"));
static_assert(sym6 + basic_symbol_text("a", "f") == basic_symbol_text("bca", "def"));

static_assert('a' + sym6 == basic_symbol_text("abc", "ade"));
static_assert(sym6 + 'f' == basic_symbol_text("bcf", "def"));

static_assert(basic_fixed_string("a") + sym6 == basic_symbol_text("abc", "ade"));
static_assert(sym6 + basic_fixed_string("f") == basic_symbol_text("bcf", "def"));

static_assert("a" + sym6 == basic_symbol_text("abc", "ade"));
static_assert(sym6 + "f" == basic_symbol_text("bcf", "def"));

}  // namespace
