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

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <mp-units/bits/fixed_point.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cstdint>
#include <limits>
#include <tuple>
#include <vector>
#endif

using namespace mp_units;
using namespace mp_units::detail;

template<std::size_t N, typename... T, std::size_t... I>
  requires(N == sizeof...(T) && N == sizeof...(I))
std::tuple<T...> at(const std::array<std::size_t, N>& idx, std::integer_sequence<std::size_t, I...>,
                    const std::vector<T>&... src)
{
  return {src[idx[I]]...};
}

template<typename... T>
std::vector<std::tuple<T...>> cartesian_product(const std::vector<T>&... src)
{
  std::vector<std::tuple<T...>> ret;
  constexpr std::size_t N = sizeof...(src);
  std::array<std::size_t, N> sizes;
  {
    std::size_t n = 1;
    std::size_t k = 0;
    for (std::size_t s : {src.size()...}) {
      sizes[k++] = s;
      n *= s;
    }
    ret.reserve(n);
  }
  std::array<std::size_t, N> idx = {};
  bool done = false;
  while (!done) {
    ret.push_back(at(idx, std::make_index_sequence<N>{}, src...));
    for (std::size_t k = 0; k < idx.size(); ++k) {
      if (++idx[k] < sizes[k]) break;
      if (k + 1 >= idx.size()) {
        done = true;
        break;
      }
      idx[k] = 0;
    }
  }
  return ret;
}


template<std::integral T>
using half_width_int_t = std::conditional_t<std::is_signed_v<T>, min_width_int_t<integer_rep_width_v<T> / 2>,
                                            min_width_uint_t<integer_rep_width_v<T> / 2>>;
template<std::integral T>
using double_width_int_t = std::conditional_t<std::is_signed_v<T>, min_width_int_t<integer_rep_width_v<T> * 2>,
                                              min_width_uint_t<integer_rep_width_v<T> * 2>>;

template<std::integral Hi, std::unsigned_integral Lo>
  requires(integer_rep_width_v<Hi> == integer_rep_width_v<Lo>)
auto combine_bits(Hi hi, Lo lo)
{
  using ret_t = double_width_int_t<Hi>;
  return (static_cast<ret_t>(hi) << integer_rep_width_v<Lo>)+static_cast<ret_t>(lo);
}

template<std::integral T, typename V>
void check(double_width_int<T> value, V&& visitor)
{
  using DT = double_width_int_t<T>;
  auto as_standard_int = static_cast<DT>(value);
  auto expected = visitor(as_standard_int);
  auto actual = visitor(value);
  auto actual_as_standard = static_cast<DT>(actual);
  REQUIRE(actual_as_standard == expected);
}

template<std::integral T>
std::vector<T> test_values()
{
  std::vector<T> ret;
  for (int msb : {0, 1, 2, 3}) {
    auto ref = static_cast<T>(msb) << (integer_rep_width_v<T> - 2);
    for (int lsb_corr : {-2, -1, 0, 1, 2}) {
      auto corr = static_cast<T>(lsb_corr);
      T value = ref + corr;
      ret.push_back(value);
    }
  }
  return ret;
}

using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;
using i64 = std::int64_t;
using du32 = double_width_int<u32>;
using di32 = double_width_int<i32>;

MP_UNITS_DIAGNOSTIC_PUSH
// double_width_int implements the same sign-conversion rules as the standard int types, and we want to verify that;
// even if those sign-conversion rules are frowned upon.
MP_UNITS_DIAGNOSTIC_IGNORE_SIGN_CONVERSION

