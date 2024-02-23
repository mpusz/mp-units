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

#include <mp-units/bits/module_macros.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/unit.h>
#endif

namespace mp_units::iec80000 {

// clang-format off
template<PrefixableUnit U> struct kibi_ : prefixed_unit<"Ki", mag_power<2, 10>, U{}> {};
template<PrefixableUnit U> struct mebi_ : prefixed_unit<"Mi", mag_power<2, 20>, U{}> {};
template<PrefixableUnit U> struct gibi_ : prefixed_unit<"Gi", mag_power<2, 30>, U{}> {};
template<PrefixableUnit U> struct tebi_ : prefixed_unit<"Ti", mag_power<2, 40>, U{}> {};
template<PrefixableUnit U> struct pebi_ : prefixed_unit<"Pi", mag_power<2, 50>, U{}> {};
template<PrefixableUnit U> struct exbi_ : prefixed_unit<"Ei", mag_power<2, 60>, U{}> {};
template<PrefixableUnit U> struct zebi_ : prefixed_unit<"Zi", mag_power<2, 70>, U{}> {};
template<PrefixableUnit U> struct yobi_ : prefixed_unit<"Yi", mag_power<2, 80>, U{}> {};

MP_UNITS_EXPORT_BEGIN

template<PrefixableUnit auto U> inline constexpr kibi_<std::remove_const_t<decltype(U)>> kibi;
template<PrefixableUnit auto U> inline constexpr mebi_<std::remove_const_t<decltype(U)>> mebi;
template<PrefixableUnit auto U> inline constexpr gibi_<std::remove_const_t<decltype(U)>> gibi;
template<PrefixableUnit auto U> inline constexpr tebi_<std::remove_const_t<decltype(U)>> tebi;
template<PrefixableUnit auto U> inline constexpr pebi_<std::remove_const_t<decltype(U)>> pebi;
template<PrefixableUnit auto U> inline constexpr exbi_<std::remove_const_t<decltype(U)>> exbi;
template<PrefixableUnit auto U> inline constexpr zebi_<std::remove_const_t<decltype(U)>> zebi;
template<PrefixableUnit auto U> inline constexpr yobi_<std::remove_const_t<decltype(U)>> yobi;
// clang-format on

MP_UNITS_EXPORT_END

}  // namespace mp_units::iec80000
