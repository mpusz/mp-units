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

#include <mp-units/ext/fixed_string.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <array>
#include <string_view>
#endif

using namespace mp_units;

namespace {

constexpr std::array array = {'a', 'b', 'c'};

auto from_string = [] {
  std::string_view txt = "abc";
  return fixed_string<3>(std::from_range, txt);
};

auto from_string_iter = [] {
  std::string_view txt = "abc";
  return fixed_string<3>(txt.begin(), txt.end());
};

constexpr fixed_string<0> txt0;
constexpr basic_fixed_string txt1('a');
constexpr basic_fixed_string txt2('a', 'b', 'c');
constexpr basic_fixed_string txt3 = "abc";
constexpr fixed_string<3> txt4(array.begin(), array.end());
constexpr basic_fixed_string txt5(std::from_range, array);
constexpr basic_fixed_string txt6(from_string());
constexpr basic_fixed_string txt7(from_string_iter());

constexpr fixed_string<3> txt8(txt2.begin(), txt2.end());
constexpr fixed_string<3> txt9(txt2.rbegin(), txt2.rend());

static_assert(txt0.size() == 0);
static_assert(txt1.size() == 1);
static_assert(txt2.size() == 3);
static_assert(txt3.size() == 3);
static_assert(txt4.size() == 3);
static_assert(txt5.size() == 3);
static_assert(txt6.size() == 3);
static_assert(txt7.size() == 3);
static_assert(txt8.size() == 3);
static_assert(txt9.size() == 3);

static_assert(txt0.length() == 0);
static_assert(txt1.length() == 1);
static_assert(txt2.length() == 3);

static_assert(txt0.max_size() == 0);
static_assert(txt1.max_size() == 1);
static_assert(txt2.max_size() == 3);

static_assert(txt0.empty() == true);
static_assert(txt1.empty() == false);
static_assert(txt2.empty() == false);
static_assert(txt3.empty() == false);
static_assert(txt4.empty() == false);
static_assert(txt5.empty() == false);
static_assert(txt6.empty() == false);
static_assert(txt7.empty() == false);
static_assert(txt8.empty() == false);
static_assert(txt9.empty() == false);

static_assert(txt1[0] == 'a');
static_assert(txt2[0] == 'a');
static_assert(txt2[1] == 'b');
static_assert(txt2[2] == 'c');
static_assert(txt9[0] == 'c');
static_assert(txt9[1] == 'b');
static_assert(txt9[2] == 'a');

#if MP_UNITS_HOSTED
static_assert(txt1.at(0) == 'a');
static_assert(txt2.at(0) == 'a');
static_assert(txt2.at(1) == 'b');
static_assert(txt2.at(2) == 'c');
static_assert(txt9.at(0) == 'c');
static_assert(txt9.at(1) == 'b');
static_assert(txt9.at(2) == 'a');
#endif

static_assert(txt1.front() == 'a');
static_assert(txt1.back() == 'a');
static_assert(txt2.front() == 'a');
static_assert(txt2.back() == 'c');
static_assert(txt5.front() == 'a');
static_assert(txt5.back() == 'c');
static_assert(txt6.front() == 'a');
static_assert(txt6.back() == 'c');
static_assert(txt7.front() == 'a');
static_assert(txt7.back() == 'c');
static_assert(txt8.front() == 'a');
static_assert(txt8.back() == 'c');
static_assert(txt9.front() == 'c');
static_assert(txt9.back() == 'a');

static_assert(std::string_view(txt0.data()) == "");
static_assert(std::string_view(txt0.c_str()) == "");
static_assert(std::string_view(txt1.data()) == "a");
static_assert(std::string_view(txt1.c_str()) == "a");
static_assert(std::string_view(txt2.data()) == "abc");
static_assert(std::string_view(txt2.c_str()) == "abc");

static_assert(txt0 == "");
static_assert("a" == txt1);
static_assert(txt2 == "abc");
static_assert(txt3 == "abc");
static_assert(txt4 == "abc");
static_assert(txt5 == "abc");
static_assert(txt6 == "abc");
static_assert(txt7 == "abc");
static_assert(txt8 == "abc");
static_assert(txt9 == "cba");

static_assert(txt1 == basic_fixed_string("a"));
static_assert(txt1 != basic_fixed_string("b"));
static_assert(txt1 != basic_fixed_string("aa"));
static_assert(txt1 < basic_fixed_string("b"));
static_assert(txt1 < basic_fixed_string("aa"));
static_assert(txt1 == "a");
static_assert(txt1 != "b");
static_assert(txt1 != "aa");
static_assert(txt1 < "b");
static_assert(txt1 < "aa");

static_assert(txt1 + basic_fixed_string('b') == "ab");
static_assert(basic_fixed_string('b') + txt1 == "ba");
static_assert(txt1 + basic_fixed_string("bc") == "abc");
static_assert(basic_fixed_string("bc") + txt1 == "bca");
static_assert(txt1 + 'b' == "ab");
static_assert('b' + txt1 == "ba");
static_assert(txt1 + "bc" == "abc");
static_assert("bc" + txt1 == "bca");

static_assert(txt2 == basic_fixed_string("abc"));
static_assert(txt2 != basic_fixed_string("cba"));
static_assert(txt2 != basic_fixed_string("abcd"));
static_assert(txt2 < basic_fixed_string("b"));
static_assert(txt2 > basic_fixed_string("aa"));
static_assert(txt2 == "abc");
static_assert(txt2 != "cba");
static_assert(txt2 != "abcd");
static_assert(txt2 < "b");
static_assert(txt2 > "aa");

static_assert(txt2 + basic_fixed_string('d') == "abcd");
static_assert(basic_fixed_string('d') + txt2 == "dabc");
static_assert(txt2 + basic_fixed_string("def") == "abcdef");
static_assert(basic_fixed_string("def") + txt2 == "defabc");
static_assert(txt2 + 'd' == "abcd");
static_assert('d' + txt2 == "dabc");
static_assert(txt2 + "def" == "abcdef");
static_assert("def" + txt2 == "defabc");

static_assert(std::string_view(txt2) == "abc");
static_assert(txt2.view() == "abc");
static_assert(std::string_view(txt2).find('b') == 1);
static_assert(txt2.view().find('b') == 1);

}  // namespace
