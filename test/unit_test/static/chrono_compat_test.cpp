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

#include <units/physical/si/base/time.h>
#include <utility>

namespace {

template <typename A, typename B>
consteval bool same(A && a, B && b)
{
    return units::equivalent<A, B> and a.count() == b.count();
}

using units::physical::si::time;
using namespace units::physical::si;
using namespace units::physical::si::literals;

constexpr static auto t1 = 123456789_q_us;
static_assert(to_chrono_duration(t1) == std::chrono::microseconds{123456789});
static_assert(to_chrono_duration<std::chrono::seconds>(t1)      == std::chrono::seconds{123});
static_assert(to_chrono_duration<std::chrono::milliseconds>(t1) == std::chrono::milliseconds{123456});
static_assert(to_chrono_duration<std::chrono::nanoseconds>(t1)  == std::chrono::nanoseconds{123456789000});

constexpr static auto t2 = 123456789._q_us;
static_assert(to_chrono_duration(t1) == std::chrono::duration<long double, std::micro>{123456789});
static_assert(to_chrono_duration<std::chrono::seconds>(t1)      == std::chrono::seconds{123});
static_assert(to_chrono_duration<std::chrono::milliseconds>(t1) == std::chrono::milliseconds{123456});
static_assert(to_chrono_duration<std::chrono::nanoseconds>(t1)  == std::chrono::nanoseconds{123456789000});

constexpr static auto t3 = std::chrono::milliseconds{123456789};
static_assert(same(from_chrono_duration(t3), time<millisecond, decltype(t3)::rep>{123456789}));
static_assert(same(from_chrono_duration<time<second, int>>(t3), time<second, int>{123456}));
static_assert(same(from_chrono_duration<time<second>>(t3), time<second>{123456.789}));

}


