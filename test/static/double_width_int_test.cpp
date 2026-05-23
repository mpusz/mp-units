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

#include <mp-units/bits/double_width_int.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cstdint>
#include <limits>
#include <type_traits>
#endif

using namespace mp_units;

namespace {

using i128 = detail::double_width_int<std::int64_t>;
using u128 = detail::double_width_int<std::uint64_t>;
using i32x2 = detail::double_width_int<std::int32_t>;
using u32x2 = detail::double_width_int<std::uint32_t>;

// Shift/divide round-trip on i128 (exercises the narrow-rhs paths)
static_assert((((83 * 79 * 73) * (i128{97} << 64u) / 89) >> 64u) == (83 * 79 * 73 * 97) / 89);

// ---- Arithmetic between two double-width operands ----
// These compile-time checks pin down the operators that the synthetic int128 emulation must
// provide on platforms without native __int128 (issue #795).

// + with carry from the low half to the high half
static_assert(u128{1, 2} + u128{3, 4} == u128{4, 6});
static_assert(u128{0, ~std::uint64_t{0}} + u128{0, 1} == u128{1, 0});
static_assert(i128{0, ~std::uint64_t{0}} + i128{0, 1} == i128{1, 0});

// - with borrow from the high half to the low half
static_assert(u128{4, 6} - u128{3, 4} == u128{1, 2});
static_assert(u128{1, 0} - u128{0, 1} == u128{0, ~std::uint64_t{0}});
static_assert(-i128{0, 1} == i128{-1, ~std::uint64_t{0}});

// * truncated to width bits; the high*high contribution overflows and is discarded
static_assert(u128{0, 5} * u128{0, 3} == u128{0, 15});
static_assert(u128{0, ~std::uint64_t{0}} * u128{0, 2} == u128{1, ~std::uint64_t{0} - 1});  // 2 * (2^64 - 1)
static_assert(u128{0, 1u << 16u} * u128{0, 1u << 16u} == u128{0, std::uint64_t{1} << 32u});
static_assert(i128{0, 5} * i128{-1, ~std::uint64_t{0} - 2} == i128{-1, ~std::uint64_t{0} - 14});  // 5 * -3 = -15

// / between two dwints — narrow fast path and wide long-division path
static_assert(u128{0, 100} / u128{0, 7} == u128{0, 14});
static_assert(u128{1, 0} / u128{0, 2} == u128{0, std::uint64_t{1} << 63u});                         // 2^64 / 2 == 2^63
static_assert(i128{0, 100} / i128{-1, ~std::uint64_t{0} - 6} == i128{-1, ~std::uint64_t{0} - 13});  // 100 / -7 == -14
static_assert(u128{1, 5} / u128{2, 0} == u128{0, 0});  // divisor wider than dividend → 0

// Bitwise operators
static_assert(~u128{0, 0} == u128{~std::uint64_t{0}, ~std::uint64_t{0}});
static_assert(~u128{~std::uint64_t{0}, ~std::uint64_t{0}} == u128{0, 0});
static_assert((u128{0xFF, 0xFF} & u128{0x0F, 0xF0}) == u128{0x0F, 0xF0});
static_assert((u128{0xF0, 0x00} | u128{0x0F, 0xFF}) == u128{0xFF, 0xFF});
static_assert((u128{0xFF, 0x00} ^ u128{0x0F, 0xFF}) == u128{0xF0, 0xFF});

// Compound assignment operators (each verified by reading back the value after one operation)
constexpr auto check_compound = [] {
  u128 v{1, 2};
  v += u128{2, 3};
  if (!(v == u128{3, 5})) return false;
  v -= u128{1, 1};
  if (!(v == u128{2, 4})) return false;
  v *= u128{0, 3};
  if (!(v == u128{6, 12})) return false;
  v /= u128{0, 2};
  if (!(v == u128{3, 6})) return false;
  v &= u128{~std::uint64_t{0}, 0xF};
  if (!(v == u128{3, 6})) return false;
  v |= u128{0, 0xF0};
  if (!(v == u128{3, 0xF6})) return false;
  v ^= u128{0, 0xFF};
  if (!(v == u128{3, 9})) return false;
  v <<= 4u;
  if (!(v == u128{0x30, 0x90})) return false;
  v >>= 4u;
  if (!(v == u128{3, 9})) return false;
  return true;
};
static_assert(check_compound());

// Pre/post increment and decrement
constexpr auto check_inc_dec = [] {
  u128 v{0, ~std::uint64_t{0}};
  ++v;
  if (!(v == u128{1, 0})) return false;  // carry across halves
  --v;
  if (!(v == u128{0, ~std::uint64_t{0}})) return false;
  auto post_inc = v++;
  if (!(post_inc == u128{0, ~std::uint64_t{0}} && v == u128{1, 0})) return false;
  auto post_dec = v--;
  if (!(post_dec == u128{1, 0} && v == u128{0, ~std::uint64_t{0}})) return false;
  return true;
};
static_assert(check_inc_dec());

// numeric_limits specialization (required by checked_int_pow etc.)
static_assert(std::numeric_limits<u128>::is_specialized);
static_assert(std::numeric_limits<u128>::is_integer);
static_assert(std::numeric_limits<u128>::is_exact);
static_assert(!std::numeric_limits<u128>::is_signed);
static_assert(std::numeric_limits<i128>::is_signed);
static_assert(std::numeric_limits<u128>::digits == 128);
static_assert(std::numeric_limits<i128>::digits == 127);
static_assert(std::numeric_limits<u32x2>::digits == 64);
static_assert(std::numeric_limits<i32x2>::digits == 63);
static_assert(std::numeric_limits<u128>::radix == 2);
static_assert(std::numeric_limits<u128>::min() == u128{0, 0});
static_assert(std::numeric_limits<i128>::min() == i128{std::numeric_limits<std::int64_t>::min(), 0});
static_assert(std::numeric_limits<u128>::max() == u128{~std::uint64_t{0}, ~std::uint64_t{0}});
static_assert(std::numeric_limits<i128>::max() == i128{std::numeric_limits<std::int64_t>::max(), ~std::uint64_t{0}});

// Conversion to long double (round-trips small integers exactly; the signed branch goes
// through unary minus to avoid catastrophic cancellation on platforms where long double
// has fewer than `width` bits of mantissa, e.g. MSVC where long double == double).
static_assert(static_cast<long double>(u128{0, 12345}) == 12345.0L);
static_assert(static_cast<long double>(i128{0, 1}) == 1.0L);
static_assert(static_cast<long double>(-i128{0, 1}) == -1.0L);

}  // namespace
