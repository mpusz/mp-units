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

#include <units/bits/prime.h>

#include <type_traits>
#include <utility>

namespace units::detail
{

template<std::size_t BasisSize, std::size_t... Is>
constexpr bool check_primes(std::index_sequence<Is...>) {
  return ((Is < 2 || WheelFactorizer<BasisSize>::is_prime(Is) == is_prime_by_trial_division(Is)) && ...);
}

static_assert(check_primes<2>(std::make_index_sequence<122>{}));

constexpr auto some_primes = primes_between<7, 19>(); // 7, 11, 13, 17
static_assert(std::is_same_v<std::remove_cvref_t<decltype(some_primes)>, std::array<std::size_t, 4>>);
static_assert(some_primes[0] == 7);
static_assert(some_primes[1] == 11);
static_assert(some_primes[2] == 13);
static_assert(some_primes[3] == 17);

static_assert(WheelFactorizer<1>::primes_in_first_wheel.size() == 0);
static_assert(WheelFactorizer<2>::primes_in_first_wheel.size() == 1);
static_assert(WheelFactorizer<3>::primes_in_first_wheel.size() == 7);
static_assert(WheelFactorizer<4>::primes_in_first_wheel.size() == 42);

static_assert(!WheelFactorizer<1>::is_prime(0));
static_assert(!WheelFactorizer<1>::is_prime(1));
static_assert(WheelFactorizer<1>::is_prime(2));

static_assert(!WheelFactorizer<2>::is_prime(0));
static_assert(!WheelFactorizer<2>::is_prime(1));
static_assert(WheelFactorizer<2>::is_prime(2));

static_assert(!WheelFactorizer<3>::is_prime(0));
static_assert(!WheelFactorizer<3>::is_prime(1));
static_assert(WheelFactorizer<3>::is_prime(2));

} // namespace units::detail