TEST_CASE("double_width_int addition and subtraction", "[double_width_int]")
{
  SECTION("u32x2 +/- u32")
  {
    for (auto [lhi, llo, rhs] : cartesian_product(test_values<u32>(), test_values<u32>(), test_values<u32>())) {
      CAPTURE(lhi, llo, rhs);
      auto lhs = double_width_int<u32>::from_hi_lo(lhi, llo);
      check(lhs, [&](auto v) { return v + rhs; });
      check(lhs, [&](auto v) { return v - rhs; });
      check(lhs, [&](auto v) { return rhs - v; });
    }
  }
  SECTION("u32x2 +/- i32")
  {
    for (auto [lhi, llo, rhs] : cartesian_product(test_values<u32>(), test_values<u32>(), test_values<i32>())) {
      CAPTURE(lhi, llo, rhs);
      auto lhs = double_width_int<u32>::from_hi_lo(lhi, llo);
      check(lhs, [&](auto v) { return v + rhs; });
      check(lhs, [&](auto v) { return v - rhs; });
      check(lhs, [&](auto v) { return rhs - v; });
    }
  }
  SECTION("i32x2 +/- u32")
  {
    for (auto [lhi, llo, rhs] : cartesian_product(test_values<i32>(), test_values<u32>(), test_values<u32>())) {
      CAPTURE(lhi, llo, rhs);
      auto lhs = double_width_int<i32>::from_hi_lo(lhi, llo);
      check(lhs, [&](auto v) { return v + rhs; });
      check(lhs, [&](auto v) { return v - rhs; });
      check(lhs, [&](auto v) { return rhs - v; });
    }
  }
  SECTION("i32x2 +/- i32")
  {
    for (auto [lhi, llo, rhs] : cartesian_product(test_values<i32>(), test_values<u32>(), test_values<i32>())) {
      CAPTURE(lhi, llo, rhs);
      auto lhs = double_width_int<i32>::from_hi_lo(lhi, llo);
      check(lhs, [&](auto v) { return v + rhs; });
      check(lhs, [&](auto v) { return v - rhs; });
      check(lhs, [&](auto v) { return rhs - v; });
    }
  }
}

TEST_CASE("double_width_int multiplication", "[double_width_int]")
{
  SECTION("u32 * u32")
  {
    for (auto [lhs, rhs] : cartesian_product(test_values<u32>(), test_values<u32>())) {
      CAPTURE(lhs, rhs);
      u64 expected = u64{lhs} * u64{rhs};
      auto actual = double_width_int<u32>::wide_product_of(lhs, rhs);
      auto actual_as_std = static_cast<u64>(actual);
      REQUIRE(actual_as_std == expected);
    }
  }
  SECTION("i32 * u32")
  {
    for (auto [lhs, rhs] : cartesian_product(test_values<i32>(), test_values<u32>())) {
      CAPTURE(lhs, rhs);
      i64 expected = i64{lhs} * i64{rhs};
      auto actual = double_width_int<i32>::wide_product_of(lhs, rhs);
      auto actual_as_std = static_cast<i64>(actual);
      REQUIRE(actual_as_std == expected);
    }
  }
  SECTION("u32x2 * u32")
  {
    for (auto [lhi, llo, rhs] : cartesian_product(test_values<u32>(), test_values<u32>(), test_values<u32>())) {
      CAPTURE(lhi, llo, rhs);
      auto lhs = double_width_int<u32>::from_hi_lo(lhi, llo);
      check(lhs, [&](auto v) { return v * rhs; });
    }
  }
  SECTION("u32x2 * i32")
  {
    for (auto [lhi, llo, rhs] : cartesian_product(test_values<u32>(), test_values<u32>(), test_values<i32>())) {
      CAPTURE(lhi, llo, rhs);
      auto lhs = double_width_int<u32>::from_hi_lo(lhi, llo);
      check(lhs, [&](auto v) { return v * rhs; });
    }
  }
  SECTION("i32x2 * u32")
  {
    for (auto [lhi, llo, rhs] : cartesian_product(test_values<i32>(), test_values<u32>(), test_values<u32>())) {
      CAPTURE(lhi, llo, rhs);
      auto lhs = double_width_int<i32>::from_hi_lo(lhi, llo);
      check(lhs, [&](auto v) { return v * rhs; });
    }
  }
  SECTION("i32x2 * i32")
  {
    for (auto [lhi, llo, rhs] : cartesian_product(test_values<i32>(), test_values<u32>(), test_values<i32>())) {
      CAPTURE(lhi, llo, rhs);
      auto lhs = double_width_int<i32>::from_hi_lo(lhi, llo);
      check(lhs, [&](auto v) { return v * rhs; });
    }
  }
}

MP_UNITS_DIAGNOSTIC_POP
