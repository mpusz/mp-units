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

#include <mp-units/bits/external/prime.h>
#include <type_traits>
#include <utility>

using namespace mp_units::detail;

namespace {

template<std::size_t BasisSize, std::size_t... Is>
constexpr bool check_primes(std::index_sequence<Is...>)
{
  return ((Is < 2 || wheel_factorizer<BasisSize>::is_prime(Is) == is_prime_by_trial_division(Is)) && ...);
}

static_assert(check_primes<2>(std::make_index_sequence<122>{}));

// This is the smallest number that can catch the bug where we use only _prime_ numbers in the first wheel, rather than
// numbers which are _coprime to the basis_.
//
// The basis for N = 4 is {2, 3, 5, 7}, so the wheel size is 210.  11 * 11 = 121 is within the first wheel.  It is
// coprime with every element of the basis, but it is _not_ prime.  If we keep only prime numbers, then we will neglect
// using numbers of the form (210 * n + 121) as trial divisors, which is a problem if any are prime.  For n = 1, we have
// a divisor of (210 + 121 = 331), which happens to be prime but will not be used.  Thus, (331 * 331 = 109561) is a
// composite number which could wrongly appear prime if we skip over 331.
static_assert(wheel_factorizer<4>::is_prime(109'561) == is_prime_by_trial_division(109'561));

static_assert(wheel_factorizer<1>::coprimes_in_first_wheel.size() == 1);
static_assert(wheel_factorizer<2>::coprimes_in_first_wheel.size() == 2);
static_assert(wheel_factorizer<3>::coprimes_in_first_wheel.size() == 8);
static_assert(wheel_factorizer<4>::coprimes_in_first_wheel.size() == 48);
static_assert(wheel_factorizer<5>::coprimes_in_first_wheel.size() == 480);

static_assert(wheel_factorizer<3>::coprimes_in_first_wheel[0] == 1);
static_assert(wheel_factorizer<3>::coprimes_in_first_wheel[1] == 7);
static_assert(wheel_factorizer<3>::coprimes_in_first_wheel[2] == 11);
static_assert(wheel_factorizer<3>::coprimes_in_first_wheel[3] == 13);
static_assert(wheel_factorizer<3>::coprimes_in_first_wheel[4] == 17);
static_assert(wheel_factorizer<3>::coprimes_in_first_wheel[5] == 19);
static_assert(wheel_factorizer<3>::coprimes_in_first_wheel[6] == 23);
static_assert(wheel_factorizer<3>::coprimes_in_first_wheel[7] == 29);

static_assert(!wheel_factorizer<1>::is_prime(0));
static_assert(!wheel_factorizer<1>::is_prime(1));
static_assert(wheel_factorizer<1>::is_prime(2));

static_assert(!wheel_factorizer<2>::is_prime(0));
static_assert(!wheel_factorizer<2>::is_prime(1));
static_assert(wheel_factorizer<2>::is_prime(2));

static_assert(!wheel_factorizer<3>::is_prime(0));
static_assert(!wheel_factorizer<3>::is_prime(1));
static_assert(wheel_factorizer<3>::is_prime(2));

}  // namespace
