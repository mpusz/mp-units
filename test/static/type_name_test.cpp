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

#include <mp-units/ext/type_name.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <string_view>
#endif

namespace type_name_test {

using namespace mp_units::detail;

struct name;
struct name2;
template<typename T>
struct box;

// fundamental types are spelled the same by every compiler
static_assert(type_name<int>() == "int");
static_assert(type_name<double>() == "double");
static_assert(type_name<bool>() == "bool");

// MSVC prefixes a class type with `struct ` or `class `, so only the end of the name is checked
static_assert(type_name<name>().ends_with("type_name_test::name"));
static_assert(type_name<box<int>>().ends_with("type_name_test::box<int>"));

// nothing from the signature of the function that extracts the name leaks into it
static_assert(type_name<name>().find(';') == std::string_view::npos);
static_assert(type_name<name>().find("string_view") == std::string_view::npos);
static_assert(type_name<box<int>>().find(']') == std::string_view::npos);

// the overload taking a value names the type of that value
inline constexpr struct value_type {
} value{};
static_assert(type_name(value) == type_name<value_type>());

// a name sorts before a longer name it is a prefix of, whatever character follows the prefix
static_assert(type_name_less<name, name2>::value);
static_assert(!type_name_less<name2, name>::value);
static_assert(!type_name_less<name, name>::value);

}  // namespace type_name_test
