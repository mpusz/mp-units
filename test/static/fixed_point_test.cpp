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

#include <mp-units/bits/fixed_point.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cstdint>
#endif

using namespace mp_units;

namespace {

static_assert(std::is_same_v<detail::min_width_uint_t<1>, std::uint8_t>);
static_assert(std::is_same_v<detail::min_width_uint_t<7>, std::uint8_t>);
static_assert(std::is_same_v<detail::min_width_uint_t<8>, std::uint8_t>);
static_assert(std::is_same_v<detail::min_width_uint_t<9>, std::uint16_t>);
static_assert(std::is_same_v<detail::min_width_uint_t<31>, std::uint32_t>);
static_assert(std::is_same_v<detail::min_width_uint_t<32>, std::uint32_t>);
static_assert(std::is_same_v<detail::min_width_uint_t<33>, std::uint64_t>);

using i128 = detail::double_width_int<std::int64_t>;
using u128 = detail::double_width_int<std::uint64_t>;

static_assert((((83 * 79 * 73) * (i128{97} << 64u) / 89) >> 64u) == (83 * 79 * 73 * 97) / 89);

}  // namespace
